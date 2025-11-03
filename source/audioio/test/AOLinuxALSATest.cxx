//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AOLinuxALSA.h"
#include "audioio/test/AOQueryALSAMock.h"
#include "audioio/test/LinuxALSAMockIF.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

tint isSampleEquivALSA(tuint32 a,tuint32 b,tint N)
{
    tint r = 0;

    if(a==b || a==b-1 || a==b+1|| a==b-2 || a==b+2)
    {
        r = 1;
    }
    else if(N==3 && (((a==0x00800000 || a==0) && b==0x00ffffff) || ((b==0x00800000 || b==0) && a==0x00ffffff)))
    {
        r = 1;
    }
    return r;
}

//-------------------------------------------------------------------------------------------

void testSampleOutputConvertionALSA(const tubyte *expect,const tubyte *out,tint noBits,tint bytesPerSample,tint noSamples)
{
#if defined(SINGLE_FLOAT_SAMPLE)
    if(bytesPerSample>=3)
    {
        const tint N = 3;
        const tubyte *x,*y;

        x = expect;
        y = out;
        for(int i=0;i<noSamples;i++)
        {
            tuint32 a,b,r = 0;

            a = 0;
            b = 0;
            for(int j=0;j<N;j++)
            {
                a |= static_cast<tuint32>(x[j]) << (j * 8);
                b |= static_cast<tuint32>(y[j]) << (j * 8);
            }
            r |= isSampleEquivALSA(a,b,N);

            a = 0;
            b = 0;
            for(int j=0;j<N;j++)
            {
                a |= static_cast<tuint32>(x[j]) << ((N-(j+1)) * 8);
                b |= static_cast<tuint32>(y[j]) << ((N-(j+1)) * 8);
            }
            r |= isSampleEquivALSA(a,b,N);

            a = 0;
            b = 0;
            for(int j=0;j<N;j++)
            {
                a |= static_cast<tuint32>(x[bytesPerSample-(j+1)]) << (j * 8);
                b |= static_cast<tuint32>(y[bytesPerSample-(j+1)]) << (j * 8);
            }
            r |= isSampleEquivALSA(a,b,N);

            a = 0;
            b = 0;
            for(int j=0;j<N;j++)
            {
                a |= static_cast<tuint32>(x[bytesPerSample-(j+1)]) << ((N-(j+1)) * 8);
                b |= static_cast<tuint32>(y[bytesPerSample-(j+1)]) << ((N-(j+1)) * 8);
            }
            r |= isSampleEquivALSA(a,b,N);

            if(!r)
            {
                int c =0;
            }
            EXPECT_TRUE(r);

            x += bytesPerSample;
            y += bytesPerSample;
        }
    }
    else
    {
        EXPECT_EQ(0,memcmp(expect,out,12 * bytesPerSample));
    }
#else
    EXPECT_EQ(0,memcmp(expect,out,12 * bytesPerSample));
#endif
}

//-------------------------------------------------------------------------------------------

class AOLinuxALSACreateSampleConverterTest : public AOLinuxALSA
{
    public:
        SampleConverter *testCreateSampleConverter(tint formatType);
};

//-------------------------------------------------------------------------------------------

