#include "gtest/gtest.h"

#include "engine/inc/PCMToDSD.h"
#include "engine/inc/Codec.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

void testDSD16FromWAVCodec(engine::PCMToDSD::ComputeMethod computeType)
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

    engine::PCMToDSD convertL(computeType);
    ASSERT_TRUE(convertL.init(codec->frequency(), 16, false));
    ASSERT_EQ(convertL.noInputSamples(), c_inputBlockSize);
    ASSERT_EQ(convertL.noOutputSamples(), outNoSamples);
    ASSERT_EQ(convertL.noOutputBytes(), outNoBytes);
    ASSERT_EQ(convertL.inputFrequency(), 44100);
    ASSERT_EQ(convertL.outputFrequency(), 44100 * 16);
    ASSERT_FALSE(convertL.isLSB());
    engine::PCMToDSD convertR(computeType);
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

            ASSERT_TRUE(convertL.process(inL, outL));
            ASSERT_TRUE(convertR.process(inR, outR));

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

TEST(PCMToDSD, DSD16_CPU)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodec(engine::PCMToDSD::e_computeMethodCPU);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA)
{
#if defined(OMEGA_CUDA)
    if(initCUDAOmega() >= 0)
    {
        ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
        testDSD16FromWAVCodec(engine::PCMToDSD::e_computeMethodCUDA);
    }
    else
    {
        ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
    }
#else
    ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
#endif
}

//-------------------------------------------------------------------------------------------

void testDSD16FromWAVCodecMTPushPull(engine::PCMToDSD::ComputeMethod computeType)
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

    engine::PCMToDSD convertL(computeType);
    ASSERT_TRUE(convertL.initInterleaved(codec->frequency(), 16, engine::e_SampleDSD8MSB, 0, codec->noChannels()));
    ASSERT_EQ(convertL.noInputSamples(), c_inputBlockSize);
    ASSERT_EQ(convertL.noOutputSamples(), outNoSamples);
    ASSERT_EQ(convertL.noOutputBytes(), outNoBytes);
    ASSERT_EQ(convertL.inputFrequency(), 44100);
    ASSERT_EQ(convertL.outputFrequency(), 44100 * 16);
    ASSERT_FALSE(convertL.isLSB());
    engine::PCMToDSD convertR(computeType);
    ASSERT_TRUE(convertR.initInterleaved(codec->frequency(), 16, engine::e_SampleDSD8MSB, 1, codec->noChannels()));
    ASSERT_EQ(convertR.noInputSamples(), c_inputBlockSize);
    ASSERT_EQ(convertR.noOutputSamples(), outNoSamples);
    ASSERT_EQ(convertR.noOutputBytes(), outNoBytes);
    ASSERT_EQ(convertL.inputFrequency(), 44100);
    ASSERT_EQ(convertL.outputFrequency(), 44100 * 16);
    ASSERT_FALSE(convertR.isLSB());

    uint8_t *outL = new uint8_t [outNoBytes];
    uint8_t *outR = new uint8_t [outNoBytes];
    sample_t *inBuffer = new sample_t [c_inputBlockSize * codec->noChannels()];

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

            memcpy(inBuffer, x, data.part(0).length() * codec->noChannels() * sizeof(sample_t));
            if(data.part(0).length() < c_inputBlockSize)
            {
                int amount = c_inputBlockSize - data.part(0).length();
                memset(&inBuffer[data.part(0).length() * codec->noChannels()], 0, amount * codec->noChannels() * sizeof(sample_t));
            }

            ASSERT_EQ(convertL.available(), 0);
            ASSERT_EQ(convertL.push(inBuffer, c_inputBlockSize), c_inputBlockSize);
            ASSERT_EQ(convertL.available(), outNoBytes);
            ASSERT_EQ(convertL.pull(outL, outNoBytes), outNoBytes);

            ASSERT_EQ(convertR.available(), 0);
            ASSERT_EQ(convertR.push(inBuffer, c_inputBlockSize), c_inputBlockSize);
            ASSERT_EQ(convertR.available(), outNoBytes);
            ASSERT_EQ(convertR.pull(outR, outNoBytes), outNoBytes);

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
    delete [] inBuffer;
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

TEST(PCMToDSD, DSD16_CPU_PushPull)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodecMTPushPull(engine::PCMToDSD::e_computeMethodCPU);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA_PushPull)
{
#if defined(OMEGA_CUDA)
    if(initCUDAOmega() >= 0)
    {
        ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
        testDSD16FromWAVCodecMTPushPull(engine::PCMToDSD::e_computeMethodCUDA);
    }
    else
    {
        ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
    }
#else
    ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
#endif
}

//-------------------------------------------------------------------------------------------

