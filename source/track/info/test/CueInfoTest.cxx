#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/model/test/TrackDBTestEnviroment.h"
#include "common/inc/DiskOps.h"
#include "track/info/inc/CueInfo.h"

using namespace omega;
using namespace omega::track::info;
using namespace testing;

//-------------------------------------------------------------------------------------------

class CueInfoTest : public CueInfo
{
    public:
        CueInfoTest();
        virtual ~CueInfoTest();

        virtual bool testIsSeparateCuesheet(const QString& trackFileName);
        virtual QString testSeparateCueFilename(const QString& trackFileName);

        virtual bool testReadCueSheet(const QString& fileName);

        virtual void setLength(const common::TimeStamp& l);

        MOCK_CONST_METHOD0(canGetTrackLength,bool());
        MOCK_METHOD0(getTrackLength,common::TimeStamp());
        MOCK_CONST_METHOD0(isPreGapPlayed,bool());
};

//-------------------------------------------------------------------------------------------

CueInfoTest::CueInfoTest() : CueInfo()
{}

//-------------------------------------------------------------------------------------------

CueInfoTest::~CueInfoTest()
{}

//-------------------------------------------------------------------------------------------

bool CueInfoTest::testIsSeparateCuesheet(const QString& trackFileName)
{
    return isSeparateCuesheet(trackFileName);
}

//-------------------------------------------------------------------------------------------

QString CueInfoTest::testSeparateCueFilename(const QString& trackFileName)
{
    return separateCueFilename(trackFileName);
}

//-------------------------------------------------------------------------------------------

bool CueInfoTest::testReadCueSheet(const QString& fileName)
{
    return readCueSheet(fileName);
}

//-------------------------------------------------------------------------------------------

void CueInfoTest::setLength(const common::TimeStamp& l)
{
    CueInfo::setLength(l);
}

//-------------------------------------------------------------------------------------------

