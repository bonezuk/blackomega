#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/info/inc/APETag.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "common/inc/BIOBufferedStream.h"
#include "common/inc/DiskOps.h"

using namespace omega;
using namespace omega::track::info;
using namespace testing;

//-------------------------------------------------------------------------------------------
//  0. Album = Avatar
//  1. Artist = James Horner
//  2. Comment = Comment Field
//  3. Genre = Soundtrack
//  4. Title = Gathering all the Na'vi clans for battle
//  5. Track = 11
//  6. Year = 2009
//  7. ALBUMARTIST = James Cameron
//  8. COMPOSER = James Horner (Composer)
//  9. DISCNUMBER = 2
// 10. Cover Art (Front) = Image (JPG)

//-------------------------------------------------------------------------------------------

TEST(APETag,readTag)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "gather1.ape");

    common::BIOBufferedStream fileIO(common::e_BIOStream_FileRead);
    ASSERT_TRUE(fileIO.open(fileName));

    APETag tag;
    ASSERT_TRUE(tag.read(&fileIO));

    ASSERT_TRUE(tag.items().size()==11);

    EXPECT_TRUE(tag.items().at(0).isString());
    EXPECT_TRUE(tag.items().at(0).key()=="Album");
    EXPECT_TRUE(tag.items().at(0).text()=="Avatar");

    EXPECT_TRUE(tag.items().at(1).isString());
    EXPECT_TRUE(tag.items().at(1).key()=="Artist");
    EXPECT_TRUE(tag.items().at(1).text()=="James Horner");

    EXPECT_TRUE(tag.items().at(2).isString());
    EXPECT_TRUE(tag.items().at(2).key()=="Comment");
    EXPECT_TRUE(tag.items().at(2).text()=="Comment Field");

    EXPECT_TRUE(tag.items().at(3).isString());
    EXPECT_TRUE(tag.items().at(3).key()=="Genre");
    EXPECT_TRUE(tag.items().at(3).text()=="SoundTrack");

    QString title = QString::fromLatin1("Gathering all the Na") + QChar(0x2019) + QString::fromLatin1("vi clans for battle");
    EXPECT_TRUE(tag.items().at(4).isString());
    EXPECT_TRUE(tag.items().at(4).key()=="Title");
    EXPECT_TRUE(tag.items().at(4).text()==title);

    EXPECT_TRUE(tag.items().at(5).isString());
    EXPECT_TRUE(tag.items().at(5).key()=="Track");
    EXPECT_TRUE(tag.items().at(5).text()=="11");

    EXPECT_TRUE(tag.items().at(6).isString());
    EXPECT_TRUE(tag.items().at(6).key()=="Year");
    EXPECT_TRUE(tag.items().at(6).text()=="2009");

    EXPECT_TRUE(tag.items().at(7).isString());
    EXPECT_TRUE(tag.items().at(7).key()=="ALBUMARTIST");
    EXPECT_TRUE(tag.items().at(7).text()=="James Cameron");

    EXPECT_TRUE(tag.items().at(8).isString());
    EXPECT_TRUE(tag.items().at(8).key()=="COMPOSER");
    EXPECT_TRUE(tag.items().at(8).text()=="James Horner (Composer)");

    EXPECT_TRUE(tag.items().at(9).isString());
    EXPECT_TRUE(tag.items().at(9).key()=="DISCNUMBER");
    EXPECT_TRUE(tag.items().at(9).text()=="2");

    EXPECT_TRUE(tag.items().at(10).isData());
    EXPECT_TRUE(tag.items().at(10).key()=="Cover Art (Front)");

    APETagImage coverArt(tag.items().at(10));
    EXPECT_TRUE(coverArt.isImage());
    EXPECT_TRUE(!coverArt.image().isNull());
    EXPECT_TRUE(coverArt.name()=="Cover Art (Front).jpg");

    ImageInfoArray *imageData = coverArt.imageData();
    EXPECT_FALSE(QImage::fromData(imageData->GetData(),imageData->GetSize()).isNull());
    delete imageData;
}

//-------------------------------------------------------------------------------------------

