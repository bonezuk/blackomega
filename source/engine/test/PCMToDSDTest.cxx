#include "gtest/gtest.h"

#include <cstring>
#include <functional>
#include <vector>

#include "engine/inc/PCMToDSD.h"
#include "engine/inc/PCMUpscale.h"
#include "engine/inc/Codec.h"
#include "engine/test/SDMTestUtils.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------
// The PCMToDSD tests convert a 2 second stereo recording to DSD16 and compare each channel
// against an expected DSD16 stream. The expected stream is made independently of PCMToDSD:
// the input is upscaled with PCMUpscale on the CPU and then modulated with a
// DeltaSigmaModulator.
//
// The streams are not compared bit for bit. The rounding of the CUDA filter bank differs from
// that of the CPU, and a single modulator decision made the other way parts the two streams
// for good, although both are still the same audio. They are compared with the DSD stream
// comparison of SDMTestUtils instead: the level of the difference between the two streams
// once low passed, over its worst window, must be below a limit.
//-------------------------------------------------------------------------------------------

namespace
{

const int c_dsd16Rate = 16;
const int c_dsd16InputBlockSize = 2048;
const int c_dsd16OutNoSamples = c_dsd16InputBlockSize * c_dsd16Rate;
const int c_dsd16OutNoBytes = c_dsd16OutNoSamples >> 3;
const int c_dsd16NoChannels = 2;

// Two DSD16 streams are compared below 2.5kHz and not over the whole audio band. The noise
// shaping of DeltaSigmaModulator is that of DSD64, so at 16 times 44.1kHz the band kept clear
// of quantisation noise is a quarter as wide: above about 5kHz the noise of a DSD16 stream is
// as loud as the recording, and two streams that are the same audio differ there by as much.
// Measured on the expected stream of the test recording, as the level of the worst window of
// the difference (see PCMToDSD.DSD16_ComparisonLimit):
//
//     cutoff     parted by rounding     one sample late
//     20kHz           -10dB                 -31dB
//     10kHz           -36dB                 -43dB
//      5kHz           -84dB                 -46dB
//      2.5kHz        -132dB                 -49dB
//      1.25kHz       -158dB                 -54dB
//
// At 2.5kHz the limit of -90dB is some 40dB from both.
const double c_dsd16ComparisonCutoff = 2500.0;
const double c_dsd16MaxDifferenceDB = engine::c_SDMTestMaxDifferenceDB;

//-------------------------------------------------------------------------------------------
// The test recording as blocks of c_dsd16InputBlockSize samples, the last padded with silence.

struct DSD16TestInput
{
    int frequency;
    // Channel interleaved samples of each block, as given to PCMToDSD::push.
    std::vector<std::vector<sample_t> > blocks;

    int noBlocks() const
    {
        return static_cast<int>(blocks.size());
    }

    // The samples of a single channel of a block, as given to PCMToDSD::process.
    std::vector<tfloat64> channel(int blockIdx, int chIdx) const
    {
        std::vector<tfloat64> x(c_dsd16InputBlockSize);
        for(int i = 0; i < c_dsd16InputBlockSize; i++)
        {
            x[i] = blocks[blockIdx][(i * c_dsd16NoChannels) + chIdx];
        }
        return x;
    }
};

void loadDSD16TestInput(DSD16TestInput& input)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString sourceFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_org.wav");
    ASSERT_TRUE(common::DiskOps::exist(sourceFileName));

    engine::Codec *codec = engine::Codec::get(sourceFileName);
    ASSERT_TRUE(codec != NULL);
    bool isOpen = codec->init();
    if(isOpen && codec->frequency() == 44100 && codec->noChannels() == c_dsd16NoChannels)
    {
        engine::RData data(c_dsd16InputBlockSize, c_dsd16NoChannels, c_dsd16NoChannels);
        bool loop = true;

        input.frequency = codec->frequency();
        do
        {
            loop = codec->next(data);
            if(data.noParts() > 0)
            {
                EXPECT_EQ(data.noParts(), 1);
                const sample_t *x = data.partData(0);
                int len = data.part(0).length();
                std::vector<sample_t> block(c_dsd16InputBlockSize * c_dsd16NoChannels, 0.0);
                memcpy(block.data(), x, len * c_dsd16NoChannels * sizeof(sample_t));
                input.blocks.push_back(block);
            }
            data.reset();
        } while(loop);
    }
    else
    {
        ADD_FAILURE() << "test recording is not 44.1kHz stereo";
    }
    codec->close();
    delete codec;
    ASSERT_GT(input.noBlocks(), 0);
}

