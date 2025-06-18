#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "engine/violetomega/inc/IFFSoundChunk.h"
#include "engine/violetomega/inc/IFFFile.h"
#include "common/inc/BIOMemory.h"

using namespace testing;
using namespace omega;
using namespace omega::engine::violetomega;

//-------------------------------------------------------------------------------------------

class IFFSoundChunkTest : public IFFSoundChunk
{
    public:
        IFFSoundChunkTest();
        virtual ~IFFSoundChunkTest();

        virtual sample_t testReadSample(const tubyte *mem,int noBits);

        virtual int testIndexPosition(int idx) const;
        virtual int testBytesPerFrame() const;
        virtual int testBytesPerSample() const;

        void setOffset(int offset);
        void setBlockSize(int blockSize);
        void setSize(int nSize);

        virtual void testSetCurrentIndexPosition(int idx);
        virtual int testNextIndexPosition();

        virtual void testSortOutputChannels(const sample_t *in,sample_t *out);
};

//-------------------------------------------------------------------------------------------

IFFSoundChunkTest::IFFSoundChunkTest() : IFFSoundChunk()
{}

//-------------------------------------------------------------------------------------------

IFFSoundChunkTest::~IFFSoundChunkTest()
{}

//-------------------------------------------------------------------------------------------

sample_t IFFSoundChunkTest::testReadSample(const tubyte *mem,int noBits)
{
    return readSample(reinterpret_cast<const tbyte *>(mem),noBits);
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunkTest::testIndexPosition(int idx) const
{
    return indexPosition(idx);
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunkTest::testBytesPerFrame() const
{
    return bytesPerFrame();
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunkTest::testBytesPerSample() const
{
    return bytesPerSample();
}

//-------------------------------------------------------------------------------------------

void IFFSoundChunkTest::setOffset(int offset)
{
    m_offset = offset;
}

//-------------------------------------------------------------------------------------------

void IFFSoundChunkTest::setBlockSize(int blockSize)
{
    m_blockSize = blockSize;
}

//-------------------------------------------------------------------------------------------

void IFFSoundChunkTest::setSize(int nSize)
{
    m_size = nSize;
}

//-------------------------------------------------------------------------------------------

void IFFSoundChunkTest::testSetCurrentIndexPosition(int idx)
{
    setCurrentIndexPosition(idx);
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunkTest::testNextIndexPosition()
{
    return nextIndexPosition();
}

//-------------------------------------------------------------------------------------------

void IFFSoundChunkTest::testSortOutputChannels(const sample_t *in,sample_t *out)
{
    sortOutputChannelsSample(in,out);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,createFromFactoryAndScanWithOffsetAndBlockSize)
{
    tubyte ssndMem[16] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x08,
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),16);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());

    EXPECT_TRUE(pSound->scan());

    EXPECT_TRUE(pSound->offset()==0x01020304);
    EXPECT_TRUE(pSound->blockSize()==0x05060708);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample1Bit)
{
    tubyte xL[1] = { 0x00 };
    tubyte xH[1] = { 0x80 };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,1),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,1),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample2Bits)
{
    tubyte xL[1] = { 0x80 };
    tubyte xH[1] = { 0x40 };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,2),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,2),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample3Bits)
{
    tubyte xL[1] = { 0x80 };
    tubyte xH[1] = { 0x60 };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,3),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,3),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample4Bits)
{
    tubyte xL[1] = { 0x80 };
    tubyte xH[1] = { 0x70 };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,4),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,4),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample5Bits)
{
    tubyte xL[1] = { 0x80 };
    tubyte xH[1] = { 0x78 };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,5),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,5),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample6Bits)
{
    tubyte xL[1] = { 0x80 };
    tubyte xH[1] = { 0x7c };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,6),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,6),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample7Bits)
{
    tubyte xL[1] = { 0x80 };
    tubyte xH[1] = { 0x7e };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,7),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,7),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample8Bits)
{
    tubyte xL[1] = { 0x80 };
    tubyte xH[1] = { 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,8),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,8),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample9BitsWhenLittleEndian)
{
    tubyte xL[2] = { 0x00, 0x80 };
    tubyte xH[2] = { 0x80, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,9),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,9),0.00000001);
}

TEST(IFFSoundChunk,readSample9BitsWhenBigEndian)
{
    tubyte xL[2] = { 0x80, 0x00 };
    tubyte xH[2] = { 0x7f, 0xc0 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,9),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,9),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample10BitsWhenLittleEndian)
{
    tubyte xL[2] = { 0x00, 0x80 };
    tubyte xH[2] = { 0xc0, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,10),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,10),0.00000001);
}

TEST(IFFSoundChunk,readSample10BitsWhenBigEndian)
{
    tubyte xL[2] = { 0x80, 0x00 };
    tubyte xH[2] = { 0x7f, 0xc0 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,10),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,10),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample11BitsWhenLittleEndian)
{
    tubyte xL[2] = { 0x00, 0x80 };
    tubyte xH[2] = { 0xe0, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,11),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,11),0.00000001);
}

TEST(IFFSoundChunk,readSample11BitsWhenBigEndian)
{
    tubyte xL[2] = { 0x80, 0x00 };
    tubyte xH[2] = { 0x7f, 0xe0 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,11),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,11),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample12BitsWhenLittleEndian)
{
    tubyte xL[2] = { 0x00, 0x80 };
    tubyte xH[2] = { 0xf0, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,12),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,12),0.00000001);
}

TEST(IFFSoundChunk,readSample12BitsWhenBigEndian)
{
    tubyte xL[2] = { 0x80, 0x00 };
    tubyte xH[2] = { 0x7f, 0xf0 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,12),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,12),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample13BitsWhenLittleEndian)
{
    tubyte xL[2] = { 0x00, 0x80 };
    tubyte xH[2] = { 0xf8, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,13),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,13),0.00000001);
}

