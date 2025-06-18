#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/model/inc/AlbumTrackModel.h"

using namespace omega::track::model;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AlbumTrackModelTest : public AlbumTrackModel
{
    public:
        AlbumTrackModelTest();
        AlbumTrackModelTest(const TrackModelKey& filterKey);
        virtual ~AlbumTrackModelTest();
        const AlbumModelKey& testGetAlbumID() const;
};

//-------------------------------------------------------------------------------------------

AlbumTrackModelTest::AlbumTrackModelTest() : AlbumTrackModel()
{}

//-------------------------------------------------------------------------------------------

AlbumTrackModelTest::AlbumTrackModelTest(const TrackModelKey& filterKey) : AlbumTrackModel(filterKey)
{}

//-------------------------------------------------------------------------------------------

AlbumTrackModelTest::~AlbumTrackModelTest()
{}

//-------------------------------------------------------------------------------------------

const AlbumModelKey& AlbumTrackModelTest::testGetAlbumID() const
{
    return getAlbumID();
}

//-------------------------------------------------------------------------------------------

TEST(AlbumTrackModel,constructorDefault)
{
    AlbumTrackModel album;
    EXPECT_TRUE(album.type()==e_modelAlbumTracks);
    EXPECT_TRUE(album.numberSections()==1);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumTrackModel,getAlbumID)
{
    AlbumModelKey albumKey(std::pair<bool,int>(true,3));
    TrackModelKey key;
    key.album() = albumKey;

    AlbumTrackModelTest album(key);
    EXPECT_TRUE(album.testGetAlbumID()==albumKey);
}

//-------------------------------------------------------------------------------------------
