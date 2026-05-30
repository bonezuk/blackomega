//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_PCMTODSD_H
#define __OMEGA_ENGINE_PCMTODSD_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/FFTRadix2_R2C.h"
#include "engine/inc/FFTRadix2_C2R.h"
#include "engine/inc/FIRFiltersDB.h"
#include "engine/inc/DeltaSigmaModulator.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT PCMToDSD
{
    public:
        PCMToDSD();
        virtual ~PCMToDSD();

        bool init(int inputFrequency, int dsdTimes, bool isLSB);

        void process(const double *in, double *out);

        bool isLSB() const;
        int noInputSamples() const;
        int noOutputSamples() const;
        int noOutputBytes() const;

        int inputFrequency() const;
        int outputFrequency() const;

    private:
        FIRConvolutionAddOverlapOctaveUpscale **m_filters;

        void done();
        int noSteps(int N) const;
        int baseFrequency(int freq) const;
        FIRFilterType filterForFrequency(int freq) const;
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "engine/inc/PCMToDSD.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

PCMToDSD::PCMToDSD() : m_filters(NULL)
{}

//-------------------------------------------------------------------------------------------

PCMToDSD::~PCMToDSD()
{
    done();
}

//-------------------------------------------------------------------------------------------

typedef struct
{
    FIRFilterType type;
    int blockSize;
    int coeffSize;
    int times;
} DSDFilterInfo;

bool PCMToDSD::init(int inputFrequency, int dsdTimes, bool isLSB)
{
    const DSDFilterInfo types[12] = {
        { e_lpHalf_DSD0_5, 1024, 1025, 0 }, // 0
        { e_lpHalf_DSD1, 2048, 2049, 1 },   // 1
        { e_lpQuarter_DSD2, 4096, 4097, 2 }, // 2
        { e_lpQuarter_DSD4, 8192, 8193, 4 }, // 3
        { e_lpQuarter_DSD8, 16384, 16385, 8 }, // 4
        { e_lpQuarter_DSD16, 32768, 32769, 16 }, // 5
        { e_lpQuarter_DSD32, 65536, 65537, 32 }, // 6
        { e_lpQuarter_DSD64, 131072, 131073, 64 }, // 7
        { e_lpQuarter_DSD128, 262144, 262145, 128 }, // 8
        { e_lpQuarter_DSD256, 524288, 524289, 256 }, // 9
        { e_lpQuarter_DSD512, 1048576, 1048577, 512 }, // 10
        { e_lpQuarter_DSD1024, 2097152, 2097153, 1024 } // 11
    };

    int steps = noSteps(dsdTimes);

    if(dsdTimes != (1 << steps))
        return false;

    return true;
}

//-------------------------------------------------------------------------------------------

void PCMToDSD::done()
{

}

//-------------------------------------------------------------------------------------------

int PCMToDSD::noSteps(int N) const
{
	int count = 0;

	while(N > 1)
	{
		N >>= 1;
		count++;
	}
	return count;
}

//-------------------------------------------------------------------------------------------

int PCMToDSD::baseFrequency(int freq) const
{
    int bFreq = 0;

    if(freq < 44100)
    {
        if(!(44100 % freq))
        {
            bFreq = 44100;
        }
        else if(!(48000 % freq))
        {
            bFreq = 48000;
        }
    }
    else
    {
        if(!(freq % 44100))
        {
            bFreq = 44100;
        }
        else if(!(freq % 48000))
        {
            bFreq = 48000;
        }
    }
    return bFreq;
}

//-------------------------------------------------------------------------------------------

FIRFilterType PCMToDSD::filterForFrequency(int freq) const
{
    FIRFilterType type = e_NoFilter;

    if(freq == 11025 || freq == 12000)
    {
        type = e_lpHalf_DSD0_5;
    }
    else if(freq == 22050 || freq == 24000)
    {
        type = e_lpHalf_DSD1;
    }
    else if(freq == 44100 || freq == 48000)
    {
        type = e_lpHalf_DSD2;
    }
    else if(freq == 88200 || freq == 96000)
    {
        type = e_lpHalf_DSD4;
    }
    else if(freq == 176400 || freq == 192000)
    {
        type = e_lpHalf_DSD8;
    }
    else if(freq == 352800 || freq == 384000)
    {
        type = e_lpQuarter_DSD16;
    }
    else if(freq == 705600 || freq == 768000)
    {
        type = e_lpQuarter_DSD32;
    }
    return type;
}

//-------------------------------------------------------------------------------------------

void PCMToDSD::process(const double *in, double *out)
{}

//-------------------------------------------------------------------------------------------

bool PCMToDSD::isLSB() const
{}

//-------------------------------------------------------------------------------------------

int PCMToDSD::noInputSamples() const
{}

//-------------------------------------------------------------------------------------------

int PCMToDSD::noOutputSamples() const
{}

//-------------------------------------------------------------------------------------------

int PCMToDSD::noOutputBytes() const
{}

//-------------------------------------------------------------------------------------------

int PCMToDSD::inputFrequency() const
{}

//-------------------------------------------------------------------------------------------

int PCMToDSD::outputFrequency() const
{}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"

#include "engine/inc/PCMToDSD.h"

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

	tfloat64 *inL = new tfloat64 [c_blockSize];
	tfloat64 *inR = new tfloat64 [c_blockSize];
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
    ASSERT_TRUE(dSigmaL.init(false));
    engine::DeltaSigmaModulator dSigmaR;
    ASSERT_TRUE(dSigmaR.init(false));

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

            filterL[0]->process(inL, pcmL[0]);
            filterL[1]->process(pcmL[0], pcmL[1]);
            filterL[2]->process(pcmL[1], pcmL[2]);
            filterL[3]->process(pcmL[2], pcmL[3]);
            dSigmaL.process(pcmL[3], expectL, outNoSamples);

            filterR[0]->process(inR, pcmR[0]);
            filterR[1]->process(pcmR[0], pcmR[1]);
            filterR[2]->process(pcmR[1], pcmR[2]);
            filterR[3]->process(pcmR[2], pcmR[3]);
            dSigmaL.process(pcmR[3], expectR, outNoSamples);

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