TEST(IFFSoundChunk,readSample13BitsWhenBigEndian)
{
    tubyte xL[2] = { 0x80, 0x00 };
    tubyte xH[2] = { 0x7f, 0xf8 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,13),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,13),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample14BitsWhenLittleEndian)
{
    tubyte xL[2] = { 0x00, 0x80 };
    tubyte xH[2] = { 0xfc, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,14),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,14),0.00000001);
}

TEST(IFFSoundChunk,readSample14BitsWhenBigEndian)
{
    tubyte xL[2] = { 0x80, 0x00 };
    tubyte xH[2] = { 0x7f, 0xfc };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,14),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,14),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample15BitsWhenLittleEndian)
{
    tubyte xL[2] = { 0x00, 0x80 };
    tubyte xH[2] = { 0xfe, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,15),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,15),0.00000001);
}

TEST(IFFSoundChunk,readSample15BitsWhenBigEndian)
{
    tubyte xL[2] = { 0x80, 0x00 };
    tubyte xH[2] = { 0x7f, 0xfe };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,15),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,15),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample16BitsWhenLittleEndian)
{
    tubyte xL[2] = { 0x00, 0x80 };
    tubyte xH[2] = { 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,16),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,16),0.00000001);
}

TEST(IFFSoundChunk,readSample16BitsWhenBigEndian)
{
    tubyte xL[2] = { 0x80, 0x00 };
    tubyte xH[2] = { 0x7f, 0xff };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,16),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,16),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample17BitsWhenLittleEndian)
{
    tubyte xL[3] = { 0x00, 0x00, 0x80 };
    tubyte xH[3] = { 0x80, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,17),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,17),0.00000001);
}

TEST(IFFSoundChunk,readSample17BitsWhenBigEndian)
{
    tubyte xL[3] = { 0x80, 0x00, 0x00 };
    tubyte xH[3] = { 0x7f, 0xff, 0x80 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,17),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,17),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample18BitsWhenLittleEndian)
{
    tubyte xL[3] = { 0x00, 0x00, 0x80 };
    tubyte xH[3] = { 0xc0, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,18),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,18),0.00000001);
}

TEST(IFFSoundChunk,readSample18BitsWhenBigEndian)
{
    tubyte xL[3] = { 0x80, 0x00, 0x00 };
    tubyte xH[3] = { 0x7f, 0xff, 0xc0 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,18),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,18),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample19BitsWhenLittleEndian)
{
    tubyte xL[3] = { 0x00, 0x00, 0x80 };
    tubyte xH[3] = { 0xe0, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,19),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,19),0.00000001);
}

TEST(IFFSoundChunk,readSample19BitsWhenBigEndian)
{
    tubyte xL[3] = { 0x80, 0x00, 0x00 };
    tubyte xH[3] = { 0x7f, 0xff, 0xe0 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,19),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,19),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample20BitsWhenLittleEndian)
{
    tubyte xL[3] = { 0x00, 0x00, 0x80 };
    tubyte xH[3] = { 0xf0, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,20),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,20),0.00000001);
}

TEST(IFFSoundChunk,readSample20BitsWhenBigEndian)
{
    tubyte xL[3] = { 0x80, 0x00, 0x00 };
    tubyte xH[3] = { 0x7f, 0xff, 0xf0 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,20),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,20),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample21BitsWhenLittleEndian)
{
    tubyte xL[3] = { 0x00, 0x00, 0x80 };
    tubyte xH[3] = { 0xf8, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,21),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,21),0.00000001);
}

TEST(IFFSoundChunk,readSample21BitsWhenBigEndian)
{
    tubyte xL[3] = { 0x80, 0x00, 0x00 };
    tubyte xH[3] = { 0x7f, 0xff, 0xf8 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,21),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,21),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample22BitsWhenLittleEndian)
{
    tubyte xL[3] = { 0x00, 0x00, 0x80 };
    tubyte xH[3] = { 0xfc, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,22),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,22),0.00000001);
}

TEST(IFFSoundChunk,readSample22BitsWhenBigEndian)
{
    tubyte xL[3] = { 0x80, 0x00, 0x00 };
    tubyte xH[3] = { 0x7f, 0xff, 0xfc };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,22),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,22),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample23BitsWhenLittleEndian)
{
    tubyte xL[3] = { 0x00, 0x00, 0x80 };
    tubyte xH[3] = { 0xfe, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,23),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,23),0.00000001);
}

TEST(IFFSoundChunk,readSample23BitsWhenBigEndian)
{
    tubyte xL[3] = { 0x80, 0x00, 0x00 };
    tubyte xH[3] = { 0x7f, 0xff, 0xfe };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,23),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,23),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample24BitsWhenLittleEndian)
{
    tubyte xL[3] = { 0x00, 0x00, 0x80 };
    tubyte xH[3] = { 0xff, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,24),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,24),0.00000001);
}

TEST(IFFSoundChunk,readSample24BitsWhenBigEndian)
{
    tubyte xL[3] = { 0x80, 0x00, 0x00 };
    tubyte xH[3] = { 0x7f, 0xff, 0xff };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,24),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,24),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample25BitsWhenLittleEndian)
{
    tubyte xL[4] = { 0x00, 0x00, 0x00, 0x80 };
    tubyte xH[4] = { 0x80, 0xff, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,25),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,25),0.00000001);
}

TEST(IFFSoundChunk,readSample25BitsWhenBigEndian)
{
    tubyte xL[4] = { 0x80, 0x00, 0x00, 0x00 };
    tubyte xH[4] = { 0x7f, 0xff, 0xff, 0x80 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,25),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,25),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample26BitsWhenLittleEndian)
{
    tubyte xL[4] = { 0x00, 0x00, 0x00, 0x80 };
    tubyte xH[4] = { 0xc0, 0xff, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,26),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,26),0.00000001);
}

