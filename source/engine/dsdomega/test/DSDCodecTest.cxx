#include "gtest/gtest.h"

#include "engine/dsdomega/inc/DSDCodec.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "test/UnitTestEnviroment.h"

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
	ASSERT_TRUE(codec->init());
	engine::dsd::DSDCodec *dsdCodec = dynamic_cast<engine::dsd::DSDCodec *>(codec);
	ASSERT_TRUE(dsdCodec != NULL);
	
	EXPECT_EQ(codec->noChannels(), 2);
	EXPECT_EQ(codec->bitrate(), 2822400);
	EXPECT_EQ(codec->frequency(), 2822400);
	// (4096 * 8) / 2822400 = 0.011609977324263s.
	EXPECT_NEAR(static_cast<tfloat64>(codec->length()), 0.011609977324263, c_tolerance);

	ASSERT_EQ(codec->dataTypesSupported(), engine::e_SampleDSD8LSB | engine::e_SampleFloat | engine::e_SampleInt24 | engine::e_SampleInt32);
	ASSERT_TRUE(codec->setDataTypeFormat(engine::e_SampleDSD8LSB));
	
	engine::RData data(256, 2, 2);
	ASSERT_TRUE(codec->next(data));
	EXPECT_EQ(data.noParts(), 1);
	EXPECT_EQ(::memcmp(data.partData(0), expectDSDSamples, 32), 0);
	EXPECT_NEAR(data.part(0).start(), 0.0, c_tolerance);
	EXPECT_NEAR(data.part(0).end(), 0.005804988662132, c_tolerance);
	EXPECT_EQ(data.part(0).getDataType(), engine::e_SampleDSD8LSB);
	data.reset();
	
	ASSERT_TRUE(codec->next(data));
	EXPECT_EQ(data.noParts(), 1);
	EXPECT_NEAR(data.part(0).start(), 0.005804988662132, c_tolerance);
	EXPECT_NEAR(data.part(0).end(), 0.011609977324263, c_tolerance);
	EXPECT_EQ(data.part(0).getDataType(), engine::e_SampleDSD8LSB);
	
	data.reset();
	EXPECT_FALSE(codec->next(data));
	
	EXPECT_TRUE(codec->isComplete());
	codec->close();
	delete codec;
}

//-------------------------------------------------------------------------------------------

void openDecodeDSFWithDSDOverPCM(engine::CodecDataType type)
{
	const tfloat64 c_tolerance = 0.0000001;
	
	const tuint32 c_expectDSD24Bit[16] = {
		0x0005d2cd, // 0
		0x0005caab, // 1
		0xfffa52d2, // 2
		0xfffa2caa, // 3
		0x00053355, // 4
		0x00052b33, // 5
		0xfffad333, // 6
		0xfffa2cab, // 7
		0x00055534, // 8
		0x0005332b, // 9
		0xfffad4d3, // 10
		0xfffa32cb, // 11
		0x0005d354, // 12
		0x000532b3, // 13
		0xfffa4d54, // 14
		0xfffaaccb  // 15
	};
	
	QString fileName = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"testexample1.dsf");
	
	ASSERT_TRUE(engine::Codec::isSupported(fileName));
	engine::Codec *codec = engine::Codec::get(fileName);
	ASSERT_TRUE(codec != NULL);
	ASSERT_TRUE(codec->init());
	engine::dsd::DSDCodec *dsdCodec = dynamic_cast<engine::dsd::DSDCodec *>(codec);
	ASSERT_TRUE(dsdCodec != NULL);
	
	EXPECT_EQ(codec->noChannels(), 2);
	EXPECT_EQ(codec->bitrate(), 2822400);
	EXPECT_EQ(codec->frequency(), 2822400);
	ASSERT_EQ(codec->dataTypesSupported(), engine::e_SampleDSD8LSB | engine::e_SampleFloat | engine::e_SampleInt24 | engine::e_SampleInt32);
	
	ASSERT_TRUE(codec->setDataTypeFormat(type));

	EXPECT_EQ(codec->bitrate(), 2822400);
	EXPECT_EQ(codec->frequency(), 176400);
	ASSERT_EQ(codec->dataTypesSupported(), type);
	
	engine::RData data(256, 2, 2);
	ASSERT_TRUE(codec->next(data));
	EXPECT_EQ(data.noParts(), 1);
	EXPECT_EQ(::memcmp(data.partData(0), c_expectDSD24Bit, 16 * sizeof(tuint32)), 0);
	EXPECT_NEAR(data.part(0).start(), 0.0, c_tolerance);
	EXPECT_NEAR(data.part(0).end(), 0.00145124716553287981859410430839, c_tolerance);
	EXPECT_EQ(data.part(0).getDataType(), type);

	codec->close();
	delete codec;
}

//-------------------------------------------------------------------------------------------

TEST(DSDCodec, openDecodeDSFWith24BitDSDOverPCM)
{
	openDecodeDSFWithDSDOverPCM(engine::e_SampleInt24);
}

//-------------------------------------------------------------------------------------------

