#include "gtest/gtest.h"

#include "common/inc/Random.h"
#include "engine/inc/Codec.h"
#include "engine/inc/PCMToDSDProcessor.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(PCM2DSDProcessor, runProcessorDSD64)
{
    const double c_tolerance = 0.0000001;
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString sourceFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_org.wav");

    ASSERT_TRUE(common::DiskOps::exist(sourceFileName));
	
	engine::Codec *codec = engine::Codec::get(sourceFileName);
    ASSERT_TRUE(codec != NULL);
	ASSERT_TRUE(codec->init());
    ASSERT_EQ(codec->frequency(), 44100);

    // DSD1 -> 2048, 4096 (DSD2), 8192 (DSD4), 16384 (DSD8), 32768 (DSD16), 65536 (DSD32), 131072 (DSD64)
    // 131072 / 8 = 16384 DSD bytes
    // 16384 / 8 = 2048 PCM output samples.
    const int c_inputBlockSize = 2048;
    const int c_outputBlockSize = 2048;
    engine::PCMToDSDProcessor processor;
    ASSERT_TRUE(processor.init(64, codec->frequency(), codec->noChannels(), engine::e_SampleDSD8LSB));

    engine::RData inputData(c_inputBlockSize, codec->noChannels(), codec->noChannels());
    engine::RData outputData(c_outputBlockSize, codec->noChannels(), codec->noChannels());

    common::TimeStamp endTs;
    const double c_outputPCMFrequency = 44100.0;
    int noOutputPCMSamples = 0;
    bool loop = true, isMore = true;
    while(loop)
    {
        while(outputData.rLength() > 0 && isMore)
        {
            if(processor.available() < outputData.rLength())
            {
                isMore = codec->next(inputData);
                processor.push(inputData);
                if(!isMore)
                {
                    ASSERT_FALSE(processor.isFinalised());
                    processor.finalise();
                    ASSERT_TRUE(processor.isFinalised());
                    endTs = inputData.part(0).end();
                }
            }
            else
            {
                processor.pull(outputData);
                EXPECT_EQ(outputData.noParts(), 1);
                engine::RData::Part& p = outputData.part(0);
                EXPECT_EQ(p.length(), c_outputBlockSize);
                double tS = static_cast<double>(noOutputPCMSamples) / c_outputPCMFrequency;
                double tE = static_cast<double>(noOutputPCMSamples + c_outputBlockSize) / c_outputPCMFrequency;
                EXPECT_NEAR(static_cast<double>(p.start()), tS, c_tolerance);
                EXPECT_NEAR(static_cast<double>(p.end()), tE, c_tolerance);
                EXPECT_NEAR(static_cast<double>(outputData.start()), tS, c_tolerance);
                EXPECT_NEAR(static_cast<double>(outputData.end()), tE, c_tolerance);
                noOutputPCMSamples += c_outputBlockSize;
            }
        }
        if(outputData.rLength() > 0)
        {
            if(processor.available() > 0)
            {
                processor.pull(outputData);
                EXPECT_EQ(outputData.noParts(), 1);
                engine::RData::Part& p = outputData.part(0);
                double tS = static_cast<double>(noOutputPCMSamples) / c_outputPCMFrequency;
                double tE = static_cast<double>(noOutputPCMSamples + p.length()) / c_outputPCMFrequency;
                EXPECT_NEAR(static_cast<double>(p.start()), tS, c_tolerance);
                EXPECT_NEAR(static_cast<double>(p.end()), tE, c_tolerance);
                EXPECT_NEAR(static_cast<double>(outputData.start()), tS, c_tolerance);
                EXPECT_NEAR(static_cast<double>(outputData.end()), tE, c_tolerance);
                EXPECT_NEAR(static_cast<double>(p.end()), static_cast<double>(endTs), c_tolerance);
                EXPECT_NEAR(static_cast<double>(outputData.end()), static_cast<double>(endTs), c_tolerance);
                noOutputPCMSamples += p.length();
            }
            else
            {
                loop = false;
            }
        }
        outputData.reset();
    }

    delete codec;
}

//-------------------------------------------------------------------------------------------