//-------------------------------------------------------------------------------------------
// The expected DSD16 stream of a single channel.

class DSD16Expected
{
    public:
        DSD16Expected() : m_upscale(engine::PCMToDSD::e_computeMethodCPU), m_modulator()
        {}

        bool init(int frequency, bool isLSB)
        {
            m_modulator.init(isLSB);
            return m_upscale.init(frequency, c_dsd16Rate) &&
                m_upscale.noInputSamples() == c_dsd16InputBlockSize &&
                m_upscale.noOutputSamples() == c_dsd16OutNoSamples;
        }

        // A block of c_dsd16InputBlockSize samples to c_dsd16OutNoBytes bytes of DSD.
        bool process(const tfloat64 *in, uint8_t *dsd)
        {
            int noOutput = 0;
            QSharedPointer<double> pcm = m_upscale.upscale(in, c_dsd16InputBlockSize, noOutput);
            if(pcm.isNull() || noOutput != c_dsd16OutNoSamples)
            {
                return false;
            }
            m_modulator.process(pcm.get(), dsd, c_dsd16OutNoSamples);
            return true;
        }

    private:
        engine::PCMUpscale m_upscale;
        engine::DeltaSigmaModulator m_modulator;
};

//-------------------------------------------------------------------------------------------
// Append packed DSD bytes to a stream held as one bit per byte in time order, as compared
// by SDMTestUtils.

void appendDSDBits(std::vector<uint8_t>& bits, const uint8_t *dsd, int noBytes, bool isLSB)
{
    for(int i = 0; i < noBytes; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            uint8_t mask = (isLSB) ? static_cast<uint8_t>(0x01 << j) : static_cast<uint8_t>(0x80 >> j);
            bits.push_back((dsd[i] & mask) ? 1 : 0);
        }
    }
}

//-------------------------------------------------------------------------------------------

void expectDSD16StreamsEquivalent(const char *name, const std::vector<uint8_t>& expect, const std::vector<uint8_t>& actual)
{
    ASSERT_EQ(expect.size(), actual.size());
    engine::SDMStreamComparison r;
    EXPECT_TRUE(engine::sdmTestDSDStreamsEquivalent(expect.data(), actual.data(), static_cast<int>(expect.size()), c_dsd16Rate, c_dsd16MaxDifferenceDB, &r, c_dsd16ComparisonCutoff)) << name;
    // The recording is not silent.
    EXPECT_GT(r.signalDB, -60.0) << name;
    printf("%s: %d of %d bits differ, worst window %.1fdB, signal %.1fdB\n", name, r.noDifferences, r.noSamples, r.worstDifferenceDB, r.signalDB);
}

//-------------------------------------------------------------------------------------------

void checkDSD16Converter(const engine::PCMToDSD& convert, bool isLSB)
{
    ASSERT_EQ(convert.noInputSamples(), c_dsd16InputBlockSize);
    ASSERT_EQ(convert.noOutputSamples(), c_dsd16OutNoSamples);
    ASSERT_EQ(convert.noOutputBytes(), c_dsd16OutNoBytes);
    ASSERT_EQ(convert.inputFrequency(), 44100);
    ASSERT_EQ(convert.outputFrequency(), 44100 * c_dsd16Rate);
    ASSERT_EQ(convert.isLSB(), isLSB);
}

//-------------------------------------------------------------------------------------------
// Run a test for CUDA if there is a device, otherwise check that CUDA is reported unavailable.

void runIfCUDAAvailable(const std::function<void()>& test)
{
#if defined(OMEGA_CUDA)
    if(initCUDAOmega() >= 0)
    {
        ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
        test();
    }
    else
    {
        ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
    }
#else
    ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
#endif
}

} // namespace

//-------------------------------------------------------------------------------------------
// PCMToDSD::process : one block of a single channel at a time.
//-------------------------------------------------------------------------------------------