SampleConverter *AOLinuxALSACreateSampleConverterTest::testCreateSampleConverter(tint formatType)
{
    return createSampleConverter(formatType);
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned8BitOutput)
{
    const tint c_bytesPerSample = 1;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x7f,
        0x66,
        0x4c,
        0x33,
        0x19,
        0xe6,
        0xcd,
        0xb3,
        0x9a,
        0x80,
        0x7f,
        0x80
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S8);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,8,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned16BitLittleEndianOutput)
{
    const tint c_bytesPerSample = 2;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0xff, 0x7f,
        0x66, 0x66,
        0xcc, 0x4c,
        0x33, 0x33,
        0x99, 0x19,
        0x66, 0xe6,
        0xcd, 0xcc,
        0x33, 0xb3,
        0x9a, 0x99,
        0x00, 0x80,
        0xff, 0x7f,
        0x00, 0x80
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S16_LE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,16,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned16BitBigEndianOutput)
{
    const tint c_bytesPerSample = 2;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x7f, 0xff,
        0x66, 0x66,
        0x4c, 0xcc,
        0x33, 0x33,
        0x19, 0x99,
        0xe6, 0x66,
        0xcc, 0xcd,
        0xb3, 0x33,
        0x99, 0x9a,
        0x80, 0x00,
        0x7f, 0xff,
        0x80, 0x00
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S16_BE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,16,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned24BitIn4BytesLittleEndianOutput)
{
    const tint c_bytesPerSample = 4;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0xff, 0xff, 0x7f, 0x00,
        0x66, 0x66, 0x66, 0x00,
        0xcc, 0xcc, 0x4c, 0x00,
        0x33, 0x33, 0x33, 0x00,
        0x99, 0x99, 0x19, 0x00,
        0x66, 0x66, 0xe6, 0xff,
        0xcd, 0xcc, 0xcc, 0xff,
        0x33, 0x33, 0xb3, 0xff,
        0x9a, 0x99, 0x99, 0xff,
        0x00, 0x00, 0x80, 0xff,
        0xff, 0xff, 0x7f, 0x00,
        0x00, 0x00, 0x80, 0xff
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S24_LE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,24,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned24BitIn4BytesBigEndianOutput)
{
    const tint c_bytesPerSample = 4;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x00, 0x7f, 0xff, 0xff,
        0x00, 0x66, 0x66, 0x66,
        0x00, 0x4c, 0xcc, 0xcc,
        0x00, 0x33, 0x33, 0x33,
        0x00, 0x19, 0x99, 0x99,
        0xff, 0xe6, 0x66, 0x66,
        0xff, 0xcc, 0xcc, 0xcd,
        0xff, 0xb3, 0x33, 0x33,
        0xff, 0x99, 0x99, 0x9a,
        0xff, 0x80, 0x00, 0x00,
        0x00, 0x7f, 0xff, 0xff,
        0xff, 0x80, 0x00, 0x00
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S24_BE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,24,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned32BitLittleEndianOutput)
{
    const tint c_bytesPerSample = 4;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0xff, 0xff, 0xff, 0x7f,
        0x66, 0x66, 0x66, 0x66,
        0xcc, 0xcc, 0xcc, 0x4c,
        0x33, 0x33, 0x33, 0x33,
        0x99, 0x99, 0x99, 0x19,
        0x66, 0x66, 0x66, 0xe6,
        0xcd, 0xcc, 0xcc, 0xcc,
        0x33, 0x33, 0x33, 0xb3,
        0x9a, 0x99, 0x99, 0x99,
        0x00, 0x00, 0x00, 0x80,
        0xff, 0xff, 0xff, 0x7f,
        0x00, 0x00, 0x00, 0x80
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S32_LE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,32,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned32BitBigEndianOutput)
{
    const tint c_bytesPerSample = 4;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x7f, 0xff, 0xff, 0xff,
        0x66, 0x66, 0x66, 0x66,
        0x4c, 0xcc, 0xcc, 0xcc,
        0x33, 0x33, 0x33, 0x33,
        0x19, 0x99, 0x99, 0x99,
        0xe6, 0x66, 0x66, 0x66,
        0xcc, 0xcc, 0xcc, 0xcd,
        0xb3, 0x33, 0x33, 0x33,
        0x99, 0x99, 0x99, 0x9a,
        0x80, 0x00, 0x00, 0x00,
        0x7f, 0xff, 0xff, 0xff,
        0x80, 0x00, 0x00, 0x00
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S32_BE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,32,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForFloatLittleEndianOutput)
{
#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tfloat32 c_sampleSingle[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };

    tbyte expect[12 * 4];
    engine::writeNative32BitsAsLittleEndian(reinterpret_cast<const tbyte*>(c_sampleSingle),expect,12);

    tubyte *out = new tubyte [12 * sizeof(tfloat32)];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_FLOAT_LE);

    sampleConverter->convert(c_sampleInput,out,12);

    EXPECT_EQ(0,memcmp(expect,out,12 * sizeof(tfloat32)));

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForFloatBigEndianOutput)
{
#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tfloat32 c_sampleSingle[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };

    tbyte expect[12 * 4];
    engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte*>(c_sampleSingle),expect,12);

    tubyte *out = new tubyte [12 * sizeof(tfloat32)];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_FLOAT_BE);

    sampleConverter->convert(c_sampleInput,out,12);

    EXPECT_EQ(0,memcmp(expect,out,12 * sizeof(tfloat32)));

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForFloat64LittleEndianOutput)
{
#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    tfloat64 sampleExpect[12];
    for(int i=0;i<12;i++)
    {
        sampleExpect[i] = static_cast<tfloat64>(c_sampleInput[i]);
    }
    tbyte expect[12 * 8];
    engine::writeNative64BitsAsLittleEndian(reinterpret_cast<const tbyte*>(sampleExpect),expect,12);

    tubyte *out = new tubyte [12 * sizeof(tfloat64)];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_FLOAT64_LE);

    sampleConverter->convert(c_sampleInput,out,12);

    EXPECT_EQ(0,memcmp(expect,out,12 * sizeof(tfloat64)));

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForFloat64BigEndianOutput)
{
#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    tfloat64 sampleExpect[12];
    for(int i=0;i<12;i++)
    {
        sampleExpect[i] = static_cast<tfloat64>(c_sampleInput[i]);
    }
    tbyte expect[12 * 8];
    engine::writeNative64BitsAsBigEndian(reinterpret_cast<const tbyte*>(sampleExpect),expect,12);

    tubyte *out = new tubyte [12 * sizeof(tfloat64)];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_FLOAT64_BE);

    sampleConverter->convert(c_sampleInput,out,12);

    EXPECT_EQ(0,memcmp(expect,out,12 * sizeof(tfloat64)));

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned24BitIn3BytesLittleEndianOutput)
{
    const tint c_bytesPerSample = 3;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0xff, 0xff, 0x7f,
        0x66, 0x66, 0x66,
        0xcc, 0xcc, 0x4c,
        0x33, 0x33, 0x33,
        0x99, 0x99, 0x19,
        0x66, 0x66, 0xe6,
        0xcd, 0xcc, 0xcc,
        0x33, 0x33, 0xb3,
        0x9a, 0x99, 0x99,
        0x00, 0x00, 0x80,
        0xff, 0xff, 0x7f,
        0x00, 0x00, 0x80
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S24_3LE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,24,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned24BitIn3BytesBigEndianOutput)
{
    const tint c_bytesPerSample = 3;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x7f, 0xff, 0xff,
        0x66, 0x66, 0x66,
        0x4c, 0xcc, 0xcc,
        0x33, 0x33, 0x33,
        0x19, 0x99, 0x99,
        0xe6, 0x66, 0x66,
        0xcc, 0xcc, 0xcd,
        0xb3, 0x33, 0x33,
        0x99, 0x99, 0x9a,
        0x80, 0x00, 0x00,
        0x7f, 0xff, 0xff,
        0x80, 0x00, 0x00
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S24_3BE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,24,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned20BitLittleEndianOutput)
{
    const tint c_bytesPerSample = 3;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
         1.0, //  0: = 0x0007FFFF =  524287 =  1048574 =  1048574.0 / 1048574.0 =  1.0
         0.8, //  1: = 0x00066666 =  419430 =   838860 =   838859.2 / 1048574.0 =  0.8 (  838859.2  838858=1.2  838860=0.8 )
         0.6, //  2: = 0x0004CCCC =  314572 =   629144 =   629144.4 / 1048574.0 =  0.6 (  629144.4  629144=0.4  629146=1.6 )
         0.4, //  3: = 0x00033333 =  209715 =   419430 =   419429.6 / 1048574.0 =  0.4 (  419429.6  419428=1.6  419430=0.4 )
         0.2, //  4: = 0x00019999 =  104857 =   209714 =   209714.8 / 1048574.0 =  0.2 (  209714.8  209714=0.8  209716=1.2 )
        -0.2, //  5: = 0xFFFE6666 = -104858 =  -209716 =  -209715.2 / 1048576.0 = -0.2 ( -209715.2 -209714=1.2 -209716=0.8 )
        -0.4, //  6: = 0xFFFCCCCD = -209715 =  -419430 =  -419430.4 / 1048576.0 = -0.4 ( -419430.4 -419430=0.4 -419432=1.6 )
        -0.6, //  7: = 0xFFFB3333 = -314573 =  -629146 =  -629145.6 / 1048576.0 = -0.6 ( -629145.6 -629144=1.6 -629146=0.4 )
        -0.8, //  8: = 0xFFF9999A = -419430 =  -838860 =  -838860.8 / 1048576.0 = -0.8 ( -838860.8 -838860=0.8 -838862=1.2 )
        -1.0, //  9: = 0xFFF80000 = -524288 = -1048576 = -1048576.0 / 1048576.0 = -1.0
         1.2, // 10: = 0x0007FFFF =  1.2
        -1.2  // 11: = 0xFFF80000 = -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0xff, 0xff, 0x07,
        0x66, 0x66, 0x06,
        0xcc, 0xcc, 0x04,
        0x33, 0x33, 0x03,
        0x99, 0x99, 0x01,
        0x66, 0x66, 0xfe,
        0xcd, 0xcc, 0xfc,
        0x33, 0x33, 0xfb,
        0x9a, 0x99, 0xf9,
        0x00, 0x00, 0xf8,
        0xff, 0xff, 0x07,
        0x00, 0x00, 0xf8
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S20_3LE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,20,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned20BitBigEndianOutput)
{
    const tint c_bytesPerSample = 3;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
         1.0, //  0: = 0x0007FFFF =  524287 =  1048574 =  1048574.0 / 1048574.0 =  1.0
         0.8, //  1: = 0x00066666 =  419430 =   838860 =   838859.2 / 1048574.0 =  0.8 (  838859.2  838858=1.2  838860=0.8 )
         0.6, //  2: = 0x0004CCCC =  314572 =   629144 =   629144.4 / 1048574.0 =  0.6 (  629144.4  629144=0.4  629146=1.6 )
         0.4, //  3: = 0x00033333 =  209715 =   419430 =   419429.6 / 1048574.0 =  0.4 (  419429.6  419428=1.6  419430=0.4 )
         0.2, //  4: = 0x00019999 =  104857 =   209714 =   209714.8 / 1048574.0 =  0.2 (  209714.8  209714=0.8  209716=1.2 )
        -0.2, //  5: = 0xFFFE6666 = -104858 =  -209716 =  -209715.2 / 1048576.0 = -0.2 ( -209715.2 -209714=1.2 -209716=0.8 )
        -0.4, //  6: = 0xFFFCCCCD = -209715 =  -419430 =  -419430.4 / 1048576.0 = -0.4 ( -419430.4 -419430=0.4 -419432=1.6 )
        -0.6, //  7: = 0xFFFB3333 = -314573 =  -629146 =  -629145.6 / 1048576.0 = -0.6 ( -629145.6 -629144=1.6 -629146=0.4 )
        -0.8, //  8: = 0xFFF9999A = -419430 =  -838860 =  -838860.8 / 1048576.0 = -0.8 ( -838860.8 -838860=0.8 -838862=1.2 )
        -1.0, //  9: = 0xFFF80000 = -524288 = -1048576 = -1048576.0 / 1048576.0 = -1.0
         1.2, // 10: = 0x0007FFFF =  1.2
        -1.2  // 11: = 0xFFF80000 = -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x07, 0xFF, 0xFF,
        0x06, 0x66, 0x66,
        0x04, 0xCC, 0xCC,
        0x03, 0x33, 0x33,
        0x01, 0x99, 0x99,
        0xFE, 0x66, 0x66,
        0xFC, 0xCC, 0xCD,
        0xFB, 0x33, 0x33,
        0xF9, 0x99, 0x9A,
        0xF8, 0x00, 0x00,
        0x07, 0xFF, 0xFF,
        0xF8, 0x00, 0x00,
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S20_3BE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,20,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned18BitLittleEndianOutput)
{
    const tint c_bytesPerSample = 3;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0xff, 0xff, 0x01,
        0x99, 0x99, 0x01,
        0x33, 0x33, 0x01,
        0xcc, 0xcc, 0x00,
        0x66, 0x66, 0x00,
        0x9a, 0x99, 0xff,
        0x33, 0x33, 0xff,
        0xcd, 0xcc, 0xfe,
        0x66, 0x66, 0xfe,
        0x00, 0x00, 0xfe,
        0xff, 0xff, 0x01,
        0x00, 0x00, 0xfe
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S18_3LE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,18,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createSampleConverterForSigned18BitBigEndianOutput)
{
    const tint c_bytesPerSample = 3;

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * c_bytesPerSample] = {
        0x01, 0xff, 0xff,
        0x01, 0x99, 0x99,
        0x01, 0x33, 0x33,
        0x00, 0xcc, 0xcc,
        0x00, 0x66, 0x66,
        0xff, 0x99, 0x9a,
        0xff, 0x33, 0x33,
        0xfe, 0xcc, 0xcd,
        0xfe, 0x66, 0x66,
        0xfe, 0x00, 0x00,
        0x01, 0xff, 0xff,
        0xfe, 0x00, 0x00
    };

    tubyte *out = new tubyte [12 * c_bytesPerSample];

    AOLinuxALSACreateSampleConverterTest audio;
    SampleConverter *sampleConverter = audio.testCreateSampleConverter(SND_PCM_FORMAT_S18_3BE);

    sampleConverter->convert(c_sampleInput,out,12);

    testSampleOutputConvertionALSA(c_expectOutput,out,18,c_bytesPerSample,12);

    delete sampleConverter;
    delete [] out;
}

