#include "gtest/gtest.h"

#include "engine/inc/PCMToDSD.h"
#include "engine/inc/Codec.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16)
{
    const tfloat64 c_TOLERANCE = 0.00000001;

	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString sourceFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_org.wav");

    ASSERT_TRUE(common::DiskOps::exist(sourceFileName));
	
	engine::Codec *codec = engine::Codec::get(sourceFileName);
    ASSERT_TRUE(codec != NULL);
	ASSERT_TRUE(codec->init());
    ASSERT_EQ(codec->frequency(), 44100);
	
    const int c_inputBlockSize = 2048;

    engine::FIRFilterType filterTypes[4] = { engine::e_lpHalf_DSD2, engine::e_lpQuarter_DSD4, engine::e_lpQuarter_DSD8, engine::e_lpQuarter_DSD16 };
    engine::FIRConvolutionAddOverlapOctaveUpscale *filterL[4];
    engine::FIRConvolutionAddOverlapOctaveUpscale *filterR[4];
    for(int idx = 0; idx < 4; idx++)
    {
        int lpSize;
        tfloat64 *lpCoeff = engine::getFIRFilterFromDB(filterTypes[idx], lpSize);
        int expectLPSize = (c_inputBlockSize << (idx + 1)) + 1;
        ASSERT_EQ(lpSize, expectLPSize);
        int blockLen = c_inputBlockSize << (idx + 1);
        filterL[idx] = new engine::FIRConvolutionAddOverlapOctaveUpscale();
        ASSERT_TRUE(filterL[idx]->init(lpCoeff, lpSize, blockLen));
        filterR[idx] = new engine::FIRConvolutionAddOverlapOctaveUpscale();
        ASSERT_TRUE(filterR[idx]->init(lpCoeff, lpSize, blockLen));
        delete [] lpCoeff;
    }

	tfloat64 *inL = new tfloat64 [c_inputBlockSize];
	tfloat64 *inR = new tfloat64 [c_inputBlockSize];
    tfloat64 *pcmL[4];
    tfloat64 *pcmR[4];
    for(int idx = 0; idx < 4; idx++)
    {
        int blockLen = c_inputBlockSize << (idx + 1);
        pcmL[idx] = new tfloat64 [blockLen];
        pcmR[idx] = new tfloat64 [blockLen];
    }

    int outNoSamples = c_inputBlockSize << 4;
    int outNoBytes = outNoSamples >> 3;
	engine::RData data(c_inputBlockSize, codec->noChannels(), codec->noChannels());
    uint8_t *expectL = new uint8_t [outNoBytes];
    uint8_t *expectR = new uint8_t [outNoBytes];

    engine::DeltaSigmaModulator dSigmaL;
    dSigmaL.init(false);
    engine::DeltaSigmaModulator dSigmaR;
    dSigmaR.init(false);

    engine::PCMToDSD convertL;
    ASSERT_TRUE(convertL.init(codec->frequency(), 16, false));
    ASSERT_EQ(convertL.noInputSamples(), c_inputBlockSize);
    ASSERT_EQ(convertL.noOutputSamples(), outNoSamples);
    ASSERT_EQ(convertL.noOutputBytes(), outNoBytes);
    ASSERT_EQ(convertL.inputFrequency(), 44100);
    ASSERT_EQ(convertL.outputFrequency(), 44100 * 16);
    ASSERT_FALSE(convertL.isLSB());
    engine::PCMToDSD convertR;
    ASSERT_TRUE(convertR.init(codec->frequency(), 16, false));
    ASSERT_EQ(convertR.noInputSamples(), c_inputBlockSize);
    ASSERT_EQ(convertR.noOutputSamples(), outNoSamples);
    ASSERT_EQ(convertR.noOutputBytes(), outNoBytes);
    ASSERT_EQ(convertL.inputFrequency(), 44100);
    ASSERT_EQ(convertL.outputFrequency(), 44100 * 16);
    ASSERT_FALSE(convertR.isLSB());

    uint8_t *outL = new uint8_t [outNoBytes];
    uint8_t *outR = new uint8_t [outNoBytes];

    int noChannels = codec->noChannels();
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
			while(idx < c_inputBlockSize)
			{
				inL[idx] = 0.0;
				inR[idx] = 0.0;
				idx++;
			}

            filterL[0]->process(inL, pcmL[0]);
            filterL[1]->process(pcmL[0], pcmL[1]);
            filterL[2]->process(pcmL[1], pcmL[2]);
            filterL[3]->process(pcmL[2], pcmL[3]);
            dSigmaL.process(pcmL[3], expectL, outNoSamples);

            filterR[0]->process(inR, pcmR[0]);
            filterR[1]->process(pcmR[0], pcmR[1]);
            filterR[2]->process(pcmR[1], pcmR[2]);
            filterR[3]->process(pcmR[2], pcmR[3]);
            dSigmaR.process(pcmR[3], expectR, outNoSamples);

            convertL.process(inL, outL);
            convertR.process(inR, outR);

            for(idx = 0; idx < outNoBytes; idx++)
            {
                EXPECT_EQ(outL[idx], expectL[idx]);
                EXPECT_EQ(outR[idx], expectR[idx]);
            }
		}
		data.reset();
	} while(loop);
	
    for(int idx = 0; idx < 4; idx++)
    {
        delete filterL[idx];
        delete filterR[idx];
        delete pcmL[idx];
        delete pcmR[idx];
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
