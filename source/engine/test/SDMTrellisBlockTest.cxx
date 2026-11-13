#include "gtest/gtest.h"

#include <cmath>
#include <vector>

#include "engine/inc/SDMTrellis.h"
#include "engine/inc/SDMTrellisBlock.h"

using namespace omega;
using namespace omega::engine;

//-------------------------------------------------------------------------------------------

namespace
{

constexpr double c_pi = 3.14159265358979323846;
constexpr int c_dsdRate = 64;
constexpr int c_order = 8;
constexpr int c_latency = 31;
constexpr double c_fs = 44100.0 * c_dsdRate;

double fsOf(int rate)
{
	return 44100.0 * static_cast<double>(rate);
}

//-------------------------------------------------------------------------------------------

std::vector<float> generateTones(int N, const std::vector<std::pair<double, double> >& tones, int rate = c_dsdRate)
{
	const double fs = fsOf(rate);
	std::vector<float> x(N, 0.0f);
	for(int i = 0; i < N; i++)
	{
		double v = 0.0;
		for(const auto& t : tones)
		{
			v += t.second * sin(2.0 * c_pi * t.first * static_cast<double>(i) / fs);
		}
		x[i] = static_cast<float>(v);
	}
	return x;
}

//-------------------------------------------------------------------------------------------

std::vector<int> unpackBits(const std::vector<uint8_t>& dsd, int N)
{
	std::vector<int> bits(N);
	for(int i = 0; i < N; i++)
	{
		bits[i] = (dsd[i >> 3] & (0x80 >> (i & 0x7))) ? 1 : 0;
	}
	return bits;
}

//-------------------------------------------------------------------------------------------
// 8th order Butterworth low pass at 20kHz built from 4 bilinear biquads.

class AudioBandFilter
{
	public:
		AudioBandFilter(double fc, double fs)
		{
			for(int k = 0; k < 4; k++)
			{
				double Q = 1.0 / (2.0 * cos(static_cast<double>(2 * k + 1) * c_pi / 16.0));
				double w0 = 2.0 * c_pi * fc / fs;
				double alpha = sin(w0) / (2.0 * Q);
				double cw = cos(w0);
				double a0 = 1.0 + alpha;
				m_b0[k] = ((1.0 - cw) / 2.0) / a0;
				m_b1[k] = (1.0 - cw) / a0;
				m_b2[k] = ((1.0 - cw) / 2.0) / a0;
				m_a1[k] = (-2.0 * cw) / a0;
				m_a2[k] = (1.0 - alpha) / a0;
			}
		}

		std::vector<double> filter(const std::vector<double>& x) const
		{
			std::vector<double> y(x);
			for(int k = 0; k < 4; k++)
			{
				double x1 = 0.0, x2 = 0.0, y1 = 0.0, y2 = 0.0;
				for(size_t i = 0; i < y.size(); i++)
				{
					double in = y[i];
					double out = m_b0[k] * in + m_b1[k] * x1 + m_b2[k] * x2 - m_a1[k] * y1 - m_a2[k] * y2;
					x2 = x1; x1 = in;
					y2 = y1; y1 = out;
					y[i] = out;
				}
			}
			return y;
		}