TEST(CueInfo,readCueSheetWithNoGapAndCanGetTrackLength)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(),"hobbit.cue");
    QString mediaFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(),"hobbit.wv");

    Info::setPlayPreGap(false);

    common::TimeStamp cLen;
    common::TimeStamp t0(0.0);
    common::TimeStamp t1(201.49333333333333);
    common::TimeStamp t2(497.86666666666667);
    common::TimeStamp t3(672.86666666666667);
    common::TimeStamp t4(965.08);
    common::TimeStamp t5(1296.50666666666667);
    common::TimeStamp t6(1871.86666666666667);
    common::TimeStamp t7(2186.96);
    common::TimeStamp t8(2356.56);
    common::TimeStamp t9(2466.17333333333333);
    common::TimeStamp t10(2775.33333333333333);
    common::TimeStamp t11(2973.77333333333333);
    common::TimeStamp t12(3193.06666666666667);
    common::TimeStamp t13(3392.56);
    common::TimeStamp tLen(3608.56);

    CueInfoTest cueInfo;
    EXPECT_CALL(cueInfo,canGetTrackLength()).WillRepeatedly(Return(true));
    EXPECT_CALL(cueInfo,getTrackLength()).WillRepeatedly(Return(tLen));
    EXPECT_CALL(cueInfo,isPreGapPlayed()).WillRepeatedly(Return(true));

    ASSERT_TRUE(cueInfo.testIsSeparateCuesheet(mediaFileName));
    EXPECT_TRUE(cueInfo.testSeparateCueFilename(mediaFileName)==fileName);

    ASSERT_TRUE(cueInfo.testReadCueSheet(fileName));

    EXPECT_TRUE(cueInfo.title()=="The Hobbit - The Desolation of Smaug");
    EXPECT_TRUE(cueInfo.album()=="The Hobbit - The Desolation of Smaug");
    EXPECT_TRUE(cueInfo.artist()=="Howard Shore");
    EXPECT_TRUE(cueInfo.genre()=="Soundtrack");
    EXPECT_TRUE(cueInfo.year()=="2013");

    ASSERT_TRUE(cueInfo.isChildren());
    ASSERT_EQ(14,cueInfo.noChildren());

    cLen = t1 - t0;
    EXPECT_TRUE(cueInfo.child(0).name()=="The Quest For Erebor");
    EXPECT_NEAR(static_cast<tfloat64>(t0),static_cast<tfloat64>(cueInfo.child(0).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(0).length()),0.00001);

    cLen = t2 - t1;
    EXPECT_TRUE(cueInfo.child(1).name()=="Wilderland");
    EXPECT_NEAR(static_cast<tfloat64>(t1),static_cast<tfloat64>(cueInfo.child(1).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(1).length()),0.00001);

    cLen = t3 - t2;
    EXPECT_TRUE(cueInfo.child(2).name()=="A Necromancer");
    EXPECT_NEAR(static_cast<tfloat64>(t2),static_cast<tfloat64>(cueInfo.child(2).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(2).length()),0.00001);

    cLen = t4 - t3;
    EXPECT_TRUE(cueInfo.child(3).name()=="The House Of Beorn (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(t3),static_cast<tfloat64>(cueInfo.child(3).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(3).length()),0.00001);

    cLen = t5 - t4;
    EXPECT_TRUE(cueInfo.child(4).name()=="Mirkwood (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(t4),static_cast<tfloat64>(cueInfo.child(4).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(4).length()),0.00001);

    cLen = t6 - t5;
    EXPECT_TRUE(cueInfo.child(5).name()=="Flies And Spiders (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(t5),static_cast<tfloat64>(cueInfo.child(5).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(5).length()),0.00001);

    cLen = t7 - t6;
    EXPECT_TRUE(cueInfo.child(6).name()=="The Woodland Realm (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(t6),static_cast<tfloat64>(cueInfo.child(6).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(6).length()),0.00001);

    cLen = t8 - t7;
    EXPECT_TRUE(cueInfo.child(7).name()=="Feast Of Starlight");
    EXPECT_NEAR(static_cast<tfloat64>(t7),static_cast<tfloat64>(cueInfo.child(7).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(7).length()),0.00001);

    cLen = t9 - t8;
    EXPECT_TRUE(cueInfo.child(8).name()=="Barrels Out Of Bond");
    EXPECT_NEAR(static_cast<tfloat64>(t8),static_cast<tfloat64>(cueInfo.child(8).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(8).length()),0.00001);

    cLen = t10 - t9;
    EXPECT_TRUE(cueInfo.child(9).name()=="The Forest River (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(t9),static_cast<tfloat64>(cueInfo.child(9).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(9).length()),0.00001);

    cLen = t11 - t10;
    EXPECT_TRUE(cueInfo.child(10).name()=="Bard, A Man Of Lake-Town (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(t10),static_cast<tfloat64>(cueInfo.child(10).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(10).length()),0.00001);

    cLen = t12 - t11;
    EXPECT_TRUE(cueInfo.child(11).name()=="The High Fells [Extended Version]");
    EXPECT_NEAR(static_cast<tfloat64>(t11),static_cast<tfloat64>(cueInfo.child(11).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(11).length()),0.00001);

    cLen = t13 - t12;
    EXPECT_TRUE(cueInfo.child(12).name()=="The Nature Of Evil");
    EXPECT_NEAR(static_cast<tfloat64>(t12),static_cast<tfloat64>(cueInfo.child(12).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(12).length()),0.00001);

    cLen = tLen - t13;
    EXPECT_TRUE(cueInfo.child(13).name()=="Protector Of The Common Folk");
    EXPECT_NEAR(static_cast<tfloat64>(t13),static_cast<tfloat64>(cueInfo.child(13).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(13).length()),0.00001);
}

//-------------------------------------------------------------------------------------------

TEST(CueInfo,readCueSheetWithPreGapAndCannotGetTrackLength)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(),"hobbit.cue");
    QString mediaFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(),"hobbit.wv");

    Info::setPlayPreGap(true);

    common::TimeStamp cLen;
    common::TimeStamp t0(0.0);
    common::TimeStamp t1(201.49333333333333),tB1(203.74666666666667);
    common::TimeStamp t2(497.86666666666667),tB2(500.16);
    common::TimeStamp t3(672.86666666666667),tB3(674.84);
    common::TimeStamp t4(965.08),tB4(967.37333333333333);
    common::TimeStamp t5(1296.50666666666667),tB5(1298.72);
    common::TimeStamp t6(1871.86666666666667),tB6(1874.22666666666667);
    common::TimeStamp t7(2186.96),tB7(2189.2);
    common::TimeStamp t8(2356.56),tB8(2357.2);
    common::TimeStamp t9(2466.17333333333333),tB9(2467.22666666666667);
    common::TimeStamp t10(2775.33333333333333),tB10(2777.85333333333333);
    common::TimeStamp t11(2973.77333333333333),tB11(2976.26666666666667);
    common::TimeStamp t12(3193.06666666666667),tB12(3195.10666666666667);
    common::TimeStamp t13(3392.56),tB13(3395.90666666666667);
    common::TimeStamp tLen(3611.90666666666667);

    CueInfoTest cueInfo;
    EXPECT_CALL(cueInfo,canGetTrackLength()).WillRepeatedly(Return(false));
    EXPECT_CALL(cueInfo,isPreGapPlayed()).WillRepeatedly(Return(false));

    ASSERT_TRUE(cueInfo.testIsSeparateCuesheet(mediaFileName));
    EXPECT_TRUE(cueInfo.testSeparateCueFilename(mediaFileName)==fileName);

    ASSERT_TRUE(cueInfo.testReadCueSheet(fileName));

    cueInfo.setLength(tLen);

    EXPECT_TRUE(cueInfo.title()=="The Hobbit - The Desolation of Smaug");
    EXPECT_TRUE(cueInfo.album()=="The Hobbit - The Desolation of Smaug");
    EXPECT_TRUE(cueInfo.artist()=="Howard Shore");
    EXPECT_TRUE(cueInfo.genre()=="Soundtrack");
    EXPECT_TRUE(cueInfo.year()=="2013");

    ASSERT_TRUE(cueInfo.isChildren());
    ASSERT_EQ(14,cueInfo.noChildren());

    cLen = t1 - t0;
    EXPECT_TRUE(cueInfo.child(0).name()=="The Quest For Erebor");
    EXPECT_NEAR(static_cast<tfloat64>(t0),static_cast<tfloat64>(cueInfo.child(0).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(0).length()),0.00001);

    cLen = t2 - tB1;
    EXPECT_TRUE(cueInfo.child(1).name()=="Wilderland");
    EXPECT_NEAR(static_cast<tfloat64>(tB1),static_cast<tfloat64>(cueInfo.child(1).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(1).length()),0.00001);

    cLen = t3 - tB2;
    EXPECT_TRUE(cueInfo.child(2).name()=="A Necromancer");
    EXPECT_NEAR(static_cast<tfloat64>(tB2),static_cast<tfloat64>(cueInfo.child(2).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(2).length()),0.00001);

    cLen = t4 - tB3;
    EXPECT_TRUE(cueInfo.child(3).name()=="The House Of Beorn (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(tB3),static_cast<tfloat64>(cueInfo.child(3).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(3).length()),0.00001);

    cLen = t5 - tB4;
    EXPECT_TRUE(cueInfo.child(4).name()=="Mirkwood (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(tB4),static_cast<tfloat64>(cueInfo.child(4).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(4).length()),0.00001);

    cLen = t6 - tB5;
    EXPECT_TRUE(cueInfo.child(5).name()=="Flies And Spiders (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(tB5),static_cast<tfloat64>(cueInfo.child(5).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(5).length()),0.00001);

    cLen = t7 - tB6;
    EXPECT_TRUE(cueInfo.child(6).name()=="The Woodland Realm (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(tB6),static_cast<tfloat64>(cueInfo.child(6).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(6).length()),0.00001);

    cLen = t8 - tB7;
    EXPECT_TRUE(cueInfo.child(7).name()=="Feast Of Starlight");
    EXPECT_NEAR(static_cast<tfloat64>(tB7),static_cast<tfloat64>(cueInfo.child(7).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(7).length()),0.00001);

    cLen = t9 - tB8;
    EXPECT_TRUE(cueInfo.child(8).name()=="Barrels Out Of Bond");
    EXPECT_NEAR(static_cast<tfloat64>(tB8),static_cast<tfloat64>(cueInfo.child(8).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(8).length()),0.00001);

    cLen = t10 - tB9;
    EXPECT_TRUE(cueInfo.child(9).name()=="The Forest River (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(tB9),static_cast<tfloat64>(cueInfo.child(9).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(9).length()),0.00001);

    cLen = t11 - tB10;
    EXPECT_TRUE(cueInfo.child(10).name()=="Bard, A Man Of Lake-Town (Extended Version)");
    EXPECT_NEAR(static_cast<tfloat64>(tB10),static_cast<tfloat64>(cueInfo.child(10).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(10).length()),0.00001);

    cLen = t12 - tB11;
    EXPECT_TRUE(cueInfo.child(11).name()=="The High Fells [Extended Version]");
    EXPECT_NEAR(static_cast<tfloat64>(tB11),static_cast<tfloat64>(cueInfo.child(11).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(11).length()),0.00001);

    cLen = t13 - tB12;
    EXPECT_TRUE(cueInfo.child(12).name()=="The Nature Of Evil");
    EXPECT_NEAR(static_cast<tfloat64>(tB12),static_cast<tfloat64>(cueInfo.child(12).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(12).length()),0.00001);

    cLen = tLen - tB13;
    EXPECT_TRUE(cueInfo.child(13).name()=="Protector Of The Common Folk");
    EXPECT_NEAR(static_cast<tfloat64>(tB13),static_cast<tfloat64>(cueInfo.child(13).startTime()),0.00001);
    EXPECT_NEAR(static_cast<tfloat64>(cLen),static_cast<tfloat64>(cueInfo.child(13).length()),0.00001);
}

//-------------------------------------------------------------------------------------------
