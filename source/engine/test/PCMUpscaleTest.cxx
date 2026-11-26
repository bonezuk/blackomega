#include "gtest/gtest.h"

#include <cmath>
#include <cstring>
#include <limits>
#include <vector>

#include "engine/inc/PCMUpscale.h"
#include "engine/inc/FIRConvolutionAddOverlap.h"
#include "engine/test/SDMTestUtils.h"

#if defined(OMEGA_CUDA)
#include "engine/inc/OmegaCUDACommon.h"
#endif

using namespace omega;

//-------------------------------------------------------------------------------------------

namespace
{

const int c_upscaleTestFrequency = 44100;
// Number of samples PCMUpscale takes per block at 44.1kHz.
const int c_upscaleTestInputBlockSize = 2048;
const double c_upscaleTestToneFrequency = 1000.0;
const double c_upscaleTestToneAmplitude = 0.5;
const double c_upscaleTestToneDuration = 0.1;

//-------------------------------------------------------------------------------------------
// Number of octave upscaling filters taking 44.1kHz up to the given DSD rate.

int upscaleTestNoFilters(int dsdRate)
{
    int noFilters = 0;
    while((1 << noFilters) < dsdRate)
    {
        noFilters++;
    }
    return noFilters;
}

//-------------------------------------------------------------------------------------------
// A tone of the given duration, followed by silence up to a whole number of input blocks.
//
// Each filter of the bank is linear phase with as many taps as its output block, so delays
// the signal by half an output block: c_upscaleTestInputBlockSize / 2 input samples for every
// filter. Enough silence follows the tone for all of it to come out of the filter bank,
// otherwise (0.1s being about the latency of DSD64) there is next to no signal to compare.

std::vector<tfloat64> upscaleTestTone(int dsdRate, int& noBlocks)
{
    const double c_pi = 3.14159265358979323846;
    const int noToneSamples = static_cast<int>(c_upscaleTestToneDuration * static_cast<double>(c_upscaleTestFrequency));
    const int latency = upscaleTestNoFilters(dsdRate) * (c_upscaleTestInputBlockSize / 2);

    noBlocks = (noToneSamples + latency + c_upscaleTestInputBlockSize - 1) / c_upscaleTestInputBlockSize;
    std::vector<tfloat64> tone(static_cast<size_t>(noBlocks) * c_upscaleTestInputBlockSize, 0.0);
    for(int i = 0; i < noToneSamples; i++)
    {
        tone[i] = c_upscaleTestToneAmplitude * sin(2.0 * c_pi * c_upscaleTestToneFrequency * static_cast<double>(i) / static_cast<double>(c_upscaleTestFrequency));
    }
    return tone;
}

//-------------------------------------------------------------------------------------------
// The expected upscale: the input passed through the CPU bank of octave upscaling low pass
// filters, one filter per doubling of the sample rate, as in testDSD16FromWAVCodec.

void upscaleTestExpected(const std::vector<tfloat64>& in, int noBlocks, int dsdRate, std::vector<tfloat64>& expect)
{
    std::vector<engine::FIRFilterType> filterTypes;
    filterTypes.push_back(engine::e_lpHalf_DSD2);
    filterTypes.push_back(engine::e_lpQuarter_DSD4);
    filterTypes.push_back(engine::e_lpQuarter_DSD8);
    filterTypes.push_back(engine::e_lpQuarter_DSD16);
    filterTypes.push_back(engine::e_lpQuarter_DSD32);
    filterTypes.push_back(engine::e_lpQuarter_DSD64);
    filterTypes.push_back(engine::e_lpQuarter_DSD128);
    filterTypes.push_back(engine::e_lpQuarter_DSD256);
    filterTypes.push_back(engine::e_lpQuarter_DSD512);
    filterTypes.push_back(engine::e_lpQuarter_DSD1024);

    const int noFilters = upscaleTestNoFilters(dsdRate);
    ASSERT_EQ(1 << noFilters, dsdRate);
    ASSERT_LE(noFilters, static_cast<int>(filterTypes.size()));

    std::vector<engine::FIRConvolutionAddOverlapOctaveUpscale> filters(noFilters);
    std::vector<std::vector<tfloat64> > pcm(noFilters);
    for(int idx = 0; idx < noFilters; idx++)
    {
        int lpSize = 0;
        int blockLen = c_upscaleTestInputBlockSize << (idx + 1);
        tfloat64 *lpCoeff = engine::getFIRFilterFromDB(filterTypes[idx], lpSize);
        ASSERT_TRUE(lpCoeff != NULL);
        EXPECT_EQ(lpSize, blockLen + 1);
        bool res = filters[idx].init(lpCoeff, lpSize, blockLen);
        delete [] lpCoeff;
        ASSERT_TRUE(res);
        pcm[idx].resize(blockLen);
    }

    const int outBlockSize = c_upscaleTestInputBlockSize * dsdRate;
    expect.resize(static_cast<size_t>(noBlocks) * outBlockSize);
    for(int blockIdx = 0; blockIdx < noBlocks; blockIdx++)
    {
        const tfloat64 *x = &in[static_cast<size_t>(blockIdx) * c_upscaleTestInputBlockSize];
        for(int idx = 0; idx < noFilters; idx++)
        {
            filters[idx].process(x, pcm[idx].data());
            x = pcm[idx].data();
        }
        memcpy(&expect[static_cast<size_t>(blockIdx) * outBlockSize], x, sizeof(tfloat64) * outBlockSize);
    }
}

//-------------------------------------------------------------------------------------------

void testPCMUpscaleTone(engine::PCMUpscale::ComputeMethod computeType, int dsdRate)
{
    const int outBlockSize = c_upscaleTestInputBlockSize * dsdRate;

    // 1. The tone to upscale.
    int noBlocks = 0;
    std::vector<tfloat64> tone = upscaleTestTone(dsdRate, noBlocks);
    const int noInput = noBlocks * c_upscaleTestInputBlockSize;
    const int noExpect = noBlocks * outBlockSize;

    double tonePower = 0.0;
    for(int i = 0; i < noInput; i++)
    {
        tonePower += tone[i] * tone[i];
    }
    const double toneDB = 10.0 * log10(tonePower / static_cast<double>(noInput));

    // 2. The expected upscaled signal.
    std::vector<tfloat64> expect;
    upscaleTestExpected(tone, noBlocks, dsdRate, expect);
    if(::testing::Test::HasFatalFailure())
    {
        return;
    }
    ASSERT_EQ(static_cast<int>(expect.size()), noExpect);

    std::vector<float> expectS(noExpect);
    for(int i = 0; i < noExpect; i++)
    {
        expectS[i] = static_cast<float>(expect[i]);
    }

    // 3 & 4. Upscale in double precision into host memory and compare.
    {
        engine::PCMUpscale upscale(computeType);
        ASSERT_EQ(upscale.computeMethod(), computeType);
        ASSERT_TRUE(upscale.init(c_upscaleTestFrequency, dsdRate));
        ASSERT_EQ(upscale.noInputSamples(), c_upscaleTestInputBlockSize);
        ASSERT_EQ(upscale.noOutputSamples(), outBlockSize);
        ASSERT_EQ(upscale.outputFrequency(), c_upscaleTestFrequency * dsdRate);

        int noOutput = 0;
        QSharedPointer<double> pOut = upscale.upscale(tone.data(), noInput, noOutput);
        ASSERT_FALSE(pOut.isNull());
        ASSERT_EQ(noOutput, noExpect);

        engine::PCMSignalComparison r;
        EXPECT_TRUE(engine::sdmTestPCMSignalsEquivalent(expect.data(), pOut.get(), noExpect, engine::c_SDMTestMinPCMSNRDoubleDB, &r));
        // All of the tone is in the expected signal: its power over the upscaled length is
        // that of the tone over the input length.
        EXPECT_NEAR(r.signalDB, toneDB, 0.1);
        if(computeType == engine::PCMToDSD::e_computeMethodCPU)
        {
            // The CPU filter bank is the same arithmetic as the expected signal.
            EXPECT_TRUE(r.isIdentical());
        }
        printf("DSD%d double: SNR %.1fdB, signal %.1fdB, noise %.1fdB, max difference %.3g\n", dsdRate, r.snrDB, r.signalDB, r.noiseDB, r.maxDifference);
    }

    // 3 & 4. Upscale block by block in single precision, in memory local to the compute
    // method, and compare.
    {
        engine::PCMUpscale upscale(computeType);
        ASSERT_TRUE(upscale.init(c_upscaleTestFrequency, dsdRate));

        // The double precision signals are no longer needed.
        std::vector<tfloat64>().swap(expect);

        std::vector<float> outS(noExpect);
        for(int blockIdx = 0; blockIdx < noBlocks; blockIdx++)
        {
            int noOutput = 0;
            const float *up = upscale.upscaleSingleOnDevice(&tone[static_cast<size_t>(blockIdx) * c_upscaleTestInputBlockSize], c_upscaleTestInputBlockSize, noOutput);
            ASSERT_TRUE(up != NULL);
            ASSERT_EQ(noOutput, outBlockSize);

            float *out = &outS[static_cast<size_t>(blockIdx) * outBlockSize];
#if defined(OMEGA_CUDA)
            if(computeType == engine::PCMToDSD::e_computeMethodCUDA)
            {
                // The upscaled block is in GPU device memory.
                ASSERT_EQ(isPointerForCUDADevice(up), 1);
                ASSERT_TRUE(copyCUDAMemory(out, up, static_cast<int>(sizeof(float)) * outBlockSize));
            }
            else
#endif
            {
                // The upscaled block is in host memory and can be read directly.
                memcpy(out, up, sizeof(float) * outBlockSize);
            }
        }

        engine::PCMSignalComparison r;
        EXPECT_TRUE(engine::sdmTestPCMSignalsEquivalent(expectS.data(), outS.data(), noExpect, engine::c_SDMTestMinPCMSNRFloatDB, &r));
        if(computeType == engine::PCMToDSD::e_computeMethodCPU)
        {
            EXPECT_TRUE(r.isIdentical());
        }
        printf("DSD%d float : SNR %.1fdB, signal %.1fdB, noise %.1fdB, max difference %.3g\n", dsdRate, r.snrDB, r.signalDB, r.noiseDB, r.maxDifference);
    }
}

//-------------------------------------------------------------------------------------------

void testPCMUpscaleToneCPU(int dsdRate)
{
    ASSERT_TRUE(engine::PCMUpscale::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testPCMUpscaleTone(engine::PCMToDSD::e_computeMethodCPU, dsdRate);
}

//-------------------------------------------------------------------------------------------

void testPCMUpscaleToneCUDA(int dsdRate)
{
#if defined(OMEGA_CUDA)
    if(initCUDAOmega() >= 0)
    {
        ASSERT_TRUE(engine::PCMUpscale::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
        testPCMUpscaleTone(engine::PCMToDSD::e_computeMethodCUDA, dsdRate);
    }
    else
    {
        ASSERT_FALSE(engine::PCMUpscale::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
    }
#else
    ASSERT_FALSE(engine::PCMUpscale::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
#endif
}

} // namespace

//-------------------------------------------------------------------------------------------

TEST(PCMUpscale, DSD64_CPU)
{
    testPCMUpscaleToneCPU(64);
}

TEST(PCMUpscale, DSD128_CPU)
{
    testPCMUpscaleToneCPU(128);
}

TEST(PCMUpscale, DSD256_CPU)
{
    testPCMUpscaleToneCPU(256);
}

TEST(PCMUpscale, DSD512_CPU)
{
    testPCMUpscaleToneCPU(512);
}

TEST(PCMUpscale, DSD1024_CPU)
{
    testPCMUpscaleToneCPU(1024);
}

//-------------------------------------------------------------------------------------------

TEST(PCMUpscale, DSD64_CUDA)
{
    testPCMUpscaleToneCUDA(64);
}

TEST(PCMUpscale, DSD128_CUDA)
{
    testPCMUpscaleToneCUDA(128);
}

TEST(PCMUpscale, DSD256_CUDA)
{
    testPCMUpscaleToneCUDA(256);
}

TEST(PCMUpscale, DSD512_CUDA)
{
    testPCMUpscaleToneCUDA(512);
}

TEST(PCMUpscale, DSD1024_CUDA)
{
    testPCMUpscaleToneCUDA(1024);
}

//-------------------------------------------------------------------------------------------

TEST(PCMUpscale, unavailableComputeMethodFallsBackToCPU)
{
    if(!engine::PCMUpscale::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA))
    {
        engine::PCMUpscale upscale(engine::PCMToDSD::e_computeMethodCUDA);
        EXPECT_EQ(upscale.computeMethod(), engine::PCMToDSD::e_computeMethodCPU);
        engine::PCMUpscale upscaleDefault;
        EXPECT_EQ(upscaleDefault.computeMethod(), engine::PCMToDSD::e_computeMethodCPU);
    }
    else
    {
        engine::PCMUpscale upscaleDefault;
        EXPECT_EQ(upscaleDefault.computeMethod(), engine::PCMToDSD::e_computeMethodCUDA);
    }
}

//-------------------------------------------------------------------------------------------
// The comparison of upscaled PCM signals
//-------------------------------------------------------------------------------------------

template <typename T> void testComparePCMSignals(double minSNRDB, double noiseAmplitude, double expectNoiseSNRDB)
{
    const double c_pi = 3.14159265358979323846;
    const int c_noSamples = 44100;

    std::vector<T> a(c_noSamples), noisy(c_noSamples), shifted(c_noSamples), silent(c_noSamples, static_cast<T>(0));
    for(int i = 0; i < c_noSamples; i++)
    {
        double s = 0.5 * sin(2.0 * c_pi * 1000.0 * static_cast<double>(i) / 44100.0);
        // Alternating error of fixed size: noise power is noiseAmplitude^2.
        double n = (i & 1) ? noiseAmplitude : -noiseAmplitude;
        a[i] = static_cast<T>(s);
        noisy[i] = static_cast<T>(s + n);
        shifted[i] = static_cast<T>(0.5 * sin(2.0 * c_pi * 1000.0 * static_cast<double>(i + 1) / 44100.0));
    }

    engine::PCMSignalComparison r;

    // Identical signals.
    EXPECT_TRUE(engine::sdmTestPCMSignalsEquivalent(a.data(), a.data(), c_noSamples, minSNRDB, &r));
    EXPECT_TRUE(r.isIdentical());
    EXPECT_EQ(r.noDifferences, 0);
    EXPECT_EQ(r.maxDifferenceIndex, -1);
    EXPECT_DOUBLE_EQ(r.snrDB, engine::c_SDMTestIdenticalSNRDB);
    // A sine of amplitude 0.5 has an RMS level of -9.03dB.
    EXPECT_NEAR(r.signalDB, -9.03, 0.01);

    // Known noise: SNR = 10.log10(0.125 / noiseAmplitude^2).
    r = engine::sdmTestComparePCMSignals(a.data(), noisy.data(), c_noSamples);
    EXPECT_FALSE(r.isIdentical());
    EXPECT_NEAR(r.snrDB, expectNoiseSNRDB, 0.5);
    EXPECT_NEAR(r.maxDifference, noiseAmplitude, noiseAmplitude * 0.1);
    EXPECT_TRUE(engine::sdmTestPCMSignalsEquivalent(a.data(), noisy.data(), c_noSamples, expectNoiseSNRDB - 1.0));
    EXPECT_FALSE(engine::sdmTestPCMSignalsEquivalent(a.data(), noisy.data(), c_noSamples, expectNoiseSNRDB + 1.0));

    // One sample out: 2.sin(pi.f/fs) of the signal is left as the difference, about 16.9dB down.
    EXPECT_FALSE(engine::sdmTestPCMSignalsEquivalent(a.data(), shifted.data(), c_noSamples, minSNRDB, &r));
    EXPECT_NEAR(r.snrDB, 16.9, 0.5);

    // No output at all.
    EXPECT_FALSE(engine::sdmTestPCMSignalsEquivalent(a.data(), silent.data(), c_noSamples, minSNRDB, &r));
    EXPECT_NEAR(r.snrDB, 0.0, 0.01);

    // Nothing to measure against.
    EXPECT_FALSE(engine::sdmTestPCMSignalsEquivalent(silent.data(), silent.data(), c_noSamples, minSNRDB));
    EXPECT_FALSE(engine::sdmTestPCMSignalsEquivalent(a.data(), a.data(), 0, minSNRDB));
    EXPECT_FALSE(engine::sdmTestPCMSignalsEquivalent(static_cast<const T *>(nullptr), a.data(), c_noSamples, minSNRDB));

    // Samples that are not numbers.
    noisy = a;
    noisy[100] = std::numeric_limits<T>::quiet_NaN();
    EXPECT_FALSE(engine::sdmTestPCMSignalsEquivalent(a.data(), noisy.data(), c_noSamples, minSNRDB, &r));
    EXPECT_EQ(r.noInvalid, 1);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTestUtils, comparePCMSignalsDouble)
{
    // 10.log10(0.125 / 1e-20) = 191dB
    testComparePCMSignals<double>(engine::c_SDMTestMinPCMSNRDoubleDB, 1.0e-10, 190.97);
}

//-------------------------------------------------------------------------------------------

TEST(SDMTestUtils, comparePCMSignalsFloat)
{
    // 10.log10(0.125 / 1e-8) = 71dB
    testComparePCMSignals<float>(engine::c_SDMTestMinPCMSNRFloatDB, 1.0e-4, 70.97);
}

//-------------------------------------------------------------------------------------------
