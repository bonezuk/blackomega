#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "engine/violetomega/inc/IFFCommonChunk.h"
#include "engine/violetomega/inc/IFFFile.h"
#include "common/inc/BIOMemory.h"

using namespace testing;
using namespace omega;
using namespace omega::engine::violetomega;

//-------------------------------------------------------------------------------------------

TEST(IFFCommonChunk,createFromFactoryAndScan)
{
    tubyte commMem[26] = {
        0x43, 0x4F, 0x4D, 0x4D, 0x00, 0x00, 0x00, 0x12,
        0x00, 0x02, 0x00, 0x01, 0x0A, 0x9F, 0x00, 0x10,
        0x40, 0x0E, 0xAC, 0x44, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    };
    
    QByteArray arr(reinterpret_cast<const char *>(commMem),26);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());
    
    IFFCommonChunkSPtr pCommon = pChunk.dynamicCast<IFFCommonChunk>();
    ASSERT_FALSE(pCommon.isNull());
    
    EXPECT_TRUE(pCommon->scan());
    
    EXPECT_EQ(2,pCommon->noChannels());
    EXPECT_EQ(68255,pCommon->noSampleFrames());
    EXPECT_EQ(16,pCommon->sampleSize());
    EXPECT_NEAR(44100.0,pCommon->sampleRate(),0.0000001);
}

//-------------------------------------------------------------------------------------------

TEST(IFFCommonChunk,createFromFactoryAndScanWhen0BitSampleSize)
{
    tubyte commMem[26] = {
        0x43, 0x4F, 0x4D, 0x4D, 0x00, 0x00, 0x00, 0x12,
        0x00, 0x02, 0x00, 0x01, 0x0A, 0x9F, 0x00, 0x00,
        0x40, 0x0E, 0xAC, 0x44, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    };
    
    QByteArray arr(reinterpret_cast<const char *>(commMem),26);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());
    
    IFFCommonChunkSPtr pCommon = pChunk.dynamicCast<IFFCommonChunk>();
    ASSERT_FALSE(pCommon.isNull());
    
    EXPECT_FALSE(pCommon->scan());
}

//-------------------------------------------------------------------------------------------

TEST(IFFCommonChunk,createFromFactoryAndScanWhen33BitSampleSize)
{
    tubyte commMem[26] = {
        0x43, 0x4F, 0x4D, 0x4D, 0x00, 0x00, 0x00, 0x12,
        0x00, 0x02, 0x00, 0x01, 0x0A, 0x9F, 0x00, 0x21,
        0x40, 0x0E, 0xAC, 0x44, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    };
    
    QByteArray arr(reinterpret_cast<const char *>(commMem),26);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());
    
    IFFCommonChunkSPtr pCommon = pChunk.dynamicCast<IFFCommonChunk>();
    ASSERT_FALSE(pCommon.isNull());
    
    EXPECT_FALSE(pCommon->scan());
}

//-------------------------------------------------------------------------------------------

TEST(IFFCommonChunk,createFromFactoryAndScanGivenTooSmallForCOMM)
{
    tubyte commMem[25] = {
        0x43, 0x4F, 0x4D, 0x4D, 0x00, 0x00, 0x00, 0x11,
        0x00, 0x02, 0x00, 0x01, 0x0A, 0x9F, 0x00, 0x10,
        0x40, 0x0E, 0xAC, 0x44, 0x00, 0x00, 0x00, 0x00,
        0x00
    };
    
    QByteArray arr(reinterpret_cast<const char *>(commMem),25);
    common::BIOMemory file(arr);
    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_FALSE(pChunk.isNull());
    
    IFFCommonChunkSPtr pCommon = pChunk.dynamicCast<IFFCommonChunk>();
    ASSERT_FALSE(pCommon.isNull());
    
    EXPECT_FALSE(pCommon->scan());
}

//-------------------------------------------------------------------------------------------