//-------------------------------------------------------------------------------------------

class AOLinuxALSAWriteToAudioOutputBufferFromPartDataTest : public AOLinuxALSA
{
    public:
        MOCK_METHOD0(getSampleConverter,SampleConverter *());

        SampleConverter *createSampleConverter(tint formatType);

        void testWriteToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,
            const engine::RData *data, tint partNumber, tint inChannelIndex,
            tint bufferIndex, tint outChannelIndex, tint inputSampleIndex,
            tint outputSampleIndex, tint amount);
};

//-------------------------------------------------------------------------------------------

SampleConverter *AOLinuxALSAWriteToAudioOutputBufferFromPartDataTest::createSampleConverter(tint formatType)
{
    return AOLinuxALSA::createSampleConverter(formatType);
}

//-------------------------------------------------------------------------------------------

void AOLinuxALSAWriteToAudioOutputBufferFromPartDataTest::testWriteToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,
            const engine::RData *data, tint partNumber, tint inChannelIndex,
            tint bufferIndex, tint outChannelIndex, tint inputSampleIndex,
            tint outputSampleIndex, tint amount)
{
    writeToAudioOutputBufferFromPartData(pBuffer,data,partNumber,inChannelIndex,bufferIndex,outChannelIndex,inputSampleIndex,outputSampleIndex,amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,writeToAudioOutputBufferFromPartData)
{
#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_sampleInput[12] = {
        1.0f, 0.8f, 0.6f, 0.4f, 0.2f, -0.2f, -0.4f, -0.6f, -0.8f, -1.0f, 1.2f, -1.2f
    };
#else
    const sample_t c_sampleInput[12] = {
        1.0, 0.8, 0.6, 0.4, 0.2, -0.2, -0.4, -0.6, -0.8, -1.0, 1.2, -1.2
    };
#endif

    const tubyte c_expectOutput[12 * 3] = {
        0xff, 0xff, 0x7f,
        0x66, 0x66, 0x66,
        0xcc, 0xcc, 0x4c,
        0x33, 0x33, 0x33,
        0x99, 0x99, 0x19,
        0x66, 0x66, 0xe6,
        0xcd, 0xcc, 0xcc,
        0x33, 0x33, 0xb3,
        0x9a, 0x99, 0x99,
        0x00, 0x00, 0x80,
        0xff, 0xff, 0x7f,
        0x00, 0x00, 0x80
    };

    engine::RData data(6,2,2);
    engine::RData::Part& part = data.nextPart();
    memcpy(data.partDataOut(0),c_sampleInput,12 * sizeof(sample_t));

    tbyte *mem = new tbyte [12 * 3];
    AudioHardwareBufferALSA outBuffer(SND_PCM_FORMAT_S24_3LE,6,2,mem,12*3);

    AOLinuxALSAWriteToAudioOutputBufferFromPartDataTest audio;

    SampleConverter *pSampleConverter = audio.createSampleConverter(SND_PCM_FORMAT_S24_3LE);
    EXPECT_CALL(audio,getSampleConverter()).WillRepeatedly(Return(pSampleConverter));

    audio.testWriteToAudioOutputBufferFromPartData(&outBuffer,&data,0,0,0,0,0,0,6);
    audio.testWriteToAudioOutputBufferFromPartData(&outBuffer,&data,0,1,0,1,0,0,6);

    testSampleOutputConvertionALSA(c_expectOutput,reinterpret_cast<const tubyte *>(outBuffer.buffer(0)),24,3,12);

    delete [] mem;
}

