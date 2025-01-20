#include "gtest/gtest.h"

#include "engine/dsdomega/inc/DSDCodec.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(DSDCodec, openDecodeAndCloseDSF)
{
	const tfloat64 c_tolerance = 0.0000001;
	const tuint8 expectDSDSamples[32] = {
		0xcd, 0xd2, 0xd2, 0x52, 0x55, 0x33, 0x33, 0xd3,
		0xab, 0xca, 0xaa, 0x2c, 0x33, 0x2b, 0xab, 0x2c,
		0x34, 0x55, 0xd3, 0xd4, 0x54, 0xd3, 0x54, 0x4d,
		0x2b, 0x33, 0xcb, 0x32, 0xb3, 0x32, 0xcb, 0xac
	};
	
	QString fileName = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"testexample1.dsf");
	
	ASSERT_TRUE(engine::Codec::isSupported(fileName));
	engine::Codec *codec = engine::Codec::get(fileName);
	ASSERT_TRUE(codec != NULL);
	engine::dsd::DSDCodec *dsdCodec = dynamic_cast<engine::dsd::DSDCodec *>(codec);
	ASSERT_TRUE(dsdCodec != NULL);
	
	EXPECT_EQ(codec->noChannels(), 2);
	EXPECT_EQ(codec->bitrate(), 2822400);
	EXPECT_EQ(codec->frequency(), 2822400);
	// (4096 * 8) / 2822400 = 0.011609977324263s.
	EXPECT_NEAR(static_cast<tfloat64>(codec->length()), 0.011609977324263, c_tolerance);

	ASSERT_EQ(codec->dataTypesSupported(), engine::e_SampleDSD8LSB);
	
	engine::RData data(256, 2, 2);
	ASSERT_TRUE(codec->next(data));
	EXPECT_EQ(data.noParts(), 1);
	EXPECT_EQ(::memcmp(data.partData(0), expectDSDSamples, 32), 0);
	EXPECT_NEAR(data.part(0).start(), 0.0, c_tolerance);
	EXPECT_NEAR(data.part(0).end(), 0.005804988662132, c_tolerance);
	
	data.reset();
	ASSERT_TRUE(codec->next(data));
	EXPECT_EQ(data.noParts(), 1);
	EXPECT_NEAR(data.part(0).start(), 0.005804988662132, c_tolerance);
	EXPECT_NEAR(data.part(0).end(), 0.011609977324263, c_tolerance);
	
	data.reset();
	EXPECT_FALSE(codec->next(data));
	
	EXPECT_TRUE(codec->isComplete());
	codec->close();
	delete codec;
}

//-------------------------------------------------------------------------------------------
