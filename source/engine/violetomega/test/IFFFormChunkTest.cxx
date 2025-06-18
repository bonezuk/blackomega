#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "engine/violetomega/inc/IFFFormChunk.h"
#include "common/inc/BIOMemory.h"

using namespace testing;
using namespace omega;
using namespace omega::engine::violetomega;

//-------------------------------------------------------------------------------------------

class IFFFormChunkTest : public IFFFormChunk
{
    public:
        IFFFormChunkTest(QVector<IFFChunkSPtr>& chunks);
        virtual ~IFFFormChunkTest();
};

//-------------------------------------------------------------------------------------------

IFFFormChunkTest::IFFFormChunkTest(QVector<IFFChunkSPtr>& chunks) : IFFFormChunk()
{
    m_chunks = chunks;
}

//-------------------------------------------------------------------------------------------

IFFFormChunkTest::~IFFFormChunkTest()
{}

//-------------------------------------------------------------------------------------------

TEST(IFFFormChunk,emptyForm)
{
    IFFFormChunk form;
    
    EXPECT_TRUE(form.isEmpty());
    EXPECT_TRUE(form.numberOfChunks()==0);
    EXPECT_TRUE(form.at(0).isNull());
    
    EXPECT_TRUE(form.begin()==form.end());
    
    IFFFormChunk::Iterator ppI;
    ppI = form.begin();
    EXPECT_TRUE((*ppI).isNull());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFormChunk,chunkAccessViaMethods)
{
    IFFChunkSPtr chunkA(new IFFChunk);
    IFFChunkSPtr chunkB(new IFFChunk);
    IFFChunkSPtr chunkC(new IFFChunk);
    
    QVector<IFFChunkSPtr> chunkList;
    chunkList.append(chunkA);
    chunkList.append(chunkB);
    chunkList.append(chunkC);
    
    IFFFormChunkTest form(chunkList);
    
    EXPECT_FALSE(form.isEmpty());
    EXPECT_TRUE(form.numberOfChunks()==3);
    
    EXPECT_TRUE(form.at(0).data()==chunkA.data());
    EXPECT_TRUE(form.at(1).data()==chunkB.data());
    EXPECT_TRUE(form.at(2).data()==chunkC.data());
    
    EXPECT_TRUE(form.at(-1).isNull());
    EXPECT_TRUE(form.at(3).isNull());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFormChunk,chunkAccessViaIterator)
{
    IFFChunkSPtr chunkA(new IFFChunk);
    IFFChunkSPtr chunkB(new IFFChunk);
    IFFChunkSPtr chunkC(new IFFChunk);
    
    QVector<IFFChunkSPtr> chunkList;
    chunkList.append(chunkA);
    chunkList.append(chunkB);
    chunkList.append(chunkC);
    
    IFFFormChunkTest form(chunkList);
    
    IFFFormChunk::Iterator ppI = form.begin();
    
    EXPECT_TRUE(ppI!=form.end());
    EXPECT_TRUE(ppI.index()==0);
    EXPECT_TRUE((*ppI).data()==chunkA.data());
    ppI++;

    EXPECT_TRUE(ppI!=form.end());
    EXPECT_TRUE(ppI.index()==1);
    EXPECT_TRUE((*ppI).data()==chunkB.data());
    ppI++;

    EXPECT_TRUE(ppI!=form.end());
    EXPECT_TRUE(ppI.index()==2);
    EXPECT_TRUE((*ppI).data()==chunkC.data());
    ppI++;

    EXPECT_TRUE(ppI==form.end());
    EXPECT_TRUE((*ppI).isNull());
    ppI++;
}

//-------------------------------------------------------------------------------------------

TEST(IFFFormChunk,scanFormWithNoFormType)
{
    tubyte chunkF[8] = { 0x46, 0x4F, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x00 };
    
    QByteArray arr(reinterpret_cast<const char *>(chunkF),8);
    
    common::BIOMemory file(arr);
    
    IFFFormChunkSPtr pForm = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig).dynamicCast<IFFFormChunk>();
    ASSERT_FALSE(pForm.isNull());
    
    ASSERT_FALSE(pForm->scan());
}

//-------------------------------------------------------------------------------------------

TEST(IFFFormChunk,scanFormWithNoChunks)
{
    tubyte chunkF[12] = { 0x46, 0x4F, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x04, 0x41, 0x49, 0x46, 0x46 };
    
    QByteArray arr(reinterpret_cast<const char *>(chunkF),12);
    
    common::BIOMemory file(arr);
    
    IFFFormChunkSPtr pForm = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig).dynamicCast<IFFFormChunk>();
    ASSERT_FALSE(pForm.isNull());
    
    ASSERT_TRUE(pForm->scan());
    
    EXPECT_TRUE(pForm->formType()==IFF_ID('A','I','F','F'));
    EXPECT_TRUE(pForm->numberOfChunks()==0);
}

//-------------------------------------------------------------------------------------------

