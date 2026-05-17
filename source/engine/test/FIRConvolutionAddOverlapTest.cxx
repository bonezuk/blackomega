#include "gtest/gtest.h"

#include "common/inc/DiskOps.h"
#include "common/inc/BIOBufferedStream.h"
#include "common/inc/BinaryArrayStream.h"
#include "engine/inc/Codec.h"
#include "engine/blueomega/inc/WaveEngine.h"
#include "engine/inc/FormatTypeFromFloat.h"
#include "engine/inc/FIRFilter.h"
#include "engine/inc/FFTRadix2_R2C.h"
#include "engine/inc/FFTRadix2_C2R.h"
#include "engine/inc/FIRFilterDB.h"
#include "engine/inc/FIRConvolutionAddOverlap.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(FIRConvolutionAddOverlap, halfLowPassFFTSameFrequency)
{
    const tfloat64 c_TOLERANCE = 0.00000001;

	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString sourceFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_org.wav");
    QString expectFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_lp12500Hz.wav");

	ASSERT_TRUE(common::DiskOps::exist(sourceFileName));
	
	engine::Codec *codec = engine::Codec::get(sourceFileName);
    ASSERT_TRUE(codec != NULL);
	ASSERT_TRUE(codec->init());
	
    engine::Codec *expectCodec = engine::Codec::get(expectFileName);
    ASSERT_TRUE(expectCodec != NULL);
	ASSERT_TRUE(expectCodec->init());
		
	const int c_blockSize = 4096;
	int noChannels = codec->noChannels();
	
	int lpCoeffSize;
	tfloat64 *lpCoeff = engine::getFIRFilterFromDB(engine::e_lowPassHalf_4097, lpCoeffSize);
	ASSERT_TRUE(lpCoeff != NULL);
	ASSERT_EQ(lpCoeffSize, 4097);

	engine::FIRConvolutionAddOverlap *filter[2];
	for(int fIdx = 0; fIdx < noChannels; fIdx++)
	{
		filter[fIdx] = new engine::FIRConvolutionAddOverlap();
		ASSERT_TRUE(filter[fIdx]->init(lpCoeff, c_blockSize + 1, c_blockSize));
	}

	tfloat64 *inL = new tfloat64 [c_blockSize];
	tfloat64 *inR = new tfloat64 [c_blockSize];
	tfloat64 *outL = new tfloat64 [c_blockSize];
	tfloat64 *outR = new tfloat64 [c_blockSize];

	engine::RData data(c_blockSize, codec->noChannels(), codec->noChannels());
    engine::RData expectData(c_blockSize, codec->noChannels(), codec->noChannels());

	bool loop = true;
	do
	{
		loop = codec->next(data);
		if(data.noParts() > 0)
		{
            ASSERT_EQ(expectCodec->next(expectData), loop);

			EXPECT_EQ(data.noParts(), 1);
			sample_t *x = data.partData(0);
			int idx;
			for(idx = 0; idx < data.part(0).length(); idx++)
			{
				inL[idx] = x[(idx * noChannels) + 0];
				inR[idx] = x[(idx * noChannels) + 1];
			}
			while(idx < c_blockSize)
			{
				inL[idx] = 0.0;
				inR[idx] = 0.0;
				idx++;
			}

			filter[0]->process(inL, outL);
			filter[1]->process(inR, outR);

            if(loop)
            {
                const tfloat64 *expect = expectData.partData(0);
                for(int i = 0; i < c_blockSize; i++)
                {
                    EXPECT_NEAR(expect[(idx << 1) + 0], outL[i], c_TOLERANCE);
                    EXPECT_NEAR(expect[(idx << 1) + 1], outR[i], c_TOLERANCE);
                }
            }

		}
		data.reset();
        expectData.reset();
	} while(loop);
	
	for(int i = 0; i < noChannels; i++)
	{
		delete filter[i];
	}
	delete [] outL;
	delete [] outR;
	delete [] inL;
	delete [] inR;

    codec->close();
    delete codec;
    expectCodec->close();
    delete expectCodec;
}

//-------------------------------------------------------------------------------------------
