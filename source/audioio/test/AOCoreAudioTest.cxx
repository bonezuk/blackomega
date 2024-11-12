#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <CoreAudio/CoreAudioTypes.h>

#include "audioio/inc/AOCoreAudioMacOS.h"
#include "engine/test/RDataMock.h"
#include "audioio/test/AudioHardwareBufferTester.h"
#include "audioio/test/CoreAudioMockIF.h"
#include "audioio/test/AOQueryDeviceMock.h"
#include "audioio/inc/AOQueryCoreAudio.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AOCoreAudioCreateIOTimeStamp : public AOCoreAudioMacOS
{
	public:
		IOTimeStamp testCreateIOTimeStamp(const AudioTimeStamp *sysTime) const;
};

//-------------------------------------------------------------------------------------------

IOTimeStamp AOCoreAudioCreateIOTimeStamp::testCreateIOTimeStamp(const AudioTimeStamp *sysTime) const
{
	return createIOTimeStamp(sysTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,createIOTimeStamp)
{
    common::TimeStamp tStamp = common::TimeStamp::reference();
	
	AudioTimeStamp sysTime;
	memset(&sysTime,0,sizeof(AudioTimeStamp));
	sysTime.mHostTime = AudioConvertNanosToHostTime(tStamp.nano64());
	
	AOCoreAudioCreateIOTimeStamp audio;
	
	IOTimeStamp tS = audio.testCreateIOTimeStamp(&sysTime);
	EXPECT_TRUE(tS.isValid());
	EXPECT_NEAR(tStamp,tS.time(),500);
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioWriteToAudioOutputBufferFromPartData : public AOCoreAudioMacOS
{
	public:
		void testWriteToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,const engine::RData *data,
			tint partNumber,tint inChannelIndex,tint bufferIndex,tint outChannelIndex,tint inputSampleIndex,tint outputSampleIndex,
			tint amount);
};

//-------------------------------------------------------------------------------------------

void AOCoreAudioWriteToAudioOutputBufferFromPartData::testWriteToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,const engine::RData *data,
		tint partNumber,tint inChannelIndex,tint bufferIndex,tint outChannelIndex,tint inputSampleIndex,tint outputSampleIndex,tint amount)
{
	m_volume = 1.0;
	writeToAudioOutputBufferFromPartData(pBuffer,data,partNumber,inChannelIndex,bufferIndex,outChannelIndex,inputSampleIndex,outputSampleIndex,amount);
}

//-------------------------------------------------------------------------------------------

void testWriteToAudioOutputBufferFromPartData(const sample_t *inputData,
                                              const tfloat32 *originalOutputBuffer,
                                              const tfloat32 *expectOutputBuffer,
                                              tint inputSize,
                                              tint outputSize,
                                              tint noInputChannels,
                                              tint noOutputChannels,
                                              tint inputChannelIndex,
                                              tint outputChannelIndex,
                                              tint inputSampleIndex,
                                              tint outputSampleIndex,
                                              tint amount)
{
	tbyte *actualOutput = new tbyte [outputSize * noOutputChannels * sizeof(tfloat32)];
	memcpy(actualOutput,originalOutputBuffer,outputSize * noOutputChannels * sizeof(tfloat32));
	
    RDataMock data;
	EXPECT_CALL(data,partDataOutConst(2)).WillRepeatedly(Return(inputData));
	EXPECT_CALL(data,noOutChannels()).Times(1).WillOnce(Return(noInputChannels));
	EXPECT_CALL(data,partGetDataType(2)).WillRepeatedly(Return(engine::e_SampleFloat));
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,buffer(3)).Times(1).WillOnce(Return(actualOutput));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(3)).Times(1).WillOnce(Return(noOutputChannels));
	
	AOCoreAudioWriteToAudioOutputBufferFromPartData audio;
	
	audio.testWriteToAudioOutputBufferFromPartData(&buffer,&data,2,inputChannelIndex,3,outputChannelIndex,inputSampleIndex,outputSampleIndex,amount);
	
    EXPECT_EQ(0,memcmp(expectOutputBuffer,actualOutput,outputSize * noOutputChannels * sizeof(tfloat32)));
	
	delete [] actualOutput;
}

//-------------------------------------------------------------------------------------------                                              
                                              
TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToEndToOutputStartToEnd1Channel)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 1;
	static const tint c_noOutputChannels = 1;
	static const tint c_amount = 10;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, //  0
		-0.8f, //  1
		-0.6f, //  2
		-0.4f, //  3
		-0.2f, //  4
		 0.2f, //  5
		 0.4f, //  6
		 0.6f, //  7
		 0.8f, //  8
		 1.0f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, //  0
		-0.8, //  1
		-0.6, //  2
		-0.4, //  3
		-0.2, //  4
		 0.2, //  5
		 0.4, //  6
		 0.6, //  7
		 0.8, //  8
		 1.0  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f, //  0
		 9.9f, //  1
		 9.9f, //  2
		 9.9f, //  3
		 9.9f, //  4
		 9.9f, //  5
		 9.9f, //  6
		 9.9f, //  7
		 9.9f, //  8
		 9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		-1.0f, //  0
		-0.8f, //  1
		-0.6f, //  2
		-0.4f, //  3
		-0.2f, //  4
		 0.2f, //  5
		 0.4f, //  6
		 0.6f, //  7
		 0.8f, //  8
		 1.0f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,0,0,0,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToMiddleToOutputStartToEnd1Channel)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 5;
	static const tint c_noInputChannels = 1;
	static const tint c_noOutputChannels = 1;
	static const tint c_amount = 5;
	
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, //  0
		-0.8f, //  1
		-0.6f, //  2
		-0.4f, //  3
		-0.2f, //  4
		 0.2f, //  5
		 0.4f, //  6
		 0.6f, //  7
		 0.8f, //  8
		 1.0f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, //  0
		-0.8, //  1
		-0.6, //  2
		-0.4, //  3
		-0.2, //  4
		 0.2, //  5
		 0.4, //  6
		 0.6, //  7
		 0.8, //  8
		 1.0  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f, //  0
		 9.9f, //  1
		 9.9f, //  2
		 9.9f, //  3
		 9.9f, //  4
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		-1.0f, //  0
		-0.8f, //  1
		-0.6f, //  2
		-0.4f, //  3
		-0.2f, //  4
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,0,0,0,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputInMiddleToOutputStartToEnd1Channel)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 5;
	static const tint c_noInputChannels = 1;
	static const tint c_noOutputChannels = 1;
	static const tint c_amount = 5;
	
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, //  0
		-0.8f, //  1
		-0.6f, //  2
		-0.4f, //  3
		-0.2f, //  4
		 0.2f, //  5
		 0.4f, //  6
		 0.6f, //  7
		 0.8f, //  8
		 1.0f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, //  0
		-0.8, //  1
		-0.6, //  2
		-0.4, //  3
		-0.2, //  4
		 0.2, //  5
		 0.4, //  6
		 0.6, //  7
		 0.8, //  8
		 1.0  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f, //  0
		 9.9f, //  1
		 9.9f, //  2
		 9.9f, //  3
		 9.9f, //  4
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		-0.6f, //  0
		-0.4f, //  1
		-0.2f, //  2
		 0.2f, //  3
		 0.4f, //  4
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,0,0,2,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputMiddleToEndToOutputStartToEnd1Channel)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 3;
	static const tint c_noInputChannels = 1;
	static const tint c_noOutputChannels = 1;
	static const tint c_amount = 3;
	
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, //  0
		-0.8f, //  1
		-0.6f, //  2
		-0.4f, //  3
		-0.2f, //  4
		 0.2f, //  5
		 0.4f, //  6
		 0.6f, //  7
		 0.8f, //  8
		 1.0f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, //  0
		-0.8, //  1
		-0.6, //  2
		-0.4, //  3
		-0.2, //  4
		 0.2, //  5
		 0.4, //  6
		 0.6, //  7
		 0.8, //  8
		 1.0  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f, //  0
		 9.9f, //  1
		 9.9f, //  2
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 0.6f, //  7
		 0.8f, //  8
		 1.0f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,0,0,7,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToEndToOutputStartToEndWith2ChannelsLeftToLeft)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 2;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 10;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, //  0
		-0.8f, -0.7f, //  1
		-0.6f, -0.5f, //  2
		-0.4f, -0.3f, //  3
		-0.2f, -0.1f, //  4
		 0.2f,  0.1f, //  5
		 0.4f,  0.3f, //  6
		 0.6f,  0.5f, //  7
		 0.8f,  0.7f, //  8
		 1.0f,  0.9f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, //  0
		-0.8, -0.7, //  1
		-0.6, -0.5, //  2
		-0.4, -0.3, //  3
		-0.2, -0.1, //  4
		 0.2,  0.1, //  5
		 0.4,  0.3, //  6
		 0.6,  0.5, //  7
		 0.8,  0.7, //  8
		 1.0,  0.9  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		-1.0f,  9.9f, //  0
		-0.8f,  9.9f, //  1
		-0.6f,  9.9f, //  2
		-0.4f,  9.9f, //  3
		-0.2f,  9.9f, //  4
		 0.2f,  9.9f, //  5
		 0.4f,  9.9f, //  6
		 0.6f,  9.9f, //  7
		 0.8f,  9.9f, //  8
		 1.0f,  9.9f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,0,0,0,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToEndToOutputStartToEndWith2ChannelsLeftToRight)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 2;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 10;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, //  0
		-0.8f, -0.7f, //  1
		-0.6f, -0.5f, //  2
		-0.4f, -0.3f, //  3
		-0.2f, -0.1f, //  4
		 0.2f,  0.1f, //  5
		 0.4f,  0.3f, //  6
		 0.6f,  0.5f, //  7
		 0.8f,  0.7f, //  8
		 1.0f,  0.9f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, //  0
		-0.8, -0.7, //  1
		-0.6, -0.5, //  2
		-0.4, -0.3, //  3
		-0.2, -0.1, //  4
		 0.2,  0.1, //  5
		 0.4,  0.3, //  6
		 0.6,  0.5, //  7
		 0.8,  0.7, //  8
		 1.0,  0.9  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f, -1.0f, //  0
		 9.9f, -0.8f, //  1
		 9.9f, -0.6f, //  2
		 9.9f, -0.4f, //  3
		 9.9f, -0.2f, //  4
		 9.9f,  0.2f, //  5
		 9.9f,  0.4f, //  6
		 9.9f,  0.6f, //  7
		 9.9f,  0.8f, //  8
		 9.9f,  1.0f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,0,1,0,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToEndToOutputStartToEndWith2ChannelsRightToLeft)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 2;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 10;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, //  0
		-0.8f, -0.7f, //  1
		-0.6f, -0.5f, //  2
		-0.4f, -0.3f, //  3
		-0.2f, -0.1f, //  4
		 0.2f,  0.1f, //  5
		 0.4f,  0.3f, //  6
		 0.6f,  0.5f, //  7
		 0.8f,  0.7f, //  8
		 1.0f,  0.9f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, //  0
		-0.8, -0.7, //  1
		-0.6, -0.5, //  2
		-0.4, -0.3, //  3
		-0.2, -0.1, //  4
		 0.2,  0.1, //  5
		 0.4,  0.3, //  6
		 0.6,  0.5, //  7
		 0.8,  0.7, //  8
		 1.0,  0.9  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		-0.9f,  9.9f, //  0
		-0.7f,  9.9f, //  1
		-0.5f,  9.9f, //  2
		-0.3f,  9.9f, //  3
		-0.1f,  9.9f, //  4
		 0.1f,  9.9f, //  5
		 0.3f,  9.9f, //  6
		 0.5f,  9.9f, //  7
		 0.7f,  9.9f, //  8
		 0.9f,  9.9f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,1,0,0,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToEndToOutputStartToEndWith2ChannelsRightToRight)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 2;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 10;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, //  0
		-0.8f, -0.7f, //  1
		-0.6f, -0.5f, //  2
		-0.4f, -0.3f, //  3
		-0.2f, -0.1f, //  4
		 0.2f,  0.1f, //  5
		 0.4f,  0.3f, //  6
		 0.6f,  0.5f, //  7
		 0.8f,  0.7f, //  8
		 1.0f,  0.9f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, //  0
		-0.8, -0.7, //  1
		-0.6, -0.5, //  2
		-0.4, -0.3, //  3
		-0.2, -0.1, //  4
		 0.2,  0.1, //  5
		 0.4,  0.3, //  6
		 0.6,  0.5, //  7
		 0.8,  0.7, //  8
		 1.0,  0.9  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f, -0.9f, //  0
		 9.9f, -0.7f, //  1
		 9.9f, -0.5f, //  2
		 9.9f, -0.3f, //  3
		 9.9f, -0.1f, //  4
		 9.9f,  0.1f, //  5
		 9.9f,  0.3f, //  6
		 9.9f,  0.5f, //  7
		 9.9f,  0.7f, //  8
		 9.9f,  0.9f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,1,1,0,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToEndToOutputStartToMiddle2ChannelLeftToLeft)
{
	static const tint c_inputSize = 5;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 2;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 5;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, //  0
		-0.8f, -0.7f, //  1
		-0.6f, -0.5f, //  2
		-0.4f, -0.3f, //  3
		-0.2f, -0.1f, //  4
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, //  0
		-0.8, -0.7, //  1
		-0.6, -0.5, //  2
		-0.4, -0.3, //  3
		-0.2, -0.1, //  4
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		-1.0f,  9.9f, //  0
		-0.8f,  9.9f, //  1
		-0.6f,  9.9f, //  2
		-0.4f,  9.9f, //  3
		-0.2f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,0,0,0,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToMiddleToOutputStartToMiddle2ChannelLeftToRight)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 2;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 4;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, //  0
		-0.8f, -0.7f, //  1
		-0.6f, -0.5f, //  2
		-0.4f, -0.3f, //  3
		-0.2f, -0.1f, //  4
		 0.2f,  0.1f, //  5
		 0.4f,  0.3f, //  6
		 0.6f,  0.5f, //  7
		 0.8f,  0.7f, //  8
		 1.0f,  0.9f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, //  0
		-0.8, -0.7, //  1
		-0.6, -0.5, //  2
		-0.4, -0.3, //  3
		-0.2, -0.1, //  4
		 0.2,  0.1, //  5
		 0.4,  0.3, //  6
		 0.6,  0.5, //  7
		 0.8,  0.7, //  8
		 1.0,  0.9  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f, -1.0f, //  0
		 9.9f, -0.8f, //  1
		 9.9f, -0.6f, //  2
		 9.9f, -0.4f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,0,1,0,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputInMiddleToOutputStartToMiddle2ChannelRightToLeft)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 2;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 6;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, //  0
		-0.8f, -0.7f, //  1
		-0.6f, -0.5f, //  2
		-0.4f, -0.3f, //  3
		-0.2f, -0.1f, //  4
		 0.2f,  0.1f, //  5
		 0.4f,  0.3f, //  6
		 0.6f,  0.5f, //  7
		 0.8f,  0.7f, //  8
		 1.0f,  0.9f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, //  0
		-0.8, -0.7, //  1
		-0.6, -0.5, //  2
		-0.4, -0.3, //  3
		-0.2, -0.1, //  4
		 0.2,  0.1, //  5
		 0.4,  0.3, //  6
		 0.6,  0.5, //  7
		 0.8,  0.7, //  8
		 1.0,  0.9  //  9
	};
#endif

	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		-0.5f,  9.9f, //  2
		-0.3f,  9.9f, //  3
		-0.1f,  9.9f, //  4
		 0.1f,  9.9f, //  5
		 0.3f,  9.9f, //  6
		 0.5f,  9.9f, //  7
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,1,0,2,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputMiddleToEndToOutputStartToMiddle2ChannelRightToRight)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 2;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 4;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, //  0
		-0.8f, -0.7f, //  1
		-0.6f, -0.5f, //  2
		-0.4f, -0.3f, //  3
		-0.2f, -0.1f, //  4
		 0.2f,  0.1f, //  5
		 0.4f,  0.3f, //  6
		 0.6f,  0.5f, //  7
		 0.8f,  0.7f, //  8
		 1.0f,  0.9f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, //  0
		-0.8, -0.7, //  1
		-0.6, -0.5, //  2
		-0.4, -0.3, //  3
		-0.2, -0.1, //  4
		 0.2,  0.1, //  5
		 0.4,  0.3, //  6
		 0.6,  0.5, //  7
		 0.8,  0.7, //  8
		 1.0,  0.9  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  0.3f, //  0
		 9.9f,  0.5f, //  1
		 9.9f,  0.7f, //  2
         9.9f,  0.9f,  //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,1,1,6,0,c_amount);
}

//-------------------------------------------------------------------------------------------


TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToEndToOutputStartToEndWithInput3ChannelsOutput4ChannelsMap1To2)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 3;
	static const tint c_noOutputChannels = 4;
	static const tint c_amount = 10;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, -0.95f, //  0
		-0.8f, -0.7f, -0.85f, //  1
		-0.6f, -0.5f, -0.65f, //  2
		-0.4f, -0.3f, -0.45f, //  3
		-0.2f, -0.1f, -0.25f, //  4
		 0.2f,  0.1f,  0.25f, //  5
		 0.4f,  0.3f,  0.45f, //  6
		 0.6f,  0.5f,  0.65f, //  7
		 0.8f,  0.7f,  0.85f, //  8
		 1.0f,  0.9f,  0.95f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, -0.95, //  0
		-0.8, -0.7, -0.85, //  1
		-0.6, -0.5, -0.65, //  2
		-0.4, -0.3, -0.45, //  3
		-0.2, -0.1, -0.25, //  4
		 0.2,  0.1,  0.25, //  5
		 0.4,  0.3,  0.45, //  6
		 0.6,  0.5,  0.65, //  7
		 0.8,  0.7,  0.85, //  8
		 1.0,  0.9,  0.95  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f,  9.9f,  9.9f, //  0
		 9.9f,  9.9f,  9.9f,  9.9f, //  1
		 9.9f,  9.9f,  9.9f,  9.9f, //  2
		 9.9f,  9.9f,  9.9f,  9.9f, //  3
		 9.9f,  9.9f,  9.9f,  9.9f, //  4
		 9.9f,  9.9f,  9.9f,  9.9f, //  5
		 9.9f,  9.9f,  9.9f,  9.9f, //  6
		 9.9f,  9.9f,  9.9f,  9.9f, //  7
		 9.9f,  9.9f,  9.9f,  9.9f, //  8
         9.9f,  9.9f,  9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f,  -0.9f,  9.9f, //  0
		 9.9f,  9.9f,  -0.7f,  9.9f, //  1
		 9.9f,  9.9f,  -0.5f,  9.9f, //  2
		 9.9f,  9.9f,  -0.3f,  9.9f, //  3
		 9.9f,  9.9f,  -0.1f,  9.9f, //  4
		 9.9f,  9.9f,   0.1f,  9.9f, //  5
		 9.9f,  9.9f,   0.3f,  9.9f, //  6
		 9.9f,  9.9f,   0.5f,  9.9f, //  7
		 9.9f,  9.9f,   0.7f,  9.9f, //  8
         9.9f,  9.9f,   0.9f,  9.9f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,1,2,0,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToEndToOutputInMiddle)
{
	static const tint c_inputSize = 5;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 3;
	static const tint c_noOutputChannels = 4;
	static const tint c_amount = 5;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, -0.95f, //  0
		-0.8f, -0.7f, -0.85f, //  1
		-0.6f, -0.5f, -0.65f, //  2
		-0.4f, -0.3f, -0.45f, //  3
		-0.2f, -0.1f, -0.25f, //  4
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, -0.95, //  0
		-0.8, -0.7, -0.85, //  1
		-0.6, -0.5, -0.65, //  2
		-0.4, -0.3, -0.45, //  3
		-0.2, -0.1, -0.25, //  4
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f,  9.9f,  9.9f, //  0
		 9.9f,  9.9f,  9.9f,  9.9f, //  1
		 9.9f,  9.9f,  9.9f,  9.9f, //  2
		 9.9f,  9.9f,  9.9f,  9.9f, //  3
		 9.9f,  9.9f,  9.9f,  9.9f, //  4
		 9.9f,  9.9f,  9.9f,  9.9f, //  5
		 9.9f,  9.9f,  9.9f,  9.9f, //  6
		 9.9f,  9.9f,  9.9f,  9.9f, //  7
		 9.9f,  9.9f,  9.9f,  9.9f, //  8
         9.9f,  9.9f,  9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f,  9.9f,  9.9f, //  0
		 9.9f,  9.9f,  9.9f,  9.9f, //  1
		 9.9f,  9.9f,  9.9f,  9.9f, //  2
		 9.9f,  9.9f,  -0.9f,  9.9f, //  3
		 9.9f,  9.9f,  -0.7f,  9.9f, //  4
		 9.9f,  9.9f,  -0.5f,  9.9f, //  5
		 9.9f,  9.9f,  -0.3f,  9.9f, //  6
		 9.9f,  9.9f,  -0.1f,  9.9f, //  7
		 9.9f,  9.9f,  9.9f,  9.9f, //  8
		 9.9f,  9.9f,  9.9f,  9.9f, //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,1,2,0,3,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToMiddleToOutputInMiddle)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 3;
	static const tint c_noOutputChannels = 4;
	static const tint c_amount = 6;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, -0.95f, //  0
		-0.8f, -0.7f, -0.85f, //  1
		-0.6f, -0.5f, -0.65f, //  2
		-0.4f, -0.3f, -0.45f, //  3
		-0.2f, -0.1f, -0.25f, //  4
		 0.2f,  0.1f,  0.25f, //  5
		 0.4f,  0.3f,  0.45f, //  6
		 0.6f,  0.5f,  0.65f, //  7
		 0.8f,  0.7f,  0.85f, //  8
		 1.0f,  0.9f,  0.95f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, -0.95, //  0
		-0.8, -0.7, -0.85, //  1
		-0.6, -0.5, -0.65, //  2
		-0.4, -0.3, -0.45, //  3
		-0.2, -0.1, -0.25, //  4
		 0.2,  0.1,  0.25, //  5
		 0.4,  0.3,  0.45, //  6
		 0.6,  0.5,  0.65, //  7
		 0.8,  0.7,  0.85, //  8
		 1.0,  0.9,  0.95  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f,  9.9f,  9.9f, //  0
		 9.9f,  9.9f,  9.9f,  9.9f, //  1
		 9.9f,  9.9f,  9.9f,  9.9f, //  2
		 9.9f,  9.9f,  9.9f,  9.9f, //  3
		 9.9f,  9.9f,  9.9f,  9.9f, //  4
		 9.9f,  9.9f,  9.9f,  9.9f, //  5
		 9.9f,  9.9f,  9.9f,  9.9f, //  6
		 9.9f,  9.9f,  9.9f,  9.9f, //  7
		 9.9f,  9.9f,  9.9f,  9.9f, //  8
         9.9f,  9.9f,  9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f,  9.9f,  9.9f, //  0
         9.9f,  9.9f,  9.9f,  9.9f,  //  1
		 9.9f,  9.9f,  -0.9f,  9.9f, //  2
		 9.9f,  9.9f,  -0.7f,  9.9f, //  3
		 9.9f,  9.9f,  -0.5f,  9.9f, //  4
		 9.9f,  9.9f,  -0.3f,  9.9f, //  5
		 9.9f,  9.9f,  -0.1f,  9.9f, //  6
		 9.9f,  9.9f,   0.1f,  9.9f, //  7
		 9.9f,  9.9f,  9.9f,  9.9f, //  8
         9.9f,  9.9f,  9.9f,  9.9f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,1,2,0,2,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputInMiddleToOutputInMiddle)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 3;
	static const tint c_noOutputChannels = 4;
	static const tint c_amount = 4;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, -0.95f, //  0
		-0.8f, -0.7f, -0.85f, //  1
		-0.6f, -0.5f, -0.65f, //  2
		-0.4f, -0.3f, -0.45f, //  3
		-0.2f, -0.1f, -0.25f, //  4
		 0.2f,  0.1f,  0.25f, //  5
		 0.4f,  0.3f,  0.45f, //  6
		 0.6f,  0.5f,  0.65f, //  7
		 0.8f,  0.7f,  0.85f, //  8
		 1.0f,  0.9f,  0.95f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, -0.95, //  0
		-0.8, -0.7, -0.85, //  1
		-0.6, -0.5, -0.65, //  2
		-0.4, -0.3, -0.45, //  3
		-0.2, -0.1, -0.25, //  4
		 0.2,  0.1,  0.25, //  5
		 0.4,  0.3,  0.45, //  6
		 0.6,  0.5,  0.65, //  7
		 0.8,  0.7,  0.85, //  8
		 1.0,  0.9,  0.95  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f,  9.9f,  9.9f, //  0
		 9.9f,  9.9f,  9.9f,  9.9f, //  1
		 9.9f,  9.9f,  9.9f,  9.9f, //  2
		 9.9f,  9.9f,  9.9f,  9.9f, //  3
		 9.9f,  9.9f,  9.9f,  9.9f, //  4
		 9.9f,  9.9f,  9.9f,  9.9f, //  5
		 9.9f,  9.9f,  9.9f,  9.9f, //  6
		 9.9f,  9.9f,  9.9f,  9.9f, //  7
		 9.9f,  9.9f,  9.9f,  9.9f, //  8
         9.9f,  9.9f,  9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f,  9.9f,  9.9f, //  0
		 9.9f,  9.9f,  9.9f,  9.9f, //  1
		 9.9f,  9.9f,  9.9f,  9.9f, //  2
		 9.9f,  9.9f,  -0.3f,  9.9f, //  3
		 9.9f,  9.9f,  -0.1f,  9.9f, //  4
		 9.9f,  9.9f,   0.1f,  9.9f, //  5
		 9.9f,  9.9f,   0.3f,  9.9f, //  6
		 9.9f,  9.9f,  9.9f,  9.9f, //  7
		 9.9f,  9.9f,  9.9f,  9.9f, //  8
		 9.9f,  9.9f,  9.9f,  9.9f, //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,1,2,3,3,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputMiddleToEndToOutputInMiddle)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 3;
	static const tint c_noOutputChannels = 4;
	static const tint c_amount = 7;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, -0.95f, //  0
		-0.8f, -0.7f, -0.85f, //  1
		-0.6f, -0.5f, -0.65f, //  2
		-0.4f, -0.3f, -0.45f, //  3
		-0.2f, -0.1f, -0.25f, //  4
		 0.2f,  0.1f,  0.25f, //  5
		 0.4f,  0.3f,  0.45f, //  6
		 0.6f,  0.5f,  0.65f, //  7
		 0.8f,  0.7f,  0.85f, //  8
		 1.0f,  0.9f,  0.95f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, -0.95, //  0
		-0.8, -0.7, -0.85, //  1
		-0.6, -0.5, -0.65, //  2
		-0.4, -0.3, -0.45, //  3
		-0.2, -0.1, -0.25, //  4
		 0.2,  0.1,  0.25, //  5
		 0.4,  0.3,  0.45, //  6
		 0.6,  0.5,  0.65, //  7
		 0.8,  0.7,  0.85, //  8
		 1.0,  0.9,  0.95  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f,  9.9f,  9.9f, //  0
		 9.9f,  9.9f,  9.9f,  9.9f, //  1
		 9.9f,  9.9f,  9.9f,  9.9f, //  2
		 9.9f,  9.9f,  9.9f,  9.9f, //  3
		 9.9f,  9.9f,  9.9f,  9.9f, //  4
		 9.9f,  9.9f,  9.9f,  9.9f, //  5
		 9.9f,  9.9f,  9.9f,  9.9f, //  6
		 9.9f,  9.9f,  9.9f,  9.9f, //  7
		 9.9f,  9.9f,  9.9f,  9.9f, //  8
         9.9f,  9.9f,  9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f,  9.9f,  9.9f, //  0
		 9.9f,  9.9f,  -0.3f,  9.9f, //  1
		 9.9f,  9.9f,  -0.1f,  9.9f, //  2
		 9.9f,  9.9f,   0.1f,  9.9f, //  3
		 9.9f,  9.9f,   0.3f,  9.9f, //  4
		 9.9f,  9.9f,   0.5f,  9.9f, //  5
		 9.9f,  9.9f,   0.7f,  9.9f, //  6
         9.9f,  9.9f,   0.9f,  9.9f, //  7
		 9.9f,  9.9f,  9.9f,  9.9f, //  8
         9.9f,  9.9f,  9.9f,  9.9f  //  9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,1,2,3,1,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToEndToOutputStartToEndWithInput8ChannelsOutput2ChannelsMap7To1)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 8;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 10;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, -0.97f, -0.96f, -0.95f, -0.94f, -0.93f, -0.92f, //  0
		-0.8f, -0.7f, -0.87f, -0.86f, -0.85f, -0.84f, -0.83f, -0.82f, //  1
		-0.6f, -0.5f, -0.67f, -0.66f, -0.65f, -0.64f, -0.63f, -0.62f, //  2
		-0.4f, -0.3f, -0.47f, -0.46f, -0.45f, -0.44f, -0.43f, -0.42f, //  3
		-0.2f, -0.1f, -0.27f, -0.26f, -0.25f, -0.24f, -0.23f, -0.22f, //  4
		 0.2f,  0.1f,  0.27f,  0.26f,  0.25f,  0.24f,  0.23f,  0.22f, //  5
		 0.4f,  0.3f,  0.47f,  0.46f,  0.45f,  0.44f,  0.43f,  0.42f, //  6
		 0.6f,  0.5f,  0.67f,  0.66f,  0.65f,  0.64f,  0.63f,  0.62f, //  7
		 0.8f,  0.7f,  0.87f,  0.86f,  0.85f,  0.84f,  0.83f,  0.82f, //  8
		 1.0f,  0.9f,  0.97f,  0.96f,  0.95f,  0.94f,  0.93f,  0.92f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, -0.97, -0.96, -0.95, -0.94, -0.93, -0.92, //  0
		-0.8, -0.7, -0.87, -0.86, -0.85, -0.84, -0.83, -0.82, //  1
		-0.6, -0.5, -0.67, -0.66, -0.65, -0.64, -0.63, -0.62, //  2
		-0.4, -0.3, -0.47, -0.46, -0.45, -0.44, -0.43, -0.42, //  3
		-0.2, -0.1, -0.27, -0.26, -0.25, -0.24, -0.23, -0.22, //  4
		 0.2,  0.1,  0.27,  0.26,  0.25,  0.24,  0.23,  0.22, //  5
		 0.4,  0.3,  0.47,  0.46,  0.45,  0.44,  0.43,  0.42, //  6
		 0.6,  0.5,  0.67,  0.66,  0.65,  0.64,  0.63,  0.62, //  7
		 0.8,  0.7,  0.87,  0.86,  0.85,  0.84,  0.83,  0.82, //  8
		 1.0,  0.9,  0.97,  0.96,  0.95,  0.94,  0.93,  0.92  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f, -0.92f, // 0
		 9.9f, -0.82f, // 1
		 9.9f, -0.62f, // 2
		 9.9f, -0.42f, // 3
		 9.9f, -0.22f, // 4
		 9.9f,  0.22f, // 5
		 9.9f,  0.42f, // 6
		 9.9f,  0.62f, // 7
		 9.9f,  0.82f, // 8
		 9.9f,  0.92f  // 9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,7,1,0,0,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToEndToOutputMiddleToEnd)
{
	static const tint c_inputSize = 3;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 8;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 3;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		 0.6f,  0.5f,  0.67f,  0.66f,  0.65f,  0.64f,  0.63f,  0.62f, //  7
		 0.8f,  0.7f,  0.87f,  0.86f,  0.85f,  0.84f,  0.83f,  0.82f, //  8
		 1.0f,  0.9f,  0.97f,  0.96f,  0.95f,  0.94f,  0.93f,  0.92f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		 0.6,  0.5,  0.67,  0.66,  0.65,  0.64,  0.63,  0.62, //  7
		 0.8,  0.7,  0.87,  0.86,  0.85,  0.84,  0.83,  0.82, //  8
		 1.0,  0.9,  0.97,  0.96,  0.95,  0.94,  0.93,  0.92  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  0.62f, // 7
		 9.9f,  0.82f, // 8
		 9.9f,  0.92f  // 9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,7,1,0,7,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputStartToMiddleToOutputMiddleToEnd)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 8;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 5;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, -0.97f, -0.96f, -0.95f, -0.94f, -0.93f, -0.92f, //  0
		-0.8f, -0.7f, -0.87f, -0.86f, -0.85f, -0.84f, -0.83f, -0.82f, //  1
		-0.6f, -0.5f, -0.67f, -0.66f, -0.65f, -0.64f, -0.63f, -0.62f, //  2
		-0.4f, -0.3f, -0.47f, -0.46f, -0.45f, -0.44f, -0.43f, -0.42f, //  3
		-0.2f, -0.1f, -0.27f, -0.26f, -0.25f, -0.24f, -0.23f, -0.22f, //  4
		 0.2f,  0.1f,  0.27f,  0.26f,  0.25f,  0.24f,  0.23f,  0.22f, //  5
		 0.4f,  0.3f,  0.47f,  0.46f,  0.45f,  0.44f,  0.43f,  0.42f, //  6
		 0.6f,  0.5f,  0.67f,  0.66f,  0.65f,  0.64f,  0.63f,  0.62f, //  7
		 0.8f,  0.7f,  0.87f,  0.86f,  0.85f,  0.84f,  0.83f,  0.82f, //  8
		 1.0f,  0.9f,  0.97f,  0.96f,  0.95f,  0.94f,  0.93f,  0.92f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, -0.97, -0.96, -0.95, -0.94, -0.93, -0.92, //  0
		-0.8, -0.7, -0.87, -0.86, -0.85, -0.84, -0.83, -0.82, //  1
		-0.6, -0.5, -0.67, -0.66, -0.65, -0.64, -0.63, -0.62, //  2
		-0.4, -0.3, -0.47, -0.46, -0.45, -0.44, -0.43, -0.42, //  3
		-0.2, -0.1, -0.27, -0.26, -0.25, -0.24, -0.23, -0.22, //  4
		 0.2,  0.1,  0.27,  0.26,  0.25,  0.24,  0.23,  0.22, //  5
		 0.4,  0.3,  0.47,  0.46,  0.45,  0.44,  0.43,  0.42, //  6
		 0.6,  0.5,  0.67,  0.66,  0.65,  0.64,  0.63,  0.62, //  7
		 0.8,  0.7,  0.87,  0.86,  0.85,  0.84,  0.83,  0.82, //  8
		 1.0,  0.9,  0.97,  0.96,  0.95,  0.94,  0.93,  0.92  //  9
	};
