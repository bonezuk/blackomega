/*
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_PCM2DSDPROCESSOR_H
#define __OMEGA_ENGINE_PCM2DSDPROCESSOR_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/RData.h"
#include "engine/inc/PCMToDSD.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class PCM2DSDProcessor
{
    public:
        PCM2DSDProcessor();
        virtual ~PCM2DSDProcessor();

        bool init(int dsdTimes, int inputFreq, int noChannels, CodecDataType dataType);

        void push(const RData& data);
        void finalise();
        bool isFinalised() const;

        int available() const;
        void pull(RData& data);
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "engine/inc/PCMToDSDProcessor.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

PCM2DSDProcessor::PCM2DSDProcessor()
{}

//-------------------------------------------------------------------------------------------

PCM2DSDProcessor::~PCM2DSDProcessor()
{}

//-------------------------------------------------------------------------------------------

bool PCM2DSDProcessor::init(int dsdTimes, int inputFreq, int noChannels, CodecDataType dataType)
{}

//-------------------------------------------------------------------------------------------

void PCM2DSDProcessor::push(const RData& data)
{}

//-------------------------------------------------------------------------------------------

void PCM2DSDProcessor::finalise()
{}

//-------------------------------------------------------------------------------------------

bool PCM2DSDProcessor::isFinalised() const
{}

//-------------------------------------------------------------------------------------------

int PCM2DSDProcessor::available() const
{}

//-------------------------------------------------------------------------------------------

void PCM2DSDProcessor::pull(RData& data)
{}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"

#include "engine/inc/PCMToDSDProcessor.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(PCM2DSDProcessor, runProcessorDSD64)
{
	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString sourceFileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "kiss2sec_org.wav");

    ASSERT_TRUE(common::DiskOps::exist(sourceFileName));
	
	engine::Codec *codec = engine::Codec::get(sourceFileName);
    ASSERT_TRUE(codec != NULL);
	ASSERT_TRUE(codec->init());
    ASSERT_EQ(codec->frequency(), 44100);

    engine::PCM2DSDProcessor processor;
    ASSERT_TRUE(processor.init(64, codec->frequency(), codec->noChannels(), engine::e_SampleDSD8LSB));

    engine::RData aData(c_inputBlockSize * 2, codec->noChannels(), codec->noChannels());
    engine::RData cData(c_inputBlockSize, codec->noChannels(), codec->noChannels());

    bool loop = true, isMore = true;
    while(loop)
    {
        while(aData.rLength() > 0 && isMore)
        {
            if(processor.available() < aData->rLength())
            {
                isMore = codec->next(cData);
                processor.push(cData);
                if(!isMore)
                {
                    ASSERT_FALSE(processor.isFinalised());
                    processor.finalise();
                    ASSERT_TRUE(processor.isFinalised());
                }
            }
            else
            {
                processor.pull(aData);
            }
        }
        if(aData.rLength() > 0)
        {
            if(processor.available() > 0)
            {
                processor.pull(aData);
            }
            else
            {
                loop = false;
            }
        }
        aData.reset();
    }

    delete codec;
}

//-------------------------------------------------------------------------------------------
*/