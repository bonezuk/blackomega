#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/test/BIOStreamMock.h"
#include "engine/violetomega/inc/IFFChunk.h"

using namespace testing;
using namespace omega;
using namespace omega::engine::violetomega;

//-------------------------------------------------------------------------------------------

class IFFChunkTest : public IFFChunk
{
    public:
        IFFChunkTest();
        IFFChunkTest(common::BIOStream *file,bool littleEndian);
        IFFChunkTest(IFFID id,common::BIOStream *file);
        
        common::BIOStream *getFile();
        bool isLittleEndian();
        
        tuint32 testRead32BitUnsigned(const tbyte *mem) const;
        tint32 testRead32BitSigned(const tbyte *mem) const;
        tuint16 testRead16BitUnsigned(const tbyte *mem) const;
        tint16 testRead16BitSigned(const tbyte *mem) const;
};

//-------------------------------------------------------------------------------------------

IFFChunkTest::IFFChunkTest() : IFFChunk()
{}

//-------------------------------------------------------------------------------------------

IFFChunkTest::IFFChunkTest(common::BIOStream *file,bool littleEndian) : IFFChunk(file,littleEndian)
{}

//-------------------------------------------------------------------------------------------

IFFChunkTest::IFFChunkTest(IFFID id,common::BIOStream *file) : IFFChunk(id,file)
{}

//-------------------------------------------------------------------------------------------

common::BIOStream *IFFChunkTest::getFile()
{
    return m_file;
}

//-------------------------------------------------------------------------------------------

bool IFFChunkTest::isLittleEndian()
{
    return m_littleEndian;
}

//-------------------------------------------------------------------------------------------

tuint32 IFFChunkTest::testRead32BitUnsigned(const tbyte *mem) const
{
    return read32BitUnsigned(mem);
}

//-------------------------------------------------------------------------------------------

tint32 IFFChunkTest::testRead32BitSigned(const tbyte *mem) const
{
    return read32BitSigned(mem);
}

//-------------------------------------------------------------------------------------------

tuint16 IFFChunkTest::testRead16BitUnsigned(const tbyte *mem) const
{
    return read16BitUnsigned(mem);
}

//-------------------------------------------------------------------------------------------

tint16 IFFChunkTest::testRead16BitSigned(const tbyte *mem) const
{
    return read16BitSigned(mem);
}

//-------------------------------------------------------------------------------------------

#define IFF_CHUNK_TEST_ID IFF_ID(0x4f,0x5e,0x6d,0x7c)

//-------------------------------------------------------------------------------------------

class IFFChunkReadHeaderFile
{
    public:
        IFFChunkReadHeaderFile(tubyte *mem,int size);
        tint read(tbyte *mem,tint len);
    protected:
        tubyte *m_mem;
        int m_size;
};

IFFChunkReadHeaderFile::IFFChunkReadHeaderFile(tubyte *mem,int size) : m_mem(mem),
    m_size(size)
{}

