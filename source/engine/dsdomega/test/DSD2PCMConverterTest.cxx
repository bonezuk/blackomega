#include "gtest/gtest.h"

#include "engine/inc/Codec.h"
#include "engine/dsdomega/inc/DSD2PCMConverter.h"
#include "test/inc/UnitTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

void runConvertDSD64ToPCMTest(int pcmFrequency, const QString& dFilename, const QString& fFilename)
{
	const tfloat64 c_tolerance = 0.00001;

	QString inFilename = common::mergeName("engine/dsdomega/test/samples", dFilename);
	QString dsfName = test::UnitTestEnviroment::instance()->testFileName(inFilename);
	ASSERT_FALSE(dsfName.isEmpty());
	inFilename = common::mergeName("engine/dsdomega/test/samples", fFilename);
	QString flacName = test::UnitTestEnviroment::instance()->testFileName(inFilename);
	ASSERT_FALSE(flacName.isEmpty());

	common::BIOStream ioFile(common::e_BIOStream_FileRead);
	ASSERT_TRUE(ioFile.open(dsfName));
	engine::dsd::DSFFileReader dsf(&ioFile);
	ASSERT_TRUE(dsf.parse());
	
	ASSERT_EQ(dsf.numberOfChannels(), 2);
	ASSERT_EQ(dsf.frequency(), 2822400);
	ASSERT_TRUE(dsf.isLSB());

	engine::dsd::DSD2PCMConverter converter[2];
	ASSERT_TRUE(converter[0].setup(dsf.frequency(), pcmFrequency, true));
	ASSERT_TRUE(converter[1].setup(dsf.frequency(), pcmFrequency, true));
	
	engine::Codec *compCodec = engine::Codec::get(flacName);
	ASSERT_TRUE(compCodec != NULL);
	ASSERT_TRUE(compCodec->setDataTypeFormat(engine::e_SampleFloat));
	ASSERT_EQ(compCodec->frequency(), pcmFrequency);
	
	int partIdx = 0, dOffset = 0, fOffset = 0;
	QByteArray inArray, outArray[2];
	engine::RData fData(1024, 2, 2);
	
	for(tint blockIdx = 0; blockIdx < dsf.numberOfBlocks(); blockIdx++)
	{
		for(int ch = 0; ch < 2; ch++)
		{
			ASSERT_TRUE(dsf.data(blockIdx, ch, inArray));
			converter[ch].push(inArray);
			converter[ch].pull(outArray[ch], ((blockIdx + 1) == dsf.numberOfBlocks()));
			dOffset = 0;
		}
		ASSERT_EQ(outArray[0].size(), outArray[1].size());
		
		tint noDSamples = outArray[0].size() / (2 * sizeof(sample_t));
		while(dOffset < noDSamples)
		{
			const tfloat64 *dPCM_L = reinterpret_cast<const tfloat64 *>(outArray[0].constData());
			const tfloat64 *dPCM_R = reinterpret_cast<const tfloat64 *>(outArray[1].constData());

			if(partIdx >= fData.noParts())
			{
				fData.reset();
				compCodec->next(fData);
				ASSERT_TRUE(fData.noParts() > 0);
				partIdx = 0;
				fOffset = 0;
			}
			while(partIdx < fData.noParts() && dOffset < noDSamples)
			{
				int noFSamples = fData.part(partIdx).length();
				const tfloat64 *fPCM = reinterpret_cast<const tfloat64 *>(fData.partDataConst(partIdx));

				while(dOffset < noDSamples && fOffset < noFSamples)
				{
					tfloat64 dL = dPCM_L[dOffset];
					tfloat64 dR = dPCM_R[dOffset];
					tfloat64 fL = fPCM[fOffset << 1];
					tfloat64 fR = fPCM[(fOffset << 1) + 1];
					EXPECT_NEAR(dL, fL, c_tolerance);
					EXPECT_NEAR(dR, fR, c_tolerance);
					dOffset++;
					fOffset++;
				}
				if(fOffset >= noFSamples)
				{
					fOffset = 0;
					partIdx++;
				}
			}
		}
	}	
	
	delete compCodec;
	ioFile.close();
}

//-------------------------------------------------------------------------------------------

TEST(DSD2PCMConverter, convertDSD64To352800Hz)
{
	runConvertDSD64ToPCMTest(352800, "test1.dsf", "test1_352kHz.flac");
}

//-------------------------------------------------------------------------------------------

TEST(DSD2PCMConverter, convertDSD64To176400Hz)
{
	runConvertDSD64ToPCMTest(352800, "test1.dsf", "test1_176kHz.flac");
}

//-------------------------------------------------------------------------------------------

TEST(DSD2PCMConverter, convertDSD64To88200Hz)
{
	runConvertDSD64ToPCMTest(352800, "test1.dsf", "test1_88kHz.flac");
}

//-------------------------------------------------------------------------------------------
