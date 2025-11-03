#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/BIOStream.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "common/inc/DiskOps.h"

using namespace testing;
using namespace omega::common;
using namespace omega;

//-------------------------------------------------------------------------------------------

class BIOStreamTest : public BIOStream
{
    public:
        BIOStreamTest(tint flags = e_BIOStream_FileRead);
        virtual ~BIOStreamTest();

        void setPosition64(tint64 nPos);
};

//-------------------------------------------------------------------------------------------

BIOStreamTest::BIOStreamTest(tint flags) : BIOStream(flags)
{}

//-------------------------------------------------------------------------------------------

BIOStreamTest::~BIOStreamTest()
{}

//-------------------------------------------------------------------------------------------

void BIOStreamTest::setPosition64(tint64 nPos)
{
    m_Position = nPos;
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,bookmarkAndPosition)
{
    const tubyte expectMem[11] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b };

    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "biostreamBookmark11Byte.dat");

    int bkArray[5];
    tbyte mem[2];
    BIOStream file(e_BIOStream_FileRead);

    EXPECT_TRUE(file.open(fileName));

    for(int i=0;i<5;i++)
    {
        bkArray[i] = file.bookmark(1);
        EXPECT_TRUE(bkArray[i]>=0);
        EXPECT_TRUE(file.seek(2,e_Seek_Current));
    }

    for(int i=0;i<5;i++)
    {
        EXPECT_TRUE(file.position(bkArray[i]));
        EXPECT_TRUE(file.read(mem,2)==2);
        EXPECT_TRUE(::memcmp(&expectMem[(i*2)+1],mem,2)==0);
    }

    file.close();

    EXPECT_TRUE(file.open(fileName));

    for(int i=0;i<5;i++)
    {
        EXPECT_FALSE(file.position(bkArray[i]));
    }
}

//-------------------------------------------------------------------------------------------

class BIOStreamSizeTest : public BIOStreamTest
{
    public:
        MOCK_CONST_METHOD0(getFileHandle,file_type());

        MOCK_CONST_METHOD2(PrintError,void(const tchar *strR,const tchar *strE));
        MOCK_CONST_METHOD3(PrintError,void(const tchar *strR,const tchar *strE1,const tchar *strE2));
        MOCK_CONST_METHOD4(PrintError,void(const tchar *strR,const tchar *strE1,const tchar *strE2,tint code));

#if defined(OMEGA_WIN32)
        MOCK_METHOD2(win32GetFileSize,DWORD(HANDLE hFile,LPDWORD pHigh));
        MOCK_METHOD0(win32GetLastError,DWORD());
#elif defined(OMEGA_POSIX)
        MOCK_METHOD3(posixLSeek,off_t(int filedes,off_t offset,int whence));
        MOCK_CONST_METHOD0(posixErrno,tint());
#endif
};

//-------------------------------------------------------------------------------------------

TEST(BIOStream,sizeGivenInvalidFile)
{
    file_type hFile = c_invalidFile;

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,PrintError(StrEq("size"),StrEq("No file open to get size from"))).Times(1);

    ASSERT_EQ(-1,file.size());
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

TEST(BIOStream,sizeGivenErrorInFileSize)
{
    file_type hFile = (file_type)(3);

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,win32GetFileSize(hFile,A<LPDWORD>())).Times(1).WillOnce(Return(INVALID_FILE_SIZE));
    EXPECT_CALL(file,win32GetLastError()).WillRepeatedly(Return(1234));
    EXPECT_CALL(file,PrintError(StrEq("size"), StrEq("Failed to obtain file size from \'"), StrEq("\'"), 1234)).Times(1);

    ASSERT_EQ(-1,file.size());
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,sizeGivenFileSizeZero)
{
    file_type hFile = (file_type)(3);

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,win32GetFileSize(hFile,A<DWORD *>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<1>(0),Return(0)));

    ASSERT_EQ(0,file.size());
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,sizeGivenFileSizeIn32BitRange)
{
    file_type hFile = (file_type)(3);

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,win32GetFileSize(hFile,A<DWORD *>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<1>(0),Return(5000)));

    ASSERT_EQ(5000,file.size());
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,sizeGivenFileSizeIn64BitRange)
{
    file_type hFile = (file_type)(3);

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,win32GetFileSize(hFile,A<DWORD *>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<1>(0x12345678),Return(0x9ABCDEF0)));

    ASSERT_EQ(0x123456789ABCDEF0ULL,file.size64());
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,sizeGivenFileSizeOn32BitBoundary)
{
    file_type hFile = (file_type)(3);

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,win32GetFileSize(hFile,A<DWORD *>())).Times(1)
        .WillOnce(Return(INVALID_FILE_SIZE));
    EXPECT_CALL(file,win32GetLastError()).WillRepeatedly(Return(NO_ERROR));

    ASSERT_EQ(0x00000000ffffffffULL,file.size64());
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

