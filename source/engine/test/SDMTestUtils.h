//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_TEST_SDMTESTUTILS_H
#define __OMEGA_ENGINE_TEST_SDMTESTUTILS_H
//-------------------------------------------------------------------------------------------

#include <cstdint>

#include "gtest/gtest.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------
// Comparison of two 1-bit DSD streams that are expected to carry the same audio.
//
// Two sigma-delta modulators given the same input do not have to produce the same bits to be
// the same audio. Once a single decision differs (for example from a rounding difference
// between a SIMD and a CUDA implementation) the two noise trajectories separate and never
// meet again, so beyond that point roughly half of all bits differ. Both streams are still
// valid modulations of the input: they are y = x + NTF(e) for two different error sequences
// e, so their difference (a - b) = NTF(ea - eb) is noise shaped and holds next to nothing in
// the audio band. A stream that is not a valid modulation of the same input (wrong state
// update, wrong latency, dropped or shifted samples, instability) does not cancel like this
// and leaves a difference in the audio band.
//
// The measure of sameness is therefore the level of the difference between the streams once
// low passed to the audio band, relative to DSD full scale:
//
//     d(n) = LP20kHz(a(n) - b(n))   with the bits taken as -1 / +1
//
// This is measured over the whole stream and also over consecutive short windows, so that a
// local glitch cannot hide in the average of an otherwise matching stream. Two streams are
// the same when the level of the worst window is below a given limit. Bit identical streams
// have no difference at all and are always the same.
//-------------------------------------------------------------------------------------------

// The audio band: 8th order Butterworth low pass.
constexpr double c_SDMTestAudioBandCutoff = 20000.0;
// Length of a measurement window, in samples per unit of DSD rate (32 * 64 = 2048 at DSD64).
constexpr int c_SDMTestWindowPerRate = 32;
// Level given to a signal with no power at all, i.e. the difference of identical streams.
constexpr double c_SDMTestSilenceDB = -300.0;
// Default limit on the level of the worst window of the audio band difference. Measured on
// the CPU and CUDA block trellises given a 200Hz tone at -6dB:
//   - same modulator, parted by rounding    -100dB at DSD64, -114dB at DSD128, -150dB from DSD256
//   - decision latency one sample out        -49dB at DSD64 falling to -82dB at DSD1024
//   - one stream stuck or all zero           above 0dB
constexpr double c_SDMTestMaxDifferenceDB = -90.0;

//-------------------------------------------------------------------------------------------

// All levels are RMS in dB relative to DSD full scale (a stream of constant 1s is 0dB).
struct SDMStreamComparison
{
	int noSamples;              // Number of bits compared. 0 if there was nothing to compare.
	int noDifferences;          // Number of bit positions at which the streams differ.
	int firstDifference;        // Position of the first bit that differs, -1 if none.
	double signalDB;            // Audio band level of stream A.
	double differenceDB;        // Audio band level of (A - B) over the whole stream.
	double worstDifferenceDB;   // Audio band level of (A - B) over the worst window.
	int worstWindow;            // Position of the first bit of the worst window, -1 if none.
	double snrDB;               // signalDB - differenceDB.

	bool isIdentical() const
	{
		return (noSamples > 0 && noDifferences == 0);
	}
};

//-------------------------------------------------------------------------------------------

// Measure stream B against stream A. Both are given as one bit per byte (zero / non-zero),
// as used by SDMTrellis::processBits and the CUDA block output.
// The cutoff of the low pass is the audio band by default. It has to be lowered for a
// modulator that does not keep its quantisation noise out of all of the audio band, as below
// DSD64, where streams that are the same audio differ in the upper part of the band.
SDMStreamComparison sdmTestCompareDSDStreams(const uint8_t *bitsA, const uint8_t *bitsB, int noSamples, int dsdRate,
	double cutoff = c_SDMTestAudioBandCutoff);