void testPopulateInputPCM2DSDProcessorPartStream(bool isGaps, engine::RData& inA, engine::RData& inB, engine::RData& inC, engine::RData& inD)
{
    const double c_infrequency = 44100.0;
	common::Random *rand = common::Random::instance();
	rand->seed(0);


    sample_t *xA = inA.data();
    sample_t *xB = inB.data();
    sample_t *xC = inC.data();
    sample_t *xD = inD.data();
    for(int idx = 0; idx < 3072 * 2; idx++)
    {
        xA[idx] = rand->randomReal1();
        xB[idx] = rand->randomReal1();
        xC[idx] = rand->randomReal1();
        xD[idx] = rand->randomReal1();
    }

    double tS;
    int amount = 0;
    {
        tS = static_cast<double>(amount) / c_infrequency;
        inA.start() = tS;
        engine::RData::Part& inA1 = inA.nextPart();
        inA1.length() = 1536;
        inA1.start() = tS;
        amount += 1536;
        tS = static_cast<double>(amount) / c_infrequency;
        inA1.end() = tS;
        if(isGaps)
        {
            amount += 10;
            tS = static_cast<double>(amount) / c_infrequency;
        }
        engine::RData::Part& inA2 = inA.nextPart();
        inA2.length() = 1536;
        inA2.start() = tS;
        amount += 1536;
        tS = static_cast<double>(amount) / c_infrequency;
        inA2.end() = tS;
        inA.end() = tS;
        ASSERT_EQ(inA.rLength(), 0);
    }
    {
        inB.start() = tS;
        engine::RData::Part& inB1 = inB.nextPart();
        inB1.length() = 512;
        inB1.start() = tS;
        amount += 512;
        tS = static_cast<double>(amount) / c_infrequency;
        inB1.end() = tS;
        if(isGaps)
        {
            amount += 10;
            tS = static_cast<double>(amount) / c_infrequency;
        }
        engine::RData::Part& inB2 = inB.nextPart();
        inB2.length() = 1024;
        inB2.start() = tS;
        amount += 1024;
        tS = static_cast<double>(amount) / c_infrequency;
        inB2.end() = tS;
        if(isGaps)
        {
            amount += 10;
            tS = static_cast<double>(amount) / c_infrequency;
        }
        engine::RData::Part& inB3 = inB.nextPart();
        inB3.length() = 1024;
        inB3.start() = tS;
        amount += 1024;
        tS = static_cast<double>(amount) / c_infrequency;
        inB3.end() = tS;
        if(isGaps)
        {
            amount += 10;
            tS = static_cast<double>(amount) / c_infrequency;
        }
        engine::RData::Part& inB4 = inB.nextPart();
        inB4.length() = 512;
        inB4.start() = tS;
        amount += 512;
        tS = static_cast<double>(amount) / c_infrequency;
        inB4.end() = tS;
        inB.end() = tS;
        ASSERT_EQ(inB.rLength(), 0);
    }
    {
        inC.start() = tS;
        engine::RData::Part& inC1 = inC.nextPart();
        inC1.length() = 2048;
        inC1.start() = tS;
        amount += 2048;
        tS = static_cast<double>(amount) / c_infrequency;
        inC1.end() = tS;
        if(isGaps)
        {
            amount += 10;
            tS = static_cast<double>(amount) / c_infrequency;
        }
        engine::RData::Part& inC2 = inC.nextPart();
        inC2.length() = 512;
        inC2.start() = tS;
        amount += 512;
        tS = static_cast<double>(amount) / c_infrequency;
        inC2.end() = tS;
        if(isGaps)
        {
            amount += 10;
            tS = static_cast<double>(amount) / c_infrequency;
        }
        engine::RData::Part& inC3 = inC.nextPart();
        inC3.length() = 512;
        inC3.start() = tS;
        amount += 512;
        tS = static_cast<double>(amount) / c_infrequency;
        inC3.end() = tS;
        inC.end() = tS;
        ASSERT_EQ(inC.rLength(), 0);
    }
    {
        inD.start() = tS;
        engine::RData::Part& inD1 = inD.nextPart();
        inD1.length() = 1024;
        inD1.start() = tS;
        amount += 1024;
        tS = static_cast<double>(amount) / c_infrequency;
        inD1.end() = tS;
        if(isGaps)
        {
            amount += 10;
            tS = static_cast<double>(amount) / c_infrequency;
        }
        engine::RData::Part& inD2 = inD.nextPart();
        inD2.length() = 1024;
        inD2.start() = tS;
        amount += 1024;
        tS = static_cast<double>(amount) / c_infrequency;
        inD2.end() = tS;
        if(isGaps)
        {
            amount += 10;
            tS = static_cast<double>(amount) / c_infrequency;
        }
        engine::RData::Part& inD3 = inD.nextPart();
        inD3.length() = 1024;
        inD3.start() = tS;
        amount += 1024;
        tS = static_cast<double>(amount) / c_infrequency;
        inD3.end() = tS;
        inD.end() = tS;
        ASSERT_EQ(inD.rLength(), 0);
    }
}

//-------------------------------------------------------------------------------------------