TEST(DSDCodec, openDecodeDSFWith32BitDSDOverPCM)
{
	openDecodeDSFWithDSDOverPCM(engine::e_SampleInt32);
}

//-------------------------------------------------------------------------------------------

void testDSDCodecAgainstFLACUsingPCM(const QString& dFilename, const QString& fFilename, int pcmFrequency)
{
	const tfloat64 c_tolerance = 0.0005;
		
	QString inFilename = common::DiskOps::mergeName("engine/dsdomega/test/samples", dFilename);
	QString dsfName = test::UnitTestEnviroment::instance()->testFileName(inFilename);
	ASSERT_FALSE(dsfName.isEmpty());
	inFilename = common::DiskOps::mergeName("engine/dsdomega/test/samples", fFilename);
	QString flacName = test::UnitTestEnviroment::instance()->testFileName(inFilename);
	ASSERT_FALSE(flacName.isEmpty());
	
	ASSERT_TRUE(engine::Codec::isSupported(dsfName));
	engine::Codec *codec = engine::Codec::get(dsfName);
	ASSERT_TRUE(codec != NULL);
	ASSERT_TRUE(codec->init());
	
	EXPECT_EQ(codec->noChannels(), 2);
	EXPECT_EQ(codec->bitrate(), 2822400);
	EXPECT_EQ(codec->frequency(), 2822400);
	ASSERT_EQ(codec->dataTypesSupported(), engine::e_SampleDSD8LSB | engine::e_SampleFloat | engine::e_SampleInt24 | engine::e_SampleInt32);
	
	engine::dsd::DSDCodec *dsdCodec = dynamic_cast<engine::dsd::DSDCodec *>(codec);
	ASSERT_TRUE(dsdCodec != NULL);
	ASSERT_TRUE(dsdCodec->setOutputPCM(pcmFrequency));

	engine::Codec *flacCodec = engine::Codec::get(flacName);
	ASSERT_TRUE(flacCodec != NULL);
	ASSERT_TRUE(flacCodec->init());
	
	ASSERT_EQ(codec->bitrate(), 2822400);
	ASSERT_EQ(codec->frequency(), pcmFrequency);
	ASSERT_EQ(flacCodec->frequency(), pcmFrequency);
	ASSERT_EQ(codec->dataTypesSupported(), engine::e_SampleFloat);

	engine::RData dData(1024, 2, 2);
	engine::RData fData(1024, 2, 2);
	
	int samplesRead = 0;
	bool loop = true;
	do
	{
		if(dData.noParts() == 0)
		{
			if(!codec->next(dData))
				loop = false;
		}
		if(fData.noParts() == 0)
		{
			if(!flacCodec->next(fData))
				loop = false;
		}
		if(dData.noParts() > 0 && fData.noParts() > 0)
		{
			engine::RData::Part& dPart = dData.part(0);
			engine::RData::Part& fPart = fData.part(0);
			EXPECT_EQ(dData.noParts(), 1);
			EXPECT_EQ(fData.noParts(), 1);
			common::TimeStamp expectTS = static_cast<tfloat64>(samplesRead) / static_cast<tfloat64>(pcmFrequency);
			common::TimeStamp expectTE = static_cast<tfloat64>(samplesRead + fPart.length()) / static_cast<tfloat64>(pcmFrequency);
			EXPECT_NEAR(static_cast<tfloat64>(dPart.start()), static_cast<tfloat64>(expectTS), c_tolerance);
			EXPECT_NEAR(static_cast<tfloat64>(dPart.end()), static_cast<tfloat64>(expectTE), c_tolerance);
			
			int len = (dPart.length() < fPart.length()) ? dPart.length() : fPart.length();
			const sample_t *dOut = dData.partData(0);
			const sample_t *fOut = fData.partData(0);
			for(int i = 0; i < len; i++)
			{
				int idx = i << 1;
				EXPECT_NEAR(dOut[idx + 0], fOut[idx + 0], c_tolerance);
				EXPECT_NEAR(dOut[idx + 1], fOut[idx + 1], c_tolerance);
			}
			samplesRead += len;
			dData.reset();
			fData.reset();
		}
	} while(loop);
	
	EXPECT_TRUE(codec->isComplete());
	codec->close();
	delete codec;
	
	flacCodec->close();
	delete flacCodec;
}

//-------------------------------------------------------------------------------------------

TEST(DSDCodec, openDecodeDSFAsPCMAt352800Hz)
{
	testDSDCodecAgainstFLACUsingPCM("test1.dsf", "test1_352kHz.flac", 352800);
}

//-------------------------------------------------------------------------------------------

TEST(DSDCodec, openDecodeDSFAsPCMAt176400Hz)
{
	testDSDCodecAgainstFLACUsingPCM("test1.dsf", "test1_176kHz.flac", 176400);
}

//-------------------------------------------------------------------------------------------

TEST(DSDCodec, openDecodeDSFAsPCMAt88200Hz)
{
	testDSDCodecAgainstFLACUsingPCM("test1.dsf", "test1_88kHz.flac", 88200);
}

//-------------------------------------------------------------------------------------------
