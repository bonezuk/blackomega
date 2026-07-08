#include "gtest/gtest.h"

#include "audioio/inc/ALSAStreamParser.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;
using namespace omega::audioio;

//-------------------------------------------------------------------------------------------

QString testGetALSAStreamFileName(const QString& name)
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	return common::DiskOps::mergeName(testEnv->getDBDirectory(), name);
}

//-------------------------------------------------------------------------------------------

TEST(ALSAStreamParser, parseStreamForIFIAmp)
{
	QString streamName = testGetALSAStreamFileName("ifi1_stream0.txt");
	ALSAStreamParser parser;
	EXPECT_TRUE(parser.parse(streamName));
	EXPECT_TRUE(parser.deviceName() == "iFi (by AMR) iFi (by AMR) HD USB Audio");
	EXPECT_TRUE(parser.isDSDSpecial());
	EXPECT_EQ(parser.noBits(), 32);
	EXPECT_TRUE(parser.isDSDOverPCM());
	EXPECT_FALSE(parser.isMSB());
	EXPECT_TRUE(parser.isLSB());
}

//-------------------------------------------------------------------------------------------

TEST(ALSAStreamParser, parseStreamForEVO150Amp)
{
	QString streamName = testGetALSAStreamFileName("evo150_stream0.txt");
	ALSAStreamParser parser;
	EXPECT_TRUE(parser.parse(streamName));
	EXPECT_TRUE(parser.deviceName() == "Cambridge Audio Evo 150");
	EXPECT_TRUE(parser.isDSDSpecial());
	EXPECT_EQ(parser.noBits(), 32);
	EXPECT_FALSE(parser.isDSDOverPCM());
	EXPECT_TRUE(parser.isMSB());
	EXPECT_FALSE(parser.isLSB());
}

//-------------------------------------------------------------------------------------------

TEST(ALSAStreamParser, parseStreamFailsWhenFileDoesNotExist)
{
	QString streamName = testGetALSAStreamFileName("no_stream0.txt");
	ALSAStreamParser parser;
	EXPECT_FALSE(parser.parse(streamName));
	EXPECT_TRUE(parser.deviceName().isEmpty());
	EXPECT_FALSE(parser.isDSDSpecial());
	EXPECT_EQ(parser.noBits(), 0);
	EXPECT_FALSE(parser.isDSDOverPCM());
	EXPECT_TRUE(parser.isMSB());
	EXPECT_FALSE(parser.isLSB());
}

//-------------------------------------------------------------------------------------------