TEST(PCM2DSDProcessor, partHandlingNoGapsDSD256)
{
    const tfloat64 c_TOLERANCE = 0.00000001;
    const double c_infrequency = 44100.0;

    engine::RData inA(3072, 2, 2);
    engine::RData inB(3072, 2, 2);
    engine::RData inC(3072, 2, 2);
    engine::RData inD(3072, 2, 2);
    testPopulateInputPCM2DSDProcessorPartStream(false, inA, inB, inC, inD);

    engine::PCMToDSDProcessor processor;
    ASSERT_TRUE(processor.init(engine::e_SampleDSD8LSB, 44100, 256, 2));
    processor.push(inA);
    processor.push(inB);
    processor.push(inC);
    processor.push(inD);
    processor.finalise();
    ASSERT_EQ(processor.available(), 49152);

    double tS;

    engine::RData outA(8192, 2, 2);
    processor.pull(outA);
    ASSERT_EQ(outA.rLength(), 0);
    ASSERT_EQ(outA.noParts(), 1);
    engine::RData::Part& outA1 = outA.part(0);
    ASSERT_EQ(outA1.length(), 8192);
    tS = 0.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outA.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outA1.start()), tS, c_TOLERANCE);
    tS = 8192.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outA.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outA1.end()), tS, c_TOLERANCE);

    engine::RData outB(8192, 2, 2);
    processor.pull(outB);
    ASSERT_EQ(outB.rLength(), 0);
    ASSERT_EQ(outB.noParts(), 1);
    engine::RData::Part& outB1 = outB.part(0);
    ASSERT_EQ(outB1.length(), 8192);
    tS = 8192.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outB.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outB1.start()), tS, c_TOLERANCE);
    tS = 16384.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outB.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outB1.end()), tS, c_TOLERANCE);
    
    engine::RData outC(8192, 2, 2);
    processor.pull(outC);
    ASSERT_EQ(outC.rLength(), 0);
    ASSERT_EQ(outC.noParts(), 1);
    engine::RData::Part& outC1 = outC.part(0);
    ASSERT_EQ(outC1.length(), 8192);
    tS = 16384.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outC.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outC1.start()), tS, c_TOLERANCE);
    tS = 24576.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outC.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outC1.end()), tS, c_TOLERANCE);

    engine::RData outD(8192, 2, 2);
    processor.pull(outD);
    ASSERT_EQ(outD.rLength(), 0);
    ASSERT_EQ(outD.noParts(), 1);
    engine::RData::Part& outD1 = outD.part(0);
    ASSERT_EQ(outD1.length(), 8192);
    tS = 24576.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outD.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outD1.start()), tS, c_TOLERANCE);
    tS = 32768.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outD.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outD1.end()), tS, c_TOLERANCE);

    engine::RData outE(8192, 2, 2);
    processor.pull(outE);
    ASSERT_EQ(outE.rLength(), 0);
    ASSERT_EQ(outE.noParts(), 1);
    engine::RData::Part& outE1 = outE.part(0);
    ASSERT_EQ(outE1.length(), 8192);
    tS = 32768.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outE.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outE1.start()), tS, c_TOLERANCE);
    tS = 40960.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outE.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outE1.end()), tS, c_TOLERANCE);

    engine::RData outF(8192, 2, 2);
    processor.pull(outF);
    ASSERT_EQ(outF.rLength(), 0);
    ASSERT_EQ(outF.noParts(), 1);
    engine::RData::Part& outF1 = outE.part(0);
    ASSERT_EQ(outF1.length(), 8192);
    tS = 40960.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outF.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outF1.start()), tS, c_TOLERANCE);
    tS = 49152.0 / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outF.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outF1.end()), tS, c_TOLERANCE);
}

//-------------------------------------------------------------------------------------------