#endif

	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f, -0.92f, // 5
		 9.9f, -0.82f, // 6
		 9.9f, -0.62f, // 7
		 9.9f, -0.42f, // 8
		 9.9f, -0.22f, // 9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,7,1,0,5,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputInMiddleToOutputMiddleToEnd)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 8;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 6;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, -0.97f, -0.96f, -0.95f, -0.94f, -0.93f, -0.92f, //  0
		-0.8f, -0.7f, -0.87f, -0.86f, -0.85f, -0.84f, -0.83f, -0.82f, //  1
		-0.6f, -0.5f, -0.67f, -0.66f, -0.65f, -0.64f, -0.63f, -0.62f, //  2
		-0.4f, -0.3f, -0.47f, -0.46f, -0.45f, -0.44f, -0.43f, -0.42f, //  3
		-0.2f, -0.1f, -0.27f, -0.26f, -0.25f, -0.24f, -0.23f, -0.22f, //  4
		 0.2f,  0.1f,  0.27f,  0.26f,  0.25f,  0.24f,  0.23f,  0.22f, //  5
		 0.4f,  0.3f,  0.47f,  0.46f,  0.45f,  0.44f,  0.43f,  0.42f, //  6
		 0.6f,  0.5f,  0.67f,  0.66f,  0.65f,  0.64f,  0.63f,  0.62f, //  7
		 0.8f,  0.7f,  0.87f,  0.86f,  0.85f,  0.84f,  0.83f,  0.82f, //  8
		 1.0f,  0.9f,  0.97f,  0.96f,  0.95f,  0.94f,  0.93f,  0.92f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, -0.97, -0.96, -0.95, -0.94, -0.93, -0.92, //  0
		-0.8, -0.7, -0.87, -0.86, -0.85, -0.84, -0.83, -0.82, //  1
		-0.6, -0.5, -0.67, -0.66, -0.65, -0.64, -0.63, -0.62, //  2
		-0.4, -0.3, -0.47, -0.46, -0.45, -0.44, -0.43, -0.42, //  3
		-0.2, -0.1, -0.27, -0.26, -0.25, -0.24, -0.23, -0.22, //  4
		 0.2,  0.1,  0.27,  0.26,  0.25,  0.24,  0.23,  0.22, //  5
		 0.4,  0.3,  0.47,  0.46,  0.45,  0.44,  0.43,  0.42, //  6
		 0.6,  0.5,  0.67,  0.66,  0.65,  0.64,  0.63,  0.62, //  7
		 0.8,  0.7,  0.87,  0.86,  0.85,  0.84,  0.83,  0.82, //  8
		 1.0,  0.9,  0.97,  0.96,  0.95,  0.94,  0.93,  0.92  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f, -0.62f, // 4
		 9.9f, -0.42f, // 5
		 9.9f, -0.22f, // 6
		 9.9f,  0.22f, // 7
		 9.9f,  0.42f, // 8
		 9.9f,  0.62f, // 9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,7,1,2,4,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,writeToAudioOutputBufferFromPartDataFromInputMiddleToEndToOutputMiddleToEnd)
{
	static const tint c_inputSize = 10;
	static const tint c_outputSize = 10;
	static const tint c_noInputChannels = 8;
	static const tint c_noOutputChannels = 2;
	static const tint c_amount = 3;

#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0f, -0.9f, -0.97f, -0.96f, -0.95f, -0.94f, -0.93f, -0.92f, //  0
		-0.8f, -0.7f, -0.87f, -0.86f, -0.85f, -0.84f, -0.83f, -0.82f, //  1
		-0.6f, -0.5f, -0.67f, -0.66f, -0.65f, -0.64f, -0.63f, -0.62f, //  2
		-0.4f, -0.3f, -0.47f, -0.46f, -0.45f, -0.44f, -0.43f, -0.42f, //  3
		-0.2f, -0.1f, -0.27f, -0.26f, -0.25f, -0.24f, -0.23f, -0.22f, //  4
		 0.2f,  0.1f,  0.27f,  0.26f,  0.25f,  0.24f,  0.23f,  0.22f, //  5
		 0.4f,  0.3f,  0.47f,  0.46f,  0.45f,  0.44f,  0.43f,  0.42f, //  6
		 0.6f,  0.5f,  0.67f,  0.66f,  0.65f,  0.64f,  0.63f,  0.62f, //  7
		 0.8f,  0.7f,  0.87f,  0.86f,  0.85f,  0.84f,  0.83f,  0.82f, //  8
		 1.0f,  0.9f,  0.97f,  0.96f,  0.95f,  0.94f,  0.93f,  0.92f  //  9
	};
#else
	const sample_t c_inputData[c_inputSize * c_noInputChannels] = {
		-1.0, -0.9, -0.97, -0.96, -0.95, -0.94, -0.93, -0.92, //  0
		-0.8, -0.7, -0.87, -0.86, -0.85, -0.84, -0.83, -0.82, //  1
		-0.6, -0.5, -0.67, -0.66, -0.65, -0.64, -0.63, -0.62, //  2
		-0.4, -0.3, -0.47, -0.46, -0.45, -0.44, -0.43, -0.42, //  3
		-0.2, -0.1, -0.27, -0.26, -0.25, -0.24, -0.23, -0.22, //  4
		 0.2,  0.1,  0.27,  0.26,  0.25,  0.24,  0.23,  0.22, //  5
		 0.4,  0.3,  0.47,  0.46,  0.45,  0.44,  0.43,  0.42, //  6
		 0.6,  0.5,  0.67,  0.66,  0.65,  0.64,  0.63,  0.62, //  7
		 0.8,  0.7,  0.87,  0.86,  0.85,  0.84,  0.83,  0.82, //  8
		 1.0,  0.9,  0.97,  0.96,  0.95,  0.94,  0.93,  0.92  //  9
	};
#endif
	
	const tfloat32 c_originalOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  9.9f, //  7
		 9.9f,  9.9f, //  8
		 9.9f,  9.9f  //  9
	};
	
	const tfloat32 c_expectOutputBuffer[c_outputSize * c_noOutputChannels] = {
		 9.9f,  9.9f, //  0
		 9.9f,  9.9f, //  1
		 9.9f,  9.9f, //  2
		 9.9f,  9.9f, //  3
		 9.9f,  9.9f, //  4
		 9.9f,  9.9f, //  5
		 9.9f,  9.9f, //  6
		 9.9f,  0.62f, // 7
		 9.9f,  0.82f, // 8
		 9.9f,  0.92f  // 9
	};

	testWriteToAudioOutputBufferFromPartData(c_inputData,c_originalOutputBuffer,c_expectOutputBuffer,c_inputSize,c_outputSize,c_noInputChannels,c_noOutputChannels,7,1,7,7,c_amount);
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioSetupPropertyRunLoopTest : public AOCoreAudioMacOS
{
	public:
		AOCoreAudioSetupPropertyRunLoopTest(CFRunLoopRef expectRunLoop,AudioObjectPropertyAddress property,OSStatus err);
		MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
		OSStatus invokeAudioObjectSetPropertyData(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 inDataSize,const void *inData);
		bool testSetupPropertyRunLoop();
	protected:
		CFRunLoopRef m_expectRunLoop;
		AudioObjectPropertyAddress m_property;
		OSStatus m_err;
};

//-------------------------------------------------------------------------------------------

AOCoreAudioSetupPropertyRunLoopTest::AOCoreAudioSetupPropertyRunLoopTest(CFRunLoopRef expectRunLoop,AudioObjectPropertyAddress property,OSStatus err) : AOCoreAudioMacOS()
{
	m_err = err;
	m_expectRunLoop = expectRunLoop;
	memcpy(&m_property,&property,sizeof(AudioObjectPropertyAddress));
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioSetupPropertyRunLoopTest::invokeAudioObjectSetPropertyData(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 inDataSize,const void *inData)
{
	const CFRunLoopRef *runLoop = reinterpret_cast<const CFRunLoopRef *>(inData);
	EXPECT_EQ(0,memcmp(inAddress,&m_property,sizeof(AudioObjectPropertyAddress)));
    EXPECT_EQ(m_expectRunLoop,*runLoop);
	return m_err;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioSetupPropertyRunLoopTest::testSetupPropertyRunLoop()
{
	return setupPropertyRunLoop();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setupPropertyRunLoopAndSetSuccessfully)
{
	CFRunLoopRef expectRunLoop = 0;

	AudioObjectPropertyAddress property;
    memset(&property,0,sizeof(AudioObjectPropertyAddress));
	property.mSelector = kAudioHardwarePropertyRunLoop;
	property.mScope = kAudioObjectPropertyScopeGlobal;
	property.mElement = kAudioObjectPropertyElementMain;

	AOCoreAudioSetupPropertyRunLoopTest audio(expectRunLoop,property,noErr);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(kAudioObjectSystemObject,A<const AudioObjectPropertyAddress *>(),0,0,sizeof(CFRunLoopRef),A<const void *>())).Times(1)
		.WillOnce(Invoke(&audio,&AOCoreAudioSetupPropertyRunLoopTest::invokeAudioObjectSetPropertyData));
	
	ASSERT_TRUE(audio.testSetupPropertyRunLoop());
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setupPropertyRunLoopAndFailsToSet)
{
	OSStatus err = kAudioHardwareNotRunningError;
	CFRunLoopRef expectRunLoop = 0;

	AudioObjectPropertyAddress property;
    memset(&property,0,sizeof(AudioObjectPropertyAddress));
	property.mSelector = kAudioHardwarePropertyRunLoop;
	property.mScope = kAudioObjectPropertyScopeGlobal;
	property.mElement = kAudioObjectPropertyElementMain;

	AOCoreAudioSetupPropertyRunLoopTest audio(expectRunLoop,property,err);
	EXPECT_CALL(audio,printErrorOS(StrEq("setupPropertyRunLoop"),StrEq("Error setting run loop"),err)).Times(1);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(kAudioObjectSystemObject,A<const AudioObjectPropertyAddress *>(),0,0,sizeof(CFRunLoopRef),A<const void *>())).Times(1)
		.WillOnce(Invoke(&audio,&AOCoreAudioSetupPropertyRunLoopTest::invokeAudioObjectSetPropertyData));
	
	ASSERT_FALSE(audio.testSetupPropertyRunLoop());
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioUseExclusiveModeIfAvailableTest : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
		MOCK_CONST_METHOD0(getCurrentProcessID,pid_t());
		bool testUseExclusiveModeIfAvailable(AudioDeviceID devId);	
};

//-------------------------------------------------------------------------------------------

bool AOCoreAudioUseExclusiveModeIfAvailableTest::testUseExclusiveModeIfAvailable(AudioDeviceID devId)
{
	return useExclusiveModeIfAvailable(devId);
}

//-------------------------------------------------------------------------------------------

MATCHER(PropertyIsHogMode, "") { return (arg->mSelector==kAudioDevicePropertyHogMode && arg->mScope==kAudioDevicePropertyScopeOutput && arg->mElement==kAudioObjectPropertyElementMain); }
MATCHER_P(PropertyHasSize,value,"") { return (*((tuint *)(arg))==value); }
MATCHER_P(PropertyHasProcessID,value,"") { return (*((pid_t *)(arg)) == value); }
ACTION_P(SetProcessIDForArguement,value) { *static_cast<pid_t*>(arg5) = value; }

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,useExclusiveModeIfAvailableWhenDeviceCannotSupportExecutiveMode)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(false));
	
	AOCoreAudioUseExclusiveModeIfAvailableTest audio;
	
	EXPECT_TRUE(audio.testUseExclusiveModeIfAvailable(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,useExclusiveModeIfAvailableWhenErrorQueryingIfHogModeCanBeSet)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));
	
	AOCoreAudioUseExclusiveModeIfAvailableTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("useExclusiveModeIfAvailable"),StrEq("Error querying if exclusive mode can be set"),kAudioHardwareNotRunningError)).Times(1);
	
	EXPECT_FALSE(audio.testUseExclusiveModeIfAvailable(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,useExclusiveModeIfAvailableWhenHogModeCannotBeSet)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(false),Return(static_cast<OSStatus>(noErr))));
	
	AOCoreAudioUseExclusiveModeIfAvailableTest audio;
	
	EXPECT_TRUE(audio.testUseExclusiveModeIfAvailable(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,useExclusiveModeIfAvailableErrorFindingProcessWithHogMode)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsHogMode(),0,0,PropertyHasSize(sizeof(pid_t)),A<void *>())).Times(1)
		.WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));
	
	AOCoreAudioUseExclusiveModeIfAvailableTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("useExclusiveModeIfAvailable"),StrEq("Failed to get exclusive mode process information"),kAudioHardwareNotRunningError)).Times(1);
	
	EXPECT_FALSE(audio.testUseExclusiveModeIfAvailable(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,useExclusiveModeIfAvailableGivenAvailableAndSuccessInGettingHogMode)
{
	pid_t eProcessID = -1;
	pid_t cProcessID = 4;
	
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
    EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsHogMode(),0,0,PropertyHasSize(sizeof(pid_t)),A<void *>())).Times(1)
    	.WillOnce(DoAll(SetProcessIDForArguement(eProcessID),Return(static_cast<OSStatus>(noErr))));
    EXPECT_CALL(apiMock,AudioObjectSetPropertyData(devID,PropertyIsHogMode(),0,0,Eq(sizeof(pid_t)),PropertyHasProcessID(cProcessID)))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
	AOCoreAudioUseExclusiveModeIfAvailableTest audio;
	EXPECT_CALL(audio,getCurrentProcessID()).Times(1).WillOnce(Return(cProcessID));
	
	EXPECT_TRUE(audio.testUseExclusiveModeIfAvailable(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,useExclusiveModeIfAvailableGivenAvailableAndFailInGettingHogMode)
{
	pid_t eProcessID = -1;
	pid_t cProcessID = 4;
	
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsHogMode(),0,0,PropertyHasSize(sizeof(pid_t)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetProcessIDForArguement(eProcessID),Return(static_cast<OSStatus>(noErr))));
    EXPECT_CALL(apiMock,AudioObjectSetPropertyData(devID,PropertyIsHogMode(),0,0,Eq(sizeof(pid_t)),PropertyHasProcessID(cProcessID)))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));
	
	AOCoreAudioUseExclusiveModeIfAvailableTest audio;
	EXPECT_CALL(audio,getCurrentProcessID()).Times(1).WillOnce(Return(cProcessID));
	EXPECT_CALL(audio,printErrorOS(StrEq("useExclusiveModeIfAvailable"),StrEq("Failed to set exclusive mode for audio device"),kAudioHardwareNotRunningError)).Times(1);
	
	EXPECT_FALSE(audio.testUseExclusiveModeIfAvailable(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,useExclusiveModeIfAvailableGivenExclusiveAccessAlreadyHeldByCallingProcess)
{
	pid_t eProcessID = 4;
	pid_t cProcessID = 4;
	
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsHogMode(),0,0,PropertyHasSize(sizeof(pid_t)),A<void *>())).Times(1)
		.WillOnce(DoAll(SetProcessIDForArguement(eProcessID),Return(static_cast<OSStatus>(noErr))));
	
	AOCoreAudioUseExclusiveModeIfAvailableTest audio;
	EXPECT_CALL(audio,getCurrentProcessID()).Times(1).WillOnce(Return(cProcessID));
	
	EXPECT_TRUE(audio.testUseExclusiveModeIfAvailable(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,useExclusiveModeIfAvailableGivenExclusiveAccessAlreadyHeldButNotByCallingProcess)
{
	pid_t eProcessID = 2;
	pid_t cProcessID = 4;
	
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
    EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsHogMode(),0,0,PropertyHasSize(sizeof(pid_t)),A<void *>())).Times(1)
    	.WillOnce(DoAll(SetProcessIDForArguement(eProcessID),Return(static_cast<OSStatus>(noErr))));
	
	AOCoreAudioUseExclusiveModeIfAvailableTest audio;
	EXPECT_CALL(audio,getCurrentProcessID()).Times(1).WillOnce(Return(cProcessID));
	
	EXPECT_FALSE(audio.testUseExclusiveModeIfAvailable(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioDisableMixingIfPossible : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
		bool testDisableMixingIfPossible(AudioDeviceID devID);
};

//-------------------------------------------------------------------------------------------

bool AOCoreAudioDisableMixingIfPossible::testDisableMixingIfPossible(AudioDeviceID devID)
{
	return disableMixingIfPossible(devID);
}

//-------------------------------------------------------------------------------------------

MATCHER(PropertyIsNominalSampleRate, "") { return (arg->mSelector==kAudioDevicePropertyNominalSampleRate && arg->mScope==kAudioDevicePropertyScopeOutput && arg->mElement==kAudioObjectPropertyElementMain); }
MATCHER_P(PropertyHasSampleRate,value,"") { return static_cast<int>(*((Float64 *)(arg)) == value); }

MATCHER(PropertyIsMixingSupported, "") { return (arg->mSelector==kAudioDevicePropertySupportsMixing && arg->mScope==kAudioObjectPropertyScopeGlobal && arg->mElement==kAudioObjectPropertyElementMain); }
MATCHER_P(PropertyHasMixingEnabled,value,"") { return (*((UInt32 *)(arg)) == value); }
ACTION_P(SetMixingEnabledForArgument,value) { *static_cast<UInt32 *>(arg5) = value; }

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,disableMixingIfPossibleWhenDeviceHasNotGotMixingProperty)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(false));
	
	AOCoreAudioDisableMixingIfPossible audio;
		
	EXPECT_FALSE(audio.testDisableMixingIfPossible(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,disableMixingIfPossibleFailToGetPropertyWriteable)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));
	
	AOCoreAudioDisableMixingIfPossible audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("disableMixingIfPossible"),StrEq("Error getting if mixing property is writeable"),kAudioHardwareNotRunningError)).Times(1);
	
	EXPECT_FALSE(audio.testDisableMixingIfPossible(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,disableMixingIfPossibleMixingPropertyNotWriteable)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(false),Return(static_cast<OSStatus>(noErr))));
	
	AOCoreAudioDisableMixingIfPossible audio;
	
	EXPECT_FALSE(audio.testDisableMixingIfPossible(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,disableMixingIfPossibleFailToGetPropertySize)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyIsMixingSupported(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(UInt32)),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));
	
	AOCoreAudioDisableMixingIfPossible audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("disableMixingIfPossible"),StrEq("Error getting mixing property size"),kAudioHardwareNotRunningError)).Times(1);
	
	EXPECT_FALSE(audio.testDisableMixingIfPossible(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,disableMixingIfPossibleFailToGetIfEnabled)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyIsMixingSupported(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(UInt32)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsMixingSupported(),0,0,PropertyHasSize(sizeof(UInt32)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetMixingEnabledForArgument(1),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));
	
	AOCoreAudioDisableMixingIfPossible audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("disableMixingIfPossible"),StrEq("Error getting mixing property"),kAudioHardwareNotRunningError)).Times(1);
	
	EXPECT_FALSE(audio.testDisableMixingIfPossible(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,disableMixingIfPossibleMixingAlreadyDisabled)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyIsMixingSupported(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(UInt32)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsMixingSupported(),0,0,PropertyHasSize(sizeof(UInt32)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetMixingEnabledForArgument(0),Return(static_cast<OSStatus>(noErr))));
	
	AOCoreAudioDisableMixingIfPossible audio;
	
	EXPECT_FALSE(audio.testDisableMixingIfPossible(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,disableMixingIfPossibleMixingEnabledAndFailureDisabling)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyIsMixingSupported(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(UInt32)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsMixingSupported(),0,0,PropertyHasSize(sizeof(UInt32)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetMixingEnabledForArgument(1),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(devID,PropertyIsMixingSupported(),0,0,Eq(sizeof(UInt32)),PropertyHasMixingEnabled(0)))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));
	
	AOCoreAudioDisableMixingIfPossible audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("disableMixingIfPossible"),StrEq("Error disabling mixing property on audio device"),kAudioHardwareNotRunningError)).Times(1);
	
	EXPECT_FALSE(audio.testDisableMixingIfPossible(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,disableMixingIfPossibleMixingEnabledAndSuccessDisabling)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyIsMixingSupported(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(UInt32)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsMixingSupported(),0,0,PropertyHasSize(sizeof(UInt32)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetMixingEnabledForArgument(1),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(devID,PropertyIsMixingSupported(),0,0,Eq(sizeof(UInt32)),PropertyHasMixingEnabled(0)))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
	AOCoreAudioDisableMixingIfPossible audio;
	
	EXPECT_TRUE(audio.testDisableMixingIfPossible(devID));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioReEnableMixingTest : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
		void testReEnableMixing(AudioDeviceID devID);
};

//-------------------------------------------------------------------------------------------

void AOCoreAudioReEnableMixingTest::testReEnableMixing(AudioDeviceID devID)
{
	reEnableMixing(devID);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,reEnableMixingDeviceDoesNotMixing)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(false));
		
	AOCoreAudioReEnableMixingTest audio;
	
	audio.testReEnableMixing(devID);
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,reEnableMixingFailToQueryIfPropertyCanBeSet)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));
		
	AOCoreAudioReEnableMixingTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("reEnableMixing"),StrEq("Error getting if mixing property is writeable"),kAudioHardwareNotRunningError)).Times(1);

	audio.testReEnableMixing(devID);
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,reEnableMixingPropertyCannotBeWrittenTo)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(false),Return(static_cast<OSStatus>(noErr))));
	
	AOCoreAudioReEnableMixingTest audio;

	audio.testReEnableMixing(devID);
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,reEnableMixingFailToQuerySizeOfMixingProperty)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyIsMixingSupported(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(UInt32)),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));
	
	AOCoreAudioReEnableMixingTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("reEnableMixing"),StrEq("Error getting mixing property size"),kAudioHardwareNotRunningError)).Times(1);

	audio.testReEnableMixing(devID);
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,reEnableMixingSuccessRenablingMixing)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyIsMixingSupported(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(UInt32)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(devID,PropertyIsMixingSupported(),0,0,Eq(sizeof(UInt32)),PropertyHasMixingEnabled(1)))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
	AOCoreAudioReEnableMixingTest audio;

	audio.testReEnableMixing(devID);
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,reEnableMixingFailsToRenableMixing)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsMixingSupported())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsMixingSupported(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyIsMixingSupported(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(UInt32)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(devID,PropertyIsMixingSupported(),0,0,Eq(sizeof(UInt32)),PropertyHasMixingEnabled(1)))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));
	
	AOCoreAudioReEnableMixingTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("reEnableMixing"),StrEq("Failed to re-enable mixing on audio device"),kAudioHardwareNotRunningError)).Times(1);
	
    audio.testReEnableMixing(devID);
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioGetAudioStreamsForDeviceTest : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD2(printError,void(const tchar *strR,const tchar *strE));
		MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
		QVector<AudioStreamID> testGetAudioStreamsForDevice(AudioDeviceID devID);
};