//-------------------------------------------------------------------------------------------

class AOLinuxALSACreateIOTimeStampTest : public AOLinuxALSA
{
    public:
        IOTimeStamp testCreateIOTimeStamp(snd_pcm_t *handle) const;
};

//-------------------------------------------------------------------------------------------

IOTimeStamp AOLinuxALSACreateIOTimeStampTest::testCreateIOTimeStamp(snd_pcm_t *handle) const
{
    return createIOTimeStamp(handle);
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createIOTimeStampGivenErrorInGettingTimeStamp)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_htimestamp(Eq(handle),A<snd_pcm_uframes_t *>(),A<snd_htimestamp_t *>())).Times(1)
        .WillOnce(Return(-1));

    AOLinuxALSACreateIOTimeStampTest audio;

    IOTimeStamp timeStamp = audio.testCreateIOTimeStamp(handle);

    EXPECT_FALSE(timeStamp.isValid());
    EXPECT_TRUE(timeStamp.time() == 0);

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,createIOTimeStampWithSuccess)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    snd_htimestamp_t tTimeStamp;
    tTimeStamp.tv_sec = 10;
    tTimeStamp.tv_nsec = 10000;
    common::TimeStamp tS;
    tS.set(&tTimeStamp);

    LinuxALSAIFSPtr pAPI = LinuxALSAIF::instance("mock");
    LinuxALSAMockIF& apiMock = dynamic_cast<LinuxALSAMockIF&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,snd_pcm_htimestamp(Eq(handle),A<snd_pcm_uframes_t *>(),A<snd_htimestamp_t *>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(tTimeStamp),Return(0)));

    AOLinuxALSACreateIOTimeStampTest audio;

    IOTimeStamp timeStamp = audio.testCreateIOTimeStamp(handle);

    EXPECT_TRUE(timeStamp.isValid());
    EXPECT_TRUE(timeStamp.time() == tS);

    LinuxALSAIF::release();
}

