#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/test/BIOStreamMock.h"
#include "engine/wavpackomega/inc/WavPackBIOReader.h"

using namespace omega;
using namespace omega::engine::wavpackomega;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,readNoFileGivesZeroBytes)
{
    tbyte t[128];
    EXPECT_EQ(0,WavPackBIOReaderReadBytes(0,t,2));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,readGivesZeroBytesOnFileReadError)
{
    tbyte t[128];
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,read(t,128)).Times(1).WillOnce(Return(-1));
    EXPECT_EQ(0, WavPackBIOReaderReadBytes(&file,t,128));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,readGivesBytesReturnedFromRead)
{
    tbyte t[128];
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,read(t,128)).Times(1).WillOnce(Return(50));
    EXPECT_EQ(50, WavPackBIOReaderReadBytes(&file,t,128));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,getPositionErrorWithNoFile)
{
    EXPECT_EQ(-1, static_cast<int>(WavPackBIOReaderGetPos(0)));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,getPositionGivesOffsetFromFile)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,offset()).Times(1).WillOnce(Return(128));
    EXPECT_EQ(128,WavPackBIOReaderGetPos((void *)&file));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,setPositionAbsoluteNoFileGivesError)
{
    EXPECT_EQ(-1,WavPackBIOReaderSetPosAbs(0,50));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,setPositionAbsoluteSeekFailGivesError)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,seek(50,common::e_Seek_Start)).Times(1).WillOnce(Return(false));
    EXPECT_EQ(-1,WavPackBIOReaderSetPosAbs((void *)&file,50));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,setPositionAbsoluteSeekSuccessGivesZero)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,seek(50,common::e_Seek_Start)).Times(1).WillOnce(Return(true));
    EXPECT_EQ(0,WavPackBIOReaderSetPosAbs((void *)&file,50));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,setPositionRelativeNoFilesGivesError)
{
    EXPECT_EQ(-1,WavPackBIOReaderSetPosRel(0,10,SEEK_SET));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,setPositionRelativeErrorOnSeekFromStart)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,seek(50,common::e_Seek_Start)).Times(1).WillOnce(Return(false));
    EXPECT_EQ(-1,WavPackBIOReaderSetPosRel((void *)&file,50,SEEK_SET));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,setPositionRelativeSuccessOnSeekFromStart)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,seek(50,common::e_Seek_Start)).Times(1).WillOnce(Return(true));
    EXPECT_EQ(0,WavPackBIOReaderSetPosRel((void *)&file,50,SEEK_SET));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,setPositionRelativeErrorOnSeekFromCurrent)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,seek(50,common::e_Seek_Current)).Times(1).WillOnce(Return(false));
    EXPECT_EQ(-1,WavPackBIOReaderSetPosRel((void *)&file,50,SEEK_CUR));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,setPositionRelativeSuccessOnSeekFromCurrent)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,seek(50,common::e_Seek_Current)).Times(1).WillOnce(Return(true));
    EXPECT_EQ(0,WavPackBIOReaderSetPosRel((void *)&file,50,SEEK_CUR));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,setPositionRelativeErrorOnSeekFromEnd)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,seek(50,common::e_Seek_End)).Times(1).WillOnce(Return(false));
    EXPECT_EQ(-1,WavPackBIOReaderSetPosRel((void *)&file,50,SEEK_END));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,setPositionRelativeSuccessOnSeekFromEnd)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,seek(50,common::e_Seek_End)).Times(1).WillOnce(Return(true));
    EXPECT_EQ(0,WavPackBIOReaderSetPosRel((void *)&file,50,SEEK_END));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,pushBackByteNoFileGivesEOF)
{
    EXPECT_EQ(EOF,WavPackBIOReaderPushBackByte(0,40));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,pushBackByteErrorInSeekGivesEOF)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,seek(-1,common::e_Seek_Current)).Times(1).WillOnce(Return(false));
    EXPECT_EQ(EOF,WavPackBIOReaderPushBackByte((void *)&file,40));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,pushBackByteSuccessInSeekGivesPassedCharacter)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,seek(-1,common::e_Seek_Current)).Times(1).WillOnce(Return(true));
    EXPECT_EQ(40,WavPackBIOReaderPushBackByte((void *)&file,40));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,getLengthGivesZeroWithNoFile)
{
    EXPECT_EQ(0,WavPackBIOReaderGetLength(0));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,getLengthGivesZeroWithOnError)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,size()).Times(1).WillOnce(Return(-1));
    EXPECT_EQ(0,WavPackBIOReaderGetLength((void *)&file));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,getLengthGivesSizeOnSuccess)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,size()).Times(1).WillOnce(Return(128));
    EXPECT_EQ(128,WavPackBIOReaderGetLength((void *)&file));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,cannotSeekGivenNoFile)
{
    EXPECT_FALSE(WavPackBIOReaderCanSeek(0));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,canSeekGivenFile)
{
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_TRUE(WavPackBIOReaderCanSeek((void *)&file));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,writeGivesZeroIfNoFile)
{
    tbyte tmp[128];
    EXPECT_EQ(0,WavPackBIOReaderWriteBytes(0,(void *)tmp,128));

}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,writeGivesZeroIfFileNotWriteable)
{
    tbyte tmp[128];
    common::test::BIOStreamMock file(common::e_BIOStream_FileRead);
    EXPECT_CALL(file,writeable()).Times(1).WillOnce(Return(false));
    EXPECT_EQ(0,WavPackBIOReaderWriteBytes((void *)&file,(void *)tmp,128));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,writeGivesZeroIfWriteGivesError)
{
    tbyte tmp[128];
    common::test::BIOStreamMock file(common::e_BIOStream_FileWrite);
    EXPECT_CALL(file,writeable()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(file,write(reinterpret_cast<const tbyte *>(tmp),128)).Times(1).WillOnce(Return(-1));
    EXPECT_EQ(0,WavPackBIOReaderWriteBytes((void *)&file,(void *)tmp,128));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,writeGivesAmountReturnedFromWrite)
{
    tbyte tmp[128];
    common::test::BIOStreamMock file(common::e_BIOStream_FileWrite);
    EXPECT_CALL(file,writeable()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(file,write(reinterpret_cast<const tbyte *>(tmp),128)).Times(1).WillOnce(Return(50));
    EXPECT_EQ(50,WavPackBIOReaderWriteBytes((void *)&file,(void *)tmp,128));
}

//-------------------------------------------------------------------------------------------

TEST(WavPackBIOReader,writeGivesAmountGivenWhenSuccessful)
{
    tbyte tmp[128];
    common::test::BIOStreamMock file(common::e_BIOStream_FileWrite);
    EXPECT_CALL(file,writeable()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(file,write(reinterpret_cast<const tbyte *>(tmp),128)).Times(1).WillOnce(Return(128));
    EXPECT_EQ(128,WavPackBIOReaderWriteBytes((void *)&file,(void *)tmp,128));
}

//-------------------------------------------------------------------------------------------
