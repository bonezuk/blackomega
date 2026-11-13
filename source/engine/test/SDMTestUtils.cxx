#include "engine/test/SDMTestUtils.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

namespace
{

constexpr double c_pi = 3.14159265358979323846;
constexpr int c_noSections = 4;

//-------------------------------------------------------------------------------------------
// 8th order Butterworth low pass built from 4 bilinear biquads, run one sample at a time as
// a cascade of transposed direct form II sections. Unity gain at DC.

class AudioBandLowPass
{
	public:
		AudioBandLowPass(double cutoff, double fs)
		{
			const double w0 = 2.0 * c_pi * cutoff / fs;
			const double cw = cos(w0);
			for(int k = 0; k < c_noSections; k++)
			{
				double Q = 1.0 / (2.0 * cos(static_cast<double>(2 * k + 1) * c_pi / (4.0 * c_noSections)));
				double alpha = sin(w0) / (2.0 * Q);
				double a0 = 1.0 + alpha;
				m_b0[k] = ((1.0 - cw) / 2.0) / a0;
				m_b1[k] = (1.0 - cw) / a0;
				m_b2[k] = ((1.0 - cw) / 2.0) / a0;
				m_a1[k] = (-2.0 * cw) / a0;
				m_a2[k] = (1.0 - alpha) / a0;
				m_z[2 * k] = 0.0;
				m_z[2 * k + 1] = 0.0;
			}
		}

		double step(double x)
		{
			for(int k = 0; k < c_noSections; k++)
			{
				double y = m_b0[k] * x + m_z[2 * k];
				m_z[2 * k] = m_b1[k] * x - m_a1[k] * y + m_z[2 * k + 1];
				m_z[2 * k + 1] = m_b2[k] * x - m_a2[k] * y;
				x = y;
			}
			return x;
		}

	private:
		double m_b0[c_noSections], m_b1[c_noSections], m_b2[c_noSections];
		double m_a1[c_noSections], m_a2[c_noSections];
		double m_z[2 * c_noSections];
};

//-------------------------------------------------------------------------------------------

double powerToDB(double power)
{
	return (power > 0.0) ? (std::max)(10.0 * log10(power), c_SDMTestSilenceDB) : c_SDMTestSilenceDB;
}

} // namespace

//-------------------------------------------------------------------------------------------

SDMStreamComparison sdmTestCompareDSDStreams(const uint8_t *bitsA, const uint8_t *bitsB, int noSamples, int dsdRate)
{
	SDMStreamComparison r;
	r.noSamples = 0;
	r.noDifferences = 0;
	r.firstDifference = -1;
	r.signalDB = c_SDMTestSilenceDB;
	r.differenceDB = c_SDMTestSilenceDB;
	r.worstDifferenceDB = c_SDMTestSilenceDB;
	r.worstWindow = -1;
	r.snrDB = 0.0;

	if(bitsA == nullptr || bitsB == nullptr || noSamples <= 0 || dsdRate <= 0)
	{
		return r;
	}
	r.noSamples = noSamples;

	const double fs = 44100.0 * static_cast<double>(dsdRate);
	const int window = c_SDMTestWindowPerRate * dsdRate;
	AudioBandLowPass lpSignal(c_SDMTestAudioBandCutoff, fs);
	AudioBandLowPass lpDifference(c_SDMTestAudioBandCutoff, fs);

	double sumSignal = 0.0, sumDifference = 0.0;
	// Window being filled, and the last full window before it.
	double sumWindow = 0.0, sumPrevWindow = 0.0;
	int noWindow = 0, windowStart = 0, prevWindowStart = -1;
	double worstPower = 0.0;

	for(int i = 0; i < noSamples; i++)
	{
		const int a = (bitsA[i]) ? 1 : 0;
		const int b = (bitsB[i]) ? 1 : 0;

		if(a != b)
		{
			if(r.firstDifference < 0)
			{
				r.firstDifference = i;
			}
			r.noDifferences++;
		}

		double s = lpSignal.step((a) ? 1.0 : -1.0);
		sumSignal += s * s;

		// The filter is linear, so the difference of the two filtered streams is the filtered
		// difference of the bits. It stays exactly zero until the streams first differ.
		if(r.noDifferences > 0)
		{
			double d = lpDifference.step(2.0 * static_cast<double>(a - b));
			d *= d;
			sumDifference += d;
			sumWindow += d;
		}

		noWindow++;
		if(noWindow == window)
		{
			double power = sumWindow / static_cast<double>(window);
			if(power > worstPower)
			{
				worstPower = power;
				r.worstWindow = windowStart;
			}
			sumPrevWindow = sumWindow;
			prevWindowStart = windowStart;
			sumWindow = 0.0;
			noWindow = 0;
			windowStart = i + 1;
		}
	}

	// The samples left over at the end. Too few of them give a poor estimate of the level, in
	// which case they are measured together with the last full window instead.
	if(noWindow > 0)
	{
		double power = sumWindow / static_cast<double>(noWindow);
		int start = windowStart;
		if(noWindow < (window >> 1) && prevWindowStart >= 0)
		{
			power = (sumPrevWindow + sumWindow) / static_cast<double>(window + noWindow);
			start = prevWindowStart;
		}
		if(power > worstPower)
		{
			worstPower = power;
			r.worstWindow = start;
		}
	}

	r.signalDB = powerToDB(sumSignal / static_cast<double>(noSamples));
	r.differenceDB = powerToDB(sumDifference / static_cast<double>(noSamples));
	r.worstDifferenceDB = powerToDB(worstPower);
	r.snrDB = r.signalDB - r.differenceDB;
	return r;
}

//-------------------------------------------------------------------------------------------

::testing::AssertionResult sdmTestDSDStreamsEquivalent(const uint8_t *expect, const uint8_t *actual, int noSamples, int dsdRate,
	double maxDifferenceDB, SDMStreamComparison *pResult)
{
	SDMStreamComparison r = sdmTestCompareDSDStreams(expect, actual, noSamples, dsdRate);
	if(pResult != nullptr)
	{
		*pResult = r;
	}

	if(r.noSamples <= 0)
	{
		return ::testing::AssertionFailure() << "no DSD stream to compare (" << noSamples << " samples at DSD" << dsdRate << ")";
	}
	if(r.isIdentical() || r.worstDifferenceDB <= maxDifferenceDB)
	{
		return ::testing::AssertionSuccess();
	}
	char msg[512];
	snprintf(msg, sizeof(msg), "DSD streams differ in the audio band: worst window at bit %d is %.1fdB (limit %.1fdB), "
		"whole stream %.1fdB, signal %.1fdB, SNR %.1fdB. %d of %d bits differ, the first at bit %d",
		r.worstWindow, r.worstDifferenceDB, maxDifferenceDB, r.differenceDB, r.signalDB, r.snrDB,
		r.noDifferences, r.noSamples, r.firstDifference);
	return ::testing::AssertionFailure() << msg;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
