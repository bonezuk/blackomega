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
#include "engine/inc/FIRConvolutionAddOverlapCUDA.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

class FIRConvolutionAddOverlapCUDA : public ::testing::Test 
{
protected:
    static void SetUpTestSuite() {
        cuda_init_result_ = initCUDAOmega();
    }

    void SetUp() override {
        if (cuda_init_result_ < 0) {
            GTEST_SKIP();
        }
    }

    static void TearDownTestSuite() {}

private:
    static int cuda_init_result_;
};

int FIRConvolutionAddOverlapCUDA::cuda_init_result_ = -1;

//-------------------------------------------------------------------------------------------

TEST_F(FIRConvolutionAddOverlapCUDA, halfLowPassFFTSameFrequency)
{
    const tfloat64 c_TOLERANCE = 0.00000001;

	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString sourceFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_org.wav");

	ASSERT_TRUE(common::DiskOps::exist(sourceFileName));
	
	engine::Codec *codec = engine::Codec::get(sourceFileName);
    ASSERT_TRUE(codec != NULL);
	ASSERT_TRUE(codec->init());
			
	const int c_blockSize = 4096;
	int noChannels = codec->noChannels();
	
	int lpCoeffSize;
	tfloat64 *lpCoeff = engine::getFIRFilterFromDB(engine::e_lowPassHalf_4097, lpCoeffSize);
	ASSERT_TRUE(lpCoeff != NULL);
	ASSERT_EQ(lpCoeffSize, 4097);

	engine::FIRConvolutionAddOverlap *filterCPU[2];
    FIRConvAddOverlapCuda_Data *filterGPU[2];

	for(int fIdx = 0; fIdx < noChannels; fIdx++)
	{
		filterCPU[fIdx] = new engine::FIRConvolutionAddOverlap();
		ASSERT_TRUE(filterCPU[fIdx]->init(lpCoeff, c_blockSize + 1, c_blockSize));
        filterGPU[fIdx] = FIRConvAddOverlapCUDA_Init(lpCoeff, c_blockSize + 1, c_blockSize);
        ASSERT_TRUE(filterGPU[fIdx] != NULL);
	}

	tfloat64 *inL = new tfloat64 [c_blockSize];
	tfloat64 *inR = new tfloat64 [c_blockSize];
	tfloat64 *outL = new tfloat64 [c_blockSize];
	tfloat64 *outR = new tfloat64 [c_blockSize];
    tfloat64 *expectL = new tfloat64 [c_blockSize];
	tfloat64 *expectR = new tfloat64 [c_blockSize];

	engine::RData data(c_blockSize, codec->noChannels(), codec->noChannels());
	bool loop = true;
	do
	{
		loop = codec->next(data);
		if(data.noParts() > 0)
		{
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

			filterCPU[0]->process(inL, expectL);
			filterCPU[1]->process(inR, expectR);

            ASSERT_TRUE(FIRConvAddOverlapCUDA_Process(inL, outL, filterGPU[0]));
            ASSERT_TRUE(FIRConvAddOverlapCUDA_Process(inR, outR, filterGPU[1]));

            if(loop)
            {
                for(int i = 0; i < c_blockSize; i++)
                {
                    EXPECT_NEAR(expectL[i], outL[i], c_TOLERANCE);
                    EXPECT_NEAR(expectR[i], outR[i], c_TOLERANCE);
                }
            }
		}
		data.reset();
	} while(loop);
	
	for(int i = 0; i < noChannels; i++)
	{
		delete filterCPU[i];
        FIRConvAddOverlapCUDA_Free(filterGPU[i]);
	}
    delete [] expectL;
    delete [] expectR;
	delete [] outL;
	delete [] outR;
	delete [] inL;
	delete [] inR;

    codec->close();
    delete codec;
}

//-------------------------------------------------------------------------------------------