//-------------------------------------------------------------------------------------------

QVector<AudioStreamID> AOCoreAudioGetAudioStreamsForDeviceTest::testGetAudioStreamsForDevice(AudioDeviceID devID)
{
	return getAudioStreamsForDevice(devID);
}

//-------------------------------------------------------------------------------------------

MATCHER(PropertyDeviceAudioStreams, "") { return (arg->mSelector==kAudioDevicePropertyStreams && arg->mScope==kAudioDevicePropertyScopeOutput && arg->mElement==kAudioObjectPropertyElementMain); }
ACTION_P(Set1StreamIDsForArgument,value) { (reinterpret_cast<AudioStreamID *>(arg5))[0] = value; }
ACTION_P3(Set3StreamIDsForArgument,valueA,valueB,valueC) { 
	(reinterpret_cast<AudioStreamID *>(arg5))[0] = valueA;
	(reinterpret_cast<AudioStreamID *>(arg5))[1] = valueB;
	(reinterpret_cast<AudioStreamID *>(arg5))[2] = valueC;
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getAudioStreamsForDeviceFailToGetNumberOfStreams)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyDeviceAudioStreams(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));

	AOCoreAudioGetAudioStreamsForDeviceTest audio;
    EXPECT_CALL(audio,printErrorOS(StrEq("getAudioStreamsForDevice"),StrEq("Error getting number of audio streams for device"),kAudioHardwareNotRunningError)).Times(1);
	
	QVector<AudioStreamID> streams = audio.testGetAudioStreamsForDevice(devID);
	
	EXPECT_EQ(0,streams.size());
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getAudioStreamsForDeviceNoStreams)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyDeviceAudioStreams(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(0),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioGetAudioStreamsForDeviceTest audio;
	EXPECT_CALL(audio,printError(StrEq("getAudioStreamsForDevice"),StrEq("No audio streams available for device"))).Times(1);
	
	QVector<AudioStreamID> streams = audio.testGetAudioStreamsForDevice(devID);
	
	EXPECT_EQ(0,streams.size());
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getAudioStreamsForDeviceFailToGetStreamIDs)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyDeviceAudioStreams(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(AudioStreamID)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyDeviceAudioStreams(),0,0,PropertyHasSize(sizeof(UInt32)),A<void *>()))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));

	AOCoreAudioGetAudioStreamsForDeviceTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("getAudioStreamsForDevice"),StrEq("Error getting audio streams for device"),kAudioHardwareNotRunningError)).Times(1);
	
	QVector<AudioStreamID> streams = audio.testGetAudioStreamsForDevice(devID);
	
	EXPECT_EQ(0,streams.size());
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getAudioStreamsForDeviceSuccessWith1Stream)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	AudioStreamID streamA = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyDeviceAudioStreams(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(AudioStreamID)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyDeviceAudioStreams(),0,0,PropertyHasSize(sizeof(UInt32)),A<void *>()))
		.Times(1).WillOnce(DoAll(Set1StreamIDsForArgument(streamA),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioGetAudioStreamsForDeviceTest audio;
	
	QVector<AudioStreamID> streams = audio.testGetAudioStreamsForDevice(devID);
	
	EXPECT_EQ(1,streams.size());
	EXPECT_EQ(streamA,streams.at(0));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getAudioStreamsForDeviceSuccessWith3Streams)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	AudioStreamID streamA = static_cast<AudioStreamID>(400);
	AudioStreamID streamB = static_cast<AudioStreamID>(401);
	AudioStreamID streamC = static_cast<AudioStreamID>(402);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));
	
	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(devID,PropertyDeviceAudioStreams(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(3 * sizeof(AudioStreamID)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyDeviceAudioStreams(),0,0,PropertyHasSize(3 * sizeof(UInt32)),A<void *>()))
		.Times(1).WillOnce(DoAll(Set3StreamIDsForArgument(streamA,streamB,streamC),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioGetAudioStreamsForDeviceTest audio;
	
	QVector<AudioStreamID> streams = audio.testGetAudioStreamsForDevice(devID);
	
	EXPECT_EQ(3,streams.size());
	EXPECT_EQ(streamA,streams.at(0));
	EXPECT_EQ(streamB,streams.at(1));
	EXPECT_EQ(streamC,streams.at(2));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioGetAudioStreamDescriptionsTest : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD2(printError,void(const tchar *strR,const tchar *strE));
		MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
		QVector<AudioStreamRangedDescription> testGetAudioStreamDescriptions(AudioStreamID streamID);
};

//-------------------------------------------------------------------------------------------

QVector<AudioStreamRangedDescription> AOCoreAudioGetAudioStreamDescriptionsTest::testGetAudioStreamDescriptions(AudioStreamID streamID)
{
	return getAudioStreamDescriptions(streamID);
}

//-------------------------------------------------------------------------------------------

MATCHER(PropertyAvailablePhysicalFormats, "") { return (arg->mSelector==kAudioStreamPropertyAvailablePhysicalFormats && arg->mScope==kAudioObjectPropertyScopeGlobal && arg->mElement==0); }
ACTION_P(Set1AudioDescriptionForArgument,value) { (reinterpret_cast<AudioStreamRangedDescription *>(arg5))[0] = value; }
ACTION_P3(Set3AudioDescriptionsForArgument,valueA,valueB,valueC) { 
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[0] = valueA;
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[1] = valueB;
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[2] = valueC;
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getAudioStreamDescriptionsFailedToGetNumberOfFormats)
{
	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));

	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(streamID,PropertyAvailablePhysicalFormats(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(AudioStreamRangedDescription)),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));
		
	AOCoreAudioGetAudioStreamDescriptionsTest audio;
    EXPECT_CALL(audio,printErrorOS(StrEq("getAudioStreamDescriptions"),StrEq("Error getting number of physical stream formats"),kAudioHardwareNotRunningError)).Times(1);
	
	QVector<AudioStreamRangedDescription> descs = audio.testGetAudioStreamDescriptions(streamID);
	
	EXPECT_EQ(0,descs.size());
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getAudioStreamDescriptionsNoFormatsForStream)
{
	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));

	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(streamID,PropertyAvailablePhysicalFormats(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(0),Return(static_cast<OSStatus>(noErr))));
		
	AOCoreAudioGetAudioStreamDescriptionsTest audio;
    EXPECT_CALL(audio,printError(StrEq("getAudioStreamDescriptions"),StrEq("No formats available for given stream"))).Times(1);
	
	QVector<AudioStreamRangedDescription> descs = audio.testGetAudioStreamDescriptions(streamID);
	
	EXPECT_EQ(0,descs.size());
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getAudioStreamDescriptionsFailedToGetFormats)
{
	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));

	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(streamID,PropertyAvailablePhysicalFormats(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(AudioStreamRangedDescription)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertyAvailablePhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamRangedDescription)),A<void *>()))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));
	
	AOCoreAudioGetAudioStreamDescriptionsTest audio;
    EXPECT_CALL(audio,printErrorOS(StrEq("getAudioStreamDescriptions"),StrEq("Failed to get formats for stream ID"),kAudioHardwareNotRunningError)).Times(1);
	
	QVector<AudioStreamRangedDescription> descs = audio.testGetAudioStreamDescriptions(streamID);
	
	EXPECT_EQ(0,descs.size());
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getAudioStreamDescriptionsFor1StreamFormat)
{
	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	AudioStreamRangedDescription rangeA;
	rangeA.mFormat.mSampleRate = 48000.0;
	rangeA.mFormat.mFormatID = kAudioFormatLinearPCM;
	rangeA.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	rangeA.mFormat.mBytesPerPacket = 4;
	rangeA.mFormat.mFramesPerPacket = 1;
	rangeA.mFormat.mBytesPerFrame = 4;
	rangeA.mFormat.mChannelsPerFrame = 2;
	rangeA.mFormat.mBitsPerChannel = 16;
	rangeA.mFormat.mReserved = 0;
	rangeA.mSampleRateRange.mMinimum = 44100.0;
	rangeA.mSampleRateRange.mMaximum = 96000.0;
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));

	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(streamID,PropertyAvailablePhysicalFormats(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(sizeof(AudioStreamRangedDescription)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertyAvailablePhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamRangedDescription)),A<void *>()))
		.Times(1).WillOnce(DoAll(Set1AudioDescriptionForArgument(rangeA),Return(static_cast<OSStatus>(noErr))));
	
	AOCoreAudioGetAudioStreamDescriptionsTest audio;
	
	QVector<AudioStreamRangedDescription> descs = audio.testGetAudioStreamDescriptions(streamID);
	
	EXPECT_EQ(1,descs.size());
	EXPECT_EQ(0,memcmp(&rangeA,&(descs[0]),sizeof(AudioStreamRangedDescription)));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getAudioStreamDescriptionsFor3StreamFormats)
{
	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	AudioStreamRangedDescription rangeA;
	rangeA.mFormat.mSampleRate = 48000.0;
	rangeA.mFormat.mFormatID = kAudioFormatLinearPCM;
	rangeA.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	rangeA.mFormat.mBytesPerPacket = 4;
	rangeA.mFormat.mFramesPerPacket = 1;
	rangeA.mFormat.mBytesPerFrame = 4;
	rangeA.mFormat.mChannelsPerFrame = 2;
	rangeA.mFormat.mBitsPerChannel = 16;
	rangeA.mFormat.mReserved = 0;
	rangeA.mSampleRateRange.mMinimum = 44100.0;
	rangeA.mSampleRateRange.mMaximum = 96000.0;

	AudioStreamRangedDescription rangeB;
	rangeB.mFormat.mSampleRate = 48000.0;
	rangeB.mFormat.mFormatID = kAudioFormatLinearPCM;
	rangeB.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsPacked;
	rangeB.mFormat.mBytesPerPacket = 6;
	rangeB.mFormat.mFramesPerPacket = 1;
	rangeB.mFormat.mBytesPerFrame = 6;
	rangeB.mFormat.mChannelsPerFrame = 2;
	rangeB.mFormat.mBitsPerChannel = 24;
	rangeB.mFormat.mReserved = 0;
	rangeB.mSampleRateRange.mMinimum = 44100.0;
	rangeB.mSampleRateRange.mMaximum = 48000.0;

	AudioStreamRangedDescription rangeC;
	rangeC.mFormat.mSampleRate = 48000.0;
	rangeC.mFormat.mFormatID = kAudioFormatAC3;
	rangeC.mFormat.mFormatFlags = 0;
	rangeC.mFormat.mBytesPerPacket = 6;
	rangeC.mFormat.mFramesPerPacket = 1;
	rangeC.mFormat.mBytesPerFrame = 6;
	rangeC.mFormat.mChannelsPerFrame = 2;
	rangeC.mFormat.mBitsPerChannel = 24;
	rangeC.mFormat.mReserved = 0;
	rangeC.mSampleRateRange.mMinimum = 48000.0;
	rangeC.mSampleRateRange.mMaximum = 48000.0;
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));

	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(streamID,PropertyAvailablePhysicalFormats(),0,0,A<UInt32 *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<4>(3 * sizeof(AudioStreamRangedDescription)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertyAvailablePhysicalFormats(),0,0,PropertyHasSize(3 * sizeof(AudioStreamRangedDescription)),A<void *>()))
		.Times(1).WillOnce(DoAll(Set3AudioDescriptionsForArgument(rangeA,rangeB,rangeC),Return(static_cast<OSStatus>(noErr))));
	
	AOCoreAudioGetAudioStreamDescriptionsTest audio;
	
	QVector<AudioStreamRangedDescription> descs = audio.testGetAudioStreamDescriptions(streamID);
	
	EXPECT_EQ(3,descs.size());
	EXPECT_EQ(0,memcmp(&rangeA,&(descs[0]),sizeof(AudioStreamRangedDescription)));
	EXPECT_EQ(0,memcmp(&rangeB,&(descs[1]),sizeof(AudioStreamRangedDescription)));
	EXPECT_EQ(0,memcmp(&rangeC,&(descs[2]),sizeof(AudioStreamRangedDescription)));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioIsFormatDataTypeCorrispondingTest : public AOCoreAudioMacOS
{
	public:
		bool testIsFormatDataTypeCorrisponding(const AudioStreamBasicDescription& format,const FormatDescription& desc);
};

//-------------------------------------------------------------------------------------------