	private:
		double m_b0[4], m_b1[4], m_b2[4], m_a1[4], m_a2[4];
};

//-------------------------------------------------------------------------------------------
// In-band error of a DSD stream against its input. Output bit i is the decision for input
// i - latency so the input is delayed to match before comparing.

std::vector<double> inBandError(const std::vector<float>& x, const std::vector<int>& bits, int rate = c_dsdRate)
{
	const int N = static_cast<int>(bits.size());
	const int delay = c_latency;
	AudioBandFilter lp(20000.0, fsOf(rate));

	std::vector<double> d(N), xd(N, 0.0);
	for(int i = 0; i < N; i++)
	{
		d[i] = bits[i] ? 1.0 : -1.0;
		if(i >= delay)
		{
			xd[i] = static_cast<double>(x[i - delay]);
		}
	}
	std::vector<double> yd = lp.filter(d);
	std::vector<double> yx = lp.filter(xd);

	// Least squares gain between the input and the modulated output.
	double num = 0.0, den = 0.0;
	for(int i = 0; i < N; i++)
	{
		num += yd[i] * yx[i];
		den += yx[i] * yx[i];
	}
	double g = (den > 0.0) ? num / den : 0.0;

	std::vector<double> e(N);
	for(int i = 0; i < N; i++)
	{
		e[i] = yd[i] - g * yx[i];
	}
	return e;
}

//-------------------------------------------------------------------------------------------

double rms(const std::vector<double>& e, int from, int to)
{
	double sum = 0.0;
	from = (std::max)(from, 0);
	to = (std::min)(to, static_cast<int>(e.size()));
	for(int i = from; i < to; i++)
	{
		sum += e[i] * e[i];
	}
	return (to > from) ? sqrt(sum / static_cast<double>(to - from)) : 0.0;
}

//-------------------------------------------------------------------------------------------

double toDB(double v)
{
	return 20.0 * log10((std::max)(v, 1.0e-12));
}

//-------------------------------------------------------------------------------------------
// Process x in noCalls equal calls. Returns the unpacked bits and the positions of every
// join (block splices and call boundaries).

template <typename Modulator> std::vector<int> modulate(Modulator& mod, const std::vector<float>& x, int noCalls, std::vector<int> *joins)
{
	const int N = static_cast<int>(x.size());
	const int callLen = N / noCalls;
	std::vector<uint8_t> dsd(N / 8, 0);

	for(int c = 0; c < noCalls; c++)
	{
		int offset = c * callLen;
		EXPECT_EQ(mod.process(&x[offset], callLen, &dsd[offset / 8], callLen / 8, false), callLen / 8);
		if(joins != nullptr)
		{
			if(c > 0)
			{
				joins->push_back(offset);
			}
			if constexpr (std::is_same<Modulator, SDMTrellisBlock<float> >::value)
			{
				for(int s : mod.lastSplicePoints())
				{
					joins->push_back(offset + s);
				}
			}
		}
	}
	return unpackBits(dsd, N);
}

//-------------------------------------------------------------------------------------------
// The window over which a join is measured: from just before the join to well after the
// decay of its transient through the 20kHz low pass. Given in samples at DSD64 and scaled
// with the rate. The error shows up delayed by the decision latency.

constexpr int c_joinPre = 256;
constexpr int c_joinPost = 2048;

int joinPre(int rate) { return c_joinPre * rate / c_dsdRate; }
int joinPost(int rate) { return c_joinPost * rate / c_dsdRate; }

double worstJoinRMS(const std::vector<double>& e, const std::vector<int>& joins, int rate = c_dsdRate)
{
	double worst = 0.0;
	for(int j : joins)
	{
		worst = (std::max)(worst, rms(e, j + c_latency - joinPre(rate), j + c_latency + joinPost(rate)));
	}
	return worst;
}

//-------------------------------------------------------------------------------------------
// RMS error excluding the join windows.

double rmsAwayFromJoins(const std::vector<double>& e, const std::vector<int>& joins, int from, int rate = c_dsdRate)
{
	std::vector<bool> mask(e.size(), true);
	for(int j : joins)
	{
		for(int i = j + c_latency - joinPre(rate); i < j + c_latency + joinPost(rate); i++)
		{
			if(i >= 0 && i < static_cast<int>(e.size()))
			{
				mask[i] = false;
			}
		}
	}
	double sum = 0.0;
	int n = 0;
	for(int i = from; i < static_cast<int>(e.size()); i++)
	{
		if(mask[i])
		{
			sum += e[i] * e[i];
			n++;
		}
	}
	return (n > 0) ? sqrt(sum / n) : 0.0;
}

//-------------------------------------------------------------------------------------------
// Measures the in-band (20kHz) error of the block modulator at every join (block splices and
// call boundaries) against the sequential modulator and against a cold start without warm
// up or splice search.

constexpr int c_joinCalls = 2;
constexpr int c_joinThreads = 8;

// Number of samples, at least minN, that gives every call one block per thread with the
// default warm up and overlap of the given rate (the minimum block length scales with both).
int joinTestLength(int minN, int rate = c_dsdRate)
{
	SDMTrellisBlock<float> probe;
	EXPECT_TRUE(probe.init(rate, c_order, c_latency, c_joinThreads));
	const int step = c_joinCalls * 8;
	int N = c_joinCalls * c_joinThreads * probe.minBlockLength();
	N = ((N + step - 1) / step) * step;
	return (std::max)(N, minN);
}

void testJoins(const char *name, const std::vector<float>& x, double maxJoinDB, int rate = c_dsdRate, double minDBOverCold = 15.0)
{
	const int noCalls = c_joinCalls;
	const int noThreads = c_joinThreads;
	const int settle = 1 << 14;

	SDMTrellis<float> ref;
	ASSERT_TRUE(ref.init(rate, c_order, c_latency));
	std::vector<int> bitsRef = modulate(ref, x, noCalls, nullptr);
	std::vector<double> eRef = inBandError(x, bitsRef, rate);

	std::vector<int> joins;
	SDMTrellisBlock<float> blk;
	ASSERT_TRUE(blk.init(rate, c_order, c_latency, noThreads));
	std::vector<int> bitsBlk = modulate(blk, x, noCalls, &joins);
	std::vector<double> eBlk = inBandError(x, bitsBlk, rate);
	ASSERT_EQ(static_cast<int>(joins.size()), noCalls * noThreads - 1);

	std::vector<int> coldJoins;
	SDMTrellisBlock<float> cold;
	ASSERT_TRUE(cold.init(rate, c_order, c_latency, noThreads, c_latency, 2, 1024));
	std::vector<int> bitsCold = modulate(cold, x, noCalls, &coldJoins);
	std::vector<double> eCold = inBandError(x, bitsCold, rate);

	double refAway = rmsAwayFromJoins(eRef, joins, settle, rate);
	double refJoin = worstJoinRMS(eRef, joins, rate);
	double blkAway = rmsAwayFromJoins(eBlk, joins, settle, rate);
	double blkJoin = worstJoinRMS(eBlk, joins, rate);
	double coldJoin = worstJoinRMS(eCold, coldJoins, rate);

	printf("%s at DSD%d: 20kHz band error RMS (dB)\n", name, rate);
	printf("  SDMTrellis       away from joins %.1f, worst at join positions %.1f\n", toDB(refAway), toDB(refJoin));
	printf("  SDMTrellisBlock  away from joins %.1f, worst join %.1f\n", toDB(blkAway), toDB(blkJoin));
	printf("  Cold start       worst join %.1f\n", toDB(coldJoin));

	// Away from the joins the block modulator is as good as the sequential one.
	EXPECT_LT(toDB(blkAway), toDB(refAway) + 1.0) << name;
	// Warm up, integrator start and splice search improve every join well over a cold start.
	EXPECT_LT(toDB(blkJoin), toDB(coldJoin) - minDBOverCold) << name;
	// Absolute bound on the residual join transient.
	EXPECT_LT(toDB(blkJoin), maxJoinDB) << name;
}

} // namespace