void testDSD16FromWAVCodec(engine::PCMToDSD::ComputeMethod computeType)
{
    DSD16TestInput input;
    loadDSD16TestInput(input);
    ASSERT_FALSE(::testing::Test::HasFatalFailure());

    std::vector<uint8_t> dsd(c_dsd16OutNoBytes);
    for(int chIdx = 0; chIdx < c_dsd16NoChannels; chIdx++)
    {
        DSD16Expected expected;
        ASSERT_TRUE(expected.init(input.frequency, false));

        engine::PCMToDSD convert(computeType);
        ASSERT_TRUE(convert.init(input.frequency, c_dsd16Rate, false));
        checkDSD16Converter(convert, false);
        ASSERT_FALSE(::testing::Test::HasFatalFailure());

        std::vector<uint8_t> expectBits, outBits;
        for(int blockIdx = 0; blockIdx < input.noBlocks(); blockIdx++)
        {
            std::vector<tfloat64> in = input.channel(blockIdx, chIdx);

            ASSERT_TRUE(expected.process(in.data(), dsd.data()));
            appendDSDBits(expectBits, dsd.data(), c_dsd16OutNoBytes, false);

            ASSERT_TRUE(convert.process(in.data(), dsd.data()));
            appendDSDBits(outBits, dsd.data(), c_dsd16OutNoBytes, false);
        }
        expectDSD16StreamsEquivalent((chIdx == 0) ? "process L" : "process R", expectBits, outBits);
    }
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CPU)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodec(engine::PCMToDSD::e_computeMethodCPU);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA)
{
    runIfCUDAAvailable([]() { testDSD16FromWAVCodec(engine::PCMToDSD::e_computeMethodCUDA); });
}

//-------------------------------------------------------------------------------------------
// PCMToDSD::push and pull : the multithreaded interface. Channel interleaved blocks are
// pushed, with the DSD bytes of the channel of the converter pulled.
//-------------------------------------------------------------------------------------------

void testDSD16FromWAVCodecMTPushPull(engine::PCMToDSD::ComputeMethod computeType)
{
    DSD16TestInput input;
    loadDSD16TestInput(input);
    ASSERT_FALSE(::testing::Test::HasFatalFailure());

    std::vector<uint8_t> dsd(c_dsd16OutNoBytes);
    for(int chIdx = 0; chIdx < c_dsd16NoChannels; chIdx++)
    {
        DSD16Expected expected;
        ASSERT_TRUE(expected.init(input.frequency, false));

        engine::PCMToDSD convert(computeType);
        ASSERT_TRUE(convert.initInterleaved(input.frequency, c_dsd16Rate, engine::e_SampleDSD8MSB, chIdx, c_dsd16NoChannels));
        checkDSD16Converter(convert, false);
        ASSERT_FALSE(::testing::Test::HasFatalFailure());

        std::vector<uint8_t> expectBits, outBits;
        for(int blockIdx = 0; blockIdx < input.noBlocks(); blockIdx++)
        {
            std::vector<tfloat64> in = input.channel(blockIdx, chIdx);

            ASSERT_TRUE(expected.process(in.data(), dsd.data()));
            appendDSDBits(expectBits, dsd.data(), c_dsd16OutNoBytes, false);

            ASSERT_EQ(convert.available(), 0);
            ASSERT_EQ(convert.push(input.blocks[blockIdx].data(), c_dsd16InputBlockSize), c_dsd16InputBlockSize);
            ASSERT_EQ(convert.available(), c_dsd16OutNoBytes);
            ASSERT_EQ(convert.pull(dsd.data(), c_dsd16OutNoBytes), c_dsd16OutNoBytes);
            appendDSDBits(outBits, dsd.data(), c_dsd16OutNoBytes, false);
        }
        expectDSD16StreamsEquivalent((chIdx == 0) ? "push/pull L" : "push/pull R", expectBits, outBits);
    }
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CPU_PushPull)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodecMTPushPull(engine::PCMToDSD::e_computeMethodCPU);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA_PushPull)
{
    runIfCUDAAvailable([]() { testDSD16FromWAVCodecMTPushPull(engine::PCMToDSD::e_computeMethodCUDA); });
}

//-------------------------------------------------------------------------------------------
// PCMToDSD::push and pullInterleaved : a converter per channel, each writing the DSD of its
// channel into the same channel interleaved output.
//-------------------------------------------------------------------------------------------
// Take the DSD bytes of a channel back out of a channel interleaved block, in the packing of
// the given data type (see PCMToDSD::pullInterleaved). For DSD over PCM everything in each
// 32-bit word other than the two DSD bytes (the marker and the padding) is checked here, with
// the number of words that are wrong returned.

