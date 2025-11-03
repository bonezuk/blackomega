#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "network/inc/Resource.h"
#include "common/inc/BIOMemory.h"
#include "engine/violetomega/inc/IFFFile.h"
#include "engine/violetomega/inc/IFFFormChunk.h"
#include "common/inc/DiskOps.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace testing;
using namespace omega;
using namespace omega::engine::violetomega;

//-------------------------------------------------------------------------------------------

class IFFFileTest : public IFFFile
{
    public:
        IFFFileTest(common::BIOStream *file);
        virtual ~IFFFileTest();

        static IFFChunkSPtr testCreateFromFactory(common::BIOStream *file,IFFChunk::EndianType type);
};

//-------------------------------------------------------------------------------------------

IFFFileTest::IFFFileTest(common::BIOStream *file) : IFFFile(file)
{}

//-------------------------------------------------------------------------------------------

IFFFileTest::~IFFFileTest()
{}

//-------------------------------------------------------------------------------------------

IFFChunkSPtr IFFFileTest::testCreateFromFactory(common::BIOStream *file,IFFChunk::EndianType type)
{
    return createFromFactory(file,type);
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,readSampleAIFFFile)
{
    QString fileName = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"sample1.aiff");

    common::BIOStream ioFile(common::e_BIOStream_FileRead);
    ASSERT_TRUE(ioFile.open(fileName));

    IFFFile iffFile(&ioFile);

    IFFChunkSPtr pRoot = iffFile.root();
    ASSERT_TRUE(pRoot.data()!=0);
    EXPECT_TRUE(pRoot->id()==IFF_ID('F','O','R','M'));
    EXPECT_TRUE(pRoot->size()==0x00042ba2);

    IFFFormChunkSPtr pFormChunk = pRoot.dynamicCast<IFFFormChunk>();
    ASSERT_TRUE(pFormChunk.data()!=0);
    EXPECT_TRUE(pFormChunk->formType()==IFF_ID('A','I','F','F'));

    IFFFormChunk::Iterator ppI = pFormChunk->begin();
    EXPECT_TRUE(ppI!=pFormChunk->end());

    IFFChunkSPtr pCommChunk = *ppI;
    ASSERT_TRUE(pCommChunk.data()!=0);
    EXPECT_TRUE(ppI!=pFormChunk->end());
    EXPECT_TRUE(pCommChunk->id()==IFF_ID('C','O','M','M'));
    EXPECT_TRUE(pCommChunk->size()==0x00000012);
    ppI++;

    IFFChunkSPtr pNameChunk = *ppI;
    ASSERT_TRUE(pNameChunk.data()!=0);
    EXPECT_TRUE(ppI!=pFormChunk->end());
    EXPECT_TRUE(pNameChunk->id()==IFF_ID('N','A','M','E'));
    EXPECT_TRUE(pNameChunk->size()==0x0000001a);
    ppI++;

    IFFChunkSPtr pAuthChunk = *ppI;
    ASSERT_TRUE(pAuthChunk.data()!=0);
    EXPECT_TRUE(ppI!=pFormChunk->end());
    EXPECT_TRUE(pAuthChunk->id()==IFF_ID('A','U','T','H'));
    EXPECT_TRUE(pAuthChunk->size()==0x0000000c);
    ppI++;

    IFFChunkSPtr pAnnoChunk = *ppI;
    ASSERT_TRUE(pAnnoChunk.data()!=0);
    EXPECT_TRUE(ppI!=pFormChunk->end());
    EXPECT_TRUE(pAnnoChunk->id()==IFF_ID('A','N','N','O'));
    EXPECT_TRUE(pAnnoChunk->size()==0x0000000e);
    ppI++;

    IFFChunkSPtr pSampleChunk = *ppI;
    ASSERT_TRUE(pSampleChunk.data()!=0);
    EXPECT_TRUE(ppI!=pFormChunk->end());
    EXPECT_TRUE(pSampleChunk->id()==IFF_ID('S','S','N','D'));
    EXPECT_TRUE(pSampleChunk->size()==0x00042a84);
    ppI++;

    IFFChunkSPtr pID3Chunk = *ppI;
    ASSERT_TRUE(pID3Chunk.data()!=0);
    EXPECT_TRUE(ppI!=pFormChunk->end());
    EXPECT_TRUE(pID3Chunk->id()==IFF_ID('I','D','3',' '));
    EXPECT_TRUE(pID3Chunk->size()==0x000000a4);
    ppI++;

    IFFChunkSPtr pNullChunk = *ppI;
    EXPECT_TRUE(pNullChunk.data()==0);
    EXPECT_TRUE(ppI==pFormChunk->end());

    ioFile.close();
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithNoFile)
{
    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(0,IFFChunk::e_EndianUnknown);
    EXPECT_TRUE(pChunk.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithFailedRead)
{
    tubyte mem[3] = { 0x46, 0x4F, 0x52 };

    QByteArray arr(reinterpret_cast<const char *>(mem),3);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(&file,IFFChunk::e_EndianUnknown);

    EXPECT_TRUE(pChunk.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithFailedReadHeaderOnFactoryCreated)
{
    tubyte mem[7] = {
        0x46, 0x4F, 0x52, 0x4D,
        0x04, 0x00, 0x00
    };

    QByteArray arr(reinterpret_cast<const char *>(mem),7);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(&file,IFFChunk::e_EndianBig);

    EXPECT_TRUE(pChunk.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithFailedReadHeaderOnBaseCreated)
{
    tubyte mem[7] = {
        0x51, 0x57, 0x54, 0x50,
        0x04, 0x00, 0x00
    };

    QByteArray arr(reinterpret_cast<const char *>(mem),7);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(&file,IFFChunk::e_EndianBig);

    EXPECT_TRUE(pChunk.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithFormBigEndianWhenUnknownType)
{
    // 0123
    // FORM - big endian
    tubyte mem[12] = {
        0x46, 0x4F, 0x52, 0x4D,
        0x00, 0x00, 0x00, 0x04,
        0x01, 0x02, 0x03, 0x04
    };

    QByteArray arr(reinterpret_cast<const char *>(mem),12);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(&file,IFFChunk::e_EndianUnknown);

    EXPECT_FALSE(pChunk.isNull());

    EXPECT_TRUE(pChunk->id()==IFF_ID('F','O','R','M'));
    EXPECT_TRUE(pChunk->size()==4);

    IFFFormChunkSPtr pForm = pChunk.dynamicCast<IFFFormChunk>();
    EXPECT_FALSE(pForm.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithFormBigEndianWhenBigType)
{
    // 0123
    // FORM - big endian
    tubyte mem[12] = {
        0x46, 0x4F, 0x52, 0x4D,
        0x00, 0x00, 0x00, 0x04,
        0x01, 0x02, 0x03, 0x04
    };

    QByteArray arr(reinterpret_cast<const char *>(mem),12);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(&file,IFFChunk::e_EndianBig);

    EXPECT_FALSE(pChunk.isNull());

    EXPECT_TRUE(pChunk->id()==IFF_ID('F','O','R','M'));
    EXPECT_TRUE(pChunk->size()==4);

    IFFFormChunkSPtr pForm = pChunk.dynamicCast<IFFFormChunk>();
    EXPECT_FALSE(pForm.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithFormLittleEndianWhenUnknownType)
{
    // 3210
    // MROF - little endian
    tubyte mem[12] = {
        0x4D, 0x52, 0x4F, 0x46,
        0x04, 0x00, 0x00, 0x00,
        0x04, 0x03, 0x02, 0x01
    };

    QByteArray arr(reinterpret_cast<const char *>(mem),12);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(&file,IFFChunk::e_EndianUnknown);

    EXPECT_FALSE(pChunk.isNull());

    EXPECT_TRUE(pChunk->id()==IFF_ID('F','O','R','M'));
    EXPECT_TRUE(pChunk->size()==4);

    IFFFormChunkSPtr pForm = pChunk.dynamicCast<IFFFormChunk>();
    EXPECT_FALSE(pForm.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithFormLittleEndianWhenLittleType)
{
    // 3210
    // MROF - little endian
    tubyte mem[12] = {
        0x4D, 0x52, 0x4F, 0x46,
        0x04, 0x00, 0x00, 0x00,
        0x04, 0x03, 0x02, 0x01
    };

    QByteArray arr(reinterpret_cast<const char *>(mem),12);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(&file,IFFChunk::e_EndianLittle);

    EXPECT_FALSE(pChunk.isNull());

    EXPECT_TRUE(pChunk->id()==IFF_ID('F','O','R','M'));
    EXPECT_TRUE(pChunk->size()==4);

    IFFFormChunkSPtr pForm = pChunk.dynamicCast<IFFFormChunk>();
    EXPECT_FALSE(pForm.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithUnknownChunkTypeWhenBigEndianType)
{
    tubyte mem[12] = {
        0x51, 0x57, 0x54, 0x50,
        0x00, 0x00, 0x00, 0x04,
        0x01, 0x02, 0x03, 0x04
    };

    QByteArray arr(reinterpret_cast<const char *>(mem),12);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(&file,IFFChunk::e_EndianBig);

    EXPECT_FALSE(pChunk.isNull());

    EXPECT_TRUE(pChunk->id()==IFF_ID('Q','W','T','P'));
    EXPECT_TRUE(pChunk->size()==4);
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithUnknownChunkTypeWhenLittleEndianType)
{
    tubyte mem[12] = {
        0x50, 0x54, 0x57, 0x51,
        0x04, 0x00, 0x00, 0x00,
        0x04, 0x03, 0x02, 0x01
    };

    QByteArray arr(reinterpret_cast<const char *>(mem),12);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(&file,IFFChunk::e_EndianLittle);

    EXPECT_FALSE(pChunk.isNull());

    EXPECT_TRUE(pChunk->id()==IFF_ID('Q','W','T','P'));
    EXPECT_TRUE(pChunk->size()==4);
}

//-------------------------------------------------------------------------------------------

TEST(IFFFile,createFromFactoryWithUnknownChunkTypeWhenUnknownEndianType)
{
    tubyte mem[12] = {
        0x51, 0x57, 0x54, 0x50,
        0x00, 0x00, 0x00, 0x04,
        0x01, 0x02, 0x03, 0x04
    };

    QByteArray arr(reinterpret_cast<const char *>(mem),12);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFileTest::testCreateFromFactory(&file,IFFChunk::e_EndianUnknown);

    EXPECT_FALSE(pChunk.isNull());

    EXPECT_TRUE(pChunk->id()==IFF_ID('Q','W','T','P'));
    EXPECT_TRUE(pChunk->size()==4);
}

//-------------------------------------------------------------------------------------------