TEST(IFFSoundChunk,readSample26BitsWhenBigEndian)
{
    tubyte xL[4] = { 0x80, 0x00, 0x00, 0x00 };
    tubyte xH[4] = { 0x7f, 0xff, 0xff, 0xc0 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,26),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,26),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample27BitsWhenLittleEndian)
{
    tubyte xL[4] = { 0x00, 0x00, 0x00, 0x80 };
    tubyte xH[4] = { 0xe0, 0xff, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,27),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,27),0.00000001);
}

TEST(IFFSoundChunk,readSample27BitsWhenBigEndian)
{
    tubyte xL[4] = { 0x80, 0x00, 0x00, 0x00 };
    tubyte xH[4] = { 0x7f, 0xff, 0xff, 0xe0 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,27),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,27),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample28BitsWhenLittleEndian)
{
    tubyte xL[4] = { 0x00, 0x00, 0x00, 0x80 };
    tubyte xH[4] = { 0xf0, 0xff, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,28),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,28),0.00000001);
}

TEST(IFFSoundChunk,readSample28BitsWhenBigEndian)
{
    tubyte xL[4] = { 0x80, 0x00, 0x00, 0x00 };
    tubyte xH[4] = { 0x7f, 0xff, 0xff, 0xf0 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,28),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,28),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample29BitsWhenLittleEndian)
{
    tubyte xL[4] = { 0x00, 0x00, 0x00, 0x80 };
    tubyte xH[4] = { 0xf8, 0xff, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,29),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,29),0.00000001);
}

TEST(IFFSoundChunk,readSample29BitsWhenBigEndian)
{
    tubyte xL[4] = { 0x80, 0x00, 0x00, 0x00 };
    tubyte xH[4] = { 0x7f, 0xff, 0xff, 0xf8 };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,29),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,29),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample30BitsWhenLittleEndian)
{
    tubyte xL[4] = { 0x00, 0x00, 0x00, 0x80 };
    tubyte xH[4] = { 0xfc, 0xff, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,30),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,30),0.00000001);
}

TEST(IFFSoundChunk,readSample30BitsWhenBigEndian)
{
    tubyte xL[4] = { 0x80, 0x00, 0x00, 0x00 };
    tubyte xH[4] = { 0x7f, 0xff, 0xff, 0xfc };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,30),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,30),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample31BitsWhenLittleEndian)
{
    tubyte xL[4] = { 0x00, 0x00, 0x00, 0x80 };
    tubyte xH[4] = { 0xfe, 0xff, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,31),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,31),0.00000001);
}

TEST(IFFSoundChunk,readSample31BitsWhenBigEndian)
{
    tubyte xL[4] = { 0x80, 0x00, 0x00, 0x00 };
    tubyte xH[4] = { 0x7f, 0xff, 0xff, 0xfe };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,31),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,31),0.00000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readSample32BitsWhenLittleEndian)
{
    tubyte xL[4] = { 0x00, 0x00, 0x00, 0x80 };
    tubyte xH[4] = { 0xff, 0xff, 0xff, 0x7f };

    IFFSoundChunkTest samples;
    samples.setup(0,true);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,32),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,32),0.00000001);
}

TEST(IFFSoundChunk,readSample32BitsWhenBigEndian)
{
    tubyte xL[4] = { 0x80, 0x00, 0x00, 0x00 };
    tubyte xH[4] = { 0x7f, 0xff, 0xff, 0xff };

    IFFSoundChunkTest samples;
    samples.setup(0,false);

    EXPECT_NEAR(-1.0f,samples.testReadSample(xL,32),0.00000001);
    EXPECT_NEAR(+1.0f,samples.testReadSample(xH,32),0.00000001);
}

//-------------------------------------------------------------------------------------------

