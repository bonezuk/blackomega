#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "engine/wavpackomega/inc/WavPackCodec.h"

using namespace testing;
using namespace omega;
using namespace omega::engine::wavpackomega;

//-------------------------------------------------------------------------------------------

class WavPackCodecTest : public WavPackCodec
{
    public:
        WavPackCodecTest();
        virtual ~WavPackCodecTest();
        
        virtual void setupReadDecodeData(tint bitsPerSample,char *buffer);
        virtual void testReadDecodedData(sample_t *output,tint sampleOffset,tint amount);
        virtual void freeReadDecodeData();
};

//-------------------------------------------------------------------------------------------

WavPackCodecTest::WavPackCodecTest() : WavPackCodec()
{}

//-------------------------------------------------------------------------------------------

WavPackCodecTest::~WavPackCodecTest()
{}

//-------------------------------------------------------------------------------------------

void WavPackCodecTest::setupReadDecodeData(tint bitsPerSample,char *buffer)
{
    m_context = reinterpret_cast<WavpackContext *>(buffer);
    m_bitsPerSample = bitsPerSample;
    m_noWavChannels = 2;
    m_noOutputChannels = 2;
    m_channelMask = static_cast<int>(engine::WavChannelMask::e_speakerFrontLeft | engine::WavChannelMask::e_speakerFrontRight);
    m_pBuffer = reinterpret_cast<char *>(buffer);
    WavPackCodec::init();
}

//-------------------------------------------------------------------------------------------

void WavPackCodecTest::testReadDecodedData(sample_t *output,tint sampleOffset,tint amount)
{
    readDecodedData(output,sampleOffset,amount);
}

//-------------------------------------------------------------------------------------------

void WavPackCodecTest::freeReadDecodeData()
{
    m_context = 0;
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWith32BitFloatNormalized)
{
    sample_t output[2] = { 0.0f, 0.0f };
    tfloat32 decodeBuffer[2] = { 1.0f, -1.0f };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(0,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger1Bit)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00000080, 0x00000000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(1,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger2Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00000040, 0x00000080 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(2,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger3Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00000060, 0x00000080 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(3,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger4Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00000070, 0x00000080 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(4,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger5Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00000078, 0x00000080 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(5,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger6Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x0000007c, 0x00000080 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(6,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger7Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x0000007e, 0x00000080 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(7,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger8Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x0000007f, 0x00000080 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(8,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger9Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00007f80, 0x00008000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(9,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger10Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00007fc0, 0x00008000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(10,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger11Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00007fe0, 0x00008000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(11,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger12Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00007ff0, 0x00008000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(12,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger13Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00007ff8, 0x00008000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(13,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger14Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00007ffc, 0x00008000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(14,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger15Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00007ffe, 0x00008000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(15,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger16Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x00007fff, 0x00008000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(16,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger17Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x007fff80, 0x00800000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(17,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger18Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x007fffc0, 0x00800000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(18,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger19Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x007fffe0, 0x00800000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(19,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger20Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x007ffff0, 0x00800000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(20,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger21Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x007ffff8, 0x00800000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(21,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger22Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x007ffffc, 0x00800000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(22,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger23Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x007ffffe, 0x00800000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(23,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger24Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tint32 decodeBuffer[2] = { 0x007fffff, 0x00800000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(24,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.000001);
    ASSERT_NEAR(-1.0,output[1],0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger25Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tuint32 decodeBuffer[2] = { 0x7fffff80, 0x80000000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(25,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.0001);
    ASSERT_NEAR(-1.0,output[1],0.0001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger26Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tuint32 decodeBuffer[2] = { 0x7fffffc0, 0x80000000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(26,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.0001);
    ASSERT_NEAR(-1.0,output[1],0.0001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger27Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tuint32 decodeBuffer[2] = { 0x7fffffe0, 0x80000000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(27,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.0001);
    ASSERT_NEAR(-1.0,output[1],0.0001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger28Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tuint32 decodeBuffer[2] = { 0x7ffffff0, 0x80000000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(28,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.0001);
    ASSERT_NEAR(-1.0,output[1],0.0001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger29Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tuint32 decodeBuffer[2] = { 0x7ffffff8, 0x80000000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(29,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.0001);
    ASSERT_NEAR(-1.0,output[1],0.0001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger30Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tuint32 decodeBuffer[2] = { 0x7ffffffc, 0x80000000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(30,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.0001);
    ASSERT_NEAR(-1.0,output[1],0.0001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger31Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tuint32 decodeBuffer[2] = { 0x7ffffffe, 0x80000000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(31,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.0001);
    ASSERT_NEAR(-1.0,output[1],0.0001);
}

//-------------------------------------------------------------------------------------------

TEST(WavPackCodec,readDecodedDataWithInteger32Bits)
{
    sample_t output[2] = { 0.0, 0.0 };
    tuint32 decodeBuffer[2] = { 0x7fffffff, 0x80000000 };
    
    WavPackCodecTest codec;
    codec.setupReadDecodeData(32,reinterpret_cast<char *>(decodeBuffer));
    codec.testReadDecodedData(output,0,1);
    codec.freeReadDecodeData();
    
    ASSERT_NEAR( 1.0,output[0],0.0001);
    ASSERT_NEAR(-1.0,output[1],0.0001);
}

//-------------------------------------------------------------------------------------------