TEST(APETagItem,readFailWithNoMemory)
{
    APETagItem item;
    EXPECT_TRUE(item.read(0,10)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(APETagItem,readFailWithNoKeyOrData)
{
    const tubyte *data[8] = {
        0x00, 0x00, 0x00, 0x00, // length - 4
        0x00, 0x00, 0x00, 0x00, // flags - 4
    };

    APETagItem item;
    EXPECT_TRUE(item.read(reinterpret_cast<const tbyte *>(data),8)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(APETagItem,readFailWithUnterminatedKey)
{
    const tubyte data[20] = {
        0x00, 0x00, 0x00, 0x00, // length - 4
        0x00, 0x00, 0x00, 0x00, // flags - 4
        0x41, 0x6C, 0x62, 0x75, 0x6D, 0x6D,
        0x41, 0x76, 0x61, 0x74, 0x61, 0x72
    };

    APETagItem item;
    EXPECT_TRUE(item.read(reinterpret_cast<const tbyte *>(data),20)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(APETagItem,readFailWithInvalidKey)
{
    const tubyte data[20] = {
        0x06, 0x00, 0x00, 0x00, // length - 4
        0x00, 0x00, 0x00, 0x00, // flags - 4
        0x41, 0x6C, 0x62, 0x75, 0x1f, 0x00,
        0x41, 0x76, 0x61, 0x74, 0x61, 0x72
    };

    APETagItem item;
    EXPECT_TRUE(item.read(reinterpret_cast<const tbyte *>(data),20)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(APETagItem,readSuccessWithNoData)
{
    const tubyte data[14] = {
        0x00, 0x00, 0x00, 0x00, // length - 4
        0x00, 0x00, 0x00, 0x00, // flags - 4
        0x41, 0x6C, 0x62, 0x75, 0x6D, 0x00
    };

    APETagItem item;
    EXPECT_TRUE(item.read(reinterpret_cast<const tbyte *>(data),14)==14);

    EXPECT_TRUE(item.isString());
    EXPECT_FALSE(item.isData());
    EXPECT_FALSE(item.isLink());

    EXPECT_TRUE(item.key()=="Album");
    EXPECT_TRUE(item.text()=="");
    EXPECT_TRUE(item.data().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(APETagItem,readFailWhenNotEnoughData)
{
    const tubyte data[20] = {
        0x07, 0x00, 0x00, 0x00, // length - 4
        0x00, 0x00, 0x00, 0x00, // flags - 4
        0x41, 0x6C, 0x62, 0x75, 0x6D, 0x00, // key - 6
        0x41, 0x76, 0x61, 0x74, 0x61, 0x72  // data - 6
    };

    APETagItem item;
    EXPECT_TRUE(item.read(reinterpret_cast<const tbyte *>(data),20)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(APETagItem,readStringTag)
{
    const tubyte data[20] = {
        0x06, 0x00, 0x00, 0x00, // length - 4
        0x00, 0x00, 0x00, 0x00, // flags - 4
        0x41, 0x6C, 0x62, 0x75, 0x6D, 0x00, // key - 6
        0x41, 0x76, 0x61, 0x74, 0x61, 0x72  // data - 6
    };

    APETagItem item;
    EXPECT_TRUE(item.read(reinterpret_cast<const tbyte *>(data),20)==20);

    EXPECT_TRUE(item.isString());
    EXPECT_FALSE(item.isData());
    EXPECT_FALSE(item.isLink());

    EXPECT_TRUE(item.key()=="Album");
    EXPECT_TRUE(item.text()=="Avatar");
}

//-------------------------------------------------------------------------------------------

TEST(APETagItem,readStringTagWithUTF8)
{
    const tubyte data[56] = {
        0x2A, 0x00, 0x00, 0x00, // 4
        0x00, 0x00, 0x00, 0x00, // 4
        0x54, 0x69, 0x74, 0x6C, 0x65, 0x00, // 6
        0x47, 0x61, 0x74, 0x68, 0x65, 0x72, 0x69, 0x6E, 0x67, 0x20, 0x61, 0x6C, 0x6C, 0x20, 0x74, 0x68, // 16
        0x65, 0x20, 0x4E, 0x61, 0xE2, 0x80, 0x99, 0x76, 0x69, 0x20, 0x63, 0x6C, 0x61, 0x6E, 0x73, 0x20, // 16
        0x66, 0x6F, 0x72, 0x20, 0x62, 0x61, 0x74, 0x74, 0x6C, 0x65 // 10
    };

    QString name = QString::fromLatin1("Gathering all the Na") + QChar(0x2019) + QString::fromLatin1("vi clans for battle");

    APETagItem item;
    EXPECT_TRUE(item.read(reinterpret_cast<const tbyte *>(data),56)==56);

    EXPECT_TRUE(item.isString());
    EXPECT_FALSE(item.isData());
    EXPECT_FALSE(item.isLink());

    EXPECT_TRUE(item.key()=="Title");
    EXPECT_TRUE(item.text()==name);
}

//-------------------------------------------------------------------------------------------

TEST(APETagItem,readDataTag)
{
    const tubyte data[17] = {
        0x04, 0x00, 0x00, 0x00, // 4
        0x02, 0x00, 0x00, 0x00, // 4
        0x54, 0x43, 0x4F, 0x4D, 0x00, // 5
        0x12, 0x34, 0x56, 0x78 // 4
    };

    // key = TCOM
    // data = { 0x12, 0x34, 0x56, 0x78 }

    APETagItem item;
    EXPECT_TRUE(item.read(reinterpret_cast<const tbyte *>(data),17)==17);

    EXPECT_FALSE(item.isString());
    EXPECT_TRUE(item.isData());
    EXPECT_FALSE(item.isLink());

    EXPECT_TRUE(item.key()=="TCOM");
    EXPECT_TRUE(::memcmp(item.data().constData(),&data[13],4 * sizeof(tubyte))==0);
}

//-------------------------------------------------------------------------------------------

TEST(APETagItem,readLinkTag)
{
    const tubyte data[37] = {
        0x18, 0x00, 0x00, 0x00, // 4
        0x04, 0x00, 0x00, 0x00, // 4
        0x6c, 0x69, 0x6e, 0x6b, 0x00, // 5
        0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x68, 0x6f, 0x73, 0x74, 0x2f, // 12
        0x66, 0x69, 0x6c, 0x65, 0x6e, 0x61, 0x6d, 0x65, 0x2e, 0x65, 0x78, 0x74  // 12
    };

    // key = link
    // data = http://host/filename.ext

    APETagItem item;
    EXPECT_TRUE(item.read(reinterpret_cast<const tbyte *>(data),37)==37);

    EXPECT_TRUE(item.isString());
    EXPECT_FALSE(item.isData());
    EXPECT_TRUE(item.isLink());

    EXPECT_TRUE(item.key()=="link");
    EXPECT_TRUE(item.text()=="http://host/filename.ext");
}

//-------------------------------------------------------------------------------------------

class APETagTest : public APETag
{
    public:
        APETagTest();
        virtual ~APETagTest();

        bool testReadHeader(const tbyte *mem,tint len,tint& tagVersion,tint& tagSize,tint& noItems,tint& flags);
};

//-------------------------------------------------------------------------------------------

APETagTest::APETagTest() : APETag()
{}

//-------------------------------------------------------------------------------------------

APETagTest::~APETagTest()
{}

//-------------------------------------------------------------------------------------------

bool APETagTest::testReadHeader(const tbyte *mem,tint len,tint& tagVersion,tint& tagSize,tint& noItems,tint& flags)
{
    return readHeader(mem,len,tagVersion,tagSize,noItems,flags);
}

//-------------------------------------------------------------------------------------------

TEST(APETag,readHeaderFailWithNoMemory)
{
    int version,size,noItems,flags;
    APETagTest tag;

    EXPECT_FALSE(tag.testReadHeader(0,32,version,size,noItems,flags));
}

//-------------------------------------------------------------------------------------------

TEST(APETag,readHeaderFailWithNotEnoughLength)
{
    const tubyte data[32] = {
        0x41, 0x50, 0x45, 0x54, 0x41, 0x47, 0x45, 0x58, // Preamble (0x0000b314) - 8
        0xd0, 0x07, 0x00, 0x00, // Tag version - 4
        0x14, 0xa2, 0x00, 0x00, // Tag size - 4
        0x12, 0x34, 0x56, 0x78, // Number of tag items - 4
        0x78, 0x56, 0x34, 0x12, // Global flags - 4
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Reserved (must be zero) - 8
    };

    int version,size,noItems,flags;
    APETagTest tag;

    EXPECT_FALSE(tag.testReadHeader(reinterpret_cast<const tbyte *>(data),31,version,size,noItems,flags));
}

//-------------------------------------------------------------------------------------------

TEST(APETag,readHeaderFailInvalidAPETagID)
{
    const tubyte data[32] = {
        0x41, 0x50, 0x45, 0x53, 0x41, 0x47, 0x45, 0x58, // Preamble (0x0000b314) - 8
        0xd0, 0x07, 0x00, 0x00, // Tag version - 4
        0x14, 0xa2, 0x00, 0x00, // Tag size - 4
        0x12, 0x34, 0x56, 0x78, // Number of tag items - 4
        0x78, 0x56, 0x34, 0x12, // Global flags - 4
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Reserved (must be zero) - 8
    };

    int version,size,noItems,flags;
    APETagTest tag;

    EXPECT_FALSE(tag.testReadHeader(reinterpret_cast<const tbyte *>(data),32,version,size,noItems,flags));
}

//-------------------------------------------------------------------------------------------

TEST(APETag,readHeaderFailUnknownVersion)
{
    const tubyte data[32] = {
        0x41, 0x50, 0x45, 0x54, 0x41, 0x47, 0x45, 0x58, // Preamble (0x0000b314) - 8
        0xe0, 0x07, 0x00, 0x00, // Tag version - 4
        0x14, 0xa2, 0x00, 0x00, // Tag size - 4
        0x12, 0x34, 0x56, 0x78, // Number of tag items - 4
        0x78, 0x56, 0x34, 0x12, // Global flags - 4
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Reserved (must be zero) - 8
    };

    int version,size,noItems,flags;
    APETagTest tag;

    EXPECT_FALSE(tag.testReadHeader(reinterpret_cast<const tbyte *>(data),32,version,size,noItems,flags));
}

//-------------------------------------------------------------------------------------------

TEST(APETag,readHeaderSuccess)
{
    const tubyte data[32] = {
        0x41, 0x50, 0x45, 0x54, 0x41, 0x47, 0x45, 0x58, // Preamble (0x0000b314) - 8
        0xd0, 0x07, 0x00, 0x00, // Tag version - 4
        0x14, 0xa2, 0x00, 0x00, // Tag size - 4
        0x12, 0x34, 0x56, 0x78, // Number of tag items - 4
        0x78, 0x56, 0x34, 0x12, // Global flags - 4
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Reserved (must be zero) - 8
    };

    int version,size,noItems,flags;
    APETagTest tag;

    EXPECT_TRUE(tag.testReadHeader(reinterpret_cast<const tbyte *>(data),32,version,size,noItems,flags));

    EXPECT_EQ(2000,version);
    EXPECT_EQ(41492,size);
    EXPECT_EQ(0x78563412,noItems);
    EXPECT_EQ(0x12345678,flags);
}

//-------------------------------------------------------------------------------------------

TEST(APETag,readFooterSuccess)
{
    const tubyte data[32] = {
        0x41, 0x50, 0x45, 0x54, 0x41, 0x47, 0x45, 0x58, // Preamble
        0xD0, 0x07, 0x00, 0x00, // Tag version
        0x14, 0xA2, 0x00, 0x00, // Tag size
        0x0B, 0x00, 0x00, 0x00, // Number of tag items
        0x00, 0x00, 0x00, 0x80, // Global flags
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Reserved (must be zero)
    };

    int version,size,noItems,flags;
    APETagTest tag;

    EXPECT_TRUE(tag.testReadHeader(reinterpret_cast<const tbyte *>(data),32,version,size,noItems,flags));

    EXPECT_EQ(2000,version);
    EXPECT_EQ(41492,size);
    EXPECT_EQ(11,noItems);
    EXPECT_EQ(0x80000000,flags);
}

//-------------------------------------------------------------------------------------------
