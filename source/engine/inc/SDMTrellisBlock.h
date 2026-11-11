//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SDMTRELLISBLOCK_H
#define __OMEGA_ENGINE_SDMTRELLISBLOCK_H
//-------------------------------------------------------------------------------------------

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <memory>
#include <vector>

#include <QThreadPool>
#include <QtConcurrent/QtConcurrentMap>

#include "engine/inc/SDMTrellis.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------
// Parallel block version of the Viterbi trellis sigma-delta modulator.
//
// Each call to process splits the given PCM into B contiguous blocks that are modulated
// concurrently using QtConcurrent. The blocks are joined as follows:
//
//   1. Continuity across calls: block 0 is always run by this object (the inherited
//      SDMTrellis), continuing from the trellis state left at the end of the previous call.
//      At the end of each call the trellis of the final block is copied back into this
//      object. Consecutive calls therefore join exactly as with the sequential SDMTrellis.
//
//   2. Warm up: every other block k is run by its own worker trellis that is reset and then
//      started warmUp samples before its overlap region. The bits produced during the warm
//      up are discarded. This lets the worker settle on to a valid noise trajectory for the
//      signal before any of its output is used.
//
//      The worker is not started from an all zero state. The first loop filter integrator
//      steps by (x - y) with y = +/-1, so the bit choices can only ever change it by even
//      integers: its value mod 2 is fixed by the input alone (see
//      SDMTrellis::integratorInvariant). Two trellises whose invariants differ mod 2 can never
//      be brought into the same state, and switching between them always leaves an in-band
//      transient equivalent to an impulse of that fractional area (measured -52dB to -73dB).
//      The worker's first integrator is therefore started at the value the preceding blocks
//      will have at that point, predicted from block 0's invariant at the start of the call
//      plus the running sum of the input, which removes this mismatch.
//
//   3. Overlap and splice: neighbouring blocks k-1 (stream A) and k (stream B) both modulate
//      an overlap region centred on the nominal block boundary. Both are valid modulations of
//      the same signal, so they agree in the audio band but carry different noise. Switching
//      from A to B at bit n leaves, as a decaying in-band transient, the response of the audio
//      band low pass L(z) to the difference (B - A) accumulated before n. For a linear L(z)
//      with state z(n) driven by (B - A), the energy of that transient is exactly
//
//          E(n) = z(n)' Q z(n)
//
//      where Q is the observability Gramian of L(z). The input PCM cancels from (B - A), so
//      E(n) only needs the two bit streams. E(n) is evaluated for every bit in the overlap
//      and the splice is made where it is smallest.
//
// The joins are not bit perfect: the remaining loop filter states of two independently run
// trellises never match exactly, so a small residual transient remains at each join. See
// SDMTrellisBlockTest for measurements against the sequential modulator. Use larger calls
// or fewer threads to reduce the number of joins.
//-------------------------------------------------------------------------------------------

// Defaults, given in samples per unit of DSD rate (e.g. 32 * 64 = 2048 samples at DSD64).
// A wider overlap gives the splice search more candidates: measured worst joins at DSD64
// were about -80dB for 16, -86dB for 64 and -88dB for 128 per unit of rate.
constexpr int c_SDMTrellisBlockWarmUpPerRate = 128;
constexpr int c_SDMTrellisBlockOverlapPerRate = 64;
// Pre-roll of the splice low pass before the overlap so that its state is settled.
constexpr int c_SDMTrellisBlockPreRollPerRate = 16;
// Audio band low pass used to measure the splice transient: 8th order Butterworth.
constexpr double c_SDMTrellisBlockSpliceCutoff = 20000.0;
constexpr int c_SDMTrellisBlockSpliceSections = 4;
constexpr int c_SDMTrellisBlockSpliceOrder = 2 * c_SDMTrellisBlockSpliceSections;
constexpr double c_SDMTrellisBlockPi = 3.14159265358979323846;

//-------------------------------------------------------------------------------------------