bool AOCoreAudioIsFormatDataTypeCorrispondingTest::testIsFormatDataTypeCorrisponding(const AudioStreamBasicDescription& format,const FormatDescription& desc)
{
	return isFormatDataTypeCorrisponding(format,desc);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isFormatDataTypeCorrispondingWhenNotLinearPCM)
{
	FormatDescription desc;
	AudioStreamBasicDescription format;
	
	memset(&format,0,sizeof(AudioStreamBasicDescription));
	format.mFormatID = kAudioFormatAC3;
	
	AOCoreAudioIsFormatDataTypeCorrispondingTest audio;
	EXPECT_FALSE(audio.testIsFormatDataTypeCorrisponding(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isFormatDataTypeCorrispondingGivenFormatFloatAndDescFloat)
{
	FormatDescription desc;
	AudioStreamBasicDescription format;
	
	memset(&format,0,sizeof(AudioStreamBasicDescription));
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsFloat;
	
	desc.setTypeOfData(FormatDescription::e_DataFloatSingle);
	
	AOCoreAudioIsFormatDataTypeCorrispondingTest audio;
	EXPECT_TRUE(audio.testIsFormatDataTypeCorrisponding(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isFormatDataTypeCorrispondingGivenFormatFloatAndDescSignedInteger)
{
	FormatDescription desc;
	AudioStreamBasicDescription format;
	
	memset(&format,0,sizeof(AudioStreamBasicDescription));
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsFloat;
	
	desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
	
	AOCoreAudioIsFormatDataTypeCorrispondingTest audio;
	EXPECT_FALSE(audio.testIsFormatDataTypeCorrisponding(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isFormatDataTypeCorrispondingGivenFormatFloatAndDescUnsignedInteger)
{
	FormatDescription desc;
	AudioStreamBasicDescription format;
	
	memset(&format,0,sizeof(AudioStreamBasicDescription));
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsFloat;
	
	desc.setTypeOfData(FormatDescription::e_DataUnsignedInteger);
	
	AOCoreAudioIsFormatDataTypeCorrispondingTest audio;
	EXPECT_FALSE(audio.testIsFormatDataTypeCorrisponding(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isFormatDataTypeCorrispondingGivenFormatSignedIntegerAndDescFloat)
{
	FormatDescription desc;
	AudioStreamBasicDescription format;
	
	memset(&format,0,sizeof(AudioStreamBasicDescription));
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	
	desc.setTypeOfData(FormatDescription::e_DataFloatSingle);
	
	AOCoreAudioIsFormatDataTypeCorrispondingTest audio;
	EXPECT_FALSE(audio.testIsFormatDataTypeCorrisponding(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isFormatDataTypeCorrispondingGivenFormatSignedIntegerAndDescSignedInteger)
{
	FormatDescription desc;
	AudioStreamBasicDescription format;
	
	memset(&format,0,sizeof(AudioStreamBasicDescription));
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	
	desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
	
	AOCoreAudioIsFormatDataTypeCorrispondingTest audio;
	EXPECT_TRUE(audio.testIsFormatDataTypeCorrisponding(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isFormatDataTypeCorrispondingGivenFormatSignedIntegerAndDescUnsignedInteger)
{
	FormatDescription desc;
	AudioStreamBasicDescription format;
	
	memset(&format,0,sizeof(AudioStreamBasicDescription));
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	
	desc.setTypeOfData(FormatDescription::e_DataUnsignedInteger);
	
	AOCoreAudioIsFormatDataTypeCorrispondingTest audio;
	EXPECT_FALSE(audio.testIsFormatDataTypeCorrisponding(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isFormatDataTypeCorrispondingGivenFormatUnsignedIntegerAndDescFloat)
{
	FormatDescription desc;
	AudioStreamBasicDescription format;
	
	memset(&format,0,sizeof(AudioStreamBasicDescription));
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = 0;
	
	desc.setTypeOfData(FormatDescription::e_DataFloatSingle);
	
	AOCoreAudioIsFormatDataTypeCorrispondingTest audio;
	EXPECT_FALSE(audio.testIsFormatDataTypeCorrisponding(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isFormatDataTypeCorrispondingGivenFormatUnsignedIntegerAndDescSignedInteger)
{
	FormatDescription desc;
	AudioStreamBasicDescription format;
	
	memset(&format,0,sizeof(AudioStreamBasicDescription));
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = 0;
	
	desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
	
	AOCoreAudioIsFormatDataTypeCorrispondingTest audio;
	EXPECT_FALSE(audio.testIsFormatDataTypeCorrisponding(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isFormatDataTypeCorrispondingGivenFormatUnsignedIntegerAndDescUnsignedInteger)
{
	FormatDescription desc;
	AudioStreamBasicDescription format;
	
	memset(&format,0,sizeof(AudioStreamBasicDescription));
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = 0;
	
	desc.setTypeOfData(FormatDescription::e_DataUnsignedInteger);
	
	AOCoreAudioIsFormatDataTypeCorrispondingTest audio;
	EXPECT_TRUE(audio.testIsFormatDataTypeCorrisponding(format,desc));
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioFindFrequenciesFromRangeTest : public AOCoreAudioMacOS
{
	public:
		AOCoreAudioFindFrequenciesFromRangeTest(int devIdx);
		MOCK_METHOD0(getDeviceInfo,AOQueryDevice *());
		QVector<int> testFindFrequenciesFromRange(const AudioStreamRangedDescription& range,bool useRange);
};

//-------------------------------------------------------------------------------------------

AOCoreAudioFindFrequenciesFromRangeTest::AOCoreAudioFindFrequenciesFromRangeTest(int devIdx) : AOCoreAudioMacOS()
{
	m_deviceIdx = devIdx;
}

//-------------------------------------------------------------------------------------------

QVector<int> AOCoreAudioFindFrequenciesFromRangeTest::testFindFrequenciesFromRange(const AudioStreamRangedDescription& range,bool useRange)
{
	return findFrequenciesFromRange(range,useRange);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,findFrequenciesFromRangeUseRangeAndCurrentDeviceNotInScope)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	range.mFormat.mBytesPerPacket = 4;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 4;
	range.mFormat.mChannelsPerFrame = 2;
	range.mFormat.mBitsPerChannel = 16;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 22050.0;
	range.mSampleRateRange.mMaximum = 64000.0;

	AOQueryDeviceMock deviceInfo;
	EXPECT_CALL(deviceInfo,noDevices()).Times(1).WillOnce(Return(3));
	
	AOCoreAudioFindFrequenciesFromRangeTest audio(4);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&deviceInfo));
	
	QVector<int> freq = audio.testFindFrequenciesFromRange(range,true);
	
	EXPECT_EQ(0,freq.size());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,findFrequenciesFromRangeUseRangeAndNoFrequenciesDefined)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	range.mFormat.mBytesPerPacket = 4;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 4;
	range.mFormat.mChannelsPerFrame = 2;
	range.mFormat.mBitsPerChannel = 16;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 22050.0;
	range.mSampleRateRange.mMaximum = 64000.0;

	QSet<int> deviceFreq;
	
	AOQueryDeviceDeviceMock device;
	EXPECT_CALL(device,frequencies()).Times(1).WillOnce(ReturnRef(deviceFreq));
	
	AOQueryDeviceMock deviceInfo;
	EXPECT_CALL(deviceInfo,noDevices()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(deviceInfo,device(2)).Times(1).WillOnce(ReturnRef(device));
	
	AOCoreAudioFindFrequenciesFromRangeTest audio(2);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&deviceInfo));
	
	QVector<int> freq = audio.testFindFrequenciesFromRange(range,true);
	
	EXPECT_EQ(0,freq.size());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,findFrequenciesFromRangeUseRangeAndNoneOfTheSupportedFrequenciesAreInRange)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	range.mFormat.mBytesPerPacket = 4;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 4;
	range.mFormat.mChannelsPerFrame = 2;
	range.mFormat.mBitsPerChannel = 16;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 8000.0;
	range.mSampleRateRange.mMaximum = 20000.0;

	QSet<int> deviceFreq;
	deviceFreq << 22050 << 32000 << 44100 << 48000 << 64000;
	
	AOQueryDeviceDeviceMock device;
	EXPECT_CALL(device,frequencies()).Times(1).WillOnce(ReturnRef(deviceFreq));
	
	AOQueryDeviceMock deviceInfo;
	EXPECT_CALL(deviceInfo,noDevices()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(deviceInfo,device(2)).Times(1).WillOnce(ReturnRef(device));
	
	AOCoreAudioFindFrequenciesFromRangeTest audio(2);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&deviceInfo));
	
	QVector<int> freq = audio.testFindFrequenciesFromRange(range,true);
	
	EXPECT_EQ(0,freq.size());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,findFrequenciesFromRangeUseRangeAndOneSupportedFrequenciesAreInRange)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	range.mFormat.mBytesPerPacket = 4;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 4;
	range.mFormat.mChannelsPerFrame = 2;
	range.mFormat.mBitsPerChannel = 16;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 48000.0;
	range.mSampleRateRange.mMaximum = 48000.0;

	QSet<int> deviceFreq;
	deviceFreq << 22050 << 32000 << 44100 << 48000 << 64000;
	
	AOQueryDeviceDeviceMock device;
	EXPECT_CALL(device,frequencies()).Times(1).WillOnce(ReturnRef(deviceFreq));
	
	AOQueryDeviceMock deviceInfo;
	EXPECT_CALL(deviceInfo,noDevices()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(deviceInfo,device(2)).Times(1).WillOnce(ReturnRef(device));
	
	AOCoreAudioFindFrequenciesFromRangeTest audio(2);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&deviceInfo));
	
	QVector<int> freq = audio.testFindFrequenciesFromRange(range,true);
	
	EXPECT_EQ(1,freq.size());
	EXPECT_TRUE(freq.contains(48000));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,findFrequenciesFromRangeUseRangeAndTwoSupportedFrequenciesAreInRange)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	range.mFormat.mBytesPerPacket = 4;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 4;
	range.mFormat.mChannelsPerFrame = 2;
	range.mFormat.mBitsPerChannel = 16;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 44100.0;
	range.mSampleRateRange.mMaximum = 48000.0;

	QSet<int> deviceFreq;
	deviceFreq << 22050 << 32000 << 44100 << 48000 << 64000;
	
	AOQueryDeviceDeviceMock device;
	EXPECT_CALL(device,frequencies()).Times(1).WillOnce(ReturnRef(deviceFreq));
	
	AOQueryDeviceMock deviceInfo;
	EXPECT_CALL(deviceInfo,noDevices()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(deviceInfo,device(2)).Times(1).WillOnce(ReturnRef(device));
	
	AOCoreAudioFindFrequenciesFromRangeTest audio(2);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&deviceInfo));
	
	QVector<int> freq = audio.testFindFrequenciesFromRange(range,true);
	
	EXPECT_EQ(2,freq.size());
	EXPECT_TRUE(freq.contains(44100));
	EXPECT_TRUE(freq.contains(48000));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,findFrequenciesFromRangeUseRangeAndAllFrequenciesAreInRange)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	range.mFormat.mBytesPerPacket = 4;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 4;
	range.mFormat.mChannelsPerFrame = 2;
	range.mFormat.mBitsPerChannel = 16;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 22050.0;
	range.mSampleRateRange.mMaximum = 64000.0;

	QSet<int> deviceFreq;
	deviceFreq << 22050 << 32000 << 44100 << 48000 << 64000;
	
	AOQueryDeviceDeviceMock device;
	EXPECT_CALL(device,frequencies()).Times(1).WillOnce(ReturnRef(deviceFreq));
	
	AOQueryDeviceMock deviceInfo;
	EXPECT_CALL(deviceInfo,noDevices()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(deviceInfo,device(2)).Times(1).WillOnce(ReturnRef(device));
	
	AOCoreAudioFindFrequenciesFromRangeTest audio(2);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&deviceInfo));
	
	QVector<int> freq = audio.testFindFrequenciesFromRange(range,true);
	
	EXPECT_EQ(5,freq.size());
	EXPECT_TRUE(freq.contains(22050));
	EXPECT_TRUE(freq.contains(32000));
	EXPECT_TRUE(freq.contains(44100));
	EXPECT_TRUE(freq.contains(48000));
	EXPECT_TRUE(freq.contains(64000));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,findFrequenciesFromRangeNotUsingRange)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	range.mFormat.mBytesPerPacket = 4;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 4;
	range.mFormat.mChannelsPerFrame = 2;
	range.mFormat.mBitsPerChannel = 16;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 22050.0;
	range.mSampleRateRange.mMaximum = 64000.0;

	AOCoreAudioFindFrequenciesFromRangeTest audio(2);
	
	QVector<int> freq = audio.testFindFrequenciesFromRange(range,false);
	
	EXPECT_EQ(1,freq.size());
	EXPECT_TRUE(freq.contains(48000));
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioFormatDescriptionsFromRangedTest : public AOCoreAudioMacOS
{
	public:
		AOCoreAudioFormatDescriptionsFromRangedTest(int devIdx);
		MOCK_METHOD0(getDeviceInfo,AOQueryDevice *());
		QVector<FormatDescription> testFormatDescriptionsFromRanged(const AudioStreamRangedDescription& range,bool useRange);
};

//-------------------------------------------------------------------------------------------

AOCoreAudioFormatDescriptionsFromRangedTest::AOCoreAudioFormatDescriptionsFromRangedTest(int devIdx) : AOCoreAudioMacOS()
{
	m_deviceIdx = devIdx;
}

//-------------------------------------------------------------------------------------------

QVector<FormatDescription> AOCoreAudioFormatDescriptionsFromRangedTest::testFormatDescriptionsFromRanged(const AudioStreamRangedDescription& range,bool useRange)
{
	return formatDescriptionsFromRanged(range,useRange);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,formatDescriptionsFromRangedNoFrequencies)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsFloat;
	range.mFormat.mBytesPerPacket = 8;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 8;
	range.mFormat.mChannelsPerFrame = 2;
	range.mFormat.mBitsPerChannel = 32;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 44100.0;
	range.mSampleRateRange.mMaximum = 48000.0;

	QSet<int> deviceFreq;
	deviceFreq << 22050 << 32000 << 64000;
	
	AOQueryDeviceDeviceMock device;
	EXPECT_CALL(device,frequencies()).Times(1).WillOnce(ReturnRef(deviceFreq));
	
	AOQueryDeviceMock deviceInfo;
	EXPECT_CALL(deviceInfo,noDevices()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(deviceInfo,device(2)).Times(1).WillOnce(ReturnRef(device));
	
    AOCoreAudioFormatDescriptionsFromRangedTest audio(2);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&deviceInfo));
	
	QVector<FormatDescription> descs = audio.testFormatDescriptionsFromRanged(range,true);
	
	EXPECT_EQ(0,descs.size());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,formatDescriptionsFromRangedWhenFormatIsNotPCM)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatAC3;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsFloat;
	range.mFormat.mBytesPerPacket = 8;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 8;
	range.mFormat.mChannelsPerFrame = 2;
	range.mFormat.mBitsPerChannel = 32;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 44100.0;
	range.mSampleRateRange.mMaximum = 48000.0;

	QSet<int> deviceFreq;
	deviceFreq << 22050 << 32000 << 44100 << 48000 << 64000;
	
	AOQueryDeviceDeviceMock device;
	EXPECT_CALL(device,frequencies()).Times(1).WillOnce(ReturnRef(deviceFreq));
	
	AOQueryDeviceMock deviceInfo;
	EXPECT_CALL(deviceInfo,noDevices()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(deviceInfo,device(2)).Times(1).WillOnce(ReturnRef(device));
	
    AOCoreAudioFormatDescriptionsFromRangedTest audio(2);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&deviceInfo));
	
	QVector<FormatDescription> descs = audio.testFormatDescriptionsFromRanged(range,true);
	
	EXPECT_EQ(0,descs.size());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,formatDescriptionsFromRangedForFloatPCM)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsFloat;
	range.mFormat.mBytesPerPacket = 8;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 8;
	range.mFormat.mChannelsPerFrame = 2;
	range.mFormat.mBitsPerChannel = 32;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 44100.0;
	range.mSampleRateRange.mMaximum = 48000.0;

	QSet<int> expectFreq;
	expectFreq << 44100 << 48000;
	QSet<int> deviceFreq;
	deviceFreq << 22050 << 32000 << 44100 << 48000 << 64000;
	
	AOQueryDeviceDeviceMock device;
	EXPECT_CALL(device,frequencies()).Times(1).WillOnce(ReturnRef(deviceFreq));
	
	AOQueryDeviceMock deviceInfo;
	EXPECT_CALL(deviceInfo,noDevices()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(deviceInfo,device(2)).Times(1).WillOnce(ReturnRef(device));
	
    AOCoreAudioFormatDescriptionsFromRangedTest audio(2);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&deviceInfo));
	
	QVector<FormatDescription> descs = audio.testFormatDescriptionsFromRanged(range,true);
	
	EXPECT_EQ(2,descs.size());
	
	EXPECT_EQ(FormatDescription::e_DataFloatSingle,descs.at(0).typeOfData());
	EXPECT_EQ(32,descs.at(0).bits());
	EXPECT_EQ(2,descs.at(0).channels());
	EXPECT_TRUE(expectFreq.contains(descs.at(0).frequency()));

	EXPECT_EQ(FormatDescription::e_DataFloatSingle,descs.at(1).typeOfData());
	EXPECT_EQ(32,descs.at(1).bits());
	EXPECT_EQ(2,descs.at(1).channels());
	EXPECT_TRUE(expectFreq.contains(descs.at(1).frequency()));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,formatDescriptionsFromRangedForIntegerPCM)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	range.mFormat.mBytesPerPacket = 24;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 24;
	range.mFormat.mChannelsPerFrame = 8;
	range.mFormat.mBitsPerChannel = 24;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 22050.0;
	range.mSampleRateRange.mMaximum = 32000.0;

	QSet<int> expectFreq;
	expectFreq << 22050 << 32000;
	QSet<int> deviceFreq;
	deviceFreq << 22050 << 32000 << 44100 << 48000 << 64000;
	
	AOQueryDeviceDeviceMock device;
	EXPECT_CALL(device,frequencies()).Times(1).WillOnce(ReturnRef(deviceFreq));
	
	AOQueryDeviceMock deviceInfo;
	EXPECT_CALL(deviceInfo,noDevices()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(deviceInfo,device(2)).Times(1).WillOnce(ReturnRef(device));
	
    AOCoreAudioFormatDescriptionsFromRangedTest audio(2);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&deviceInfo));
	
	QVector<FormatDescription> descs = audio.testFormatDescriptionsFromRanged(range,true);
	
	EXPECT_EQ(2,descs.size());
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,descs.at(0).typeOfData());
	EXPECT_EQ(24,descs.at(0).bits());
	EXPECT_EQ(8,descs.at(0).channels());
	EXPECT_TRUE(expectFreq.contains(descs.at(0).frequency()));

	EXPECT_EQ(FormatDescription::e_DataSignedInteger,descs.at(1).typeOfData());
	EXPECT_EQ(24,descs.at(1).bits());
	EXPECT_EQ(8,descs.at(1).channels());
	EXPECT_TRUE(expectFreq.contains(descs.at(1).frequency()));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,formatDescriptionsFromRangedForUnsignedIntegerPCM)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = 48000.0;
	range.mFormat.mFormatID = kAudioFormatLinearPCM;
	range.mFormat.mFormatFlags = 0;
	range.mFormat.mBytesPerPacket = 24;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = 24;
	range.mFormat.mChannelsPerFrame = 8;
	range.mFormat.mBitsPerChannel = 24;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = 22050.0;
	range.mSampleRateRange.mMaximum = 32000.0;

	QSet<int> deviceFreq;
	deviceFreq << 22050 << 32000 << 44100 << 48000 << 64000;
	
    AOCoreAudioFormatDescriptionsFromRangedTest audio(2);
	
	QVector<FormatDescription> descs = audio.testFormatDescriptionsFromRanged(range,false);
	
	EXPECT_EQ(1,descs.size());
	
	EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,descs.at(0).typeOfData());
	EXPECT_EQ(24,descs.at(0).bits());
	EXPECT_EQ(8,descs.at(0).channels());
	EXPECT_EQ(48000,descs.at(0).frequency());
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioAreDescriptionsEquivalentTest : public AOCoreAudioMacOS
{
	public:
		bool testAreDescriptionsEquivalent(const AudioStreamBasicDescription& format,const FormatDescription& desc);
};

//-------------------------------------------------------------------------------------------

