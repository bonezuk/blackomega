#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/info/inc/APEInfo.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "common/inc/BIOBufferedStream.h"
#include "common/inc/DiskOps.h"

using namespace omega;
using namespace omega::track::info;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(APEInfo,readGather1APE)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "gather1.ape");

    common::BIOBufferedStream fileIO(common::e_BIOStream_FileRead);
    ASSERT_TRUE(fileIO.open(fileName));
    
    APEInfo tag;
    ASSERT_TRUE(tag.read(&fileIO));

    QString title = QString::fromLatin1("Gathering all the Na") + QChar(0x2019) + QString::fromLatin1("vi clans for battle");

    EXPECT_TRUE(tag.artist()=="James Horner");
    EXPECT_TRUE(tag.title()==title);
    EXPECT_TRUE(tag.album()=="Avatar");
    EXPECT_TRUE(tag.year()=="2009");
    EXPECT_TRUE(tag.comment()=="Comment Field");
    EXPECT_TRUE(tag.genre()=="SoundTrack");
    EXPECT_TRUE(tag.track()=="11");
    EXPECT_TRUE(tag.disc()=="2");
    EXPECT_TRUE(tag.composer()=="James Horner (Composer)");
    EXPECT_TRUE(tag.originalArtist()=="James Cameron");

    Info::ImageFormat format;
    EXPECT_TRUE(tag.isImage());
    
    ImageInfoArray *imageData = tag.getImageData(format);
    ASSERT_TRUE(imageData!=0);
    EXPECT_FALSE(QImage::fromData(imageData->GetData(),imageData->GetSize()).isNull());
    
    EXPECT_EQ(tag.hashID(), 0); // TODO : get ID
}

//-------------------------------------------------------------------------------------------