TEST(BIOStream,sizeSeekToEndFail)
{
    file_type hFile = (file_type)(3);

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,posixErrno()).WillRepeatedly(Return(1234));
    EXPECT_CALL(file,posixLSeek(hFile,0,SEEK_END)).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(file,PrintError(StrEq("size"), StrEq("Failed to seek to end of file position in \'"), StrEq("\'"),1234)).Times(1);

    ASSERT_EQ(-1,file.size());
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,sizeSeekBackToPositionFail)
{
    file_type hFile = (file_type)(3);

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,posixErrno()).WillRepeatedly(Return(1234));
    EXPECT_CALL(file,posixLSeek(hFile,0,SEEK_END)).Times(1).WillOnce(Return(1000));
    EXPECT_CALL(file,posixLSeek(hFile,500,SEEK_SET)).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(file,PrintError(StrEq("size"), StrEq("Failed to seek current position in \'"), StrEq("\'"), 1234)).Times(1);

    file.setPosition64(500);

    ASSERT_EQ(-1,file.size());
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,sizeGivenZeroFileSize)
{
    file_type hFile = (file_type)(3);

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,posixErrno()).WillRepeatedly(Return(1234));
    EXPECT_CALL(file,posixLSeek(hFile,0,SEEK_END)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(file,posixLSeek(hFile,500,SEEK_SET)).Times(1).WillOnce(Return(0));

    file.setPosition64(500);

    ASSERT_EQ(0,file.size());
    ASSERT_EQ(0,file.offset());
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,sizeGiven32BitFileSize)
{
    file_type hFile = (file_type)(3);

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,posixErrno()).WillRepeatedly(Return(1234));
    EXPECT_CALL(file,posixLSeek(hFile,0,SEEK_END)).Times(1).WillOnce(Return(6000));
    EXPECT_CALL(file,posixLSeek(hFile,500,SEEK_SET)).Times(1).WillOnce(Return(500));

    file.setPosition64(500);

    ASSERT_EQ(6000,file.size());
    ASSERT_EQ(500,file.offset());
}

//-------------------------------------------------------------------------------------------
#if !defined(OMEGA_LINUX)

TEST(BIOStream,sizeGiven64BitFileSize)
{
    off_t fSize = static_cast<off_t>(0x1234567889ABCDEFULL);
    off_t fPosition = static_cast<off_t>(0x0123456789ABCDEFULL);

    file_type hFile = (file_type)(3);

    BIOStreamSizeTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,posixErrno()).WillRepeatedly(Return(1234));
    EXPECT_CALL(file,posixLSeek(hFile,0,SEEK_END)).Times(1).WillOnce(Return(fSize));
    EXPECT_CALL(file,posixLSeek(hFile,fPosition,SEEK_SET)).Times(1).WillOnce(Return(fPosition));

    file.setPosition64(fPosition);

    ASSERT_EQ(fSize,file.size64());
    ASSERT_EQ(fPosition,file.offset64());
}

#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

class BIOStreamSeekTest : public BIOStreamTest
{
    public:
        MOCK_CONST_METHOD0(getFileHandle,file_type());
#if defined(OMEGA_WIN32)
        MOCK_METHOD2(win32GetFileSize,DWORD(HANDLE hFile,LPDWORD pHigh));
        MOCK_METHOD0(win32GetLastError,DWORD());
        MOCK_METHOD4(win32SetFilePointer,DWORD(HANDLE hFile,LONG lDistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod));
#elif defined(OMEGA_POSIX)
        MOCK_METHOD3(posixLSeek,off_t(int filedes,off_t offset,int whence));
        MOCK_CONST_METHOD0(posixErrno,tint());
#endif
        MOCK_METHOD0(size64,tint64());