TEST(PCM2DSDProcessor, partHandlingGapsDSD256)
{
	const tfloat64 c_TOLERANCE = 0.00000001;
    const double c_infrequency = 44100.0;

    engine::RData inA(3072, 2, 2);
    engine::RData inB(3072, 2, 2);
    engine::RData inC(3072, 2, 2);
    engine::RData inD(3072, 2, 2);
    testPopulateInputPCM2DSDProcessorPartStream(true, inA, inB, inC, inD);
    
    engine::PCMToDSDProcessor processor;
    ASSERT_TRUE(processor.init(engine::e_SampleDSD8LSB, 44100, 256, 2));
    processor.push(inA);
    processor.push(inB);
    processor.push(inC);
    processor.push(inD);
    processor.finalise();
    ASSERT_EQ(processor.available(), 49152);

    const int c_gapLength = 10 * 4;
    int amount = 0;
    double tS;

    engine::RData outA(8192, 2, 2);
    processor.pull(outA);
    ASSERT_EQ(outA.rLength(), 0);
    ASSERT_EQ(outA.noParts(), 2);
    engine::RData::Part& outA1 = outA.part(0);
    ASSERT_EQ(outA1.length(), 6144);
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outA.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outA1.start()), tS, c_TOLERANCE);
    amount += 6144;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outA1.end()), tS, c_TOLERANCE);
    engine::RData::Part& outA2 = outA.part(1);
    ASSERT_EQ(outA2.length(), 2048);
    amount += c_gapLength;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outA2.start()), tS, c_TOLERANCE);
    amount += 2048;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outA2.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outA.end()), tS, c_TOLERANCE);
    
    engine::RData outB(8192, 2, 2);
    processor.pull(outB);
    ASSERT_EQ(outB.rLength(), 0);
    ASSERT_EQ(outB.noParts(), 2);
    engine::RData::Part& outB1 = outB.part(0);
    ASSERT_EQ(outB1.length(), 6144);
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outB.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outB1.start()), tS, c_TOLERANCE);
    amount += 6144;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outB1.end()), tS, c_TOLERANCE);
    engine::RData::Part& outB2 = outB.part(1);
    ASSERT_EQ(outB2.length(), 2048);
    amount += c_gapLength;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outB2.start()), tS, c_TOLERANCE);
    amount += 2048;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outB2.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outB.end()), tS, c_TOLERANCE);

    engine::RData outC(8192, 2, 2);
    processor.pull(outC);
    ASSERT_EQ(outC.rLength(), 0);
    ASSERT_EQ(outC.noParts(), 3);
    engine::RData::Part& outC1 = outC.part(0);
    ASSERT_EQ(outC1.length(), 2048);
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outC.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outC1.start()), tS, c_TOLERANCE);
    amount += 2048;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outC1.end()), tS, c_TOLERANCE);
    engine::RData::Part& outC2 = outC.part(1);
    ASSERT_EQ(outC2.length(), 2048);
    amount += c_gapLength;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outC2.start()), tS, c_TOLERANCE);
    amount += 2048;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outC2.end()), tS, c_TOLERANCE);
    engine::RData::Part& outC3 = outC.part(2);
    ASSERT_EQ(outC3.length(), 2048);
    amount += c_gapLength;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outC3.start()), tS, c_TOLERANCE);
    amount += 2048;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outC3.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outC.end()), tS, c_TOLERANCE);

    engine::RData outD(8192, 2, 2);
    processor.pull(outD);
    ASSERT_EQ(outD.rLength(), 0);
    ASSERT_EQ(outD.noParts(), 1);
    engine::RData::Part& outD1 = outD.part(0);
    ASSERT_EQ(outD1.length(), 8192);
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outD.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outD1.start()), tS, c_TOLERANCE);
    amount += 8192;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outD1.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outD.end()), tS, c_TOLERANCE);

    engine::RData outE(8192, 2, 2);
    processor.pull(outE);
    ASSERT_EQ(outE.rLength(), 0);
    ASSERT_EQ(outE.noParts(), 2);
    engine::RData::Part& outE1 = outE.part(0);
    ASSERT_EQ(outE1.length(), 2048);
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outE.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outE1.start()), tS, c_TOLERANCE);
    amount += 2048;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outE1.end()), tS, c_TOLERANCE);
    engine::RData::Part& outE2 = outE.part(1);
    ASSERT_EQ(outE2.length(), 6144);
    amount += c_gapLength;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outE2.start()), tS, c_TOLERANCE);
    amount += 6144;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outE2.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outE.end()), tS, c_TOLERANCE);

    engine::RData outF(8192, 2, 2);
    processor.pull(outF);
    ASSERT_EQ(outF.rLength(), 0);
    ASSERT_EQ(outF.noParts(), 2);
    engine::RData::Part& outF1 = outF.part(0);
    ASSERT_EQ(outF1.length(), 4096);
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outF.start()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outF1.start()), tS, c_TOLERANCE);
    amount += 4096;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outF1.end()), tS, c_TOLERANCE);
    engine::RData::Part& outF2 = outF.part(1);
    ASSERT_EQ(outF2.length(), 4096);
    amount += c_gapLength;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outF2.start()), tS, c_TOLERANCE);
    amount += 4098;
    tS = static_cast<double>(amount) / (c_infrequency * 4);
    ASSERT_NEAR(static_cast<double>(outF2.end()), tS, c_TOLERANCE);
    ASSERT_NEAR(static_cast<double>(outF.end()), tS, c_TOLERANCE);
}

//-------------------------------------------------------------------------------------------