// For use as EXPECT_TRUE(sdmTestDSDStreamsEquivalent(...)). Succeeds if the streams are bit
// identical, or their audio band difference is at most maxDifferenceDB in every window. The
// measurements are also written to pResult when given.
::testing::AssertionResult sdmTestDSDStreamsEquivalent(const uint8_t *expect, const uint8_t *actual, int noSamples, int dsdRate,
	double maxDifferenceDB = c_SDMTestMaxDifferenceDB, SDMStreamComparison *pResult = nullptr,
	double cutoff = c_SDMTestAudioBandCutoff);

//-------------------------------------------------------------------------------------------
// Comparison of two upscaled PCM signals that are expected to be the same signal.
//
// Unlike two DSD streams, two upscales of the same PCM through the same filter bank are the
// same samples up to the rounding of the arithmetic (CPU against CUDA FFT, double against
// single precision). The measure of sameness is the signal to noise ratio, taking the
// expected signal as the signal and the difference between the two as the noise:
//
//     SNR = 10.log10( sum(expect(n)^2) / sum((expect(n) - actual(n))^2) )
//
// Measured on PCMUpscale given a 1kHz tone at -6dB for 0.1s, DSD64 to DSD1024 (Release):
//   - CPU against CPU filter bank                 identical, in double and single precision
//   - CUDA against CPU filter bank (double)       297dB at DSD64 falling to 293dB at DSD1024
//   - CUDA against CPU (single precision)         261dB to 279dB. Few samples differ at all,
//                                                 only those that round to a different float.
// For comparison, by calculation:
//   - a difference of one unit of single precision in every sample is about 150dB
//   - a 1kHz tone one output sample out is 53dB at DSD64 rising to 77dB at DSD1024
//-------------------------------------------------------------------------------------------

// SNR given to two signals with no difference between them at all.
constexpr double c_SDMTestIdenticalSNRDB = 300.0;
// Default lower limits on the SNR for signals held in double and single precision.
constexpr double c_SDMTestMinPCMSNRDoubleDB = 240.0;
constexpr double c_SDMTestMinPCMSNRFloatDB = 130.0;

// All levels are RMS in dB relative to PCM full scale (a constant 1.0 is 0dB).
struct PCMSignalComparison
{
	int noSamples;              // Number of samples compared. 0 if there was nothing to compare.
	int noDifferences;          // Number of samples that are not exactly equal.
	int noInvalid;              // Number of samples where either signal is NaN or infinite.
	double signalDB;            // Level of the expected signal.
	double noiseDB;             // Level of (expect - actual).
	double snrDB;               // signalDB - noiseDB, c_SDMTestIdenticalSNRDB at most.
	double maxDifference;       // Largest absolute difference between two samples.
	int maxDifferenceIndex;     // Position of that sample, -1 if none.

	bool isIdentical() const
	{
		return (noSamples > 0 && noDifferences == 0 && noInvalid == 0);
	}
};

//-------------------------------------------------------------------------------------------

// Measure the actual upscaled signal against the expected one.
PCMSignalComparison sdmTestComparePCMSignals(const double *expect, const double *actual, int noSamples);
PCMSignalComparison sdmTestComparePCMSignals(const float *expect, const float *actual, int noSamples);

// For use as EXPECT_TRUE(sdmTestPCMSignalsEquivalent(...)). Succeeds if the signals are
// identical or their SNR is at least minSNRDB. Fails if there is no signal in the expected
// input or any sample is not finite. The measurements are also written to pResult when given.
::testing::AssertionResult sdmTestPCMSignalsEquivalent(const double *expect, const double *actual, int noSamples,
	double minSNRDB = c_SDMTestMinPCMSNRDoubleDB, PCMSignalComparison *pResult = nullptr);
::testing::AssertionResult sdmTestPCMSignalsEquivalent(const float *expect, const float *actual, int noSamples,
	double minSNRDB = c_SDMTestMinPCMSNRFloatDB, PCMSignalComparison *pResult = nullptr);

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