        MOCK_CONST_METHOD2(PrintError,void(const tchar *strR,const tchar *strE));
        MOCK_CONST_METHOD3(PrintError,void(const tchar *strR,const tchar *strE1,const tchar *strE2));
        MOCK_CONST_METHOD4(PrintError,void(const tchar *strR,const tchar *strE1,const tchar *strE2,tint code));
};

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromStartWithNegativeOffsetFromFileStart)
{
    BIOStreamSeekTest file;
    EXPECT_CALL(file,PrintError(StrEq("seek"),StrEq("Cannot seek to position beyond beginning of file"))).Times(1);

    ASSERT_FALSE(file.seek(-1,e_Seek_Start));
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromStartWithPositiveOffsetBeyondFileEnd)
{
    BIOStreamSeekTest file;
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,PrintError(StrEq("seek"),StrEq("Cannot seek to position beyond end of file"))).Times(1);

    ASSERT_FALSE(file.seek(101,e_Seek_Start));
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromCurrentWithNegativeOffsetFromFileStart)
{
    BIOStreamSeekTest file;
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,PrintError(StrEq("seek"),StrEq("Cannot seek to position beyond beginning of file"))).Times(1);
    file.setPosition64(60);

    ASSERT_FALSE(file.seek(-61,e_Seek_Current));
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromCurrentWithPositiveOffsetBeyondFileEnd)
{
    BIOStreamSeekTest file;
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,PrintError(StrEq("seek"),StrEq("Cannot seek to position beyond end of file"))).Times(1);
    file.setPosition64(60);

    ASSERT_FALSE(file.seek(41,e_Seek_Current));
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromEndWithNegativeOffsetFromFileStart)
{
    BIOStreamSeekTest file;
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,PrintError(StrEq("seek"),StrEq("Cannot seek to position beyond beginning of file"))).Times(1);

    ASSERT_FALSE(file.seek(-101,e_Seek_End));
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromEndWithPostiveOffsetBeyondFileEnd)
{
    BIOStreamSeekTest file;
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,PrintError(StrEq("seek"),StrEq("Cannot seek to position beyond end of file"))).Times(1);

    ASSERT_FALSE(file.seek(1,e_Seek_End));
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromUnknownPosition)
{
    BIOStreamSeekTest file;
    EXPECT_CALL(file,PrintError(StrEq("seek"),StrEq("Unknown position flag given"))).Times(1);

    ASSERT_FALSE(file.seek(1,static_cast<BIOStreamPosition>(1234)));
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekGivenSeekError)
{
    file_type hFile = reinterpret_cast<file_type>(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,win32GetLastError()).WillRepeatedly(Return(1234));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,win32SetFilePointer(hFile,40,A<PLONG>(),FILE_BEGIN)).Times(1)
        .WillOnce(Return(INVALID_SET_FILE_POINTER));
    EXPECT_CALL(file,PrintError(StrEq("seek"),StrEq("Failed to move file position in '"),StrEq("'"),1234)).Times(1);
    file.setPosition64(30);

    ASSERT_FALSE(file.seek(10,e_Seek_Current));
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromStart)
{
    LONG posH = -1;
    file_type hFile = reinterpret_cast<file_type>(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,win32SetFilePointer(hFile,10,A<PLONG>(),FILE_BEGIN)).Times(1)
        .WillOnce(DoAll(SaveArgPointee<2>(&posH),SetArgPointee<2>(0),Return(10)));
    file.setPosition64(30);

    ASSERT_TRUE(file.seek(10,e_Seek_Start));

    ASSERT_EQ(10,file.offset());
    EXPECT_EQ(0,posH);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromCurrentBackwards)
{
    LONG posH = -1;
    file_type hFile = reinterpret_cast<file_type>(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,win32SetFilePointer(hFile,20,A<PLONG>(),FILE_BEGIN)).Times(1)
        .WillOnce(DoAll(SaveArgPointee<2>(&posH),SetArgPointee<2>(0),Return(20)));
    file.setPosition64(30);

    ASSERT_TRUE(file.seek(-10,e_Seek_Current));

    ASSERT_EQ(20,file.offset());
    EXPECT_EQ(0,posH);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromCurrentForwards)
{
    LONG posH = -1;
    file_type hFile = reinterpret_cast<file_type>(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,win32SetFilePointer(hFile,40,A<PLONG>(),FILE_BEGIN)).Times(1)
        .WillOnce(DoAll(SaveArgPointee<2>(&posH),SetArgPointee<2>(0),Return(40)));
    file.setPosition64(30);

    ASSERT_TRUE(file.seek(10,e_Seek_Current));

    ASSERT_EQ(40,file.offset());
    EXPECT_EQ(0,posH);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromEnd)
{
    LONG posH = -1;
    file_type hFile = reinterpret_cast<file_type>(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,win32SetFilePointer(hFile,90,A<PLONG>(),FILE_BEGIN)).Times(1)
        .WillOnce(DoAll(SaveArgPointee<2>(&posH),SetArgPointee<2>(0),Return(90)));
    file.setPosition64(30);

    ASSERT_TRUE(file.seek(-10,e_Seek_End));

    ASSERT_EQ(90,file.offset());
    EXPECT_EQ(0,posH);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekWith64BitValue)
{
    tint64 nPos = 0x123456789ABCDEF0ULL;

    LONG posH = -1;
    file_type hFile = reinterpret_cast<file_type>(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(nPos));
    EXPECT_CALL(file,win32SetFilePointer(hFile,0x9ABCDEF0,A<PLONG>(),FILE_BEGIN)).Times(1)
        .WillOnce(DoAll(SaveArgPointee<2>(&posH),SetArgPointee<2>(0x12345678),Return(0x9ABCDEF0)));
    file.setPosition64(30);

    ASSERT_TRUE(file.seek64(nPos,e_Seek_Start));

    ASSERT_EQ(nPos,file.offset64());
    EXPECT_EQ(0x12345678,posH);
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekGivenSeekError)
{
    file_type hFile = (file_type)(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,posixErrno()).WillRepeatedly(Return(1234));
    EXPECT_CALL(file,posixLSeek(hFile,10,SEEK_SET)).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(file,PrintError(StrEq("seek"),StrEq("Failed to move file position in '"),StrEq("'"),1234)).Times(1);

    ASSERT_FALSE(file.seek64(10,e_Seek_Start));
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromStart)
{
    file_type hFile = (file_type)(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,posixLSeek(hFile,10,SEEK_SET)).Times(1).WillOnce(Return(10));
    file.setPosition64(30);

    ASSERT_TRUE(file.seek64(10,e_Seek_Start));

    ASSERT_EQ(10,file.offset());
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromCurrentBackwards)
{
    file_type hFile = (file_type)(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,posixLSeek(hFile,20,SEEK_SET)).Times(1).WillOnce(Return(20));
    file.setPosition64(30);

    ASSERT_TRUE(file.seek64(-10,e_Seek_Current));

    ASSERT_EQ(20,file.offset());
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromCurrentForwards)
{
    file_type hFile = (file_type)(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,posixLSeek(hFile,40,SEEK_SET)).Times(1).WillOnce(Return(40));
    file.setPosition64(30);

    ASSERT_TRUE(file.seek64(10,e_Seek_Current));

    ASSERT_EQ(40,file.offset());
}

//-------------------------------------------------------------------------------------------

TEST(BIOStream,seekFromEnd)
{
    file_type hFile = (file_type)(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(100));
    EXPECT_CALL(file,posixLSeek(hFile,10,SEEK_SET)).Times(1).WillOnce(Return(10));
    file.setPosition64(30);

    ASSERT_TRUE(file.seek64(-90,e_Seek_End));

    ASSERT_EQ(10,file.offset());
}

//-------------------------------------------------------------------------------------------
#if !defined(OMEGA_LINUX)

TEST(BIOStream,seekWith64BitValue)
{
    tint64 nPos = 0x123456789ABCDEF0ULL;
    file_type hFile = (file_type)(3);

    BIOStreamSeekTest file;
    EXPECT_CALL(file,getFileHandle()).WillRepeatedly(Return(hFile));
    EXPECT_CALL(file,size64()).WillRepeatedly(Return(nPos));
    EXPECT_CALL(file,posixLSeek(hFile,nPos,SEEK_SET)).Times(1).WillOnce(Return(nPos));

    ASSERT_TRUE(file.seek64(nPos,e_Seek_Start));

    ASSERT_EQ(nPos,file.offset64());
}

#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
