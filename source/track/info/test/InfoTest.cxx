#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/info/inc/Info.h"
#include "common/test/BIOStreamMock.h"

using namespace omega::track::info;
using namespace testing;

//-------------------------------------------------------------------------------------------

class InfoTest : public Info
{
    public:
        InfoTest();
        bool testIsAppleFinderFile(const QString& name,omega::common::BIOStream *reader);
};

//-------------------------------------------------------------------------------------------

InfoTest::InfoTest() : Info()
{}

//-------------------------------------------------------------------------------------------

bool InfoTest::testIsAppleFinderFile(const QString& name,omega::common::BIOStream *reader)
{
    return isAppleFinderFile(name,reader);
}

//-------------------------------------------------------------------------------------------

TEST(Info,isAppleFinderFileWithNoName)
{
    omega::common::test::BIOStreamMock reader(omega::common::e_BIOStream_FileRead);
    InfoTest infoA;
    EXPECT_TRUE(infoA.testIsAppleFinderFile("",&reader));
}

//-------------------------------------------------------------------------------------------

TEST(Info,isAppleFinderFileWithNoReader)
{
    InfoTest infoA;
    EXPECT_TRUE(infoA.testIsAppleFinderFile("/Users/name.m4a",0));
}

//-------------------------------------------------------------------------------------------

TEST(Info,isAppleFinderFileWithNoDirectory)
{
    omega::common::test::BIOStreamMock reader(omega::common::e_BIOStream_FileRead);
    InfoTest infoA;
    EXPECT_TRUE(infoA.testIsAppleFinderFile("name.m4a",&reader));
}

//-------------------------------------------------------------------------------------------

TEST(Info,isAppleFinderFileWithSingleLetterName)
{
    omega::common::test::BIOStreamMock reader(omega::common::e_BIOStream_FileRead);
    InfoTest infoA;
    EXPECT_TRUE(infoA.testIsAppleFinderFile("n",&reader));    
}

//-------------------------------------------------------------------------------------------

TEST(Info,isAppleFinderFileWithMatchingNameButNoFile)
{
    QString name = "._name.m4a";
    omega::common::test::BIOStreamMock reader(omega::common::e_BIOStream_FileRead);
    EXPECT_CALL(reader,open(name)).Times(1).WillOnce(Return(false));
    InfoTest infoA;
    EXPECT_FALSE(infoA.testIsAppleFinderFile(name,&reader));    
}

//-------------------------------------------------------------------------------------------

TEST(Info,isAppleFinderFileWithOpenButNoRead)
{
    QString name = "._name.m4a";
    omega::common::test::BIOStreamMock reader(omega::common::e_BIOStream_FileRead);
    EXPECT_CALL(reader,open(name)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(reader,read(A<tubyte*>(),4)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(reader,close()).Times(1);
    InfoTest infoA;
    EXPECT_FALSE(infoA.testIsAppleFinderFile(name,&reader));
}

//-------------------------------------------------------------------------------------------

int InfoIsAppleFinderFileWithOpenButNotMagicNumberRead(tubyte *mem)
{
    mem[0] = 0x00;
    mem[1] = 0x01;
    mem[2] = 0x02;
    mem[3] = 0x03;
    return 4;
}

TEST(Info,isAppleFinderFileWithOpenButNotMagicNumber)
{
    QString name = "/Users/Music/._name.m4a";
    omega::common::test::BIOStreamMock reader(omega::common::e_BIOStream_FileRead);
    EXPECT_CALL(reader,open(name)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(reader,read(A<tubyte*>(),4)).Times(1).WillOnce(WithArg<0>(Invoke(InfoIsAppleFinderFileWithOpenButNotMagicNumberRead)));
    EXPECT_CALL(reader,close()).Times(1);
    InfoTest infoA;
    EXPECT_TRUE(infoA.testIsAppleFinderFile(name,&reader));
}

//-------------------------------------------------------------------------------------------

int InfoIsAppleFinderFileWithOpenAndAppleSingleMagicNumberRead(tubyte *mem)
{
    mem[0] = 0x00;
    mem[1] = 0x05;
    mem[2] = 0x16;
    mem[3] = 0x00;
    return 4;
}

TEST(Info,isAppleFinderFileWithOpenAndAppleSingleMagicNumber)
{
    QString name = "C:\\Users\\Music\\._name.m4a";
    omega::common::test::BIOStreamMock reader(omega::common::e_BIOStream_FileRead);
    EXPECT_CALL(reader,open(name)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(reader,read(A<tubyte*>(),4)).Times(1).WillOnce(WithArg<0>(Invoke(InfoIsAppleFinderFileWithOpenAndAppleSingleMagicNumberRead)));
    EXPECT_CALL(reader,close()).Times(1);
    InfoTest infoA;
    EXPECT_FALSE(infoA.testIsAppleFinderFile(name,&reader));
}

//-------------------------------------------------------------------------------------------

int InfoIsAppleFinderFileWithOpenAndAppleDoubleMagicNumberRead(tubyte *mem)
{
    mem[0] = 0x00;
    mem[1] = 0x05;
    mem[2] = 0x16;
    mem[3] = 0x07;
    return 4;
}

//-------------------------------------------------------------------------------------------

TEST(Info,isAppleFinderFileWithOpenAndAppleDoubleMagicNumber)
{
    QString name = "C:\\Users\\Music\\._name.m4a";
    omega::common::test::BIOStreamMock reader(omega::common::e_BIOStream_FileRead);
    EXPECT_CALL(reader,open(name)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(reader,read(A<tubyte*>(),4)).Times(1).WillOnce(WithArg<0>(Invoke(InfoIsAppleFinderFileWithOpenAndAppleDoubleMagicNumberRead)));
    EXPECT_CALL(reader,close()).Times(1);
    InfoTest infoA;
    EXPECT_FALSE(infoA.testIsAppleFinderFile(name,&reader));
}

//-------------------------------------------------------------------------------------------
