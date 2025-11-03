#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/BIOMemory.h"
#include "engine/violetomega/inc/IFFFile.h"
#include "engine/violetomega/inc/IFFTextChunk.h"

using namespace testing;
using namespace omega;
using namespace omega::engine::violetomega;

//-------------------------------------------------------------------------------------------

TEST(IFFTextChunk,createFromFactoryAndScanNameChunkForEmptyString)
{
    const tubyte chunkHdr[8] = { 0x4E, 0x41, 0x4D, 0x45, 0x00, 0x00, 0x00, 0x00 };

    QByteArray arr(reinterpret_cast<const char *>(chunkHdr),8);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_TRUE(!pChunk.isNull());

    IFFTextChunkSPtr pText = pChunk.dynamicCast<IFFTextChunk>();
    ASSERT_TRUE(!pText.isNull());
    ASSERT_TRUE(pText->scan());

    EXPECT_TRUE(pText->text().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(IFFTextChunk,createFromFactoryAndScanNameChunk)
{
    const tubyte chunkHdr[8] = { 0x4E, 0x41, 0x4D, 0x45, 0x00, 0x00, 0x00, 0x12 };
    const char text[] = "Piece of test text";

    QByteArray arr(reinterpret_cast<const char *>(chunkHdr),8);
    arr.append(text,18);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_TRUE(!pChunk.isNull());

    IFFTextChunkSPtr pText = pChunk.dynamicCast<IFFTextChunk>();
    ASSERT_TRUE(!pText.isNull());
    ASSERT_TRUE(pText->scan());

    EXPECT_TRUE(pText->text()==QString::fromLatin1(text,18));
}

//-------------------------------------------------------------------------------------------

TEST(IFFTextChunk,createFromFactoryAndScanAuthorChunk)
{
    const tubyte chunkHdr[8] = { 0x41, 0x55, 0x54, 0x48, 0x00, 0x00, 0x00, 0x12 };
    const char text[] = "Piece of test text";

    QByteArray arr(reinterpret_cast<const char *>(chunkHdr),8);
    arr.append(text,18);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_TRUE(!pChunk.isNull());

    IFFTextChunkSPtr pText = pChunk.dynamicCast<IFFTextChunk>();
    ASSERT_TRUE(!pText.isNull());
    ASSERT_TRUE(pText->scan());

    EXPECT_TRUE(pText->text()==QString::fromLatin1(text,18));
}

//-------------------------------------------------------------------------------------------

TEST(IFFTextChunk,createFromFactoryAndScanCopyrightChunk)
{
    const tubyte chunkHdr[8] = { 0x28, 0x63, 0x29, 0x20, 0x00, 0x00, 0x00, 0x12 };
    const char text[] = "Piece of test text";

    QByteArray arr(reinterpret_cast<const char *>(chunkHdr),8);
    arr.append(text,18);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_TRUE(!pChunk.isNull());

    IFFTextChunkSPtr pText = pChunk.dynamicCast<IFFTextChunk>();
    ASSERT_TRUE(!pText.isNull());
    ASSERT_TRUE(pText->scan());

    EXPECT_TRUE(pText->text()==QString::fromLatin1(text,18));
}

//-------------------------------------------------------------------------------------------

TEST(IFFTextChunk,createFromFactoryAndScanAnnotationChunk)
{
    const tubyte chunkHdr[8] = { 0x41, 0x4E, 0x4E, 0x4F, 0x00, 0x00, 0x00, 0x12 };
    const char text[] = "Piece of test text";

    QByteArray arr(reinterpret_cast<const char *>(chunkHdr),8);
    arr.append(text,18);
    common::BIOMemory file(arr);

    IFFChunkSPtr pChunk = IFFFile::createFromFactory(&file,IFFChunk::e_EndianBig);
    ASSERT_TRUE(!pChunk.isNull());

    IFFTextChunkSPtr pText = pChunk.dynamicCast<IFFTextChunk>();
    ASSERT_TRUE(!pText.isNull());
    ASSERT_TRUE(pText->scan());

    EXPECT_TRUE(pText->text()==QString::fromLatin1(text,18));
}

//-------------------------------------------------------------------------------------------
