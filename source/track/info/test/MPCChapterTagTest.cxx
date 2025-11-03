#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/info/inc/MPCChapterTag.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "common/inc/BIOBufferedStream.h"
#include "common/inc/BIOMemory.h"
#include "common/inc/DiskOps.h"

using namespace omega;
using namespace omega::track::info;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTag,readTag)
{
    const static tfloat64 c_TOLERANCE = 0.0000001;

    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "mpcchapter.mpc");

    common::BIOBufferedStream fileIO(common::e_BIOStream_FileRead);
    ASSERT_TRUE(fileIO.open(fileName));

    MPCChapterTag tag;
    ASSERT_TRUE(tag.read(&fileIO));

    ASSERT_TRUE(tag.items().size()==5);

    EXPECT_TRUE(tag.items().at(0).title()=="A Way Of Life");
    EXPECT_EQ(0,tag.items().at(0).sampleIndex());
    EXPECT_NEAR(0.0,static_cast<tfloat64>(tag.items().at(0).time()),c_TOLERANCE);

    EXPECT_TRUE(tag.items().at(1).title()=="Spectres In The Fog");
    EXPECT_EQ(44100,tag.items().at(1).sampleIndex());
    EXPECT_NEAR(1.0,static_cast<tfloat64>(tag.items().at(1).time()),c_TOLERANCE);

    EXPECT_TRUE(tag.items().at(2).title()=="Taken");
    EXPECT_EQ(88200,tag.items().at(2).sampleIndex());
    EXPECT_NEAR(2.0,static_cast<tfloat64>(tag.items().at(2).time()),c_TOLERANCE);

    EXPECT_TRUE(tag.items().at(3).title()=="A Hard Teacher");
    EXPECT_EQ(132300,tag.items().at(3).sampleIndex());
    EXPECT_NEAR(3.0,static_cast<tfloat64>(tag.items().at(3).time()),c_TOLERANCE);

    EXPECT_TRUE(tag.items().at(4).title()=="To Know My Enemy");
    EXPECT_EQ(176400,tag.items().at(4).sampleIndex());
    EXPECT_NEAR(4.0,static_cast<tfloat64>(tag.items().at(4).time()),c_TOLERANCE);
}

//-------------------------------------------------------------------------------------------

class MPCChapterTagItemTest : public MPCChapterTagItem
{
    public:
        MPCChapterTagItemTest();
        virtual ~MPCChapterTagItemTest();

        tint testDecodeSize(common::BIOStream *file,tint& pos);
        bool testTrackNumber(const QString& data,int& trackIndex,int& noOfTracks);
};

//-------------------------------------------------------------------------------------------

MPCChapterTagItemTest::MPCChapterTagItemTest() : MPCChapterTagItem()
{}

//-------------------------------------------------------------------------------------------

MPCChapterTagItemTest::~MPCChapterTagItemTest()
{}

//-------------------------------------------------------------------------------------------

tint MPCChapterTagItemTest::testDecodeSize(common::BIOStream *file,tint& pos)
{
    return decodeSize(file,pos);
}

//-------------------------------------------------------------------------------------------

bool MPCChapterTagItemTest::testTrackNumber(const QString& data,int& trackIndex,int& noOfTracks)
{
    return trackNumber(data,trackIndex,noOfTracks);
}

//-------------------------------------------------------------------------------------------