class IFFMockCommonChunk : public IFFCommonChunk
{
    public:
        MOCK_CONST_METHOD0(noChannels,const tint&());
        MOCK_CONST_METHOD0(noSampleFrames,const tint&());
        MOCK_CONST_METHOD0(sampleSize,const tint&());
        MOCK_CONST_METHOD0(sampleRate,const tfloat64&());
};

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerSampleGiven5BitSampleSize)
{
    int noChannels = 2;
    int noSampleFrames = 20;
    int sampleSize = 5;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(1,sound.testBytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerSampleGiven8BitSampleSize)
{
    int noChannels = 2;
    int noSampleFrames = 20;
    int sampleSize = 8;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(1,sound.testBytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerSampleGiven9BitSampleSize)
{
    int noChannels = 2;
    int noSampleFrames = 20;
    int sampleSize = 9;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(2,sound.testBytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerSampleGiven16BitSampleSize)
{
    int noChannels = 2;
    int noSampleFrames = 20;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(2,sound.testBytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerSampleGiven17BitSampleSize)
{
    int noChannels = 2;
    int noSampleFrames = 20;
    int sampleSize = 17;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(3,sound.testBytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerSampleGiven24BitSampleSize)
{
    int noChannels = 2;
    int noSampleFrames = 20;
    int sampleSize = 24;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(3,sound.testBytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerSampleGiven25BitSampleSize)
{
    int noChannels = 2;
    int noSampleFrames = 20;
    int sampleSize = 25;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(4,sound.testBytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerSampleGiven32BitSampleSize)
{
    int noChannels = 2;
    int noSampleFrames = 20;
    int sampleSize = 32;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(4,sound.testBytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerFrameGiven8BitSampleSizeAnd1Channel)
{
    int noChannels = 1;
    int noSampleFrames = 20;
    int sampleSize = 8;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(1,sound.testBytesPerFrame());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerFrameGiven16BitSampleSizeAnd2Channels)
{
    int noChannels = 2;
    int noSampleFrames = 20;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(4,sound.testBytesPerFrame());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,bytesPerFrameGiven24BitSampleSizeAnd6Channels)
{
    int noChannels = 6;
    int noSampleFrames = 20;
    int sampleSize = 24;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);

    EXPECT_EQ(18,sound.testBytesPerFrame());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,indexPositionAsContinous)
{
    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.setOffset(0);
    sound.setBlockSize(0);

    EXPECT_EQ( 0,sound.testIndexPosition( 0));
    EXPECT_EQ( 4,sound.testIndexPosition( 1));
    EXPECT_EQ( 8,sound.testIndexPosition( 2));
    EXPECT_EQ(12,sound.testIndexPosition( 3));
    EXPECT_EQ(16,sound.testIndexPosition( 4));
    EXPECT_EQ(20,sound.testIndexPosition( 5));
    EXPECT_EQ(24,sound.testIndexPosition( 6));
    EXPECT_EQ(28,sound.testIndexPosition( 7));
    EXPECT_EQ(32,sound.testIndexPosition( 8));
    EXPECT_EQ(36,sound.testIndexPosition( 9));
    EXPECT_EQ(40,sound.testIndexPosition(10));
    EXPECT_EQ(44,sound.testIndexPosition(11));
    EXPECT_EQ(48,sound.testIndexPosition(12));
    EXPECT_EQ(52,sound.testIndexPosition(13));
    EXPECT_EQ(56,sound.testIndexPosition(14));
    EXPECT_EQ(60,sound.testIndexPosition(15));
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,indexPositionAsContinousWithOffset)
{
    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.setOffset(3);
    sound.setBlockSize(0);

    EXPECT_EQ( 0+3,sound.testIndexPosition( 0));
    EXPECT_EQ( 4+3,sound.testIndexPosition( 1));
    EXPECT_EQ( 8+3,sound.testIndexPosition( 2));
    EXPECT_EQ(12+3,sound.testIndexPosition( 3));
    EXPECT_EQ(16+3,sound.testIndexPosition( 4));
    EXPECT_EQ(20+3,sound.testIndexPosition( 5));
    EXPECT_EQ(24+3,sound.testIndexPosition( 6));
    EXPECT_EQ(28+3,sound.testIndexPosition( 7));
    EXPECT_EQ(32+3,sound.testIndexPosition( 8));
    EXPECT_EQ(36+3,sound.testIndexPosition( 9));
    EXPECT_EQ(40+3,sound.testIndexPosition(10));
    EXPECT_EQ(44+3,sound.testIndexPosition(11));
    EXPECT_EQ(48+3,sound.testIndexPosition(12));
    EXPECT_EQ(52+3,sound.testIndexPosition(13));
    EXPECT_EQ(56+3,sound.testIndexPosition(14));
    EXPECT_EQ(60+3,sound.testIndexPosition(15));
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,indexPositionWithOffsetAndBlockSize)
{
    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.setOffset(5);
    sound.setBlockSize(12);

    EXPECT_EQ( 5,sound.testIndexPosition( 0));
    EXPECT_EQ( 9,sound.testIndexPosition( 1));
    EXPECT_EQ(13,sound.testIndexPosition( 2));
    EXPECT_EQ(17,sound.testIndexPosition( 3));
    EXPECT_EQ(21,sound.testIndexPosition( 4));
    EXPECT_EQ(29,sound.testIndexPosition( 5));
    EXPECT_EQ(33,sound.testIndexPosition( 6));
    EXPECT_EQ(37,sound.testIndexPosition( 7));
    EXPECT_EQ(41,sound.testIndexPosition( 8));
    EXPECT_EQ(45,sound.testIndexPosition( 9));
    EXPECT_EQ(53,sound.testIndexPosition(10));
    EXPECT_EQ(57,sound.testIndexPosition(11));
    EXPECT_EQ(61,sound.testIndexPosition(12));
    EXPECT_EQ(65,sound.testIndexPosition(13));
    EXPECT_EQ(69,sound.testIndexPosition(14));
    EXPECT_EQ(77,sound.testIndexPosition(15));
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,numberOfSamplesWhereContinuousAndSampleAndChunkSizeEquate)
{
    tubyte ssndMem[16] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x1C,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),16);
    common::BIOMemory file(arr);

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());

    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_EQ(5,pSound->numberOfSamples());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,numberOfSamplesWhereContinuousAndSampleGreaterThanChunkSize)
{
    tubyte ssndMem[16] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x1C,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    int noChannels = 2;
    int noSampleFrames = 7;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),16);
    common::BIOMemory file(arr);

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());

    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_EQ(5,pSound->numberOfSamples());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,numberOfSamplesWhereContinuousAndSampleLessThanChunkSize)
{
    tubyte ssndMem[16] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x1C,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    int noChannels = 2;
    int noSampleFrames = 3;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),16);
    common::BIOMemory file(arr);

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());

    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_EQ(3,pSound->numberOfSamples());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,numberOfSamplesWhereBlockAlignedAndFinishOnBlockAligned)
{
    tubyte ssndMem[16] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x6D,
        0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x0C
    };

    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),16);
    common::BIOMemory file(arr);

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());

    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_EQ(20,pSound->numberOfSamples());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,numberOfSamplesWhereBlockAlignedAndFinishOnLastSample)
{
    tubyte ssndMem[16] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x69,
        0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x0C
    };

    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),16);
    common::BIOMemory file(arr);

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());

    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_EQ(20,pSound->numberOfSamples());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,numberOfSamplesWhereBlockAlignedAndFinishInMiddleOfSection)
{
    tubyte ssndMem[16] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x61,
        0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x0C
    };

    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),16);
    common::BIOMemory file(arr);

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());

    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_EQ(18,pSound->numberOfSamples());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,nextIndexPositionWhereContinuous)
{
    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.setOffset(0);
    sound.setBlockSize(0);
    sound.setSize(8 + (16 * 4));

    sound.testSetCurrentIndexPosition(0);

    EXPECT_EQ( 0,sound.testNextIndexPosition()); // 0
    EXPECT_EQ( 4,sound.testNextIndexPosition()); // 1
    EXPECT_EQ( 8,sound.testNextIndexPosition()); // 2
    EXPECT_EQ(12,sound.testNextIndexPosition()); // 3
    EXPECT_EQ(16,sound.testNextIndexPosition()); // 4
    EXPECT_EQ(20,sound.testNextIndexPosition()); // 5
    EXPECT_EQ(24,sound.testNextIndexPosition()); // 6
    EXPECT_EQ(28,sound.testNextIndexPosition()); // 7
    EXPECT_EQ(32,sound.testNextIndexPosition()); // 8
    EXPECT_EQ(36,sound.testNextIndexPosition()); // 9
    EXPECT_EQ(40,sound.testNextIndexPosition()); // 10
    EXPECT_EQ(44,sound.testNextIndexPosition()); // 11
    EXPECT_EQ(48,sound.testNextIndexPosition()); // 12
    EXPECT_EQ(52,sound.testNextIndexPosition()); // 13
    EXPECT_EQ(56,sound.testNextIndexPosition()); // 14
    EXPECT_EQ(60,sound.testNextIndexPosition()); // 15
    EXPECT_EQ(-1,sound.testNextIndexPosition()); // eof

    sound.testSetCurrentIndexPosition(10);

    EXPECT_EQ(40,sound.testNextIndexPosition()); // 10
    EXPECT_EQ(44,sound.testNextIndexPosition()); // 11
    EXPECT_EQ(48,sound.testNextIndexPosition()); // 12
    EXPECT_EQ(52,sound.testNextIndexPosition()); // 13
    EXPECT_EQ(56,sound.testNextIndexPosition()); // 14
    EXPECT_EQ(60,sound.testNextIndexPosition()); // 15
    EXPECT_EQ(-1,sound.testNextIndexPosition()); // eof
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,nextIndexPositionWhereContinuousAndOffset)
{
    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.setOffset(3);
    sound.setBlockSize(0);
    sound.setSize(8 + (16 * 4));

    sound.testSetCurrentIndexPosition(0);

    EXPECT_EQ( 0+3,sound.testNextIndexPosition()); // 0
    EXPECT_EQ( 4+3,sound.testNextIndexPosition()); // 1
    EXPECT_EQ( 8+3,sound.testNextIndexPosition()); // 2
    EXPECT_EQ(12+3,sound.testNextIndexPosition()); // 3
    EXPECT_EQ(16+3,sound.testNextIndexPosition()); // 4
    EXPECT_EQ(20+3,sound.testNextIndexPosition()); // 5
    EXPECT_EQ(24+3,sound.testNextIndexPosition()); // 6
    EXPECT_EQ(28+3,sound.testNextIndexPosition()); // 7
    EXPECT_EQ(32+3,sound.testNextIndexPosition()); // 8
    EXPECT_EQ(36+3,sound.testNextIndexPosition()); // 9
    EXPECT_EQ(40+3,sound.testNextIndexPosition()); // 10
    EXPECT_EQ(44+3,sound.testNextIndexPosition()); // 11
    EXPECT_EQ(48+3,sound.testNextIndexPosition()); // 12
    EXPECT_EQ(52+3,sound.testNextIndexPosition()); // 13
    EXPECT_EQ(56+3,sound.testNextIndexPosition()); // 14
    EXPECT_EQ(60+3,sound.testNextIndexPosition()); // 15
    EXPECT_EQ(-1,sound.testNextIndexPosition()); // eof

    sound.testSetCurrentIndexPosition(10);

    EXPECT_EQ(40+3,sound.testNextIndexPosition()); // 10
    EXPECT_EQ(44+3,sound.testNextIndexPosition()); // 11
    EXPECT_EQ(48+3,sound.testNextIndexPosition()); // 12
    EXPECT_EQ(52+3,sound.testNextIndexPosition()); // 13
    EXPECT_EQ(56+3,sound.testNextIndexPosition()); // 14
    EXPECT_EQ(60+3,sound.testNextIndexPosition()); // 15
    EXPECT_EQ(-1,sound.testNextIndexPosition()); // eof
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,nextIndexPositionWithOffsetAndBlockSize)
{
    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.setOffset(5);
    sound.setBlockSize(12);
    sound.setSize(109);

    sound.testSetCurrentIndexPosition(0);

    EXPECT_EQ( 5,sound.testNextIndexPosition());
    EXPECT_EQ( 9,sound.testNextIndexPosition());
    EXPECT_EQ(13,sound.testNextIndexPosition());
    EXPECT_EQ(17,sound.testNextIndexPosition());
    EXPECT_EQ(21,sound.testNextIndexPosition());
    EXPECT_EQ(29,sound.testNextIndexPosition());
    EXPECT_EQ(33,sound.testNextIndexPosition());
    EXPECT_EQ(37,sound.testNextIndexPosition());
    EXPECT_EQ(41,sound.testNextIndexPosition());
    EXPECT_EQ(45,sound.testNextIndexPosition());
    EXPECT_EQ(53,sound.testNextIndexPosition());
    EXPECT_EQ(57,sound.testNextIndexPosition());
    EXPECT_EQ(61,sound.testNextIndexPosition());
    EXPECT_EQ(65,sound.testNextIndexPosition());
    EXPECT_EQ(69,sound.testNextIndexPosition());
    EXPECT_EQ(77,sound.testNextIndexPosition());
    EXPECT_EQ(81,sound.testNextIndexPosition());
    EXPECT_EQ(85,sound.testNextIndexPosition());
    EXPECT_EQ(89,sound.testNextIndexPosition());
    EXPECT_EQ(93,sound.testNextIndexPosition());
    EXPECT_EQ(-1,sound.testNextIndexPosition());

    sound.testSetCurrentIndexPosition(11);

    EXPECT_EQ(57,sound.testNextIndexPosition());
    EXPECT_EQ(61,sound.testNextIndexPosition());
    EXPECT_EQ(65,sound.testNextIndexPosition());
    EXPECT_EQ(69,sound.testNextIndexPosition());
    EXPECT_EQ(77,sound.testNextIndexPosition());
    EXPECT_EQ(81,sound.testNextIndexPosition());
    EXPECT_EQ(85,sound.testNextIndexPosition());
    EXPECT_EQ(89,sound.testNextIndexPosition());
    EXPECT_EQ(93,sound.testNextIndexPosition());
    EXPECT_EQ(-1,sound.testNextIndexPosition());
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readWhenContinous)
{
    tubyte ssndMem[52] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x2C, //  8
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 16
        0x80, 0x00, 0x90, 0x00, 0xA0, 0x00, 0xB0, 0x00, // 24 - 0, 1 -  0,  1,  2,  3
        0xC0, 0x00, 0xD0, 0x00, 0xE0, 0x00, 0xF0, 0x00, // 32 - 2, 3 -  4,  5,  6,  7
        0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x30, 0x00, // 40 - 4, 5 -  8,  9, 10, 11
        0x40, 0x00, 0x4F, 0xFF, 0x5F, 0xFF, 0x6F, 0xFF, // 48 - 6, 7 - 12, 13, 14, 15
        0x7F, 0xFF, 0x80, 0x00                          // 52 - 8    - 16, 17
    };

    int noChannels = 2;
    int noSampleFrames = 10;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    sample_t samples[20];

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),52);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());
    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_EQ(9,pSound->read(samples,9,engine::e_SampleFloat));

    EXPECT_NEAR(-1.000,samples[ 0],0.00001526);
    EXPECT_NEAR(-0.875,samples[ 1],0.00001526);
    EXPECT_NEAR(-0.750,samples[ 2],0.00001526);
    EXPECT_NEAR(-0.625,samples[ 3],0.00001526);
    EXPECT_NEAR(-0.500,samples[ 4],0.00001526);
    EXPECT_NEAR(-0.375,samples[ 5],0.00001526);
    EXPECT_NEAR(-0.250,samples[ 6],0.00001526);
    EXPECT_NEAR(-0.125,samples[ 7],0.00001526);
    EXPECT_NEAR( 0.000,samples[ 8],0.00001526);
    EXPECT_NEAR( 0.125,samples[ 9],0.00001526);
    EXPECT_NEAR( 0.250,samples[10],0.00001526);
    EXPECT_NEAR( 0.375,samples[11],0.00001526);
    EXPECT_NEAR( 0.500,samples[12],0.00001526);
    EXPECT_NEAR( 0.625,samples[13],0.00001526);
    EXPECT_NEAR( 0.750,samples[14],0.00001526);
    EXPECT_NEAR( 0.875,samples[15],0.00001526);
    EXPECT_NEAR( 1.000,samples[16],0.00001526);
    EXPECT_NEAR(-1.000,samples[17],0.00001526);

    EXPECT_EQ(0,pSound->read(samples,9,engine::e_SampleFloat));
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readWhenBlockAlignedAndOffset)
{
    tubyte ssndMem[61] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x35, //  8
        0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x0C, // 16
        0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0x80, 0x00, 0x90, // 24 - A
        0x00, 0xA0, 0x00, 0xB0, 0x00, 0xC0, 0x00, 0xD0, // 32 - B
        0x00, 0xE0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x10, // 40 - C
        0x00, 0xAB, 0xAB, 0xAB, 0xAB, 0x20, 0x00, 0x30, // 48 - D
        0x00, 0x40, 0x00, 0x4F, 0xFF, 0x5F, 0xFF, 0x6F, // 56 - E
        0xFF, 0x7F, 0xFF, 0x80, 0x00                    // 61 - F
    };

    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    sample_t samples[20];

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),61);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());
    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_EQ(9,pSound->read(samples,9,engine::e_SampleFloat));

    EXPECT_NEAR(-1.000,samples[ 0],0.00001526);
    EXPECT_NEAR(-0.875,samples[ 1],0.00001526);
    EXPECT_NEAR(-0.750,samples[ 2],0.00001526);
    EXPECT_NEAR(-0.625,samples[ 3],0.00001526);
    EXPECT_NEAR(-0.500,samples[ 4],0.00001526);
    EXPECT_NEAR(-0.375,samples[ 5],0.00001526);
    EXPECT_NEAR(-0.250,samples[ 6],0.00001526);
    EXPECT_NEAR(-0.125,samples[ 7],0.00001526);
    EXPECT_NEAR( 0.000,samples[ 8],0.00001526);
    EXPECT_NEAR( 0.125,samples[ 9],0.00001526);
    EXPECT_NEAR( 0.250,samples[10],0.00001526);
    EXPECT_NEAR( 0.375,samples[11],0.00001526);
    EXPECT_NEAR( 0.500,samples[12],0.00001526);
    EXPECT_NEAR( 0.625,samples[13],0.00001526);
    EXPECT_NEAR( 0.750,samples[14],0.00001526);
    EXPECT_NEAR( 0.875,samples[15],0.00001526);
    EXPECT_NEAR( 1.000,samples[16],0.00001526);
    EXPECT_NEAR(-1.000,samples[17],0.00001526);

    EXPECT_EQ(0,pSound->read(samples,9,engine::e_SampleFloat));
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,seekAndReadWhenContinous)
{
    tubyte ssndMem[52] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x2C, //  8
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 16
        0x80, 0x00, 0x90, 0x00, 0xA0, 0x00, 0xB0, 0x00, // 24 - 0, 1 -  0,  1,  2,  3
        0xC0, 0x00, 0xD0, 0x00, 0xE0, 0x00, 0xF0, 0x00, // 32 - 2, 3 -  4,  5,  6,  7
        0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x30, 0x00, // 40 - 4, 5 -  8,  9, 10, 11
        0x40, 0x00, 0x4F, 0xFF, 0x5F, 0xFF, 0x6F, 0xFF, // 48 - 6, 7 - 12, 13, 14, 15
        0x7F, 0xFF, 0x80, 0x00                          // 52 - 8    - 16, 17
    };

    int noChannels = 2;
    int noSampleFrames = 10;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    sample_t samples[8];

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),52);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());
    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_TRUE(pSound->seek(5));

    EXPECT_EQ(4,pSound->read(samples,4,engine::e_SampleFloat));

    EXPECT_NEAR( 0.250,samples[0],0.00001526);
    EXPECT_NEAR( 0.375,samples[1],0.00001526);
    EXPECT_NEAR( 0.500,samples[2],0.00001526);
    EXPECT_NEAR( 0.625,samples[3],0.00001526);
    EXPECT_NEAR( 0.750,samples[4],0.00001526);
    EXPECT_NEAR( 0.875,samples[5],0.00001526);
    EXPECT_NEAR( 1.000,samples[6],0.00001526);
    EXPECT_NEAR(-1.000,samples[7],0.00001526);

    EXPECT_EQ(0,pSound->read(samples,4,engine::e_SampleFloat));
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,seekAndReadWhenBlockAlignedAndOffset)
{
    tubyte ssndMem[61] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x35, //  8
        0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x0C, // 16
        0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0x80, 0x00, 0x90, // 24 - A
        0x00, 0xA0, 0x00, 0xB0, 0x00, 0xC0, 0x00, 0xD0, // 32 - B
        0x00, 0xE0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x10, // 40 - C
        0x00, 0xAB, 0xAB, 0xAB, 0xAB, 0x20, 0x00, 0x30, // 48 - D
        0x00, 0x40, 0x00, 0x4F, 0xFF, 0x5F, 0xFF, 0x6F, // 56 - E
        0xFF, 0x7F, 0xFF, 0x80, 0x00                    // 61 - F
    };

    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    sample_t samples[8];

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),61);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());
    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_TRUE(pSound->seek(5));

    EXPECT_EQ(4,pSound->read(samples,4,engine::e_SampleFloat));

    EXPECT_NEAR( 0.250,samples[0],0.00001526);
    EXPECT_NEAR( 0.375,samples[1],0.00001526);
    EXPECT_NEAR( 0.500,samples[2],0.00001526);
    EXPECT_NEAR( 0.625,samples[3],0.00001526);
    EXPECT_NEAR( 0.750,samples[4],0.00001526);
    EXPECT_NEAR( 0.875,samples[5],0.00001526);
    EXPECT_NEAR( 1.000,samples[6],0.00001526);
    EXPECT_NEAR(-1.000,samples[7],0.00001526);

    EXPECT_EQ(0,pSound->read(samples,4,engine::e_SampleFloat));
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,seekCannotGoOutOfBoundsWhenContinous)
{
    tubyte ssndMem[52] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x2C, //  8
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 16
        0x80, 0x00, 0x90, 0x00, 0xA0, 0x00, 0xB0, 0x00, // 24 - 0, 1 -  0,  1,  2,  3
        0xC0, 0x00, 0xD0, 0x00, 0xE0, 0x00, 0xF0, 0x00, // 32 - 2, 3 -  4,  5,  6,  7
        0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x30, 0x00, // 40 - 4, 5 -  8,  9, 10, 11
        0x40, 0x00, 0x4F, 0xFF, 0x5F, 0xFF, 0x6F, 0xFF, // 48 - 6, 7 - 12, 13, 14, 15
        0x7F, 0xFF, 0x80, 0x00                          // 52 - 8    - 16, 17
    };

    int noChannels = 2;
    int noSampleFrames = 10;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),52);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());
    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_FALSE(pSound->seek(-1));
    EXPECT_FALSE(pSound->seek(10));
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,seekCannotGoOutOfBoundsWhenBlockAlignedAndOffset)
{
    tubyte ssndMem[61] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x35, //  8
        0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x0C, // 16
        0xAB, 0xAB, 0xAB, 0xAB, 0xAB, 0x80, 0x00, 0x90, // 24 - A
        0x00, 0xA0, 0x00, 0xB0, 0x00, 0xC0, 0x00, 0xD0, // 32 - B
        0x00, 0xE0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x10, // 40 - C
        0x00, 0xAB, 0xAB, 0xAB, 0xAB, 0x20, 0x00, 0x30, // 48 - D
        0x00, 0x40, 0x00, 0x4F, 0xFF, 0x5F, 0xFF, 0x6F, // 56 - E
        0xFF, 0x7F, 0xFF, 0x80, 0x00                    // 61 - F
    };

    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),61);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());
    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_FALSE(pSound->seek(-1));
    EXPECT_FALSE(pSound->seek(10));
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,sortOutputChannelsGiven1Channel)
{
    int noChannels = 1;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    sample_t in[1] = { 0.25 };
    sample_t out[1];

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.testSortOutputChannels(in,out);

    EXPECT_EQ(1,sound.noOutChannels());

    // Type A = 0.Center
    EXPECT_NEAR(in[0],out[0],0.0000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,sortOutputChannelsGiven2Channels)
{
    int noChannels = 2;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    sample_t in[2] = { 0.25, 0.5 };
    sample_t out[2];

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.testSortOutputChannels(in,out);

    EXPECT_EQ(2,sound.noOutChannels());

    // Type B = 0.F-Left, 1.F-Right
    EXPECT_NEAR(in[0],out[0],0.0000001);
    EXPECT_NEAR(in[1],out[1],0.0000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,sortOutputChannelsGiven3Channels)
{
    int noChannels = 3;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    sample_t in[3] = { 0.25, 0.5, 0.75 };
    sample_t out[3];

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.testSortOutputChannels(in,out);

    EXPECT_EQ(3,sound.noOutChannels());

    // Type C = 0.F-Left, 1.F-Right, 2.Center
    EXPECT_NEAR(in[0],out[0],0.0000001); // I-Left
    EXPECT_NEAR(in[1],out[1],0.0000001); // I-Right
    EXPECT_NEAR(in[2],out[2],0.0000001); // I-Center
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,sortOutputChannelsGiven4Channels)
{
    int noChannels = 4;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    sample_t in[4] = { 0.1, 0.2, 0.3, 0.4 };
    sample_t out[4];

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.testSortOutputChannels(in,out);

    EXPECT_EQ(4,sound.noOutChannels());

    // Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
    EXPECT_NEAR(in[0],out[0],0.0000001); // I- Front Left
    EXPECT_NEAR(in[1],out[1],0.0000001); // I- Front Right
    EXPECT_NEAR(in[2],out[2],0.0000001); // I- Rear Left
    EXPECT_NEAR(in[3],out[3],0.0000001); // I- Rear Right
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,sortOutputChannelsGiven6Channels)
{
    int noChannels = 6;
    int noSampleFrames = 5;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    sample_t in[6] = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6 };
    sample_t out[7];

    IFFSoundChunkTest sound;
    sound.setCommon(pCommon);
    sound.testSortOutputChannels(in,out);

    EXPECT_EQ(7,sound.noOutChannels());

    // Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
    EXPECT_NEAR(in[1],out[0],0.0000001); // I- Left Center (1)
    EXPECT_NEAR(in[4],out[1],0.0000001); // I- Right Center (4)
    EXPECT_NEAR(in[2],out[2],0.0000001); // I- Center (2)
    EXPECT_NEAR(in[0],out[3],0.0000001); // I- Left (0)
    EXPECT_NEAR(in[3],out[4],0.0000001); // I- Right (3)
    EXPECT_NEAR(in[5],out[5],0.0000001); // I- Surround (5)
    EXPECT_NEAR(in[5],out[6],0.0000001); // I- Surround (5)
}

//-------------------------------------------------------------------------------------------

TEST(IFFSoundChunk,readWhenContinousWith6Channels)
{
    tubyte ssndMem[52] = {
        0x53, 0x53, 0x4E, 0x44, 0x00, 0x00, 0x00, 0x2C, //  8
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 16
        0x80, 0x00, 0x90, 0x00, 0xA0, 0x00, 0xB0, 0x00, // 24 -  0,  1,  2,  3
        0xC0, 0x00, 0xD0, 0x00, 0xE0, 0x00, 0xF0, 0x00, // 32 -  4,  5,  6,  7
        0x00, 0x00, 0x10, 0x00, 0x20, 0x00, 0x30, 0x00, // 40 -  8,  9, 10, 11
        0x40, 0x00, 0x4F, 0xFF, 0x5F, 0xFF, 0x6F, 0xFF, // 48 - 12, 13, 14, 15
        0x7F, 0xFF, 0x80, 0x00                          // 52 - 16, 17
    };

    int noChannels = 6;
    int noSampleFrames = 3;
    int sampleSize = 16;
    tfloat64 sampleRate = 44100.0;

    sample_t samples[21];

    IFFCommonChunkSPtr pCommon(new IFFMockCommonChunk());
    IFFMockCommonChunk& common = dynamic_cast<IFFMockCommonChunk&>(*(pCommon.data()));

    EXPECT_CALL(common,noChannels()).WillRepeatedly(ReturnRef(noChannels));
    EXPECT_CALL(common,noSampleFrames()).WillRepeatedly(ReturnRef(noSampleFrames));
    EXPECT_CALL(common,sampleSize()).WillRepeatedly(ReturnRef(sampleSize));
    EXPECT_CALL(common,sampleRate()).WillRepeatedly(ReturnRef(sampleRate));

    QByteArray arr(reinterpret_cast<const char *>(ssndMem),52);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());

    IFFSoundChunkSPtr pSound = pChunk.dynamicCast<IFFSoundChunk>();
    ASSERT_FALSE(pSound.isNull());
    pSound->setCommon(pCommon);
    EXPECT_TRUE(pSound->scan());

    EXPECT_EQ(3,pSound->read(samples,3,engine::e_SampleFloat));

    EXPECT_NEAR(-0.875,samples[ 0],0.00001526);
    EXPECT_NEAR(-0.500,samples[ 1],0.00001526);
    EXPECT_NEAR(-0.750,samples[ 2],0.00001526);
    EXPECT_NEAR(-1.000,samples[ 3],0.00001526);
    EXPECT_NEAR(-0.625,samples[ 4],0.00001526);
    EXPECT_NEAR(-0.375,samples[ 5],0.00001526);
    EXPECT_NEAR(-0.375,samples[ 6],0.00001526);

    EXPECT_NEAR(-0.125,samples[ 7],0.00001526); // 1
    EXPECT_NEAR( 0.250,samples[ 8],0.00001526); // 4
    EXPECT_NEAR( 0.000,samples[ 9],0.00001526); // 2
    EXPECT_NEAR(-0.250,samples[10],0.00001526); // 0
    EXPECT_NEAR( 0.125,samples[11],0.00001526); // 3
    EXPECT_NEAR( 0.375,samples[12],0.00001526); // 5
    EXPECT_NEAR( 0.375,samples[13],0.00001526); // 5

    EXPECT_NEAR( 0.625,samples[14],0.00001526); // 1
    EXPECT_NEAR( 1.000,samples[15],0.00001526); // 4
    EXPECT_NEAR( 0.750,samples[16],0.00001526); // 2
    EXPECT_NEAR( 0.500,samples[17],0.00001526); // 0
    EXPECT_NEAR( 0.875,samples[18],0.00001526); // 3
    EXPECT_NEAR(-1.000,samples[19],0.00001526); // 5
    EXPECT_NEAR(-1.000,samples[20],0.00001526); // 5

    EXPECT_EQ(0,pSound->read(samples,3,engine::e_SampleFloat));
}

//-------------------------------------------------------------------------------------------