int deinterleaveDSD16Block(engine::CodecDataType type, const sample_t *out, int chIdx, uint8_t *dsd)
{
    int noInvalid = 0;

    if(type == engine::e_SampleDSD8LSB || type == engine::e_SampleDSD8MSB)
    {
        // A sample_t of DSD bytes for each channel in turn.
        const uint8_t *x = reinterpret_cast<const uint8_t *>(out);
        const int noPerSample = static_cast<int>(sizeof(sample_t));
        for(int idx = 0; idx < c_dsd16OutNoBytes; idx += noPerSample)
        {
            memcpy(&dsd[idx], &x[((idx / noPerSample) * c_dsd16NoChannels + chIdx) * noPerSample], noPerSample);
        }
    }
    else
    {
        // Two DSD bytes in the low 16 bits of a 24-bit sample, below an alternating marker.
        const uint32_t *x = reinterpret_cast<const uint32_t *>(out);
        for(int idx = 0; idx < c_dsd16OutNoBytes; idx += 2)
        {
            uint32_t s = x[((idx >> 1) * c_dsd16NoChannels) + chIdx];
            uint32_t v = (type == engine::e_SampleInt32) ? (s >> 8) : s;
            dsd[idx] = static_cast<uint8_t>((v >> 8) & 0x000000ff);
            dsd[idx + 1] = static_cast<uint8_t>(v & 0x000000ff);

            uint32_t e = (idx & 0x02) ? 0xfffa0000 : 0x00050000;
            e |= v & 0x0000ffff;
            if(type == engine::e_SampleInt32)
            {
                e <<= 8;
            }
            if(s != e)
            {
                noInvalid++;
            }
        }
    }
    return noInvalid;
}

//-------------------------------------------------------------------------------------------

void testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::ComputeMethod computeType, engine::CodecDataType dataType)
{
    DSD16TestInput input;
    loadDSD16TestInput(input);
    ASSERT_FALSE(::testing::Test::HasFatalFailure());

    const bool isLSB = (dataType == engine::e_SampleDSD8LSB);
    const bool isNative = (dataType == engine::e_SampleDSD8LSB || dataType == engine::e_SampleDSD8MSB);
    // Number of DSD bytes held by each sample_t (native) or 32-bit sample (DSD over PCM).
    const int dsdBytesPerPCMSample = (isNative) ? static_cast<int>(sizeof(sample_t)) : 2;
    const int noPCMSamplesPerChannel = c_dsd16OutNoBytes / dsdBytesPerPCMSample;

    DSD16Expected expected[c_dsd16NoChannels];
    engine::PCMToDSD convertL(computeType), convertR(computeType);
    engine::PCMToDSD *convert[c_dsd16NoChannels] = { &convertL, &convertR };
    for(int chIdx = 0; chIdx < c_dsd16NoChannels; chIdx++)
    {
        ASSERT_TRUE(expected[chIdx].init(input.frequency, isLSB));
        ASSERT_TRUE(convert[chIdx]->initInterleaved(input.frequency, c_dsd16Rate, dataType, chIdx, c_dsd16NoChannels));
        checkDSD16Converter(*convert[chIdx], isLSB);
        ASSERT_FALSE(::testing::Test::HasFatalFailure());
        ASSERT_EQ(convert[chIdx]->dataType(), dataType);
    }

    int noInvalidWords = 0;
    std::vector<uint8_t> dsd(c_dsd16OutNoBytes);
    std::vector<sample_t> out(c_dsd16NoChannels * noPCMSamplesPerChannel);
    std::vector<uint8_t> expectBits[c_dsd16NoChannels], outBits[c_dsd16NoChannels];

    for(int blockIdx = 0; blockIdx < input.noBlocks(); blockIdx++)
    {
        // Both converters write their own channel into the one interleaved output.
        memset(out.data(), 0, out.size() * sizeof(sample_t));
        for(int chIdx = 0; chIdx < c_dsd16NoChannels; chIdx++)
        {
            ASSERT_EQ(convert[chIdx]->available(), 0);
            ASSERT_EQ(convert[chIdx]->push(input.blocks[blockIdx].data(), c_dsd16InputBlockSize), c_dsd16InputBlockSize);
            ASSERT_EQ(convert[chIdx]->available(), c_dsd16OutNoBytes);
            ASSERT_EQ(convert[chIdx]->availablePCMSamples(), noPCMSamplesPerChannel);
            ASSERT_EQ(convert[chIdx]->pullInterleaved(out.data(), noPCMSamplesPerChannel), noPCMSamplesPerChannel);
        }

        for(int chIdx = 0; chIdx < c_dsd16NoChannels; chIdx++)
        {
            std::vector<tfloat64> in = input.channel(blockIdx, chIdx);
            ASSERT_TRUE(expected[chIdx].process(in.data(), dsd.data()));
            appendDSDBits(expectBits[chIdx], dsd.data(), c_dsd16OutNoBytes, isLSB);

            noInvalidWords += deinterleaveDSD16Block(dataType, out.data(), chIdx, dsd.data());
            appendDSDBits(outBits[chIdx], dsd.data(), c_dsd16OutNoBytes, isLSB);
        }
    }

    // The DSD over PCM markers are not subject to rounding and must be exact.
    EXPECT_EQ(noInvalidWords, 0);
    expectDSD16StreamsEquivalent("interleaved L", expectBits[0], outBits[0]);
    expectDSD16StreamsEquivalent("interleaved R", expectBits[1], outBits[1]);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CPU_PushPullInterleaved_NativeLSB)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCPU, engine::e_SampleDSD8LSB);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA_PushPullInterleaved_NativeLSB)
{
    runIfCUDAAvailable([]() { testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCUDA, engine::e_SampleDSD8LSB); });
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CPU_PushPullInterleaved_NativeMSB)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCPU, engine::e_SampleDSD8MSB);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA_PushPullInterleaved_NativeMSB)
{
    runIfCUDAAvailable([]() { testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCUDA, engine::e_SampleDSD8MSB); });
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CPU_PushPullInterleaved_DSDOverPCM_24)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCPU, engine::e_SampleInt24);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA_PushPullInterleaved_DSDOverPCM_24)
{
    runIfCUDAAvailable([]() { testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCUDA, engine::e_SampleInt24); });
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CPU_PushPullInterleaved_DSDOverPCM_32)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCPU, engine::e_SampleInt32);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA_PushPullInterleaved_DSDOverPCM_32)
{
    runIfCUDAAvailable([]() { testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCUDA, engine::e_SampleInt32); });
}