template <typename T> class SDMTrellisBlock : public SDMTrellis<T>
{
	public:
		struct BlockJob
		{
			int index;      // Block index, 0 = this object.
			int start;      // First input sample processed (including warm up).
			int end;        // One past the last input sample processed.
			uint8_t *bits;  // One output bit per byte for samples [start, end).
			double firstIntegrator; // Starting value of the worker's first loop filter integrator.
		};

		struct SpliceJob
		{
			int index;      // Index of the block spliced to (k), from block k - 1.
			int from;       // Overlap region [from, to).
			int to;
			int splice;     // Chosen splice position.
		};

		struct PackJob
		{
			int fromByte;
			int toByte;
		};

	public:
		SDMTrellisBlock();
		virtual ~SDMTrellisBlock();

		// noThreads      - Maximum number of blocks processed in parallel.
		//                  0 = QThreadPool::globalInstance()->maxThreadCount().
		// warmUp         - Samples each worker runs before its output may be used. 0 = 32 * dsdRate.
		// overlap        - Samples over which the splice point between two blocks is searched.
		//                  0 = 64 * dsdRate.
		// minBlockLength - Smallest block size. 0 = 4 * (warmUp + overlap). Fewer blocks than
		//                  noThreads are used when the input is too short to fill them all.
		bool init(int dsdRate, int trellisOrder, int latency, int noThreads = 0,
			int warmUp = 0, int overlap = 0, int minBlockLength = 0);

		int noThreads() const;
		int warmUp() const;
		int overlap() const;
		int minBlockLength() const;

		// Restart the modulator from its initial (silent) state.
		void reset();

		int process(const float *pcm, int noSamples, uint8_t *dsd, int noDSDBytes, bool isLSB);
		int process(const double *pcm, int noSamples, uint8_t *dsd, int noDSDBytes, bool isLSB);

		// Number of blocks used by the most recent call to process.
		int lastNoBlocks() const;
		// Bit positions, relative to the start of the most recent call, at which the output
		// switched from one block to the next.
		const std::vector<int>& lastSplicePoints() const;

	protected:

		// Worker trellis for the blocks after the first. Exposes the protected functionality
		// of SDMTrellis that is needed to run a block.
		class Worker : public SDMTrellis<T>
		{
			public:
				using SDMTrellis<T>::resetTrellis;
				template <typename S> void run(const S *pcm, int noSamples, uint8_t *bits)
				{
					this->processBits(pcm, noSamples, bits);
				}
		};

		int m_noThreads;
		int m_warmUp;
		int m_overlap;
		int m_minBlockLength;
		int m_preRoll;

		std::vector<std::unique_ptr<Worker> > m_workers;
		std::vector<std::vector<uint8_t> > m_bits;
		std::vector<BlockJob> m_jobs;
		std::vector<SpliceJob> m_spliceJobs;
		std::vector<PackJob> m_packJobs;
		std::vector<int> m_splices;
		int m_lastNoBlocks;

		// Splice low pass as a cascade of transposed direct form II biquads.
		double m_lpB0[c_SDMTrellisBlockSpliceSections];
		double m_lpB1[c_SDMTrellisBlockSpliceSections];
		double m_lpB2[c_SDMTrellisBlockSpliceSections];
		double m_lpA1[c_SDMTrellisBlockSpliceSections];
		double m_lpA2[c_SDMTrellisBlockSpliceSections];
		// Observability Gramian of the splice low pass.
		double m_gramian[c_SDMTrellisBlockSpliceOrder][c_SDMTrellisBlockSpliceOrder];

		void releaseBlock();
		int calcNoBlocks(int N) const;

		void initSpliceFilter();
		double stepSpliceFilter(const double *z, double *zNext, double x) const;
		double spliceEnergy(const double *z) const;

		template <typename S> int processBlocks(const S *pcm, int noSamples, uint8_t *dsd, int noDSDBytes, bool isLSB);
		template <typename S> void calcFirstIntegrator(const S *pcm, std::vector<BlockJob>& jobs, int noBlocks);
		template <typename S> void runVibertiTrellis(const S *pcm, std::vector<BlockJob>& jobs, int noBlocks);

		int findSplice(const BlockJob& a, const BlockJob& b, int from, int to) const;
		void pack(int N, uint8_t *dsd, int noDSDBytes, bool isLSB);
};

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellisBlock<T>::SDMTrellisBlock() : SDMTrellis<T>(),
	m_noThreads(1),
	m_warmUp(0),
	m_overlap(0),
	m_minBlockLength(0),
	m_preRoll(0),
	m_lastNoBlocks(0)
{
	for(int k = 0; k < c_SDMTrellisBlockSpliceSections; k++)
	{
		m_lpB0[k] = m_lpB1[k] = m_lpB2[k] = m_lpA1[k] = m_lpA2[k] = 0.0;
	}
	for(int i = 0; i < c_SDMTrellisBlockSpliceOrder; i++)
	{
		for(int j = 0; j < c_SDMTrellisBlockSpliceOrder; j++)
		{
			m_gramian[i][j] = 0.0;
		}
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> SDMTrellisBlock<T>::~SDMTrellisBlock()
{
	releaseBlock();
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellisBlock<T>::releaseBlock()
{
	m_workers.clear();
	m_bits.clear();
	m_jobs.clear();
	m_spliceJobs.clear();
	m_packJobs.clear();
	m_splices.clear();
	m_lastNoBlocks = 0;
}

//-------------------------------------------------------------------------------------------

template <typename T> bool SDMTrellisBlock<T>::init(int dsdRate, int trellisOrder, int latency, int noThreads, int warmUp, int overlap, int minBlockLength)
{
	releaseBlock();

	if(!SDMTrellis<T>::init(dsdRate, trellisOrder, latency))
	{
		return false;
	}
	if(noThreads <= 0)
	{
		noThreads = QThreadPool::globalInstance()->maxThreadCount();
	}
	if(noThreads < 1)
	{
		noThreads = 1;
	}
	if(warmUp <= 0)
	{
		warmUp = c_SDMTrellisBlockWarmUpPerRate * dsdRate;
	}
	if(overlap <= 0)
	{
		overlap = c_SDMTrellisBlockOverlapPerRate * dsdRate;
	}
	// The warm up must at least cover the decision latency so that every kept bit is a real decision.
	if(warmUp < latency || overlap < 2)
	{
		return false;
	}
	overlap &= ~1;

	m_noThreads = noThreads;
	m_warmUp = warmUp;
	m_overlap = overlap;
	m_preRoll = c_SDMTrellisBlockPreRollPerRate * dsdRate;
	m_minBlockLength = (minBlockLength > 0) ? minBlockLength : 4 * (warmUp + overlap);
	// A block must be long enough to hold the warm up and overlap of the block following it.
	m_minBlockLength = (std::max)(m_minBlockLength, warmUp + overlap + 1);

	initSpliceFilter();

	for(int idx = 1; idx < m_noThreads; idx++)
	{
		std::unique_ptr<Worker> worker(new Worker());
		if(!worker->init(dsdRate, trellisOrder, latency))
		{
			releaseBlock();
			return false;
		}
		m_workers.push_back(std::move(worker));
	}
	m_bits.resize(m_noThreads);
	return true;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisBlock<T>::noThreads() const
{
	return m_noThreads;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisBlock<T>::warmUp() const
{
	return m_warmUp;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisBlock<T>::overlap() const
{
	return m_overlap;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisBlock<T>::minBlockLength() const
{
	return m_minBlockLength;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisBlock<T>::lastNoBlocks() const
{
	return m_lastNoBlocks;
}

//-------------------------------------------------------------------------------------------

template <typename T> const std::vector<int>& SDMTrellisBlock<T>::lastSplicePoints() const
{
	return m_splices;
}

//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellisBlock<T>::reset()
{
	this->resetTrellis();
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisBlock<T>::calcNoBlocks(int N) const
{
	int noBlocks = N / m_minBlockLength;
	if(noBlocks > m_noThreads)
	{
		noBlocks = m_noThreads;
	}
	if(noBlocks < 1)
	{
		noBlocks = 1;
	}
	return noBlocks;
}

//-------------------------------------------------------------------------------------------
// Butterworth low pass designed by the bilinear transform per biquad section, followed by
// its observability Gramian Q = sum_t (C A^t)' (C A^t), computed from the zero input
// response of each state.
//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellisBlock<T>::initSpliceFilter()
{
	const int N = c_SDMTrellisBlockSpliceOrder;
	const double fs = static_cast<double>(this->m_rate) * 44100.0;
	const double w0 = 2.0 * c_SDMTrellisBlockPi * c_SDMTrellisBlockSpliceCutoff / fs;

	for(int k = 0; k < c_SDMTrellisBlockSpliceSections; k++)
	{
		double Q = 1.0 / (2.0 * cos(static_cast<double>(2 * k + 1) * c_SDMTrellisBlockPi / (2.0 * N)));
		double alpha = sin(w0) / (2.0 * Q);
		double cw = cos(w0);
		double a0 = 1.0 + alpha;
		m_lpB0[k] = ((1.0 - cw) / 2.0) / a0;
		m_lpB1[k] = (1.0 - cw) / a0;
		m_lpB2[k] = ((1.0 - cw) / 2.0) / a0;
		m_lpA1[k] = (-2.0 * cw) / a0;
		m_lpA2[k] = (1.0 - alpha) / a0;
	}

	// The impulse response decays by e per ~3 / w0 samples. 400 * rate covers well over 50
	// time constants for all supported rates.
	const int noSteps = 400 * this->m_rate;
	std::vector<std::vector<double> > response(N, std::vector<double>(noSteps));
	for(int i = 0; i < N; i++)
	{
		double z[N], zNext[N];
		for(int j = 0; j < N; j++)
		{
			z[j] = (i == j) ? 1.0 : 0.0;
		}
		for(int t = 0; t < noSteps; t++)
		{
			response[i][t] = stepSpliceFilter(z, zNext, 0.0);
			memcpy(z, zNext, sizeof(z));
		}
	}
	for(int i = 0; i < N; i++)
	{
		for(int j = i; j < N; j++)
		{
			double sum = 0.0;
			for(int t = 0; t < noSteps; t++)
			{
				sum += response[i][t] * response[j][t];
			}
			m_gramian[i][j] = sum;
			m_gramian[j][i] = sum;
		}
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> double SDMTrellisBlock<T>::stepSpliceFilter(const double *z, double *zNext, double x) const
{
	double in = x;
	for(int k = 0; k < c_SDMTrellisBlockSpliceSections; k++)
	{
		double y = m_lpB0[k] * in + z[2 * k];
		zNext[2 * k] = m_lpB1[k] * in - m_lpA1[k] * y + z[2 * k + 1];
		zNext[2 * k + 1] = m_lpB2[k] * in - m_lpA2[k] * y;
		in = y;
	}
	return in;
}

//-------------------------------------------------------------------------------------------

template <typename T> double SDMTrellisBlock<T>::spliceEnergy(const double *z) const
{
	double e = 0.0;
	for(int i = 0; i < c_SDMTrellisBlockSpliceOrder; i++)
	{
		double t = 0.0;
		for(int j = 0; j < c_SDMTrellisBlockSpliceOrder; j++)
		{
			t += m_gramian[i][j] * z[j];
		}
		e += z[i] * t;
	}
	return e;
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisBlock<T>::process(const float *pcm, int noSamples, uint8_t *dsd, int noDSDBytes, bool isLSB)
{
	return processBlocks<float>(pcm, noSamples, dsd, noDSDBytes, isLSB);
}

//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisBlock<T>::process(const double *pcm, int noSamples, uint8_t *dsd, int noDSDBytes, bool isLSB)
{
	return processBlocks<double>(pcm, noSamples, dsd, noDSDBytes, isLSB);
}

//-------------------------------------------------------------------------------------------
// Start each worker's first integrator in step, mod 2, with the trellis it joins:
//     J(start) = J0 + sum(x[0..start-1]) - start   (mod 2)
// where J0 is the invariant of block 0 at the start of this call and x is the input
// exactly as the trellis sees it.
//-------------------------------------------------------------------------------------------

template <typename T> template <typename S> void SDMTrellisBlock<T>::calcFirstIntegrator(const S *pcm, std::vector<BlockJob>& jobs, int noBlocks)
{
	const double J0 = this->integratorInvariant();
	double prefix = 0.0;
	for(int k = 1; k < noBlocks; k++)
	{
		BlockJob& job = jobs[k];
		for(int pos = 0; pos < job.start; pos++)
		{
			prefix += static_cast<double>(static_cast<T>(pcm[pos]) * c_SDMTrellisGain);
		}
		double v = fmod(J0 + prefix - static_cast<double>(job.start & 1), 2.0);
		if(v >= 1.0)
		{
			v -= 2.0;
		}
		else if(v < -1.0)
		{
			v += 2.0;
		}
		job.firstIntegrator = v;
	}
}

//-------------------------------------------------------------------------------------------

template <typename T> template <typename S> void SDMTrellisBlock<T>::runVibertiTrellis(const S *pcm, std::vector<BlockJob>& jobs, int noBlocks)
{
	calcFirstIntegrator(pcm, jobs, noBlocks);

	// Modulate all blocks in parallel.
	QtConcurrent::blockingMap(jobs, [this, pcm](BlockJob& job) {
		if(job.index == 0)
		{
			// Continue the trellis from where the previous call left off.
			this->processBits(pcm, job.end, job.bits);
		}
		else
		{
			Worker *worker = m_workers[job.index - 1].get();
			worker->resetTrellis(static_cast<T>(job.firstIntegrator));
			worker->run(pcm + job.start, job.end - job.start, job.bits);
		}
	});	
}

//-------------------------------------------------------------------------------------------

template <typename T> template <typename S> int SDMTrellisBlock<T>::processBlocks(const S *pcm, int noSamples, uint8_t *dsd, int noDSDBytes, bool isLSB)
{
	int N = (std::min)(noSamples, noDSDBytes << 3);

	m_splices.clear();
	m_lastNoBlocks = 0;
	if(pcm == nullptr || dsd == nullptr || N <= 0 || m_bits.empty())
	{
		return 0;
	}

	const int noBlocks = calcNoBlocks(N);
	const int halfOverlap = m_overlap >> 1;

	// Nominal block boundaries.
	std::vector<int> boundary(noBlocks + 1);
	for(int k = 0; k <= noBlocks; k++)
	{
		boundary[k] = static_cast<int>((static_cast<int64_t>(N) * k) / noBlocks);
	}

	m_jobs.resize(noBlocks);
	for(int k = 0; k < noBlocks; k++)
	{
		BlockJob& job = m_jobs[k];
		job.index = k;
		job.start = (k == 0) ? 0 : boundary[k] - halfOverlap - m_warmUp;
		job.end = (k == noBlocks - 1) ? N : boundary[k + 1] + halfOverlap;
		m_bits[k].resize(job.end - job.start);
		job.bits = m_bits[k].data();
		job.firstIntegrator = 0.0;
	}
	
	runTrellis(pcm, m_jobs, noBlocks);

	// Choose the splice point within each overlap region, again in parallel.
	m_spliceJobs.resize(noBlocks - 1);
	for(int k = 1; k < noBlocks; k++)
	{
		SpliceJob& sJob = m_spliceJobs[k - 1];
		sJob.index = k;
		sJob.from = boundary[k] - halfOverlap;
		sJob.to = boundary[k] + halfOverlap;
		sJob.splice = boundary[k];
	}
	QtConcurrent::blockingMap(m_spliceJobs, [this](SpliceJob& sJob) {
		sJob.splice = findSplice(m_jobs[sJob.index - 1], m_jobs[sJob.index], sJob.from, sJob.to);
	});

	m_splices.resize(noBlocks + 1);
	m_splices[0] = 0;
	for(int k = 1; k < noBlocks; k++)
	{
		m_splices[k] = m_spliceJobs[k - 1].splice;
	}
	m_splices[noBlocks] = N;

	pack(N, dsd, noDSDBytes, isLSB);

	// The final block holds the trellis state at the end of this call. Hand it back to this
	// object so that the next call continues seamlessly from it.
	if(noBlocks > 1)
	{
		this->copyTrellisFrom(*m_workers[noBlocks - 2]);
	}

	// Expose only the interior splice points.
	m_splices.erase(m_splices.begin());
	m_splices.pop_back();
	m_lastNoBlocks = noBlocks;
	return N / 8;
}

//-------------------------------------------------------------------------------------------
// Find the bit position n in [from, to) at which switching from block a to block b (bits < n
// from a, bits >= n from b) leaves the smallest in-band transient E(n) = z(n)' Q z(n), where
// z(n) is the state of the splice low pass driven by the bit difference (b - a) up to n.
// The filter is started from rest a pre-roll before the overlap; its memory is far shorter
// than the pre-roll so the start has no effect on z within the overlap.
//-------------------------------------------------------------------------------------------

template <typename T> int SDMTrellisBlock<T>::findSplice(const BlockJob& a, const BlockJob& b, int from, int to) const
{
	const uint8_t *bitsA = a.bits - a.start;
	const uint8_t *bitsB = b.bits - b.start;
	const int start = (std::max)((std::max)(from - m_preRoll, a.start), b.start);

	double z[c_SDMTrellisBlockSpliceOrder], zNext[c_SDMTrellisBlockSpliceOrder];
	for(int i = 0; i < c_SDMTrellisBlockSpliceOrder; i++)
	{
		z[i] = 0.0;
	}

	double minEnergy = (std::numeric_limits<double>::max)();
	int best = (from + to) >> 1;

	for(int n = start; n < to; n++)
	{
		if(n >= from)
		{
			double e = spliceEnergy(z);
			if(e < minEnergy)
			{
				minEnergy = e;
				best = n;
			}
		}
		// Bits are 0 / 1 for -1 / +1 so the difference of the bipolar values is 2 * (b - a).
		double d = 2.0 * static_cast<double>(static_cast<int>(bitsB[n]) - static_cast<int>(bitsA[n]));
		stepSpliceFilter(z, zNext, d);
		memcpy(z, zNext, sizeof(z));
	}
	return best;
}

//-------------------------------------------------------------------------------------------
// Pack the chosen bits of each block into the DSD byte stream. Done in parallel over whole
// byte ranges so that no two threads ever write the same byte.
//-------------------------------------------------------------------------------------------

template <typename T> void SDMTrellisBlock<T>::pack(int N, uint8_t *dsd, int noDSDBytes, bool isLSB)
{
	const int noBytes = (std::min)((N + 7) >> 3, noDSDBytes);
	const int noBlocks = static_cast<int>(m_jobs.size());

	memset(dsd, 0, noDSDBytes);

	m_packJobs.resize(noBlocks);
	for(int k = 0; k < noBlocks; k++)
	{
		m_packJobs[k].fromByte = static_cast<int>((static_cast<int64_t>(noBytes) * k) / noBlocks);
		m_packJobs[k].toByte = static_cast<int>((static_cast<int64_t>(noBytes) * (k + 1)) / noBlocks);
	}

	QtConcurrent::blockingMap(m_packJobs, [this, N, dsd, isLSB](const PackJob& pJob) {
		const int fromBit = pJob.fromByte << 3;
		const int toBit = (std::min)(pJob.toByte << 3, N);
		// Find the block that supplies the first bit of the range.
		int k = static_cast<int>(std::upper_bound(m_splices.begin(), m_splices.end(), fromBit) - m_splices.begin()) - 1;
		for(int n = fromBit; n < toBit; n++)
		{
			while(n >= m_splices[k + 1])
			{
				k++;
			}
			const BlockJob& job = m_jobs[k];
			if(job.bits[n - job.start])
			{
				int shift = (isLSB) ? (n & 0x7) : (7 - (n & 0x7));
				dsd[n >> 3] |= static_cast<uint8_t>(1 << shift);
			}
		}
	});
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
