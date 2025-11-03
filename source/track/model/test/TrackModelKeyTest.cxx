#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/model/inc/TrackModelKey.h"

using namespace omega::track::model;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(TrackModelKey,constructorDefault)
{
    TrackModelKey k;
    EXPECT_TRUE(!k.isAlbum());
    EXPECT_TRUE(k.album().isAll());
    EXPECT_TRUE(!k.isTrack());
    EXPECT_TRUE(k.track()==-1);
    EXPECT_TRUE(!k.isSubtrack());
    EXPECT_TRUE(k.subtrack()==-1);
    EXPECT_TRUE(!k.isGenre());
    EXPECT_TRUE(k.genre()==-1);
    EXPECT_TRUE(!k.isArtist());
    EXPECT_TRUE(k.artist().isEmpty());
    EXPECT_TRUE(k.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(TrackModelKey,constructorCopy)
{
    AlbumModelKey keyA(std::pair<bool,int>(false,2));

    TrackModelKey a;
    a.album() = keyA;
    a.track() = 3;
    a.subtrack() = 5;
    a.genre() = 4;
    a.artist() = "Artist";
    TrackModelKey b(a);
    EXPECT_TRUE(b.album().isAlbum());
    EXPECT_TRUE(b.album().id()==2);
    EXPECT_TRUE(b.track()==3);
    EXPECT_TRUE(b.subtrack()==5);
    EXPECT_TRUE(b.genre()==4);
    EXPECT_TRUE(b.artist()=="Artist");
}

//-------------------------------------------------------------------------------------------

TEST(TrackModelKey,copyOperator)
{
    AlbumModelKey keyA(std::pair<bool,int>(false,2));

    TrackModelKey a;
    a.album() = keyA;
    a.track() = 3;
    a.subtrack() = 5;
    a.genre() = 4;
    a.artist() = "Artist";
    TrackModelKey b;
    b = a;
    EXPECT_TRUE(b.album().isAlbum());
    EXPECT_TRUE(b.album().id()==2);
    EXPECT_TRUE(b.track()==3);
    EXPECT_TRUE(b.subtrack()==5);
    EXPECT_TRUE(b.genre()==4);
    EXPECT_TRUE(b.artist()=="Artist");
}

//-------------------------------------------------------------------------------------------

TEST(TrackModelKey,isAlbum)
{
    TrackModelKey a;
    a.album() = AlbumModelKey(std::pair<bool,int>(false,2));
    EXPECT_TRUE(a.isAlbum());
    a.album() = AlbumModelKey();
    EXPECT_FALSE(a.isAlbum());
}

//-------------------------------------------------------------------------------------------

TEST(TrackModelKey,isTrack)
{
    TrackModelKey a;
    a.album() = AlbumModelKey();
    a.track() = 0;
    EXPECT_FALSE(a.isTrack());
    a.album() = AlbumModelKey(std::pair<bool,int>(false,2));
    a.track() = -1;
    EXPECT_FALSE(a.isTrack());
    a.album() = AlbumModelKey(std::pair<bool,int>(false,2));
    a.track() = 0;
    EXPECT_TRUE(a.isTrack());
    a.album() = AlbumModelKey(std::pair<bool,int>(false,2));
    a.track() = 2;
    EXPECT_TRUE(a.isTrack());
}

//-------------------------------------------------------------------------------------------

TEST(TrackModelKey,isSubtrack)
{
    TrackModelKey a;
    a.album() = AlbumModelKey(std::pair<bool,int>(false,2));
    a.track() = 2;
    a.subtrack() = -1;
    EXPECT_FALSE(a.isSubtrack());
    a.album() = AlbumModelKey(std::pair<bool,int>(false,2));
    a.track() = -1;
    a.subtrack() = 2;
    EXPECT_FALSE(a.isSubtrack());
    a.album() = AlbumModelKey(std::pair<bool,int>(false,2));
    a.track() = 2;
    a.subtrack() = -1;
    EXPECT_FALSE(a.isSubtrack());
    a.album() = AlbumModelKey(std::pair<bool,int>(false,2));
    a.track() = 0;
    a.subtrack() = 0;
    EXPECT_TRUE(a.isSubtrack());
    a.album() = AlbumModelKey(std::pair<bool,int>(false,2));
    a.track() = 3;
    a.subtrack() = 1;
    EXPECT_TRUE(a.isSubtrack());
}

//-------------------------------------------------------------------------------------------

TEST(TrackModelKey,isGenre)
{
    TrackModelKey a;
    a.genre() = 0;
    EXPECT_TRUE(a.isGenre());
    a.genre() = -1;
    EXPECT_FALSE(a.isGenre());
    a.genre() = 1;
    EXPECT_TRUE(a.isGenre());
}

//-------------------------------------------------------------------------------------------

TEST(TrackModelKey,isArtist)
{
    TrackModelKey a;
    a.artist() = "Artist";
    EXPECT_TRUE(a.isArtist());
    a.artist() = "";
    EXPECT_FALSE(a.isArtist());
}

//-------------------------------------------------------------------------------------------

TEST(TrackModelKey,isEmpty)
{
    TrackModelKey a;
    EXPECT_TRUE(a.isEmpty());
    a.album() = AlbumModelKey(std::pair<bool,int>(false,2));
    EXPECT_FALSE(a.isEmpty());
    TrackModelKey b;
    b.genre() = 1;
    EXPECT_FALSE(b.isEmpty());
    TrackModelKey c;
    c.artist() = "Artist";
    EXPECT_FALSE(c.isEmpty());
    TrackModelKey d;
    EXPECT_TRUE(d.isEmpty());
}

//-------------------------------------------------------------------------------------------