//-------------------------------------------------------------------------------------------
// With a single thread the block modulator must be bit identical to SDMTrellis, including
// across process calls.

TEST(SDMTrellisBlock, singleThreadMatchesSDMTrellis)
{
	const int N = 1 << 18;
	std::vector<float> x = generateTones(N, { { 1000.0, 0.5 } });

	SDMTrellis<float> ref;
	ASSERT_TRUE(ref.init(c_dsdRate, c_order, c_latency));
	SDMTrellisBlock<float> blk;
	ASSERT_TRUE(blk.init(c_dsdRate, c_order, c_latency, 1));

	std::vector<int> bitsRef = modulate(ref, x, 4, nullptr);
	std::vector<int> bitsBlk = modulate(blk, x, 4, nullptr);
	EXPECT_EQ(blk.lastNoBlocks(), 1);
	EXPECT_TRUE(bitsRef == bitsBlk);
}

//-------------------------------------------------------------------------------------------
// Exposes the trellis internals needed to reproduce a worker block.

class TestTrellis : public SDMTrellis<float>
{
	public:
		using SDMTrellis<float>::resetTrellis;
		using SDMTrellis<float>::integratorInvariant;
		using SDMTrellis<float>::processBits;
};

//-------------------------------------------------------------------------------------------
// The first loop filter integrator invariant J mod 2 must follow the input alone. Checked on
// the sequential modulator against the running sum of its input.

TEST(SDMTrellisBlock, integratorInvariantFollowsInput)
{
	const int N = 1 << 18;
	std::vector<float> x = generateTones(N, { { 1000.0, 0.5 }, { 7100.0, 0.2 } });
	TestTrellis t;
	ASSERT_TRUE(t.init(c_dsdRate, c_order, c_latency));
	std::vector<uint8_t> bits(N);
	double prefix = 0.0, maxDev = 0.0;
	for(int i = 0; i < N; i++)
	{
		t.processBits(&x[i], 1, &bits[i]);
		prefix += static_cast<double>(x[i] * c_SDMTrellisGain);
		double v = fmod(t.integratorInvariant() - (prefix - static_cast<double>((i + 1) & 1)), 2.0);
		if(v >= 1.0) v -= 2.0;
		if(v < -1.0) v += 2.0;
		maxDev = (std::max)(maxDev, fabs(v));
	}
	printf("Max deviation of the integrator invariant from the input prediction: %.3g\n", maxDev);
	EXPECT_LT(maxDev, 0.01);
}

//-------------------------------------------------------------------------------------------
// Each block after the first must be identical to a fresh SDMTrellis started at the same
// position with the same first integrator, over repeated calls (checks the worker reset and
// the hand over of the final trellis between calls). Also checks that the bits before the
// splice come from the first block, the sequential modulator in the first call.