bool AOCoreAudioAreDescriptionsEquivalentTest::testAreDescriptionsEquivalent(const AudioStreamBasicDescription& format,const FormatDescription& desc)
{
	return areDescriptionsEquivalent(format,desc);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,areDescriptionsEquivalentWhenNotPCMFormat)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatAC3;
	format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	FormatDescription desc(FormatDescription::e_DataSignedInteger,16,2,44100);

	AOCoreAudioAreDescriptionsEquivalentTest audio;
	EXPECT_FALSE(audio.testAreDescriptionsEquivalent(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,areDescriptionsEquivalentWhenDataTypesAreDifferent)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	FormatDescription desc(FormatDescription::e_DataUnsignedInteger,16,2,44100);

	AOCoreAudioAreDescriptionsEquivalentTest audio;
	EXPECT_FALSE(audio.testAreDescriptionsEquivalent(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,areDescriptionsEquivalentWhenBitsPerSampleAreDifferent)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	FormatDescription desc(FormatDescription::e_DataSignedInteger,24,2,44100);

	AOCoreAudioAreDescriptionsEquivalentTest audio;
	EXPECT_FALSE(audio.testAreDescriptionsEquivalent(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,areDescriptionsEquivalentWhenChannelsAreDifferent)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	FormatDescription desc(FormatDescription::e_DataSignedInteger,16,3,44100);

	AOCoreAudioAreDescriptionsEquivalentTest audio;
	EXPECT_FALSE(audio.testAreDescriptionsEquivalent(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,areDescriptionsEquivalentWhenFrequenciesAreDifferent)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	FormatDescription desc(FormatDescription::e_DataSignedInteger,16,3,48000);

	AOCoreAudioAreDescriptionsEquivalentTest audio;
	EXPECT_FALSE(audio.testAreDescriptionsEquivalent(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,areDescriptionsEquivalentWhenBothAreEquivalent)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	FormatDescription desc(FormatDescription::e_DataSignedInteger,16,2,44100);

	AOCoreAudioAreDescriptionsEquivalentTest audio;
	EXPECT_TRUE(audio.testAreDescriptionsEquivalent(format,desc));
}

//-------------------------------------------------------------------------------------------

AudioStreamRangedDescription buildTestAudioStreamRangedDescription(UInt32 id,UInt32 flags,tint bits,tint channels,tint freq)
{
	AudioStreamRangedDescription range;
	range.mFormat.mSampleRate = static_cast<Float64>(freq);
	range.mFormat.mFormatID = id;
	range.mFormat.mFormatFlags = flags;
	range.mFormat.mBytesPerPacket = (bits / 8) * channels;
	range.mFormat.mFramesPerPacket = 1;
	range.mFormat.mBytesPerFrame = (bits / 8) * channels;
	range.mFormat.mChannelsPerFrame = channels;
	range.mFormat.mBitsPerChannel = bits;
	range.mFormat.mReserved = 0;
	range.mSampleRateRange.mMinimum = static_cast<Float64>(freq);
	range.mSampleRateRange.mMaximum = static_cast<Float64>(freq);
	return range;
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioGetSupportedFormatsForStreamsTest : public AOCoreAudioMacOS
{
	public:
		MOCK_METHOD1(getAudioStreamDescriptions,QVector<AudioStreamRangedDescription>(AudioStreamID streamID));
		FormatsSupported testGetSupportedFormatsForStreams(const QVector<AudioStreamID>& streamIDs);
};

//-------------------------------------------------------------------------------------------

FormatsSupported AOCoreAudioGetSupportedFormatsForStreamsTest::testGetSupportedFormatsForStreams(const QVector<AudioStreamID>& streamIDs)
{
	return getSupportedFormatsForStreams(streamIDs);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,getSupportedFormatsForStreamsMultipleStreams)
{
	AudioStreamRangedDescription rangeA = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger,24,8,48000);
	AudioStreamRangedDescription rangeB = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger,16,2,44100);
	AudioStreamRangedDescription rangeC = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger,24,2,48000);
	AudioStreamRangedDescription rangeD = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger,16,2,48000);
	
	AudioStreamRangedDescription rangeE = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,24,8,48000);
	FormatDescription descE(FormatDescription::e_DataSignedInteger,24,8,48000);
	AudioStreamRangedDescription rangeF = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,16,2,44100);
	FormatDescription descF(FormatDescription::e_DataSignedInteger,16,2,44100);
	AudioStreamRangedDescription rangeG = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,24,2,48000);
	FormatDescription descG(FormatDescription::e_DataSignedInteger,24,2,48000);
	AudioStreamRangedDescription rangeH = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,16,2,48000);
	FormatDescription descH(FormatDescription::e_DataSignedInteger,16,2,48000);

	AudioStreamRangedDescription rangeI = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger,16,4,192000);
	FormatDescription descI(FormatDescription::e_DataSignedInteger,16,4,192000);
	AudioStreamRangedDescription rangeJ = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger,24,2,192000);
	FormatDescription descJ(FormatDescription::e_DataSignedInteger,24,2,192000);
	AudioStreamRangedDescription rangeK = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,16,4,192000);
	
	QVector<AudioStreamRangedDescription> streamListA;
	streamListA.append(rangeA);
	streamListA.append(rangeB);
	streamListA.append(rangeC);
	streamListA.append(rangeD);
	streamListA.append(rangeE);
	streamListA.append(rangeF);
	streamListA.append(rangeG);
	streamListA.append(rangeH);
	
	QVector<AudioStreamRangedDescription> streamListB;
	streamListB.append(rangeI);
	streamListB.append(rangeJ);
	streamListB.append(rangeK);
	
	AudioStreamID streamIDA = static_cast<AudioStreamID>(400);
	AudioStreamID streamIDB = static_cast<AudioStreamID>(401);
	QVector<AudioStreamID> streamIDs;
	streamIDs.append(streamIDA);
	streamIDs.append(streamIDB);
	
	AOCoreAudioGetSupportedFormatsForStreamsTest audio;
	EXPECT_CALL(audio,getAudioStreamDescriptions(streamIDA)).Times(1).WillOnce(Return(streamListA));
	EXPECT_CALL(audio,getAudioStreamDescriptions(streamIDB)).Times(1).WillOnce(Return(streamListB));
	
	FormatsSupported supported = audio.testGetSupportedFormatsForStreams(streamIDs);
	
	EXPECT_TRUE(supported.isSupported(descE));
	EXPECT_TRUE(supported.isSupported(descF));
	EXPECT_TRUE(supported.isSupported(descG));
	EXPECT_TRUE(supported.isSupported(descH));
	
	EXPECT_TRUE(supported.isSupported(descI));
	EXPECT_FALSE(supported.isSupported(descJ));
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioFindClosestStreamTest : public AOCoreAudioMacOS
{
	public:
		QPair<AudioStreamID,AudioStreamBasicDescription *> testFindClosestStream(const FormatDescription& sourceDesc,const QVector<AudioStreamID>& streamIDs);
};

//-------------------------------------------------------------------------------------------

QPair<AudioStreamID,AudioStreamBasicDescription *> AOCoreAudioFindClosestStreamTest::testFindClosestStream(const FormatDescription& sourceDesc,const QVector<AudioStreamID>& streamIDs)
{
	return findClosestStream(sourceDesc,streamIDs);
}

//-------------------------------------------------------------------------------------------

ACTION_P2(Set2AudioDescriptionForArgument,valueA,valueB) { 
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[0] = valueA;
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[1] = valueB;
}

ACTION_P6(Set6AudioDescriptionsForArgument,valueA,valueB,valueC,valueD,valueE,valueF) { 
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[0] = valueA;
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[1] = valueB;
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[2] = valueC;
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[3] = valueD;
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[4] = valueE;
	(reinterpret_cast<AudioStreamRangedDescription *>(arg5))[5] = valueF;
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,findClosestStreamWhenNoStreamsAvailable)
{
	AudioStreamRangedDescription rangeH = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger,16,2,32000);
	AudioStreamRangedDescription rangeI = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger,16,2,22050);

	AudioStreamID streamIDB = static_cast<AudioStreamID>(401);
	QVector<AudioStreamID> streamIDs;
	streamIDs.append(streamIDB);

	FormatDescription sourceDesc(FormatDescription::e_DataFloatDouble,64,2,96000);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(streamIDB,PropertyAvailablePhysicalFormats(),0,0,A<UInt32 *>()))
		.WillRepeatedly(DoAll(SetArgPointee<4>(2 * sizeof(AudioStreamRangedDescription)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamIDB,PropertyAvailablePhysicalFormats(),0,0,PropertyHasSize(2 * sizeof(AudioStreamRangedDescription)),A<void *>()))
		.WillRepeatedly(DoAll(Set2AudioDescriptionForArgument(rangeH,rangeI),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioFindClosestStreamTest audio;
	
	QPair<AudioStreamID,AudioStreamBasicDescription *> closest = audio.testFindClosestStream(sourceDesc,streamIDs);
	
	EXPECT_EQ(static_cast<AudioStreamID>(kAudioObjectUnknown),closest.first);
	EXPECT_TRUE(closest.second==0);
	
	delete closest.second;
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,findClosestStreamFromStreams)
{
	AudioStreamRangedDescription rangeA = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,24,8,48000);
	AudioStreamRangedDescription rangeB = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,16,2,44100);
	AudioStreamRangedDescription rangeC = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,24,2,48000);
	AudioStreamRangedDescription rangeD = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,16,2,48000);
	AudioStreamRangedDescription rangeE = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger,24,2,48000);
	AudioStreamRangedDescription rangeF = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger,24,2,96000);

	AudioStreamRangedDescription rangeH = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,16,2,32000);
	AudioStreamRangedDescription rangeI = buildTestAudioStreamRangedDescription(kAudioFormatLinearPCM,kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsNonMixable,16,2,22050);
	
	AudioStreamID streamIDA = static_cast<AudioStreamID>(400);
	AudioStreamID streamIDB = static_cast<AudioStreamID>(401);
	QVector<AudioStreamID> streamIDs;
	streamIDs.append(streamIDA);
	streamIDs.append(streamIDB);

	FormatDescription sourceDesc(FormatDescription::e_DataFloatDouble,64,2,96000);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(streamIDA,PropertyAvailablePhysicalFormats(),0,0,A<UInt32 *>()))
		.WillRepeatedly(DoAll(SetArgPointee<4>(6 * sizeof(AudioStreamRangedDescription)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamIDA,PropertyAvailablePhysicalFormats(),0,0,PropertyHasSize(6 * sizeof(AudioStreamRangedDescription)),A<void *>()))
		.WillRepeatedly(DoAll(Set6AudioDescriptionsForArgument(rangeA,rangeB,rangeC,rangeD,rangeE,rangeF),Return(static_cast<OSStatus>(noErr))));

	EXPECT_CALL(apiMock,AudioObjectGetPropertyDataSize(streamIDB,PropertyAvailablePhysicalFormats(),0,0,A<UInt32 *>()))
		.WillRepeatedly(DoAll(SetArgPointee<4>(2 * sizeof(AudioStreamRangedDescription)),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamIDB,PropertyAvailablePhysicalFormats(),0,0,PropertyHasSize(2 * sizeof(AudioStreamRangedDescription)),A<void *>()))
		.WillRepeatedly(DoAll(Set2AudioDescriptionForArgument(rangeH,rangeI),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioFindClosestStreamTest audio;
	
	QPair<AudioStreamID,AudioStreamBasicDescription *> closest = audio.testFindClosestStream(sourceDesc,streamIDs);
	
	EXPECT_EQ(streamIDA,closest.first);
	EXPECT_EQ(0,memcmp(closest.second,&rangeC.mFormat,sizeof(AudioStreamBasicDescription)));
	
	delete closest.second;
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioSampleConverterFromDescriptionTest : public AOCoreAudioMacOS
{
	public:
		QSharedPointer<SampleConverter> testSampleConverterFromDescription(const AudioStreamBasicDescription& format);
};

//-------------------------------------------------------------------------------------------

QSharedPointer<SampleConverter> AOCoreAudioSampleConverterFromDescriptionTest::testSampleConverterFromDescription(const AudioStreamBasicDescription& format)
{
	return sampleConverterFromDescription(format);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,sampleConverterFromDescriptionWithFloatAndLittleEndian)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsFloat;
	format.mBytesPerPacket = 8;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 8;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 32;
	format.mReserved = 0;

	AOCoreAudioSampleConverterFromDescriptionTest audio;
	QSharedPointer<SampleConverter> pConverter = audio.testSampleConverterFromDescription(format);
	
	ASSERT_FALSE(pConverter.isNull());
	EXPECT_TRUE(pConverter->isSupported());
	EXPECT_TRUE(pConverter->isLittleEndian());
	EXPECT_EQ(FormatDescription::e_DataFloatSingle,pConverter->type());
	EXPECT_EQ(32,pConverter->bits());
	EXPECT_EQ(4,pConverter->bytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,sampleConverterFromDescriptionWithFloatAndBigEndian)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsBigEndian;
	format.mBytesPerPacket = 16;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 16;
	format.mChannelsPerFrame = 4;
	format.mBitsPerChannel = 32;
	format.mReserved = 0;

	AOCoreAudioSampleConverterFromDescriptionTest audio;
	QSharedPointer<SampleConverter> pConverter = audio.testSampleConverterFromDescription(format);
	
	ASSERT_FALSE(pConverter.isNull());
	EXPECT_TRUE(pConverter->isSupported());
	EXPECT_FALSE(pConverter->isLittleEndian());
	EXPECT_EQ(FormatDescription::e_DataFloatSingle,pConverter->type());
	EXPECT_EQ(32,pConverter->bits());
	EXPECT_EQ(4,pConverter->bytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,sampleConverterFromDescriptionWithSignedIntegerLittleEndianAndAlignLow)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
	format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AOCoreAudioSampleConverterFromDescriptionTest audio;
	QSharedPointer<SampleConverter> pConverter = audio.testSampleConverterFromDescription(format);
	
	ASSERT_FALSE(pConverter.isNull());
	EXPECT_TRUE(pConverter->isSupported());
	EXPECT_TRUE(pConverter->isLittleEndian());
	EXPECT_FALSE(pConverter->isAlignedHigh());
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,pConverter->type());
	EXPECT_EQ(16,pConverter->bits());
	EXPECT_EQ(2,pConverter->bytesPerSample());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,sampleConverterFromDescriptionWithUnsignedIntegerBigEndianAndAlignHigh)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = 24;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 24;
	format.mChannelsPerFrame = 8;
	format.mBitsPerChannel = 20;
	format.mReserved = 0;

	AOCoreAudioSampleConverterFromDescriptionTest audio;
	QSharedPointer<SampleConverter> pConverter = audio.testSampleConverterFromDescription(format);
	
	ASSERT_FALSE(pConverter.isNull());
	EXPECT_TRUE(pConverter->isSupported());
	EXPECT_FALSE(pConverter->isLittleEndian());
	EXPECT_TRUE(pConverter->isAlignedHigh());
	EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,pConverter->type());
	EXPECT_EQ(20,pConverter->bits());
	EXPECT_EQ(3,pConverter->bytesPerSample());
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioFormatFromStreamDescription : public AOCoreAudioMacOS
{
	public:
		bool testFormatFromStreamDescription(const AudioStreamBasicDescription& format,FormatDescription& desc);
};

//-------------------------------------------------------------------------------------------