tint IFFChunkReadHeaderFile::read(tbyte *mem,tint len)
{
    if(m_size>0)
    {
        ::memcpy(mem,m_mem,m_size);
    }
    return m_size;
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,readHeaderGivenFailureToReadHeader)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,read(A<tbyte*>(),8)).Times(1).WillOnce(Return(7));
    
    IFFChunk chunk(&file,true);
    
    EXPECT_FALSE(chunk.readHeader());    
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,readHeaderForUnknownHeaderSetToLittleEndianAndFailToBookmarkStart)
{
    tubyte hdr[8] = {0x7c, 0x6d, 0x5e, 0x4f, 0x04, 0x03, 0x02, 0x01};
    
    IFFChunkReadHeaderFile fileInvoke(hdr,8);
    
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,read(A<tbyte*>(),8)).Times(1)
        .WillOnce(Invoke(&fileInvoke,&IFFChunkReadHeaderFile::read));
    EXPECT_CALL(file,bookmark(0)).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(file,bookmark(0x01020304)).Times(1).WillOnce(Return(1));
    
    IFFChunk chunk(&file,true);
    
    EXPECT_FALSE(chunk.readHeader());
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,readHeaderForUnknownHeaderSetToLittleEndianAndFailToBookmarkEnd)
{
    tubyte hdr[8] = {0x7c, 0x6d, 0x5e, 0x4f, 0x04, 0x03, 0x02, 0x01};
    
    IFFChunkReadHeaderFile fileInvoke(hdr,8);
    
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,read(A<tbyte*>(),8)).Times(1)
        .WillOnce(Invoke(&fileInvoke,&IFFChunkReadHeaderFile::read));
    EXPECT_CALL(file,bookmark(0)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(file,bookmark(0x01020304)).Times(1).WillOnce(Return(-1));
    
    IFFChunk chunk(&file,true);
    
    EXPECT_FALSE(chunk.readHeader());
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,readHeaderForUnknownHeaderSetToLittleEndianAndSuccess)
{
    tubyte hdr[8] = {0x7c, 0x6d, 0x5e, 0x4f, 0x04, 0x03, 0x02, 0x01};
    
    IFFChunkReadHeaderFile fileInvoke(hdr,8);
    
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,read(A<tbyte*>(),8)).Times(1)
        .WillOnce(Invoke(&fileInvoke,&IFFChunkReadHeaderFile::read));
    EXPECT_CALL(file,bookmark(0)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(file,bookmark(0x01020304)).Times(1).WillOnce(Return(1));
    
    IFFChunk chunk(&file,true);
    
    EXPECT_TRUE(chunk.readHeader());
    
    EXPECT_TRUE(chunk.id()==0x4f5e6d7c);
    EXPECT_TRUE(chunk.size()==0x01020304);
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,readHeaderForUnknownHeaderSetToBigEndianAndSuccess)
{
    tubyte hdr[8] = {0x4f, 0x5e, 0x6d, 0x7c, 0x01, 0x02, 0x03, 0x04};
    
    IFFChunkReadHeaderFile fileInvoke(hdr,8);
    
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,read(A<tbyte*>(),8)).Times(1)
        .WillOnce(Invoke(&fileInvoke,&IFFChunkReadHeaderFile::read));
    EXPECT_CALL(file,bookmark(0)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(file,bookmark(0x01020304)).Times(1).WillOnce(Return(1));
    
    IFFChunk chunk(&file,false);
    
    EXPECT_TRUE(chunk.readHeader());
    
    EXPECT_TRUE(chunk.id()==0x4f5e6d7c);
    EXPECT_TRUE(chunk.size()==0x01020304);

}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,readHeaderForExpectedHeaderInLittleEndian)
{
    tubyte hdr[8] = {0x7c, 0x6d, 0x5e, 0x4f, 0x04, 0x03, 0x02, 0x01};
    
    IFFChunkReadHeaderFile fileInvoke(hdr,8);
    
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,read(A<tbyte*>(),8)).Times(1)
        .WillOnce(Invoke(&fileInvoke,&IFFChunkReadHeaderFile::read));
    EXPECT_CALL(file,bookmark(0)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(file,bookmark(0x01020304)).Times(1).WillOnce(Return(1));
    
    IFFChunk chunk(IFF_CHUNK_TEST_ID,&file);
    
    EXPECT_TRUE(chunk.readHeader());
    
    EXPECT_TRUE(chunk.size()==0x01020304);
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,readHeaderForExpectedHeaderInBigEndian)
{
    tubyte hdr[8] = {0x4f, 0x5e, 0x6d, 0x7c, 0x01, 0x02, 0x03, 0x04};
    
    IFFChunkReadHeaderFile fileInvoke(hdr,8);
    
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,read(A<tbyte*>(),8)).Times(1)
        .WillOnce(Invoke(&fileInvoke,&IFFChunkReadHeaderFile::read));
    EXPECT_CALL(file,bookmark(0)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(file,bookmark(0x01020304)).Times(1).WillOnce(Return(1));
    
    IFFChunk chunk(IFF_CHUNK_TEST_ID,&file);
    
    EXPECT_TRUE(chunk.readHeader());
    
    EXPECT_TRUE(chunk.size()==0x01020304);
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,readHeaderForExpectedHeaderIsNotFound)
{
    tubyte hdr[8] = {0x7c, 0x6d, 0x5d, 0x4f, 0x04, 0x03, 0x02, 0x01};
    
    IFFChunkReadHeaderFile fileInvoke(hdr,8);
    
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,read(A<tbyte*>(),8)).Times(1)
        .WillOnce(Invoke(&fileInvoke,&IFFChunkReadHeaderFile::read));
    
    IFFChunk chunk(IFF_CHUNK_TEST_ID,&file);
    
    EXPECT_FALSE(chunk.readHeader());
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,defaultConstructorAndSetup)
{
    IFFChunkTest chunk;
    
    EXPECT_TRUE(chunk.getFile()==0);
    EXPECT_FALSE(chunk.isLittleEndian());
    
    common::BIOStream file(common::e_BIOStream_FileRead);
    chunk.setup(&file,true);

    EXPECT_TRUE(chunk.getFile()==&file);
    EXPECT_TRUE(chunk.isLittleEndian());
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,endianTypeWhenBig)
{
    common::BIOStream file(common::e_BIOStream_FileRead);
    IFFChunk chunk(&file,false);
    EXPECT_TRUE(chunk.endianType()==IFFChunk::e_EndianBig);
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,endianTypeWhenLittle)
{
    common::BIOStream file(common::e_BIOStream_FileRead);
    IFFChunk chunk(&file,true);
    EXPECT_TRUE(chunk.endianType()==IFFChunk::e_EndianLittle);
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,read32BitUnsignedWhenLittleEndian)
{
    tubyte x[4] = { 0x4d, 0x3c, 0x2b, 0x1a };
    tchar *y = reinterpret_cast<tchar *>(x);
    IFFChunkTest chunk(0,true);
    EXPECT_EQ(0x1a2b3c4d,chunk.testRead32BitUnsigned(y));
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,read32BitUnsignedWhenBigEndian)
{
    tubyte x[4] = { 0x1a, 0x2b, 0x3c, 0x4d };
    tchar *y = reinterpret_cast<tchar *>(x);
    IFFChunkTest chunk(0,false);
    EXPECT_EQ(0x1a2b3c4d,chunk.testRead32BitUnsigned(y));
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,read32BitSignedWhenLittleEndian)
{
    tubyte x[4] = { 0xeb, 0x32, 0xa4, 0xf8 };
    tchar *y = reinterpret_cast<tchar *>(x);
    IFFChunkTest chunk(0,true);
    EXPECT_EQ(-123456789,chunk.testRead32BitSigned(y));
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,read32BitSignedWhenBigEndian)
{
    tubyte x[4] = { 0xf8, 0xa4, 0x32, 0xeb };
    tchar *y = reinterpret_cast<tchar *>(x);
    IFFChunkTest chunk(0,false);
    EXPECT_EQ(-123456789,chunk.testRead32BitSigned(y));
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,read16BitUnsignedWhenLittleEndian)
{
    tubyte x[2] = { 0x2b, 0x1a };
    tchar *y = reinterpret_cast<tchar *>(x);
    IFFChunkTest chunk(0,true);
    EXPECT_EQ(0x1a2b,chunk.testRead16BitUnsigned(y));
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,read16BitUnsignedWhenBigEndian)
{
    tubyte x[2] = { 0x1a, 0x2b };
    tchar *y = reinterpret_cast<tchar *>(x);
    IFFChunkTest chunk(0,false);
    EXPECT_EQ(0x1a2b,chunk.testRead16BitUnsigned(y));
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,read16BitSignedWhenLittleEndian)
{
    tubyte x[2] = { 0x2e, 0xfb };
    tchar *y = reinterpret_cast<tchar *>(x);
    IFFChunkTest chunk(0,true);
    EXPECT_EQ(-1234,chunk.testRead16BitSigned(y));
}

//-------------------------------------------------------------------------------------------

TEST(IFFChunk,read16BitSignedWhenBigEndian)
{
    tubyte x[2] = { 0xfb, 0x2e };
    tchar *y = reinterpret_cast<tchar *>(x);
    IFFChunkTest chunk(0,false);
    EXPECT_EQ(-1234,chunk.testRead16BitSigned(y));
}

//-------------------------------------------------------------------------------------------