TEST(SDMTrellisBlock, workerBlockMatchesFreshSDMTrellis)
{
	const int N = 1 << 17;
	const int warmUp = 2048, overlap = 1024;
	std::vector<float> x = generateTones(N, { { 1000.0, 0.5 } });

	SDMTrellisBlock<float> blk;
	ASSERT_TRUE(blk.init(c_dsdRate, c_order, c_latency, 2, warmUp, overlap));
	SDMTrellis<float> seq;
	ASSERT_TRUE(seq.init(c_dsdRate, c_order, c_latency));

	const int start = (N / 2) - (overlap / 2) - warmUp;
	double prefix = 0.0;
	for(int i = 0; i < start; i++)
	{
		prefix += static_cast<double>(static_cast<float>(x[i]) * c_SDMTrellisGain);
	}
	// Invariant of the trellis that block 0 continues from: zero at the very start.
	double J0 = 0.0;

	for(int call = 0; call < 2; call++)
	{
		std::vector<uint8_t> dsd(N / 8);
		ASSERT_EQ(blk.process(x.data(), N, dsd.data(), N / 8, false), N / 8);
		ASSERT_EQ(blk.lastNoBlocks(), 2);
		ASSERT_EQ(blk.lastSplicePoints().size(), 1);
		int splice = blk.lastSplicePoints()[0];
		EXPECT_GE(splice, (N / 2) - (overlap / 2));
		EXPECT_LT(splice, (N / 2) + (overlap / 2));
		std::vector<int> bitsBlk = unpackBits(dsd, N);

		// Second block: fresh trellis started at the worker start with the predicted integrator.
		double v = fmod(J0 + prefix - static_cast<double>(start & 1), 2.0);
		if(v >= 1.0) v -= 2.0;
		else if(v < -1.0) v += 2.0;
		TestTrellis fresh;
		ASSERT_TRUE(fresh.init(c_dsdRate, c_order, c_latency));
		fresh.resetTrellis(static_cast<float>(v));
		std::vector<uint8_t> bitsFresh(N - start);
		fresh.processBits(&x[start], N - start, bitsFresh.data());

		int noDiff = 0;
		for(int i = splice; i < N; i++)
		{
			if(bitsBlk[i] != bitsFresh[i - start])
			{
				noDiff++;
			}
		}
		EXPECT_EQ(noDiff, 0) << "call " << call;

		// The next call continues from the trellis of this final block.
		J0 = fresh.integratorInvariant();

		// First block: in the first call it is the sequential modulator from silence.
		if(call == 0)
		{
			std::vector<uint8_t> dsdSeq(N / 8);
			seq.process(x.data(), N, dsdSeq.data(), N / 8, false);
			std::vector<int> bitsSeq = unpackBits(dsdSeq, N);
			noDiff = 0;
			for(int i = 0; i < splice; i++)
			{
				if(bitsBlk[i] != bitsSeq[i])
				{
					noDiff++;
				}
			}
			EXPECT_EQ(noDiff, 0);
		}
	}
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellisBlock, joinsSine)
{
	testJoins("1kHz -6dB", generateTones(1 << 20, { { 1000.0, 0.5 } }), -80.0);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellisBlock, joinsTwoTone)
{
	testJoins("440Hz + 7.1kHz", generateTones(1 << 20, { { 440.0, 0.3 }, { 7100.0, 0.2 } }), -80.0);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellisBlock, joinsSilence)
{
	// A cold start into silence has no signal to disturb, so its join is far cleaner (around
	// -73dB against -46dB with a tone) and the margin over it is smaller. The absolute bound
	// still holds the block joins to the same level as with tones.
	testJoins("silence", std::vector<float>(1 << 20, 0.0f), -80.0, c_dsdRate, 10.0);
}

//-------------------------------------------------------------------------------------------
// The higher DSD rates use different loop filters, which the trellis must also keep stable
// across block joins.

TEST(SDMTrellisBlock, joinsTwoToneDSD128)
{
	testJoins("440Hz + 7.1kHz", generateTones(joinTestLength(1 << 21, 128), { { 440.0, 0.3 }, { 7100.0, 0.2 } }, 128), -80.0, 128);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellisBlock, joinsTwoToneDSD256)
{
	testJoins("440Hz + 7.1kHz", generateTones(joinTestLength(1 << 21, 256), { { 440.0, 0.3 }, { 7100.0, 0.2 } }, 256), -80.0, 256);
}

//-------------------------------------------------------------------------------------------
// The warm up and overlap defaults scale with the rate, so the blocks must grow with it to
// keep one block per thread.

TEST(SDMTrellisBlock, joinsTwoToneDSD512)
{
	testJoins("440Hz + 7.1kHz", generateTones(joinTestLength(1 << 22, 512), { { 440.0, 0.3 }, { 7100.0, 0.2 } }, 512), -80.0, 512);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTrellisBlock, joinsTwoToneDSD1024)
{
	testJoins("440Hz + 7.1kHz", generateTones(joinTestLength(1 << 23, 1024), { { 440.0, 0.3 }, { 7100.0, 0.2 } }, 1024), -80.0, 1024);
}

//-------------------------------------------------------------------------------------------