bool AOCoreAudioFormatFromStreamDescription::testFormatFromStreamDescription(const AudioStreamBasicDescription& format,FormatDescription& desc)
{
    return formatFromStreamDescription(format,desc);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,formatFromStreamDescriptionNonLinearPCM)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 48000.0;
	format.mFormatID = kAudioFormatAC3;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 16;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 16;
	format.mChannelsPerFrame = 4;
	format.mBitsPerChannel = 32;
	format.mReserved = 0;

	FormatDescription desc;

	AOCoreAudioFormatFromStreamDescription audio;
	ASSERT_FALSE(audio.testFormatFromStreamDescription(format,desc));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,formatFromStreamDescriptionWithSingleFloat)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 48000.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsFloat;
	format.mBytesPerPacket = 16;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 16;
	format.mChannelsPerFrame = 4;
	format.mBitsPerChannel = 32;
	format.mReserved = 0;

	FormatDescription desc;

	AOCoreAudioFormatFromStreamDescription audio;
	ASSERT_TRUE(audio.testFormatFromStreamDescription(format,desc));
	
	EXPECT_EQ(FormatDescription::e_DataFloatSingle,desc.typeOfData());
	EXPECT_EQ(32,desc.bits());
	EXPECT_EQ(4,desc.channels());
	EXPECT_EQ(48000,desc.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,formatFromStreamDescriptionWithSignedInteger)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	FormatDescription desc;

	AOCoreAudioFormatFromStreamDescription audio;
	ASSERT_TRUE(audio.testFormatFromStreamDescription(format,desc));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,desc.typeOfData());
	EXPECT_EQ(16,desc.bits());
	EXPECT_EQ(2,desc.channels());
	EXPECT_EQ(44100,desc.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,formatFromStreamDescriptionWithUnsignedInteger)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 192000.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = 24;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 24;
	format.mChannelsPerFrame = 8;
	format.mBitsPerChannel = 24;
	format.mReserved = 0;

	FormatDescription desc;

	AOCoreAudioFormatFromStreamDescription audio;
	ASSERT_TRUE(audio.testFormatFromStreamDescription(format,desc));
	
	EXPECT_EQ(FormatDescription::e_DataUnsignedInteger,desc.typeOfData());
	EXPECT_EQ(24,desc.bits());
	EXPECT_EQ(8,desc.channels());
	EXPECT_EQ(192000,desc.frequency());
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioSetAudioStreamTest : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD2(printError,void(const tchar *strR,const tchar *strE));
		MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
		MOCK_CONST_METHOD0(audioStreamWaitTimeout,tint());

		static OSStatus testAudioStreamChangeListener(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddress[],void *inClientData);
		bool testSetAudioStream(AudioStreamID streamID,const AudioStreamBasicDescription& format);
};

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioSetAudioStreamTest::testAudioStreamChangeListener(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddress[],void *inClientData)
{
	return audioStreamChangeListener(inObjectID,inNumberAddresses,inAddress,inClientData);
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioSetAudioStreamTest::testSetAudioStream(AudioStreamID streamID,const AudioStreamBasicDescription& format)
{
	return setAudioStream(streamID,format);
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioSetAudioStreamThread : public QThread
{
	public:
        AOCoreAudioSetAudioStreamThread(AudioStreamID streamID,AOCoreAudioSetAudioStreamTest *pAudio,QMutex *pMutex);
		virtual ~AOCoreAudioSetAudioStreamThread();
	protected:
		AudioStreamID m_streamID;
		AOCoreAudioSetAudioStreamTest *m_pAudio;
		QMutex *m_mutex;
		
		virtual void run();
};

//-------------------------------------------------------------------------------------------

AOCoreAudioSetAudioStreamThread::AOCoreAudioSetAudioStreamThread(AudioStreamID streamID,AOCoreAudioSetAudioStreamTest *pAudio,QMutex *pMutex) : QThread(),
	m_streamID(streamID),
	m_pAudio(pAudio),
    m_mutex(pMutex)
{}

//-------------------------------------------------------------------------------------------

AOCoreAudioSetAudioStreamThread::~AOCoreAudioSetAudioStreamThread()
{}

//-------------------------------------------------------------------------------------------

void AOCoreAudioSetAudioStreamThread::run()
{
	AudioObjectPropertyAddress property = { kAudioStreamPropertyPhysicalFormat, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };
	
	m_mutex->lock();	
	for(tint i=0;i<5;i++)
	{
		QThread::msleep(20);
		m_pAudio->testAudioStreamChangeListener(m_streamID,1,&property,m_pAudio);
	}
    m_mutex->unlock();
}

//-------------------------------------------------------------------------------------------

MATCHER(PropertySetPhysicalFormats, "") { return (arg->mSelector==kAudioStreamPropertyPhysicalFormat && arg->mScope==kAudioObjectPropertyScopeGlobal && arg->mElement==kAudioObjectPropertyElementMain); }
ACTION_P(SetAudioStreamBasicDescriptionForArguement,value) { *static_cast<AudioStreamBasicDescription*>(arg5) = value; }

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamFailToGetDescription)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatAC3;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,printError(StrEq("setAudioStream"),StrEq("Incompatible audio format"))).Times(1);

	AudioStreamID streamID = static_cast<AudioStreamID>(400);

	EXPECT_FALSE(audio.testSetAudioStream(streamID,format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamFailToAddPropertyListener)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("setAudioStream"),StrEq("Failed to attach property listener"),kAudioHardwareNotRunningError)).Times(1);

	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	
	EXPECT_CALL(apiMock,AudioObjectAddPropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));

	EXPECT_FALSE(audio.testSetAudioStream(streamID,format));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamFailToSetPropertyAndRemoveListener)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("setAudioStream"),StrEq("Could not set the stream format"),kAudioHardwareNotRunningError)).Times(1);
	EXPECT_CALL(audio,printErrorOS(StrEq("setAudioStream"),StrEq("Failed to remove property listener"),kAudioHardwareNotRunningError)).Times(1);

	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	
	EXPECT_CALL(apiMock,AudioObjectAddPropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,Eq(sizeof(AudioStreamBasicDescription)),&format))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));
	EXPECT_CALL(apiMock,AudioObjectRemovePropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));
	
	EXPECT_FALSE(audio.testSetAudioStream(streamID,format));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamFailToGetPropertyInLoopWithoutListener)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AudioStreamBasicDescription oldFormat;
	oldFormat.mSampleRate = 22050.0;
	oldFormat.mFormatID = kAudioFormatLinearPCM;
    oldFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	oldFormat.mBytesPerPacket = 4;
	oldFormat.mFramesPerPacket = 1;
	oldFormat.mBytesPerFrame = 4;
	oldFormat.mChannelsPerFrame = 2;
	oldFormat.mBitsPerChannel = 16;
	oldFormat.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,audioStreamWaitTimeout()).WillRepeatedly(Return(40));
	EXPECT_CALL(audio,printErrorOS(StrEq("setAudioStream"),StrEq("Error getting physical format for device"),kAudioHardwareNotRunningError)).Times(9);

	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	
	EXPECT_CALL(apiMock,AudioObjectAddPropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,Eq(sizeof(AudioStreamBasicDescription)),&format))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamBasicDescription)),A<void *>()))
		.WillRepeatedly(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));
	EXPECT_CALL(apiMock,AudioObjectRemovePropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
	EXPECT_FALSE(audio.testSetAudioStream(streamID,format));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamFailToGetPropertyInLoopWithListener)
{
	QMutex runMutex;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AudioStreamBasicDescription oldFormat;
	oldFormat.mSampleRate = 22050.0;
	oldFormat.mFormatID = kAudioFormatLinearPCM;
    oldFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	oldFormat.mBytesPerPacket = 4;
	oldFormat.mFramesPerPacket = 1;
	oldFormat.mBytesPerFrame = 4;
	oldFormat.mChannelsPerFrame = 2;
	oldFormat.mBitsPerChannel = 16;
	oldFormat.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,audioStreamWaitTimeout()).WillRepeatedly(Return(40));
	EXPECT_CALL(audio,printErrorOS(StrEq("setAudioStream"),StrEq("Error getting physical format for device"),kAudioHardwareNotRunningError)).Times(9);

	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	
	EXPECT_CALL(apiMock,AudioObjectAddPropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,Eq(sizeof(AudioStreamBasicDescription)),&format))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamBasicDescription)),A<void *>()))
		.WillRepeatedly(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));
	EXPECT_CALL(apiMock,AudioObjectRemovePropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
    runMutex.lock();
    AOCoreAudioSetAudioStreamThread thread(streamID,&audio,&runMutex);
	thread.start();
    runMutex.unlock();
	
	EXPECT_FALSE(audio.testSetAudioStream(streamID,format));
	
	thread.wait();
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamFailToDetectStreamChangeInLoopWithoutListener)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AudioStreamBasicDescription oldFormat;
	oldFormat.mSampleRate = 22050.0;
	oldFormat.mFormatID = kAudioFormatLinearPCM;
    oldFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	oldFormat.mBytesPerPacket = 4;
	oldFormat.mFramesPerPacket = 1;
	oldFormat.mBytesPerFrame = 4;
	oldFormat.mChannelsPerFrame = 2;
	oldFormat.mBitsPerChannel = 16;
	oldFormat.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,audioStreamWaitTimeout()).WillRepeatedly(Return(40));

	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	
	EXPECT_CALL(apiMock,AudioObjectAddPropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,Eq(sizeof(AudioStreamBasicDescription)),&format))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamBasicDescription)),A<void *>()))
		.WillRepeatedly(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectRemovePropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
	EXPECT_FALSE(audio.testSetAudioStream(streamID,format));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamFailToDetectStreamChangeInLoopWithListener)
{
	QMutex runMutex;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AudioStreamBasicDescription oldFormat;
	oldFormat.mSampleRate = 22050.0;
	oldFormat.mFormatID = kAudioFormatLinearPCM;
    oldFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	oldFormat.mBytesPerPacket = 4;
	oldFormat.mFramesPerPacket = 1;
	oldFormat.mBytesPerFrame = 4;
	oldFormat.mChannelsPerFrame = 2;
	oldFormat.mBitsPerChannel = 16;
	oldFormat.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,audioStreamWaitTimeout()).WillRepeatedly(Return(40));

	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	
	EXPECT_CALL(apiMock,AudioObjectAddPropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,Eq(sizeof(AudioStreamBasicDescription)),&format))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamBasicDescription)),A<void *>()))
		.WillRepeatedly(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectRemovePropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
    runMutex.lock();
    AOCoreAudioSetAudioStreamThread thread(streamID,&audio,&runMutex);
	thread.start();
    runMutex.unlock();
	
	EXPECT_FALSE(audio.testSetAudioStream(streamID,format));
	
	thread.wait();
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamSuccessOnFirstTryWithoutListener)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AudioStreamBasicDescription oldFormat;
	oldFormat.mSampleRate = 22050.0;
	oldFormat.mFormatID = kAudioFormatLinearPCM;
    oldFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	oldFormat.mBytesPerPacket = 4;
	oldFormat.mFramesPerPacket = 1;
	oldFormat.mBytesPerFrame = 4;
	oldFormat.mChannelsPerFrame = 2;
	oldFormat.mBitsPerChannel = 16;
	oldFormat.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,audioStreamWaitTimeout()).WillRepeatedly(Return(40));

	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	
	EXPECT_CALL(apiMock,AudioObjectAddPropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,Eq(sizeof(AudioStreamBasicDescription)),&format))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamBasicDescription)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(format),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectRemovePropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
	EXPECT_TRUE(audio.testSetAudioStream(streamID,format));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamSuccessOnFirstTryWithListener)
{
	QMutex runMutex;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AudioStreamBasicDescription oldFormat;
	oldFormat.mSampleRate = 22050.0;
	oldFormat.mFormatID = kAudioFormatLinearPCM;
    oldFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	oldFormat.mBytesPerPacket = 4;
	oldFormat.mFramesPerPacket = 1;
	oldFormat.mBytesPerFrame = 4;
	oldFormat.mChannelsPerFrame = 2;
	oldFormat.mBitsPerChannel = 16;
	oldFormat.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,audioStreamWaitTimeout()).WillRepeatedly(Return(40));

	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	
	EXPECT_CALL(apiMock,AudioObjectAddPropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,Eq(sizeof(AudioStreamBasicDescription)),&format))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamBasicDescription)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(format),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectRemovePropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
    runMutex.lock();
    AOCoreAudioSetAudioStreamThread thread(streamID,&audio,&runMutex);
	thread.start();
    runMutex.unlock();
	
	EXPECT_TRUE(audio.testSetAudioStream(streamID,format));
	
	thread.wait();
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamSuccessAfter5TriesWithoutListener)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AudioStreamBasicDescription oldFormat;
	oldFormat.mSampleRate = 22050.0;
	oldFormat.mFormatID = kAudioFormatLinearPCM;
    oldFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	oldFormat.mBytesPerPacket = 4;
	oldFormat.mFramesPerPacket = 1;
	oldFormat.mBytesPerFrame = 4;
	oldFormat.mChannelsPerFrame = 2;
	oldFormat.mBitsPerChannel = 16;
	oldFormat.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,audioStreamWaitTimeout()).WillRepeatedly(Return(40));

	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	
	EXPECT_CALL(apiMock,AudioObjectAddPropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,Eq(sizeof(AudioStreamBasicDescription)),&format))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamBasicDescription)),A<void *>()))
		.Times(5)
		.WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(noErr))))
		.WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(noErr))))
		.WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(noErr))))
		.WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(noErr))))
		.WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(format),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectRemovePropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
	EXPECT_TRUE(audio.testSetAudioStream(streamID,format));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setAudioStreamSuccessAfter5TriesWithListener)
{
	QMutex runMutex;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AudioStreamBasicDescription oldFormat;
	oldFormat.mSampleRate = 22050.0;
	oldFormat.mFormatID = kAudioFormatLinearPCM;
    oldFormat.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	oldFormat.mBytesPerPacket = 4;
	oldFormat.mFramesPerPacket = 1;
	oldFormat.mBytesPerFrame = 4;
	oldFormat.mChannelsPerFrame = 2;
	oldFormat.mBitsPerChannel = 16;
	oldFormat.mReserved = 0;

	AOCoreAudioSetAudioStreamTest audio;
	EXPECT_CALL(audio,audioStreamWaitTimeout()).WillRepeatedly(Return(40));

	AudioStreamID streamID = static_cast<AudioStreamID>(400);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	
	EXPECT_CALL(apiMock,AudioObjectAddPropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,Eq(sizeof(AudioStreamBasicDescription)),&format))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamBasicDescription)),A<void *>()))
		.Times(5)
		.WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(noErr))))
		.WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(noErr))))
		.WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(noErr))))
		.WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(oldFormat),Return(static_cast<OSStatus>(noErr))))
		.WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(format),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectRemovePropertyListener(streamID,PropertySetPhysicalFormats(),A<AudioObjectPropertyListenerProc>(),&audio))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));
	
    runMutex.lock();
    AOCoreAudioSetAudioStreamThread thread(streamID,&audio,&runMutex);
	thread.start();
    runMutex.unlock();
	
	EXPECT_TRUE(audio.testSetAudioStream(streamID,format));
	
	thread.wait();
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioSaveAudioDescriptionForStreamTest : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
		AudioStreamBasicDescription *testSaveAudioDescriptionForStream(AudioStreamID streamID);
};

//-------------------------------------------------------------------------------------------

AudioStreamBasicDescription *AOCoreAudioSaveAudioDescriptionForStreamTest::testSaveAudioDescriptionForStream(AudioStreamID streamID)
{
	return saveAudioDescriptionForStream(streamID);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,saveAudioDescriptionForStreamFailToGetStream)
{
	AudioStreamID streamID = static_cast<AudioStreamID>(400);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamBasicDescription)),A<void *>()))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));

	AOCoreAudioSaveAudioDescriptionForStreamTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("saveAudioDescriptionForStream"),StrEq("Error getting current audio stream description"),kAudioHardwareNotRunningError)).Times(1);
	
	AudioStreamBasicDescription *saveFormat = audio.testSaveAudioDescriptionForStream(streamID);
	
	ASSERT_TRUE(saveFormat==0);

	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,saveAudioDescriptionForStreamSuccess)
{
	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = 4;
	format.mFramesPerPacket = 1;
	format.mBytesPerFrame = 4;
	format.mChannelsPerFrame = 2;
	format.mBitsPerChannel = 16;
	format.mReserved = 0;

	AudioStreamID streamID = static_cast<AudioStreamID>(400);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(streamID,PropertySetPhysicalFormats(),0,0,PropertyHasSize(sizeof(AudioStreamBasicDescription)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetAudioStreamBasicDescriptionForArguement(format),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioSaveAudioDescriptionForStreamTest audio;
	AudioStreamBasicDescription *saveFormat = audio.testSaveAudioDescriptionForStream(streamID);
	
	ASSERT_TRUE(saveFormat!=0);
	EXPECT_EQ(0,memcmp(saveFormat,&format,sizeof(AudioStreamBasicDescription)));
	delete saveFormat;

	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioSetSampleRateIfPossibleTest : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
		tint testSetSampleRateIfPossible(AudioDeviceID devID,int sampleRate);
};

//-------------------------------------------------------------------------------------------

tint AOCoreAudioSetSampleRateIfPossibleTest::testSetSampleRateIfPossible(AudioDeviceID devID,int sampleRate)
{
	return setSampleRateIfPossible(devID,sampleRate);
}

//-------------------------------------------------------------------------------------------

MATCHER(PropertySetSampleRate, "") { return (arg->mSelector==kAudioDevicePropertyNominalSampleRate && arg->mScope==kAudioDevicePropertyScopeOutput && arg->mElement==kAudioObjectPropertyElementMain); }
ACTION_P(SetSampleRateForArguement,value) { *static_cast<Float64*>(arg5) = value; }

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setSampleRateIfPossibleAndDeviceHasNoRateProperty)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertySetSampleRate())).Times(1).WillOnce(Return(false));

	AOCoreAudioSetSampleRateIfPossibleTest audio;
	
	EXPECT_EQ(-1,audio.testSetSampleRateIfPossible(devID,192000));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setSampleRateIfPossibleAndFailToDetermineIsSettable)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertySetSampleRate())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertySetSampleRate(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));

	AOCoreAudioSetSampleRateIfPossibleTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("setSampleRateIfPossible"),StrEq("Error getting if sample rate is settable for device"),kAudioHardwareNotRunningError)).Times(1);
	
	EXPECT_EQ(-1,audio.testSetSampleRateIfPossible(devID,192000));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setSampleRateIfPossibleAndRateCannotBeSet)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertySetSampleRate())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertySetSampleRate(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(false),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioSetSampleRateIfPossibleTest audio;
	
	EXPECT_EQ(-1,audio.testSetSampleRateIfPossible(devID,192000));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setSampleRateIfPossibleAndFailToGetCurrentSampleRate)
{
	Float64 expectedOldRate = 44100.0;

	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertySetSampleRate())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertySetSampleRate(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertySetSampleRate(),0,0,PropertyHasSize(sizeof(sizeof(Float64))),A<void *>()))
		.Times(1).WillOnce(DoAll(SetSampleRateForArguement(expectedOldRate),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));

	AOCoreAudioSetSampleRateIfPossibleTest audio;
	EXPECT_CALL(audio,printErrorOS(StrEq("setSampleRateIfPossible"),StrEq("Failed to get sample rate from device"),kAudioHardwareNotRunningError)).Times(1);
	
	EXPECT_EQ(-1,audio.testSetSampleRateIfPossible(devID,192000));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setSampleRateIfPossibleAndFailToSetNewSampleRate)
{
	Float64 expectedOldRate = 44100.0;

	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertySetSampleRate())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertySetSampleRate(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertySetSampleRate(),0,0,PropertyHasSize(sizeof(sizeof(Float64))),A<void *>()))
		.Times(1).WillOnce(DoAll(SetSampleRateForArguement(expectedOldRate),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(devID,PropertySetSampleRate(),0,0,Eq(sizeof(Float64)),PropertyHasSampleRate(192000)))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(kAudioHardwareNotRunningError)));

	AOCoreAudioSetSampleRateIfPossibleTest audio;
    EXPECT_CALL(audio,printErrorOS(StrEq("setSampleRateIfPossible"),StrEq("Failed to set sample rate to 192000"),kAudioHardwareNotRunningError)).Times(1);
	
	EXPECT_EQ(44100,audio.testSetSampleRateIfPossible(devID,192000));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setSampleRateIfPossibleAndSuccessInSettingNewSampleRate)
{
	Float64 expectedOldRate = 44100.0;

	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertySetSampleRate())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertySetSampleRate(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertySetSampleRate(),0,0,PropertyHasSize(sizeof(sizeof(Float64))),A<void *>()))
		.Times(1).WillOnce(DoAll(SetSampleRateForArguement(expectedOldRate),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectSetPropertyData(devID,PropertySetSampleRate(),0,0,Eq(sizeof(Float64)),PropertyHasSampleRate(192000)))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));

	AOCoreAudioSetSampleRateIfPossibleTest audio;
	
	EXPECT_EQ(44100,audio.testSetSampleRateIfPossible(devID,192000));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,setSampleRateIfPossibleAndDACAndRequiredAreTheSameThenNoFrequencyIsReturned)
{
	Float64 expectedOldRate = 44100.0;

	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertySetSampleRate())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertySetSampleRate(),A<Boolean *>()))
		.Times(1).WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertySetSampleRate(),0,0,PropertyHasSize(sizeof(sizeof(Float64))),A<void *>()))
		.Times(1).WillOnce(DoAll(SetSampleRateForArguement(expectedOldRate),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioSetSampleRateIfPossibleTest audio;
	
	EXPECT_EQ(-1,audio.testSetSampleRateIfPossible(devID,44100));
	
	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioSetAndResetDACFrequencyTest : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD0(getFrequency,tint());
		MOCK_METHOD2(setSampleRateIfPossible,tint(AudioDeviceID devID,int sampleRate));
		
		virtual tint getOriginalSampleRate();
		virtual void setSampleRateWhileOpeningCoreDevice(AudioDeviceID devID);
		virtual void resetSampleRateToOriginalWhenClosing(AudioDeviceID devID);
};

//-------------------------------------------------------------------------------------------

tint AOCoreAudioSetAndResetDACFrequencyTest::getOriginalSampleRate()
{
	return m_originalSampleRate;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioSetAndResetDACFrequencyTest::setSampleRateWhileOpeningCoreDevice(AudioDeviceID devID)
{
	AOCoreAudioMacOS::setSampleRateWhileOpeningCoreDevice(devID);
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioSetAndResetDACFrequencyTest::resetSampleRateToOriginalWhenClosing(AudioDeviceID devID)
{
	AOCoreAudioMacOS::resetSampleRateToOriginalWhenClosing(devID);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,audioIsClosedAndDACFrequencyHasNotBeenSetupToBeReset)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	AOCoreAudioSetAndResetDACFrequencyTest audio;
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
	
	audio.resetSampleRateToOriginalWhenClosing(devID);
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,audioIsOpenedViaAudioUnitsWithAudioAndDACHavingTheSameFrequncySetAndThenClosed)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	AOCoreAudioSetAndResetDACFrequencyTest audio;
	ASSERT_EQ(-1,audio.getOriginalSampleRate());

	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,44100)).Times(1).WillOnce(Return(-1));
	
	audio.setSampleRateWhileOpeningCoreDevice(devID);
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
	
	audio.resetSampleRateToOriginalWhenClosing(devID);
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,audioIsOpenedViaAudioUnitsWithAudioAndDACHavingDifferentFrequnciesSetAndThenClosedButFailedToSetDACFrequencyOnOpening)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	AOCoreAudioSetAndResetDACFrequencyTest audio;
	ASSERT_EQ(-1,audio.getOriginalSampleRate());

	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(192000));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,192000)).Times(1).WillOnce(Return(-1));
	
	audio.setSampleRateWhileOpeningCoreDevice(devID);
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
	
	audio.resetSampleRateToOriginalWhenClosing(devID);
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,audioIsOpenedViaAudioUnitsWithAudioAndDACHavingDifferentFrequnciesSetAndThenClosedAndSucceededToSetDACFrequencyOnOpeningButFailsToResetOnClosing)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	AOCoreAudioSetAndResetDACFrequencyTest audio;
	ASSERT_EQ(-1,audio.getOriginalSampleRate());

	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(192000));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,192000)).Times(1).WillOnce(Return(44100));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,44100)).Times(1).WillOnce(Return(-1));
	
	audio.setSampleRateWhileOpeningCoreDevice(devID);
	ASSERT_EQ(44100,audio.getOriginalSampleRate());
	
	audio.resetSampleRateToOriginalWhenClosing(devID);
	ASSERT_EQ(44100,audio.getOriginalSampleRate());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,audioIsOpenedViaAudioUnitsWithAudioAndDACHavingDifferentFrequnciesSetAndThenClosedAndSucceededToSetDACFrequencyOnOpeningAndClosing)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	AOCoreAudioSetAndResetDACFrequencyTest audio;
	ASSERT_EQ(-1,audio.getOriginalSampleRate());

	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(192000));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,192000)).Times(1).WillOnce(Return(44100));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,44100)).Times(1).WillOnce(Return(192000));
	
	audio.setSampleRateWhileOpeningCoreDevice(devID);
	ASSERT_EQ(44100,audio.getOriginalSampleRate());
	
	audio.resetSampleRateToOriginalWhenClosing(devID);
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,audioOpenAndDACFrequencyIsChangedThenReOpenedAtAnotherFrequencyBeforeClosingSuccessfully)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	AOCoreAudioSetAndResetDACFrequencyTest audio;
	ASSERT_EQ(-1,audio.getOriginalSampleRate());

	EXPECT_CALL(audio,getFrequency()).Times(2).WillOnce(Return(192000)).WillOnce(Return(48000));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,192000)).Times(1).WillOnce(Return(44100));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,48000)).Times(1).WillOnce(Return(192000));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,44100)).Times(1).WillOnce(Return(48000));
	
	audio.setSampleRateWhileOpeningCoreDevice(devID);
	ASSERT_EQ(44100,audio.getOriginalSampleRate());
	
	audio.setSampleRateWhileOpeningCoreDevice(devID);
	ASSERT_EQ(44100,audio.getOriginalSampleRate());
	
	audio.resetSampleRateToOriginalWhenClosing(devID);
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,audioOpenAndDACFrequencyIsChangedThenReOpenedAtOriginalFrequencyBeforeClosing)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	AOCoreAudioSetAndResetDACFrequencyTest audio;
	ASSERT_EQ(-1,audio.getOriginalSampleRate());

	EXPECT_CALL(audio,getFrequency()).Times(2).WillOnce(Return(192000)).WillOnce(Return(44100));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,192000)).Times(1).WillOnce(Return(44100));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,44100)).Times(1).WillOnce(Return(192000));
	
	audio.setSampleRateWhileOpeningCoreDevice(devID);
	ASSERT_EQ(44100,audio.getOriginalSampleRate());
	
	audio.setSampleRateWhileOpeningCoreDevice(devID);
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
	
	audio.resetSampleRateToOriginalWhenClosing(devID);
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,audioOpenAndDACFrequencyIsChangedThenReOpenedAtOriginalFrequencyButFailsBeforeClosingSuccessfully)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	AOCoreAudioSetAndResetDACFrequencyTest audio;
	ASSERT_EQ(-1,audio.getOriginalSampleRate());

	EXPECT_CALL(audio,getFrequency()).Times(2).WillOnce(Return(192000)).WillOnce(Return(44100));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,192000)).Times(1).WillOnce(Return(44100));
	EXPECT_CALL(audio,setSampleRateIfPossible(devID,44100)).Times(2).WillOnce(Return(-1)).WillOnce(Return(192000));
	
	audio.setSampleRateWhileOpeningCoreDevice(devID);
	ASSERT_EQ(44100,audio.getOriginalSampleRate());
	
	audio.setSampleRateWhileOpeningCoreDevice(devID);
	ASSERT_EQ(44100,audio.getOriginalSampleRate());
	
	audio.resetSampleRateToOriginalWhenClosing(devID);
	ASSERT_EQ(-1,audio.getOriginalSampleRate());
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioReleaseExclusiveModeTest : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD0(getCurrentProcessID,pid_t());
		void releaseExclusiveMode(AudioDeviceID devID);
};