//-------------------------------------------------------------------------------------------
// The limit used to compare DSD16 streams must accept two streams that differ only through
// rounding, and reject a stream that is not the same audio.
//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_ComparisonLimit)
{
    DSD16TestInput input;
    loadDSD16TestInput(input);
    ASSERT_FALSE(::testing::Test::HasFatalFailure());

    DSD16Expected expected, rounded;
    ASSERT_TRUE(expected.init(input.frequency, false));
    ASSERT_TRUE(rounded.init(input.frequency, false));

    std::vector<uint8_t> dsd(c_dsd16OutNoBytes);
    std::vector<uint8_t> expectBits, roundedBits;
    for(int blockIdx = 0; blockIdx < input.noBlocks(); blockIdx++)
    {
        std::vector<tfloat64> in = input.channel(blockIdx, 0);
        ASSERT_TRUE(expected.process(in.data(), dsd.data()));
        appendDSDBits(expectBits, dsd.data(), c_dsd16OutNoBytes, false);

        // A difference in the input of the size of the rounding between a CPU and a CUDA
        // filter bank. The modulators soon make a different decision and part for good.
        for(size_t i = 0; i < in.size(); i++)
        {
            in[i] += (i & 1) ? 1.0e-13 : -1.0e-13;
        }
        ASSERT_TRUE(rounded.process(in.data(), dsd.data()));
        appendDSDBits(roundedBits, dsd.data(), c_dsd16OutNoBytes, false);
    }
    const int noSamples = static_cast<int>(expectBits.size());

    engine::SDMStreamComparison r;
    EXPECT_TRUE(engine::sdmTestDSDStreamsEquivalent(expectBits.data(), roundedBits.data(), noSamples, c_dsd16Rate, c_dsd16MaxDifferenceDB, &r, c_dsd16ComparisonCutoff));
    // The streams did part, otherwise nothing has been measured.
    EXPECT_GT(r.noDifferences, noSamples / 4);
    printf("rounding: %d of %d bits differ, the first at %d, worst window %.1fdB, whole stream %.1fdB\n", r.noDifferences, r.noSamples, r.firstDifference, r.worstDifferenceDB, r.differenceDB);

    // The same stream one sample late.
    EXPECT_FALSE(engine::sdmTestDSDStreamsEquivalent(&expectBits[1], expectBits.data(), noSamples - 1, c_dsd16Rate, c_dsd16MaxDifferenceDB, &r, c_dsd16ComparisonCutoff));
    printf("one sample late: %d of %d bits differ, worst window %.1fdB, whole stream %.1fdB\n", r.noDifferences, r.noSamples, r.worstDifferenceDB, r.differenceDB);

    // No stream at all.
    std::vector<uint8_t> silent(expectBits.size(), 0);
    EXPECT_FALSE(engine::sdmTestDSDStreamsEquivalent(expectBits.data(), silent.data(), noSamples, c_dsd16Rate, c_dsd16MaxDifferenceDB, &r, c_dsd16ComparisonCutoff));
}

//-------------------------------------------------------------------------------------------