void testInterleaveOfDSDForDataType(engine::CodecDataType type, const uint8_t *inL, const uint8_t *inR, sample_t *out, int noDSDByteSamples)
{
    int idx;

    if(type == engine::e_SampleDSD8LSB || type == engine::e_SampleDSD8MSB)
    {
        uint8_t *x = reinterpret_cast<uint8_t *>(out);
        for(idx = 0; idx < noDSDByteSamples; idx += sizeof(sample_t))
        {
            for(int j = 0; j < sizeof(sample_t); j++)
            {
                *x++ = inL[idx + j];
            }
            for(int j = 0; j < sizeof(sample_t); j++)
            {
                *x++ = inR[idx + j];
            }
        }
    }
    else
    {
        uint32_t *x = reinterpret_cast<uint32_t *>(out);
        for(idx = 0; idx < noDSDByteSamples; idx += 2)
        {
            tuint32 s = (idx & 0x02) ? 0xfffa0000 : 0x00050000;
            s |= ((static_cast<tuint32>(inL[idx]) << 8) & 0x0000ff00) | (static_cast<tuint32>(inL[idx+1]) & 0x000000ff);
            if(type == engine::e_SampleInt32)
            {
                s <<= 8;
            }
            x[0] = s;
            s = (idx & 0x02) ? 0xfffa0000 : 0x00050000;
            s |= ((static_cast<tuint32>(inR[idx]) << 8) & 0x0000ff00) | (static_cast<tuint32>(inR[idx+1]) & 0x000000ff);
            if(type == engine::e_SampleInt32)
            {
                s <<= 8;
            }
            x[1] = s;
            x += 2;
        }
    }
}

//-------------------------------------------------------------------------------------------

void testInterleavedDSDSampleOfType(engine::CodecDataType type, const sample_t *expect, const sample_t *out, int noPCMSamples, int noChannels)
{
    if(type == engine::e_SampleDSD8LSB || type == engine::e_SampleDSD8MSB)
    {
        const uint8_t *e = reinterpret_cast<const uint8_t *>(expect);
        const uint8_t *o = reinterpret_cast<const uint8_t *>(out);
        int noDSDBytes = noPCMSamples * 8;
        for(int idx = 0; idx < noDSDBytes; idx++)
        {
            for(int chIdx = 0; chIdx < noChannels; chIdx++)
            {
                EXPECT_EQ(o[chIdx], e[chIdx]);
            }
            e += noChannels;
            o += noChannels;
        }
    }
    else
    {
        const uint32_t *e = reinterpret_cast<const uint32_t *>(expect);
        const uint32_t *o = reinterpret_cast<const uint32_t *>(out);
        int noDSDBytes = noPCMSamples * 4;
        for(int idx = 0; idx < noDSDBytes; idx += 2)
        {
            for(int chIdx = 0; chIdx < noChannels; chIdx++)
            {
                EXPECT_EQ(o[chIdx], e[chIdx]);
            }
            e += noChannels;
            o += noChannels;
        }
    }
}

//-------------------------------------------------------------------------------------------

void testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::ComputeMethod computeType, engine::CodecDataType dataType)
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

    int dsdBytesPerPCMSample;
    if(dataType == engine::e_SampleDSD8LSB || dataType == engine::e_SampleDSD8MSB)
    {
        dsdBytesPerPCMSample = 8;
    }
    else
    {
        dsdBytesPerPCMSample = 2;
    }
    int noPCMSamplesPerChannel = outNoBytes / dsdBytesPerPCMSample;
    sample_t *expect = new sample_t [2 * noPCMSamplesPerChannel];
    uint8_t *expectL = new uint8_t [outNoBytes];
    uint8_t *expectR = new uint8_t [outNoBytes];

    engine::DeltaSigmaModulator dSigmaL;
    dSigmaL.init((dataType == engine::e_SampleDSD8LSB) ? true : false);
    engine::DeltaSigmaModulator dSigmaR;
    dSigmaR.init((dataType == engine::e_SampleDSD8LSB) ? true : false);

    engine::PCMToDSD convertL(computeType);
    ASSERT_TRUE(convertL.initInterleaved(codec->frequency(), 16, dataType, 0, codec->noChannels()));
    ASSERT_EQ(convertL.noInputSamples(), c_inputBlockSize);
    ASSERT_EQ(convertL.noOutputSamples(), outNoSamples);
    ASSERT_EQ(convertL.noOutputBytes(), outNoBytes);
    ASSERT_EQ(convertL.inputFrequency(), 44100);
    ASSERT_EQ(convertL.outputFrequency(), 44100 * 16);
    
    engine::PCMToDSD convertR(computeType);
    ASSERT_TRUE(convertR.initInterleaved(codec->frequency(), 16, dataType, 1, codec->noChannels()));
    ASSERT_EQ(convertR.noInputSamples(), c_inputBlockSize);
    ASSERT_EQ(convertR.noOutputSamples(), outNoSamples);
    ASSERT_EQ(convertR.noOutputBytes(), outNoBytes);
    ASSERT_EQ(convertL.inputFrequency(), 44100);
    ASSERT_EQ(convertL.outputFrequency(), 44100 * 16);

    ASSERT_EQ(convertL.dataType(), dataType);
    ASSERT_EQ(convertR.dataType(), dataType);
    if(dataType == engine::e_SampleDSD8LSB)
    {
        ASSERT_TRUE(convertL.isLSB());
        ASSERT_TRUE(convertR.isLSB());
    }
    else
    {
        ASSERT_FALSE(convertL.isLSB());
        ASSERT_FALSE(convertR.isLSB());
    }

    sample_t *inBuffer = new sample_t [c_inputBlockSize * codec->noChannels()];
    sample_t *out = new sample_t [2 * noPCMSamplesPerChannel];

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

            testInterleaveOfDSDForDataType(dataType, expectL, expectR, expect, outNoBytes);

            memcpy(inBuffer, x, data.part(0).length() * codec->noChannels() * sizeof(sample_t));
            if(data.part(0).length() < c_inputBlockSize)
            {
                int amount = c_inputBlockSize - data.part(0).length();
                memset(&inBuffer[data.part(0).length() * codec->noChannels()], 0, amount * codec->noChannels() * sizeof(sample_t));
            }

            ASSERT_EQ(convertL.available(), 0);
            ASSERT_EQ(convertL.push(inBuffer, c_inputBlockSize), c_inputBlockSize);
            ASSERT_EQ(convertL.available(), outNoBytes);
            ASSERT_EQ(convertL.availablePCMSamples(), noPCMSamplesPerChannel);
            ASSERT_EQ(convertL.pullInterleaved(out, noPCMSamplesPerChannel), noPCMSamplesPerChannel);

            ASSERT_EQ(convertR.available(), 0);
            ASSERT_EQ(convertR.push(inBuffer, c_inputBlockSize), c_inputBlockSize);
            ASSERT_EQ(convertR.available(), outNoBytes);
            ASSERT_EQ(convertR.availablePCMSamples(), noPCMSamplesPerChannel);
            ASSERT_EQ(convertR.pullInterleaved(out, noPCMSamplesPerChannel), noPCMSamplesPerChannel);

            testInterleavedDSDSampleOfType(dataType, expect, out, noPCMSamplesPerChannel, 2);
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
    delete [] inBuffer;
    delete [] expectL;
    delete [] expectR;
    delete [] expect;
    delete [] out;
	delete [] inL;
	delete [] inR;

    codec->close();
    delete codec;
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CPU_PushPullInterleaved_NativeLSB)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCPU, engine::e_SampleDSD8LSB);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA_PushPullInterleaved_NativeLSB)
{
#if defined(OMEGA_CUDA)
    if(initCUDAOmega() >= 0)
    {
        ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
        testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCUDA, engine::e_SampleDSD8LSB);
    }
    else
    {
        ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
    }
#else
    ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
#endif
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CPU_PushPullInterleaved_NativeMSB)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCPU, engine::e_SampleDSD8MSB);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA_PushPullInterleaved_NativeMSB)
{
#if defined(OMEGA_CUDA)
    if(initCUDAOmega() >= 0)
    {
        ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
        testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCUDA, engine::e_SampleDSD8MSB);
    }
    else
    {
        ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
    }
#else
    ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
#endif
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CPU_PushPullInterleaved_DSDOverPCM_24)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCPU, engine::e_SampleInt24);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA_PushPullInterleaved_DSDOverPCM_24)
{
#if defined(OMEGA_CUDA)
    if(initCUDAOmega() >= 0)
    {
        ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
        testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCUDA, engine::e_SampleInt24);
    }
    else
    {
        ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
    }
#else
    ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
#endif
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CPU_PushPullInterleaved_DSDOverPCM_32)
{
    ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCPU));
    testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCPU, engine::e_SampleInt32);
}

//-------------------------------------------------------------------------------------------

TEST(PCMToDSD, DSD16_CUDA_PushPullInterleaved_DSDOverPCM_32)
{
#if defined(OMEGA_CUDA)
    if(initCUDAOmega() >= 0)
    {
        ASSERT_TRUE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
        testDSD16FromWAVCodecMTPushPullInterleaved(engine::PCMToDSD::e_computeMethodCUDA, engine::e_SampleInt32);
    }
    else
    {
        ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
    }
#else
    ASSERT_FALSE(engine::PCMToDSD::isComputeMethodAvailable(engine::PCMToDSD::e_computeMethodCUDA));
#endif
}

//-------------------------------------------------------------------------------------------