TEST(IFFFormChunk,scanFormWith1Chunk)
{
    //      FORM
    // C1 - ABCD, 4 bytes
    
    tubyte chunkF[12] = { 0x46, 0x4F, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x10, 0x41, 0x49, 0x46, 0x46 };
    tubyte chunk1[12] = { 0x41, 0x42, 0x43, 0x44, 0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04 };
    
    QByteArray arr(reinterpret_cast<const char *>(chunkF),12);
    arr.append(reinterpret_cast<const char *>(chunk1),12);
    
    common::BIOMemory file(arr);
    
    IFFFormChunkSPtr pForm = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig).dynamicCast<IFFFormChunk>();
    ASSERT_FALSE(pForm.isNull());
    
    ASSERT_TRUE(pForm->scan());
    
    EXPECT_TRUE(pForm->formType()==IFF_ID('A','I','F','F'));
    EXPECT_TRUE(pForm->numberOfChunks()==1);
    
    EXPECT_TRUE(pForm->at(0)->id()==IFF_ID('A','B','C','D'));
    EXPECT_TRUE(pForm->at(0)->size()==4);
}

//-------------------------------------------------------------------------------------------

TEST(IFFFormChunk,scanFormWith3Chunks)
{
    //      FORM
    // C1 - ABCD, 4 bytes
    // C2 - EFGH, 8 bytes
    // C3 - IJKL, 4 bytes
    
    tubyte chunkF[12] = { 0x46, 0x4F, 0x52, 0x4D, 0x00, 0x00, 0x00, 0x2C, 0x41, 0x49, 0x46, 0x46 };
    tubyte chunk1[12] = { 0x41, 0x42, 0x43, 0x44, 0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04 };
    tubyte chunk2[16] = { 0x45, 0x46, 0x47, 0x48, 0x00, 0x00, 0x00, 0x08, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    tubyte chunk3[12] = { 0x49, 0x4A, 0x4B, 0x4C, 0x00, 0x00, 0x00, 0x04, 0x01, 0x02, 0x03, 0x04 };
    
    QByteArray arr(reinterpret_cast<const char *>(chunkF),12);
    arr.append(reinterpret_cast<const char *>(chunk1),12);
    arr.append(reinterpret_cast<const char *>(chunk2),16);
    arr.append(reinterpret_cast<const char *>(chunk3),12);
    
    common::BIOMemory file(arr);
    
    IFFFormChunkSPtr pForm = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig).dynamicCast<IFFFormChunk>();
    ASSERT_FALSE(pForm.isNull());
    
    ASSERT_TRUE(pForm->scan());
    
    EXPECT_TRUE(pForm->formType()==IFF_ID('A','I','F','F'));
    EXPECT_TRUE(pForm->numberOfChunks()==3);
    
    EXPECT_TRUE(pForm->at(0)->id()==IFF_ID('A','B','C','D'));
    EXPECT_TRUE(pForm->at(0)->size()==4);

    EXPECT_TRUE(pForm->at(1)->id()==IFF_ID('E','F','G','H'));
    EXPECT_TRUE(pForm->at(1)->size()==8);

    EXPECT_TRUE(pForm->at(2)->id()==IFF_ID('I','J','K','L'));
    EXPECT_TRUE(pForm->at(2)->size()==4);
}

//-------------------------------------------------------------------------------------------

TEST(IFFFormChunk,scanFormWhenSizeTooSmall)
{
    //      FORM
    // C1 - ABCD, 4 bytes
    // C2 - EFGH, 8 bytes
    // C3 - IJKL, 4 bytes
    
    tubyte chunkF[12] = { 0x46, 0x4F, 0x52, 0x4D, 0x2B, 0x00, 0x00, 0x00, 0x41, 0x49, 0x46, 0x46 };
    tubyte chunk1[12] = { 0x41, 0x42, 0x43, 0x44, 0x04, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04 };
    tubyte chunk2[16] = { 0x45, 0x46, 0x47, 0x48, 0x08, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    tubyte chunk3[12] = { 0x49, 0x50, 0x51, 0x52, 0x04, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04 };
    
    QByteArray arr(reinterpret_cast<const char *>(chunkF),12);
    arr.append(reinterpret_cast<const char *>(chunk1),12);
    arr.append(reinterpret_cast<const char *>(chunk2),16);
    arr.append(reinterpret_cast<const char *>(chunk3),12);
    
    common::BIOMemory file(arr);
    
    IFFFormChunkSPtr pForm = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig).dynamicCast<IFFFormChunk>();
    ASSERT_FALSE(pForm.isNull());
    
    EXPECT_FALSE(pForm->scan());    
}

//-------------------------------------------------------------------------------------------

TEST(IFFFormChunk,scanFormWhenSizeTooLarge)
{
    //      FORM
    // C1 - ABCD, 4 bytes
    // C2 - EFGH, 8 bytes
    // C3 - IJKL, 4 bytes
    
    tubyte chunkF[12] = { 0x46, 0x4F, 0x52, 0x4D, 0x2D, 0x00, 0x00, 0x00, 0x41, 0x49, 0x46, 0x46 };
    tubyte chunk1[12] = { 0x41, 0x42, 0x43, 0x44, 0x04, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04 };
    tubyte chunk2[16] = { 0x45, 0x46, 0x47, 0x48, 0x08, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
    tubyte chunk3[12] = { 0x49, 0x50, 0x51, 0x52, 0x04, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04 };
    
    QByteArray arr(reinterpret_cast<const char *>(chunkF),12);
    arr.append(reinterpret_cast<const char *>(chunk1),12);
    arr.append(reinterpret_cast<const char *>(chunk2),16);
    arr.append(reinterpret_cast<const char *>(chunk3),12);
    
    common::BIOMemory file(arr);
    
    IFFFormChunkSPtr pForm = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig).dynamicCast<IFFFormChunk>();
    ASSERT_FALSE(pForm.isNull());
    
    EXPECT_FALSE(pForm->scan());        
}

//-------------------------------------------------------------------------------------------
