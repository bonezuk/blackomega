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
                    EXPECT_NEAR(expect[(i << 1) + 0], outL[i], c_TOLERANCE);
                    EXPECT_NEAR(expect[(i << 1) + 1], outR[i], c_TOLERANCE);
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

TEST(FIRConvolutionAddOverlap, halfLowPassFFTDoubleFrequency)
{
    const tfloat64 c_TOLERANCE = 0.00000001;

	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString sourceFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_org.wav");
    QString expectFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_upx2.wav");

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

	engine::RData data(c_blockSize / 2, codec->noChannels(), codec->noChannels());
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
				inL[(idx << 1) + 0] = x[(idx * noChannels) + 0];
				inL[(idx << 1) + 1] = 0.0;
				inR[(idx << 1) + 0] = x[(idx * noChannels) + 1];
				inR[(idx << 1) + 1] = 0.0;
			}
			idx <<= 1;
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
					// apply *2 gain
					outL[i] *= 2.0;
					outR[i] *= 2.0;
                    EXPECT_NEAR(expect[(i << 1) + 0], outL[i], c_TOLERANCE);
                    EXPECT_NEAR(expect[(i << 1) + 1], outR[i], c_TOLERANCE);
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

TEST(FIRConvolutionAddOverlap, halfLowPassFFTQuadripleFrequency)
{
    const tfloat64 c_TOLERANCE = 0.00000001;

	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString sourceFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_org.wav");
    QString expectFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_upx4.wav");

	ASSERT_TRUE(common::DiskOps::exist(sourceFileName));
	
	engine::Codec *codec = engine::Codec::get(sourceFileName);
    ASSERT_TRUE(codec != NULL);
	ASSERT_TRUE(codec->init());
	
    engine::Codec *expectCodec = engine::Codec::get(expectFileName);
    ASSERT_TRUE(expectCodec != NULL);
	ASSERT_TRUE(expectCodec->init());
	
	const int c_blockSize = 4096;
	int noChannels = codec->noChannels();

	int lpCoeffSizeA;
	tfloat64 *lpCoeffA = engine::getFIRFilterFromDB(engine::e_lowPassHalf_4097, lpCoeffSizeA);
	ASSERT_TRUE(lpCoeffA != NULL);
	ASSERT_EQ(lpCoeffSizeA, 4097);
	int lpCoeffSizeB;
	tfloat64 *lpCoeffB = engine::getFIRFilterFromDB(engine::e_lowPassQuarter_8193, lpCoeffSizeB);
	ASSERT_TRUE(lpCoeffB != NULL);
	ASSERT_EQ(lpCoeffSizeB, 8193);

	engine::FIRConvolutionAddOverlap *filterA[2], *filterB[2];
	for(int fIdx = 0; fIdx < noChannels; fIdx++)
	{
		filterA[fIdx] = new engine::FIRConvolutionAddOverlap();
		ASSERT_TRUE(filterA[fIdx]->init(lpCoeffA, c_blockSize + 1, c_blockSize));
		filterB[fIdx] = new engine::FIRConvolutionAddOverlap();
		ASSERT_TRUE(filterB[fIdx]->init(lpCoeffB, (c_blockSize * 2) + 1, (c_blockSize * 2)));
	}

	tfloat64 *inL = new tfloat64 [c_blockSize];
	tfloat64 *inR = new tfloat64 [c_blockSize];

	tfloat64 *sA_L = new tfloat64 [c_blockSize];
	tfloat64 *sA_R = new tfloat64 [c_blockSize];

	tfloat64 *inB_L = new tfloat64 [c_blockSize * 2];
	tfloat64 *inB_R = new tfloat64 [c_blockSize * 2];

	tfloat64 *outL = new tfloat64 [c_blockSize * 2];
	tfloat64 *outR = new tfloat64 [c_blockSize * 2];

	int totalDataSize = 0;
	engine::RData data(c_blockSize / 2, codec->noChannels(), codec->noChannels());
	engine::RData expectData(c_blockSize * 2, codec->noChannels(), codec->noChannels());

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
				inL[(idx << 1) + 0] = x[(idx * noChannels) + 0];
				inL[(idx << 1) + 1] = 0.0;
				inR[(idx << 1) + 0] = x[(idx * noChannels) + 1];
				inR[(idx << 1) + 1] = 0.0;
			}
			idx <<= 1;
			while(idx < c_blockSize)
			{
				inL[idx] = 0.0;
				inR[idx] = 0.0;
				idx++;
			}

			filterA[0]->process(inL, sA_L);
			filterA[1]->process(inR, sA_R);
			for(idx = 0; idx < c_blockSize; idx++)
			{
				inB_L[(idx << 1) + 0] = sA_L[idx] * 2.0;
				inB_L[(idx << 1) + 1] = 0.0;
				inB_R[(idx << 1) + 0] = sA_R[idx] * 2.0;
				inB_R[(idx << 1) + 1] = 0.0;
			}
			filterB[0]->process(inB_L, outL);
			filterB[1]->process(inB_R, outR);

            if(loop)
            {
                const tfloat64 *expect = expectData.partData(0);
				for(idx = 0; idx < c_blockSize * 2; idx++)
				{
					// apply *2 gain
					outL[idx] *= 2.0;
					outR[idx] *= 2.0;
                    EXPECT_NEAR(expect[(idx << 1) + 0], outL[idx], c_TOLERANCE);
                    EXPECT_NEAR(expect[(idx << 1) + 1], outR[idx], c_TOLERANCE);
				}
			}
		}
		data.reset();
        expectData.reset();
	} while(loop);

	for(int i = 0; i < noChannels; i++)
	{
		delete filterA[i];
		delete filterB[i];
	}
	delete [] lpCoeffA;
	delete [] lpCoeffB;
	delete [] outL;
	delete [] outR;
	delete [] inB_L;
	delete [] inB_R;
	delete [] sA_L;
	delete [] sA_R;
	delete [] inL;
	delete [] inR;

    codec->close();
    delete codec;
    expectCodec->close();
    delete expectCodec;
}

//-------------------------------------------------------------------------------------------

TEST(FIRConvolutionAddOverlap, halfLowPassFFTDoubleFrequencyOctaveUpscale)
{
    const tfloat64 c_TOLERANCE = 0.00000001;

	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString sourceFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_org.wav");
    QString expectFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_upx2.wav");

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

	engine::FIRConvolutionAddOverlapOctaveUpscale *filter[2];
	for(int fIdx = 0; fIdx < noChannels; fIdx++)
	{
		filter[fIdx] = new engine::FIRConvolutionAddOverlapOctaveUpscale();
		ASSERT_TRUE(filter[fIdx]->init(lpCoeff, c_blockSize + 1, c_blockSize));
	}

	tfloat64 *inL = new tfloat64 [c_blockSize / 2];
	tfloat64 *inR = new tfloat64 [c_blockSize / 2];
	tfloat64 *outL = new tfloat64 [c_blockSize];
	tfloat64 *outR = new tfloat64 [c_blockSize];

	engine::RData data(c_blockSize / 2, codec->noChannels(), codec->noChannels());
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
			while(idx < c_blockSize / 2)
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
					EXPECT_NEAR(expect[(i << 1) + 0], outL[i], c_TOLERANCE);
                    EXPECT_NEAR(expect[(i << 1) + 1], outR[i], c_TOLERANCE);
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