unsigned int encodeSizeMPCChapterTagItemTest(tuint64 size,char *buff,bool addCodeSize)
{
    unsigned int i = 1;
    int j;

    if (addCodeSize)
    {
        while ((1ull << (7 * i)) - i <= size)
        {
            i++;
        }
        size += i;
    } else
    {
        while ((1ull << (7 * i)) <= size)
        {
            i++;
        }
    }

    for( j = i - 1; j >= 0; j--)
    {
        buff[j] = (char) (size | 0x80);
        size >>= 7;
    }
    buff[i - 1] &= 0x7F;

    return i;
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,decodeSizeWithBoundarySizesNoAddCodeSize)
{
    for(tint i=0;i<31;i++)
    {
        tint pos = 0;
        char tmp[8];
        tint expectSize = 1 << i;
        tuint64 s = static_cast<tuint64>(expectSize);
        tuint encodeSize = encodeSizeMPCChapterTagItemTest(s,tmp,false);

        EXPECT_TRUE(encodeSize > 0);

        QByteArray mem(tmp,encodeSize);
        common::BIOMemory file(mem);

        MPCChapterTagItemTest item;
        tint actualSize = item.testDecodeSize(&file,pos);

        EXPECT_EQ(expectSize,actualSize);
        EXPECT_EQ(encodeSize,pos);
    }
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,decodeSizeWithBoundarySizesAddCodeSize)
{
    for(tint i=0;i<31;i++)
    {
        tint pos = 0;
        char tmp[8];
        tint expectSize = 1 << i;
        tuint64 s = static_cast<tuint64>(expectSize);
        tuint encodeSize = encodeSizeMPCChapterTagItemTest(s,tmp,true);
        expectSize += encodeSize;

        EXPECT_TRUE(encodeSize > 0);

        QByteArray mem(tmp,encodeSize);
        common::BIOMemory file(mem);

        MPCChapterTagItemTest item;
        tint actualSize = item.testDecodeSize(&file,pos);

        EXPECT_EQ(expectSize,actualSize);
        EXPECT_EQ(encodeSize,pos);
    }
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenNoFile)
{
    MPCChapterTagItem item;
    EXPECT_FALSE(item.read(0));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenUnableToReadChapterHeader)
{
    tbyte t[1] = {'C'};
    QByteArray arr(t,1);
    common::BIOMemory file(arr);
    MPCChapterTagItem item;
    EXPECT_FALSE(item.read(&file));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenInvalidChapterHeader)
{
    tbyte t[2] = {'C','t'};
    QByteArray arr(t,2);
    common::BIOMemory file(arr);
    MPCChapterTagItem item;
    EXPECT_FALSE(item.read(&file));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenNoChapterItemSize)
{
    tbyte t[2] = {'C','T'};
    QByteArray arr(t,2);
    common::BIOMemory file(arr);
    MPCChapterTagItem item;
    EXPECT_FALSE(item.read(&file));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenUnableToDecodeSampleIndex)
{
    tubyte t[6] = {0x43, 0x54, 0x6E, 0x82, 0xD8, 0xF4};

    QByteArray arr(reinterpret_cast<const tbyte *>(t),110);
    common::BIOMemory file(arr);

    MPCChapterTagItem item;
    EXPECT_FALSE(item.read(&file));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenTooShortATag)
{
    tubyte t[110] = {
        0x43, 0x54, 0x07, 0x82, 0xD8, 0x44, 0x00, 0x00, 0x00, 0x00, 0xD0, 0x07, 0x00, 0x00, 0x4C, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x72, 0x61, 0x63, 0x6B, 0x00,
        0x32, 0x2F, 0x35, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x72, 0x74, 0x69, 0x73,
        0x74, 0x00, 0x48, 0x61, 0x6E, 0x73, 0x20, 0x5A, 0x69, 0x6D, 0x6D, 0x65, 0x72, 0x13, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x69, 0x74, 0x6C, 0x65, 0x00, 0x53, 0x70, 0x65, 0x63, 0x74,
        0x72, 0x65, 0x73, 0x20, 0x49, 0x6E, 0x20, 0x54, 0x68, 0x65, 0x20, 0x46, 0x6F, 0x67
    };

    QByteArray arr(reinterpret_cast<const tbyte *>(t),110);
    common::BIOMemory file(arr);

    MPCChapterTagItem item;
    EXPECT_FALSE(item.read(&file));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenTooLongTagSize)
{
    tubyte t[110] = {
        0x43, 0x54, 0x6F, 0x82, 0xD8, 0x44, 0x00, 0x00, 0x00, 0x00, 0xD0, 0x07, 0x00, 0x00, 0x4C, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x72, 0x61, 0x63, 0x6B, 0x00,
        0x32, 0x2F, 0x35, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x72, 0x74, 0x69, 0x73,
        0x74, 0x00, 0x48, 0x61, 0x6E, 0x73, 0x20, 0x5A, 0x69, 0x6D, 0x6D, 0x65, 0x72, 0x13, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x69, 0x74, 0x6C, 0x65, 0x00, 0x53, 0x70, 0x65, 0x63, 0x74,
        0x72, 0x65, 0x73, 0x20, 0x49, 0x6E, 0x20, 0x54, 0x68, 0x65, 0x20, 0x46, 0x6F, 0x67
    };

    QByteArray arr(reinterpret_cast<const tbyte *>(t),110);
    common::BIOMemory file(arr);

    MPCChapterTagItem item;
    EXPECT_FALSE(item.read(&file));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenInvalidVersion)
{
    tubyte t[110] = {
        0x43, 0x54, 0x6E, 0x82, 0xD8, 0x44, 0x00, 0x00, 0x00, 0x00, 0xD1, 0x07, 0x00, 0x00, 0x4C, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x72, 0x61, 0x63, 0x6B, 0x00,
        0x32, 0x2F, 0x35, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x72, 0x74, 0x69, 0x73,
        0x74, 0x00, 0x48, 0x61, 0x6E, 0x73, 0x20, 0x5A, 0x69, 0x6D, 0x6D, 0x65, 0x72, 0x13, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x69, 0x74, 0x6C, 0x65, 0x00, 0x53, 0x70, 0x65, 0x63, 0x74,
        0x72, 0x65, 0x73, 0x20, 0x49, 0x6E, 0x20, 0x54, 0x68, 0x65, 0x20, 0x46, 0x6F, 0x67
    };

    QByteArray arr(reinterpret_cast<const tbyte *>(t),110);
    common::BIOMemory file(arr);

    MPCChapterTagItem item;
    EXPECT_FALSE(item.read(&file));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenAPETagHeaderTooLarge)
{
    tubyte t[110] = {
        0x43, 0x54, 0x6E, 0x82, 0xD8, 0x44, 0x00, 0x00, 0x00, 0x00, 0xD0, 0x07, 0x00, 0x00, 0x4D, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x72, 0x61, 0x63, 0x6B, 0x00,
        0x32, 0x2F, 0x35, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x72, 0x74, 0x69, 0x73,
        0x74, 0x00, 0x48, 0x61, 0x6E, 0x73, 0x20, 0x5A, 0x69, 0x6D, 0x6D, 0x65, 0x72, 0x13, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x69, 0x74, 0x6C, 0x65, 0x00, 0x53, 0x70, 0x65, 0x63, 0x74,
        0x72, 0x65, 0x73, 0x20, 0x49, 0x6E, 0x20, 0x54, 0x68, 0x65, 0x20, 0x46, 0x6F, 0x67
    };

    QByteArray arr(reinterpret_cast<const tbyte *>(t),110);
    common::BIOMemory file(arr);

    MPCChapterTagItem item;
    EXPECT_FALSE(item.read(&file));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenNumberOfItemsNotEqualToActualItems)
{
    tubyte t[110] = {
        0x43, 0x54, 0x6E, 0x82, 0xD8, 0x44, 0x00, 0x00, 0x00, 0x00, 0xD0, 0x07, 0x00, 0x00, 0x4C, 0x00,
        0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x72, 0x61, 0x63, 0x6B, 0x00,
        0x32, 0x2F, 0x35, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x72, 0x74, 0x69, 0x73,
        0x74, 0x00, 0x48, 0x61, 0x6E, 0x73, 0x20, 0x5A, 0x69, 0x6D, 0x6D, 0x65, 0x72, 0x13, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x69, 0x74, 0x6C, 0x65, 0x00, 0x53, 0x70, 0x65, 0x63, 0x74,
        0x72, 0x65, 0x73, 0x20, 0x49, 0x6E, 0x20, 0x54, 0x68, 0x65, 0x20, 0x46, 0x6F, 0x67
    };

    QByteArray arr(reinterpret_cast<const tbyte *>(t),110);
    common::BIOMemory file(arr);

    MPCChapterTagItem item;
    EXPECT_FALSE(item.read(&file));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenValidVersion1Tag)
{
    tubyte t[110] = {
        0x43, 0x54, 0x6E, 0x82, 0xD8, 0x44, 0x00, 0x00, 0x00, 0x00, 0xE8, 0x03, 0x00, 0x00, 0x4C, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x72, 0x61, 0x63, 0x6B, 0x00,
        0x32, 0x2F, 0x35, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x72, 0x74, 0x69, 0x73,
        0x74, 0x00, 0x48, 0x61, 0x6E, 0x73, 0x20, 0x5A, 0x69, 0x6D, 0x6D, 0x65, 0x72, 0x13, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x69, 0x74, 0x6C, 0x65, 0x00, 0x53, 0x70, 0x65, 0x63, 0x74,
        0x72, 0x65, 0x73, 0x20, 0x49, 0x6E, 0x20, 0x54, 0x68, 0x65, 0x20, 0x46, 0x6F, 0x67
    };

    QByteArray arr(reinterpret_cast<const tbyte *>(t),110);
    common::BIOMemory file(arr);

    MPCChapterTagItem item;
    EXPECT_TRUE(item.read(&file));

    EXPECT_EQ(44100,item.sampleIndex());
    EXPECT_TRUE(item.title()=="Spectres In The Fog");
    EXPECT_TRUE(item.performer()=="Hans Zimmer");
    EXPECT_TRUE(item.songWriter().isEmpty());
    EXPECT_TRUE(item.composer().isEmpty());
    EXPECT_TRUE(item.arranger().isEmpty());
    EXPECT_TRUE(item.message().isEmpty());
    EXPECT_TRUE(item.genre().isEmpty());
    EXPECT_EQ(1,item.trackIndex());
    EXPECT_EQ(5,item.numberOfTracks());
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,readGivenValidVersion2Tag)
{
    tubyte t[110] = {
        0x43, 0x54, 0x6E, 0x82, 0xD8, 0x44, 0x00, 0x00, 0x00, 0x00, 0xD0, 0x07, 0x00, 0x00, 0x4C, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x72, 0x61, 0x63, 0x6B, 0x00,
        0x32, 0x2F, 0x35, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x72, 0x74, 0x69, 0x73,
        0x74, 0x00, 0x48, 0x61, 0x6E, 0x73, 0x20, 0x5A, 0x69, 0x6D, 0x6D, 0x65, 0x72, 0x13, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x69, 0x74, 0x6C, 0x65, 0x00, 0x53, 0x70, 0x65, 0x63, 0x74,
        0x72, 0x65, 0x73, 0x20, 0x49, 0x6E, 0x20, 0x54, 0x68, 0x65, 0x20, 0x46, 0x6F, 0x67
    };

    QByteArray arr(reinterpret_cast<const tbyte *>(t),110);
    common::BIOMemory file(arr);

    MPCChapterTagItem item;
    EXPECT_TRUE(item.read(&file));

    EXPECT_EQ(44100,item.sampleIndex());
    EXPECT_TRUE(item.title()=="Spectres In The Fog");
    EXPECT_TRUE(item.performer()=="Hans Zimmer");
    EXPECT_TRUE(item.songWriter().isEmpty());
    EXPECT_TRUE(item.composer().isEmpty());
    EXPECT_TRUE(item.arranger().isEmpty());
    EXPECT_TRUE(item.message().isEmpty());
    EXPECT_TRUE(item.genre().isEmpty());
    EXPECT_EQ(1,item.trackIndex());
    EXPECT_EQ(5,item.numberOfTracks());
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenInvalidEmpty)
{
    QString data = "";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_FALSE(item.testTrackNumber(data,trackIndex,noTracks));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenInvalidOnlyString)
{
    QString data = "abc";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_FALSE(item.testTrackNumber(data,trackIndex,noTracks));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenInvalidOnlyNumber)
{
    QString data = "123";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_FALSE(item.testTrackNumber(data,trackIndex,noTracks));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenNonNumberInTrackIndex)
{
    QString data = "a/5";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_FALSE(item.testTrackNumber(data,trackIndex,noTracks));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenNonNumberInNumberOfTracks)
{
    QString data = "5/b";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_FALSE(item.testTrackNumber(data,trackIndex,noTracks));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenInvalidNegativeNumberOfTracks)
{
    QString data = "3/-1";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_FALSE(item.testTrackNumber(data,trackIndex,noTracks));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenInvalidNegativeTrackIndex)
{
    QString data = "0/5";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_FALSE(item.testTrackNumber(data,trackIndex,noTracks));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenAdditionCharacter)
{
    QString data = "1/5a";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_FALSE(item.testTrackNumber(data,trackIndex,noTracks));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenAdditionSeparator)
{
    QString data = "1/5/";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_FALSE(item.testTrackNumber(data,trackIndex,noTracks));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenInvalidWithGreaterTrackIndexThanTracks)
{
    QString data = "6/5";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_FALSE(item.testTrackNumber(data,trackIndex,noTracks));
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenValidSingleDigits)
{
    QString data = "2/5";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_TRUE(item.testTrackNumber(data,trackIndex,noTracks));
    EXPECT_EQ(1,trackIndex);
    EXPECT_EQ(5,noTracks);
}

//-------------------------------------------------------------------------------------------

TEST(MPCChapterTagItem,trackNumberWhenValidTwoDigits)
{
    QString data = "13/56";
    int trackIndex,noTracks;
    MPCChapterTagItemTest item;
    EXPECT_TRUE(item.testTrackNumber(data,trackIndex,noTracks));
    EXPECT_EQ(12,trackIndex);
    EXPECT_EQ(56,noTracks);
}

//-------------------------------------------------------------------------------------------