//-------------------------------------------------------------------------------------------

class AOLinuxALSAFormatFromDescriptionTest : public AOLinuxALSA
{
    public:
        int testFormatFromDescription(snd_pcm_t *handle,QSharedPointer<AOQueryALSA::DeviceALSA>& pDeviceALSA,const FormatDescription& desc);
};

//-------------------------------------------------------------------------------------------

int AOLinuxALSAFormatFromDescriptionTest::testFormatFromDescription(snd_pcm_t *handle,QSharedPointer<AOQueryALSA::DeviceALSA>& pDeviceALSA,const FormatDescription& desc)
{
    return formatFromDescription(handle,pDeviceALSA,desc);
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,formatFromDescriptionGivenNoFormatsFromDescription)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    FormatDescription desc(FormatDescription::e_DataSignedInteger,24,2,44100,true);

    QVector<int> formats;

    QSharedPointer<AOQueryALSA::DeviceALSA> pDeviceALSA(new AOQueryALSADeviceMock);
    AOQueryALSADeviceMock& device = dynamic_cast<AOQueryALSADeviceMock&>(*(pDeviceALSA.data()));

    EXPECT_CALL(device,formatsFromDescription(Eq(desc))).Times(1).WillOnce(Return(formats));

    AOLinuxALSAFormatFromDescriptionTest audio;

    ASSERT_EQ(SND_PCM_FORMAT_UNKNOWN,audio.testFormatFromDescription(handle,pDeviceALSA,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,formatFromDescriptionGivenAFormatFromDescription)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    FormatDescription desc(FormatDescription::e_DataSignedInteger,24,2,44100,true);

    QVector<int> formats;
    formats.append(SND_PCM_FORMAT_S16_LE);

    QSharedPointer<AOQueryALSA::DeviceALSA> pDeviceALSA(new AOQueryALSADeviceMock);
    AOQueryALSADeviceMock& device = dynamic_cast<AOQueryALSADeviceMock&>(*(pDeviceALSA.data()));

    EXPECT_CALL(device,formatsFromDescription(Eq(desc))).Times(1).WillOnce(Return(formats));

    AOLinuxALSAFormatFromDescriptionTest audio;

    ASSERT_EQ(SND_PCM_FORMAT_S16_LE,audio.testFormatFromDescription(handle,pDeviceALSA,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,formatFromDescriptionGivenFormatsFromDescriptionButNoneAreSupported)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    FormatDescription desc(FormatDescription::e_DataSignedInteger,24,2,44100,true);

    QVector<int> formats;
    formats.append(SND_PCM_FORMAT_S24_LE);
    formats.append(SND_PCM_FORMAT_S24_3BE);

    QSharedPointer<AOQueryALSA::DeviceALSA> pDeviceALSA(new AOQueryALSADeviceMock);
    AOQueryALSADeviceMock& device = dynamic_cast<AOQueryALSADeviceMock&>(*(pDeviceALSA.data()));

    EXPECT_CALL(device,formatsFromDescription(Eq(desc))).Times(1).WillOnce(Return(formats));
    EXPECT_CALL(device,canSupportFormat(Eq(handle),Eq((int)SND_PCM_FORMAT_S24_LE),Eq(2),Eq(44100))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(device,canSupportFormat(Eq(handle),Eq((int)SND_PCM_FORMAT_S24_3BE),Eq(2),Eq(44100))).Times(1).WillOnce(Return(false));

    AOLinuxALSAFormatFromDescriptionTest audio;

    ASSERT_EQ(SND_PCM_FORMAT_UNKNOWN,audio.testFormatFromDescription(handle,pDeviceALSA,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,formatFromDescriptionGivenFormatsFromDescriptionFirstIsSupported)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    FormatDescription desc(FormatDescription::e_DataSignedInteger,24,2,44100,true);

    QVector<int> formats;
    formats.append(SND_PCM_FORMAT_S24_LE);
    formats.append(SND_PCM_FORMAT_S24_3BE);

    QSharedPointer<AOQueryALSA::DeviceALSA> pDeviceALSA(new AOQueryALSADeviceMock);
    AOQueryALSADeviceMock& device = dynamic_cast<AOQueryALSADeviceMock&>(*(pDeviceALSA.data()));

    EXPECT_CALL(device,formatsFromDescription(Eq(desc))).Times(1).WillOnce(Return(formats));
    EXPECT_CALL(device,canSupportFormat(Eq(handle),Eq((int)SND_PCM_FORMAT_S24_LE),Eq(2),Eq(44100))).Times(1).WillOnce(Return(true));

    AOLinuxALSAFormatFromDescriptionTest audio;

    ASSERT_EQ(SND_PCM_FORMAT_S24_LE,audio.testFormatFromDescription(handle,pDeviceALSA,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,formatFromDescriptionGivenFormatsFromDescriptionSecondIsSupported)
{
    snd_pcm_t *handle = (snd_pcm_t *)(5);
    FormatDescription desc(FormatDescription::e_DataSignedInteger,24,2,44100,true);

    QVector<int> formats;
    formats.append(SND_PCM_FORMAT_S24_LE);
    formats.append(SND_PCM_FORMAT_S24_3BE);

    QSharedPointer<AOQueryALSA::DeviceALSA> pDeviceALSA(new AOQueryALSADeviceMock);
    AOQueryALSADeviceMock& device = dynamic_cast<AOQueryALSADeviceMock&>(*(pDeviceALSA.data()));

    EXPECT_CALL(device,formatsFromDescription(Eq(desc))).Times(1).WillOnce(Return(formats));
    EXPECT_CALL(device,canSupportFormat(Eq(handle),Eq((int)SND_PCM_FORMAT_S24_LE),Eq(2),Eq(44100))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(device,canSupportFormat(Eq(handle),Eq((int)SND_PCM_FORMAT_S24_3BE),Eq(2),Eq(44100))).Times(1).WillOnce(Return(true));

    AOLinuxALSAFormatFromDescriptionTest audio;

    ASSERT_EQ(SND_PCM_FORMAT_S24_3BE,audio.testFormatFromDescription(handle,pDeviceALSA,desc));
}

//-------------------------------------------------------------------------------------------

class AOLinuxALSAProcessMessagesTest : public AOLinuxALSA
{
    public:
        MOCK_CONST_METHOD0(getStopTimeFlag,bool());
        MOCK_METHOD1(setStopTimeFlag,void(bool f));
        MOCK_CONST_METHOD0(getFlagStart,bool());
        MOCK_CONST_METHOD0(getReferenceClockTime,common::TimeStamp());
        MOCK_CONST_METHOD0(getStopTimeClock,common::TimeStamp());
        MOCK_METHOD1(stopCodec,void(bool));
        MOCK_METHOD1(processMessagesForStopSetTimer,void(tint delay));

        void testProcessMessagesForStop();
};

//-------------------------------------------------------------------------------------------

void AOLinuxALSAProcessMessagesTest::testProcessMessagesForStop()
{
    processMessagesForStop();
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,processMessagesForStopGivenStopFlagNotSet)
{
    AOLinuxALSAProcessMessagesTest audio;
    EXPECT_CALL(audio,getStopTimeFlag()).Times(1).WillOnce(Return(false));

    audio.testProcessMessagesForStop();
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,processMessagesForStopGivenStopFlagIsSetButAudioIsNotStarted)
{
    AOLinuxALSAProcessMessagesTest audio;
    EXPECT_CALL(audio,getStopTimeFlag()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getFlagStart()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(audio,setStopTimeFlag(false)).Times(1);

    audio.testProcessMessagesForStop();
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,processMessagesForStopGivenStopAndAudioWithReferenceAfterStopTime)
{
    common::TimeStamp refeT(50.0);
    common::TimeStamp stopT(49.0);

    AOLinuxALSAProcessMessagesTest audio;
    EXPECT_CALL(audio,getStopTimeFlag()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getFlagStart()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getReferenceClockTime()).Times(1).WillOnce(Return(refeT));
    EXPECT_CALL(audio,getStopTimeClock()).Times(1).WillOnce(Return(stopT));
    EXPECT_CALL(audio,stopCodec(true)).Times(1);
    EXPECT_CALL(audio,setStopTimeFlag(false)).Times(1);

    audio.testProcessMessagesForStop();
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,processMessagesForStopGivenStopAndAudioWithReferenceBeforeStopTimeWithInsignifcantDelay)
{
    common::TimeStamp refeT(50.0);
    common::TimeStamp stopT(50.000001);

    AOLinuxALSAProcessMessagesTest audio;
    EXPECT_CALL(audio,getStopTimeFlag()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getFlagStart()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getReferenceClockTime()).Times(1).WillOnce(Return(refeT));
    EXPECT_CALL(audio,getStopTimeClock()).Times(1).WillOnce(Return(stopT));
    EXPECT_CALL(audio,stopCodec(true)).Times(1);
    EXPECT_CALL(audio,setStopTimeFlag(false)).Times(1);

    audio.testProcessMessagesForStop();
}

//-------------------------------------------------------------------------------------------

TEST(AOLinuxALSA,processMessagesForStopGivenStopAndAudioWithReferenceBeforeStopTimeWithDelay)
{
    common::TimeStamp refeT(50.0);
    common::TimeStamp stopT(53.2);

    AOLinuxALSAProcessMessagesTest audio;
    EXPECT_CALL(audio,getStopTimeFlag()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getFlagStart()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(audio,getReferenceClockTime()).Times(1).WillOnce(Return(refeT));
    EXPECT_CALL(audio,getStopTimeClock()).Times(1).WillOnce(Return(stopT));
    EXPECT_CALL(audio,processMessagesForStopSetTimer(3200)).Times(1);
    EXPECT_CALL(audio,setStopTimeFlag(false)).Times(1);

    audio.testProcessMessagesForStop();
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