//-------------------------------------------------------------------------------------------

void AOCoreAudioReleaseExclusiveModeTest::releaseExclusiveMode(AudioDeviceID devID)
{
	AOCoreAudioMacOS::releaseExclusiveMode(devID);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,releaseExclusiveModeWhenHogNotAvailable)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(false));

	AOCoreAudioReleaseExclusiveModeTest audio;

	audio.releaseExclusiveMode(devID);

	CoreAudioIF::release();	
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,releaseExclusiveModeErrorGettingAIfCanBeSet)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));

	AOCoreAudioReleaseExclusiveModeTest audio;

	audio.releaseExclusiveMode(devID);

	CoreAudioIF::release();	
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,releaseExclusiveModeHogModeCannotBeSet)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(false),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioReleaseExclusiveModeTest audio;

	audio.releaseExclusiveMode(devID);

	CoreAudioIF::release();	
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,releaseExclusiveModeFailedToGetProperty)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	pid_t aProcessID = static_cast<pid_t>(20);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
    EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsHogMode(),0,0,PropertyHasSize(sizeof(pid_t)),A<void *>())).Times(1)
    	.WillOnce(DoAll(SetProcessIDForArguement(aProcessID),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));

	AOCoreAudioReleaseExclusiveModeTest audio;

	audio.releaseExclusiveMode(devID);

	CoreAudioIF::release();	
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,releaseExclusiveModeWhenProcessDoesNotHaveExclusiveAccess)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	pid_t aProcessID = static_cast<pid_t>(10);
	pid_t bProcessID = static_cast<pid_t>(20);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
    EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsHogMode(),0,0,PropertyHasSize(sizeof(pid_t)),A<void *>())).Times(1)
    	.WillOnce(DoAll(SetProcessIDForArguement(aProcessID),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioReleaseExclusiveModeTest audio;
	EXPECT_CALL(audio,getCurrentProcessID()).WillRepeatedly(Return(bProcessID));

	audio.releaseExclusiveMode(devID);

	CoreAudioIF::release();	
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,releaseExclusiveModeExclusiveModeIsReleased)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);

	pid_t aProcessID = static_cast<pid_t>(20);
	pid_t bProcessID = static_cast<pid_t>(20);
	
	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	

	EXPECT_CALL(apiMock,AudioObjectHasProperty(devID,PropertyIsHogMode())).Times(1).WillOnce(Return(true));
	EXPECT_CALL(apiMock,AudioObjectIsPropertySettable(devID,PropertyIsHogMode(),A<Boolean *>())).Times(1)
		.WillOnce(DoAll(SetArgPointee<2>(true),Return(static_cast<OSStatus>(noErr))));
    EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsHogMode(),0,0,PropertyHasSize(sizeof(pid_t)),A<void *>())).Times(1)
    	.WillOnce(DoAll(SetProcessIDForArguement(aProcessID),Return(static_cast<OSStatus>(noErr))));
    EXPECT_CALL(apiMock,AudioObjectSetPropertyData(devID,PropertyIsHogMode(),0,0,Eq(sizeof(pid_t)),PropertyHasProcessID(-1)))
		.Times(1).WillOnce(Return(static_cast<OSStatus>(noErr)));

	AOCoreAudioReleaseExclusiveModeTest audio;
	EXPECT_CALL(audio,getCurrentProcessID()).WillRepeatedly(Return(bProcessID));

	audio.releaseExclusiveMode(devID);

	CoreAudioIF::release();	
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioCopyDeviceInformationTest : public AOCoreAudioMacOS
{
	public:
		virtual QSharedPointer<AOQueryDevice::Device> copyDeviceInformation(const AOQueryDevice::Device& iDevice);
};

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryDevice::Device> AOCoreAudioCopyDeviceInformationTest::copyDeviceInformation(const AOQueryDevice::Device& iDevice)
{
	return AOCoreAudioMacOS::copyDeviceInformation(iDevice);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,copyDeviceInformation)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	AOQueryCoreAudio::DeviceCoreAudio iDevice;
	iDevice.setInitialized();
	iDevice.id() = "Device";
	iDevice.name() = "SoundBlaster";
	iDevice.addFrequency(44100);
	iDevice.addFrequency(48000);
	iDevice.setNoChannels(4);
	iDevice.setDeviceID(devID);
	
	AOCoreAudioCopyDeviceInformationTest audio;
	
	QSharedPointer<AOQueryDevice::Device> oDevice = audio.copyDeviceInformation(iDevice);
	ASSERT_FALSE(oDevice.isNull());
	
	QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice = oDevice.dynamicCast<AOQueryCoreAudio::DeviceCoreAudio>();
	ASSERT_FALSE(pDevice.isNull());
	
	EXPECT_TRUE(pDevice->isInitialized());
	EXPECT_TRUE(pDevice->id()=="Device");
	EXPECT_TRUE(pDevice->name()=="SoundBlaster");
	EXPECT_TRUE(pDevice->isFrequencySupported(44100));
	EXPECT_TRUE(pDevice->isFrequencySupported(48000));
	EXPECT_FALSE(pDevice->isFrequencySupported(192000));
	ASSERT_EQ(4,pDevice->noChannels());
	EXPECT_EQ(devID,pDevice->deviceID());
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioIsConvertionAlignedHigh : public AOCoreAudioMacOS
{
	public:
		virtual bool isConvertionAlignedHigh(const AudioStreamBasicDescription& format) const;
};

//-------------------------------------------------------------------------------------------

bool AOCoreAudioIsConvertionAlignedHigh::isConvertionAlignedHigh(const AudioStreamBasicDescription& format) const
{
	return AOCoreAudioMacOS::isConvertionAlignedHigh(format);
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor16BitSampleInLittleEndian24BitsAndHighFlagNotSet)
{
	const tint c_bitsPerSample = 16;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 3;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor16BitSampleInLittleEndian24BitsAndHighFlagIsSet)
{
	const tint c_bitsPerSample = 16;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 3;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor16BitSampleInBigEndian24BitsAndHighFlagNotSet)
{
	const tint c_bitsPerSample = 16;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 3;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor16BitSampleInBigEndian24BitsAndHighFlagIsSet)
{
	const tint c_bitsPerSample = 16;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 3;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor16BitSampleInLittleEndian32BitsAndHighFlagNotSet)
{
	const tint c_bitsPerSample = 16;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor16BitSampleInLittleEndian32BitsAndHighFlagIsSet)
{
	const tint c_bitsPerSample = 16;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor16BitSampleInBigEndian32BitsAndHighFlagNotSet)
{
	const tint c_bitsPerSample = 16;
	const tint c_noChannels = 8;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor16BitSampleInBigEndian32BitsAndHighFlagIsSet)
{
	const tint c_bitsPerSample = 16;
	const tint c_noChannels = 4;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor24BitSampleInLittleEndian32BitsAndHighFlagNotSet)
{
	const tint c_bitsPerSample = 24;
	const tint c_noChannels = 4;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor24BitSampleInLittleEndian32BitsAndHighFlagIsSet)
{
	const tint c_bitsPerSample = 24;
	const tint c_noChannels = 4;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor24BitSampleInBigEndian32BitsAndHighFlagNotSet)
{
	const tint c_bitsPerSample = 24;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor24BitSampleInBigEndian32BitsAndHighFlagIsSet)
{
	const tint c_bitsPerSample = 24;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_TRUE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor32BitSampleInLittleEndian32BitsAndHighFlagNotSet)
{
	const tint c_bitsPerSample = 32;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor32BitSampleInLittleEndian32BitsAndHighFlagIsSet)
{
	const tint c_bitsPerSample = 32;
	const tint c_noChannels = 4;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger |  kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor32BitSampleInBigEndian32BitsAndHighFlagNotSet)
{
	const tint c_bitsPerSample = 32;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isConvertionAlignedHighFor32BitSampleInBigEndian32BitsAndHighFlagIsSet)
{
	const tint c_bitsPerSample = 32;
	const tint c_noChannels = 2;
	const tint c_noFramesPerPacket = 1;
	const tint c_bytesPerSample = 4;

	AudioStreamBasicDescription format;
	format.mSampleRate = 44100.0;
	format.mFormatID = kAudioFormatLinearPCM;
    format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsAlignedHigh;
	format.mBytesPerPacket = c_bytesPerSample * c_noChannels;
	format.mFramesPerPacket = c_noFramesPerPacket;
	format.mBytesPerFrame = c_bytesPerSample * c_noChannels * c_noFramesPerPacket;
	format.mChannelsPerFrame = c_noChannels;
	format.mBitsPerChannel = c_bitsPerSample;
	format.mReserved = 0;

	AOCoreAudioIsConvertionAlignedHigh audio;
	
	EXPECT_FALSE(audio.isConvertionAlignedHigh(format));
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioIsDeviceAliveTest : public AOCoreAudioMacOS
{
	public:
		MOCK_CONST_METHOD1(printToLog,void(const tchar *msg));
		MOCK_CONST_METHOD3(printErrorOS,void(const tchar *strR,const tchar *strE,OSStatus err));
		bool testIsDeviceAlive(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pDevice);
};

//-------------------------------------------------------------------------------------------

bool AOCoreAudioIsDeviceAliveTest::testIsDeviceAlive(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pDevice)
{
	return isDeviceAlive(pDevice);
}

//-------------------------------------------------------------------------------------------

MATCHER(PropertyIsDeviceAlive, "") { return (arg->mSelector==kAudioDevicePropertyDeviceIsAlive && arg->mScope==kAudioObjectPropertyScopeGlobal && arg->mElement==kAudioObjectPropertyElementMain); }
ACTION_P(SetIsDeviceAlive,value) { *static_cast<bool*>(arg5) = value; }

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isDeviceAliveGivenNoOSErrorAndDeviceIsAlive)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice(new AOQueryCoreAudio::DeviceCoreAudio());
	pDevice->setInitialized();
	pDevice->id() = "Device";
	pDevice->name() = "SoundBlaster";
	pDevice->setDeviceID(devID);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsDeviceAlive(),0,0,PropertyHasSize(sizeof(bool)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetIsDeviceAlive(true),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioIsDeviceAliveTest audio;
	
	EXPECT_TRUE(audio.testIsDeviceAlive(pDevice));

	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isDeviceAliveGivenNoOSErrorAndDeviceIsNotAlive)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice(new AOQueryCoreAudio::DeviceCoreAudio());
	pDevice->setInitialized();
	pDevice->id() = "Device";
	pDevice->name() = "SoundBlaster";
	pDevice->setDeviceID(devID);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsDeviceAlive(),0,0,PropertyHasSize(sizeof(bool)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetIsDeviceAlive(false),Return(static_cast<OSStatus>(noErr))));

	AOCoreAudioIsDeviceAliveTest audio;
	EXPECT_CALL(audio,printToLog(StrEq("Audio device 'SoundBlaster' is no longer alive and available"))).Times(1);
	
	EXPECT_FALSE(audio.testIsDeviceAlive(pDevice));

	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreAudioMacOS,isDeviceAliveGivenOSErrorButDeviceIsAliveFlagIsSetToTrue)
{
	AudioDeviceID devID = static_cast<AudioDeviceID>(300);
	
	QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice(new AOQueryCoreAudio::DeviceCoreAudio());
	pDevice->setInitialized();
	pDevice->id() = "Device";
	pDevice->name() = "SoundBlaster";
	pDevice->setDeviceID(devID);

	CoreAudioIFSPtr pAPI = CoreAudioIF::instance("mock");
	CoreAudioMockIF& apiMock = dynamic_cast<CoreAudioMockIF&>(*(pAPI.data()));	
	EXPECT_CALL(apiMock,AudioObjectGetPropertyData(devID,PropertyIsDeviceAlive(),0,0,PropertyHasSize(sizeof(bool)),A<void *>()))
		.Times(1).WillOnce(DoAll(SetIsDeviceAlive(true),Return(static_cast<OSStatus>(kAudioHardwareNotRunningError))));

	AOCoreAudioIsDeviceAliveTest audio;
	
	EXPECT_TRUE(audio.testIsDeviceAlive(pDevice));

	CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------
