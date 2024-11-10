#include "audioio/test/AOBaseTest.h"
#include "audioio/test/AOQueryDeviceMock.h"

#include <QApplication>

//-------------------------------------------------------------------------------------------

class AOBaseTest : public AOBase
{
	public:
		MOCK_CONST_METHOD2(printError,void(const tchar *strR,const tchar *strE));
		MOCK_METHOD0(openAudio,bool());
		MOCK_METHOD0(closeAudio,void());
		MOCK_METHOD0(startAudioDevice,bool());
		MOCK_METHOD0(stopAudioDevice,void());
		MOCK_CONST_METHOD0(isAudio,bool());
		MOCK_METHOD0(processMessages,void());
		
		virtual QSharedPointer<AOQueryDevice::Device> copyDeviceInformation(const AOQueryDevice::Device& iDevice);
		
        virtual engine::AData *allocateData(tint len,tint inChannel,tint outChannel);
		
		virtual void writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,const engine::RData *data,tint partNumber,tint inChannelIndex,tint bufferIndex,tint outChannelIndex,tint inputSampleIndex,tint outputSampleIndex,tint amount);
		
		void testProcessCodec(bool initF);
		AudioItem *testProcessCodecLoop(AudioItem *item,bool& initF,bool& loop);
		bool testProcessCodecState(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF);
		bool testProcessCodecPlay(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF);
		
		bool testProcessCodecPlayDecode(AudioItem* item,const common::TimeStamp& currentT,bool& initF);
		bool testProcessCodecPlayDecodeInTime(AudioItem *item,const common::TimeStamp& currentT,bool& initF);
		void testProcessCodecPlayTagPartAsRequired(engine::RData *data);
		void testProcessCodecReadyForNext(AudioItem* item,bool completeFlag,tint iFrom);
		void testProcessCodecPlayNextEndInParts(engine::RData *data,bool completeFlag,tint iFrom);
		void testProcessCodecPlayNextOutStateZero(engine::RData::Part& part);
		void testProcessCodecPlayNextOutStateOne(engine::RData::Part& part,engine::RData *data);
		
		bool testProcessCodecPlayPostProcess(AudioItem **pItem,const common::TimeStamp& currentT,bool completeFlag);
		bool testProcessCodecPlayPostProcessComplete(AudioItem **pItem,const common::TimeStamp& currentT);
		bool testProcessCodecPlayPostProcessCompleteRemote(AudioItem **pItem,const common::TimeStamp& currentT);
		bool testProcessCodecPlayPostProcessCompleteLocal(AudioItem **pItem);
		bool testProcessCodecPlayPostProcessRunning(AudioItem **pItem);
		void testProcessCodecPlayPostProcessRunningWithNext(AudioItem **pItem);
		void testProcessCodecPlayPostProcessRunningWithNoNext(AudioItem **pItem);
		bool testProcessCodecPlayPostProcessCheckBufferedState(AudioItem **pItem);

		bool testProcessCodecCrossFade(AudioItem* item,const common::TimeStamp& currentT,bool& initF);
		bool testProcessCodecPreBuffer();
		
		common::TimeStamp testTimeFromEndOfItemBeingPlayed(AudioItem *item);
		
        tint testDecodeAndResampleInterleaveOutputChannels(sample_t *out,tint dLen,tint rLen);
		bool testDecodeAndResampleSetCompletePartTiming(engine::AData& dData,engine::RData::Part& p,tint idx,tint dLen);
		tint testDecodeAndResampleCalculateOutputLength();

		bool testPausePlayback(bool shutdown ,bool signalFlag);
		
		bool testUnpausePlayback(bool signalFlag);
		bool testUnpausePlaybackCodecStateNoPlay();
		
		bool testUnpausePlaybackCodecStateSingle();
		bool testUnpausePlaybackCodecStateSingleTiming();
		bool testUnpausePlaybackCodecStateSingleSeekToPauseTime();
		bool testUnpausePlaybackCodecStateSingleBufferOnRemote(bool process);
		void testUnpausePlaybackCodecStateSinglePlayState(bool process);
		void testUnpausePlaybackCodecStateSinglePlayToCrossfadeState();
		void testUnpausePlaybackCodecStateSingleSetPlayState(bool process);
		
		bool testUnpausePlaybackCodecStateFinish();
		bool testUnpausePlaybackProcess(bool signalFlag);
		void testUnpausePlaybackProcessSetTimeAndState();
		void testUnpausePlaybackProcessOpenAudio();
		bool testUnpausePlaybackProcessRestartPlayback(bool signalFlag);
};

//-------------------------------------------------------------------------------------------

engine::AData *AOBaseTest::allocateData(tint len,tint inChannel,tint outChannel)
{
    engine::RData *d = new engine::RData(len,inChannel,outChannel);
	return reinterpret_cast<engine::AData *>(d);
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryDevice::Device> AOBaseTest::copyDeviceInformation(const AOQueryDevice::Device& iDevice)
{
	return QSharedPointer<AOQueryDevice::Device>();
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testProcessCodec(bool initF)
{
	return processCodec(initF);
}

//-------------------------------------------------------------------------------------------

AudioItem *AOBaseTest::testProcessCodecLoop(AudioItem *item,bool& initF,bool& loop)
{
	return processCodecLoop(item,initF,loop);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecState(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF)
{
	return processCodecState(pItem,currentT,initF);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecPlay(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF)
{
	return processCodecPlay(pItem,currentT,initF);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecCrossFade(AudioItem* item,const common::TimeStamp& currentT,bool& initF)
{
	return processCodecCrossFade(item,currentT,initF);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecPreBuffer()
{
	return processCodecPreBuffer();
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecPlayDecode(AudioItem* item,const common::TimeStamp& currentT,bool& initF)
{
	return processCodecPlayDecode(item,currentT,initF);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecPlayDecodeInTime(AudioItem *item,const common::TimeStamp& currentT,bool& initF)
{
	return processCodecPlayDecodeInTime(item,currentT,initF);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testProcessCodecPlayTagPartAsRequired(engine::RData *data)
{
	processCodecPlayTagPartAsRequired(data);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testProcessCodecReadyForNext(AudioItem* item,bool completeFlag,tint iFrom)
{
	processCodecReadyForNext(item,completeFlag,iFrom);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testProcessCodecPlayNextEndInParts(engine::RData *data,bool completeFlag,tint iFrom)
{
	processCodecPlayNextEndInParts(data,completeFlag,iFrom);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testProcessCodecPlayNextOutStateZero(engine::RData::Part& part)
{
	processCodecPlayNextOutStateZero(part);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testProcessCodecPlayNextOutStateOne(engine::RData::Part& part,engine::RData *data)
{
	processCodecPlayNextOutStateOne(part,data);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecPlayPostProcess(AudioItem **pItem,const common::TimeStamp& currentT,bool completeFlag)
{
	return processCodecPlayPostProcess(pItem,currentT,completeFlag);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecPlayPostProcessComplete(AudioItem **pItem,const common::TimeStamp& currentT)
{
	return processCodecPlayPostProcessComplete(pItem,currentT);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecPlayPostProcessCompleteRemote(AudioItem **pItem,const common::TimeStamp& currentT)
{
	return processCodecPlayPostProcessCompleteRemote(pItem,currentT);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecPlayPostProcessCompleteLocal(AudioItem **pItem)
{
	return processCodecPlayPostProcessCompleteLocal(pItem);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecPlayPostProcessRunning(AudioItem **pItem)
{
	return processCodecPlayPostProcessRunning(pItem);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testProcessCodecPlayPostProcessRunningWithNext(AudioItem **pItem)
{
	processCodecPlayPostProcessRunningWithNext(pItem);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testProcessCodecPlayPostProcessRunningWithNoNext(AudioItem **pItem)
{
	processCodecPlayPostProcessRunningWithNoNext(pItem);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testProcessCodecPlayPostProcessCheckBufferedState(AudioItem **pItem)
{
	return processCodecPlayPostProcessCheckBufferedState(pItem);
}

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBaseTest::testTimeFromEndOfItemBeingPlayed(AudioItem *item)
{
	return timeFromEndOfItemBeingPlayed(item);
}

//-------------------------------------------------------------------------------------------

tint AOBaseTest::testDecodeAndResampleInterleaveOutputChannels(sample_t *out,tint dLen,tint rLen)
{
    return decodeAndResampleInterleaveOutputChannels(out,dLen,rLen);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testDecodeAndResampleSetCompletePartTiming(engine::AData& dData,engine::RData::Part& p,tint idx,tint dLen)
{
	return decodeAndResampleSetCompletePartTiming(dData,p,idx,dLen);
}

//-------------------------------------------------------------------------------------------

tint AOBaseTest::testDecodeAndResampleCalculateOutputLength()
{
	return decodeAndResampleCalculateOutputLength();
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testPausePlayback(bool shutdown,bool signalFlag)
{
	return pausePlayback(shutdown,signalFlag);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testUnpausePlayback(bool signalFlag)
{
	return unpausePlayback(signalFlag);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testUnpausePlaybackCodecStateNoPlay()
{
	return unpausePlaybackCodecStateNoPlay();
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testUnpausePlaybackCodecStateSingle()
{
	return unpausePlaybackCodecStateSingle();
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testUnpausePlaybackCodecStateSingleTiming()
{
	return unpausePlaybackCodecStateSingleTiming();
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testUnpausePlaybackCodecStateSingleSeekToPauseTime()
{
	return unpausePlaybackCodecStateSingleSeekToPauseTime();
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testUnpausePlaybackCodecStateSingleBufferOnRemote(bool process)
{
	return unpausePlaybackCodecStateSingleBufferOnRemote(process);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testUnpausePlaybackCodecStateSinglePlayState(bool process)
{
	return unpausePlaybackCodecStateSinglePlayState(process);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testUnpausePlaybackCodecStateSinglePlayToCrossfadeState()
{
	unpausePlaybackCodecStateSinglePlayToCrossfadeState();
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testUnpausePlaybackCodecStateSingleSetPlayState(bool process)
{
	unpausePlaybackCodecStateSingleSetPlayState(process);
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testUnpausePlaybackCodecStateFinish()
{
	return unpausePlaybackCodecStateFinish();
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testUnpausePlaybackProcess(bool signalFlag)
{
	return unpausePlaybackProcess(signalFlag);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testUnpausePlaybackProcessSetTimeAndState()
{
	unpausePlaybackProcessSetTimeAndState();
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::testUnpausePlaybackProcessOpenAudio()
{
	unpausePlaybackProcessOpenAudio();
}

//-------------------------------------------------------------------------------------------

bool AOBaseTest::testUnpausePlaybackProcessRestartPlayback(bool signalFlag)
{
	return unpausePlaybackProcessRestartPlayback(signalFlag);
}

//-------------------------------------------------------------------------------------------

void AOBaseTest::writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,const engine::RData *data,tint partNumber,tint inChannelIndex,tint bufferIndex,tint outChannelIndex,tint inputSampleIndex,tint outputSampleIndex,tint amount)
{}

//-------------------------------------------------------------------------------------------

class AOBaseStartCodecTest : public AOBaseTest
{
	public:
		bool testStartCodec(const QString& url,const common::TimeStamp& t,const common::TimeStamp& tLen);
	
		MOCK_CONST_METHOD0(getState,AOBase::States());
		MOCK_METHOD1(setState,void(AOBase::States s));
		MOCK_CONST_METHOD0(getCodecState,AOBase::CodecState());
		MOCK_METHOD1(setCodecState,void(AOBase::CodecState s));
		MOCK_CONST_METHOD0(getStartCodecSeekTime,const common::TimeStamp&());
		MOCK_METHOD0(getCodec,engine::Codec *());
		MOCK_METHOD1(setCodec,void(engine::Codec *c));
		MOCK_METHOD0(getNoChannelsMapped,int())

		MOCK_CONST_METHOD1(createNewCodecFromUrl,engine::Codec *(const QString& url));
		MOCK_METHOD0(connectPreBufferedRemoteCodec,void());
		MOCK_METHOD0(startPlayTimeWithSingleCodec,void());
		MOCK_METHOD1(startInternalTimer,void(int period));
		MOCK_METHOD1(emitOnStart,void(const QString& url));

		MOCK_METHOD1(startAudio,bool(const QString& url));
		MOCK_METHOD1(stopCodec,void(bool eFlag));
		
		MOCK_METHOD1(setStartCodecSeekTime,void(const common::TimeStamp& t));
		MOCK_METHOD1(setCodecTimePositionComplete,void(const common::TimeStamp& t));
};

//-------------------------------------------------------------------------------------------

bool AOBaseStartCodecTest::testStartCodec(const QString& url,const common::TimeStamp& t,const common::TimeStamp& tLen)
{
	return startCodec(url,t,tLen);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startCodecWhereAudioFailedToStop)
{
	QString nullStr = QString();
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp tLen = 0.0;

	AOBaseStartCodecTest audio;
	
	EXPECT_CALL(audio,stopCodec(false)).Times(1);
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_statePlay));
	EXPECT_CALL(audio,printError(StrEq("startCodec"),StrEq("Audio state is not in stop state"))).Times(1);
	EXPECT_CALL(audio,emitOnStart(nullStr)).Times(1);
	EXPECT_CALL(audio,stopCodec(true)).Times(1);

	EXPECT_FALSE(audio.testStartCodec(fileName,startTime,tLen));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startCodecWherePreviousCodecFailedToStop)
{
	QString nullStr = QString();
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp tLen = 0.0;

	AOBaseStartCodecTest audio;
	
	EXPECT_CALL(audio,stopCodec(false)).Times(1);
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_stateStop));
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecSingle));
	EXPECT_CALL(audio,printError(StrEq("startCodec"),StrEq("Audio state is not in stop state"))).Times(1);
	EXPECT_CALL(audio,emitOnStart(nullStr)).Times(1);
	EXPECT_CALL(audio,stopCodec(true)).Times(1);

	EXPECT_FALSE(audio.testStartCodec(fileName,startTime,tLen));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startCodecWhereCodecFailsToBeCreated)
{
	QString nullStr = QString();
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp tLen = 0.0;
	
	AOBaseStartCodecTest audio;
	
	EXPECT_CALL(audio,stopCodec(false)).Times(1);
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_stateStop));
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecNoPlay));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return((engine::Codec *)0));
	EXPECT_CALL(audio,setCodec((engine::Codec *)0)).Times(1);
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return((engine::Codec *)0));
	EXPECT_CALL(audio,printError(StrEq("startCodec"),StrEq("Could not find compitable codec for '/pathto/music/track.flac'"))).Times(1);
	EXPECT_CALL(audio,emitOnStart(nullStr)).Times(1);
	EXPECT_CALL(audio,stopCodec(true)).Times(1);

	EXPECT_FALSE(audio.testStartCodec(fileName,startTime,tLen));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startCodecWithRemoteThatOpensSuccessfully)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp tLen = 0.0;

	AOBaseStartCodecTest audio;
	CodecMock codec;
	
	EXPECT_CALL(audio,stopCodec(false)).Times(1);
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_stateStop));
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecNoPlay));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,setStartCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setCodecTimePositionComplete(tLen)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,startPlayTimeWithSingleCodec()).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,connectPreBufferedRemoteCodec()).Times(1);
	EXPECT_CALL(audio,emitOnStart(fileName)).Times(1);

	EXPECT_TRUE(audio.testStartCodec(fileName,startTime,tLen));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startCodecWithLocalThatOpensButAudioFails)
{
	QString nullStr = QString();
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp tLen = 0.0;

	AOBaseStartCodecTest audio;
	CodecMock codec;
	
	EXPECT_CALL(audio,stopCodec(false)).Times(1);
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_stateStop));
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecNoPlay));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,setStartCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setCodecTimePositionComplete(tLen)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,startPlayTimeWithSingleCodec()).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,startAudio(fileName)).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,printError(StrEq("startCodec"),StrEq("Error initializing audio device"))).Times(1);
	EXPECT_CALL(audio,emitOnStart(nullStr)).Times(1);
	EXPECT_CALL(audio,stopCodec(true)).Times(1);

	EXPECT_FALSE(audio.testStartCodec(fileName,startTime,tLen));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startCodecWithLocalThatOpensSuccessfullyAndTrackLengthIsUndefined)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp tLen = 0.0;

	AOBaseStartCodecTest audio;
	CodecMock codec;
	
	EXPECT_CALL(audio,stopCodec(false)).Times(1);
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_stateStop));
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecNoPlay));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,setStartCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setCodecTimePositionComplete(tLen)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,startPlayTimeWithSingleCodec()).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,startAudio(fileName)).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,startInternalTimer(100)).Times(1);

	EXPECT_TRUE(audio.testStartCodec(fileName,startTime,tLen));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startCodecWithLocalThatOpensSuccessfullyAndTrackLengthIsDefined)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp tLen(3.0),tEnd;

	tEnd = startTime + tLen;

	AOBaseStartCodecTest audio;
	CodecMock codec;
	
	EXPECT_CALL(audio,stopCodec(false)).Times(1);
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_stateStop));
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecNoPlay));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,setStartCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setCodecTimePositionComplete(tEnd)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,startPlayTimeWithSingleCodec()).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,startAudio(fileName)).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,startInternalTimer(100)).Times(1);

	EXPECT_TRUE(audio.testStartCodec(fileName,startTime,tLen));
}

//-------------------------------------------------------------------------------------------

class AOBaseStartNextCodecTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getState,AOBase::States());
		MOCK_METHOD1(setState,void(AOBase::States s));
		MOCK_CONST_METHOD0(getCodecState,AOBase::CodecState());
		MOCK_METHOD1(setCodecState,void(AOBase::CodecState s));
		MOCK_METHOD0(stopNextCodec,void());
        MOCK_METHOD3(startCodec,bool(const QString& url,const common::TimeStamp& t,const common::TimeStamp& tLen));
		MOCK_CONST_METHOD0(getNextCodecSeekTime,const common::TimeStamp&());
		MOCK_METHOD1(setNextCodecSeekTime,void(const common::TimeStamp& t));
		MOCK_METHOD1(setNextCodecTimeLengthComplete,void(const common::TimeStamp& t));
		MOCK_METHOD0(getNextCodec,engine::Codec *());
		MOCK_METHOD1(setNextCodec,void(engine::Codec *c));
		MOCK_CONST_METHOD1(createNewCodecFromUrl,engine::Codec *(const QString& url));
		MOCK_METHOD0(getNoChannelsMapped,int());
		MOCK_METHOD2(connectPreBufferedNextRemoteCodec,void(const QString& url,bool fade));
		MOCK_CONST_METHOD0(getFrequency,tint());
		MOCK_CONST_METHOD0(getNoInChannels,tint());
		MOCK_METHOD0(calculateNextCodecCrossFadeTime,void());
		MOCK_METHOD1(setNextName,void(const QString& n));
		MOCK_METHOD0(calcNextCodecTime,void());
		MOCK_CONST_METHOD0(getNextOutState,tint());
		MOCK_METHOD1(setNextOutState,void(tint s));
		MOCK_METHOD0(emitOnNoNext,void());
		MOCK_METHOD1(setCodec,void(engine::Codec *c));
		MOCK_METHOD1(setStartCodecSeekTime,void(const common::TimeStamp& t));
		MOCK_METHOD1(setCodecTimePositionComplete,void(const common::TimeStamp& t));
		MOCK_METHOD0(getCodec,engine::Codec *());
		
		bool testStartNextCodec(const QString& url,const common::TimeStamp& nT,const common::TimeStamp& nTLen,bool fade);
};

//-------------------------------------------------------------------------------------------

bool AOBaseStartNextCodecTest::testStartNextCodec(const QString& url,const common::TimeStamp& nT,const common::TimeStamp& nTLen,bool fade)
{
	return startNextCodec(url,nT,nTLen,fade);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecNothingPlayingAndStartCodecSuccessful)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 3.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(1);
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecNoPlay));
	EXPECT_CALL(audio,startCodec(fileName,startTime,trackLen)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testStartNextCodec(fileName,startTime,trackLen,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecNothingPlayingAndStartCodecFails)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(1);
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecNoPlay));
	EXPECT_CALL(audio,startCodec(fileName,startTime,trackLen)).Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testStartNextCodec(fileName,startTime,trackLen,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecGivenNextCodecAlreadyExists)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(2);
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecSingleFinish));
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,printError(StrEq("startNextCodec"),StrEq("Next codec already defined"))).Times(1);
	EXPECT_CALL(audio,getNextOutState()).Times(1).WillOnce(Return(1));
	
	EXPECT_FALSE(audio.testStartNextCodec(fileName,startTime,trackLen,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecWhenFailureToCreateNextCodec)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(2);
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecSingleFinish));
	EXPECT_CALL(audio,getNextCodec()).Times(2).WillRepeatedly(Return((engine::Codec *)0));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return((engine::Codec *)0));
	EXPECT_CALL(audio,setNextCodec((engine::Codec *)0)).Times(1);
	EXPECT_CALL(audio,printError(StrEq("startNextCodec"),StrEq("Failed to open codec for '/pathto/music/track.flac'"))).Times(1);
	EXPECT_CALL(audio,getNextOutState()).Times(1).WillOnce(Return(1));
	
	EXPECT_FALSE(audio.testStartNextCodec(fileName,startTime,trackLen,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecWhenNextCodecIsRemote)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(1);
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecSingleFinish));
	EXPECT_CALL(audio,getNextCodec()).Times(4).WillOnce(Return((engine::Codec *)0)).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setNextCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,setNextCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(trackLen)).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,connectPreBufferedNextRemoteCodec(fileName,false)).Times(1);
	
	EXPECT_TRUE(audio.testStartNextCodec(fileName,startTime,trackLen,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecLocalNextCodecIsNotInitialized)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;
	
	AOBaseStartNextCodecTest audio;
	CodecMock codec;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(2);
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecSingleFinish));
	EXPECT_CALL(audio,getNextCodec()).Times(5).WillOnce(Return((engine::Codec *)0)).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setNextCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,setNextCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(trackLen)).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,init()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,printError(StrEq("startNextCodec"),StrEq("Failed to initialized codec for '/pathto/music/track.flac'"))).Times(1);
	EXPECT_CALL(audio,getNextOutState()).Times(1).WillOnce(Return(1));
	
	EXPECT_FALSE(audio.testStartNextCodec(fileName,startTime,trackLen,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecLocalSuccessWithSeek)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec,currentCodec;
	common::TimeStamp nextCodecSeekTime = 3.0;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(1);
	EXPECT_CALL(audio,getCodecState()).Times(2).WillRepeatedly(Return(AOBase::e_codecCurrentFinish));
    EXPECT_CALL(audio,getNextCodec()).WillOnce(Return((engine::Codec *)0)).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setNextCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,setNextCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(trackLen)).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,init()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,getNextCodecSeekTime()).Times(2).WillRepeatedly(ReturnRef(nextCodecSeekTime));
	EXPECT_CALL(codec,seek(nextCodecSeekTime)).Times(1);
	
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&currentCodec));
	EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(44100));
    EXPECT_CALL(currentCodec,frequency()).WillRepeatedly(Return(44100));
    EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
    EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codec,dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));
    EXPECT_CALL(currentCodec,dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));
	
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,setNextName(fileName)).Times(1);
	EXPECT_CALL(audio,setCodecState(AOBase::e_codecNextQueued)).Times(1);
	
	EXPECT_TRUE(audio.testStartNextCodec(fileName,startTime,trackLen,true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecLocalWithNextCodecNotSetToSameFrequency)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec,currentCodec;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(2);
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecSingleFinish));
	EXPECT_CALL(audio,getNextCodec()).WillOnce(Return((engine::Codec *)0)).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setNextCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,setNextCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(trackLen)).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,init()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(44100));	
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&currentCodec));
	EXPECT_CALL(currentCodec,frequency()).WillRepeatedly(Return(68000));
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
#endif
	EXPECT_CALL(audio,getNextOutState()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,setNextOutState(2)).Times(1);
	EXPECT_CALL(audio,emitOnNoNext()).Times(1);
	
	EXPECT_FALSE(audio.testStartNextCodec(fileName,startTime,trackLen,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecLocalWithNextCodecHavingDifferentNumberOfChannels)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec,currentCodec;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(2);
	EXPECT_CALL(audio,getCodecState()).Times(1).WillOnce(Return(AOBase::e_codecSingleFinish));
	EXPECT_CALL(audio,getNextCodec()).WillOnce(Return((engine::Codec *)0)).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setNextCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,setNextCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(trackLen)).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,init()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&currentCodec));
	EXPECT_CALL(currentCodec,frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(4));
	EXPECT_CALL(audio,getNextOutState()).Times(1).WillOnce(Return(1));
	
	EXPECT_FALSE(audio.testStartNextCodec(fileName,startTime,trackLen,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecSuccessWithFadeAndStatePlay)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec,currentCodec;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(1);
	EXPECT_CALL(audio,getNextCodec()).WillOnce(Return((engine::Codec *)0)).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setNextCodec(&codec)).Times(1);
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(trackLen)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,setNextCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,init()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(false));

	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&currentCodec));
	EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(44100));
    EXPECT_CALL(currentCodec,frequency()).WillRepeatedly(Return(44100));
    EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
    EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codec,dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));
    EXPECT_CALL(currentCodec,dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));

	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_statePlay));
	EXPECT_CALL(audio,calculateNextCodecCrossFadeTime()).Times(1);
	EXPECT_CALL(audio,setNextName(fileName)).Times(1);
	EXPECT_CALL(audio,getCodecState()).Times(2).WillRepeatedly(Return(AOBase::e_codecCurrentFinish));
	EXPECT_CALL(audio,setCodecState(AOBase::e_codecNextQueued)).Times(1);
	
	EXPECT_TRUE(audio.testStartNextCodec(fileName,startTime,trackLen,true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecSuccessWithFadeAndStateNotPlay)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec,currentCodec;
	
	EXPECT_CALL(audio,stopNextCodec()).Times(1);
	EXPECT_CALL(audio,getNextCodec()).WillOnce(Return((engine::Codec *)0)).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setNextCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(audio,setNextCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(trackLen)).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,init()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(false));
	
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&currentCodec));
	EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(44100));
    EXPECT_CALL(currentCodec,frequency()).WillRepeatedly(Return(44100));
    EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
    EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codec,dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));
    EXPECT_CALL(currentCodec,dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));

	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,setNextName(fileName)).Times(1);
	EXPECT_CALL(audio,getCodecState()).Times(2).WillRepeatedly(Return(AOBase::e_codecCurrentFinish));
	EXPECT_CALL(audio,setCodecState(AOBase::e_codecNextQueued)).Times(1);
	
	EXPECT_TRUE(audio.testStartNextCodec(fileName,startTime,trackLen,true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecSuccessWithCurrentCodecFinishedAndLengthOfNextCodecUndefined)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 0.0;

	AOBaseStartNextCodecTest audio;
	CodecMock codec,currentCodec;

	EXPECT_CALL(audio,stopNextCodec()).Times(1);
	EXPECT_CALL(audio,getNextCodec()).WillOnce(Return((engine::Codec *)0)).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setNextCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,setNextCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(trackLen)).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,init()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(false));
	
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&currentCodec));
	EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(44100));
    EXPECT_CALL(currentCodec,frequency()).WillRepeatedly(Return(44100));
    EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
    EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codec,dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));
    EXPECT_CALL(currentCodec,dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));

	EXPECT_CALL(audio,setNextName(fileName)).Times(1);
	EXPECT_CALL(audio,getCodecState()).Times(2).WillRepeatedly(Return(AOBase::e_codecSingleFinish));
	EXPECT_CALL(audio,setCodecState(AOBase::e_codecCurrentFinish)).Times(1);
	EXPECT_CALL(audio,setCodec(&codec)).Times(1);
	EXPECT_CALL(audio,setNextCodec((engine::Codec *)0)).Times(1);
	EXPECT_CALL(audio,setStartCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setCodecTimePositionComplete(trackLen)).Times(1);
	EXPECT_CALL(audio,calcNextCodecTime()).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_statePlay));
	
	EXPECT_TRUE(audio.testStartNextCodec(fileName,startTime,trackLen,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,startNextCodecSuccessWithCurrentCodecFinishedAndLengthOfNextCodecDefined)
{
	QString fileName = "/pathto/music/track.flac";
	common::TimeStamp startTime = 5.0;
	common::TimeStamp trackLen = 3.0;
	common::TimeStamp endPos;
	
	endPos = startTime + trackLen;

	AOBaseStartNextCodecTest audio;
	CodecMock codec,currentCodec;

	EXPECT_CALL(audio,stopNextCodec()).Times(1);
	EXPECT_CALL(audio,getNextCodec()).WillOnce(Return((engine::Codec *)0)).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,createNewCodecFromUrl(fileName)).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,setNextCodec(&codec)).Times(1);
	EXPECT_CALL(audio,getNoChannelsMapped()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(codec,setNoOutputChannels(2)).Times(1);
	EXPECT_CALL(audio,setNextCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(trackLen)).Times(1);
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,init()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(false));
	
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&currentCodec));
	EXPECT_CALL(codec,frequency()).WillRepeatedly(Return(44100));
    EXPECT_CALL(currentCodec,frequency()).WillRepeatedly(Return(44100));
    EXPECT_CALL(codec,noChannels()).WillRepeatedly(Return(2));
    EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codec,dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));
    EXPECT_CALL(currentCodec,dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));

	EXPECT_CALL(audio,setNextName(fileName)).Times(1);
	EXPECT_CALL(audio,getCodecState()).Times(2).WillRepeatedly(Return(AOBase::e_codecSingleFinish));
	EXPECT_CALL(audio,setCodecState(AOBase::e_codecCurrentFinish)).Times(1);
	EXPECT_CALL(audio,setCodec(&codec)).Times(1);
	EXPECT_CALL(audio,setNextCodec((engine::Codec *)0)).Times(1);
	EXPECT_CALL(audio,setStartCodecSeekTime(startTime)).Times(1);
	EXPECT_CALL(audio,setCodecTimePositionComplete(endPos)).Times(1);
	EXPECT_CALL(audio,calcNextCodecTime()).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_statePlay));
	
	EXPECT_TRUE(audio.testStartNextCodec(fileName,startTime,trackLen,false));
}

//-------------------------------------------------------------------------------------------

class AOBaseCalculateNextCodecCrossFadeTime : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getCrossFadeTime,const common::TimeStamp&());
		MOCK_METHOD1(setCrossFadeTime,void(const common::TimeStamp& t));
		MOCK_CONST_METHOD0(getCodecCurrentTime,const common::TimeStamp&());
		MOCK_CONST_METHOD0(getProgFadeTime,const common::TimeStamp&());
		MOCK_CONST_METHOD0(getCodecTimeLength,const common::TimeStamp&());
		MOCK_METHOD1(setCodecTimeLength,void(const common::TimeStamp& t));
		MOCK_METHOD1(setCodecTimeLengthUpdate,void(bool f));
		MOCK_METHOD1(setNextOutState,void(tint s));
		MOCK_CONST_METHOD0(getCodecTimePositionComplete,const common::TimeStamp&());

		void testCalculateNextCodecCrossFadeTime();
};

//-------------------------------------------------------------------------------------------

void AOBaseCalculateNextCodecCrossFadeTime::testCalculateNextCodecCrossFadeTime()
{
    calculateNextCodecCrossFadeTime();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calculateNextCodecCrossFadeTimeSoCodecTimeLengthIsSet)
{
	common::TimeStamp codecCurrentTime = 2.0;
	common::TimeStamp crossFadeTime;
	common::TimeStamp progFadeTime = 1.99;
	common::TimeStamp codecTimeLength = 6.0;
	
	AOBaseCalculateNextCodecCrossFadeTime audio;
	EXPECT_CALL(audio,getCodecCurrentTime()).Times(1).WillOnce(ReturnRef(codecCurrentTime));
	EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).Times(1)
		.WillOnce(SaveArg<0>(&crossFadeTime));
	EXPECT_CALL(audio,getCrossFadeTime()).Times(1).WillOnce(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,getProgFadeTime()).Times(1).WillOnce(ReturnRef(progFadeTime));
	EXPECT_CALL(audio,getCodecTimeLength()).Times(1).WillOnce(ReturnRef(codecTimeLength));
	EXPECT_CALL(audio,setCodecTimeLength(A<const common::TimeStamp&>())).Times(1)
		.WillOnce(SaveArg<0>(&codecTimeLength));
	EXPECT_CALL(audio,setCodecTimeLengthUpdate(false)).Times(1);
	EXPECT_CALL(audio,setNextOutState(1)).Times(1);
	
	audio.testCalculateNextCodecCrossFadeTime();
	
	EXPECT_NEAR(static_cast<tfloat64>(crossFadeTime),2.01,0.00001);
	EXPECT_NEAR(static_cast<tfloat64>(codecTimeLength),4.0,0.00001);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calculateNextCodecCrossFadeTimeSoCodecTimeLengthIsNotSet)
{
	common::TimeStamp codecCurrentTime = 2.0;
	common::TimeStamp crossFadeTime;
	common::TimeStamp progFadeTime = 1.99;
	common::TimeStamp codecTimeLength = 3.0;
	common::TimeStamp codecTimePosition = 0.0;
	
	AOBaseCalculateNextCodecCrossFadeTime audio;
	EXPECT_CALL(audio,getCodecCurrentTime()).Times(1).WillOnce(ReturnRef(codecCurrentTime));
	EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).Times(1)
		.WillOnce(SaveArg<0>(&crossFadeTime));
	EXPECT_CALL(audio,getCrossFadeTime()).Times(1).WillOnce(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,getProgFadeTime()).Times(1).WillOnce(ReturnRef(progFadeTime));
	EXPECT_CALL(audio,getCodecTimeLength()).Times(1).WillOnce(ReturnRef(codecTimeLength));
	EXPECT_CALL(audio,getCodecTimePositionComplete()).Times(1).WillOnce(ReturnRef(codecTimePosition));
	EXPECT_CALL(audio,setCodecTimeLengthUpdate(false)).Times(1);
	EXPECT_CALL(audio,setNextOutState(1)).Times(1);
	
	audio.testCalculateNextCodecCrossFadeTime();
	
	EXPECT_NEAR(static_cast<tfloat64>(crossFadeTime),2.01,0.00001);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calculateNextCodecCrossFadeTimeSoCodecTimeLengthIsSetAsCodecEndPositionAfter)
{
	common::TimeStamp codecCurrentTime = 2.0;
	common::TimeStamp crossFadeTime;
	common::TimeStamp progFadeTime = 1.99;
	common::TimeStamp codecTimeLength = 3.0;
	common::TimeStamp codecTimePosition = 5.0;
	
	AOBaseCalculateNextCodecCrossFadeTime audio;
	EXPECT_CALL(audio,getCodecCurrentTime()).Times(1).WillOnce(ReturnRef(codecCurrentTime));
	EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).Times(1)
		.WillOnce(SaveArg<0>(&crossFadeTime));
	EXPECT_CALL(audio,getCrossFadeTime()).Times(1).WillOnce(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,getProgFadeTime()).Times(1).WillOnce(ReturnRef(progFadeTime));
    EXPECT_CALL(audio,getCodecTimeLength()).Times(1).WillOnce(ReturnRef(codecTimeLength));
    EXPECT_CALL(audio,getCodecTimePositionComplete()).Times(2).WillRepeatedly(ReturnRef(codecTimePosition));
	EXPECT_CALL(audio,setCodecTimeLength(A<const common::TimeStamp&>())).Times(1)
		.WillOnce(SaveArg<0>(&codecTimeLength));
	EXPECT_CALL(audio,setCodecTimeLengthUpdate(false)).Times(1);
	EXPECT_CALL(audio,setNextOutState(1)).Times(1);
	
	audio.testCalculateNextCodecCrossFadeTime();
	
	EXPECT_NEAR(static_cast<tfloat64>(crossFadeTime),2.01,0.00001);
	EXPECT_NEAR(static_cast<tfloat64>(codecTimeLength),4.0,0.00001);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getCodec,engine::Codec *());
		MOCK_METHOD0(getCodecAudioItem,AudioItem *());
		MOCK_METHOD3(processCodecLoop,AudioItem *(AudioItem* item,bool& initF,bool& loop));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecGivenNoCodec)
{
	CodecMock codec;
	
	AOBaseProcessCodecTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return((engine::Codec *)0));
	
	audio.testProcessCodec(false);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecGivenNoCodecAudioItem)
{
	CodecMock codec;
	
	AOBaseProcessCodecTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCodecAudioItem()).Times(1).WillOnce(Return((AudioItem *)0));
	
	audio.testProcessCodec(false);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecGivenAudioItemFullState)
{
	CodecMock codec;
	
	AudioItemMock itemA;
	EXPECT_CALL(itemA,state()).WillRepeatedly(Return(AudioItem::e_stateFull));
	
	AOBaseProcessCodecTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCodecAudioItem()).Times(1).WillOnce(Return(&itemA));
	
	audio.testProcessCodec(false);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecGivenAudioItemFullEndState)
{
	CodecMock codec;
	
	AudioItemMock itemA;
	EXPECT_CALL(itemA,state()).WillRepeatedly(Return(AudioItem::e_stateFullEnd));
	
	AOBaseProcessCodecTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCodecAudioItem()).Times(1).WillOnce(Return(&itemA));
	
	audio.testProcessCodec(false);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecGivenAudioItemCallbackState)
{
	CodecMock codec;
	
	AudioItemMock itemA;
	EXPECT_CALL(itemA,state()).WillRepeatedly(Return(AudioItem::e_stateCallback));
	
	AOBaseProcessCodecTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCodecAudioItem()).Times(1).WillOnce(Return(&itemA));
	
	audio.testProcessCodec(false);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecGivenAudioItemCallbackEndState)
{
	CodecMock codec;
	
	AudioItemMock itemA;
	EXPECT_CALL(itemA,state()).WillRepeatedly(Return(AudioItem::e_stateCallbackEnd));
	
	AOBaseProcessCodecTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCodecAudioItem()).Times(1).WillOnce(Return(&itemA));
	
	audio.testProcessCodec(false);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecGivenAudioItemEmptyStateInitFlagSetWithNoOtherItemsToProcess)
{
	CodecMock codec;
	
	AudioItemMock itemA;
	EXPECT_CALL(itemA,state()).Times(3).WillOnce(Return(AudioItem::e_stateEmpty)).WillRepeatedly(Return(AudioItem::e_stateDone));
	
	AOBaseProcessCodecTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCodecAudioItem()).Times(1).WillOnce(Return(&itemA));
	EXPECT_CALL(audio,processCodecLoop(&itemA,A<bool&>(),A<bool&>())).Times(1).WillOnce(Return(&itemA));
	
	audio.testProcessCodec(true);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecGivenTwoEmptyOneCodecAndDoneStates)
{
	CodecMock codec;
	
	AudioItemMock itemA;
	EXPECT_CALL(itemA,state()).WillRepeatedly(Return(AudioItem::e_stateEmpty));
	AudioItemMock itemB;
	EXPECT_CALL(itemB,state()).WillRepeatedly(Return(AudioItem::e_stateEmpty));
	AudioItemMock itemC;
	EXPECT_CALL(itemC,state()).WillRepeatedly(Return(AudioItem::e_stateCodec));
	AudioItemMock itemD;
	EXPECT_CALL(itemD,state()).WillRepeatedly(Return(AudioItem::e_stateDone));
	
	AOBaseProcessCodecTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCodecAudioItem()).Times(1).WillOnce(Return(&itemA));
	EXPECT_CALL(audio,processCodecLoop(&itemA,A<bool&>(),A<bool&>())).Times(1).WillOnce(Return(&itemB));
	EXPECT_CALL(audio,processCodecLoop(&itemB,A<bool&>(),A<bool&>())).Times(1).WillOnce(Return(&itemC));
	EXPECT_CALL(audio,processCodecLoop(&itemC,A<bool&>(),A<bool&>())).Times(1).WillOnce(Return(&itemD));
	
	audio.testProcessCodec(false);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecGivenOnSecondLoopIsSetToFalse)
{
	CodecMock codec;
	
	AudioItemMock itemA;
	EXPECT_CALL(itemA,state()).WillRepeatedly(Return(AudioItem::e_stateEmpty));
	AudioItemMock itemB;
	EXPECT_CALL(itemB,state()).WillRepeatedly(Return(AudioItem::e_stateEmpty));
	AudioItemMock itemC;
	EXPECT_CALL(itemC,state()).WillRepeatedly(Return(AudioItem::e_stateCodec));
	
	AOBaseProcessCodecTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCodecAudioItem()).Times(1).WillOnce(Return(&itemA));
	EXPECT_CALL(audio,processCodecLoop(&itemA,A<bool&>(),A<bool&>())).Times(1).WillOnce(Return(&itemB));
	EXPECT_CALL(audio,processCodecLoop(&itemB,A<bool&>(),A<bool&>())).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(&itemC)));
	
	audio.testProcessCodec(false);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecLoopTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(currentCallbackTime,common::TimeStamp());
		MOCK_METHOD0(getAudioProcessType,tint());
		MOCK_METHOD3(processCodecState,bool(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF));
		MOCK_METHOD1(setCodecAudioItem,void(AudioItem *item));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecLoopDataResetAudioProcessZero)
{
	bool loop = true;
	bool initF = false;
	common::TimeStamp currentTime(1.0);
	
	RDataMock cData;
	EXPECT_CALL(cData,reset()).Times(1);
	
	AudioItemMock cItem;
	EXPECT_CALL(cItem,setState(AudioItem::e_stateCodec)).Times(1);
	EXPECT_CALL(cItem,data()).Times(1).WillOnce(Return(&cData));
	EXPECT_CALL(cItem,state()).Times(1).WillOnce(Return(AudioItem::e_stateCodec));
	
	AOBaseProcessCodecLoopTest audio;
	EXPECT_CALL(audio,currentCallbackTime()).Times(1).WillOnce(Return(currentTime));
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,processCodecState(Pointee(&cItem),currentTime,initF)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(&cItem==audio.testProcessCodecLoop(&cItem,initF,loop));
	EXPECT_TRUE(loop);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecLoopNextItemWithLoopFalse)
{
	bool loop = true;
	bool initF = false;
	common::TimeStamp currentTime(1.0);
	
	AudioItemMock cItem;
	
	AOBaseProcessCodecLoopTest audio;
	EXPECT_CALL(audio,currentCallbackTime()).Times(1).WillOnce(Return(currentTime));
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(1));
	EXPECT_CALL(audio,processCodecState(Pointee(&cItem),currentTime,initF)).Times(1).WillOnce(Return(false));
	
	EXPECT_TRUE(&cItem==audio.testProcessCodecLoop(&cItem,initF,loop));
	EXPECT_FALSE(loop);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecLoopNextItemWithLoopTrueAndItemCodecState)
{
	bool loop = true;
	bool initF = false;
	common::TimeStamp currentTime(1.0);
	
	AudioItemMock cItem;
	EXPECT_CALL(cItem,state()).Times(1).WillOnce(Return(AudioItem::e_stateCodec));
	
	AOBaseProcessCodecLoopTest audio;
	EXPECT_CALL(audio,currentCallbackTime()).Times(1).WillOnce(Return(currentTime));
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(1));
	EXPECT_CALL(audio,processCodecState(Pointee(&cItem),currentTime,initF)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(&cItem==audio.testProcessCodecLoop(&cItem,initF,loop));
	EXPECT_TRUE(loop);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecLoopNextItemWithLoopTrueAndItemEmptyState)
{
	bool loop = true;
	bool initF = false;
	common::TimeStamp currentTime(1.0);
	
	AudioItemMock cItem,nItem;
	EXPECT_CALL(cItem,state()).Times(1).WillOnce(Return(AudioItem::e_stateEmpty));
	EXPECT_CALL(cItem,next()).Times(1).WillOnce(Return(&nItem));
	
	AOBaseProcessCodecLoopTest audio;
	EXPECT_CALL(audio,currentCallbackTime()).Times(1).WillOnce(Return(currentTime));
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(1));
	EXPECT_CALL(audio,processCodecState(Pointee(&cItem),currentTime,initF)).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,setCodecAudioItem(&nItem)).Times(1);
	
	EXPECT_TRUE(&nItem==audio.testProcessCodecLoop(&cItem,initF,loop));
	EXPECT_TRUE(loop);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecLoopNextItemWithLoopTrueAndItemFullState)
{
	bool loop = true;
	bool initF = false;
	common::TimeStamp currentTime(1.0);
	
	AudioItemMock cItem,nItem;
	EXPECT_CALL(cItem,state()).Times(1).WillOnce(Return(AudioItem::e_stateFull));
	EXPECT_CALL(cItem,next()).Times(1).WillOnce(Return(&nItem));
	
	AOBaseProcessCodecLoopTest audio;
	EXPECT_CALL(audio,currentCallbackTime()).Times(1).WillOnce(Return(currentTime));
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(1));
	EXPECT_CALL(audio,processCodecState(Pointee(&cItem),currentTime,initF)).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,setCodecAudioItem(&nItem)).Times(1);
	
	EXPECT_TRUE(&nItem==audio.testProcessCodecLoop(&cItem,initF,loop));
	EXPECT_TRUE(loop);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecLoopWhenCodecStateGivesBackDifferentItem)
{
	bool loop = true;
	bool initF = false;
	common::TimeStamp currentTime(1.0);
	
	AudioItemMock cItem,dItem,nItem;
	EXPECT_CALL(dItem,state()).Times(1).WillOnce(Return(AudioItem::e_stateFull));
	EXPECT_CALL(dItem,next()).Times(1).WillOnce(Return(&nItem));
	
	AOBaseProcessCodecLoopTest audio;
	EXPECT_CALL(audio,currentCallbackTime()).Times(1).WillOnce(Return(currentTime));
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(1));
	EXPECT_CALL(audio,processCodecState(Pointee(&cItem),currentTime,initF)).Times(1).WillOnce(DoAll(SetArgPointee<0>(&dItem),Return(true)));
	
	EXPECT_CALL(audio,setCodecAudioItem(&nItem)).Times(1);
	
	EXPECT_TRUE(&nItem==audio.testProcessCodecLoop(&cItem,initF,loop));
	EXPECT_TRUE(loop);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecStateTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getState,AOBase::States());
		MOCK_METHOD3(processCodecPlay,bool(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF));
		MOCK_METHOD3(processCodecCrossFade,bool(AudioItem* item,const common::TimeStamp& currentT,bool& initF));
		MOCK_METHOD0(processCodecPreBuffer,bool());
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecStatePlay)
{
	bool initF = true;
	AudioItem item;
	AudioItem *pItem = &item;
	common::TimeStamp currentT(1.0);

	AOBaseProcessCodecStateTest audio;
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_statePlay));
	EXPECT_CALL(audio,processCodecPlay(Pointee(&item),currentT,initF)).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)));
	
	EXPECT_TRUE(audio.testProcessCodecState(&pItem,currentT,initF));
	EXPECT_FALSE(initF);
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecStatePlayGivesBackDifferentItem)
{
	bool initF = true;
	AudioItem itemA,itemB;
	AudioItem *pItem = &itemA;
	common::TimeStamp currentT(1.0);

	AOBaseProcessCodecStateTest audio;
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_statePlay));
	EXPECT_CALL(audio,processCodecPlay(Pointee(&itemA),currentT,initF)).Times(1)
		.WillOnce(DoAll(SetArgPointee<0>(&itemB),SetArgReferee<2>(false),Return(true)));
	
	EXPECT_TRUE(audio.testProcessCodecState(&pItem,currentT,initF));
	EXPECT_FALSE(initF);
	EXPECT_TRUE(pItem==&itemB);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecStateCrossFade)
{
	bool initF = true;
	AudioItem item;
	AudioItem *pItem = &item;
	common::TimeStamp currentT(1.0);

	AOBaseProcessCodecStateTest audio;
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_stateCrossFade));
	EXPECT_CALL(audio,processCodecCrossFade(&item,currentT,initF)).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)));
	
	EXPECT_TRUE(audio.testProcessCodecState(&pItem,currentT,initF));
	EXPECT_FALSE(initF);
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecStatePreBuffer)
{
	bool initF = true;
	AudioItem item;
	AudioItem *pItem = &item;
	common::TimeStamp currentT(1.0);

	AOBaseProcessCodecStateTest audio;
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_statePreBuffer));
	EXPECT_CALL(audio,processCodecPreBuffer()).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testProcessCodecState(&pItem,currentT,initF));
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecStateNoCodec)
{
	bool initF = true;
	AudioItem item;
	AudioItem *pItem = &item;
	common::TimeStamp currentT(1.0);

	AOBaseProcessCodecStateTest audio;
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_stateNoCodec));
	
	EXPECT_FALSE(audio.testProcessCodecState(&pItem,currentT,initF));
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecStatePause)
{
	bool initF = true;
	AudioItem item;
	AudioItem *pItem = &item;
	common::TimeStamp currentT(1.0);

	AOBaseProcessCodecStateTest audio;
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_statePause));
	
	EXPECT_FALSE(audio.testProcessCodecState(&pItem,currentT,initF));
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecStateStop)
{
	bool initF = true;
	AudioItem item;
	AudioItem *pItem = &item;
	common::TimeStamp currentT(1.0);

	AOBaseProcessCodecStateTest audio;
	EXPECT_CALL(audio,getState()).Times(1).WillOnce(Return(AOBase::e_stateStop));
	
	EXPECT_FALSE(audio.testProcessCodecState(&pItem,currentT,initF));
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getAudioProcessType,tint());
		MOCK_METHOD3(processCodecPlayDecode,bool(AudioItem* item,const common::TimeStamp& currentT,bool& initF));
		MOCK_METHOD3(processCodecReadyForNext,void(AudioItem* item,bool completeFlag,tint iFrom));
		MOCK_METHOD3(processCodecPlayPostProcess,bool(AudioItem **pItem,const common::TimeStamp& currentT,bool completeFlag));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayGivenAPreviousCodecWasNotCompletedInLastLoopIteration)
{
	engine::RData *data = new engine::RData(20,2,2);
	engine::RData::Part& partA = data->nextPart();
	partA.length() = 5;
	partA.done() = true;
	engine::RData::Part& partB = data->nextPart();
	partB.length() = 5;
	partB.done() = true;

	AudioItem item;
	item.setData(data);
	
	AudioItem *pItem = &item;
	common::TimeStamp currentT(1.0);
	bool initF = true;
	
	AOBaseProcessCodecPlayTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,processCodecPlayDecode(&item,currentT,initF)).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)));
	EXPECT_CALL(audio,processCodecReadyForNext(&item,true,0)).Times(1);
	EXPECT_CALL(audio,processCodecPlayPostProcess(Pointee(&item),currentT,true)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testProcessCodecPlay(&pItem,currentT,initF));
	EXPECT_FALSE(initF);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayGivenAPreviousCodecWasCompletedInLastLoopIteration)
{
	engine::RData *data = new engine::RData(20,2,2);
	engine::RData::Part& partA = data->nextPart();
	partA.length() = 5;
	partA.done() = true;
	engine::RData::Part& partB = data->nextPart();
	partB.length() = 5;
	partB.done() = true;

	AudioItem item;
	item.setData(data);
	
	AudioItem *pItem = &item;
	common::TimeStamp currentT(1.0);
	bool initF = true;
	
	AOBaseProcessCodecPlayTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(audio,processCodecPlayDecode(&item,currentT,initF)).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)));
	EXPECT_CALL(audio,processCodecReadyForNext(&item,true,2)).Times(1);
	EXPECT_CALL(audio,processCodecPlayPostProcess(Pointee(&item),currentT,true)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testProcessCodecPlay(&pItem,currentT,initF));
	EXPECT_FALSE(initF);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayGivesBackDifferentItem)
{
	engine::RData *data = new engine::RData(20,2,2);
	engine::RData::Part& partA = data->nextPart();
	partA.length() = 5;
	partA.done() = true;
	engine::RData::Part& partB = data->nextPart();
	partB.length() = 5;
	partB.done() = true;

	AudioItem itemA,itemB;
	itemA.setData(data);
	
	AudioItem *pItem = &itemA;
	common::TimeStamp currentT(1.0);
	bool initF = true;
	
	AOBaseProcessCodecPlayTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,processCodecPlayDecode(&itemA,currentT,initF)).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)));
	EXPECT_CALL(audio,processCodecReadyForNext(&itemA,true,0)).Times(1);
	EXPECT_CALL(audio,processCodecPlayPostProcess(Pointee(&itemA),currentT,true)).Times(1).WillOnce(DoAll(SetArgPointee<0>(&itemB),Return(true)));
	
	EXPECT_TRUE(audio.testProcessCodecPlay(&pItem,currentT,initF));
	EXPECT_FALSE(initF);
	EXPECT_TRUE(pItem==&itemB);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayDecodeTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getAudioProcessType,tint());
		MOCK_CONST_METHOD0(getTrackTimeState,tint());
		MOCK_METHOD0(getCodec,engine::Codec *());
		MOCK_METHOD3(decodeAndResample,bool(engine::Codec *c,AudioItem *outputItem,bool& initF));
		MOCK_METHOD3(processCodecPlayDecodeInTime,bool(AudioItem *item,const common::TimeStamp& currentT,bool& initF));
		MOCK_METHOD2(processCodecEndForTimePositionComplete,bool(AudioItem *item,bool decodeFlag));
		MOCK_METHOD1(setCodecCurrentTime,void(const common::TimeStamp& t));
		MOCK_METHOD1(processCodecPlayTagPartAsRequired,void(engine::RData *data));
		MOCK_METHOD1(setAudioProcessType,void(tint pType));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayDecodeNotTrackingTimeStateAndNotEnd)
{
	common::TimeStamp currentT(1.0);
	bool initF = true;

	common::TimeStamp endPT(25.0);
	RDataPartMock partEnd;
	EXPECT_CALL(partEnd,end()).Times(1).WillOnce(ReturnRef(endPT));

	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(4));
	EXPECT_CALL(data,part(3)).Times(1).WillOnce(ReturnRef(partEnd));

	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;

	AOBaseProcessCodecPlayDecodeTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,getTrackTimeState()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,decodeAndResample(&codec,&item,initF)).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)));
	EXPECT_CALL(audio,setCodecCurrentTime(endPT)).Times(1);
	EXPECT_CALL(audio,processCodecPlayTagPartAsRequired(&data)).Times(1);
	EXPECT_CALL(audio,processCodecEndForTimePositionComplete(&item,true)).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,setAudioProcessType(0)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayDecode(&item,currentT,initF));
	EXPECT_FALSE(initF);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayDecodeNotTrackingTimeStateAndEnd)
{
	common::TimeStamp currentT(1.0);
	bool initF = true;

	common::TimeStamp endPT(25.0);
	RDataPartMock partEnd;
	EXPECT_CALL(partEnd,end()).Times(1).WillOnce(ReturnRef(endPT));

	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(4));
	EXPECT_CALL(data,part(3)).Times(1).WillOnce(ReturnRef(partEnd));

	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;

	AOBaseProcessCodecPlayDecodeTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,getTrackTimeState()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,decodeAndResample(&codec,&item,initF)).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)));
	EXPECT_CALL(audio,setCodecCurrentTime(endPT)).Times(1);
	EXPECT_CALL(audio,processCodecPlayTagPartAsRequired(&data)).Times(1);
	EXPECT_CALL(audio,processCodecEndForTimePositionComplete(&item,true)).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,setAudioProcessType(0)).Times(1);
	
	EXPECT_FALSE(audio.testProcessCodecPlayDecode(&item,currentT,initF));
	EXPECT_FALSE(initF);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayDecodeTrackingTimeState)
{
	common::TimeStamp currentT(1.0);
	bool initF = true;

	common::TimeStamp endPT(25.0);
	RDataPartMock partEnd;
	EXPECT_CALL(partEnd,end()).Times(1).WillOnce(ReturnRef(endPT));

	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(4));
	EXPECT_CALL(data,part(3)).Times(1).WillOnce(ReturnRef(partEnd));

	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;

	AOBaseProcessCodecPlayDecodeTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,getTrackTimeState()).Times(1).WillOnce(Return(1));
	EXPECT_CALL(audio,processCodecPlayDecodeInTime(&item,currentT,initF)).Times(1).
		WillOnce(DoAll(SetArgReferee<2>(false),Return(true)));
	EXPECT_CALL(audio,setCodecCurrentTime(endPT)).Times(1);
	EXPECT_CALL(audio,processCodecPlayTagPartAsRequired(&data)).Times(1);
	EXPECT_CALL(audio,processCodecEndForTimePositionComplete(&item,true)).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,setAudioProcessType(0)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayDecode(&item,currentT,initF));
	EXPECT_FALSE(initF);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayDecodeGivenNoDecodeProcess)
{
	common::TimeStamp currentT(1.0);
	bool initF = true;

	RDataMock data;

	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));

	AOBaseProcessCodecPlayDecodeTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(1));
	EXPECT_CALL(audio,setAudioProcessType(0)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayDecode(&item,currentT,initF));
	EXPECT_TRUE(initF);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayDecodeInTime : public AOBaseTest
{
	public:
		MOCK_METHOD0(getCodec,engine::Codec *());
		MOCK_METHOD3(decodeAndResample,bool(engine::Codec *c,AudioItem *outputItem,bool& initF));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayDecodeInTimeGivenFirstDecodeFails)
{
	common::TimeStamp currentT(18.0);
	bool initF = false;

	RDataMock data;
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));

	CodecMock codec;
	
	AOBaseProcessCodecPlayDecodeInTime audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,decodeAndResample(&codec,&item,A<bool&>())).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(true),Return(false)));
		
	EXPECT_FALSE(audio.testProcessCodecPlayDecodeInTime(&item,currentT,initF));
	EXPECT_TRUE(initF);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayDecodeInTimeGivenFirstDecodeHasNoParts)
{
	common::TimeStamp currentT(18.0);
	bool initF = true;

	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(0));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;
	
	AOBaseProcessCodecPlayDecodeInTime audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,decodeAndResample(&codec,&item,A<bool&>())).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(true),Return(true)));
		
	EXPECT_TRUE(audio.testProcessCodecPlayDecodeInTime(&item,currentT,initF));
	EXPECT_TRUE(initF);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayDecodeInTimeGivenFirstDecodeIsBeyondCurrentTime)
{
	common::TimeStamp timeA(19.0);
	common::TimeStamp currentT(18.0);
	bool initF = true;

	RDataPartMock partA;
	EXPECT_CALL(partA,end()).Times(1).WillOnce(ReturnRef(timeA));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(2)
		.WillOnce(Return(4)).WillOnce(Return(4));
	EXPECT_CALL(data,part(3)).Times(1).WillOnce(ReturnRef(partA));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;
	
	AOBaseProcessCodecPlayDecodeInTime audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,decodeAndResample(&codec,&item,A<bool&>())).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)));
		
	EXPECT_TRUE(audio.testProcessCodecPlayDecodeInTime(&item,currentT,initF));
	EXPECT_FALSE(initF);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayDecodeInTimeSecondDecodeFails)
{
	common::TimeStamp timeA(5.0);
	common::TimeStamp currentT(18.0);
	bool initF = true;

	RDataPartMock partA;
	EXPECT_CALL(partA,end()).Times(1).WillOnce(ReturnRef(timeA));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(2)
		.WillOnce(Return(4)).WillOnce(Return(4));
	EXPECT_CALL(data,part(3)).Times(1).WillOnce(ReturnRef(partA));
	EXPECT_CALL(data,reset()).Times(1);
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;
	
	AOBaseProcessCodecPlayDecodeInTime audio;
	EXPECT_CALL(audio,getCodec()).Times(2).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,decodeAndResample(&codec,&item,A<bool&>())).Times(2)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)))
		.WillOnce(Return(false));
		
	EXPECT_FALSE(audio.testProcessCodecPlayDecodeInTime(&item,currentT,initF));
	EXPECT_FALSE(initF);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayDecodeInTimeSecondDecodeHasNoParts)
{
	common::TimeStamp timeA(5.0);
	common::TimeStamp currentT(18.0);
	bool initF = true;

	RDataPartMock partA;
	EXPECT_CALL(partA,end()).Times(1).WillOnce(ReturnRef(timeA));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(3)
		.WillOnce(Return(4)).WillOnce(Return(4))
		.WillOnce(Return(0));
	EXPECT_CALL(data,part(3)).Times(1).WillOnce(ReturnRef(partA));
	EXPECT_CALL(data,reset()).Times(1);
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;
	
	AOBaseProcessCodecPlayDecodeInTime audio;
	EXPECT_CALL(audio,getCodec()).Times(2).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,decodeAndResample(&codec,&item,A<bool&>())).Times(2)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)))
		.WillOnce(Return(true));
		
	EXPECT_TRUE(audio.testProcessCodecPlayDecodeInTime(&item,currentT,initF));
	EXPECT_FALSE(initF);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayDecodeInTimeThreeDecodesBehindCurrentTime)
{
	common::TimeStamp timeA(5.0);
	common::TimeStamp timeB(10.0);
	common::TimeStamp timeC(15.0);
	common::TimeStamp timeD(20.0);
	common::TimeStamp currentT(18.0);
	bool initF = true;

	RDataPartMock partA;
	EXPECT_CALL(partA,end()).Times(1).WillOnce(ReturnRef(timeA));
	RDataPartMock partB;
	EXPECT_CALL(partB,end()).Times(1).WillOnce(ReturnRef(timeB));
	RDataPartMock partC;
	EXPECT_CALL(partC,end()).Times(1).WillOnce(ReturnRef(timeC));
	RDataPartMock partD;
	EXPECT_CALL(partD,end()).Times(1).WillOnce(ReturnRef(timeD));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(8)
		.WillOnce(Return(4)).WillOnce(Return(4))
		.WillOnce(Return(3)).WillOnce(Return(3))
		.WillOnce(Return(2)).WillOnce(Return(2))
		.WillOnce(Return(1)).WillOnce(Return(1));
	EXPECT_CALL(data,part(3)).Times(1).WillOnce(ReturnRef(partA));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(partB));
	EXPECT_CALL(data,part(1)).Times(1).WillOnce(ReturnRef(partC));
	EXPECT_CALL(data,part(0)).Times(1).WillOnce(ReturnRef(partD));
	EXPECT_CALL(data,reset()).Times(3);
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;
	
	AOBaseProcessCodecPlayDecodeInTime audio;
	EXPECT_CALL(audio,getCodec()).Times(4).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,decodeAndResample(&codec,&item,A<bool&>())).Times(4)
		.WillOnce(DoAll(SetArgReferee<2>(false),Return(true)))
		.WillRepeatedly(Return(true));
		
	EXPECT_TRUE(audio.testProcessCodecPlayDecodeInTime(&item,currentT,initF));
	EXPECT_FALSE(initF);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayTagPartAsRequiredTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getAudioProcessType,tint());
		MOCK_CONST_METHOD0(getRefStartAudioTime,common::TimeStamp());
		MOCK_METHOD1(setRefStartAudioTime,void(const common::TimeStamp& t));
		MOCK_METHOD0(getCodec,engine::Codec *());
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayTagPartAsRequiredWhenProcessTypeZero)
{
	RDataMock data;

	AOBaseProcessCodecPlayTagPartAsRequiredTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	
	audio.testProcessCodecPlayTagPartAsRequired(&data);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayTagPartAsRequiredWhenProcessTypeOne)
{
	RDataMock data;

	AOBaseProcessCodecPlayTagPartAsRequiredTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(1));
	
	audio.testProcessCodecPlayTagPartAsRequired(&data);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayTagPartAsRequiredTagWhenLocal)
{
	RDataPartMock part;
	EXPECT_CALL(part,setNext(true)).Times(1);
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(4));
	EXPECT_CALL(data,part(3)).Times(1).WillOnce(ReturnRef(part));
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	
	AOBaseProcessCodecPlayTagPartAsRequiredTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	
	audio.testProcessCodecPlayTagPartAsRequired(&data);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayTagPartAsRequiredTagWhenRemote)
{
	common::TimeStamp startRefTime(20.0);
	common::TimeStamp partStartTime(0.0);
	common::TimeStamp zeroT(0);
	
	RDataPartMock part;
	EXPECT_CALL(part,setNext(true)).Times(1);
	EXPECT_CALL(part,refStartTime()).Times(1).WillOnce(ReturnRef(partStartTime));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(2).WillRepeatedly(Return(4));
	EXPECT_CALL(data,part(3)).Times(2).WillRepeatedly(ReturnRef(part));
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	
	AOBaseProcessCodecPlayTagPartAsRequiredTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(2));
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getRefStartAudioTime()).Times(1).WillOnce(Return(startRefTime));
	EXPECT_CALL(audio,setRefStartAudioTime(Eq(zeroT))).Times(1);
	
	audio.testProcessCodecPlayTagPartAsRequired(&data);
	
	EXPECT_TRUE(partStartTime==20.0);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecReadyForNextTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getCodecTimeLength,const common::TimeStamp&());
		MOCK_METHOD1(setCodecTimeLength,void(const common::TimeStamp& t));
		MOCK_CONST_METHOD0(getCodecTimeLengthUpdate,bool());
		MOCK_METHOD3(processCodecPlayNextEndInParts,void(engine::RData *data,bool completeFlag,tint iFrom));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecReadyForNextGivenNoParts)
{
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(0));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return((engine::AData *)&data));
	
	AOBaseProcessCodecReadyForNextTest audio;
	
	audio.testProcessCodecReadyForNext(&item,false,0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecReadyForNextGivenCompleteButPartLess)
{
	common::TimeStamp codecEndTime(10.0);
	
	common::TimeStamp pEndT(8.0);
	RDataPartMock part;
	EXPECT_CALL(part,end()).Times(1).WillOnce(ReturnRef(pEndT));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(2).WillRepeatedly(Return(3));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(part));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return((engine::AData *)&data));
	
	AOBaseProcessCodecReadyForNextTest audio;
	EXPECT_CALL(audio,getCodecTimeLength()).Times(1).WillOnce(ReturnRef(codecEndTime));
	EXPECT_CALL(audio,processCodecPlayNextEndInParts(&data,true,0)).Times(1);
	
	audio.testProcessCodecReadyForNext(&item,true,0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecReadyForNextGivenCompleteButPartGreater)
{
	common::TimeStamp codecEndTime(7.0);
	
	common::TimeStamp pEndT(8.0);
	RDataPartMock part;
	EXPECT_CALL(part,end()).Times(1).WillOnce(ReturnRef(pEndT));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(2).WillRepeatedly(Return(3));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(part));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return((engine::AData *)&data));
	
	AOBaseProcessCodecReadyForNextTest audio;
	EXPECT_CALL(audio,getCodecTimeLength()).Times(1).WillOnce(ReturnRef(codecEndTime));
	EXPECT_CALL(audio,getCodecTimeLengthUpdate()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,setCodecTimeLength(Eq(pEndT))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextEndInParts(&data,true,0)).Times(1);
	
	audio.testProcessCodecReadyForNext(&item,true,0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecReadyForNextGivenNotCompleteAndCannotUpdate)
{
	common::TimeStamp pEndT(8.0);
	RDataPartMock part;
	EXPECT_CALL(part,end()).Times(1).WillOnce(ReturnRef(pEndT));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(2).WillRepeatedly(Return(3));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(part));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return((engine::AData *)&data));
	
	AOBaseProcessCodecReadyForNextTest audio;
	EXPECT_CALL(audio,getCodecTimeLengthUpdate()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,processCodecPlayNextEndInParts(&data,false,1)).Times(1);
	
	audio.testProcessCodecReadyForNext(&item,false,1);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecReadyForNextGivenNotCompleteAndCanUpdate)
{
	common::TimeStamp pEndT(8.0);
	RDataPartMock part;
	EXPECT_CALL(part,end()).Times(1).WillOnce(ReturnRef(pEndT));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(2).WillRepeatedly(Return(3));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(part));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return((engine::AData *)&data));
	
	AOBaseProcessCodecReadyForNextTest audio;
	EXPECT_CALL(audio,getCodecTimeLengthUpdate()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,setCodecTimeLength(Eq(pEndT))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextEndInParts(&data,false,2)).Times(1);
	
	audio.testProcessCodecReadyForNext(&item,false,2);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlaytNextEndInPartsTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getAudioProcessType,tint());
		MOCK_CONST_METHOD0(getCodecTimeLength,const common::TimeStamp&());
		MOCK_METHOD1(setCodecTimeLength,void(const common::TimeStamp& t));
		MOCK_CONST_METHOD0(getCodecTimeLengthUpdate,bool());
		MOCK_CONST_METHOD0(getNextOutState,tint());
		MOCK_METHOD1(processCodecPlayNextOutStateZero,void(engine::RData::Part& part));
		MOCK_METHOD2(processCodecPlayNextOutStateOne,void(engine::RData::Part& part,engine::RData *data));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextEndInPartsWithAudioProcessTypeAsTwo)
{
	RDataMock data;
	
	AOBaseProcessCodecPlaytNextEndInPartsTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(2));
	
	audio.testProcessCodecPlayNextEndInParts(&data,true,0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextEndInPartsWithNoParts)
{
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillRepeatedly(Return(0));
	
	AOBaseProcessCodecPlaytNextEndInPartsTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	
	audio.testProcessCodecPlayNextEndInParts(&data,true,0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextEndInPartsWithOnePart)
{
	RDataPartMock partA;
	
	RDataMock data;
	EXPECT_CALL(data,part(0)).Times(1).WillOnce(ReturnRef(partA));
	EXPECT_CALL(data,noParts()).Times(2).WillRepeatedly(Return(1));
	
	AOBaseProcessCodecPlaytNextEndInPartsTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,getNextOutState()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,processCodecPlayNextOutStateZero(Ref(partA))).Times(1);
	
	audio.testProcessCodecPlayNextEndInParts(&data,false,0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextEndInPartsOnMultiplePartsNoComplete)
{
	RDataPartMock partA;
	RDataPartMock partB;
	RDataPartMock partC;
	
	RDataMock data;
	EXPECT_CALL(data,part(0)).Times(1).WillOnce(ReturnRef(partA));
	EXPECT_CALL(data,part(1)).Times(1).WillOnce(ReturnRef(partB));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(partC));
	EXPECT_CALL(data,noParts()).Times(4).WillRepeatedly(Return(3));
	
	AOBaseProcessCodecPlaytNextEndInPartsTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,getNextOutState()).Times(3).WillOnce(Return(0))
		.WillOnce(Return(0))
		.WillOnce(Return(1));
	EXPECT_CALL(audio,processCodecPlayNextOutStateZero(Ref(partA))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextOutStateZero(Ref(partB))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextOutStateOne(Ref(partC),&data)).Times(1);
	
	audio.testProcessCodecPlayNextEndInParts(&data,false,0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextEndInPartsOnMultiplePartsLessThanEnd)
{
	common::TimeStamp codecEndTime(10.0);
	
	common::TimeStamp pAEnd(5.0);
	RDataPartMock partA;
	EXPECT_CALL(partA,end()).Times(1).WillOnce(ReturnRef(pAEnd));
	
	common::TimeStamp pBEnd(7.0);
	RDataPartMock partB;
	EXPECT_CALL(partB,end()).Times(1).WillRepeatedly(ReturnRef(pBEnd));
	
	common::TimeStamp pCEnd(9.0);
	RDataPartMock partC;
	EXPECT_CALL(partC,end()).Times(1).WillRepeatedly(ReturnRef(pCEnd));
	
	RDataMock data;
	EXPECT_CALL(data,part(0)).Times(1).WillOnce(ReturnRef(partA));
	EXPECT_CALL(data,part(1)).Times(1).WillOnce(ReturnRef(partB));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(partC));
	EXPECT_CALL(data,noParts()).Times(4).WillRepeatedly(Return(3));
	
	AOBaseProcessCodecPlaytNextEndInPartsTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,getCodecTimeLength()).Times(3).WillRepeatedly(ReturnRef(codecEndTime));
	EXPECT_CALL(audio,getNextOutState()).Times(3).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(1));
	EXPECT_CALL(audio,processCodecPlayNextOutStateZero(Ref(partA))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextOutStateZero(Ref(partB))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextOutStateOne(Ref(partC),&data)).Times(1);
	
	audio.testProcessCodecPlayNextEndInParts(&data,true,0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextEndInPartsOnMultiplePartsWithNoCodecTimeUpdate)
{
	common::TimeStamp codecEndTime(8.0);
	
	common::TimeStamp pAEnd(5.0);
	RDataPartMock partA;
	EXPECT_CALL(partA,end()).Times(1).WillOnce(ReturnRef(pAEnd));
	
	common::TimeStamp pBEnd(7.0);
	RDataPartMock partB;
	EXPECT_CALL(partB,end()).Times(1).WillRepeatedly(ReturnRef(pBEnd));
	
	common::TimeStamp pCEnd(9.0);
	RDataPartMock partC;
	EXPECT_CALL(partC,end()).Times(1).WillRepeatedly(ReturnRef(pCEnd));
	
	RDataMock data;
	EXPECT_CALL(data,part(0)).Times(1).WillOnce(ReturnRef(partA));
	EXPECT_CALL(data,part(1)).Times(1).WillOnce(ReturnRef(partB));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(partC));
	EXPECT_CALL(data,noParts()).Times(4).WillRepeatedly(Return(3));
	
	AOBaseProcessCodecPlaytNextEndInPartsTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(1));
	EXPECT_CALL(audio,getCodecTimeLength()).Times(3).WillRepeatedly(ReturnRef(codecEndTime));
	EXPECT_CALL(audio,getCodecTimeLengthUpdate()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,getNextOutState()).Times(3).WillOnce(Return(0)).WillOnce(Return(0)).WillOnce(Return(1));
	EXPECT_CALL(audio,processCodecPlayNextOutStateZero(Ref(partA))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextOutStateZero(Ref(partB))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextOutStateOne(Ref(partC),&data)).Times(1);
	
	audio.testProcessCodecPlayNextEndInParts(&data,true,0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextEndInPartsOnMultiplePartsAndDifferentStates)
{
	common::TimeStamp codecEndTime(8.0);
	
	common::TimeStamp pAEnd(5.0);
	RDataPartMock partA;
	EXPECT_CALL(partA,end()).Times(1).WillOnce(ReturnRef(pAEnd));
	
	common::TimeStamp pBEnd(7.0);
	RDataPartMock partB;
	EXPECT_CALL(partB,end()).Times(1).WillRepeatedly(ReturnRef(pBEnd));
	
	common::TimeStamp pCEnd(9.0);
	RDataPartMock partC;
	EXPECT_CALL(partC,end()).Times(2).WillRepeatedly(ReturnRef(pCEnd));
	
	RDataMock data;
	EXPECT_CALL(data,part(0)).Times(1).WillOnce(ReturnRef(partA));
	EXPECT_CALL(data,part(1)).Times(1).WillOnce(ReturnRef(partB));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(partC));
	EXPECT_CALL(data,noParts()).Times(4).WillRepeatedly(Return(3));
	
	AOBaseProcessCodecPlaytNextEndInPartsTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(1));
	EXPECT_CALL(audio,getCodecTimeLength()).Times(3).WillRepeatedly(ReturnRef(codecEndTime));
	EXPECT_CALL(audio,getCodecTimeLengthUpdate()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,setCodecTimeLength(pCEnd)).Times(1);
	EXPECT_CALL(audio,getNextOutState()).Times(3).WillOnce(Return(0)).WillOnce(Return(0)).WillRepeatedly(Return(1));
	EXPECT_CALL(audio,processCodecPlayNextOutStateZero(Ref(partA))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextOutStateZero(Ref(partB))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextOutStateOne(Ref(partC),&data)).Times(1);
	
	audio.testProcessCodecPlayNextEndInParts(&data,true,0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextEndInPartsOnMultiplePartsStartingFromKnownEndOfTrackPosition)
{
	common::TimeStamp codecEndTime(100.0);
	
	engine::RData *data = new engine::RData(20,2,2);
	
	common::TimeStamp pAEnd(150.0);
	engine::RData::Part& partA = data->nextPart();
	partA.length() = 5;
	partA.done() = true;
	partA.end() = pAEnd;
	
	common::TimeStamp pBEnd(3.0);
	engine::RData::Part& partB = data->nextPart();
	partB.length() = 5;
	partB.done() = true;
	partB.end() = pBEnd;
	
	common::TimeStamp pCEnd(6.0);
	engine::RData::Part& partC = data->nextPart();
	partC.length() = 5;
	partC.done() = true;
	partC.end() = pCEnd;
		
	AOBaseProcessCodecPlaytNextEndInPartsTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,getCodecTimeLength()).WillRepeatedly(ReturnRef(codecEndTime));
	EXPECT_CALL(audio,getNextOutState()).Times(2).WillOnce(Return(0)).WillOnce(Return(1));
	EXPECT_CALL(audio,processCodecPlayNextOutStateZero(Ref(partB))).Times(1);
	EXPECT_CALL(audio,processCodecPlayNextOutStateOne(Ref(partC),data)).Times(1);
	
	audio.testProcessCodecPlayNextEndInParts(data,true,1);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayNextOutStateZeroTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getNextCodecTime,common::TimeStamp());
		MOCK_METHOD1(setNextOutState,void(tint state));
		MOCK_METHOD0(getNextCodec,engine::Codec *());
		MOCK_METHOD0(emitOnReadyForNext,void());
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextOutStateZeroWhenNotReadyForNext)
{
	common::TimeStamp partStartT(4.0);
	common::TimeStamp nextCodecTime(5.0);
	
	RDataPartMock part;
	EXPECT_CALL(part,start()).Times(1).WillOnce(ReturnRef(partStartT));
	
	AOBaseProcessCodecPlayNextOutStateZeroTest audio;
	EXPECT_CALL(audio,getNextCodecTime()).Times(1).WillOnce(Return(nextCodecTime));

	audio.testProcessCodecPlayNextOutStateZero(part);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextOutStateZeroWhenReadyForNextAndIsSet)
{
	common::TimeStamp partStartT(6.0);
	common::TimeStamp nextCodecTime(5.0);
	
	RDataPartMock part;
	EXPECT_CALL(part,start()).Times(1).WillOnce(ReturnRef(partStartT));
	
	CodecMock codec;
	
	AOBaseProcessCodecPlayNextOutStateZeroTest audio;
	EXPECT_CALL(audio,getNextCodecTime()).Times(1).WillOnce(Return(nextCodecTime));
	EXPECT_CALL(audio,setNextOutState(1)).Times(1);
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return(&codec));
	
	audio.testProcessCodecPlayNextOutStateZero(part);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextOutStateZeroWhenReadyForNextButNotSet)
{
	common::TimeStamp partStartT(6.0);
	common::TimeStamp nextCodecTime(5.0);
	
	RDataPartMock part;
	EXPECT_CALL(part,start()).Times(1).WillOnce(ReturnRef(partStartT));
	
	AOBaseProcessCodecPlayNextOutStateZeroTest audio;
	EXPECT_CALL(audio,getNextCodecTime()).Times(1).WillOnce(Return(nextCodecTime));
	EXPECT_CALL(audio,setNextOutState(1)).Times(1);
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return((engine::Codec*)0));
	EXPECT_CALL(audio,emitOnReadyForNext()).Times(1);
	
	audio.testProcessCodecPlayNextOutStateZero(part);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayNextOutStateOneTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getCrossFadeFlag,bool());
		MOCK_METHOD0(getNextCodec,engine::Codec *());
		MOCK_CONST_METHOD0(getCrossFadeTime,const common::TimeStamp&());
		MOCK_METHOD1(setState,void(AOBase::States s));
		MOCK_METHOD0(getCrossFadeItem,AudioItem *());
		MOCK_METHOD1(setFrameFadeTime,void(const common::TimeStamp& t));
		MOCK_METHOD1(setAudioProcessType,void(tint type));
		MOCK_METHOD0(emitOnCrossfade,void());
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextOutStateOneAndCrossFadeIsNotSetup)
{
	RDataPartMock part;
	RDataMock data;

	AOBaseProcessCodecPlayNextOutStateOneTest audio;
	EXPECT_CALL(audio,getCrossFadeFlag()).Times(1).WillOnce(Return(false));
	
	audio.testProcessCodecPlayNextOutStateOne(part,&data);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextOutStateOneAndCrossFadeSetupButNoNextCodec)
{
	common::TimeStamp crossFadeT(9.0);

	RDataPartMock part;
	RDataMock data;

	AOBaseProcessCodecPlayNextOutStateOneTest audio;
	EXPECT_CALL(audio,getCrossFadeFlag()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return((engine::Codec *)0));
	
	audio.testProcessCodecPlayNextOutStateOne(part,&data);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextOutStateOneAndCrossFadeSetupReadyWithPartNotInFadeScope)
{
	common::TimeStamp partEndT(8.0);
	common::TimeStamp crossFadeT(9.0);
	
	RDataPartMock part;
	EXPECT_CALL(part,end()).Times(1).WillOnce(ReturnRef(partEndT));
	
	RDataMock data;
	CodecMock codec;

	AOBaseProcessCodecPlayNextOutStateOneTest audio;
	EXPECT_CALL(audio,getCrossFadeFlag()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCrossFadeTime()).Times(1).WillOnce(ReturnRef(crossFadeT));
	
	audio.testProcessCodecPlayNextOutStateOne(part,&data);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayNextOutStateOneAndCrossFadeSetupReadyAndRequired)
{
	common::TimeStamp partEndT(10.0);
	common::TimeStamp crossFadeT(9.0);
	
	RDataPartMock part;
	EXPECT_CALL(part,end()).Times(1).WillOnce(ReturnRef(partEndT));
	
	common::TimeStamp partStartTime(8.0);
	RDataPartMock partS;
	EXPECT_CALL(partS,start()).Times(1).WillOnce(ReturnRef(partStartTime));
	
	RDataMock data;
	EXPECT_CALL(data,part(0)).Times(1).WillOnce(ReturnRef(partS));
	
	ADataMock cfData;
	EXPECT_CALL(cfData,reset()).Times(1);
	AudioItemMock cfItem;
	EXPECT_CALL(cfItem,data()).Times(1).WillOnce(Return(&cfData));
	
	CodecMock codec;

	AOBaseProcessCodecPlayNextOutStateOneTest audio;
	EXPECT_CALL(audio,getCrossFadeFlag()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCrossFadeTime()).Times(1).WillOnce(ReturnRef(crossFadeT));
	EXPECT_CALL(audio,setState(AOBase::e_stateCrossFade)).Times(1);
	EXPECT_CALL(audio,getCrossFadeItem()).Times(1).WillOnce(Return(&cfItem));
	EXPECT_CALL(audio,setFrameFadeTime(partStartTime)).Times(1);
	EXPECT_CALL(audio,setAudioProcessType(1)).Times(1);
	EXPECT_CALL(audio,emitOnCrossfade()).Times(1);
	
	audio.testProcessCodecPlayNextOutStateOne(part,&data);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayPostProcessTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getAudioProcessType,tint());
		MOCK_METHOD2(processCodecPlayPostProcessComplete,bool(AudioItem **pItem,const common::TimeStamp& currentT));
		MOCK_METHOD1(processCodecPlayPostProcessRunning,bool(AudioItem **pItem));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,ProcessCodecPlayPostProcessNoPostProcess)
{
	common::TimeStamp currentT(1.0);
	AudioItem itemA;
	AudioItem *pItem = &itemA;

	AOBaseProcessCodecPlayPostProcessTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(1));
		
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcess(&pItem,currentT,true));
	EXPECT_TRUE(pItem==&itemA);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,ProcessCodecPlayPostProcessWhenComplete)
{
	common::TimeStamp currentT(1.0);
	AudioItem itemA;
	AudioItem *pItem = &itemA;

	AOBaseProcessCodecPlayPostProcessTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,processCodecPlayPostProcessComplete(Pointee(&itemA),currentT)).Times(1)
		.WillOnce(Return(false));
		
	EXPECT_FALSE(audio.testProcessCodecPlayPostProcess(&pItem,currentT,true));
	EXPECT_TRUE(pItem==&itemA);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,ProcessCodecPlayPostProcessWhenCompleteWhenDifferentItem)
{
	common::TimeStamp currentT(1.0);
	AudioItem itemA,itemB;
	AudioItem *pItem = &itemA;

	AOBaseProcessCodecPlayPostProcessTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,processCodecPlayPostProcessComplete(Pointee(&itemA),currentT)).Times(1)
		.WillOnce(DoAll(SetArgPointee<0>(&itemB),Return(true)));
		
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcess(&pItem,currentT,true));
	EXPECT_TRUE(pItem==&itemB);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,ProcessCodecPlayPostProcessWhenNotComplete)
{
	common::TimeStamp currentT(1.0);
	AudioItem itemA;
	AudioItem *pItem = &itemA;

	AOBaseProcessCodecPlayPostProcessTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,processCodecPlayPostProcessRunning(Pointee(&itemA))).Times(1)
		.WillOnce(Return(false));
		
	EXPECT_FALSE(audio.testProcessCodecPlayPostProcess(&pItem,currentT,false));
	EXPECT_TRUE(pItem==&itemA);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,ProcessCodecPlayPostProcessWhenNotCompleteWhenDifferentItem)
{
	common::TimeStamp currentT(1.0);
	AudioItem itemA,itemB;
	AudioItem *pItem = &itemA;

	AOBaseProcessCodecPlayPostProcessTest audio;
	EXPECT_CALL(audio,getAudioProcessType()).Times(1).WillOnce(Return(0));
	EXPECT_CALL(audio,processCodecPlayPostProcessRunning(Pointee(&itemA))).Times(1)
		.WillOnce(DoAll(SetArgPointee<0>(&itemB),Return(true)));
		
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcess(&pItem,currentT,false));
	EXPECT_TRUE(pItem==&itemB);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayPostProcessCompleteTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getCodec,engine::Codec *());
		MOCK_METHOD2(processCodecPlayPostProcessCompleteRemote,bool(AudioItem **pItem,const common::TimeStamp& currentT));
		MOCK_METHOD1(processCodecPlayPostProcessCompleteLocal,bool(AudioItem **pItem));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCompleteGivenNoParts)
{
	common::TimeStamp currentT(1.0);
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(0));

	AudioItemMock item;
	AudioItem *pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	AOBaseProcessCodecPlayPostProcessCompleteTest audio;
	
	EXPECT_FALSE(audio.testProcessCodecPlayPostProcessComplete(&pItem,currentT));
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCompleteGivenPartsAndRemote)
{
	common::TimeStamp currentT(1.0);
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(1));

	AudioItemMock item;
	AudioItem *pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	
	AOBaseProcessCodecPlayPostProcessCompleteTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,processCodecPlayPostProcessCompleteRemote(Pointee(&item),currentT))
		.Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testProcessCodecPlayPostProcessComplete(&pItem,currentT));
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCompleteGivenPartsAndRemoteWhenDifferentTime)
{
	common::TimeStamp currentT(1.0);
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(3));

	AudioItemMock item;
	AudioItem itemB,*pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	
	AOBaseProcessCodecPlayPostProcessCompleteTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,processCodecPlayPostProcessCompleteRemote(Pointee(&item),currentT))
		.Times(1).WillOnce(DoAll(SetArgPointee<0>(&itemB),Return(true)));
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessComplete(&pItem,currentT));
	EXPECT_TRUE(pItem==&itemB);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCompleteGivenPartsAndLocal)
{
	common::TimeStamp currentT(1.0);
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(1));

	AudioItemMock item;
	AudioItem *pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	
	AOBaseProcessCodecPlayPostProcessCompleteTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,processCodecPlayPostProcessCompleteLocal(Pointee(&item)))
		.Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testProcessCodecPlayPostProcessComplete(&pItem,currentT));
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCompleteGivenPartsAndLocalWhenDifferentItem)
{
	common::TimeStamp currentT(1.0);
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(3));

	AudioItemMock item;
	AudioItem itemB,*pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	
	AOBaseProcessCodecPlayPostProcessCompleteTest audio;
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,processCodecPlayPostProcessCompleteLocal(Pointee(&item)))
		.Times(1).WillOnce(DoAll(SetArgPointee<0>(&itemB),Return(true)));
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessComplete(&pItem,currentT));
	EXPECT_TRUE(pItem==&itemB);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayPostProcessCompleteRemoteTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getAudioStartFlag,bool());
		MOCK_METHOD1(postProcess,void(engine::RData *data));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCompleteRemoteAudioFlagNotSetAndCurrentLessThanEnd)
{
	common::TimeStamp currentT(1.0);
	common::TimeStamp endT(4.0);
	
	RDataPartMock part;
	EXPECT_CALL(part,end()).Times(1).WillOnce(ReturnRef(endT));

	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(part));

	AudioItemMock item;
	AudioItem *pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	EXPECT_CALL(item,setState(AudioItem::e_stateFull)).Times(1);
	
	AOBaseProcessCodecPlayPostProcessCompleteRemoteTest audio;
	EXPECT_CALL(audio,getAudioStartFlag()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,postProcess(&data)).Times(1);

	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessCompleteRemote(&pItem,currentT));
	EXPECT_TRUE(pItem==&item);
}


//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCompleteRemoteAudioFlagSetAndCurrentLessThanEnd)
{
	common::TimeStamp currentT(1.0);
	common::TimeStamp endT(4.0);
	
	RDataPartMock part;
	EXPECT_CALL(part,end()).Times(1).WillOnce(ReturnRef(endT));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(part));

	AudioItemMock item;
	AudioItem *pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	EXPECT_CALL(item,setState(AudioItem::e_stateFull)).Times(1);
	
	AOBaseProcessCodecPlayPostProcessCompleteRemoteTest audio;
	EXPECT_CALL(audio,getAudioStartFlag()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,postProcess(&data)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessCompleteRemote(&pItem,currentT));
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCompleteRemoteAudioFlagNotSetAndCurrentGreaterThanEnd)
{
	common::TimeStamp currentT(4.0);
	common::TimeStamp endT(1.0);
	
	RDataPartMock part;
	EXPECT_CALL(part,end()).Times(1).WillOnce(ReturnRef(endT));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(part));

	AudioItemMock item;
	AudioItem *pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	EXPECT_CALL(item,setState(AudioItem::e_stateFull)).Times(1);
	
	AOBaseProcessCodecPlayPostProcessCompleteRemoteTest audio;
	EXPECT_CALL(audio,getAudioStartFlag()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,postProcess(&data)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessCompleteRemote(&pItem,currentT));
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCompleteRemoteAudioFlagSetAndCurrentGreaterThanEnd)
{
	common::TimeStamp currentT(4.0);
	common::TimeStamp endT(1.0);
	
	RDataPartMock part;
	EXPECT_CALL(part,end()).Times(1).WillOnce(ReturnRef(endT));
	
	RDataMock data;
	EXPECT_CALL(data,noParts()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(data,part(2)).Times(1).WillOnce(ReturnRef(part));

	AudioItemMock item;
	AudioItem prevItem,*pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	EXPECT_CALL(item,prev()).Times(1).WillOnce(Return(&prevItem));
	
	AOBaseProcessCodecPlayPostProcessCompleteRemoteTest audio;
	EXPECT_CALL(audio,getAudioStartFlag()).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessCompleteRemote(&pItem,currentT));
	EXPECT_TRUE(pItem==&prevItem);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayPostProcessCompleteLocalTest : public AOBaseTest
{
	public:
		MOCK_METHOD1(postProcess,void(engine::RData *data));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCompleteLocalFunctionality)
{
	RDataMock data;
	AudioItemMock item;
	AudioItem *pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	EXPECT_CALL(item,setState(AudioItem::e_stateFull)).Times(1);
	
	AOBaseProcessCodecPlayPostProcessCompleteLocalTest audio;
	EXPECT_CALL(audio,postProcess(&data)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessCompleteLocal(&pItem));
	EXPECT_TRUE(pItem==&item);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayPostProcessRunningTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getCodec,engine::Codec *());
		MOCK_METHOD0(stopCodecDoNext,bool());
		MOCK_METHOD1(processCodecPlayPostProcessRunningWithNext,void(AudioItem **pItem));
		MOCK_METHOD1(processCodecPlayPostProcessRunningWithNoNext,void(AudioItem **pItem));
		MOCK_METHOD1(processCodecPlayPostProcessCheckBufferedState,bool(AudioItem **pItem));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessRunningCodecLocalAndDoNext)
{
	AudioItem item;
	AudioItem *pItem = &item;
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	
	AOBaseProcessCodecPlayPostProcessRunningTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,stopCodecDoNext()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,processCodecPlayPostProcessRunningWithNext(&pItem)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessRunning(&pItem));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessRunningCodecLocalAndNotDoNext)
{
	AudioItem item;
	AudioItem *pItem = &item;
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	
	AOBaseProcessCodecPlayPostProcessRunningTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,stopCodecDoNext()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,processCodecPlayPostProcessRunningWithNoNext(&pItem)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessRunning(&pItem));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessRunningCodecRemoteCompleteAndDoNext)
{
	AudioItem item;
	AudioItem *pItem = &item;
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isComplete()).Times(1).WillOnce(Return(true));
	
	AOBaseProcessCodecPlayPostProcessRunningTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,stopCodecDoNext()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,processCodecPlayPostProcessRunningWithNext(&pItem)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessRunning(&pItem));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessRunningCodecRemoteCompleteAndNotDoNext)
{
	AudioItem item;
	AudioItem *pItem = &item;
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isComplete()).Times(1).WillOnce(Return(true));
	
	AOBaseProcessCodecPlayPostProcessRunningTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,stopCodecDoNext()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,processCodecPlayPostProcessRunningWithNoNext(&pItem)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessRunning(&pItem));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessRunningCodecRemoteIncompleteAndPreBuffer)
{
	AudioItem item;
	AudioItem *pItem = &item;
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isComplete()).Times(1).WillOnce(Return(false));
	
	AOBaseProcessCodecPlayPostProcessRunningTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,processCodecPlayPostProcessCheckBufferedState(&pItem)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessRunning(&pItem));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessRunningCodecRemoteIncompleteAndNoPreBuffer)
{
	AudioItem item;
	AudioItem *pItem = &item;
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isComplete()).Times(1).WillOnce(Return(false));
	
	AOBaseProcessCodecPlayPostProcessRunningTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,processCodecPlayPostProcessCheckBufferedState(&pItem)).Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testProcessCodecPlayPostProcessRunning(&pItem));
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayPostProcessRunningWithNextTest : public AOBaseTest
{
	public:
		MOCK_METHOD1(setAudioProcessType,void(tint pType));
		MOCK_METHOD1(setTrackTimeState,void(tint state));
		MOCK_METHOD1(postProcess,void(engine::RData *data));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessRunningWithNextDataHasRemaining)
{
	RDataMock data;
	EXPECT_CALL(data,rLength()).Times(1).WillOnce(Return(50));
	
	AudioItemMock item;
	AudioItem *pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	AOBaseProcessCodecPlayPostProcessRunningWithNextTest audio;
	EXPECT_CALL(audio,setAudioProcessType(2)).Times(1);
	EXPECT_CALL(audio,setTrackTimeState(0)).Times(1);
	
	audio.testProcessCodecPlayPostProcessRunningWithNext(&pItem);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessRunningWithNextDataHasNoRemaining)
{
	RDataMock data;
	EXPECT_CALL(data,rLength()).Times(1).WillOnce(Return(0));
	
	AudioItemMock item;
	AudioItem *pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	EXPECT_CALL(item,setState(AudioItem::e_stateFull)).Times(1);
	
	AOBaseProcessCodecPlayPostProcessRunningWithNextTest audio;
	EXPECT_CALL(audio,setAudioProcessType(2)).Times(1);
	EXPECT_CALL(audio,setTrackTimeState(0)).Times(1);
	EXPECT_CALL(audio,postProcess(&data)).Times(1);
	
	audio.testProcessCodecPlayPostProcessRunningWithNext(&pItem);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayPostProcessRunningWithNoNextTest : public AOBaseTest
{
	public:
		MOCK_METHOD1(postProcess,void(engine::RData *data));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessRunningWithNoNextFunctionality)
{
	RDataMock data;
	
	AudioItemMock item;
	AudioItem *pItem = &item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	EXPECT_CALL(item,setState(AudioItem::e_stateFullEnd)).Times(1);
	
	AOBaseProcessCodecPlayPostProcessRunningWithNoNextTest audio;
	EXPECT_CALL(audio,postProcess(&data)).Times(1);
	
	audio.testProcessCodecPlayPostProcessRunningWithNoNext(&pItem);
}

//-------------------------------------------------------------------------------------------
// work forward round from the current callback item until the given item is reached
//
// terminate if item, after processing, is codec
// terminate if item, before processing, is seen to be the orginal callback from which
// processing started.
// terminate if item, after processing, is the given item
//
// if the state of an item is empty or done then it's time values are not counted.
//
// if no callback item => no play time hence return 0
//
// for each part add up the difference between start and end of given part
// this means that if multiple tracks are already decoded on the buffer
// the absolute play time marked on the tracks are not referenced. Rather
// the time is created from the sum of the relative time differences.
//-------------------------------------------------------------------------------------------
// The timeFromEndOfItemBeingPlayed all use 5 item cyclic buffers.
//-------------------------------------------------------------------------------------------

AudioItem **TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer()
{
	int i;
    AudioItem **pItemArray = new AudioItem* [5];
	AudioItem *pItem = 0,*cItem;
	
	for(i=0;i<5;i++)
	{
        engine::RData *data = new engine::RData();
		pItemArray[i] = cItem = new AudioItem;
		cItem->setData(data);
		if(pItem!=0)
		{
			pItem->setNext(cItem);
		}
		cItem->setPrev(pItem);
		pItem = cItem;
	}
	pItemArray[0]->setPrev(pItemArray[4]);
	pItemArray[4]->setNext(pItemArray[0]);
	return pItemArray;
}

//-------------------------------------------------------------------------------------------

void TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart(AudioItem *item,AudioItem::ItemStates state,double start,double end)
{
	item->setState(state);
	item->data()->reset();
	item->data()->start() = start;
	item->data()->end() = end;
	
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());
	engine::RData::Part& p = data->nextPart();
	p.length() = data->rLength();
	p.start() = start;
	p.end() = end;
}

//-------------------------------------------------------------------------------------------

void TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(AudioItem *item,AudioItem::ItemStates state,double pS1,double pE1,double pS2,double pE2,bool nTrack)
{
	item->setState(state);
	item->data()->reset();
    item->data()->start() = pS1;
    item->data()->end() = pE1;
	
	engine::RData *data = dynamic_cast<engine::RData *>(item->data());

	engine::RData::Part& pA = data->nextPart();
	pA.length() = data->rLength() / 2;
	pA.start() = pS1;
	pA.end() = pE1;

	engine::RData::Part& pB = data->nextPart();
	pB.length() = data->rLength();
	pB.start() = pS2;
	pB.end() = pE2;
	if(nTrack)
	{
		pB.setNext(nTrack);
	}
}

//-------------------------------------------------------------------------------------------

void TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(AudioItem **pItemArray)
{
    for(tint i=0;i<5;i++)
	{
		AudioItem *pItem = pItemArray[i];
		delete pItem;
	}
	delete [] pItemArray; 
}

//-------------------------------------------------------------------------------------------

class AOBaseTimeFromEndOfItemBeingPlayedTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(currentPlayTime,common::TimeStamp());
		MOCK_METHOD0(getCallbackAudioItem,AudioItem *());
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeBeforeCallAndCallbackSingle)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0),  1.0       = 3.0 =  3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 =  6.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 15.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 17.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 21.0
	
	common::TimeStamp playTime(2.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 + 1.0,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 + 1.0,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 + 1.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeInPartACallAndCallbackSingle)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0),  1.0       = 3.0 =  3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 =  6.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 15.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 17.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 21.0
	
	common::TimeStamp playTime(3.4);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 0.4,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 0.4,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 0.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 0.4,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeInMissingGapCallAndCallbackSingle)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0),  1.0       = 3.0 =  3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 =  6.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 15.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 17.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 21.0
	
	common::TimeStamp playTime(4.4);
	
    AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 1.4,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 1.4,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 1.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 1.4,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeInPartBCallAndCallbackSingle)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0),  1.0       = 3.0 =  3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 =  6.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 15.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 17.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 21.0
	
	common::TimeStamp playTime(5.4);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 2.4,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 2.4,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 2.4,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 2.4,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeAfterCallInGapAndCallbackSingle)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0),  1.0       = 3.0 =  3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 =  6.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 15.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 17.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 21.0
	
	common::TimeStamp playTime(6.5);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 3.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 3.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 3.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 3.5,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 3.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 3.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 3.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 3.5,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 3.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 3.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 3.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 3.5,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeAfterCallInNextAndCallbackSingle)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0),  1.0       = 3.0 =  3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 =  6.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 15.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 17.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 21.0
	
	common::TimeStamp playTime(8.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),       0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 4.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 4.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 4.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 4.0,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),       0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 4.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 4.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 4.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 4.0,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),       0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0 - 4.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0 - 4.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0 - 4.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0 - 4.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeBeforeCallAndCallbackDouble)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback ,10.0,12.0, 1.0, 6.0,  true); //  2.0 ,(1.0),  5.0       = 8.0 =  8.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 = 11.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 20.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 22.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 26.0
	
	common::TimeStamp playTime(9.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 + 1.0,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 + 1.0,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 + 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 + 1.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeInPartACallAndCallbackDouble)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback ,10.0,12.0, 1.0, 6.0,  true); //  2.0 ,(1.0),  5.0       = 8.0 =  8.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 = 11.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 20.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 22.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 26.0
	
	common::TimeStamp playTime(11.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 1.0,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 1.0,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 1.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 1.0,c_TOLERANCE);

	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeInMissingGapCallAndCallbackDouble)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback ,10.0,12.0, 1.0, 6.0,  true); //  2.0 ,(1.0),  5.0       = 8.0 =  8.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 = 11.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 20.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 22.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 26.0
	
	common::TimeStamp playTime(0.5);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 2.5,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 2.5,c_TOLERANCE);

	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 2.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 2.5,c_TOLERANCE);

	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeInPartBCallAndCallbackDouble)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback ,10.0,12.0, 1.0, 6.0,  true); //  2.0 ,(1.0),  5.0       = 8.0 =  8.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 = 11.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 20.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 22.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 26.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 5.0,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 5.0,c_TOLERANCE);

	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 5.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 5.0,c_TOLERANCE);

	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeAfterCallInGapAndCallbackDouble)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback ,10.0,12.0, 1.0, 6.0,  true); //  2.0 ,(1.0),  5.0       = 8.0 =  8.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 = 11.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 20.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 22.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 26.0
	
	common::TimeStamp playTime(6.5);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),      11.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 8.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 8.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 8.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 8.5,c_TOLERANCE);
	
	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),      11.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 8.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 8.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 8.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 8.5,c_TOLERANCE);

	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),      11.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 - 8.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 - 8.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 - 8.5,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 - 8.5,c_TOLERANCE);

	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);		
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenPlayTimeAfterCallInNextAndCallbackDouble)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback ,10.0,12.0, 1.0, 6.0,  true); //  2.0 ,(1.0),  5.0       = 8.0 =  8.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull     , 7.0, 9.0);                  // (1.0), 2.0              = 3.0 = 11.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull     ,11.5,13.5,16.0,18.0, false); // (2.5), 2.0 , (2.5), 2.0 = 9.0 = 20.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec    ,18.0,20.0);                  //  2.0                    = 2.0 = 22.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty    ,21.0,22.0,22.0,24.0, false); // (1.0), 1.0 ,  2.0       = 4.0 = 26.0
	
	common::TimeStamp playTime(8.0); // This is to be considered to before the callback
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 + 2.0,c_TOLERANCE);

	pItemArray[0]->setState(AudioItem::e_stateFull);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 + 2.0,c_TOLERANCE);

	pItemArray[0]->setState(AudioItem::e_stateDone);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 8.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),11.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),20.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),22.0 + 2.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),22.0 + 2.0,c_TOLERANCE);

	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenEmptyEmptyEmptyEmptyEmptySingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateEmpty, 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateEmpty, 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateEmpty, 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateEmpty,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),0.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenEmptyDoneEmptyEmptyEmptySingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateEmpty, 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateDone , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateEmpty, 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateEmpty,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),0.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenEmptyCodecEmptyEmptyEmptySingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateEmpty, 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateCodec, 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateEmpty, 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateEmpty,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),3.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenEmptyFullEmptyEmptyEmptySingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateEmpty, 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateEmpty, 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateEmpty,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),3.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenDoneEmptyEmptyEmptyEmptySingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateDone , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateEmpty, 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateEmpty, 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateEmpty,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),0.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenDoneDoneEmptyEmptyEmptySingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateDone , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateDone , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateEmpty , 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateEmpty ,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty ,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),0.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),0.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenDoneCodecEmptyEmptyEmptySingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateDone  , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateCodec , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateEmpty , 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateEmpty ,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty ,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),6.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenDoneFullEmptyEmptyEmptySingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateDone  , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull  , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateEmpty , 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateEmpty ,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty ,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),6.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenCallCodecEmptyEmptyEmptySingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateCodec , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateEmpty , 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateEmpty ,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty ,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])),3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])),6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),6.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenCallFullFullDoneDoneSingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull  , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull  , 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateDone  ,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateDone  ,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),15.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),15.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenCallFullFullCodecEmptySingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull  , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull  , 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec ,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty ,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenCallFullFullCodecDoneSingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull  , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull  , 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateCodec ,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateDone  ,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),17.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenCallFullFullFullCodecSingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull , 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateFull ,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateCodec ,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),21.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenCallFullFullFullFullSingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull , 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateFull ,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateFull ,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),21.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenDoneFullFullFullFullSingleTrack)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateDone , 3.0, 4.0, 5.0, 6.0, false); //  1.0, (1.0), 1.0        = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[1],AudioItem::e_stateFull , 6.0, 9.0);                  //  3.0                    = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull , 9.0,16.0,16.0,18.0, false); //  7.0 , 2.0              = 9.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithOnePart( pItemArray[3],AudioItem::e_stateFull ,18.0,20.0);                  //  2.0                    = 2.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateFull ,21.0,22.0,23.0,24.0, false); // (1.0), 1.0 , (1.0), 1.0 = 4.0
	
	// 0 - --- , 1.0, (1.0), 1.0 = 3.0 =  3.0
	// 1 - --- , 3.0             = 3.0 =  6.0
	// 2 - --- , 7.0,  --- , 2.0 = 9.0 = 15.0
	// 3 - --- , 2.0             = 2.0 = 17.0
	// 4 -(1.0), 1.0, (1.0), 1.0 = 4.0 = 21.0
	
	common::TimeStamp playTime(3.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 3.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 6.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])),15.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),17.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),21.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenCallFullFullCodecEmptyThreeTracks)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback ,13.0,15.0, 0.0, 5.0, true); // 2.0, N, 5.0 = 7.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[1],AudioItem::e_stateFull , 8.0,10.0, 0.0, 2.0, true); // (3.0), 2.0, N, 2.0 = 7.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull , 2.5, 3.5, 3.5, 5.0,false); // (0.5), 1.0, 1.5 = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[3],AudioItem::e_stateCodec , 0.0, 3.5, 4.0, 6.0,false); // N, 3.5, (0.5), 2.0 = 6.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateEmpty , 6.0, 8.0, 8.5,11.0,false); // 2.0, (0.5), 2.5 = 4.0
    engine::RData *rData = dynamic_cast<engine::RData *>(pItemArray[3]->data());
    rData->part(0).setNext(true);

	// 0 -  --- , 2.0,    N , 5.0 = 7.0 =  7.0
	// 1 - (3.0), 2.0,    N , 2.0 = 7.0 = 14.0
	// 2 - (0.5), 1.0,  --- , 1.5 = 3.0 = 17.0
	// 3 -    N , 3.5, (0.5), 2.0 = 6.0 = 23.0
	// 4 -  --- , 2.0, (0.5), 2.5 = 5.0 = 28.0

	common::TimeStamp playTime(13.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 7.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 14.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])), 17.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),23.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),23.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenCallbackFullFullFullFullEndThreeTracks)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateCallback ,13.0,15.0, 0.0, 5.0, true); // 2.0, N, 5.0 = 7.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[1],AudioItem::e_stateFull , 8.0,10.0, 0.0, 2.0, true); // (3.0), 2.0, N, 2.0 = 7.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull , 2.5, 3.5, 3.5, 5.0,false); // (0.5), 1.0, 1.5 = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[3],AudioItem::e_stateFull , 0.0, 3.5, 4.0, 6.0,false); // N, 3.5, (0.5), 2.0 = 6.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateFullEnd , 6.0, 8.0, 8.5,11.0,false); // 2.0, (0.5), 2.5 = 4.0
    engine::RData *rData = dynamic_cast<engine::RData *>(pItemArray[3]->data());
    rData->part(0).setNext(true);

	// 0 -  --- , 2.0,    N , 5.0 = 7.0 =  7.0
	// 1 - (3.0), 2.0,    N , 2.0 = 7.0 = 14.0
	// 2 - (0.5), 1.0,  --- , 1.5 = 3.0 = 17.0
    // 3 -    N , 3.5, (0.5), 2.0 = 6.0 = 23.0
	// 4 -  --- , 2.0, (0.5), 2.5 = 5.0 = 28.0

	common::TimeStamp playTime(13.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 7.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 14.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])), 17.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),23.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),28.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeFromEndOfItemBeingPlayedGivenDoneFullFullFullFullThreeTracks)
{
	const tfloat64 c_TOLERANCE = 0.0000001;
	
	AudioItem **pItemArray = TimeFromEndOfItemBeingPlayedGivenSetupDefaultCyclicBuffer();
	
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[0],AudioItem::e_stateDone ,13.0,15.0, 0.0, 5.0, true); // 2.0, N, 5.0 = 7.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[1],AudioItem::e_stateFull , 8.0,10.0, 0.0, 2.0, true); // (3.0), 2.0, N, 2.0 = 7.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[2],AudioItem::e_stateFull , 2.5, 3.5, 3.5, 5.0,false); // (0.5), 1.0, 1.5 = 3.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[3],AudioItem::e_stateFull , 0.0, 3.5, 4.0, 6.0,false); // N, 3.5, (0.5), 2.0 = 6.0
	TimeFromEndOfItemBeingPlayedGivenSetupItemWithTwoParts(pItemArray[4],AudioItem::e_stateFull , 6.0, 8.0, 8.5,11.0,false); // 2.0, (0.5), 2.5 = 4.0
    engine::RData *rData = dynamic_cast<engine::RData *>(pItemArray[3]->data());
    rData->part(0).setNext(true);

	// 0 -  --- , 2.0,    N , 5.0 = 7.0 =  7.0
	// 1 - (3.0), 2.0,    N , 2.0 = 7.0 = 14.0
	// 2 - (0.5), 1.0,  --- , 1.5 = 3.0 = 17.0
	// 3 -    N , 3.5, (0.5), 2.0 = 6.0 = 23.0
	// 4 -  --- , 2.0, (0.5), 2.5 = 5.0 = 28.0

	common::TimeStamp playTime(13.0);
	
	AOBaseTimeFromEndOfItemBeingPlayedTest audio;
	EXPECT_CALL(audio,currentPlayTime()).WillRepeatedly(Return(playTime));
	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(pItemArray[0]));
	
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[0])), 7.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[1])), 14.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[2])), 17.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[3])),23.0,c_TOLERANCE);
	EXPECT_NEAR(static_cast<tfloat64>(audio.testTimeFromEndOfItemBeingPlayed(pItemArray[4])),28.0,c_TOLERANCE);
	
	TimeFromEndOfItemBeingPlayedGivenFreeCyclicBuffer(pItemArray);
}

//-------------------------------------------------------------------------------------------

class AOBaseProcessCodecPlayPostProcessCheckBufferedStateTest : public AOBaseTest
{
	public:
		MOCK_METHOD1(timeFromEndOfItemBeingPlayed,common::TimeStamp(AudioItem *item));
		MOCK_METHOD0(getRemoteTimeSync,common::TimeStamp());
		MOCK_METHOD1(setState,void(AOBase::States s));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCheckBufferedStateWithTimeBelowPreBufferLimit)
{
	AudioItem *item = new AudioItem;

	common::TimeStamp limitT(3.0);
	common::TimeStamp currentT(0.8);

	AOBaseProcessCodecPlayPostProcessCheckBufferedStateTest audio;
	EXPECT_CALL(audio,timeFromEndOfItemBeingPlayed(item)).Times(1).WillOnce(Return(currentT));
	EXPECT_CALL(audio,getRemoteTimeSync()).Times(1).WillOnce(Return(limitT));
	EXPECT_CALL(audio,setState(AOBase::e_statePreBuffer)).Times(1);
	
	EXPECT_TRUE(audio.testProcessCodecPlayPostProcessCheckBufferedState(&item));
	
    delete item;
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecPlayPostProcessCheckBufferedStateWithTimeAbovePreBufferLimit)
{
	AudioItem *item = new AudioItem;

	common::TimeStamp limitT(3.0);
	common::TimeStamp currentT(1.1);

	AOBaseProcessCodecPlayPostProcessCheckBufferedStateTest audio;
	EXPECT_CALL(audio,timeFromEndOfItemBeingPlayed(item)).Times(1).WillOnce(Return(currentT));
	EXPECT_CALL(audio,getRemoteTimeSync()).Times(1).WillOnce(Return(limitT));
	
	EXPECT_FALSE(audio.testProcessCodecPlayPostProcessCheckBufferedState(&item));
	
    delete item;
}

//-------------------------------------------------------------------------------------------

class AOBaseDecodeAndResampleInterleaveOutputChannelsTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getRUsedO,const tint&());
		MOCK_METHOD1(setRUsedO,void(tint usedO));
		MOCK_CONST_METHOD0(getROutNo,const tint&());
        MOCK_METHOD1(getROut,sample_t *(tint ch));
        MOCK_CONST_METHOD0(getNoInChannels,tint());
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith2ChannelsAndZeroTimeLength)
{
	tint dLen = 0;
	tint rLen = 3;
    tint rUsedO = 0;
    tint rOutNo = 3;
#if defined(SINGLE_FLOAT_SAMPLE)
    sample_t chA[3] = {1.0f, 2.0f, 3.0f};
    sample_t chB[3] = {5.0f, 6.0f, 7.0f};
    sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    sample_t ext[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
#else
    sample_t chA[3] = {1.0, 2.0, 3.0};
    sample_t chB[3] = {5.0, 6.0, 7.0};
    sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    sample_t ext[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
#endif

	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getROut(1)).WillRepeatedly(Return(chB));
    EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
    EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(audio,setRUsedO(0)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==0);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith2ChannelsAndZeroDataLength)
{
	tint dLen = 3;
	tint rLen = 0;
    tint rUsedO = 0;
    tint rOutNo = 3;
#if defined(SINGLE_FLOAT_SAMPLE)
    sample_t chA[3] = {1.0f, 2.0f, 3.0f};
    sample_t chB[3] = {5.0f, 6.0f, 7.0f};
    sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    sample_t ext[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
#else
    sample_t chA[3] = {1.0, 2.0, 3.0};
    sample_t chB[3] = {5.0, 6.0, 7.0};
    sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    sample_t ext[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
#endif
	
	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getROut(1)).WillRepeatedly(Return(chB));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(audio,setRUsedO(0)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==0);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith2ChannelsAndZeroOutputLength)
{
	tint dLen = 3;
	tint rLen = 3;
    tint rUsedO = 0;
    tint rOutNo = 0;
#if defined(SINGLE_FLOAT_SAMPLE)
    sample_t chA[3] = {1.0f, 2.0f, 3.0f};
    sample_t chB[3] = {5.0f, 6.0f, 7.0f};
    sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    sample_t ext[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
#else
    sample_t chA[3] = {1.0, 2.0, 3.0};
    sample_t chB[3] = {5.0, 6.0, 7.0};
    sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    sample_t ext[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
#endif
	
	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getROut(1)).WillRepeatedly(Return(chB));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(audio,setRUsedO(0)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==0);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith2ChannelsAndNoOutputLength)
{
	tint dLen = 3;
	tint rLen = 3;
	tint rUsedO = 3;
	tint rOutNo = 3;
#if defined(SINGLE_FLOAT_SAMPLE)
    sample_t chA[3] = {1.0f, 2.0f, 3.0f};
    sample_t chB[3] = {5.0f, 6.0f, 7.0f};
    sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    sample_t ext[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
#else
    sample_t chA[3] = {1.0, 2.0, 3.0};
    sample_t chB[3] = {5.0, 6.0, 7.0};
    sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    sample_t ext[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
#endif

	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getROut(1)).WillRepeatedly(Return(chB));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(audio,setRUsedO(3)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==0);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith1ChannelAndSmallerTimeLength)
{
	// dLen < rLen = getROutNo()
	tint dLen = 4;
	tint rLen = 6;
	tint rUsedO = 1;
	tint rOutNo = 6;
#if defined(SINGLE_FLOAT_SAMPLE)
    sample_t chA[6] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
	sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	sample_t ext[6] = {2.0f, 3.0f, 4.0f, 5.0f, 0.0f, 0.0f};
#else
    sample_t chA[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
	sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	sample_t ext[6] = {2.0, 3.0, 4.0, 5.0, 0.0, 0.0};
#endif

	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(1));
	EXPECT_CALL(audio,setRUsedO(5)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==4);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith2ChannelsAndSmallerTimeLength)
{
	// dLen < rLen = getROutNo()
	tint dLen = 2;
	tint rLen = 3;
	tint rUsedO = 0;
	tint rOutNo = 3;
#if defined(SINGLE_FLOAT_SAMPLE)
	sample_t chA[3] = {1.0f, 2.0f, 3.0f};
	sample_t chB[3] = {5.0f, 6.0f, 7.0f};
	sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	sample_t ext[6] = {1.0f, 5.0f, 2.0f, 6.0f, 0.0f, 0.0f};
#else
	sample_t chA[3] = {1.0, 2.0, 3.0};
	sample_t chB[3] = {5.0, 6.0, 7.0};
	sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	sample_t ext[6] = {1.0, 5.0, 2.0, 6.0, 0.0, 0.0};
#endif

	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getROut(1)).WillRepeatedly(Return(chB));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(audio,setRUsedO(2)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==2);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith1ChannelAndSmallerDataLength)
{
	// rLen < dLen = getROutNo()	
	tint dLen = 6;
	tint rLen = 4;
	tint rUsedO = 1;
	tint rOutNo = 6;
#if defined(SINGLE_FLOAT_SAMPLE)
    sample_t chA[6] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
	sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	sample_t ext[6] = {2.0f, 3.0f, 4.0f, 5.0f, 0.0f, 0.0f};
#else
    sample_t chA[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
	sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	sample_t ext[6] = {2.0, 3.0, 4.0, 5.0, 0.0, 0.0};
#endif
	
	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(1));
	EXPECT_CALL(audio,setRUsedO(5)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==4);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith2ChannelsAndSmallerDataLength)
{
	// rLen < dLen = getROutNo()
	tint dLen = 3;
	tint rLen = 2;
	tint rUsedO = 0;
	tint rOutNo = 3;
#if defined(SINGLE_FLOAT_SAMPLE)
	sample_t chA[3] = {1.0f, 2.0f, 3.0f};
	sample_t chB[3] = {5.0f, 6.0f, 7.0f};
	sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	sample_t ext[6] = {1.0f, 5.0f, 2.0f, 6.0f, 0.0f, 0.0f};
#else
	sample_t chA[3] = {1.0, 2.0, 3.0};
	sample_t chB[3] = {5.0, 6.0, 7.0};
	sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	sample_t ext[6] = {1.0, 5.0, 2.0, 6.0, 0.0, 0.0};
#endif

	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getROut(1)).WillRepeatedly(Return(chB));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(audio,setRUsedO(2)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==2);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith1ChannelAndSmallerOutputLength)
{
	// getROutNo() < rLen = dLen
	tint dLen = 6;
	tint rLen = 6;
	tint rUsedO = 1;
	tint rOutNo = 4;
#if defined(SINGLE_FLOAT_SAMPLE)
    sample_t chA[6] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
	sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	sample_t ext[6] = {2.0f, 3.0f, 4.0f, 0.0f, 0.0f, 0.0f};
#else
    sample_t chA[6] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
	sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	sample_t ext[6] = {2.0, 3.0, 4.0, 0.0, 0.0, 0.0};
#endif

	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(1));
	EXPECT_CALL(audio,setRUsedO(4)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==3);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith2ChannelsAndSmallerOutputLength)
{
	// getROutNo() < rLen = dLen
	tint dLen = 3;
	tint rLen = 3;
	tint rUsedO = 0;
	tint rOutNo = 2;
#if defined(SINGLE_FLOAT_SAMPLE)
	sample_t chA[3] = {1.0f, 2.0f, 3.0f};
	sample_t chB[3] = {5.0f, 6.0f, 7.0f};
	sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	sample_t ext[6] = {1.0f, 5.0f, 2.0f, 6.0f, 0.0f, 0.0f};
#else
	sample_t chA[3] = {1.0, 2.0, 3.0};
	sample_t chB[3] = {5.0, 6.0, 7.0};
	sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	sample_t ext[6] = {1.0, 5.0, 2.0, 6.0, 0.0, 0.0};
#endif

	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getROut(1)).WillRepeatedly(Return(chB));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(audio,setRUsedO(2)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==2);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith2ChannelsAndSmallerOutputLengthAndOutputUsed)
{
	// getROutNo() < rLen = dLen
	tint dLen = 3;
	tint rLen = 3;
	tint rUsedO = 1;
	tint rOutNo = 2;
#if defined(SINGLE_FLOAT_SAMPLE)
	sample_t chA[3] = {1.0f, 2.0f, 3.0f};
	sample_t chB[3] = {5.0f, 6.0f, 7.0f};
	sample_t out[6] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	sample_t ext[6] = {2.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f};
#else
	sample_t chA[3] = {1.0, 2.0, 3.0};
	sample_t chB[3] = {5.0, 6.0, 7.0};
	sample_t out[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	sample_t ext[6] = {2.0, 6.0, 0.0, 0.0, 0.0, 0.0};
#endif

	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(chA));
	EXPECT_CALL(audio,getROut(1)).WillRepeatedly(Return(chB));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(audio,setRUsedO(2)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==1);
	
	for(int i=0;i<6;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleInterleaveOutputWith8Channels)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	sample_t ch1[3] = { 1.0f, 2.0f, 3.0f};
	sample_t ch2[3] = { 4.0f, 5.0f, 6.0f};
	sample_t ch3[3] = { 7.0f, 8.0f, 9.0f};
	sample_t ch4[3] = {10.0f,11.0f,12.0f};
	sample_t ch5[3] = {13.0f,14.0f,15.0f};
	sample_t ch6[3] = {16.0f,17.0f,18.0f};
	sample_t ch7[3] = {19.0f,20.0f,21.0f};
	sample_t ch8[3] = {22.0f,23.0f,24.0f};
	
	sample_t ext[24] = { 1.0f, 4.0f, 7.0f, 10.0f, 13.0f, 16.0f, 19.0f, 22.0f,
	                     2.0f, 5.0f, 8.0f, 11.0f, 14.0f, 17.0f, 20.0f, 23.0f,
	                     3.0f, 6.0f, 9.0f, 12.0f, 15.0f, 18.0f, 21.0f, 24.0f };
#else
	sample_t ch1[3] = { 1.0, 2.0, 3.0};
	sample_t ch2[3] = { 4.0, 5.0, 6.0};
	sample_t ch3[3] = { 7.0, 8.0, 9.0};
	sample_t ch4[3] = {10.0,11.0,12.0};
	sample_t ch5[3] = {13.0,14.0,15.0};
	sample_t ch6[3] = {16.0,17.0,18.0};
	sample_t ch7[3] = {19.0,20.0,21.0};
	sample_t ch8[3] = {22.0,23.0,24.0};
	
	sample_t ext[24] = { 1.0, 4.0, 7.0, 10.0, 13.0, 16.0, 19.0, 22.0,
	                     2.0, 5.0, 8.0, 11.0, 14.0, 17.0, 20.0, 23.0,
	                     3.0, 6.0, 9.0, 12.0, 15.0, 18.0, 21.0, 24.0 };
#endif

    sample_t out[24];
    ::memset(out,0,24 * sizeof(sample_t));
	
	tint dLen = 3;
	tint rLen = 3;
	tint rUsedO = 0;
	tint rOutNo = 3;
	
	AOBaseDecodeAndResampleInterleaveOutputChannelsTest audio;
	EXPECT_CALL(audio,getROut(0)).WillRepeatedly(Return(ch1));
	EXPECT_CALL(audio,getROut(1)).WillRepeatedly(Return(ch2));
	EXPECT_CALL(audio,getROut(2)).WillRepeatedly(Return(ch3));
	EXPECT_CALL(audio,getROut(3)).WillRepeatedly(Return(ch4));
	EXPECT_CALL(audio,getROut(4)).WillRepeatedly(Return(ch5));
	EXPECT_CALL(audio,getROut(5)).WillRepeatedly(Return(ch6));
	EXPECT_CALL(audio,getROut(6)).WillRepeatedly(Return(ch7));
	EXPECT_CALL(audio,getROut(7)).WillRepeatedly(Return(ch8));
	EXPECT_CALL(audio,getRUsedO()).WillRepeatedly(ReturnRef(rUsedO));
	EXPECT_CALL(audio,getROutNo()).WillRepeatedly(ReturnRef(rOutNo));
	EXPECT_CALL(audio,getNoInChannels()).WillRepeatedly(Return(8));
	EXPECT_CALL(audio,setRUsedO(3)).Times(1);
	
    EXPECT_TRUE(audio.testDecodeAndResampleInterleaveOutputChannels(out,dLen,rLen)==3);
	
	for(int i=0;i<24;i++)
	{
		EXPECT_DOUBLE_EQ(ext[i],out[i]);
	}
}

//-------------------------------------------------------------------------------------------

class AOBaseDecodeAndResampleSetCompletePartTiming : public AOBaseTest
{
	public:
		MOCK_METHOD0(resampleList,QList<AOResampleInfo>&());
		MOCK_CONST_METHOD0(getRCodecCompleteFlag,const bool&());
		MOCK_CONST_METHOD0(getFrequency,tint());
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleSetCompletePartTimingWhenCurrentSamplePartiallyCopied)
{
	const tfloat64 c_TOLERANCE = 0.0000001;

	engine::AData aData(44100,1,1);

	engine::RData rData(44100,1,1);
	engine::RData::Part& rPart = rData.nextPart();
	
	common::TimeStamp startT(2.0);
	common::TimeStamp endT(3.0);
	common::TimeStamp endP(2.5);
	
	AOResampleInfo dInfoA;
	dInfoA.start() = startT;
	dInfoA.end() = endT;
	dInfoA.complete() = false;
	
	QList<AOResampleInfo> resampleList;
	resampleList.append(dInfoA);
	
	AOBaseDecodeAndResampleSetCompletePartTiming audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(resampleList));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(44100));
	
	EXPECT_TRUE(audio.testDecodeAndResampleSetCompletePartTiming(aData,rPart,22050,44100));
	
	EXPECT_TRUE(rPart.length()==22050);
	EXPECT_TRUE(rPart.done());
	EXPECT_TRUE(rPart.start()==startT);
	EXPECT_NEAR(static_cast<tfloat64>(rPart.end()),static_cast<tfloat64>(endP),c_TOLERANCE);

	EXPECT_TRUE(resampleList.size()==1);
	EXPECT_NEAR(static_cast<tfloat64>(resampleList.at(0).start()),static_cast<tfloat64>(endP),c_TOLERANCE);
	EXPECT_TRUE(resampleList.at(0).end()==dInfoA.end());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleSetCompletePartTimingWhenEmptyListAndCodecComplete)
{
	engine::AData aData(44100,1,1);

	engine::RData rData(44100,1,1);
	engine::RData::Part& rPart = rData.nextPart();
	
	common::TimeStamp startT(2.0);
	common::TimeStamp endT(3.0);
	
	AOResampleInfo dInfoA;
	dInfoA.start() = startT;
	dInfoA.end() = endT;
	dInfoA.complete() = false;
	
	QList<AOResampleInfo> resampleList;
	resampleList.append(dInfoA);
	
	bool rComplete = true;
	
	AOBaseDecodeAndResampleSetCompletePartTiming audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(resampleList));
	EXPECT_CALL(audio,getRCodecCompleteFlag()).WillRepeatedly(ReturnRef(rComplete));
	
	EXPECT_FALSE(audio.testDecodeAndResampleSetCompletePartTiming(aData,rPart,44100,44100));
	
	EXPECT_TRUE(rPart.length()==44100);
	EXPECT_TRUE(rPart.done());
	EXPECT_TRUE(rPart.start()==startT);
	EXPECT_TRUE(rPart.end()==endT);
	
	EXPECT_TRUE(aData.end()!=endT);
	
	EXPECT_TRUE(resampleList.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleSetCompletePartTimingWhenNonEmptyListAndCodecComplete)
{
	engine::AData aData(44100,1,1);

	engine::RData rData(44100,1,1);
	engine::RData::Part& rPart = rData.nextPart();
	
	common::TimeStamp startT(2.0);
	common::TimeStamp endT(3.0);
	
	AOResampleInfo dInfoA;
	dInfoA.start() = startT;
	dInfoA.end() = endT;
	dInfoA.complete() = false;
	
	AOResampleInfo dInfoB;
	
	QList<AOResampleInfo> resampleList;
	resampleList.append(dInfoA);
	resampleList.append(dInfoB);
	
	bool rComplete = true;
	
	AOBaseDecodeAndResampleSetCompletePartTiming audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(resampleList));
	EXPECT_CALL(audio,getRCodecCompleteFlag()).WillRepeatedly(ReturnRef(rComplete));
	
	EXPECT_TRUE(audio.testDecodeAndResampleSetCompletePartTiming(aData,rPart,44100,44100));
	
	EXPECT_TRUE(rPart.length()==44100);
	EXPECT_TRUE(rPart.done());
	EXPECT_TRUE(rPart.start()==startT);
	EXPECT_TRUE(rPart.end()==endT);
	
	EXPECT_TRUE(aData.end()!=endT);
	
	EXPECT_TRUE(resampleList.size()==1);
	EXPECT_TRUE(resampleList.at(0).start()==dInfoB.start());
	EXPECT_TRUE(resampleList.at(0).end()==dInfoB.end());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleSetCompletePartTimingWhenEmptyListAndCodecNotComplete)
{
	engine::AData aData(44100,1,1);

	engine::RData rData(44100,1,1);
	engine::RData::Part& rPart = rData.nextPart();
	
	common::TimeStamp startT(2.0);
	common::TimeStamp endT(3.0);
	
	AOResampleInfo dInfoA;
	dInfoA.start() = startT;
	dInfoA.end() = endT;
	dInfoA.complete() = false;
	
	QList<AOResampleInfo> resampleList;
	resampleList.append(dInfoA);
	
	bool rComplete = false;
	
	AOBaseDecodeAndResampleSetCompletePartTiming audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(resampleList));
	EXPECT_CALL(audio,getRCodecCompleteFlag()).WillRepeatedly(ReturnRef(rComplete));
	
	EXPECT_TRUE(audio.testDecodeAndResampleSetCompletePartTiming(aData,rPart,44100,44100));
	
	EXPECT_TRUE(rPart.length()==44100);
	EXPECT_TRUE(rPart.done());
	EXPECT_TRUE(rPart.start()==startT);
	EXPECT_TRUE(rPart.end()==endT);
	
	EXPECT_TRUE(aData.end()!=endT);
	
	EXPECT_TRUE(resampleList.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleSetCompletePartTimingWhenCodecCompleteViaResampling)
{
	engine::AData aData(44100,1,1);

	engine::RData rData(44100,1,1);
	engine::RData::Part& rPart = rData.nextPart();
	
	common::TimeStamp startT(2.0);
	common::TimeStamp endT(3.0);
	
	AOResampleInfo dInfoA;
	dInfoA.start() = startT;
	dInfoA.end() = endT;
	dInfoA.complete() = true;
	
	AOResampleInfo dInfoB;
	
	QList<AOResampleInfo> resampleList;
	resampleList.append(dInfoA);
	resampleList.append(dInfoB);
		
	AOBaseDecodeAndResampleSetCompletePartTiming audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(resampleList));
	
	EXPECT_FALSE(audio.testDecodeAndResampleSetCompletePartTiming(aData,rPart,44100,44100));
	
	EXPECT_TRUE(rPart.length()==44100);
	EXPECT_TRUE(rPart.done());
	EXPECT_TRUE(rPart.start()==startT);
	EXPECT_TRUE(rPart.end()==endT);
	
	EXPECT_TRUE(aData.end()==endT);
	
	EXPECT_TRUE(resampleList.size()==1);
	EXPECT_TRUE(resampleList.at(0).start()==dInfoB.start());
	EXPECT_TRUE(resampleList.at(0).end()==dInfoB.end());
}

//-------------------------------------------------------------------------------------------

class AOBaseDecodeAndResampleCalculateOutputLengthTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(resampleList,QList<AOResampleInfo>&());
		MOCK_CONST_METHOD0(getFrequency,tint());
		MOCK_CONST_METHOD0(getROutDrift,const tfloat64&());
		MOCK_METHOD1(setROutDrift,void(tfloat64 drift));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleCalculateOutputLengthNoDrift)
{
	// -> 10 samples per second
	// -> 20 samples in time period
	// -> 0.1s is space between samples

	const tfloat64 c_TOLERANCE = 0.00000001;

	tfloat64 rOutDrift = 0.0;
	common::TimeStamp startT(3.0);
	common::TimeStamp endT(5.0);
	
	AOResampleInfo dInfo;
	dInfo.start() = startT;
	dInfo.end() = endT;
	
	QList<AOResampleInfo> infoList;
	infoList.append(dInfo);
	
	AOBaseDecodeAndResampleCalculateOutputLengthTest audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(infoList));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getROutDrift()).Times(1).WillOnce(ReturnRef(rOutDrift));
	EXPECT_CALL(audio,setROutDrift(A<tfloat64>())).Times(1).WillOnce(SaveArg<0>(&rOutDrift));
	
	tint dLen = audio.testDecodeAndResampleCalculateOutputLength();
	
	EXPECT_EQ(20,dLen);
	EXPECT_NEAR(0.0,rOutDrift,c_TOLERANCE);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleCalculateOutputLengthUpperHalfDiffNoDriftCorrection)
{
	const tfloat64 c_TOLERANCE = 0.00000001;
	
	tfloat64 rOutDrift = 0.0;
	common::TimeStamp startT(3.0);
	common::TimeStamp endT(5.06);
	
	AOResampleInfo dInfo;
	dInfo.start() = startT;
	dInfo.end() = endT;
	
	QList<AOResampleInfo> infoList;
	infoList.append(dInfo);
	
	// dLenActual = (5.06 - 3.00) * 10 = 2.06 * 10 = 20.6
	// dLen = 20
	// diff = 20.6 - 20 = 0.6
	// Hence dLen = 20 + 1 = 21
	// rDrift = 0.0 - (21.0 - 20.6) = 0.0 - 0.4 = -0.4
	
	AOBaseDecodeAndResampleCalculateOutputLengthTest audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(infoList));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getROutDrift()).Times(1).WillOnce(ReturnRef(rOutDrift));
	EXPECT_CALL(audio,setROutDrift(A<tfloat64>())).Times(1).WillOnce(SaveArg<0>(&rOutDrift));
	
	tint dLen = audio.testDecodeAndResampleCalculateOutputLength();
	
	EXPECT_EQ(21,dLen);
	EXPECT_NEAR(-0.4,rOutDrift,c_TOLERANCE);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleCalculateOutputLengthUpperHalfDiffWithPositiveDriftCorrection)
{
	// rDrift >= 1.0
	const tfloat64 c_TOLERANCE = 0.00000001;
	
	tfloat64 rOutDrift = 1.6;
	common::TimeStamp startT(3.0);
	common::TimeStamp endT(5.06);
	
	AOResampleInfo dInfo;
	dInfo.start() = startT;
	dInfo.end() = endT;
	
	QList<AOResampleInfo> infoList;
	infoList.append(dInfo);
	
	// dLenActual = (5.06 - 3.00) * 10 = 2.06 * 10 = 20.6
	// dLen = 20
	// diff = 20.6 - 20 = 0.6
	// Hence dLen = 20 + 1 = 21
	// rDrift = 1.6 - (21.0 - 20.6) = 1.6 - 0.4 = 1.2
	// Hence rDrift = 1.2 - 1.0 = 0.2, dLen = 21 + 1 = 22
	
	AOBaseDecodeAndResampleCalculateOutputLengthTest audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(infoList));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getROutDrift()).Times(1).WillOnce(ReturnRef(rOutDrift));
	EXPECT_CALL(audio,setROutDrift(A<tfloat64>())).Times(1).WillOnce(SaveArg<0>(&rOutDrift));
	
	tint dLen = audio.testDecodeAndResampleCalculateOutputLength();
	
	EXPECT_EQ(22,dLen);
	EXPECT_NEAR(0.2,rOutDrift,c_TOLERANCE);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleCalculateOutputLengthUpperHalfDiffWithNegativeDriftCorrection)
{
	// rDrift >= -1.0
	const tfloat64 c_TOLERANCE = 0.00000001;
	
	tfloat64 rOutDrift = -0.8;
	common::TimeStamp startT(3.0);
	common::TimeStamp endT(5.06);
	
	AOResampleInfo dInfo;
	dInfo.start() = startT;
	dInfo.end() = endT;
	
	QList<AOResampleInfo> infoList;
	infoList.append(dInfo);
	
	// dLenActual = (5.06 - 3.00) * 10 = 2.06 * 10 = 20.6
	// dLen = 20
	// diff = 20.6 - 20 = 0.6
	// Hence dLen = 20 + 1 = 21
	// rDrift = -0.8 - (21.0 - 20.6) = -0.8 - 0.4 = -1.2
	// Hence rDrift = -1.2 + 1.0 = -0.2, dLen = 21 - 1 = 20
	
	AOBaseDecodeAndResampleCalculateOutputLengthTest audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(infoList));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getROutDrift()).Times(1).WillOnce(ReturnRef(rOutDrift));
	EXPECT_CALL(audio,setROutDrift(A<tfloat64>())).Times(1).WillOnce(SaveArg<0>(&rOutDrift));
	
	tint dLen = audio.testDecodeAndResampleCalculateOutputLength();
	
	EXPECT_EQ(20,dLen);
	EXPECT_NEAR(-0.2,rOutDrift,c_TOLERANCE);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleCalculateOutputLengthLowerHalfDiffNoDriftCorrection)
{
	const tfloat64 c_TOLERANCE = 0.00000001;
	
	tfloat64 rOutDrift = 0.0;
	common::TimeStamp startT(3.0);
	common::TimeStamp endT(5.04);
	
	AOResampleInfo dInfo;
	dInfo.start() = startT;
	dInfo.end() = endT;
	
	QList<AOResampleInfo> infoList;
	infoList.append(dInfo);
	
	// dLenActual = (5.04 - 3.00) * 10 = 2.04 * 10 = 20.4
	// dLen = 20
	// diff = 20.4 - 20 = 0.4
	// Hence dLen = 20
	// rDrift = 0.0 + (20.4 - 20.0) = 0.4
	
	AOBaseDecodeAndResampleCalculateOutputLengthTest audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(infoList));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getROutDrift()).Times(1).WillOnce(ReturnRef(rOutDrift));
	EXPECT_CALL(audio,setROutDrift(A<tfloat64>())).Times(1).WillOnce(SaveArg<0>(&rOutDrift));
	
	tint dLen = audio.testDecodeAndResampleCalculateOutputLength();
	
	EXPECT_EQ(20,dLen);
	EXPECT_NEAR(0.4,rOutDrift,c_TOLERANCE);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleCalculateOutputLengthLowerHalfDiffWithPositiveDriftCorrection)
{
	// rDrift >= 1.0
	const tfloat64 c_TOLERANCE = 0.00000001;
	
	tfloat64 rOutDrift = 0.8;
	common::TimeStamp startT(3.0);
	common::TimeStamp endT(5.04);
	
	AOResampleInfo dInfo;
	dInfo.start() = startT;
	dInfo.end() = endT;
	
	QList<AOResampleInfo> infoList;
	infoList.append(dInfo);
	
	// dLenActual = (5.04 - 3.00) * 10 = 2.04 * 10 = 20.4
	// dLen = 20
	// diff = 20.4 - 20 = 0.4
	// Hence dLen = 20
	// rDrift = 0.8 + (20.4 - 20.0) = 1.2
	// Hence rDrift = 1.2 - 1.0 = 0.2, dLen = 20 + 1 = 21
	
	AOBaseDecodeAndResampleCalculateOutputLengthTest audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(infoList));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getROutDrift()).Times(1).WillOnce(ReturnRef(rOutDrift));
	EXPECT_CALL(audio,setROutDrift(A<tfloat64>())).Times(1).WillOnce(SaveArg<0>(&rOutDrift));
	
	tint dLen = audio.testDecodeAndResampleCalculateOutputLength();
	
	EXPECT_EQ(21,dLen);
	EXPECT_NEAR(0.2,rOutDrift,c_TOLERANCE);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,decodeAndResampleCalculateOutputLengthLowerHalfDiffWithNegativeDriftCorrection)
{
	// rDrift >= -1.0
	const tfloat64 c_TOLERANCE = 0.00000001;
	
	tfloat64 rOutDrift = -1.6;
	common::TimeStamp startT(3.0);
	common::TimeStamp endT(5.04);
	
	AOResampleInfo dInfo;
	dInfo.start() = startT;
	dInfo.end() = endT;
	
	QList<AOResampleInfo> infoList;
	infoList.append(dInfo);
	
	// dLenActual = (5.04 - 3.00) * 10 = 2.04 * 10 = 20.4
	// dLen = 20
	// diff = 20.4 - 20 = 0.4
	// Hence dLen = 20
	// rDrift = -1.6 + (20.4 - 20.0) = -1.6 + 0.4 = -1.2
	// Hence rDrift = -1.2 + 1.0 = -0.2, dLen = 20 - 1 = 19
	
	AOBaseDecodeAndResampleCalculateOutputLengthTest audio;
	EXPECT_CALL(audio,resampleList()).WillRepeatedly(ReturnRef(infoList));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getROutDrift()).Times(1).WillOnce(ReturnRef(rOutDrift));
	EXPECT_CALL(audio,setROutDrift(A<tfloat64>())).Times(1).WillOnce(SaveArg<0>(&rOutDrift));
	
	tint dLen = audio.testDecodeAndResampleCalculateOutputLength();
	
	EXPECT_EQ(19,dLen);
	EXPECT_NEAR(-0.2,rOutDrift,c_TOLERANCE);
}

//-------------------------------------------------------------------------------------------

class AOBasePausePlaybackTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getState,AOBase::States());
		MOCK_METHOD1(setState,void(AOBase::States s));
		MOCK_METHOD1(setPauseTime,void(const common::TimeStamp& t));
		MOCK_METHOD1(setPauseAudioFlag,void(bool f));
		MOCK_METHOD0(currentPlayTime,common::TimeStamp());
		MOCK_METHOD0(closeAudio,void());
		MOCK_METHOD0(stopAudioDevice,void());
		MOCK_METHOD0(emitOnPause,void());
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,pausePlaybackWithoutShutdownWithoutSignalAlreadyInPauseState)
{
	common::TimeStamp playT(3.0);
	
	AOBasePausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	
	EXPECT_TRUE(audio.testPausePlayback(false,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,pausePlaybackWithoutShutdownWithSignalAlreadyInStopState)
{
	common::TimeStamp playT(3.0);
	
	AOBasePausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_stateStop));
	
	EXPECT_TRUE(audio.testPausePlayback(false,true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,pausePlaybackWithShutdownWithoutSignalAlreadyInPauseState)
{
	common::TimeStamp playT(3.0);
	
	AOBasePausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	
	EXPECT_TRUE(audio.testPausePlayback(true,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,pausePlaybackWithShutdownWithSignalAlreadyInStopState)
{
	common::TimeStamp playT(3.0);
	
	AOBasePausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_stateStop));
	
	EXPECT_TRUE(audio.testPausePlayback(true,true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,pausePlaybackWithoutShutdownWithoutSignalInPlayState)
{
	common::TimeStamp playT(3.0);
	
	AOBasePausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePlay));
	EXPECT_CALL(audio,currentPlayTime()).Times(1).WillOnce(Return(playT));
	EXPECT_CALL(audio,setPauseTime(Eq(playT))).Times(1);
	EXPECT_CALL(audio,setPauseAudioFlag(true)).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_statePause)).Times(1);
	EXPECT_CALL(audio,stopAudioDevice()).Times(1);
	
	EXPECT_TRUE(audio.testPausePlayback(false,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,pausePlaybackWithoutShutdownWithSignalInPlayState)
{
	common::TimeStamp playT(3.0);
	
	AOBasePausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePlay));
	EXPECT_CALL(audio,currentPlayTime()).Times(1).WillOnce(Return(playT));
	EXPECT_CALL(audio,setPauseTime(Eq(playT))).Times(1);
	EXPECT_CALL(audio,setPauseAudioFlag(true)).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_statePause)).Times(1);
	EXPECT_CALL(audio,stopAudioDevice()).Times(1);
	EXPECT_CALL(audio,emitOnPause()).Times(1);
	
	EXPECT_TRUE(audio.testPausePlayback(false,true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,pausePlaybackWithShutdownWithoutSignalInPlayState)
{
	common::TimeStamp playT(3.0);
	
	AOBasePausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePlay));
	EXPECT_CALL(audio,currentPlayTime()).Times(1).WillOnce(Return(playT));
	EXPECT_CALL(audio,setPauseTime(Eq(playT))).Times(1);
	EXPECT_CALL(audio,setPauseAudioFlag(true)).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_statePause)).Times(1);
	EXPECT_CALL(audio,stopAudioDevice()).Times(1);
	EXPECT_CALL(audio,closeAudio()).Times(1);
	
	EXPECT_TRUE(audio.testPausePlayback(true,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,pausePlaybackWithShutdownWithSignalInPlayState)
{
	common::TimeStamp playT(3.0);
	
	AOBasePausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePlay));
	EXPECT_CALL(audio,currentPlayTime()).Times(1).WillOnce(Return(playT));
	EXPECT_CALL(audio,setPauseTime(Eq(playT))).Times(1);
	EXPECT_CALL(audio,setPauseAudioFlag(true)).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_statePause)).Times(1);
	EXPECT_CALL(audio,stopAudioDevice()).Times(1);
	EXPECT_CALL(audio,closeAudio()).Times(1);
	EXPECT_CALL(audio,emitOnPause()).Times(1);
	
	EXPECT_TRUE(audio.testPausePlayback(true,true));
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getState,AOBase::States());
		MOCK_CONST_METHOD0(getCodecState,AOBase::CodecState());
		MOCK_METHOD0(flushCyclicBuffer,void());
		MOCK_METHOD0(unpausePlaybackCodecStateNoPlay,bool());
		MOCK_METHOD0(unpausePlaybackCodecStateSingle,bool());
		MOCK_METHOD0(unpausePlaybackCodecStateFinish,bool());
		MOCK_METHOD1(unpausePlaybackProcess,bool(bool signalFlag));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackInPlayState)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePlay));
	
	EXPECT_TRUE(audio.testUnpausePlayback(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackInStopState)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_stateStop));
	
	EXPECT_TRUE(audio.testUnpausePlayback(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackInPrebufferState)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePreBuffer));
	
	EXPECT_TRUE(audio.testUnpausePlayback(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackWhenPausedAndCodecNoPlayAndNoProcess)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,getCodecState()).WillRepeatedly(Return(AOBase::e_codecNoPlay));
	EXPECT_CALL(audio,flushCyclicBuffer()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateNoPlay()).Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testUnpausePlayback(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackWhenPausedAndCodecNoPlayAndCallProcess)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,getCodecState()).WillRepeatedly(Return(AOBase::e_codecNoPlay));
	EXPECT_CALL(audio,flushCyclicBuffer()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateNoPlay()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,unpausePlaybackProcess(true)).Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testUnpausePlayback(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackWhenPausedAndCodecSingleAndNoProcess)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,getCodecState()).WillRepeatedly(Return(AOBase::e_codecSingle));
	EXPECT_CALL(audio,flushCyclicBuffer()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingle()).Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testUnpausePlayback(false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackWhenPausedAndCodecSingleAndCallProcess)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,getCodecState()).WillRepeatedly(Return(AOBase::e_codecSingle));
	EXPECT_CALL(audio,flushCyclicBuffer()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingle()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,unpausePlaybackProcess(false)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testUnpausePlayback(false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackWhenPausedAndCodecNextQueuedAndNoProcess)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,getCodecState()).WillRepeatedly(Return(AOBase::e_codecNextQueued));
	EXPECT_CALL(audio,flushCyclicBuffer()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingle()).Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testUnpausePlayback(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackWhenPausedAndCodecNextQueuedAndCallProcess)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,getCodecState()).WillRepeatedly(Return(AOBase::e_codecNextQueued));
	EXPECT_CALL(audio,flushCyclicBuffer()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingle()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,unpausePlaybackProcess(true)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testUnpausePlayback(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackWhenPausedAndCodecSingleFinishUnsuccessful)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,getCodecState()).WillRepeatedly(Return(AOBase::e_codecSingleFinish));
	EXPECT_CALL(audio,flushCyclicBuffer()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateFinish()).Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testUnpausePlayback(false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackWhenPausedAndCodecSingleFinishSuccessfully)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,getCodecState()).WillRepeatedly(Return(AOBase::e_codecSingleFinish));
	EXPECT_CALL(audio,flushCyclicBuffer()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateFinish()).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testUnpausePlayback(false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackWhenPausedAndCodecCurrentFinishUnsuccessful)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,getCodecState()).WillRepeatedly(Return(AOBase::e_codecCurrentFinish));
	EXPECT_CALL(audio,flushCyclicBuffer()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateFinish()).Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testUnpausePlayback(false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackWhenPausedAndCodecCurrentFinishSuccessfully)
{
	AOBaseUnpausePlaybackTest audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(AOBase::e_statePause));
	EXPECT_CALL(audio,getCodecState()).WillRepeatedly(Return(AOBase::e_codecCurrentFinish));
	EXPECT_CALL(audio,flushCyclicBuffer()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateFinish()).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testUnpausePlayback(false));
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackCodecStateSingle : public AOBaseTest
{
	public:
		MOCK_METHOD0(unpausePlaybackCodecStateSingleTiming,bool());
		MOCK_METHOD1(unpausePlaybackCodecStateSinglePlayState,void(bool process));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleWhenProcessFromTiming)
{
	AOBaseUnpausePlaybackCodecStateSingle audio;
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleTiming()).Times(1)
		.WillOnce(Return(true));
	EXPECT_CALL(audio,unpausePlaybackCodecStateSinglePlayState(true)).Times(1);
	
	EXPECT_TRUE(audio.testUnpausePlaybackCodecStateSingle());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleWhenNoProcessFromTiming)
{
	AOBaseUnpausePlaybackCodecStateSingle audio;
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleTiming()).Times(1)
		.WillOnce(Return(false));
	EXPECT_CALL(audio,unpausePlaybackCodecStateSinglePlayState(false)).Times(1);
	
	EXPECT_FALSE(audio.testUnpausePlaybackCodecStateSingle());
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackCodecStateSingleTimingTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(unpausePlaybackCodecStateSingleSeekToPauseTime,bool());
		MOCK_METHOD1(unpausePlaybackCodecStateSingleBufferOnRemote,bool(bool process));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleTimingPauseFailRemoteFail)
{
	AOBaseUnpausePlaybackCodecStateSingleTimingTest audio;
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleSeekToPauseTime()).Times(1)
		.WillOnce(Return(false));
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleBufferOnRemote(false)).Times(1)
		.WillOnce(Return(false));
		
	EXPECT_FALSE(audio.testUnpausePlaybackCodecStateSingleTiming());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleTimingPauseSuccessRemoteFail)
{
	AOBaseUnpausePlaybackCodecStateSingleTimingTest audio;
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleSeekToPauseTime()).Times(1)
		.WillOnce(Return(true));
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleBufferOnRemote(true)).Times(1)
		.WillOnce(Return(false));
		
	EXPECT_FALSE(audio.testUnpausePlaybackCodecStateSingleTiming());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleTimingPauseFailRemoteSuccess)
{
	AOBaseUnpausePlaybackCodecStateSingleTimingTest audio;
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleSeekToPauseTime()).Times(1)
		.WillOnce(Return(false));
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleBufferOnRemote(false)).Times(1)
		.WillOnce(Return(true));
		
	EXPECT_TRUE(audio.testUnpausePlaybackCodecStateSingleTiming());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleTimingPauseSuccessRemoteSuccess)
{
	AOBaseUnpausePlaybackCodecStateSingleTimingTest audio;
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleSeekToPauseTime()).Times(1)
		.WillOnce(Return(true));
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleBufferOnRemote(true)).Times(1)
		.WillOnce(Return(true));
		
	EXPECT_TRUE(audio.testUnpausePlaybackCodecStateSingleTiming());
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackCodecStateSingleSeekToPauseTimeTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getCodec,engine::Codec *());
		MOCK_CONST_METHOD0(getPauseTime,common::TimeStamp());
		MOCK_METHOD1(stopCodec,void(bool eFlag));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleSeekToPauseTimeCannotSeek)
{
	common::TimeStamp pauseT(1.0);

	CodecMock codec;
	EXPECT_CALL(codec,isSeek()).WillRepeatedly(Return(false));
	
	AOBaseUnpausePlaybackCodecStateSingleSeekToPauseTimeTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	
	EXPECT_TRUE(audio.testUnpausePlaybackCodecStateSingleSeekToPauseTime());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleSeekToPauseTimeSeekSuccess)
{
	common::TimeStamp pauseT(1.0);

	CodecMock codec;
	EXPECT_CALL(codec,isSeek()).WillRepeatedly(Return(true));
	EXPECT_CALL(codec,seek(Eq(pauseT))).Times(1).WillOnce(Return(true));
	
	AOBaseUnpausePlaybackCodecStateSingleSeekToPauseTimeTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,getPauseTime()).Times(1).WillOnce(Return(pauseT));
	
	EXPECT_TRUE(audio.testUnpausePlaybackCodecStateSingleSeekToPauseTime());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleSeekToPauseTimeSeekFails)
{
	common::TimeStamp pauseT(1.0);

	CodecMock codec;
	EXPECT_CALL(codec,isSeek()).WillRepeatedly(Return(true));
	EXPECT_CALL(codec,seek(Eq(pauseT))).Times(1).WillOnce(Return(false));
	
	AOBaseUnpausePlaybackCodecStateSingleSeekToPauseTimeTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,getPauseTime()).Times(1).WillOnce(Return(pauseT));
	EXPECT_CALL(audio,stopCodec(true)).Times(1);
	
	EXPECT_FALSE(audio.testUnpausePlaybackCodecStateSingleSeekToPauseTime());
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackCodecStateSingleBufferOnRemote : public AOBaseTest
{
	public:
		MOCK_METHOD0(getCodec,engine::Codec *());
		MOCK_METHOD0(getRemoteTimeSync,common::TimeStamp());
		MOCK_METHOD1(setState,void(AOBase::States s));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateBufferOnRemoteWhenNoProcess)
{
	AOBaseUnpausePlaybackCodecStateSingleBufferOnRemote audio;
	
	EXPECT_FALSE(audio.testUnpausePlaybackCodecStateSingleBufferOnRemote(false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateBufferOnRemoteWhenCodecLocal)
{
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).WillRepeatedly(Return(false));
	
	AOBaseUnpausePlaybackCodecStateSingleBufferOnRemote audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	
	EXPECT_TRUE(audio.testUnpausePlaybackCodecStateSingleBufferOnRemote(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateBufferOnRemoteWhenCodecRemote)
{
	common::TimeStamp bT(2.0);
	
	CodecMock codec;
	EXPECT_CALL(codec,isRemote()).WillRepeatedly(Return(true));
	EXPECT_CALL(codec,buffer(Eq(bT))).Times(1);
	
	AOBaseUnpausePlaybackCodecStateSingleBufferOnRemote audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,getRemoteTimeSync()).Times(1).WillOnce(Return(bT));
	EXPECT_CALL(audio,setState(AOBase::e_statePreBuffer)).Times(1);
	
	EXPECT_FALSE(audio.testUnpausePlaybackCodecStateSingleBufferOnRemote(true));
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackCodecStateSinglePlayToCrossfadeStateTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getNextCodec,engine::Codec *());
		MOCK_CONST_METHOD0(getPauseTime,common::TimeStamp());
		MOCK_CONST_METHOD0(getCrossFadeTime,const common::TimeStamp&());
		MOCK_METHOD0(stopNextCodec,void());
		MOCK_METHOD1(setNextOutState,void(tint s));
		MOCK_METHOD1(setState,void(AOBase::States s));
		MOCK_METHOD1(unpausePlaybackCodecStateSingleSetPlayState,void(bool process));
		MOCK_METHOD0(getRemoteTimeSync,common::TimeStamp());
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSinglePlayToCrossfadeStateNoSeekNoRemote)
{
	common::TimeStamp pauseT(5.0),crossT(3.0);
	common::TimeStamp bufferT(7.0);

	CodecMock codec;
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	
	AOBaseUnpausePlaybackCodecStateSinglePlayToCrossfadeStateTest audio;
	EXPECT_CALL(audio,getNextCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,setNextOutState(1)).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_stateCrossFade)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSinglePlayToCrossfadeState();

}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSinglePlayToCrossfadeStateSeekSuccessNoRemote)
{
	common::TimeStamp pauseT(5.0),crossT(3.0);
	common::TimeStamp seekT = pauseT - crossT;
	common::TimeStamp bufferT(7.0);

	CodecMock codec;
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,seek(Eq(seekT))).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(false));
	
	AOBaseUnpausePlaybackCodecStateSinglePlayToCrossfadeStateTest audio;
	EXPECT_CALL(audio,getNextCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,getPauseTime()).Times(1).WillOnce(Return(pauseT));
	EXPECT_CALL(audio,getCrossFadeTime()).Times(1).WillOnce(ReturnRef(crossT));
	EXPECT_CALL(audio,setNextOutState(1)).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_stateCrossFade)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSinglePlayToCrossfadeState();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSinglePlayToCrossfadeStateNoSeekRemote)
{
	common::TimeStamp bufferT(7.0);

	CodecMock codec;
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,buffer(bufferT)).Times(1);
	
	AOBaseUnpausePlaybackCodecStateSinglePlayToCrossfadeStateTest audio;
	EXPECT_CALL(audio,getNextCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,getRemoteTimeSync()).Times(1).WillOnce(Return(bufferT));
	EXPECT_CALL(audio,setNextOutState(1)).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_stateCrossFade)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSinglePlayToCrossfadeState();

}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSinglePlayToCrossfadeStateSeekSuccessRemote)
{
	common::TimeStamp pauseT(5.0),crossT(3.0);
	common::TimeStamp seekT = pauseT - crossT;
	common::TimeStamp bufferT(7.0);

	CodecMock codec;
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,seek(Eq(seekT))).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,isRemote()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,buffer(bufferT)).Times(1);
	
	AOBaseUnpausePlaybackCodecStateSinglePlayToCrossfadeStateTest audio;
	EXPECT_CALL(audio,getNextCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,getPauseTime()).Times(1).WillOnce(Return(pauseT));
	EXPECT_CALL(audio,getCrossFadeTime()).Times(1).WillOnce(ReturnRef(crossT));
	EXPECT_CALL(audio,getRemoteTimeSync()).Times(1).WillOnce(Return(bufferT));
	EXPECT_CALL(audio,setNextOutState(1)).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_stateCrossFade)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSinglePlayToCrossfadeState();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSinglePlayToCrossfadeStateSeekFailNoRemote)
{
	common::TimeStamp pauseT(5.0),crossT(3.0);
	common::TimeStamp seekT = pauseT - crossT;

	CodecMock codec;
	EXPECT_CALL(codec,isSeek()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codec,seek(Eq(seekT))).Times(1).WillOnce(Return(false));
	
	AOBaseUnpausePlaybackCodecStateSinglePlayToCrossfadeStateTest audio;
	EXPECT_CALL(audio,getNextCodec()).Times(3)
		.WillOnce(Return(&codec))
		.WillOnce(Return(&codec))
		.WillOnce(Return((engine::Codec *)0));	
	EXPECT_CALL(audio,getPauseTime()).Times(1).WillOnce(Return(pauseT));
	EXPECT_CALL(audio,getCrossFadeTime()).Times(1).WillOnce(ReturnRef(crossT));
	EXPECT_CALL(audio,stopNextCodec()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleSetPlayState(true)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSinglePlayToCrossfadeState();
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackCodecStateSingleSetPlayStateTest : public AOBaseTest
{
	public:
		MOCK_METHOD1(setNextOutState,void(tint s));
		MOCK_METHOD1(setState,void(AOBase::States s));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleSetPlayStateDoProcess)
{
	AOBaseUnpausePlaybackCodecStateSingleSetPlayStateTest audio;
	EXPECT_CALL(audio,setNextOutState(0)).Times(1);
	EXPECT_CALL(audio,setState(AOBase::e_statePlay)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSingleSetPlayState(true);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSingleSetPlayStateNoProcess)
{
	AOBaseUnpausePlaybackCodecStateSingleSetPlayStateTest audio;
	EXPECT_CALL(audio,setNextOutState(0)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSingleSetPlayState(false);
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackCodecStateSinglePlayStateTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getNextCodec,engine::Codec *());
		MOCK_METHOD0(getCrossFadeFlag,bool());
		MOCK_CONST_METHOD0(getPauseTime,common::TimeStamp());
		MOCK_CONST_METHOD0(getCrossFadeTime,const common::TimeStamp&());
		MOCK_METHOD0(unpausePlaybackCodecStateSinglePlayToCrossfadeState,void());
		MOCK_METHOD1(unpausePlaybackCodecStateSingleSetPlayState,void(bool process));

};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSinglePlayStateNoProcess)
{
	AOBaseUnpausePlaybackCodecStateSinglePlayStateTest audio;
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleSetPlayState(false)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSinglePlayState(false);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSinglePlayStateNoNextCodec)
{
	AOBaseUnpausePlaybackCodecStateSinglePlayStateTest audio;
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return((engine::Codec *)0));
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleSetPlayState(true)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSinglePlayState(true);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSinglePlayStateNoCrossFadeFlag)
{
	CodecMock codec;
	
	AOBaseUnpausePlaybackCodecStateSinglePlayStateTest audio;
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCrossFadeFlag()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleSetPlayState(true)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSinglePlayState(true);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSinglePlayStateCrossFadeAndPauseBeforeFade)
{
	common::TimeStamp crossT(3.0),pauseT(2.0);
	CodecMock codec;
	
	AOBaseUnpausePlaybackCodecStateSinglePlayStateTest audio;
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCrossFadeFlag()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,getPauseTime()).Times(1).WillOnce(Return(pauseT));
	EXPECT_CALL(audio,getCrossFadeTime()).Times(1).WillOnce(ReturnRef(crossT));
	EXPECT_CALL(audio,unpausePlaybackCodecStateSingleSetPlayState(true)).Times(1);
	
	audio.testUnpausePlaybackCodecStateSinglePlayState(true);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateSinglePlayStateCrossFadeAndAfterBeforeFade)
{
	common::TimeStamp crossT(3.0),pauseT(5.0);
	CodecMock codec;
	
	AOBaseUnpausePlaybackCodecStateSinglePlayStateTest audio;
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getCrossFadeFlag()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,getPauseTime()).Times(1).WillOnce(Return(pauseT));
	EXPECT_CALL(audio,getCrossFadeTime()).Times(1).WillOnce(ReturnRef(crossT));
	EXPECT_CALL(audio,unpausePlaybackCodecStateSinglePlayToCrossfadeState()).Times(1);
	
	audio.testUnpausePlaybackCodecStateSinglePlayState(true);
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackCodecStateFinishTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getCodec,engine::Codec *());
		MOCK_METHOD1(setCodec,void(engine::Codec *c));
		MOCK_METHOD0(getNextCodec,engine::Codec *());
		MOCK_METHOD1(setNextCodec,void(engine::Codec *c));
		MOCK_METHOD0(getCompleteCodec,engine::Codec *());
		MOCK_METHOD1(setCompleteCodec,void(engine::Codec *c));
		MOCK_METHOD1(setCodecState,void(AOBase::CodecState s));
		MOCK_METHOD1(setTrackTimeStateFlag,void(bool flag));
		MOCK_METHOD0(stopNextCodec,void());
		MOCK_METHOD0(calcNextCodecTime,void());
		MOCK_METHOD1(unpausePlayback,bool(bool signalFlag));
		MOCK_METHOD1(stopCodec,void(bool eFlag));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateFinishWhenCompleteCannotSeek)
{
	CodecMock codecComplete;
	EXPECT_CALL(codecComplete,isSeek()).Times(1).WillOnce(Return(false));
	
	AOBaseUnpausePlaybackCodecStateFinishTest audio;
	EXPECT_CALL(audio,getCompleteCodec()).WillRepeatedly(Return(&codecComplete));
	EXPECT_CALL(audio,stopCodec(true)).Times(1);
	
	EXPECT_FALSE(audio.testUnpausePlaybackCodecStateFinish());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateFinishWhenNoNext)
{
	CodecMock codecComplete;
	EXPECT_CALL(codecComplete,isSeek()).Times(1).WillOnce(Return(true));
	
	AOBaseUnpausePlaybackCodecStateFinishTest audio;
	EXPECT_CALL(audio,getCompleteCodec()).Times(2).WillOnce(Return(&codecComplete)).WillOnce(Return(&codecComplete));
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return((engine::Codec*)0));
	EXPECT_CALL(audio,setNextCodec((engine::Codec*)0)).Times(1);
	EXPECT_CALL(audio,setCodec(&codecComplete)).Times(1);
	EXPECT_CALL(audio,setCompleteCodec((engine::Codec *)0)).Times(1);
	EXPECT_CALL(audio,getNextCodec()).WillRepeatedly(Return((engine::Codec*)0));
	EXPECT_CALL(audio,setCodecState(AOBase::e_codecSingle)).Times(1);
	EXPECT_CALL(audio,setTrackTimeStateFlag(true)).Times(1);
	EXPECT_CALL(audio,calcNextCodecTime()).Times(1);
	EXPECT_CALL(audio,unpausePlayback(true)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testUnpausePlaybackCodecStateFinish());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateFinishWhenNextCannotSeek)
{
	CodecMock codecComplete;
	EXPECT_CALL(codecComplete,isSeek()).Times(1).WillOnce(Return(true));
	
	CodecMock codecCurrent;
	EXPECT_CALL(codecCurrent,isSeek()).Times(1).WillOnce(Return(false));
	
	AOBaseUnpausePlaybackCodecStateFinishTest audio;
	EXPECT_CALL(audio,getCompleteCodec()).Times(2).WillOnce(Return(&codecComplete)).WillOnce(Return(&codecComplete));
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codecCurrent));
	EXPECT_CALL(audio,setNextCodec(&codecCurrent)).Times(1);
	EXPECT_CALL(audio,setCodec(&codecComplete)).Times(1);
	EXPECT_CALL(audio,setCompleteCodec((engine::Codec *)0)).Times(1);
	EXPECT_CALL(audio,getNextCodec()).WillRepeatedly(Return(&codecCurrent));
	EXPECT_CALL(audio,setCodecState(AOBase::e_codecNextQueued)).Times(1);
	EXPECT_CALL(audio,setTrackTimeStateFlag(true)).Times(1);
	EXPECT_CALL(audio,calcNextCodecTime()).Times(1);
	EXPECT_CALL(audio,unpausePlayback(true)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testUnpausePlaybackCodecStateFinish());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateFinishWhenNextSeekSuccess)
{
	common::TimeStamp zeroT(0);

	CodecMock codecComplete;
	EXPECT_CALL(codecComplete,isSeek()).Times(1).WillOnce(Return(true));
	
	CodecMock codecCurrent;
	EXPECT_CALL(codecCurrent,isSeek()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codecCurrent,seek(Eq(zeroT))).Times(1).WillOnce(Return(true));
	
	
	AOBaseUnpausePlaybackCodecStateFinishTest audio;
	EXPECT_CALL(audio,getCompleteCodec()).Times(2).WillOnce(Return(&codecComplete)).WillOnce(Return(&codecComplete));
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codecCurrent));
	EXPECT_CALL(audio,setNextCodec(&codecCurrent)).Times(1);
	EXPECT_CALL(audio,setCodec(&codecComplete)).Times(1);
	EXPECT_CALL(audio,setCompleteCodec((engine::Codec *)0)).Times(1);
	EXPECT_CALL(audio,getNextCodec()).WillRepeatedly(Return(&codecCurrent));
	EXPECT_CALL(audio,setCodecState(AOBase::e_codecNextQueued)).Times(1);
	EXPECT_CALL(audio,setTrackTimeStateFlag(true)).Times(1);
	EXPECT_CALL(audio,calcNextCodecTime()).Times(1);
	EXPECT_CALL(audio,unpausePlayback(true)).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testUnpausePlaybackCodecStateFinish());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackCodecStateFinishWhenNextSeekFailure)
{
	common::TimeStamp zeroT(0);

	CodecMock codecComplete;
	EXPECT_CALL(codecComplete,isSeek()).Times(1).WillOnce(Return(true));
	
	CodecMock codecCurrent;
	EXPECT_CALL(codecCurrent,isSeek()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(codecCurrent,seek(Eq(zeroT))).Times(1).WillOnce(Return(false));
	
	
	AOBaseUnpausePlaybackCodecStateFinishTest audio;
	EXPECT_CALL(audio,getCompleteCodec()).Times(2).WillOnce(Return(&codecComplete)).WillOnce(Return(&codecComplete));
	EXPECT_CALL(audio,getCodec()).Times(1).WillOnce(Return(&codecCurrent));
	EXPECT_CALL(audio,setNextCodec(&codecCurrent)).Times(1);
	EXPECT_CALL(audio,setCodec(&codecComplete)).Times(1);
	EXPECT_CALL(audio,setCompleteCodec((engine::Codec *)0)).Times(1);
	EXPECT_CALL(audio,getNextCodec()).WillRepeatedly(Return(&codecCurrent));
	EXPECT_CALL(audio,setCodecState(AOBase::e_codecNextQueued)).Times(1);
	EXPECT_CALL(audio,setTrackTimeStateFlag(true)).Times(1);
	EXPECT_CALL(audio,stopNextCodec()).Times(1);
	EXPECT_CALL(audio,calcNextCodecTime()).Times(1);
	EXPECT_CALL(audio,unpausePlayback(true)).Times(1).WillOnce(Return(false));
	
	EXPECT_FALSE(audio.testUnpausePlaybackCodecStateFinish());
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackProcessTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(unpausePlaybackProcessSetTimeAndState,void());
		MOCK_METHOD0(unpausePlaybackProcessOpenAudio,void());
		MOCK_METHOD1(unpausePlaybackProcessRestartPlayback,bool(bool signalFlag));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackProcessGivenNoSignalFlag)
{
	AOBaseUnpausePlaybackProcessTest audio;
	EXPECT_CALL(audio,unpausePlaybackProcessSetTimeAndState()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackProcessOpenAudio()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackProcessRestartPlayback(false)).Times(1)
		.WillOnce(Return(true));
		
	EXPECT_TRUE(audio.testUnpausePlaybackProcess(false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackProcessGivenSignalFlag)
{
	AOBaseUnpausePlaybackProcessTest audio;
	EXPECT_CALL(audio,unpausePlaybackProcessSetTimeAndState()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackProcessOpenAudio()).Times(1);
	EXPECT_CALL(audio,unpausePlaybackProcessRestartPlayback(true)).Times(1)
		.WillOnce(Return(false));
		
	EXPECT_FALSE(audio.testUnpausePlaybackProcess(true));
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackProcessOpenAudioTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD2(printError,void(const tchar *strR,const tchar *strE));
		MOCK_METHOD0(resetResampler,void());
		MOCK_CONST_METHOD0(isAudio,bool());
		MOCK_METHOD0(openAudio,bool());
		MOCK_METHOD0(closeAudio,void());
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackProcessOpenAudioWhenAudioAlreadyOpen)
{
	AOBaseUnpausePlaybackProcessOpenAudioTest audio;
	EXPECT_CALL(audio,resetResampler()).Times(1);
	EXPECT_CALL(audio,isAudio()).Times(1).WillOnce(Return(true));
	
	audio.testUnpausePlaybackProcessOpenAudio();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackProcessOpenAudioSuccessOpenAudio)
{
	AOBaseUnpausePlaybackProcessOpenAudioTest audio;
	EXPECT_CALL(audio,resetResampler()).Times(1);
	EXPECT_CALL(audio,isAudio()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,openAudio()).Times(1).WillOnce(Return(true));
	
	audio.testUnpausePlaybackProcessOpenAudio();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackProcessOpenAudioFailOpenAudio)
{
	AOBaseUnpausePlaybackProcessOpenAudioTest audio;
	EXPECT_CALL(audio,resetResampler()).Times(1);
	EXPECT_CALL(audio,isAudio()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,openAudio()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,printError(StrEq("unpausePlayback"),StrEq("Error opening audio device"))).Times(1);
	EXPECT_CALL(audio,closeAudio()).Times(1);
	
	audio.testUnpausePlaybackProcessOpenAudio();
}

//-------------------------------------------------------------------------------------------

class AOBaseUnpausePlaybackProcessRestartPlayback : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD2(printError,void(const tchar *strR,const tchar *strE));
		MOCK_CONST_METHOD0(isAudio,bool());
		MOCK_METHOD0(emitOnPlay,void());
		MOCK_METHOD1(processCodec,void(bool initF));
		MOCK_METHOD0(startAudioDevice,bool());
		MOCK_METHOD1(stopCodec,void(bool eFlag));
};

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackProcessRestartPlaybackGivenNoAudio)
{
	AOBaseUnpausePlaybackProcessRestartPlayback audio;
	EXPECT_CALL(audio,isAudio()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,stopCodec(true)).Times(1);
	
	EXPECT_FALSE(audio.testUnpausePlaybackProcessRestartPlayback(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackProcessRestartPlaybackFailToStartAudioDevice)
{
	AOBaseUnpausePlaybackProcessRestartPlayback audio;
	EXPECT_CALL(audio,isAudio()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,processCodec(false)).Times(1);
	EXPECT_CALL(audio,startAudioDevice()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,printError(StrEq("unpausePlayback"),StrEq("Error starting audio device")))
		.Times(1);
	EXPECT_CALL(audio,stopCodec(true)).Times(1);
	
	EXPECT_FALSE(audio.testUnpausePlaybackProcessRestartPlayback(true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackProcessRestartPlaybackGivenNoSignalFlag)
{
	AOBaseUnpausePlaybackProcessRestartPlayback audio;
	EXPECT_CALL(audio,isAudio()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,processCodec(false)).Times(1);
	EXPECT_CALL(audio,startAudioDevice()).Times(1).WillOnce(Return(true));
	
	EXPECT_TRUE(audio.testUnpausePlaybackProcessRestartPlayback(false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,unpausePlaybackProcessRestartPlaybackGivenSignalFlag)
{
	AOBaseUnpausePlaybackProcessRestartPlayback audio;
	EXPECT_CALL(audio,isAudio()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,processCodec(false)).Times(1);
	EXPECT_CALL(audio,startAudioDevice()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,emitOnPlay()).Times(1);
	
	EXPECT_TRUE(audio.testUnpausePlaybackProcessRestartPlayback(true));
}

//-------------------------------------------------------------------------------------------

class AOBaseSetCodecCompletePositionFromNextTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getNextCodec,engine::Codec *());
		MOCK_CONST_METHOD0(getNextCodecSeekTime,const common::TimeStamp&());
		MOCK_METHOD1(setCodecTimePositionComplete,void(const common::TimeStamp& t));
		MOCK_CONST_METHOD0(getNextCodecTimeLengthComplete,const common::TimeStamp&());
		MOCK_METHOD1(setNextCodecTimeLengthComplete,void(const common::TimeStamp& t));
		
		void testSetCodecCompletePositionFromNext();
};

//-------------------------------------------------------------------------------------------

void AOBaseSetCodecCompletePositionFromNextTest::testSetCodecCompletePositionFromNext()
{
	setCodecCompletePositionFromNext();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,setCodecCompletePositionFromNextGivenNoNextCodec)
{
	common::TimeStamp codecTimePosition(2.0);
	common::TimeStamp nextTimeLength(3.0);

	AOBaseSetCodecCompletePositionFromNextTest audio;
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return((engine::Codec *)0));
	EXPECT_CALL(audio,setCodecTimePositionComplete(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&codecTimePosition));
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&nextTimeLength));
	
	audio.testSetCodecCompletePositionFromNext();

	EXPECT_NEAR(0.0,static_cast<tfloat64>(codecTimePosition),0.0000001);
	EXPECT_NEAR(0.0,static_cast<tfloat64>(nextTimeLength),0.0000001);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,setCodecCompletePositionFromNextGivenNextButLengthIsUndefined)
{
	CodecMock codec;
	common::TimeStamp codecTimePosition(2.0);
	common::TimeStamp nextTimeLength(0.0);

	AOBaseSetCodecCompletePositionFromNextTest audio;
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getNextCodecTimeLengthComplete()).Times(1).WillOnce(ReturnRef(nextTimeLength));
	EXPECT_CALL(audio,setCodecTimePositionComplete(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&codecTimePosition));
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&nextTimeLength));
	
	audio.testSetCodecCompletePositionFromNext();

	EXPECT_NEAR(0.0,static_cast<tfloat64>(codecTimePosition),0.0000001);
	EXPECT_NEAR(0.0,static_cast<tfloat64>(nextTimeLength),0.0000001);
}

//-------------------------------------------------------------------------------------------

TEST(AOBASE,setCodecCompletePositionFromNextGivenNextAndLengthIsDefined)
{
	CodecMock codec;
	common::TimeStamp codecTimePosition(2.0);
	common::TimeStamp nextSeekPositionTime(3.0);
	common::TimeStamp nextTimeLength(5.0);

	AOBaseSetCodecCompletePositionFromNextTest audio;
	EXPECT_CALL(audio,getNextCodec()).Times(1).WillOnce(Return(&codec));
	EXPECT_CALL(audio,getNextCodecTimeLengthComplete()).Times(2).WillRepeatedly(ReturnRef(nextTimeLength));
	EXPECT_CALL(audio,getNextCodecSeekTime()).Times(1).WillOnce(ReturnRef(nextSeekPositionTime));
	EXPECT_CALL(audio,setCodecTimePositionComplete(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&codecTimePosition));
	EXPECT_CALL(audio,setNextCodecTimeLengthComplete(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&nextTimeLength));
	
	audio.testSetCodecCompletePositionFromNext();

	EXPECT_NEAR(8.0,static_cast<tfloat64>(codecTimePosition),0.0000001);
	EXPECT_NEAR(0.0,static_cast<tfloat64>(nextTimeLength),0.0000001);
}

//-------------------------------------------------------------------------------------------

class AOBaseCalcCrossFadeTimeTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(calcCrossFadeTimeSetLength,void());
		MOCK_METHOD0(calcCrossFadeTimeAdjustToCodecLength,void());
		
		void testCalcCrossFadeTime();
};

//-------------------------------------------------------------------------------------------

void AOBaseCalcCrossFadeTimeTest::testCalcCrossFadeTime()
{
	calcCrossFadeTime();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calcCrossFadeTimeIsSetAndAdjustedAccordingly)
{
	AOBaseCalcCrossFadeTimeTest audio;
	EXPECT_CALL(audio,calcCrossFadeTimeSetLength()).Times(1);
	EXPECT_CALL(audio,calcCrossFadeTimeAdjustToCodecLength()).Times(1);
	
	audio.testCalcCrossFadeTime();
}

//-------------------------------------------------------------------------------------------

class AOBaseCalcCrossFadeTimeSetLengthTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getProgFadeState,tint());
		MOCK_METHOD1(setProgFadeState,void(tint state));
		MOCK_METHOD1(setCrossFadeTimeLen,void(const common::TimeStamp& t));
		MOCK_CONST_METHOD0(getProgFadeTime,const common::TimeStamp&());
		MOCK_METHOD1(setCrossFadeFlag,void(bool flag));
		MOCK_CONST_METHOD0(getCrossFadeTimeLen,const common::TimeStamp&());
		MOCK_METHOD1(setCrossFadeTime,void(const common::TimeStamp& t));
		MOCK_CONST_METHOD0(getCrossFadeTime,const common::TimeStamp&());
		
		void testCalcCrossFadeTimeSetLength();
};

//-------------------------------------------------------------------------------------------

void AOBaseCalcCrossFadeTimeSetLengthTest::testCalcCrossFadeTimeSetLength()
{
	calcCrossFadeTimeSetLength();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calcCrossFadeTimeSetLengthWithNoProgramFadeStateAndNoCrossFadeTimeLength)
{
	tint progFadeState = 0;
	common::TimeStamp crossFadeTimeLen(0.0);
	common::TimeStamp crossFadeTime(1.0);
	bool crossFadeFlag = true;
	
	AOBaseCalcCrossFadeTimeSetLengthTest audio;
	EXPECT_CALL(audio,getProgFadeState()).Times(1).WillOnce(Return(progFadeState));
	EXPECT_CALL(audio,setProgFadeState(A<tint>())).Times(1).WillOnce(SaveArg<0>(&progFadeState));
	EXPECT_CALL(audio,getCrossFadeTimeLen()).Times(1).WillOnce(ReturnRef(crossFadeTimeLen));
	EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&crossFadeTime));
	EXPECT_CALL(audio,getCrossFadeTime()).WillOnce(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,setCrossFadeFlag(A<bool>())).Times(1).WillOnce(SaveArg<0>(&crossFadeFlag));
	
	audio.testCalcCrossFadeTimeSetLength();
	
	EXPECT_EQ(0,progFadeState);
	EXPECT_NEAR(0.0,static_cast<tfloat64>(crossFadeTimeLen),0.0000001);
	EXPECT_NEAR(0.0,static_cast<tfloat64>(crossFadeTime),0.0000001);
	EXPECT_FALSE(crossFadeFlag);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calcCrossFadeTimeSetLengthWithNoProgramFadeStateAndCrossFadeTimeLength)
{
	tint progFadeState = 0;
	common::TimeStamp crossFadeTimeLen(4.0);
	common::TimeStamp crossFadeTime(1.0);
	bool crossFadeFlag = false;
	
	AOBaseCalcCrossFadeTimeSetLengthTest audio;
	EXPECT_CALL(audio,getProgFadeState()).Times(1).WillOnce(Return(progFadeState));
	EXPECT_CALL(audio,setProgFadeState(A<tint>())).Times(1).WillOnce(SaveArg<0>(&progFadeState));
	EXPECT_CALL(audio,getCrossFadeTimeLen()).Times(1).WillOnce(ReturnRef(crossFadeTimeLen));
	EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&crossFadeTime));
	EXPECT_CALL(audio,getCrossFadeTime()).WillOnce(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,setCrossFadeFlag(A<bool>())).Times(1).WillOnce(SaveArg<0>(&crossFadeFlag));
	
	audio.testCalcCrossFadeTimeSetLength();
	
	EXPECT_EQ(0,progFadeState);
	EXPECT_NEAR(4.0,static_cast<tfloat64>(crossFadeTimeLen),0.0000001);
	EXPECT_NEAR(4.0,static_cast<tfloat64>(crossFadeTime),0.0000001);
	EXPECT_TRUE(crossFadeFlag);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calcCrossFadeTimeSetLengthWithProgramFadeStateAndCrossFadeTimeLength)
{
	tint progFadeState = 1;
	common::TimeStamp progFadeTime(4.0);
	common::TimeStamp crossFadeTimeLen(0.0);
	common::TimeStamp crossFadeTime(1.0);
	bool crossFadeFlag = false;
	
	AOBaseCalcCrossFadeTimeSetLengthTest audio;
	EXPECT_CALL(audio,getProgFadeState()).Times(1).WillOnce(Return(progFadeState));
	EXPECT_CALL(audio,getProgFadeTime()).Times(1).WillOnce(ReturnRef(progFadeTime));
	EXPECT_CALL(audio,setCrossFadeTimeLen(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&crossFadeTimeLen));
	EXPECT_CALL(audio,setProgFadeState(A<tint>())).Times(1).WillOnce(SaveArg<0>(&progFadeState));
	EXPECT_CALL(audio,getCrossFadeTimeLen()).Times(1).WillOnce(ReturnRef(crossFadeTimeLen));
	EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&crossFadeTime));
	EXPECT_CALL(audio,getCrossFadeTime()).WillOnce(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,setCrossFadeFlag(A<bool>())).Times(1).WillOnce(SaveArg<0>(&crossFadeFlag));
	
	audio.testCalcCrossFadeTimeSetLength();
	
	EXPECT_EQ(0,progFadeState);
	EXPECT_NEAR(4.0,static_cast<tfloat64>(crossFadeTimeLen),0.0000001);
	EXPECT_NEAR(4.0,static_cast<tfloat64>(crossFadeTime),0.0000001);
	EXPECT_TRUE(crossFadeFlag);
}

//-------------------------------------------------------------------------------------------

class AOBaseCalcCrossFadeTimeAdjustToCodecLengthTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getCrossFadeFlag,bool());
		MOCK_CONST_METHOD0(getCrossFadeTime,const common::TimeStamp&());
		MOCK_METHOD1(setCrossFadeTime,void(const common::TimeStamp& t));
		MOCK_CONST_METHOD0(getCodecTimePositionComplete,const common::TimeStamp&());
		MOCK_CONST_METHOD0(getCodecTimeLength,const common::TimeStamp&());
		
		void testCalcCrossFadeTimeAdjustToCodecLength();
};

//-------------------------------------------------------------------------------------------

void AOBaseCalcCrossFadeTimeAdjustToCodecLengthTest::testCalcCrossFadeTimeAdjustToCodecLength()
{
	calcCrossFadeTimeAdjustToCodecLength();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calcCrossFadeTimeAdjustToCodecLengthGivenCrossFadeTimeLessThanLengthAndPositionIsUndefined)
{
	bool crossFadeFlag = true;
	common::TimeStamp codecTimePosition(0.0);
	common::TimeStamp codecTimeLength(8.0);
	common::TimeStamp crossFadeTime(3.0);
	
	AOBaseCalcCrossFadeTimeAdjustToCodecLengthTest audio;
	EXPECT_CALL(audio,getCrossFadeFlag()).WillRepeatedly(Return(crossFadeFlag));
    EXPECT_CALL(audio,getCrossFadeTime()).WillRepeatedly(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(codecTimePosition));
	EXPECT_CALL(audio,getCodecTimeLength()).WillRepeatedly(ReturnRef(codecTimeLength));
    EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).WillRepeatedly(SaveArg<0>(&crossFadeTime));
	
	audio.testCalcCrossFadeTimeAdjustToCodecLength();
	
	EXPECT_NEAR(5.0,static_cast<tfloat64>(crossFadeTime),0.0000001);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calcCrossFadeTimeAdjustToCodecLengthGivenCrossFadeTimeLessThanPositionAndPositionIsDefined)
{
	bool crossFadeFlag = true;
	common::TimeStamp codecTimePosition(7.0);
	common::TimeStamp codecTimeLength(8.0);
	common::TimeStamp crossFadeTime(3.0);
	
	AOBaseCalcCrossFadeTimeAdjustToCodecLengthTest audio;
	EXPECT_CALL(audio,getCrossFadeFlag()).WillRepeatedly(Return(crossFadeFlag));
    EXPECT_CALL(audio,getCrossFadeTime()).WillRepeatedly(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(codecTimePosition));
	EXPECT_CALL(audio,getCodecTimeLength()).WillRepeatedly(ReturnRef(codecTimeLength));
    EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).WillRepeatedly(SaveArg<0>(&crossFadeTime));
	
	audio.testCalcCrossFadeTimeAdjustToCodecLength();
	
	EXPECT_NEAR(4.0,static_cast<tfloat64>(crossFadeTime),0.0000001);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calcCrossFadeTimeAdjustToCodecLengthGivenCrossFadeTimeGreaterThanLengthAndPositionIsUndefined)
{
	bool crossFadeFlag = true;
	common::TimeStamp codecTimePosition(0.0);
	common::TimeStamp codecTimeLength(4.0);
	common::TimeStamp crossFadeTime(5.0);
	
	AOBaseCalcCrossFadeTimeAdjustToCodecLengthTest audio;
	EXPECT_CALL(audio,getCrossFadeFlag()).WillRepeatedly(Return(crossFadeFlag));
    EXPECT_CALL(audio,getCrossFadeTime()).WillRepeatedly(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(codecTimePosition));
	EXPECT_CALL(audio,getCodecTimeLength()).WillRepeatedly(ReturnRef(codecTimeLength));
    EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).WillRepeatedly(SaveArg<0>(&crossFadeTime));
	
	audio.testCalcCrossFadeTimeAdjustToCodecLength();
	
	EXPECT_NEAR(4.0,static_cast<tfloat64>(crossFadeTime),0.0000001);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calcCrossFadeTimeAdjustToCodecLengthGivenCrossFadeTimeGreaterThanPositionAndPositionIsDefined)
{
	bool crossFadeFlag = true;
	common::TimeStamp codecTimePosition(3.0);
	common::TimeStamp codecTimeLength(4.0);
	common::TimeStamp crossFadeTime(5.0);
	
	AOBaseCalcCrossFadeTimeAdjustToCodecLengthTest audio;
	EXPECT_CALL(audio,getCrossFadeFlag()).WillRepeatedly(Return(crossFadeFlag));
    EXPECT_CALL(audio,getCrossFadeTime()).WillRepeatedly(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(codecTimePosition));
	EXPECT_CALL(audio,getCodecTimeLength()).WillRepeatedly(ReturnRef(codecTimeLength));
    EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).WillRepeatedly(SaveArg<0>(&crossFadeTime));
	
	audio.testCalcCrossFadeTimeAdjustToCodecLength();
	
	EXPECT_NEAR(3.0,static_cast<tfloat64>(crossFadeTime),0.0000001);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calcCrossFadeTimeAdjustToCodecLengthGivenNoCrossFadeAndPositionIsUndefined)
{
	bool crossFadeFlag = false;
	common::TimeStamp codecTimePosition(0.0);
	common::TimeStamp codecTimeLength(12.0);
	common::TimeStamp crossFadeTime(5.0);
	
	AOBaseCalcCrossFadeTimeAdjustToCodecLengthTest audio;
	EXPECT_CALL(audio,getCrossFadeFlag()).WillRepeatedly(Return(crossFadeFlag));
    EXPECT_CALL(audio,getCrossFadeTime()).WillRepeatedly(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(codecTimePosition));
	EXPECT_CALL(audio,getCodecTimeLength()).WillRepeatedly(ReturnRef(codecTimeLength));
    EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).WillRepeatedly(SaveArg<0>(&crossFadeTime));
	
	audio.testCalcCrossFadeTimeAdjustToCodecLength();
	
	EXPECT_NEAR(12.0,static_cast<tfloat64>(crossFadeTime),0.0000001);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,calcCrossFadeTimeAdjustToCodecLengthGivenNoCrossFadeAndPositionIsDefined)
{
	bool crossFadeFlag = false;
	common::TimeStamp codecTimePosition(10.0);
	common::TimeStamp codecTimeLength(12.0);
	common::TimeStamp crossFadeTime(5.0);
	
	AOBaseCalcCrossFadeTimeAdjustToCodecLengthTest audio;
	EXPECT_CALL(audio,getCrossFadeFlag()).WillRepeatedly(Return(crossFadeFlag));
    EXPECT_CALL(audio,getCrossFadeTime()).WillRepeatedly(ReturnRef(crossFadeTime));
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(codecTimePosition));
	EXPECT_CALL(audio,getCodecTimeLength()).WillRepeatedly(ReturnRef(codecTimeLength));
    EXPECT_CALL(audio,setCrossFadeTime(A<const common::TimeStamp&>())).WillRepeatedly(SaveArg<0>(&crossFadeTime));
	
	audio.testCalcCrossFadeTimeAdjustToCodecLength();
	
	EXPECT_NEAR(10.0,static_cast<tfloat64>(crossFadeTime),0.0000001);
}

//-------------------------------------------------------------------------------------------

class  AOBaseProcessCodecEndForTimePositionCompleteTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getCodecTimePositionComplete,const common::TimeStamp&());
		
		bool testProcessCodecEndForTimePositionComplete(AudioItem *item,bool decodeFlag);
};

//-------------------------------------------------------------------------------------------

bool AOBaseProcessCodecEndForTimePositionCompleteTest::testProcessCodecEndForTimePositionComplete(AudioItem *item,bool decodeFlag)
{
	return processCodecEndForTimePositionComplete(item,decodeFlag);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecEndForTimePositionCompleteNoEndTimeAndStillDecoding)
{
	common::TimeStamp endPartTime(0.0);
	common::TimeStamp positionCompleteTime(7.0);
	
	RDataMock data;
	EXPECT_CALL(data,end()).WillRepeatedly(ReturnRef(endPartTime));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	AOBaseProcessCodecEndForTimePositionCompleteTest audio;
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(positionCompleteTime));
	
	ASSERT_TRUE(audio.testProcessCodecEndForTimePositionComplete(&item,true));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecEndForTimePositionCompleteNoEndTimeAndFinishedDecoding)
{
	common::TimeStamp endPartTime(0.0);
	common::TimeStamp positionCompleteTime(7.0);
	
	RDataMock data;
	EXPECT_CALL(data,end()).WillRepeatedly(ReturnRef(endPartTime));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	AOBaseProcessCodecEndForTimePositionCompleteTest audio;
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(positionCompleteTime));
	
	ASSERT_FALSE(audio.testProcessCodecEndForTimePositionComplete(&item,false));	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecEndForTimePositionCompleteIsEndTimeAndStillDecodingAndCodecNotPastEndTime)
{
	common::TimeStamp endPartTime(6.0);
	common::TimeStamp positionCompleteTime(7.0);
	
	RDataMock data;
	EXPECT_CALL(data,end()).WillRepeatedly(ReturnRef(endPartTime));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	AOBaseProcessCodecEndForTimePositionCompleteTest audio;
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(positionCompleteTime));
	
	ASSERT_TRUE(audio.testProcessCodecEndForTimePositionComplete(&item,true));	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecEndForTimePositionCompleteIsEndTimeAndFinishedDecodingAndCodecNotPastEndTime)
{
	common::TimeStamp endPartTime(6.0);
	common::TimeStamp positionCompleteTime(7.0);
	
	RDataMock data;
	EXPECT_CALL(data,end()).WillRepeatedly(ReturnRef(endPartTime));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	AOBaseProcessCodecEndForTimePositionCompleteTest audio;
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(positionCompleteTime));
	
	ASSERT_FALSE(audio.testProcessCodecEndForTimePositionComplete(&item,false));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecEndForTimePositionCompleteIsEndTimeAndStillDecodingAndCodecPastEndTime)
{
	common::TimeStamp endPartTime(8.0);
	common::TimeStamp positionCompleteTime(7.0);
	
	RDataMock data;
	EXPECT_CALL(data,end()).WillRepeatedly(ReturnRef(endPartTime));
	EXPECT_CALL(data,clipToTime(positionCompleteTime));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	AOBaseProcessCodecEndForTimePositionCompleteTest audio;
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(positionCompleteTime));
	
	ASSERT_FALSE(audio.testProcessCodecEndForTimePositionComplete(&item,true));	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,processCodecEndForTimePositionCompleteIsEndTimeAndFinishedDecodingAndCodecPastEndTime)
{
	common::TimeStamp endPartTime(8.0);
	common::TimeStamp positionCompleteTime(7.0);
	
	RDataMock data;
	EXPECT_CALL(data,end()).WillRepeatedly(ReturnRef(endPartTime));
	EXPECT_CALL(data,clipToTime(positionCompleteTime));
	
	AudioItemMock item;
	EXPECT_CALL(item,data()).Times(1).WillOnce(Return(&data));
	
	AOBaseProcessCodecEndForTimePositionCompleteTest audio;
	EXPECT_CALL(audio,getCodecTimePositionComplete()).WillRepeatedly(ReturnRef(positionCompleteTime));
	
	ASSERT_FALSE(audio.testProcessCodecEndForTimePositionComplete(&item,false));
}

//-------------------------------------------------------------------------------------------

class AOBaseUpdateCurrentPlayTimeFromStreamTime : public AOBaseTest
{
	public:

		MOCK_METHOD0(getAudioStartFlag,bool());
		MOCK_METHOD1(setAudioStartFlag,void(bool f));
		MOCK_CONST_METHOD0(getPauseTime,common::TimeStamp());
		MOCK_METHOD1(setPauseTime,void(const common::TimeStamp& t));
		MOCK_CONST_METHOD0(getTrackTimeState,tint());
		MOCK_METHOD1(setTrackTimeState,void(tint state));
		MOCK_CONST_METHOD0(getTrackTimeStateFlag,bool());
		MOCK_METHOD1(setTrackTimeStateFlag,void(bool flag));
		MOCK_METHOD1(setCurrentPlayTime,void(const common::TimeStamp& v));
		MOCK_CONST_METHOD0(getAudioStartClock,const common::TimeStamp&());
		MOCK_METHOD1(setAudioStartClock,void(const common::TimeStamp& t));

		void testUpdateCurrentPlayTimeFromStreamTime(const IOTimeStamp& systemTime);
};

//-------------------------------------------------------------------------------------------

void AOBaseUpdateCurrentPlayTimeFromStreamTime::testUpdateCurrentPlayTimeFromStreamTime(const IOTimeStamp& systemTime)
{
	updateCurrentPlayTimeFromStreamTime(systemTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,updateCurrentPlayTimeFromStreamTimeWithInvalidSystemTime)
{
	common::TimeStamp streamTime(10.0);
	IOTimeStamp systemTime(false,streamTime);
	
	AOBaseUpdateCurrentPlayTimeFromStreamTime audio;	
	audio.testUpdateCurrentPlayTimeFromStreamTime(systemTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,updateCurrentPlayTimeFromStreamTimeAfterAudioHasStarted)
{
	common::TimeStamp streamTime(10.0);
	common::TimeStamp actualStartTime(7.0);
	common::TimeStamp actualPlayTime,expectPlayTime = streamTime - actualStartTime;
	
	IOTimeStamp systemTime(true,streamTime);
	
	AOBaseUpdateCurrentPlayTimeFromStreamTime audio;
	EXPECT_CALL(audio,getAudioStartFlag()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,getAudioStartClock()).Times(1).WillOnce(ReturnRef(actualStartTime));
	EXPECT_CALL(audio,setCurrentPlayTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualPlayTime));
	
	audio.testUpdateCurrentPlayTimeFromStreamTime(systemTime);
	
	EXPECT_EQ(expectPlayTime,actualPlayTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,updateCurrentPlayTimeFromStreamTimeAudioStartingAndTrackTimeStateNotSet)
{
	common::TimeStamp streamTime(10.0);
	common::TimeStamp pauseTime(3.0);
	common::TimeStamp actualStartTime,expectStartTime = streamTime - pauseTime;
	common::TimeStamp actualPlayTime,expectPlayTime = streamTime - expectStartTime;
	
	IOTimeStamp systemTime(true,streamTime);
	
	AOBaseUpdateCurrentPlayTimeFromStreamTime audio;
	EXPECT_CALL(audio,getAudioStartFlag()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,getPauseTime()).Times(1).WillOnce(Return(pauseTime));
	EXPECT_CALL(audio,setAudioStartClock(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualStartTime));
	EXPECT_CALL(audio,setAudioStartFlag(true)).Times(1);
	EXPECT_CALL(audio,getTrackTimeStateFlag()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,setTrackTimeStateFlag(false)).Times(1);
	EXPECT_CALL(audio,getAudioStartClock()).Times(1).WillOnce(ReturnRef(actualStartTime));
	EXPECT_CALL(audio,setCurrentPlayTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualPlayTime));
	
	audio.testUpdateCurrentPlayTimeFromStreamTime(systemTime);
	
	EXPECT_EQ(expectStartTime,actualStartTime);
	EXPECT_EQ(expectPlayTime,actualPlayTime);	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,updateCurrentPlayTimeFromStreamTimeAudioStartingAndTrackTimeStateSet)
{
	common::TimeStamp streamTime(10.0);
	common::TimeStamp pauseTime(3.0);
	common::TimeStamp actualStartTime,expectStartTime = streamTime - pauseTime;
	common::TimeStamp actualPlayTime,expectPlayTime = streamTime - expectStartTime;
	
	IOTimeStamp systemTime(true,streamTime);
	
	AOBaseUpdateCurrentPlayTimeFromStreamTime audio;
	EXPECT_CALL(audio,getAudioStartFlag()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,getPauseTime()).Times(1).WillOnce(Return(pauseTime));
	EXPECT_CALL(audio,setAudioStartClock(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualStartTime));
	EXPECT_CALL(audio,setAudioStartFlag(true)).Times(1);
	EXPECT_CALL(audio,getTrackTimeStateFlag()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,setTrackTimeState(1)).Times(1);
	EXPECT_CALL(audio,setTrackTimeStateFlag(false)).Times(1);
	EXPECT_CALL(audio,getAudioStartClock()).Times(1).WillOnce(ReturnRef(actualStartTime));
	EXPECT_CALL(audio,setCurrentPlayTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualPlayTime));
	
	audio.testUpdateCurrentPlayTimeFromStreamTime(systemTime);
	
	EXPECT_EQ(expectStartTime,actualStartTime);
	EXPECT_EQ(expectPlayTime,actualPlayTime);
}

//-------------------------------------------------------------------------------------------

class AOBaseIncrementMutexCountTest : public AOBaseTest
{
	public:
		void testIncrementMutexCount();
		tint testGetMutexCount() const;
};

//-------------------------------------------------------------------------------------------

void AOBaseIncrementMutexCountTest::testIncrementMutexCount()
{
	incrementMutexCount();
}

//-------------------------------------------------------------------------------------------

tint AOBaseIncrementMutexCountTest::testGetMutexCount() const
{
	return getMutexCount();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,incrementMutexCount)
{
	AOBaseIncrementMutexCountTest audio;
	ASSERT_EQ(0,audio.testGetMutexCount());
	audio.testIncrementMutexCount();
	ASSERT_EQ(1,audio.testGetMutexCount());
	audio.testIncrementMutexCount();
	ASSERT_EQ(2,audio.testGetMutexCount());
	audio.testIncrementMutexCount();
	ASSERT_EQ(3,audio.testGetMutexCount());
}

//-------------------------------------------------------------------------------------------

class AOBaseSetItemStateToCallbackAsApplicableTest : public AOBaseTest
{
	public:
		MOCK_METHOD1(setCallbackAudioTime,void(AudioItem *item));
		void testSetItemStateToCallbackAsApplicable(AudioItem *item);
};

//-------------------------------------------------------------------------------------------

void AOBaseSetItemStateToCallbackAsApplicableTest::testSetItemStateToCallbackAsApplicable(AudioItem *item)
{
	setItemStateToCallbackAsApplicable(item);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,setItemStateToCallbackAsApplicableWhenNotFull)
{
	AudioItem item;
	item.setState(AudioItem::e_stateDone);
	
	AOBaseSetItemStateToCallbackAsApplicableTest audio;
	
	audio.testSetItemStateToCallbackAsApplicable(&item);
	EXPECT_EQ(AudioItem::e_stateDone,item.state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,setItemStateToCallbackAsApplicableWhenCallbackSet)
{
	AudioItem item;
	item.setState(AudioItem::e_stateCallback);
	
	AOBaseSetItemStateToCallbackAsApplicableTest audio;
	
	audio.testSetItemStateToCallbackAsApplicable(&item);
	EXPECT_EQ(AudioItem::e_stateCallback,item.state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,setItemStateToCallbackAsApplicableWhenFull)
{
	AudioItem item;
	item.setState(AudioItem::e_stateFull);
	
	AOBaseSetItemStateToCallbackAsApplicableTest audio;
	EXPECT_CALL(audio,setCallbackAudioTime(&item)).Times(1);
	
	audio.testSetItemStateToCallbackAsApplicable(&item);
	EXPECT_EQ(AudioItem::e_stateCallback,item.state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,setItemStateToCallbackAsApplicableWhenFullAndEnd)
{
	AudioItem item;
	item.setState(AudioItem::e_stateFullEnd);
	
	AOBaseSetItemStateToCallbackAsApplicableTest audio;
	EXPECT_CALL(audio,setCallbackAudioTime(&item)).Times(1);
	
	audio.testSetItemStateToCallbackAsApplicable(&item);
	EXPECT_EQ(AudioItem::e_stateCallbackEnd,item.state());
}

//-------------------------------------------------------------------------------------------

class AOBaseRemainingSamplesInBufferTest : public AOBaseTest
{
	public:
		tint testRemainingSamplesInBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex);
};

//-------------------------------------------------------------------------------------------

tint AOBaseRemainingSamplesInBufferTest::testRemainingSamplesInBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
    return remainingSamplesInBuffer(pBuffer,fromIndex);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,remainingSamplesInBuffer)
{
	const tint c_channels[] = {3, 2, 1};
	AudioHardwareBufferTester buffer(c_channels,3);
	
	AOBaseRemainingSamplesInBufferTest audio;
	
	for(tint i=0;i<AudioHardwareBufferTester::c_bufferLength;i++)
	{
		EXPECT_EQ(AudioHardwareBufferTester::c_bufferLength - i,audio.testRemainingSamplesInBuffer(&buffer,i));
	}
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteToAudioSilenceToEndOfBufferTest : public AOBaseTest
{
	public:
		void testWriteToAudioSilenceToEndOfBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex);
};

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioSilenceToEndOfBufferTest::testWriteToAudioSilenceToEndOfBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
	writeToAudioSilenceToEndOfBuffer(pBuffer,fromIndex);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithNoChannels)
{
	AudioHardwareBufferTester buffer(0,0);
	
	AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
	audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,3);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithMono)
{
	const tint c_channels[1] = {1};
	
	const tfloat32 c_BufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f };

	const tfloat32 c_expectBufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 0.0f, 0.0f };
		
	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_BufferA,1 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
	audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,3);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,1 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithStereo)
{
	const tint c_channels[1] = {2};
	
	const tfloat32 c_BufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };

	const tfloat32 c_expectBufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		
	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_BufferA,2 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
	audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,3);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,2 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithQuadrophonicInSingleBuffer)
{
	const tint c_channels[1] = {4};
	
	const tfloat32 c_BufferA[4 * 5] = {
		10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f,
		20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f
	};

	const tfloat32 c_expectBufferA[4 * 5] = {
		10.0f, 11.0f, 12.0f, 13.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f	
	};
		
	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_BufferA,4 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
	audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,1);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,4 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithQuadrophonicInStereoBuffers)
{
	const tint c_channels[2] = {2, 2};
	
	const tfloat32 c_BufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };
	const tfloat32 c_BufferB[2 * 5] = { 20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f };

	const tfloat32 c_expectBufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 0.0f, 0.0f };
	const tfloat32 c_expectBufferB[2 * 5] = { 20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 0.0f, 0.0f };
		
	AudioHardwareBufferTester buffer(c_channels,2);
	memcpy(buffer.buffer(0),c_BufferA,2 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
	audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,4);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithQuadrophonicInMonoBuffers)
{
	const tint c_channels[4] = {1, 1, 1, 1};
	
	const tfloat32 c_BufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f };
	const tfloat32 c_BufferB[1 * 5] = { 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };
	const tfloat32 c_BufferC[1 * 5] = { 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };
	const tfloat32 c_BufferD[1 * 5] = { 25.0f, 26.0f, 27.0f, 28.0f, 29.0f };

	const tfloat32 c_expectBufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 0.0f };
	const tfloat32 c_expectBufferB[1 * 5] = { 15.0f, 16.0f, 17.0f, 18.0f, 0.0f };
	const tfloat32 c_expectBufferC[1 * 5] = { 20.0f, 21.0f, 22.0f, 23.0f, 0.0f };
	const tfloat32 c_expectBufferD[1 * 5] = { 25.0f, 26.0f, 27.0f, 28.0f, 0.0f };
	
	AudioHardwareBufferTester buffer(c_channels,4);
	memcpy(buffer.buffer(0),c_BufferA,1 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(1),c_BufferB,1 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(3),c_BufferD,1 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
	audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,4);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,1 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,1 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(3),c_expectBufferD,1 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithHetreogeneousBuffers)
{
	const tint c_channels[3] = {3, 2, 1};

	const tfloat32 c_BufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 7.0f,8.0f,9.0f, 10.0f,11.0f,12.0f, 13.0f,14.0f,15.0f };
	const tfloat32 c_BufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 21.0f,22.0f, 23.0f,24.0f, 25.0f,26.0f };
	const tfloat32 c_BufferC[1 * 5] = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };

	const tfloat32 c_expectBufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f };
	const tfloat32 c_expectBufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f };
	const tfloat32 c_expectBufferC[1 * 5] = { 26.0f, 27.0f, 0.0f, 0.0f, 0.0f };
	
	AudioHardwareBufferTester buffer(c_channels,3);
	memcpy(buffer.buffer(0),c_BufferA,3 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
	audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,2);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,3 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithHetreogeneousAndNoSilence)
{
	const tint c_channels[3] = {3, 2, 1};

	const tfloat32 c_BufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 7.0f,8.0f,9.0f, 10.0f,11.0f,12.0f, 13.0f,14.0f,15.0f };
	const tfloat32 c_BufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 21.0f,22.0f, 23.0f,24.0f, 25.0f,26.0f };
	const tfloat32 c_BufferC[1 * 5] = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };

	const tfloat32 c_expectBufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 7.0f,8.0f,9.0f, 10.0f,11.0f,12.0f, 13.0f,14.0f,15.0f };
	const tfloat32 c_expectBufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 21.0f,22.0f, 23.0f,24.0f, 25.0f,26.0f };
	const tfloat32 c_expectBufferC[1 * 5] = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };
	
	AudioHardwareBufferTester buffer(c_channels,3);
	memcpy(buffer.buffer(0),c_BufferA,3 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
	audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,5);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,3 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceToEndOfBufferWithHetreogeneousAndFullSilence)
{
	const tint c_channels[3] = {3, 2, 1};

	const tfloat32 c_BufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 7.0f,8.0f,9.0f, 10.0f,11.0f,12.0f, 13.0f,14.0f,15.0f };
	const tfloat32 c_BufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 21.0f,22.0f, 23.0f,24.0f, 25.0f,26.0f };
	const tfloat32 c_BufferC[1 * 5] = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };

	const tfloat32 c_expectBufferA[3 * 5] = { 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f };
	const tfloat32 c_expectBufferB[2 * 5] = { 0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f };
	const tfloat32 c_expectBufferC[1 * 5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	
	AudioHardwareBufferTester buffer(c_channels,3);
	memcpy(buffer.buffer(0),c_BufferA,3 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceToEndOfBufferTest audio;
	audio.testWriteToAudioSilenceToEndOfBuffer(&buffer,0);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,3 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

class AOBaseLengthOfTimeTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getFrequency,tint());
		common::TimeStamp testLengthOfTime(tint noSamples) const;
};

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBaseLengthOfTimeTest::testLengthOfTime(tint noSamples) const
{
	return lengthOfTime(noSamples);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,lengthOfTime)
{
	common::TimeStamp expectA(10.0 / 44100.0);
	common::TimeStamp expectB(400.0 / 48000.0);
	common::TimeStamp expectC(90000.0 / 192000.0);
	
	AOBaseLengthOfTimeTest audio;
    EXPECT_CALL(audio,getFrequency()).Times(3).WillOnce(Return(44100))
		.WillOnce(Return(48000))
		.WillOnce(Return(192000));
	
	EXPECT_EQ(expectA,audio.testLengthOfTime(10));
	EXPECT_EQ(expectB,audio.testLengthOfTime(400));
	EXPECT_EQ(expectC,audio.testLengthOfTime(90000));
}

//-------------------------------------------------------------------------------------------

class AOBasePlaybackOfNextTrackIsStartingTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getFrequency,tint());
		MOCK_METHOD1(setAudioStartClock,void(const common::TimeStamp& t));
		MOCK_METHOD1(setCurrentOutTime,void(const common::TimeStamp& v));
		MOCK_METHOD1(setCurrentPlayTime,void(const common::TimeStamp& v));
		MOCK_METHOD1(setCurrentCallbackTime,void(const common::TimeStamp& v));
		MOCK_CONST_METHOD0(getTrackTimeStateFlag,bool());
		MOCK_METHOD1(setTrackTimeState,void(tint state));
		MOCK_METHOD1(setTrackTimeStateFlag,void(bool flag));
		MOCK_METHOD1(setStartNextTrackFlag,void(bool f));
		void testPlaybackOfNextTrackIsStarting(const RData::Part& part,const IOTimeStamp& systemTime,tint sIndex);
};

//-------------------------------------------------------------------------------------------

void AOBasePlaybackOfNextTrackIsStartingTest::testPlaybackOfNextTrackIsStarting(const RData::Part& part,const IOTimeStamp& systemTime,tint sIndex)
{
	playbackOfNextTrackIsStarting(part,systemTime,sIndex);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,playbackOfNextTrackIsStartingPartIsNotNext)
{
	common::TimeStamp sysTime(5.0);
    IOTimeStamp systemTime(true,sysTime);

	RDataPartMock part;
	EXPECT_CALL(part,isNext()).Times(1).WillOnce(Return(false));

	AOBasePlaybackOfNextTrackIsStartingTest audio;
	
	audio.testPlaybackOfNextTrackIsStarting(part,systemTime,40);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,playbackOfNextTrackIsStartingPartIsNextAndTimeState)
{
	common::TimeStamp partStart(3.0);
	common::TimeStamp sysTime(5.0);
	common::TimeStamp actualAudioStartClock,expectAudioStartClock(5.0 + 2.0 - 3.0);
	common::TimeStamp actualCurrentOutTime,actualPlayTime,actualCurrentCallbackTime;
	
    IOTimeStamp systemTime(true,sysTime);

	RDataPartMock part;
	EXPECT_CALL(part,isNext()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(part,startConst()).Times(4).WillRepeatedly(ReturnRef(partStart));

	AOBasePlaybackOfNextTrackIsStartingTest audio;
	EXPECT_CALL(audio,getFrequency()).Times(1).WillOnce(Return(20));
    EXPECT_CALL(audio,setAudioStartClock(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualAudioStartClock));
	EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentOutTime));
	EXPECT_CALL(audio,setCurrentPlayTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualPlayTime));
	EXPECT_CALL(audio,setCurrentCallbackTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentCallbackTime));
	EXPECT_CALL(audio,getTrackTimeStateFlag()).Times(1).WillOnce(Return(true));
	EXPECT_CALL(audio,setTrackTimeStateFlag(false)).Times(1);
	EXPECT_CALL(audio,setStartNextTrackFlag(true)).Times(1);
	
	audio.testPlaybackOfNextTrackIsStarting(part,systemTime,40);
	
	EXPECT_EQ(expectAudioStartClock,actualAudioStartClock);
	EXPECT_EQ(partStart,actualCurrentOutTime);
	EXPECT_EQ(partStart,actualPlayTime);
	EXPECT_EQ(partStart,actualCurrentCallbackTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,playbackOfNextTrackIsStartingPartIsNextAndNotTimeState)
{
	common::TimeStamp partStart(3.0);
	common::TimeStamp sysTime(5.0);
	common::TimeStamp actualAudioStartClock,expectAudioStartClock(5.0 + 2.0 - 3.0);
	common::TimeStamp actualCurrentOutTime,actualPlayTime,actualCurrentCallbackTime;
	
    IOTimeStamp systemTime(true,sysTime);

	RDataPartMock part;
	EXPECT_CALL(part,isNext()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(part,startConst()).Times(4).WillRepeatedly(ReturnRef(partStart));

	AOBasePlaybackOfNextTrackIsStartingTest audio;
	EXPECT_CALL(audio,getFrequency()).Times(1).WillOnce(Return(20));
    EXPECT_CALL(audio,setAudioStartClock(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualAudioStartClock));
	EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentOutTime));
	EXPECT_CALL(audio,setCurrentPlayTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualPlayTime));
	EXPECT_CALL(audio,setCurrentCallbackTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentCallbackTime));
	EXPECT_CALL(audio,getTrackTimeStateFlag()).Times(1).WillOnce(Return(false));
	EXPECT_CALL(audio,setTrackTimeState(1)).Times(1);
	EXPECT_CALL(audio,setTrackTimeStateFlag(false)).Times(1);
	EXPECT_CALL(audio,setStartNextTrackFlag(true)).Times(1);
	
	audio.testPlaybackOfNextTrackIsStarting(part,systemTime,40);
	
	EXPECT_EQ(expectAudioStartClock,actualAudioStartClock);
	EXPECT_EQ(partStart,actualCurrentOutTime);
	EXPECT_EQ(partStart,actualPlayTime);
	EXPECT_EQ(partStart,actualCurrentCallbackTime);	
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteToAudioSilenceForRemainder : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getFrequency,tint());
		MOCK_CONST_METHOD0(getCurrentOutTime,const common::TimeStamp&());
		MOCK_METHOD1(setCurrentOutTime,void(const common::TimeStamp& v));
		void testWriteToAudioSilenceForRemainder(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex);
};

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioSilenceForRemainder::testWriteToAudioSilenceForRemainder(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex)
{
	writeToAudioSilenceForRemainder(pBuffer,fromIndex);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceForRemainder)
{
	const tint c_channels[3] = {3, 2, 1};

	const tfloat32 c_BufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 7.0f,8.0f,9.0f, 10.0f,11.0f,12.0f, 13.0f,14.0f,15.0f };
	const tfloat32 c_BufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 21.0f,22.0f, 23.0f,24.0f, 25.0f,26.0f };
	const tfloat32 c_BufferC[1 * 5] = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };

	const tfloat32 c_expectBufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f };
	const tfloat32 c_expectBufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 0.0f,0.0f, 0.0f,0.0f, 0.0f,0.0f };
	const tfloat32 c_expectBufferC[1 * 5] = { 26.0f, 27.0f, 0.0f, 0.0f, 0.0f };
	
	AudioHardwareBufferTester buffer(c_channels,3);
	memcpy(buffer.buffer(0),c_BufferA,3 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));
	
    common::TimeStamp actualCurrentOutTime(3.0);
	common::TimeStamp expectDT = 3.0 / 20.0;
	common::TimeStamp expectCurrentOutTime = actualCurrentOutTime + expectDT;
	
	AOBaseWriteToAudioSilenceForRemainder audio;
	EXPECT_CALL(audio,getFrequency()).Times(1).WillOnce(Return(20));
	EXPECT_CALL(audio,getCurrentOutTime()).Times(1).WillOnce(ReturnRef(actualCurrentOutTime));
	EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentOutTime));
	
	audio.testWriteToAudioSilenceForRemainder(&buffer,2);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,3 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
	
	EXPECT_EQ(expectCurrentOutTime,actualCurrentOutTime);
}

//-------------------------------------------------------------------------------------------

class AOBasePartBufferIndexForChannelTest : public AOBaseTest
{
	public:
        AOBasePartBufferIndexForChannelTest(const tint *chArray,tint len);
		tint testPartBufferIndexForChannel(tint channelIndex) const;
};

//-------------------------------------------------------------------------------------------

AOBasePartBufferIndexForChannelTest::AOBasePartBufferIndexForChannelTest(const tint *chArray,tint len) : AOBaseTest()
{
	tint i;
	for(i=0;i<len;i++)
	{
		m_outputChannelArray[i] = chArray[i];
	}
	while(i<c_kMaxOutputChannels)
	{
		m_outputChannelArray[i] = -1;
		i++;
	}
}

//-------------------------------------------------------------------------------------------

tint AOBasePartBufferIndexForChannelTest::testPartBufferIndexForChannel(tint channelIndex) const
{
	return partBufferIndexForChannel(channelIndex);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,partBufferIndexForChannel)
{
	const tint c_chArray[10] = {
		0, 1, -1, -1, 5, 4, 2, 3, -1, 7
	};
	
	AOBasePartBufferIndexForChannelTest audio(c_chArray,10);
	
	EXPECT_EQ(0,audio.testPartBufferIndexForChannel(0));
	EXPECT_EQ(1,audio.testPartBufferIndexForChannel(1));
	EXPECT_EQ(-1,audio.testPartBufferIndexForChannel(2));
	EXPECT_EQ(-1,audio.testPartBufferIndexForChannel(3));
	EXPECT_EQ(5,audio.testPartBufferIndexForChannel(4));
	EXPECT_EQ(4,audio.testPartBufferIndexForChannel(5));
	EXPECT_EQ(2,audio.testPartBufferIndexForChannel(6));
	EXPECT_EQ(3,audio.testPartBufferIndexForChannel(7));
	EXPECT_EQ(-1,audio.testPartBufferIndexForChannel(8));
	EXPECT_EQ(7,audio.testPartBufferIndexForChannel(9));
	
	EXPECT_EQ(-1,audio.testPartBufferIndexForChannel(-1));
	EXPECT_EQ(-1,audio.testPartBufferIndexForChannel(c_kMaxOutputChannels));
	EXPECT_EQ(-1,audio.testPartBufferIndexForChannel(c_kMaxOutputChannels+1));
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteToAudioOutputBufferSilence : public AOBaseTest
{
	public:
		void testWriteToAudioOutputBufferSilence(AbstractAudioHardwareBuffer *pBuffer,tint bufferIndex,tint outChannelIndex,tint outputSampleIndex,tint amount);
};

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioOutputBufferSilence::testWriteToAudioOutputBufferSilence(AbstractAudioHardwareBuffer *pBuffer,tint bufferIndex,tint outChannelIndex,tint outputSampleIndex,tint amount)
{
	writeToAudioOutputBufferSilence(pBuffer,bufferIndex,outChannelIndex,outputSampleIndex,amount);
}

//-------------------------------------------------------------------------------------------

void writeToAudioOutputBufferSilenceTester(const tuint16 *originalOutputBuffer,const tuint16 *expectOutputBuffer,
	tint outputNoSamples,tint outputNoChannels,tint channelIndex,tint sampleIndex,tint amount)
{
	tbyte *actualBuffer = new tbyte [outputNoSamples * outputNoChannels * sizeof(tuint16)];
	memcpy(actualBuffer,originalOutputBuffer,outputNoSamples * outputNoChannels * sizeof(tuint16));
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,buffer(3)).Times(1).WillOnce(Return(actualBuffer));
    EXPECT_CALL(buffer,sampleSize(3)).Times(2).WillRepeatedly(Return(2));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(3)).Times(2).WillRepeatedly(Return(outputNoChannels));
	
	AOBaseWriteToAudioOutputBufferSilence audio;
    audio.testWriteToAudioOutputBufferSilence(&buffer,3,channelIndex,sampleIndex,amount);
	
	EXPECT_EQ(0,memcmp(expectOutputBuffer,actualBuffer,outputNoSamples * outputNoChannels * sizeof(tuint16)));
	
	delete [] actualBuffer;
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceStartToEnd1Channel)
{
	const tint c_outputNoSamples = 16;
	const tint c_outputNoChannels = 1;
	const tint c_channelIndex = 0;
	const tint c_sampleIndex = 0;
	const tint c_amount = 16;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceStartToMiddle1Channel)
{
	const tint c_outputNoSamples = 16;
	const tint c_outputNoChannels = 1;
	const tint c_channelIndex = 0;
	const tint c_sampleIndex = 0;
	const tint c_amount = 7;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceMiddleToEnd1Channel)
{
	const tint c_outputNoSamples = 16;
	const tint c_outputNoChannels = 1;
	const tint c_channelIndex = 0;
	const tint c_sampleIndex = 9;
	const tint c_amount = 7;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceInMiddle1Channel)
{
	const tint c_outputNoSamples = 16;
	const tint c_outputNoChannels = 1;
	const tint c_channelIndex = 0;
	const tint c_sampleIndex = 4;
	const tint c_amount = 8;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceAtEnd1Channel)
{
	const tint c_outputNoSamples = 16;
	const tint c_outputNoChannels = 1;
	const tint c_channelIndex = 0;
	const tint c_sampleIndex = 15;
	const tint c_amount = 1;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x0000
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceStartToEnd2Channels)
{
	const tint c_outputNoSamples = 8;
	const tint c_outputNoChannels = 2;
	const tint c_channelIndex = 1;
	const tint c_sampleIndex = 0;
	const tint c_amount = 8;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x0000, 0x89ab, 0x0000,
		0xfedc, 0x0000, 0x7654, 0x0000,
		0x1f2e, 0x0000, 0x5b6a, 0x0000,
		0x8879, 0x0000, 0x4c3d, 0x0000
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceStartToMiddle2Channels)
{
	const tint c_outputNoSamples = 8;
	const tint c_outputNoChannels = 2;
	const tint c_channelIndex = 0;
	const tint c_sampleIndex = 0;
	const tint c_amount = 4;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x0000, 0x4567, 0x0000, 0xcdef,
		0x0000, 0xba98, 0x0000, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceMiddleToEnd2Channels)
{
	const tint c_outputNoSamples = 8;
	const tint c_outputNoChannels = 2;
	const tint c_channelIndex = 1;
	const tint c_sampleIndex = 5;
	const tint c_amount = 3;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x0000,
		0x8879, 0x0000, 0x4c3d, 0x0000
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceInMiddle2Channels)
{
	const tint c_outputNoSamples = 8;
	const tint c_outputNoChannels = 2;
	const tint c_channelIndex = 0;
	const tint c_sampleIndex = 2;
	const tint c_amount = 3;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0x0000, 0xba98, 0x0000, 0x4321,
		0x0000, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceAtEnd2Channels)
{
	const tint c_outputNoSamples = 8;
	const tint c_outputNoChannels = 2;
	const tint c_channelIndex = 1;
	const tint c_sampleIndex = 7;
	const tint c_amount = 1;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x0000
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceStartToEnd4Channels)
{
	const tint c_outputNoSamples = 4;
	const tint c_outputNoChannels = 4;
	const tint c_channelIndex = 2;
	const tint c_sampleIndex = 0;
	const tint c_amount = 4;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x0000, 0xcdef,
		0xfedc, 0xba98, 0x0000, 0x4321,
		0x1f2e, 0x3d4c, 0x0000, 0x7988,
		0x8879, 0x6a5b, 0x0000, 0x2e1f
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferSilenceStartToEnd8Channels)
{
	const tint c_outputNoSamples = 2;
	const tint c_outputNoChannels = 8;
	const tint c_channelIndex = 5;
	const tint c_sampleIndex = 0;
	const tint c_amount = 2;

	const tuint16 c_originalOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0xba98, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x6a5b, 0x4c3d, 0x2e1f
	};
	
	const tuint16 c_expectOutputBuffer[c_outputNoSamples * c_outputNoChannels] = {
		0x1234, 0x4567, 0x89ab, 0xcdef,
		0xfedc, 0x0000, 0x7654, 0x4321,
		0x1f2e, 0x3d4c, 0x5b6a, 0x7988,
		0x8879, 0x0000, 0x4c3d, 0x2e1f
	};
	
	writeToAudioOutputBufferSilenceTester(c_originalOutputBuffer,c_expectOutputBuffer,c_outputNoSamples,c_outputNoChannels,c_channelIndex,c_sampleIndex,c_amount);
}

//-------------------------------------------------------------------------------------------

class AOBaseTestWriteToAudioOutputBufferTest : public AOBasePartBufferIndexForChannelTest
{
	public:
		AOBaseTestWriteToAudioOutputBufferTest(const tint *chArray,tint len);
	
		MOCK_METHOD9(writeToAudioOutputBufferFromPartData,void(AbstractAudioHardwareBuffer *pBuffer,const engine::RData *data,tint partNumber,tint inChannelIndex,tint bufferIndex,tint outChannelIndex,tint inputSampleIndex,tint outputSampleIndex,tint amount));
		MOCK_METHOD5(writeToAudioOutputBufferSilence,void(AbstractAudioHardwareBuffer *pBuffer,tint bufferIndex,tint outChannelIndex,tint outputSampleIndex,tint amount));

		void testWriteToAudioOutputBuffer(AbstractAudioHardwareBuffer *pBuffer,
                                           engine::RData *data,
                                           tint partNumber,
                                           tint inputSampleIndex,
                                           tint outputSampleIndex,
                                           tint amount);
};

//-------------------------------------------------------------------------------------------

AOBaseTestWriteToAudioOutputBufferTest::AOBaseTestWriteToAudioOutputBufferTest(const tint *chArray,tint len) : AOBasePartBufferIndexForChannelTest(chArray,len)
{}

//-------------------------------------------------------------------------------------------

void AOBaseTestWriteToAudioOutputBufferTest::testWriteToAudioOutputBuffer(AbstractAudioHardwareBuffer *pBuffer,
                                           engine::RData *data,
                                           tint partNumber,
                                           tint inputSampleIndex,
                                           tint outputSampleIndex,
                                           tint amount)
{
	writeToAudioOutputBuffer(pBuffer,data,partNumber,inputSampleIndex,outputSampleIndex,amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferNoAudioBuffers)
{
	const tint c_channel[1] = { 3 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(0));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,1);

	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,20);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferBuffersWithNoChannels)
{
	const tint c_channel[1] = { 3 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(1));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(0));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,1);

	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,20);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferMonoBufferWithChannelIndexMapped)
{
	const tint c_channel[1] = { 3 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(1));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(1));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,3,0,0,7,3,20)).Times(1);

	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,20);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferMonoBufferWithNoChannelIndexMapped)
{
	const tint c_channel[1] = { -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(1));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(1));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,0,3,20)).Times(1);

	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,20);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoBufferWithBothChannelIndexMapped)
{
	const tint c_channel[2] = { 0, 1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(1));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(2));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,2);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,0,7,3,15)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,1,7,3,15)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,15);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoBufferWithBothHavingNoChannelIndexMapped)
{
	const tint c_channel[2] = { -1, -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(1));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(2));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,2);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,0,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,1,3,20)).Times(1);

	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,20);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo2Of8OutputsAsMonoBuffers)
{
	const tint c_channel[8] = { 1, 0, -1, -1, -1, -1, -1, -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(8));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(1));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,0,7,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,1,0,7,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,2,0,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,3,0,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,4,0,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,5,0,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,6,0,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,7,0,3,20)).Times(1);

	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,20);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo4Of8OutputsAsMonoBuffers)
{
	const tint c_channel[8] = { 0, 1, 1, 0, -1, -1, -1, -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(8));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(1));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,0,7,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,1,0,7,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,2,0,7,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,3,0,7,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,4,0,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,5,0,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,6,0,3,20)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,7,0,3,20)).Times(1);

	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,20);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo6Of8OutputsAsMonoBuffers)
{
	const tint c_channel[8] = { 0, 1, 0, 1, 0, 1, -1, -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(8));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(1));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,1,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,2,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,3,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,4,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,5,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,6,0,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,7,0,3,10)).Times(1);

	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo8Of8OutputsAsMonoBuffers)
{
	const tint c_channel[8] = { 0, 1, 0, 1, 0, 1, 0, 1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(8));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(1));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,1,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,2,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,3,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,4,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,5,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,6,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,7,0,7,3,10)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBuffer6ChSurroundDataMappedTo6Of8OutputsAsMonoBuffers)
{
	const tint c_channel[8] = { 5, 4, 3, 2, 1, 0, -1, -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(8));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(1));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,5,0,0,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,4,1,0,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,3,2,0,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,2,3,0,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,1,4,0,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,0,5,0,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,6,0,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,7,0,5,10)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,3,4,5,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo2Of8OutputsAsStereoBuffers)
{
	const tint c_channel[8] = { 0, 1, -1, -1, -1, -1, -1, -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(4));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(2));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,1,0,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,1,1,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,2,0,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,2,1,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,3,0,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,3,1,3,10)).Times(1);

	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo4Of8OutputsAsStereoBuffers)
{
	const tint c_channel[8] = { -1, -1, 0, 1, 0, 1, -1, -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(4));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(2));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,0,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,1,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,1,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,1,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,2,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,2,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,3,0,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,3,1,3,10)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo6Of8OutputsAsStereoBuffers)
{
	const tint c_channel[8] = { 0, 1, 0, 1, 0, 1, -1, -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(4));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(2));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,1,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,1,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,2,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,2,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,3,0,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,3,1,3,10)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo8Of8OutputsAsStereoBuffers)
{
	const tint c_channel[8] = { 0, 1, 0, 1, 0, 1, 0, 1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(4));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(2));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,1,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,1,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,2,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,2,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,3,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,3,1,7,3,10)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBuffer6ChSurroundDataMappedTo6Of8OutputsAsStereoBuffers)
{
	const tint c_channel[8] = { 3, 4, 1, -1, 0, -1, 2, 5 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(4));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(2));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,3,0,0,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,4,0,1,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,1,1,0,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,1,1,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,0,2,0,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,2,1,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,2,3,0,4,5,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,3,5,3,1,4,5,10)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,3,4,5,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo2Of8OutputsAsOctaBuffer)
{
	const tint c_channel[8] = { 2, 3, -1, -1, -1, -1, -1, -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(1));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(8));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,2,0,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,3,0,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,2,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,3,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,4,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,5,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,6,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,7,3,10)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo4Of8OutputsAsOctaBuffer)
{
	const tint c_channel[8] = { 0, 1, 0, 1, -1, -1, -1, -1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(1));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(8));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,2,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,3,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,4,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,5,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,6,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,7,3,10)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo6Of8OutputsAsOctaBuffer)
{
	const tint c_channel[8] = { 0, -1, 0, 0, -1, 1, 0, 0 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(1));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(8));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,1,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,2,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,3,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferSilence(&buffer,0,4,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,5,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,6,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,7,7,3,10)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,10);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoDataMappedTo8Of8OutputsAsOctaBuffer)
{
	const tint c_channel[8] = { 0, 1, 0, 1, 0, 1, 0, 1 };
	
	engine::RData data;
	
	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,numberOfBuffers()).WillRepeatedly(Return(1));
	EXPECT_CALL(buffer,numberOfChannelsInBuffer(A<tint>())).WillRepeatedly(Return(8));
	
	AOBaseTestWriteToAudioOutputBufferTest audio(c_channel,8);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,0,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,1,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,2,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,3,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,4,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,5,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,0,0,6,7,3,10)).Times(1);
	EXPECT_CALL(audio,writeToAudioOutputBufferFromPartData(&buffer,&data,4,1,0,7,7,3,10)).Times(1);
	
	audio.testWriteToAudioOutputBuffer(&buffer,&data,4,7,3,10);
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteToAudioOutputBufferWithSampleConverterTest : public AOBasePartBufferIndexForChannelTest
{
	public:
		AOBaseWriteToAudioOutputBufferWithSampleConverterTest(const tint *chArray,tint len,tint noBits,bool isLittleEndian);
		virtual ~AOBaseWriteToAudioOutputBufferWithSampleConverterTest();

		void testWriteToAudioOutputBuffer(AbstractAudioHardwareBuffer *pBuffer,
                                           engine::RData *data,
                                           tint partNumber,
                                           tint inputSampleIndex,
                                           tint outputSampleIndex,
                                           tint amount);

	protected:
	
		SampleConverter *m_outputConverter;
		
		virtual void writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,const engine::RData *data,
			tint partNumber,tint inChannelIndex,tint bufferIndex,tint outChannelIndex,tint inputSampleIndex,tint outputSampleIndex,tint amount);
};

//-------------------------------------------------------------------------------------------

AOBaseWriteToAudioOutputBufferWithSampleConverterTest::AOBaseWriteToAudioOutputBufferWithSampleConverterTest(const tint *chArray,tint len,tint noBits,bool isLittleEndian) : AOBasePartBufferIndexForChannelTest(chArray,len)
{
    m_outputConverter = new SampleConverter(noBits,noBits >> 3,isLittleEndian,false,true);
}

//-------------------------------------------------------------------------------------------

AOBaseWriteToAudioOutputBufferWithSampleConverterTest::~AOBaseWriteToAudioOutputBufferWithSampleConverterTest()
{
	delete m_outputConverter;
}

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioOutputBufferWithSampleConverterTest::writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,const engine::RData *data,
		tint partNumber,tint inChannelIndex,tint bufferIndex,tint outChannelIndex,tint inputSampleIndex,tint outputSampleIndex,tint amount)
{
	const sample_t *input = data->partDataOutConst(partNumber);
	tint iIdx = (inputSampleIndex * data->noOutChannels()) + inChannelIndex;

	tbyte *out = pBuffer->buffer(bufferIndex);
	tint oIdx = ((outputSampleIndex * pBuffer->numberOfChannelsInBuffer(bufferIndex)) + outChannelIndex) * pBuffer->sampleSize(bufferIndex);

	m_outputConverter->setNumberOfInputChannels(data->noOutChannels());
	m_outputConverter->setNumberOfOutputChannels(pBuffer->numberOfChannelsInBuffer(bufferIndex));
	
	m_outputConverter->convert(&input[iIdx],&out[oIdx],amount);
}

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioOutputBufferWithSampleConverterTest::testWriteToAudioOutputBuffer(AbstractAudioHardwareBuffer *pBuffer,
                                           engine::RData *data,
                                           tint partNumber,
                                           tint inputSampleIndex,
                                           tint outputSampleIndex,
                                           tint amount)
{
	writeToAudioOutputBuffer(pBuffer,data,partNumber,inputSampleIndex,outputSampleIndex,amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoPartToSingleStereoBufferAt16BitsLE)
{
	const tint c_channelsAudio[2] = {0, 1};
	const tint c_channelsBuffer[1] = { 2 };
	
#if defined(SINGLE_FLOAT_SAMPLE)
	const sample_t c_inputSamples[9 * 2] = {
		 1.0f,  0.8f, // 0
		 0.6f,  0.4f, // 1
		 0.2f, -0.2f, // 2
		-0.6f, -0.8f, // 3
		-1.0f,  1.0f, // 4
		 0.8f,  0.6f, // 5
		 0.4f,  0.2f, // 6
		-0.2f, -0.4f, // 7
		-0.6f, -0.8f  // 8
	};
#else
	const sample_t c_inputSamples[9 * 2] = {
		 1.0,  0.8, // 0
		 0.6,  0.4, // 1
		 0.2, -0.2, // 2
		-0.6, -0.8, // 3
		-1.0,  1.0, // 4
		 0.8,  0.6, // 5
		 0.4,  0.2, // 6
		-0.2, -0.4, // 7
		-0.6, -0.8  // 8
	};
#endif

	const tubyte c_expectedOutputBuffer[9 * 2 * 2] = {
		0xff, 0x7f, 0x66, 0x66, // 0
		0xcc, 0x4c, 0x33, 0x33, // 1
		0x99, 0x19, 0x66, 0xe6, // 2
		0x33, 0xb3, 0x9a, 0x99, // 3
		0x00, 0x80, 0xff, 0x7f, // 4
		0x66, 0x66, 0xcc, 0x4c, // 5
		0x33, 0x33, 0x99, 0x19, // 6
		0x66, 0xe6, 0xcd, 0xcc, // 7
		0x33, 0xb3, 0x9a, 0x99  // 8
	};
	
	engine::RData data(9,2,2);
	engine::RData::Part& part = data.nextPart();
	part.length() = 9;
	part.done() = true;
    memcpy(data.partDataOut(0),c_inputSamples,9 * 2 * sizeof(sample_t));
	
	AudioHardwareBufferTester buffer(c_channelsBuffer,1,2,9);
    AOBaseWriteToAudioOutputBufferWithSampleConverterTest audio(c_channelsAudio,2,16,true);
	
    audio.testWriteToAudioOutputBuffer(&buffer,&data,0,0,0,9);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectedOutputBuffer,9 * 2 * 2));
}

//-------------------------------------------------------------------------------------------
#if !defined(SINGLE_FLOAT_SAMPLE)
//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoPartToSingleQuadrophonicBufferAt24BitsLE)
{
    const tint c_channelsAudio[4] = {1, 0, -1, -1};
	const tint c_channelsBuffer[1] = { 4 };

	const sample_t c_inputSamples[9 * 2] = {
		 1.0,  0.8, // 0
		 0.6,  0.4, // 1
		 0.2, -0.2, // 2
		-0.6, -0.8, // 3
		-1.0,  1.0, // 4
		 0.8,  0.6, // 5
		 0.4,  0.2, // 6
		-0.2, -0.4, // 7
		-0.6, -0.8  // 8
	};
	
	const tubyte c_expectedOutputBuffer[9 * 3 * 4] = {
		0x66, 0x66, 0x66, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 0
		0x33, 0x33, 0x33, 0xcc, 0xcc, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 1
		0x66, 0x66, 0xe6, 0x99, 0x99, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 2
		0x9a, 0x99, 0x99, 0x33, 0x33, 0xb3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 3
		0xff, 0xff, 0x7f, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 4
		0xcc, 0xcc, 0x4c, 0x66, 0x66, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 5
		0x99, 0x99, 0x19, 0x33, 0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 6
		0xcd, 0xcc, 0xcc, 0x66, 0x66, 0xe6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 7
		0x9a, 0x99, 0x99, 0x33, 0x33, 0xb3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // 8
	};
	
	engine::RData data(9,2,2);
	engine::RData::Part& part = data.nextPart();
	part.length() = 9;
	part.done() = true;
    memcpy(data.partDataOut(0),c_inputSamples,9 * 2 * sizeof(sample_t));
	
	AudioHardwareBufferTester buffer(c_channelsBuffer,1,3,9);
    AOBaseWriteToAudioOutputBufferWithSampleConverterTest audio(c_channelsAudio,4,24,true);
	
    audio.testWriteToAudioOutputBuffer(&buffer,&data,0,0,0,9);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectedOutputBuffer,9 * 3 * 4));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioOutputBufferStereoPartToFourMonoBuffersAt32BitsBE)
{
    const tint c_channelsAudio[4] = {-1, 0, -1, 1};
	const tint c_channelsBuffer[4] = { 1, 1, 1, 1 };
	
	const sample_t c_inputSamples[9 * 2] = {
		 1.0,  0.8, // 0
		 0.6,  0.4, // 1
		 0.2, -0.2, // 2
		-0.6, -0.8, // 3
		-1.0,  1.0, // 4
		 0.8,  0.6, // 5
		 0.4,  0.2, // 6
		-0.2, -0.4, // 7
		-0.6, -0.8  // 8
	};

	const tubyte c_expectedOutputBufferA[9 * 4] = {
		0x00, 0x00, 0x00, 0x00, // 0
		0x00, 0x00, 0x00, 0x00, // 1
		0x00, 0x00, 0x00, 0x00, // 2
		0x00, 0x00, 0x00, 0x00, // 3
		0x00, 0x00, 0x00, 0x00, // 4
		0x00, 0x00, 0x00, 0x00, // 5
		0x00, 0x00, 0x00, 0x00, // 6
		0x00, 0x00, 0x00, 0x00, // 7
		0x00, 0x00, 0x00, 0x00, // 8
	};
	
	const tubyte c_expectedOutputBufferB[9 * 4] = {
		0x7f, 0xff, 0xff, 0xff, // 0
		0x4c, 0xcc, 0xcc, 0xcc, // 1
		0x19, 0x99, 0x99, 0x99, // 2
		0xb3, 0x33, 0x33, 0x33, // 3
		0x80, 0x00, 0x00, 0x00, // 4
		0x66, 0x66, 0x66, 0x66, // 5
		0x33, 0x33, 0x33, 0x33, // 6
		0xe6, 0x66, 0x66, 0x66, // 7
		0xb3, 0x33, 0x33, 0x33, // 8
	};
	
	const tubyte c_expectedOutputBufferC[9 * 4] = {
		0x00, 0x00, 0x00, 0x00, // 0
		0x00, 0x00, 0x00, 0x00, // 1
		0x00, 0x00, 0x00, 0x00, // 2
		0x00, 0x00, 0x00, 0x00, // 3
		0x00, 0x00, 0x00, 0x00, // 4
		0x00, 0x00, 0x00, 0x00, // 5
		0x00, 0x00, 0x00, 0x00, // 6
		0x00, 0x00, 0x00, 0x00, // 7
		0x00, 0x00, 0x00, 0x00, // 8
	};
	
	const tubyte c_expectedOutputBufferD[9 * 4] = {
		0x66, 0x66, 0x66, 0x66, // 0
		0x33, 0x33, 0x33, 0x33, // 1
		0xe6, 0x66, 0x66, 0x66, // 2
		0x99, 0x99, 0x99, 0x9a, // 3
		0x7f, 0xff, 0xff, 0xff, // 4
		0x4c, 0xcc, 0xcc, 0xcc, // 5
		0x19, 0x99, 0x99, 0x99, // 6
		0xcc, 0xcc, 0xcc, 0xcd, // 7
		0x99, 0x99, 0x99, 0x9a, // 8
	};

	engine::RData data(9,2,2);
	engine::RData::Part& part = data.nextPart();
	part.length() = 9;
	part.done() = true;
    memcpy(data.partDataOut(0),c_inputSamples,9 * 2 * sizeof(sample_t));
	
	AudioHardwareBufferTester buffer(c_channelsBuffer,4,4,9);
    AOBaseWriteToAudioOutputBufferWithSampleConverterTest audio(c_channelsAudio,4,32,false);
	
    audio.testWriteToAudioOutputBuffer(&buffer,&data,0,0,0,9);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectedOutputBufferA,9 * 4));
	EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectedOutputBufferB,9 * 4));
	EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectedOutputBufferC,9 * 4));
	EXPECT_EQ(0,memcmp(buffer.buffer(3),c_expectedOutputBufferD,9 * 4));
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

class AOBaseSyncAudioTimeToPartReferenceLatencyDelay : public AOBaseTest
{
	public:
		MOCK_METHOD1(setCurrentOutTime,void(const common::TimeStamp& v));
		MOCK_METHOD1(setAudioStartClock,void(const common::TimeStamp& t));
		MOCK_METHOD1(setCurrentPlayTime,void(const common::TimeStamp& v));
		void testSyncAudioTimeToPartReferenceLatencyDelay(engine::RData::Part& part,const IOTimeStamp& systemTime,const common::TimeStamp& referenceTime);
};

//-------------------------------------------------------------------------------------------

void AOBaseSyncAudioTimeToPartReferenceLatencyDelay::testSyncAudioTimeToPartReferenceLatencyDelay(engine::RData::Part& part,const IOTimeStamp& systemTime,const common::TimeStamp& referenceTime)
{
	syncAudioTimeToPartReferenceLatencyDelay(part,systemTime,referenceTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,syncAudioTimeToPartReferenceLatencyDelayWithInvalidSystemTime)
{
	common::TimeStamp referenceTime(10.0),partReferenceTime(7.0),partStartTime(2.0),hardwareSystemTime(14.0);
	common::TimeStamp actualCurrentOutTime,expectCurrentOutTime(5.0);
	common::TimeStamp actualCurrentPlayTime,expectCurrentPlayTime(5.0);

	IOTimeStamp systemTime(false,hardwareSystemTime);

	engine::RData data;
	engine::RData::Part& part = data.nextPart();
	part.refStartTime() = partReferenceTime;
	part.start() = partStartTime;
	
	AOBaseSyncAudioTimeToPartReferenceLatencyDelay audio;
	EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentOutTime));
	EXPECT_CALL(audio,setCurrentPlayTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentPlayTime));
	
	audio.testSyncAudioTimeToPartReferenceLatencyDelay(part,systemTime,referenceTime);
	
	EXPECT_EQ(expectCurrentOutTime,actualCurrentOutTime);
	EXPECT_EQ(expectCurrentPlayTime,actualCurrentPlayTime);
	EXPECT_EQ(0,part.refStartTime());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,syncAudioTimeToPartReferenceLatencyDelayWithValidSystemTime)
{
	common::TimeStamp referenceTime(10.0),partReferenceTime(7.0),partStartTime(2.0),hardwareSystemTime(14.0);
	common::TimeStamp actualCurrentOutTime,expectCurrentOutTime(5.0);
	common::TimeStamp actualAudioStartClock,expectAudioStartClock(9.0);
	common::TimeStamp actualCurrentPlayTime,expectCurrentPlayTime(5.0);

	IOTimeStamp systemTime(true,hardwareSystemTime);

	engine::RData data;
	engine::RData::Part& part = data.nextPart();
	part.refStartTime() = partReferenceTime;
	part.start() = partStartTime;
	
	AOBaseSyncAudioTimeToPartReferenceLatencyDelay audio;
	EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentOutTime));
	EXPECT_CALL(audio,setAudioStartClock(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualAudioStartClock));
	EXPECT_CALL(audio,setCurrentPlayTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentPlayTime));
	
	audio.testSyncAudioTimeToPartReferenceLatencyDelay(part,systemTime,referenceTime);
	
	EXPECT_EQ(expectCurrentOutTime,actualCurrentOutTime);
	EXPECT_EQ(expectAudioStartClock,actualAudioStartClock);
	EXPECT_EQ(expectCurrentPlayTime,actualCurrentPlayTime);
	EXPECT_EQ(0,part.refStartTime());
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteToAudioSilenceForGivenRangeTest : public AOBaseTest
{
	public:
		void testWriteToAudioSilenceForGivenRange(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex,tint numberOfSamples);
};

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioSilenceForGivenRangeTest::testWriteToAudioSilenceForGivenRange(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex,tint numberOfSamples)
{
	writeToAudioSilenceForGivenRange(pBuffer,fromIndex,numberOfSamples);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceForGivenRangeWithMonoAndNoSample)
{
	const tint c_channels[1] = {1};
	
	const tfloat32 c_BufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f };

	const tfloat32 c_expectBufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f };
		
	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_BufferA,1 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceForGivenRangeTest audio;
	audio.testWriteToAudioSilenceForGivenRange(&buffer,1,0);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,1 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceForGivenRangeWithMono)
{
	const tint c_channels[1] = {1};
	
	const tfloat32 c_BufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f };

	const tfloat32 c_expectBufferA[1 * 5] = { 10.0f, 0.0f, 0.0f, 13.0f, 14.0f };
		
	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_BufferA,1 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceForGivenRangeTest audio;
	audio.testWriteToAudioSilenceForGivenRange(&buffer,1,2);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,1 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceForGivenRangeWithStereo)
{
	const tint c_channels[1] = {2};
	
	const tfloat32 c_BufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };

	const tfloat32 c_expectBufferA[2 * 5] = { 10.0f, 11.0f, 0.0f, 0.0f, 0.0f, 0.0f, 16.0f, 17.0f, 18.0f, 19.0f };
		
	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_BufferA,2 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceForGivenRangeTest audio;
	audio.testWriteToAudioSilenceForGivenRange(&buffer,1,2);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,2 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceForGivenRangeWithQuadrophonicInSingleBuffer)
{
	const tint c_channels[1] = {4};
	
	const tfloat32 c_BufferA[4 * 5] = {
		10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f,
		20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f
	};

	const tfloat32 c_expectBufferA[4 * 5] = {
		10.0f, 11.0f, 12.0f, 13.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f
	};
		
	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_BufferA,4 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceForGivenRangeTest audio;
	audio.testWriteToAudioSilenceForGivenRange(&buffer,1,2);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,4 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceForGivenRangeWithQuadrophonicInStereoBuffers)
{
	const tint c_channels[2] = {2, 2};
	
	const tfloat32 c_BufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };
	const tfloat32 c_BufferB[2 * 5] = { 20.0f, 21.0f, 22.0f, 23.0f, 24.0f, 25.0f, 26.0f, 27.0f, 28.0f, 29.0f };

	const tfloat32 c_expectBufferA[2 * 5] = { 10.0f, 11.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 18.0f, 19.0f };
	const tfloat32 c_expectBufferB[2 * 5] = { 20.0f, 21.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 28.0f, 29.0f };
		
	AudioHardwareBufferTester buffer(c_channels,2);
	memcpy(buffer.buffer(0),c_BufferA,2 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceForGivenRangeTest audio;
	audio.testWriteToAudioSilenceForGivenRange(&buffer,1,3);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceForGivenRangeWithQuadrophonicInMonoBuffers)
{
	const tint c_channels[4] = {1, 1, 1, 1};
	
	const tfloat32 c_BufferA[1 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f };
	const tfloat32 c_BufferB[1 * 5] = { 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };
	const tfloat32 c_BufferC[1 * 5] = { 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };
	const tfloat32 c_BufferD[1 * 5] = { 25.0f, 26.0f, 27.0f, 28.0f, 29.0f };

	const tfloat32 c_expectBufferA[1 * 5] = { 0.0f, 11.0f, 12.0f, 13.0f, 14.0f };
	const tfloat32 c_expectBufferB[1 * 5] = { 0.0f, 16.0f, 17.0f, 18.0f, 19.0f };
	const tfloat32 c_expectBufferC[1 * 5] = { 0.0f, 21.0f, 22.0f, 23.0f, 24.0f };
	const tfloat32 c_expectBufferD[1 * 5] = { 0.0f, 26.0f, 27.0f, 28.0f, 29.0f };
	
	AudioHardwareBufferTester buffer(c_channels,4);
	memcpy(buffer.buffer(0),c_BufferA,1 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(1),c_BufferB,1 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(3),c_BufferD,1 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceForGivenRangeTest audio;
	audio.testWriteToAudioSilenceForGivenRange(&buffer,0,1);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,1 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,1 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(3),c_expectBufferD,1 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceForGivenRangeWithHetreogeneousBuffers)
{
	const tint c_channels[3] = {3, 2, 1};

	const tfloat32 c_BufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 7.0f,8.0f,9.0f, 10.0f,11.0f,12.0f, 13.0f,14.0f,15.0f };
	const tfloat32 c_BufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 21.0f,22.0f, 23.0f,24.0f, 25.0f,26.0f };
	const tfloat32 c_BufferC[1 * 5] = { 26.0f, 27.0f, 28.0f, 29.0f, 30.0f };

	const tfloat32 c_expectBufferA[3 * 5] = { 1.0f,2.0f,3.0f, 4.0f,5.0f,6.0f, 0.0f,0.0f,0.0f, 0.0f,0.0f,0.0f, 13.0f,14.0f,15.0f };
	const tfloat32 c_expectBufferB[2 * 5] = { 17.0f,18.0f, 19.0f,20.0f, 0.0f,0.0f, 0.0f,0.0f, 25.0f,26.0f };
	const tfloat32 c_expectBufferC[1 * 5] = { 26.0f, 27.0f, 0.0f, 0.0f, 30.0f };
	
	AudioHardwareBufferTester buffer(c_channels,3);
	memcpy(buffer.buffer(0),c_BufferA,3 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(1),c_BufferB,2 * 5 * sizeof(tfloat32));
	memcpy(buffer.buffer(2),c_BufferC,1 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceForGivenRangeTest audio;
	audio.testWriteToAudioSilenceForGivenRange(&buffer,2,2);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,3 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(1),c_expectBufferB,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(0,memcmp(buffer.buffer(2),c_expectBufferC,1 * 5 * sizeof(tfloat32)));
}

//-------------------------------------------------------------------------------------------

class AOBaseNumberOfSamplesInTimeTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getFrequency,tint());
		tint testNumberOfSamplesInTime(common::TimeStamp& t) const;
};

//-------------------------------------------------------------------------------------------

tint AOBaseNumberOfSamplesInTimeTest::testNumberOfSamplesInTime(common::TimeStamp& t) const
{
	return numberOfSamplesInTime(t);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,numberOfSamplesInTimeDiffFromWholeNumber)
{
	common::TimeStamp actualT(2.0),expectT(2.0);
	
	AOBaseNumberOfSamplesInTimeTest audio;
	EXPECT_CALL(audio,getFrequency()).Times(1).WillOnce(Return(10));
	EXPECT_EQ(20,audio.testNumberOfSamplesInTime(actualT));
	EXPECT_EQ(expectT,actualT);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,numberOfSamplesInTimeDiffFromQuarter)
{
	common::TimeStamp actualT(2.025),expectT(2.0);
	
	AOBaseNumberOfSamplesInTimeTest audio;
	EXPECT_CALL(audio,getFrequency()).Times(1).WillOnce(Return(10));
	EXPECT_EQ(20,audio.testNumberOfSamplesInTime(actualT));
	EXPECT_EQ(expectT,actualT);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,numberOfSamplesInTimeDiffFromHalf)
{
	common::TimeStamp actualT(2.05),expectT(2.0);
	
	AOBaseNumberOfSamplesInTimeTest audio;
	EXPECT_CALL(audio,getFrequency()).Times(1).WillOnce(Return(10));
	EXPECT_EQ(20,audio.testNumberOfSamplesInTime(actualT));
	EXPECT_EQ(expectT,actualT);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,numberOfSamplesInTimeDiffFromThreeQuarteres)
{
	common::TimeStamp actualT(2.075),expectT(2.0);
	
	AOBaseNumberOfSamplesInTimeTest audio;
	EXPECT_CALL(audio,getFrequency()).Times(1).WillOnce(Return(10));
	EXPECT_EQ(20,audio.testNumberOfSamplesInTime(actualT));
	EXPECT_EQ(expectT,actualT);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,numberOfSamplesInTimeDiffFromNearWholeNumber)
{
	common::TimeStamp actualT(2.0999999999),expectT(2.0);
	
	AOBaseNumberOfSamplesInTimeTest audio;
	EXPECT_CALL(audio,getFrequency()).Times(1).WillOnce(Return(10));
	EXPECT_EQ(20,audio.testNumberOfSamplesInTime(actualT));
	EXPECT_EQ(expectT,actualT);
}

//-------------------------------------------------------------------------------------------

class AOBaseTimeForNumberOfSamplesTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getFrequency,tint());
		common::TimeStamp testTimeForNumberOfSamples(tint numberOfSamples) const;
};

//-------------------------------------------------------------------------------------------

common::TimeStamp AOBaseTimeForNumberOfSamplesTest::testTimeForNumberOfSamples(tint numberOfSamples) const
{
	return timeForNumberOfSamples(numberOfSamples);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,timeForNumberOfSamples)
{
	common::TimeStamp expectT(20.0);
	
	AOBaseTimeForNumberOfSamplesTest audio;
	EXPECT_CALL(audio,getFrequency()).Times(1).WillOnce(Return(10));
	EXPECT_EQ(expectT,audio.testTimeForNumberOfSamples(200));
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteSilenceForSynchronizedLatencyDelayTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getFrequency,tint());
        MOCK_CONST_METHOD0(getCurrentOutTime,const common::TimeStamp&());
		MOCK_METHOD1(setCurrentOutTime,void(const common::TimeStamp& v));
		void testWriteSilenceForSynchronizedLatencyDelay(AbstractAudioHardwareBuffer *pBuffer,engine::RData::Part& part,const common::TimeStamp& referenceTime,tint& outputSampleIndex);
};

//-------------------------------------------------------------------------------------------

void AOBaseWriteSilenceForSynchronizedLatencyDelayTest::testWriteSilenceForSynchronizedLatencyDelay(AbstractAudioHardwareBuffer *pBuffer,engine::RData::Part& part,const common::TimeStamp& referenceTime,tint& outputSampleIndex)
{
    writeSilenceForSynchronizedLatencyDelay(pBuffer,part,referenceTime,outputSampleIndex);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeSilenceForSynchronizedLatencyDelayAndBeforeEndOfBuffer)
{
	const tint c_channels[1] = {2};
	const tfloat32 c_Buffer[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };
	const tfloat32 c_expectBuffer[2 * 5] = { 10.0f, 11.0f, 0.0f, 0.0f, 0.0f, 0.0f, 16.0f, 17.0f, 18.0f, 19.0f };
	
	common::TimeStamp partReferenceTime(4.25),referenceTime(4.0);
	common::TimeStamp actualCurrentOutTime(20.0),expectCurrentOutTime(20.2);
	
	tint amount = 1;
	engine::RData data;
	engine::RData::Part& part = data.nextPart();
	part.refStartTime() = partReferenceTime;
	
	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_Buffer,2 * 5 * sizeof(tfloat32));
	
	AOBaseWriteSilenceForSynchronizedLatencyDelayTest audio;
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getCurrentOutTime()).Times(1).WillOnce(ReturnRef(actualCurrentOutTime));
	EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentOutTime));
	
	audio.testWriteSilenceForSynchronizedLatencyDelay(&buffer,part,referenceTime,amount);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBuffer,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(expectCurrentOutTime,actualCurrentOutTime);
	EXPECT_EQ(3,amount);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeSilenceForSynchronizedLatencyDelayAndToAfterEndOfBuffer)
{
	const tint c_channels[1] = {2};
	const tfloat32 c_Buffer[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };
	const tfloat32 c_expectBuffer[2 * 5] = { 10.0f, 11.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	
	tint amount = 1;
	common::TimeStamp partReferenceTime(5.0),referenceTime(4.0);
	common::TimeStamp actualCurrentOutTime(20.0),expectCurrentOutTime(20.4);
	
	engine::RData data;
	engine::RData::Part& part = data.nextPart();
	part.refStartTime() = partReferenceTime;
	
	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_Buffer,2 * 5 * sizeof(tfloat32));
	
	AOBaseWriteSilenceForSynchronizedLatencyDelayTest audio;
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getCurrentOutTime()).Times(1).WillOnce(ReturnRef(actualCurrentOutTime));
	EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentOutTime));
	
	audio.testWriteSilenceForSynchronizedLatencyDelay(&buffer,part,referenceTime,amount);
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBuffer,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(expectCurrentOutTime,actualCurrentOutTime);
	EXPECT_EQ(5,amount);
}

//-------------------------------------------------------------------------------------------

class AOBaseSyncAudioToPartReferenceLatencyDelayTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getReferenceClockTime,common::TimeStamp());
		MOCK_METHOD3(syncAudioTimeToPartReferenceLatencyDelay,void(engine::RData::Part& part,const IOTimeStamp& systemTime,const common::TimeStamp& referenceTime));
		MOCK_METHOD4(writeSilenceForSynchronizedLatencyDelay,void(AbstractAudioHardwareBuffer *pBuffer,engine::RData::Part& part,const common::TimeStamp& referenceTime,tint& outputSampleIndex));

		virtual void testSyncAudioToPartReferenceLatencyDelay(AbstractAudioHardwareBuffer *pBuffer,engine::RData::Part& part,const IOTimeStamp& systemTime,tint& outputSampleIndex);
};

//-------------------------------------------------------------------------------------------

void AOBaseSyncAudioToPartReferenceLatencyDelayTest::testSyncAudioToPartReferenceLatencyDelay(AbstractAudioHardwareBuffer *pBuffer,engine::RData::Part& part,const IOTimeStamp& systemTime,tint& outputSampleIndex)
{
	syncAudioToPartReferenceLatencyDelay(pBuffer,part,systemTime,outputSampleIndex);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,syncAudioTimeToPartReferenceLatencyDelayAfterCurrentReferenceTime)
{
	common::TimeStamp referenceTime(6.0),partTime(5.5);
	IOTimeStamp systemTime(true,partTime);
	AudioHardwareBufferMock buffer;
	
	tint outputSampleIndex = 3;
	
	engine::RData data;
    engine::RData::Part& part = data.nextPart();
    part.refStartTime() = partTime;
	
	AOBaseSyncAudioToPartReferenceLatencyDelayTest audio;
	EXPECT_CALL(audio,getReferenceClockTime()).Times(1).WillOnce(Return(referenceTime));
	EXPECT_CALL(audio,syncAudioTimeToPartReferenceLatencyDelay(part,systemTime,referenceTime)).Times(1);
	
	audio.testSyncAudioToPartReferenceLatencyDelay(&buffer,part,systemTime,outputSampleIndex);
	
	EXPECT_EQ(3,outputSampleIndex);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,syncAudioTimeToPartReferenceLatencyDelayBeforeCurrentReferenceTime)
{
	common::TimeStamp referenceTime(5.0),partTime(5.5);
	IOTimeStamp systemTime;
	AudioHardwareBufferMock buffer;
	
	tint actualOutputSampleIndex = 3,expectedOutputSampleIndex = 6;
	
	engine::RData data;
    engine::RData::Part& part = data.nextPart();
    part.refStartTime() = partTime;
	
	AOBaseSyncAudioToPartReferenceLatencyDelayTest audio;
	EXPECT_CALL(audio,getReferenceClockTime()).Times(1).WillOnce(Return(referenceTime));
    EXPECT_CALL(audio,writeSilenceForSynchronizedLatencyDelay(&buffer,part,referenceTime,actualOutputSampleIndex)).Times(1).WillOnce(SetArgReferee<3>(6));
	
	audio.testSyncAudioToPartReferenceLatencyDelay(&buffer,part,systemTime,actualOutputSampleIndex);
	
	EXPECT_EQ(expectedOutputSampleIndex,actualOutputSampleIndex);
}

//-------------------------------------------------------------------------------------------

class AOBaseOffsetAndPartToAudioItemTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getFrequency,tint());
		MOCK_CONST_METHOD0(getCurrentOutTime,const common::TimeStamp&());
        tint testPartNumberFromAudioItem(AudioItem *item) const;
        tint testOffsetFromAudioItem(AudioItem *item) const;
        void testSetOffsetAndPartToAudioItem(AudioItem *item,tint offset,tint partNumber) const;
        engine::RData::Part& testPartFromAudioItem(AudioItem *item) const;
};

//-------------------------------------------------------------------------------------------

tint AOBaseOffsetAndPartToAudioItemTest::testPartNumberFromAudioItem(AudioItem *item) const
{
	return partNumberFromAudioItem(item);
}

//-------------------------------------------------------------------------------------------

tint AOBaseOffsetAndPartToAudioItemTest::testOffsetFromAudioItem(AudioItem *item) const
{
	return offsetFromAudioItem(item);
}

//-------------------------------------------------------------------------------------------

engine::RData::Part& AOBaseOffsetAndPartToAudioItemTest::testPartFromAudioItem(AudioItem *item) const
{
	return partFromAudioItem(item);
}

//-------------------------------------------------------------------------------------------

void AOBaseOffsetAndPartToAudioItemTest::testSetOffsetAndPartToAudioItem(AudioItem *item,tint offset,tint partNumber) const
{
	setOffsetAndPartToAudioItem(item,offset,partNumber);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,offsetAndPartToAndFromAudioItemZeroPartAndOffset)
{
	common::TimeStamp currentOutTime(20.0),partStartTime(20.0);

	engine::RData *data = new engine::RData();
	engine::RData::Part& partA = data->nextPart();
	partA.start() = partStartTime;

	AudioItem item;
	item.setData(data);
	
	AOBaseOffsetAndPartToAudioItemTest audio;
    EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
    EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(currentOutTime));
	
	EXPECT_EQ(0,audio.testOffsetFromAudioItem(&item));
	EXPECT_EQ(0,audio.testPartNumberFromAudioItem(&item));
    EXPECT_EQ(&partA,&(audio.testPartFromAudioItem(&item)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,offsetAndPartToAndFromAudioItemWithInitialPositiveOffset)
{
	AudioItem item;
	
	AOBaseOffsetAndPartToAudioItemTest audio;
	audio.testSetOffsetAndPartToAudioItem(&item,100,3);
	
	EXPECT_EQ(100,audio.testOffsetFromAudioItem(&item));
	EXPECT_EQ(3,audio.testPartNumberFromAudioItem(&item));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,offsetAndPartToAndFromAudioItemWithInitialNegativeOffset)
{
	AudioItem item;
	
	AOBaseOffsetAndPartToAudioItemTest audio;
	audio.testSetOffsetAndPartToAudioItem(&item,-5,3);
	
	EXPECT_EQ(-5,audio.testOffsetFromAudioItem(&item));
	EXPECT_EQ(3,audio.testPartNumberFromAudioItem(&item));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,offsetAndPartToAndFromAudioItemWithInitialZeroOffsetAndCurrentPartStartEqual)
{
	common::TimeStamp currentOutTime(20.0),partStartTime(20.0);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& partB = data->nextPart();
	partB.start() = partStartTime;

	AudioItem item;
	item.setData(data);
	
	AOBaseOffsetAndPartToAudioItemTest audio;
    EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
    EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(currentOutTime));
    	
	audio.testSetOffsetAndPartToAudioItem(&item,0,1);
	
	EXPECT_EQ(0,audio.testOffsetFromAudioItem(&item));
	EXPECT_EQ(1,audio.testPartNumberFromAudioItem(&item));
    EXPECT_EQ(&partB,&(audio.testPartFromAudioItem(&item)));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,offsetAndPartToAndFromAudioItemWithInitialZeroOffsetAndCurrentPartStartNotEqual)
{
	common::TimeStamp currentOutTime(20.0),partStartTime(18.0);

	engine::RData *data = new engine::RData();
	data->nextPart();
	data->nextPart();
	engine::RData::Part& partC = data->nextPart();
	partC.start() = partStartTime;

	AudioItem item;
	item.setData(data);
	
	AOBaseOffsetAndPartToAudioItemTest audio;
    EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
    EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(currentOutTime));
    	
	audio.testSetOffsetAndPartToAudioItem(&item,0,2);
	
	EXPECT_EQ(20,audio.testOffsetFromAudioItem(&item));
	EXPECT_EQ(2,audio.testPartNumberFromAudioItem(&item));
    EXPECT_EQ(&partC,&(audio.testPartFromAudioItem(&item)));
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteToAudioProcessPartTest : public AOBaseOffsetAndPartToAudioItemTest
{
	public:
		MOCK_METHOD1(setCurrentOutTime,void(const common::TimeStamp& v));
		MOCK_METHOD6(writeToAudioOutputBuffer,void(AbstractAudioHardwareBuffer *pBuffer,engine::RData *data,tint partNumber,tint inputSampleIndex,tint outputSampleIndex,tint amount));
		
		tint getPartNumberFromAudioItem(AudioItem *item) const;
		tint getOffsetFromAudioItem(AudioItem *item) const;

		tint testWriteToAudioProcessPart(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,int outputSampleIndex);
};

//-------------------------------------------------------------------------------------------

tint AOBaseWriteToAudioProcessPartTest::getPartNumberFromAudioItem(AudioItem *item) const
{
	return static_cast<tint>((static_cast<tuint32>(item->done()) >> 22) & 0x000003ff);
}

//-------------------------------------------------------------------------------------------

tint AOBaseWriteToAudioProcessPartTest::getOffsetFromAudioItem(AudioItem *item) const
{
	return static_cast<tint>(static_cast<tuint32>(item->done()) & 0x003fffff);
}

//-------------------------------------------------------------------------------------------

tint AOBaseWriteToAudioProcessPartTest::testWriteToAudioProcessPart(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,int outputSampleIndex)
{
	return writeToAudioProcessPart(pBuffer,item,outputSampleIndex);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioProcessPartWithAmountEqualToLengthRemainingInPartAndDataToBeWritten)
{
	tint inputSampleIndex = 20;
	tint outputSampleIndex = 10;
	common::TimeStamp initialCurrentOutTime(22.0),expectCurrentOutTime(25.0),actualCurrentOutTime;
	common::TimeStamp partStartTime(20.0);
	common::TimeStamp partEndTime(25.0);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	
	part.length() = 50;
	part.start() = partStartTime;
	part.end() = partEndTime;

	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(60));
	
	AOBaseWriteToAudioProcessPartTest audio;
    EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
    EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(initialCurrentOutTime));
    EXPECT_CALL(audio,writeToAudioOutputBuffer(&buffer,data,1,inputSampleIndex,outputSampleIndex,30)).Times(1);
    EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(2).WillRepeatedly(SaveArg<0>(&actualCurrentOutTime));
	audio.testSetOffsetAndPartToAudioItem(&item,inputSampleIndex,1);
	
	ASSERT_EQ(40,audio.testWriteToAudioProcessPart(&buffer,&item,outputSampleIndex));
	
	EXPECT_EQ(0,audio.getOffsetFromAudioItem(&item));
	EXPECT_EQ(2,audio.getPartNumberFromAudioItem(&item));
	EXPECT_EQ(expectCurrentOutTime,actualCurrentOutTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioProcessPartWithAmountEqualToLengthRemainingInBufferGivingDataToBeWrittenAndNotEndOfPart)
{
	tint inputSampleIndex = 20;
	tint outputSampleIndex = 10;
	common::TimeStamp initialCurrentOutTime(22.0),expectCurrentOutTime(24.0),actualCurrentOutTime;
	common::TimeStamp partStartTime(20.0);
	common::TimeStamp partEndTime(25.0);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	
	part.length() = 50;
	part.start() = partStartTime;
	part.end() = partEndTime;

	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(30));
	
	AOBaseWriteToAudioProcessPartTest audio;
    EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
    EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(initialCurrentOutTime));
    EXPECT_CALL(audio,writeToAudioOutputBuffer(&buffer,data,1,inputSampleIndex,outputSampleIndex,20)).Times(1);
    EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualCurrentOutTime));
	audio.testSetOffsetAndPartToAudioItem(&item,inputSampleIndex,1);
	
	ASSERT_EQ(30,audio.testWriteToAudioProcessPart(&buffer,&item,outputSampleIndex));
	
	EXPECT_EQ(40,audio.getOffsetFromAudioItem(&item));
	EXPECT_EQ(1,audio.getPartNumberFromAudioItem(&item));
	EXPECT_EQ(expectCurrentOutTime,actualCurrentOutTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioProcessPartWithAmountEqualToLengthRemainingInBufferGivingDataToBeWrittenAndEndOfPart)
{
	tint inputSampleIndex = 20;
	tint outputSampleIndex = 10;
	common::TimeStamp initialCurrentOutTime(22.0),expectCurrentOutTime(25.0),actualCurrentOutTime;
	common::TimeStamp partStartTime(20.0);
	common::TimeStamp partEndTime(25.0);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	
	part.length() = 50;
	part.start() = partStartTime;
	part.end() = partEndTime;

	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(40));
	
	AOBaseWriteToAudioProcessPartTest audio;
    EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
    EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(initialCurrentOutTime));
    EXPECT_CALL(audio,writeToAudioOutputBuffer(&buffer,data,1,inputSampleIndex,outputSampleIndex,30)).Times(1);
    EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(2).WillRepeatedly(SaveArg<0>(&actualCurrentOutTime));
	audio.testSetOffsetAndPartToAudioItem(&item,inputSampleIndex,1);
	
	ASSERT_EQ(40,audio.testWriteToAudioProcessPart(&buffer,&item,outputSampleIndex));
	
	EXPECT_EQ(0,audio.getOffsetFromAudioItem(&item));
	EXPECT_EQ(2,audio.getPartNumberFromAudioItem(&item));
	EXPECT_EQ(expectCurrentOutTime,actualCurrentOutTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioProcessPartWithAmountEqualToLengthRemainingInBufferGivingNoDataToBeWritten)
{
	tint inputSampleIndex = 20;
	tint outputSampleIndex = 10;
	common::TimeStamp initialCurrentOutTime(22.0);
	common::TimeStamp partStartTime(20.0);
	common::TimeStamp partEndTime(25.0);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	
	part.length() = 50;
	part.start() = partStartTime;
	part.end() = partEndTime;

	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(10));
	
	AOBaseWriteToAudioProcessPartTest audio;
    EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
    EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(initialCurrentOutTime));
	audio.testSetOffsetAndPartToAudioItem(&item,inputSampleIndex,1);
	
	ASSERT_EQ(10,audio.testWriteToAudioProcessPart(&buffer,&item,outputSampleIndex));
	
	EXPECT_EQ(20,audio.getOffsetFromAudioItem(&item));
	EXPECT_EQ(1,audio.getPartNumberFromAudioItem(&item));
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioProcessPartWithOffsetBeforePart)
{
	tint inputSampleIndex = -5;
	tint outputSampleIndex = 10;
	common::TimeStamp initialCurrentOutTime(22.0),expectCurrentOutTime(20.0),actualCurrentOutTime;
	common::TimeStamp partStartTime(20.0);
	common::TimeStamp partEndTime(25.0);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	
	part.length() = 50;
	part.start() = partStartTime;
	part.end() = partEndTime;

	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(60));
	
	AOBaseWriteToAudioProcessPartTest audio;
    EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
    EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(initialCurrentOutTime));
    EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(1).WillRepeatedly(SaveArg<0>(&actualCurrentOutTime));
	audio.testSetOffsetAndPartToAudioItem(&item,inputSampleIndex,1);
	
	ASSERT_EQ(10,audio.testWriteToAudioProcessPart(&buffer,&item,outputSampleIndex));
	
	EXPECT_EQ(0,audio.getOffsetFromAudioItem(&item));
	EXPECT_EQ(1,audio.getPartNumberFromAudioItem(&item));
	EXPECT_EQ(expectCurrentOutTime,actualCurrentOutTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioProcessPartWithOffsetAfterPart)
{
	tint inputSampleIndex = 55;
	tint outputSampleIndex = 10;
	common::TimeStamp initialCurrentOutTime(22.0),expectCurrentOutTime(25.0),actualCurrentOutTime;
	common::TimeStamp partStartTime(20.0);
	common::TimeStamp partEndTime(25.0);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	
	part.length() = 50;
	part.start() = partStartTime;
	part.end() = partEndTime;

	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
    EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(60));
	
	AOBaseWriteToAudioProcessPartTest audio;
    EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
    EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(initialCurrentOutTime));
    EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).Times(1).WillRepeatedly(SaveArg<0>(&actualCurrentOutTime));
	audio.testSetOffsetAndPartToAudioItem(&item,inputSampleIndex,1);
	
	ASSERT_EQ(10,audio.testWriteToAudioProcessPart(&buffer,&item,outputSampleIndex));
	
	EXPECT_EQ(0,audio.getOffsetFromAudioItem(&item));
	EXPECT_EQ(2,audio.getPartNumberFromAudioItem(&item));
	EXPECT_EQ(expectCurrentOutTime,actualCurrentOutTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioProcessPartWithCurrentTimeAfterEnd)
{
	tint inputSampleIndex = 20;
	tint outputSampleIndex = 10;
	common::TimeStamp initialCurrentOutTime(26.0);
	common::TimeStamp partStartTime(20.0);
	common::TimeStamp partEndTime(25.0);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	
	part.length() = 50;
	part.start() = partStartTime;
	part.end() = partEndTime;

	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
	
	AOBaseWriteToAudioProcessPartTest audio;
    EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
    EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(initialCurrentOutTime));
	audio.testSetOffsetAndPartToAudioItem(&item,inputSampleIndex,1);
	
	ASSERT_EQ(10,audio.testWriteToAudioProcessPart(&buffer,&item,outputSampleIndex));
	
	EXPECT_EQ(0,audio.getOffsetFromAudioItem(&item));
	EXPECT_EQ(2,audio.getPartNumberFromAudioItem(&item));
}

//-------------------------------------------------------------------------------------------

class AOBaseAudioItemCallbackIsDoneTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getReferenceClockTime,common::TimeStamp());
		MOCK_CONST_METHOD0(getOutputLatencyTime,const common::TimeStamp&());
		MOCK_CONST_METHOD0(getFrequency,tint());
		MOCK_METHOD1(setStopTimeClock,void(const common::TimeStamp& t));
		MOCK_METHOD1(setStopTimeFlag,void (bool f));
		
		AudioItem *testAudioItemCallbackIsDone(AudioItem *item,tint outputSampleIndex,bool& loop,bool& loopFlag);
};

//-------------------------------------------------------------------------------------------

AudioItem *AOBaseAudioItemCallbackIsDoneTest::testAudioItemCallbackIsDone(AudioItem *item,tint outputSampleIndex,bool& loop,bool& loopFlag)
{
	return audioItemCallbackIsDone(item,outputSampleIndex,loop,loopFlag);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,audioItemCallbackIsDoneWhenAtEnd)
{
	bool loop = true, loopFlag = false;
	common::TimeStamp referenceClockTime(10.0),outputLatencyTime(2.0);
	common::TimeStamp actualStopTime,expectStopTime(17.0);
	
	AudioItem itemA,itemB;
	itemA.setNext(&itemB);
	itemA.setState(AudioItem::e_stateCallbackEnd);
	itemB.setState(AudioItem::e_stateFull);
	
	AOBaseAudioItemCallbackIsDoneTest audio;
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getReferenceClockTime()).WillRepeatedly(Return(referenceClockTime));
	EXPECT_CALL(audio,getOutputLatencyTime()).WillRepeatedly(ReturnRef(outputLatencyTime));
	EXPECT_CALL(audio,setStopTimeClock(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&actualStopTime));
	EXPECT_CALL(audio,setStopTimeFlag(true)).Times(1);
	
	AudioItem *nItem = audio.testAudioItemCallbackIsDone(&itemA,50,loop,loopFlag);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA.state());
	EXPECT_EQ(AudioItem::e_stateFull,itemB.state());
	EXPECT_EQ(expectStopTime,actualStopTime);
	EXPECT_EQ(&itemB,nItem);
	EXPECT_TRUE(loopFlag);
	EXPECT_FALSE(loop);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,audioItemCallbackIsDoneWhenMoreToPlay)
{
	bool loop = true, loopFlag = false;
	
	AudioItem itemA,itemB;
	itemA.setNext(&itemB);
	itemA.setState(AudioItem::e_stateCallback);
	itemB.setState(AudioItem::e_stateFull);
	
	AOBaseAudioItemCallbackIsDoneTest audio;
	
	AudioItem *nItem = audio.testAudioItemCallbackIsDone(&itemA,50,loop,loopFlag);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA.state());
	EXPECT_EQ(AudioItem::e_stateFull,itemB.state());
	EXPECT_EQ(&itemB,nItem);
	EXPECT_TRUE(loopFlag);
	EXPECT_TRUE(loop);	
}

//-------------------------------------------------------------------------------------------

class AOBaseNumberOfSamplesInFixedTime : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getFrequency,tint());
		tint testNumberOfSamplesInFixedTime(const common::TimeStamp& t) const;
};

//-------------------------------------------------------------------------------------------

tint AOBaseNumberOfSamplesInFixedTime::testNumberOfSamplesInFixedTime(const common::TimeStamp& t) const
{
	return numberOfSamplesInFixedTime(t);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,numberOfSamplesInFixedTimeWhereRoundedDown)
{
	common::TimeStamp t(7.245);
	AOBaseNumberOfSamplesInFixedTime audio;
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_EQ(72,audio.testNumberOfSamplesInFixedTime(t));	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,numberOfSamplesInFixedTimeWhereRoundedUp)
{
	common::TimeStamp t(7.255);
	AOBaseNumberOfSamplesInFixedTime audio;
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_EQ(73,audio.testNumberOfSamplesInFixedTime(t));	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,numberOfSamplesInFixedTimeWhereCodedInputIsWhole)
{
	common::TimeStamp t(7.2);
	AOBaseNumberOfSamplesInFixedTime audio;
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_EQ(72,audio.testNumberOfSamplesInFixedTime(t));
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteToAudioSilenceUntilStartOfNextPartTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getFrequency,tint());
		MOCK_CONST_METHOD0(getCurrentOutTime,const common::TimeStamp&());
		MOCK_METHOD1(setCurrentOutTime,void(const common::TimeStamp& v));
		tint testWriteToAudioSilenceUntilStartOfNextPart(AbstractAudioHardwareBuffer *pBuffer,const engine::RData::Part& part,tint outputSampleIndex);
};

//-------------------------------------------------------------------------------------------

tint AOBaseWriteToAudioSilenceUntilStartOfNextPartTest::testWriteToAudioSilenceUntilStartOfNextPart(AbstractAudioHardwareBuffer *pBuffer,const engine::RData::Part& part,tint outputSampleIndex)
{
	return writeToAudioSilenceUntilStartOfNextPart(pBuffer,part,outputSampleIndex);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceUntilStartOfNextPartWhenAudioBufferEndsBeforePartBegins)
{
	const tint c_channels[1] = {2};
	const tfloat32 c_BufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };
	const tfloat32 c_expectBufferA[2 * 5] = { 10.0f, 11.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	common::TimeStamp partStart(10.5);
	common::TimeStamp currentOutTime(10.0),expectOutTime(10.4);

	engine::RData data;
	engine::RData::Part& part = data.nextPart();
	part.start() = partStart;

	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_BufferA,2 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceUntilStartOfNextPartTest audio;
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(currentOutTime));
	EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).WillRepeatedly(SaveArg<0>(&currentOutTime));
	
	EXPECT_EQ(5,audio.testWriteToAudioSilenceUntilStartOfNextPart(&buffer,part,1));
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(expectOutTime,currentOutTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioSilenceUntilStartOfNextPartWhenPartBeginsInsideTheCurrentAudioBuffer)
{
	const tint c_channels[1] = {2};
	const tfloat32 c_BufferA[2 * 5] = { 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f, 17.0f, 18.0f, 19.0f };
	const tfloat32 c_expectBufferA[2 * 5] = { 10.0f, 11.0f, 0.0f, 0.0f, 0.0f, 0.0f, 16.0f, 17.0f, 18.0f, 19.0f };

	common::TimeStamp partStart(10.2);
	common::TimeStamp currentOutTime(10.0),expectOutTime(10.2);

	engine::RData data;
	engine::RData::Part& part = data.nextPart();
	part.start() = partStart;

	AudioHardwareBufferTester buffer(c_channels,1);
	memcpy(buffer.buffer(0),c_BufferA,2 * 5 * sizeof(tfloat32));
	
	AOBaseWriteToAudioSilenceUntilStartOfNextPartTest audio;
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(10));
	EXPECT_CALL(audio,getCurrentOutTime()).WillRepeatedly(ReturnRef(currentOutTime));
	EXPECT_CALL(audio,setCurrentOutTime(A<const common::TimeStamp&>())).WillRepeatedly(SaveArg<0>(&currentOutTime));
	
	EXPECT_EQ(3,audio.testWriteToAudioSilenceUntilStartOfNextPart(&buffer,part,1));
	
	EXPECT_EQ(0,memcmp(buffer.buffer(0),c_expectBufferA,2 * 5 * sizeof(tfloat32)));
	EXPECT_EQ(expectOutTime,currentOutTime);
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteToAudioFromItemTest : public AOBaseTest
{
	public:
		MOCK_CONST_METHOD0(getCurrentOutTime,const common::TimeStamp&());
		MOCK_METHOD1(processDataForOutput,void(engine::RData *data));
		MOCK_METHOD3(playbackOfNextTrackIsStarting,void(const engine::RData::Part& part,const IOTimeStamp& systemTime,tint sIndex));
		MOCK_METHOD4(syncAudioToPartReferenceLatencyDelay,void(AbstractAudioHardwareBuffer *pBuffer,engine::RData::Part& part,const IOTimeStamp& systemTime,tint& outputSampleIndex));
		MOCK_METHOD3(writeToAudioProcessPart,tint(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,int outputSampleIndex));
		MOCK_METHOD3(writeToAudioSilenceUntilStartOfNextPart,tint(AbstractAudioHardwareBuffer *pBuffer,const engine::RData::Part& part,tint outputSampleIndex));
		MOCK_METHOD4(audioItemCallbackIsDone,AudioItem *(AudioItem *item,tint outputSampleIndex,bool& loop,bool& loopFlag));

		AudioItem *testWriteToAudioFromItem(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,const IOTimeStamp& systemTime,tint& outputSampleIndex,bool& loop,bool& loopFlag);
		void callSetOffsetAndPartToAudioItem(AudioItem *item,tint offset,tint partNumber) const;
};

//-------------------------------------------------------------------------------------------

AudioItem *AOBaseWriteToAudioFromItemTest::testWriteToAudioFromItem(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,const IOTimeStamp& systemTime,tint& outputSampleIndex,bool& loop,bool& loopFlag)
{
	return writeToAudioFromItem(pBuffer,item,systemTime,outputSampleIndex,loop,loopFlag);
}

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioFromItemTest::callSetOffsetAndPartToAudioItem(AudioItem *item,tint offset,tint partNumber) const
{
	setOffsetAndPartToAudioItem(item,offset,partNumber);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioFromItemWithPartContainingReferenceStartTime)
{
	common::TimeStamp partStartTime(10.0);
	common::TimeStamp partRefStartTime(20.0);
	common::TimeStamp audioTimeStamp(30.0);
	
	tint outputSampleIndex = 40;
	bool loop = true,loopFlag = false;
	IOTimeStamp systemTime(true,audioTimeStamp);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	part.start() = partStartTime;
	part.refStartTime() = partRefStartTime;
	
	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
	
	AOBaseWriteToAudioFromItemTest audio;
	audio.callSetOffsetAndPartToAudioItem(&item,100,1);
	
	EXPECT_CALL(audio,processDataForOutput(data)).Times(1);
	EXPECT_CALL(audio,playbackOfNextTrackIsStarting(part,systemTime,outputSampleIndex)).Times(1);
    EXPECT_CALL(audio,syncAudioToPartReferenceLatencyDelay(&buffer,part,systemTime,outputSampleIndex)).Times(1).WillOnce(SetArgReferee<3>(55));
	
	AudioItem *nItem = audio.testWriteToAudioFromItem(&buffer,&item,systemTime,outputSampleIndex,loop,loopFlag);
	
	EXPECT_EQ(nItem,&item);
	EXPECT_EQ(55,outputSampleIndex);
	EXPECT_TRUE(loop);
	EXPECT_FALSE(loopFlag);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioFromItemWithOutputTimeAfterPartStartTime)
{
	common::TimeStamp partStartTime(10.0);
	common::TimeStamp currentOutTime(15.0);
	common::TimeStamp audioTimeStamp(30.0);
	
	tint outputSampleIndex = 40;
	bool loop = true,loopFlag = false;
	IOTimeStamp systemTime(true,audioTimeStamp);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	part.start() = partStartTime;
	
	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
	
	AOBaseWriteToAudioFromItemTest audio;
	audio.callSetOffsetAndPartToAudioItem(&item,100,1);
	
	EXPECT_CALL(audio,getCurrentOutTime()).Times(1).WillOnce(ReturnRef(currentOutTime));
	EXPECT_CALL(audio,processDataForOutput(data)).Times(1);
	EXPECT_CALL(audio,playbackOfNextTrackIsStarting(part,systemTime,outputSampleIndex)).Times(1);
	EXPECT_CALL(audio,writeToAudioProcessPart(&buffer,&item,outputSampleIndex)).Times(1).WillOnce(Return(60));
	
	AudioItem *nItem = audio.testWriteToAudioFromItem(&buffer,&item,systemTime,outputSampleIndex,loop,loopFlag);
	
	EXPECT_EQ(nItem,&item);
	EXPECT_EQ(60,outputSampleIndex);
	EXPECT_TRUE(loop);
	EXPECT_FALSE(loopFlag);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioFromItemWithOutputTimeEqualToPartStartTime)
{
	common::TimeStamp partStartTime(10.0);
	common::TimeStamp currentOutTime(10.0);
	common::TimeStamp audioTimeStamp(30.0);
	
	tint outputSampleIndex = 40;
	bool loop = true,loopFlag = false;
	IOTimeStamp systemTime(true,audioTimeStamp);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	part.start() = partStartTime;
	
	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
	
	AOBaseWriteToAudioFromItemTest audio;
	audio.callSetOffsetAndPartToAudioItem(&item,100,1);
	
	EXPECT_CALL(audio,getCurrentOutTime()).Times(1).WillOnce(ReturnRef(currentOutTime));
	EXPECT_CALL(audio,processDataForOutput(data)).Times(1);
	EXPECT_CALL(audio,playbackOfNextTrackIsStarting(part,systemTime,outputSampleIndex)).Times(1);
	EXPECT_CALL(audio,writeToAudioProcessPart(&buffer,&item,outputSampleIndex)).Times(1).WillOnce(Return(70));
	
	AudioItem *nItem = audio.testWriteToAudioFromItem(&buffer,&item,systemTime,outputSampleIndex,loop,loopFlag);
	
	EXPECT_EQ(nItem,&item);
	EXPECT_EQ(70,outputSampleIndex);
	EXPECT_TRUE(loop);
	EXPECT_FALSE(loopFlag);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioFromItemWithOutputTimeBeforePartStartTime)
{
	common::TimeStamp partStartTime(10.0);
	common::TimeStamp currentOutTime(5.0);
	common::TimeStamp audioTimeStamp(30.0);
	
	tint outputSampleIndex = 40;
	bool loop = true,loopFlag = false;
	IOTimeStamp systemTime(true,audioTimeStamp);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	part.start() = partStartTime;
	
	AudioItem item;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
	
	AOBaseWriteToAudioFromItemTest audio;
	audio.callSetOffsetAndPartToAudioItem(&item,100,1);
	
	EXPECT_CALL(audio,getCurrentOutTime()).Times(1).WillOnce(ReturnRef(currentOutTime));
	EXPECT_CALL(audio,processDataForOutput(data)).Times(1);
	EXPECT_CALL(audio,playbackOfNextTrackIsStarting(part,systemTime,outputSampleIndex)).Times(1);
    EXPECT_CALL(audio,writeToAudioSilenceUntilStartOfNextPart(&buffer,part,outputSampleIndex)).Times(1).WillOnce(Return(80));
	
	AudioItem *nItem = audio.testWriteToAudioFromItem(&buffer,&item,systemTime,outputSampleIndex,loop,loopFlag);
	
	EXPECT_EQ(nItem,&item);
	EXPECT_EQ(80,outputSampleIndex);
	EXPECT_TRUE(loop);
	EXPECT_FALSE(loopFlag);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioFromItemWithAllPartsInItemWritten)
{
	common::TimeStamp partStartTime(10.0);
	common::TimeStamp audioTimeStamp(30.0);
	
	tint outputSampleIndex = 40;
	bool loop = true,loopFlag = false;
	IOTimeStamp systemTime(true,audioTimeStamp);

	engine::RData *data = new engine::RData();
	data->nextPart();
	engine::RData::Part& part  = data->nextPart();
	part.start() = partStartTime;
	
	AudioItem item,itemB;
	item.setData(data);
	
	AudioHardwareBufferMock buffer;
	
	AOBaseWriteToAudioFromItemTest audio;
	audio.callSetOffsetAndPartToAudioItem(&item,100,2);
	
	EXPECT_CALL(audio,processDataForOutput(data)).Times(1);
	EXPECT_CALL(audio,audioItemCallbackIsDone(&item,outputSampleIndex,loop,loopFlag)).Times(1)
		.WillOnce(DoAll(SetArgReferee<2>(false),SetArgReferee<3>(true),Return(&itemB)));
	
	AudioItem *nItem = audio.testWriteToAudioFromItem(&buffer,&item,systemTime,outputSampleIndex,loop,loopFlag);
	
	EXPECT_EQ(nItem,&itemB);
	EXPECT_EQ(40,outputSampleIndex);
	EXPECT_FALSE(loop);
	EXPECT_TRUE(loopFlag);
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteToAudioTest : public AOBaseTest
{
	public:
		virtual ~AOBaseWriteToAudioTest();
		
		MOCK_METHOD0(getCallbackAudioItem,AudioItem *());
		MOCK_METHOD6(writeToAudioFromItem,AudioItem *(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,const IOTimeStamp& systemTime,tint& outputSampleIndex,bool& loop,bool& loopFlag));
		MOCK_METHOD2(writeToAudioSilenceForRemainder,void(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex));

		QVector<AudioItem *> createCyclic3ItemTestBuffer(AudioItem::ItemStates stateA,AudioItem::ItemStates stateB,AudioItem::ItemStates stateC);
		AudioItem *writeToAudioFromItemInvoke(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,const IOTimeStamp& systemTime,tint& outputSampleIndex,bool& loop,bool& loopFlag);
		
		void testWriteToAudio(AbstractAudioHardwareBuffer *pBuffer,const IOTimeStamp& systemTime);
		void setSyncAudioToTimestamp(bool syncFlag);
		
		const common::TimeStamp& getCurrentOutTimeTest() const;
		void setCurrentOutTimeTest(const common::TimeStamp& v);

	private:
		QVector<AudioItem *> m_cyclicBufferItems;
		
		void freeCyclicTestBuffer();
		QVector<AudioItem *> createCyclicTestBuffer(QVector<AudioItem::ItemStates>& states);
};

//-------------------------------------------------------------------------------------------

AOBaseWriteToAudioTest::~AOBaseWriteToAudioTest()
{
	freeCyclicTestBuffer();
}

//-------------------------------------------------------------------------------------------

AudioItem *AOBaseWriteToAudioTest::writeToAudioFromItemInvoke(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,const IOTimeStamp& systemTime,tint& outputSampleIndex,bool& loop,bool& loopFlag)
{
	item = audioItemCallbackIsDone(item,outputSampleIndex,loop,loopFlag);
	return item;
}

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioTest::setSyncAudioToTimestamp(bool syncFlag)
{
	m_syncAudioToTimestamp = syncFlag;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AOBaseWriteToAudioTest::getCurrentOutTimeTest() const
{
	return getCurrentOutTime();
}

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioTest::setCurrentOutTimeTest(const common::TimeStamp& v)
{
	setCurrentOutTime(v);
}

//-------------------------------------------------------------------------------------------

QVector<AudioItem *> AOBaseWriteToAudioTest::createCyclic3ItemTestBuffer(AudioItem::ItemStates stateA,AudioItem::ItemStates stateB,AudioItem::ItemStates stateC)
{
	QVector<AudioItem::ItemStates> states;
	states.append(stateA);
	states.append(stateB);
	states.append(stateC);
	return createCyclicTestBuffer(states);
}

//-------------------------------------------------------------------------------------------

QVector<AudioItem *> AOBaseWriteToAudioTest::createCyclicTestBuffer(QVector<AudioItem::ItemStates>& states)
{
	AudioItem *pItem = 0;
	QVector<AudioItem *> items;
	
	freeCyclicTestBuffer();
	for(QVector<AudioItem::ItemStates>::iterator ppI=states.begin();ppI!=states.end();ppI++)
	{
		AudioItem *item = new AudioItem;
		item->setState(*ppI);
		if(pItem!=0)
		{
			pItem->setNext(item);
		}
		item->setPrev(pItem);
		items.append(item);
		pItem = item;
	}
	
	items[0]->setPrev(items[items.size()-1]);
	items[items.size()-1]->setNext(items[0]);
	
	m_cyclicBufferItems = items;
	return items;
}

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioTest::freeCyclicTestBuffer()
{
	for(QVector<AudioItem *>::iterator ppI=m_cyclicBufferItems.begin();ppI!=m_cyclicBufferItems.end();ppI++)
	{
		AudioItem *item = *ppI;
		delete item;
	}
}

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioTest::testWriteToAudio(AbstractAudioHardwareBuffer *pBuffer,const IOTimeStamp& systemTime)
{
	writeToAudio(pBuffer,systemTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioWithSilenceNoSync)
{
	common::TimeStamp audioTimeStamp(30.0);
	common::TimeStamp partStartTime(40.0);
	common::TimeStamp outTime(20.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(30));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateEmpty,AudioItem::e_stateEmpty,AudioItem::e_stateEmpty);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	audio.setSyncAudioToTimestamp(false);
	audio.setCurrentOutTimeTest(outTime);

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(items[0]));
	EXPECT_CALL(audio,writeToAudioSilenceForRemainder(&buffer,0)).Times(1);
	EXPECT_CALL(audio, writeToAudioFromItem(&buffer, itemA, systemTime, A<int&>(), A<bool&>(), A<bool&>())).Times(1)
		.WillOnce(DoAll(SetArgReferee<3>(30), Return(itemB)));

	audio.testWriteToAudio(&buffer,systemTime);
	
	engine::RData *data = new engine::RData();
	engine::RData::Part& part = data->nextPart();
	part.start() = partStartTime;
	itemA->setData(data);
	itemA->setState(AudioItem::e_stateFull);

	audio.testWriteToAudio(&buffer, systemTime);

	EXPECT_EQ(audio.getCurrentOutTimeTest(), partStartTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, writeToAudioWithSilenceWithSync)
{
	common::TimeStamp audioTimeStamp(30.0);
	common::TimeStamp partStartTime(40.0);
	common::TimeStamp outTime(20.0);
	IOTimeStamp systemTime(true, audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer, bufferLength()).WillRepeatedly(Return(30));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateEmpty, AudioItem::e_stateEmpty, AudioItem::e_stateEmpty);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	audio.setSyncAudioToTimestamp(true);
	audio.setCurrentOutTimeTest(outTime);

	EXPECT_CALL(audio, getCallbackAudioItem()).WillRepeatedly(Return(items[0]));
	EXPECT_CALL(audio, writeToAudioSilenceForRemainder(&buffer, 0)).Times(1);
	EXPECT_CALL(audio, writeToAudioFromItem(&buffer, itemA, systemTime, A<int&>(), A<bool&>(), A<bool&>())).Times(1)
		.WillOnce(DoAll(SetArgReferee<3>(30), Return(itemB)));

	audio.testWriteToAudio(&buffer, systemTime);

	engine::RData *data = new engine::RData();
	engine::RData::Part& part = data->nextPart();
	part.start() = partStartTime;
	itemA->setData(data);
	itemA->setState(AudioItem::e_stateFull);

	audio.testWriteToAudio(&buffer, systemTime);

	EXPECT_EQ(audio.getCurrentOutTimeTest(), outTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenAllItemsInCyclicBufferAreEmpty)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(30));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateEmpty,AudioItem::e_stateEmpty,AudioItem::e_stateEmpty);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(items[0]));
	EXPECT_CALL(audio,writeToAudioSilenceForRemainder(&buffer,0)).Times(1);
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateEmpty,itemA->state());
	EXPECT_EQ(AudioItem::e_stateEmpty,itemB->state());
	EXPECT_EQ(AudioItem::e_stateEmpty,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenAllItemsInCyclicBufferAreDone)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(30));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateDone,AudioItem::e_stateDone,AudioItem::e_stateDone);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(items[0]));
	EXPECT_CALL(audio,writeToAudioSilenceForRemainder(&buffer,0)).Times(1);
	
	audio.testWriteToAudio(&buffer,systemTime);
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemB->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenItemsAreEmptyExpectOneBeingProcessedForCodec)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(30));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateCodec,AudioItem::e_stateEmpty,AudioItem::e_stateEmpty);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(items[0]));
	EXPECT_CALL(audio,writeToAudioSilenceForRemainder(&buffer,0)).Times(1);
	
	audio.testWriteToAudio(&buffer,systemTime);

	EXPECT_EQ(AudioItem::e_stateCodec,itemA->state());
	EXPECT_EQ(AudioItem::e_stateEmpty,itemB->state());
	EXPECT_EQ(AudioItem::e_stateEmpty,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenItemsAreDoneExpectOneBeingProcessedForCodec)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(30));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateCodec,AudioItem::e_stateDone,AudioItem::e_stateDone);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(items[0]));
	EXPECT_CALL(audio,writeToAudioSilenceForRemainder(&buffer,0)).Times(1);
	
	audio.testWriteToAudio(&buffer,systemTime);

	EXPECT_EQ(AudioItem::e_stateCodec,itemA->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemB->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGiven3ItemsAreDoneFullCodecWithoutFillingAudioBuffer)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(30));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateDone,AudioItem::e_stateFull,AudioItem::e_stateCodec);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemB,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(3)
		.WillOnce(DoAll(SetArgReferee<3>(10),Return(itemB)))
		.WillOnce(DoAll(SetArgReferee<3>(29),Return(itemB)))
		.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
	EXPECT_CALL(audio,writeToAudioSilenceForRemainder(&buffer,29)).Times(1);
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemB->state());
	EXPECT_EQ(AudioItem::e_stateCodec,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGiven3ItemsAreDoneFullCodecWithAudioBufferFilled)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(40));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateDone,AudioItem::e_stateFull,AudioItem::e_stateCodec);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemB,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
		.WillOnce(DoAll(SetArgReferee<3>(20),Return(itemB)))
		.WillOnce(DoAll(SetArgReferee<3>(40),Return(itemB)));
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateCallback,itemB->state());
	EXPECT_EQ(AudioItem::e_stateCodec,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGiven3ItemsAreFullFullDoneWithoutFillingAudioBuffer)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(30));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateFull,AudioItem::e_stateFull,AudioItem::e_stateDone);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	
	{
		InSequence seq;
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemA,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(10),Return(itemA)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemB,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(29),Return(itemB)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
	}
	
	EXPECT_CALL(audio,writeToAudioSilenceForRemainder(&buffer,29)).Times(1);
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemB->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGiven3ItemsAreFullFullDoneWithAudioBufferFilledExactly)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(30));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateFull,AudioItem::e_stateFull,AudioItem::e_stateDone);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	
	{
		InSequence seq;
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemA,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(15),Return(itemA)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemB,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(1)
			.WillOnce(DoAll(SetArgReferee<3>(30),Return(itemB)));
	}
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateCallback,itemB->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenItemsAreAllFullWithAudioBufferFilledOnFirst)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(60));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateFull,AudioItem::e_stateFull,AudioItem::e_stateFull);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemA,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(1)
		.WillOnce(DoAll(SetArgReferee<3>(60),Return(itemA)));
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateCallback,itemA->state());
	EXPECT_EQ(AudioItem::e_stateFull,itemB->state());
	EXPECT_EQ(AudioItem::e_stateFull,itemC->state());	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenItemsAreAllFullWithAudioBufferFilledOnSecond)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(60));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateFull,AudioItem::e_stateFull,AudioItem::e_stateFull);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	
	{
		InSequence seq;
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemA,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(20),Return(itemA)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
	
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemB,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(1)
			.WillOnce(DoAll(SetArgReferee<3>(70),Return(itemB)));
	}
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateCallback,itemB->state());
	EXPECT_EQ(AudioItem::e_stateFull,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenItemsAreAllFullWithAudioBufferFilledOnThird)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(60));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateFull,AudioItem::e_stateFull,AudioItem::e_stateFull);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	
	{
		InSequence seq;
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemA,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(20),Return(itemA)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
	
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemB,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(40),Return(itemB)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemC,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(1)
			.WillOnce(DoAll(SetArgReferee<3>(60),Return(itemC)));
	}
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemB->state());
	EXPECT_EQ(AudioItem::e_stateCallback,itemC->state());	
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenItemsAreAllFullWithoutFillingAudioBuffer)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(60));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateFull,AudioItem::e_stateFull,AudioItem::e_stateFull);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	
	{
		InSequence seq;
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemA,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(20),Return(itemA)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
	
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemB,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(40),Return(itemB)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemC,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(50),Return(itemC)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
	}
	
	EXPECT_CALL(audio,writeToAudioSilenceForRemainder(&buffer,50)).Times(1);

	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemB->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenItemsAreAllFullWithSecondMakedAsEndWithoutFillingAudioBuffer)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(60));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateFull,AudioItem::e_stateFullEnd,AudioItem::e_stateFull);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	
	{
		InSequence seq;
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemA,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(20),Return(itemA)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
	
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemB,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(40),Return(itemB)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
			
		EXPECT_CALL(audio,writeToAudioSilenceForRemainder(&buffer,40)).Times(1);
	}
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateDone,itemB->state());
	EXPECT_EQ(AudioItem::e_stateFull,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenItemsAreAllFullWithSecondMakedAsEndWithAudioBufferFilledBeforeEnd)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(60));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateFull,AudioItem::e_stateFullEnd,AudioItem::e_stateFull);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	
	{
		InSequence seq;
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemA,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(30),Return(itemA)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
	
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemB,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(1)
			.WillOnce(DoAll(SetArgReferee<3>(60),Return(itemB)));
	}
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateCallbackEnd,itemB->state());
	EXPECT_EQ(AudioItem::e_stateFull,itemC->state());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioGivenItemsHasCallbackEndWithAudioBufferNotFilledBeforeEnd)
{
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	EXPECT_CALL(buffer,bufferLength()).WillRepeatedly(Return(60));

	AOBaseWriteToAudioTest audio;
	QVector<AudioItem *> items = audio.createCyclic3ItemTestBuffer(AudioItem::e_stateCallbackEnd,AudioItem::e_stateFull,AudioItem::e_stateFull);
	AudioItem *itemA = items[0];
	AudioItem *itemB = items[1];
	AudioItem *itemC = items[2];

	EXPECT_CALL(audio,getCallbackAudioItem()).WillRepeatedly(Return(itemA));
	
	{
		InSequence seq;
		
		EXPECT_CALL(audio,writeToAudioFromItem(&buffer,itemA,systemTime,A<int&>(),A<bool&>(),A<bool&>())).Times(2)
			.WillOnce(DoAll(SetArgReferee<3>(40),Return(itemA)))
			.WillOnce(Invoke(&audio,&AOBaseWriteToAudioTest::writeToAudioFromItemInvoke));
	
		EXPECT_CALL(audio,writeToAudioSilenceForRemainder(&buffer,40)).Times(1);
	}
	
	audio.testWriteToAudio(&buffer,systemTime);
	
	EXPECT_EQ(AudioItem::e_stateDone,itemA->state());
	EXPECT_EQ(AudioItem::e_stateFull,itemB->state());
	EXPECT_EQ(AudioItem::e_stateFull,itemC->state());
}

//-------------------------------------------------------------------------------------------

class AOBaseWriteToAudioIOCallback : public AOBaseTest
{
	public:
		MOCK_METHOD0(incrementMutexCount,void());
		MOCK_METHOD0(getAudioStartFlag,bool());
		MOCK_CONST_METHOD0(getState,States());
		MOCK_CONST_METHOD0(getCurrentOutTime,const common::TimeStamp&());
		MOCK_METHOD1(setCurrentCallbackTime,void(const common::TimeStamp& v));
		MOCK_METHOD1(updateCurrentPlayTimeFromStreamTime,void(const IOTimeStamp& systemTime));
		MOCK_METHOD0(writeToAudioPostProcess,void());
		MOCK_METHOD2(writeToAudio,void(AbstractAudioHardwareBuffer *pBuffer,const IOTimeStamp& systemTime));
		MOCK_METHOD2(writeToAudioSilenceToEndOfBuffer,void(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex));

		void testWriteToAudioIOCallback(AbstractAudioHardwareBuffer *pBuffer,const IOTimeStamp& systemTime);
};

//-------------------------------------------------------------------------------------------

void AOBaseWriteToAudioIOCallback::testWriteToAudioIOCallback(AbstractAudioHardwareBuffer *pBuffer,const IOTimeStamp& systemTime)
{
	writeToAudioIOCallback(pBuffer,systemTime);
}

//-------------------------------------------------------------------------------------------

void testWriteToAudioIOCallbackToWriteAudio(bool audioStartFlag,AOBase::States state)
{
	common::TimeStamp currentOutTime(5.0),currentCallbackTime;
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	
	AOBaseWriteToAudioIOCallback audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(state));
	EXPECT_CALL(audio,getAudioStartFlag()).WillRepeatedly(Return(audioStartFlag));
	EXPECT_CALL(audio,getCurrentOutTime()).Times(1).WillOnce(ReturnRef(currentOutTime));
	EXPECT_CALL(audio,setCurrentCallbackTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&currentCallbackTime));
	EXPECT_CALL(audio,incrementMutexCount()).Times(2);
	EXPECT_CALL(audio,updateCurrentPlayTimeFromStreamTime(systemTime)).Times(1);
	EXPECT_CALL(audio,writeToAudio(&buffer,systemTime)).Times(1);
	EXPECT_CALL(audio,writeToAudioPostProcess()).Times(1);
	
	audio.testWriteToAudioIOCallback(&buffer,systemTime);
}

//-------------------------------------------------------------------------------------------

void testWriteToAudioIOCallbackToWriteSilence(bool audioStartFlag,AOBase::States state)
{
	common::TimeStamp currentOutTime(5.0),currentCallbackTime;
	common::TimeStamp audioTimeStamp(30.0);
	IOTimeStamp systemTime(true,audioTimeStamp);

	AudioHardwareBufferMock buffer;
	
	AOBaseWriteToAudioIOCallback audio;
	EXPECT_CALL(audio,getState()).WillRepeatedly(Return(state));
	EXPECT_CALL(audio,getAudioStartFlag()).WillRepeatedly(Return(audioStartFlag));
	EXPECT_CALL(audio,getCurrentOutTime()).Times(1).WillOnce(ReturnRef(currentOutTime));
	EXPECT_CALL(audio,setCurrentCallbackTime(A<const common::TimeStamp&>())).Times(1).WillOnce(SaveArg<0>(&currentCallbackTime));
	EXPECT_CALL(audio,incrementMutexCount()).Times(2);
	EXPECT_CALL(audio,updateCurrentPlayTimeFromStreamTime(systemTime)).Times(1);
	EXPECT_CALL(audio,writeToAudioSilenceToEndOfBuffer(&buffer,0)).Times(1);
	EXPECT_CALL(audio,writeToAudioPostProcess()).Times(1);

	audio.testWriteToAudioIOCallback(&buffer,systemTime);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenNoCodecStateAndAudioStartIsOn)
{
	testWriteToAudioIOCallbackToWriteAudio(true,AOBase::e_stateNoCodec);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenNoCodecStateAndAudioStartIsOff)
{
	testWriteToAudioIOCallbackToWriteSilence(false,AOBase::e_stateNoCodec);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenPreBufferStateAndAudioStartIsOn)
{
	testWriteToAudioIOCallbackToWriteAudio(true,AOBase::e_statePreBuffer);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenPreBufferStateAndAudioStartIsOff)
{
	testWriteToAudioIOCallbackToWriteSilence(false,AOBase::e_statePreBuffer);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenPlayStateAndAudioStartIsOn)
{
	testWriteToAudioIOCallbackToWriteAudio(true,AOBase::e_statePlay);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenPlayStateAndAudioStartIsOff)
{
	testWriteToAudioIOCallbackToWriteSilence(false,AOBase::e_statePlay);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenPauseStateAndAudioStartIsOn)
{
	testWriteToAudioIOCallbackToWriteSilence(false,AOBase::e_statePause);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenPauseStateAndAudioStartIsOff)
{
	testWriteToAudioIOCallbackToWriteSilence(false,AOBase::e_statePause);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenStopStateAndAudioStartIsOn)
{
	testWriteToAudioIOCallbackToWriteSilence(false,AOBase::e_stateStop);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenStopStateAndAudioStartIsOff)
{
	testWriteToAudioIOCallbackToWriteSilence(false,AOBase::e_stateStop);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenCrossFadeStateAndAudioStartIsOn)
{
	testWriteToAudioIOCallbackToWriteAudio(true,AOBase::e_stateCrossFade);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,writeToAudioIOCallbackWhenCrossFadeStateAndAudioStartIsOff)
{
	testWriteToAudioIOCallbackToWriteSilence(false,AOBase::e_stateCrossFade);
}

//-------------------------------------------------------------------------------------------

class AOBaseExclusiveTest : public AOBaseTest
{
	public:
		AOBaseExclusiveTest();
		virtual ~AOBaseExclusiveTest();
		
		virtual void setCurrentState(AOBase::States state);
		virtual bool hasPlaybackBeenReset();
		virtual void setActiveDeviceIndex(tint deviceIdx);
		
	protected:
		
		bool m_resetPlaybackCalledFlag;
		QMutex m_exclusiveTestMutex;
        QWaitCondition m_exclusiveTestCondition;
		QString m_activeDeviceName;
		
		virtual bool init();
		virtual bool isLive() const;
		
		virtual QString getDeviceName(tint devIdx);
		virtual bool resetPlayback();
		virtual void audioEventMain(AudioEvent *e);
		virtual void closeAudio();
};

//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(AOFactory,AOBase, \
                            AOBaseExclusiveTestFactory,AOBaseExclusiveTest, \
                            "exclusive_test",false)

//-------------------------------------------------------------------------------------------

AOBaseExclusiveTest::AOBaseExclusiveTest() : AOBaseTest(),
	m_resetPlaybackCalledFlag(false),
	m_exclusiveTestMutex(),
	m_exclusiveTestCondition(),
	m_activeDeviceName("dummyAudioDevice")
{}

//-------------------------------------------------------------------------------------------

AOBaseExclusiveTest::~AOBaseExclusiveTest()
{}

//-------------------------------------------------------------------------------------------

bool AOBaseExclusiveTest::init()
{
	m_timer = new QTimer(this);
	QObject::connect(m_timer,SIGNAL(timeout()),this,SLOT(onTimer()));
	m_eventQueueTimer = new QTimer(this);
	QObject::connect(m_eventQueueTimer,SIGNAL(timeout()),this,SLOT(onEventTimer()));
	m_eventQueueTimer->start(100);
	return true;
}

//-------------------------------------------------------------------------------------------

bool AOBaseExclusiveTest::isLive() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

void AOBaseExclusiveTest::setCurrentState(AOBase::States state)
{
	setState(state);
}

//-------------------------------------------------------------------------------------------

bool AOBaseExclusiveTest::hasPlaybackBeenReset()
{
	bool res;
	m_exclusiveTestMutex.lock();
	m_exclusiveTestCondition.wait(&m_exclusiveTestMutex);
	m_exclusiveTestMutex.unlock();
	res = m_resetPlaybackCalledFlag;
    m_resetPlaybackCalledFlag = false;
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOBaseExclusiveTest::resetPlayback()
{
	m_resetPlaybackCalledFlag = true;
	return true;
}

//-------------------------------------------------------------------------------------------

void AOBaseExclusiveTest::audioEventMain(AudioEvent *e)
{
	AOBase::audioEventMain(e);
	m_exclusiveTestMutex.lock();
	m_exclusiveTestCondition.wakeAll();
	m_exclusiveTestMutex.unlock();
}

//-------------------------------------------------------------------------------------------

QString AOBaseExclusiveTest::getDeviceName(tint devIdx)
{
	if(devIdx==200)
	{
        return QString::fromUtf8("device200");
	}
	else if(devIdx==300)
	{
        return QString::fromUtf8("device300");
	}
    return m_activeDeviceName;
}

//-------------------------------------------------------------------------------------------

void AOBaseExclusiveTest::setActiveDeviceIndex(tint deviceIdx)
{
	m_deviceIdx = deviceIdx;
}

//-------------------------------------------------------------------------------------------

void AOBaseExclusiveTest::closeAudio()
{}

//-------------------------------------------------------------------------------------------

AOBaseQtUnitApplication::AOBaseQtUnitApplication(int argc,char **argv) : QCoreApplication(argc,argv),
	m_succeeded(false)
{
	QTimer::singleShot(10,this,SLOT(runTests()));
}

//-------------------------------------------------------------------------------------------

AOBaseQtUnitApplication::~AOBaseQtUnitApplication()
{}

//-------------------------------------------------------------------------------------------

bool AOBaseQtUnitApplication::testSucceeded() const
{
	return m_succeeded;
}

//-------------------------------------------------------------------------------------------

void AOBaseQtUnitApplication::runTests()
{
	AOBaseQtUnitTest tests;
	m_succeeded = (QTest::qExec(&tests)==0) ? true : false;
	quit();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,RunQtSpecificUnitTests)
{
    QStringList libPaths = QApplication::libraryPaths();
    {
        char *argv = 0;
        AOBaseQtUnitApplication tester(0,&argv);
        tester.exec();
        ASSERT_TRUE(tester.testSucceeded());
    }
    QApplication::setLibraryPaths(libPaths);
}

//-------------------------------------------------------------------------------------------

void AOBaseQtUnitTest::setExclusiveSettingsAreSpecificToActiveDevice()
{
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
        QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		pAudioTest->setCurrentState(AOBase::e_statePlay);
		pAudioTest->setActiveDeviceIndex(200);
		
        pAudioTest->setExclusiveMode(200,true);
		QVERIFY(pAudioTest->hasPlaybackBeenReset());
        QVERIFY(pAudioTest->isExclusive(200));
		
        pAudioTest->setExclusiveMode(300,false);
		QVERIFY(!pAudioTest->hasPlaybackBeenReset());
        QVERIFY(!pAudioTest->isExclusive(300));
	
		AOBase::end(audio);
	}
	
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		QVERIFY(pAudioTest->isExclusive(200));
		QVERIFY(!pAudioTest->isExclusive(300));
		
		AOBase::end(audio);
	}
}

//-------------------------------------------------------------------------------------------

void AOBaseQtUnitTest::setExclusiveInNoCodecState()
{
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		pAudioTest->setCurrentState(AOBase::e_stateNoCodec);
	
		pAudioTest->setExclusiveMode(false);
		QVERIFY(pAudioTest->hasPlaybackBeenReset());
		QVERIFY(!pAudioTest->isExclusive());
		
		pAudioTest->setExclusiveMode(true);
		QVERIFY(pAudioTest->hasPlaybackBeenReset());
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
	
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
}

//-------------------------------------------------------------------------------------------

void AOBaseQtUnitTest::setExclusiveInPreBufferState()
{
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		pAudioTest->setCurrentState(AOBase::e_statePreBuffer);
	
		pAudioTest->setExclusiveMode(false);
		QVERIFY(!pAudioTest->hasPlaybackBeenReset());
		QVERIFY(!pAudioTest->isExclusive());
		
		pAudioTest->setExclusiveMode(true);
		QVERIFY(!pAudioTest->hasPlaybackBeenReset());
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
	
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
}

//-------------------------------------------------------------------------------------------

void AOBaseQtUnitTest::setExclusiveInPlayState()
{
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		pAudioTest->setCurrentState(AOBase::e_statePlay);
	
		pAudioTest->setExclusiveMode(false);
		QVERIFY(pAudioTest->hasPlaybackBeenReset());
		QVERIFY(!pAudioTest->isExclusive());
		
		pAudioTest->setExclusiveMode(true);
		QVERIFY(pAudioTest->hasPlaybackBeenReset());
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
	
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
}

//-------------------------------------------------------------------------------------------

void AOBaseQtUnitTest::setExclusiveInCrossFadeState()
{
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
        pAudioTest->setCurrentState(AOBase::e_stateCrossFade);
	
		pAudioTest->setExclusiveMode(false);
		QVERIFY(pAudioTest->hasPlaybackBeenReset());
		QVERIFY(!pAudioTest->isExclusive());
		
		pAudioTest->setExclusiveMode(true);
		QVERIFY(pAudioTest->hasPlaybackBeenReset());
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
	
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
}

//-------------------------------------------------------------------------------------------

void AOBaseQtUnitTest::setExclusiveInPauseState()
{
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		pAudioTest->setCurrentState(AOBase::e_statePause);
	
		pAudioTest->setExclusiveMode(false);
		QVERIFY(!pAudioTest->hasPlaybackBeenReset());
		QVERIFY(!pAudioTest->isExclusive());
		
		pAudioTest->setExclusiveMode(true);
		QVERIFY(!pAudioTest->hasPlaybackBeenReset());
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
	
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
}

//-------------------------------------------------------------------------------------------

void AOBaseQtUnitTest::setExclusiveInStopState()
{
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		pAudioTest->setCurrentState(AOBase::e_stateStop);
	
		pAudioTest->setExclusiveMode(false);
		QVERIFY(!pAudioTest->hasPlaybackBeenReset());
		QVERIFY(!pAudioTest->isExclusive());
		
		pAudioTest->setExclusiveMode(true);
		QVERIFY(!pAudioTest->hasPlaybackBeenReset());
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
	
	{
		QSharedPointer<AOBase> audio = AOBase::get("exclusive_test");
		QVERIFY(!audio.isNull());
		QSharedPointer<AOBaseExclusiveTest> pAudioTest = audio.dynamicCast<AOBaseExclusiveTest>();
		QVERIFY(!pAudioTest.isNull());
		
		QVERIFY(pAudioTest->isExclusive());
	
		AOBase::end(audio);
	}
}

//-------------------------------------------------------------------------------------------

class AOCoreAudioGetCoreAudioDeviceTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(currentOutputDeviceIndex,int());
		MOCK_METHOD0(getDeviceInfo,AOQueryDevice *());
		MOCK_METHOD1(setDeviceIndex,void(int idx));
		virtual QSharedPointer<AOQueryDevice::Device> copyDeviceInformation(const AOQueryDevice::Device& iDevice);
		
		virtual QSharedPointer<AOQueryDevice::Device> getCurrentDevice();
};

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryDevice::Device> AOCoreAudioGetCoreAudioDeviceTest::copyDeviceInformation(const AOQueryDevice::Device& iDevice)
{
	QSharedPointer<AOQueryDevice::Device> pDevice(new AOQueryDevice::Device(iDevice));
	return pDevice;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryDevice::Device> AOCoreAudioGetCoreAudioDeviceTest::getCurrentDevice()
{
	return AOBase::getCurrentDevice();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,getCurrentDeviceWithInvalidDefaultAndInfoIsInvalid)
{
	AOQueryDeviceMock devices;
	EXPECT_CALL(devices,noDevices()).WillRepeatedly(Return(2));
	EXPECT_CALL(devices,defaultDeviceIndex()).WillRepeatedly(Return(4));

	AOCoreAudioGetCoreAudioDeviceTest audio;
    EXPECT_CALL(audio,currentOutputDeviceIndex()).WillRepeatedly(Return(3));
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&devices));
	
	QSharedPointer<AOQueryDevice::Device> pDevice = audio.getCurrentDevice();
	ASSERT_TRUE(pDevice.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,getCurrentDeviceWithInvalidDefaultAndInfoDefaultIsUninitialized)
{
	AOQueryDevice::Device deviceA;
	deviceA.id() = "Device";
	deviceA.name() = "SoundBlaster";
	deviceA.addFrequency(44100);
	deviceA.addFrequency(48000);
	deviceA.setNoChannels(4);
	deviceA.channel(0).name() = "FL";
	deviceA.channel(1).name() = "FR";
	deviceA.channel(2).name() = "BL";
	deviceA.channel(3).name() = "BR";

	AOQueryDeviceMock devices;
	EXPECT_CALL(devices,device(0)).WillRepeatedly(ReturnRef(deviceA));
	EXPECT_CALL(devices,noDevices()).WillRepeatedly(Return(2));
	EXPECT_CALL(devices,queryDevice(0)).Times(1).WillOnce(Return(true));
	EXPECT_CALL(devices,defaultDeviceIndex()).WillRepeatedly(Return(0));

	AOCoreAudioGetCoreAudioDeviceTest audio;
	EXPECT_CALL(audio,currentOutputDeviceIndex()).Times(1).WillOnce(Return(3));
	EXPECT_CALL(audio,setDeviceIndex(0)).Times(1);
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&devices));
	
	QSharedPointer<AOQueryDevice::Device> pDevice = audio.getCurrentDevice();
	ASSERT_FALSE(pDevice.isNull());
	
	EXPECT_TRUE(pDevice->id()=="Device");
	EXPECT_TRUE(pDevice->name()=="SoundBlaster");
	EXPECT_TRUE(pDevice->isFrequencySupported(44100));
	EXPECT_TRUE(pDevice->isFrequencySupported(48000));
	EXPECT_FALSE(pDevice->isFrequencySupported(192000));
	ASSERT_EQ(4,pDevice->noChannels());
	EXPECT_TRUE(pDevice->channel(0).name()=="FL");
	EXPECT_TRUE(pDevice->channel(1).name()=="FR");
	EXPECT_TRUE(pDevice->channel(2).name()=="BL");
	EXPECT_TRUE(pDevice->channel(3).name()=="BR");
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,getCurrentDeviceWithDefaultUninitializedAndFailureOnQuery)
{
	AOQueryDevice::Device deviceA;
	deviceA.id() = "Device";
	deviceA.name() = "SoundBlaster";
	deviceA.addFrequency(44100);
	deviceA.addFrequency(48000);
	deviceA.setNoChannels(4);
	deviceA.channel(0).name() = "FL";
	deviceA.channel(1).name() = "FR";
	deviceA.channel(2).name() = "BL";
	deviceA.channel(3).name() = "BR";

	AOQueryDeviceMock devices;
	EXPECT_CALL(devices,device(1)).WillRepeatedly(ReturnRef(deviceA));
	EXPECT_CALL(devices,noDevices()).WillRepeatedly(Return(2));
	EXPECT_CALL(devices,queryDevice(1)).Times(1).WillOnce(Return(false));

	AOCoreAudioGetCoreAudioDeviceTest audio;
    EXPECT_CALL(audio,currentOutputDeviceIndex()).WillRepeatedly(Return(1));
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&devices));
	
	QSharedPointer<AOQueryDevice::Device> pDevice = audio.getCurrentDevice();
	ASSERT_TRUE(pDevice.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,getCurrentDeviceWithDefaultUninitializedAndSuccessOnQuery)
{
	AOQueryDevice::Device deviceA;
	deviceA.id() = "Device";
	deviceA.name() = "SoundBlaster";
	deviceA.addFrequency(44100);
	deviceA.addFrequency(48000);
	deviceA.setNoChannels(4);
	deviceA.channel(0).name() = "FL";
	deviceA.channel(1).name() = "FR";
	deviceA.channel(2).name() = "BL";
	deviceA.channel(3).name() = "BR";

	AOQueryDeviceMock devices;
	EXPECT_CALL(devices,device(1)).WillRepeatedly(ReturnRef(deviceA));
	EXPECT_CALL(devices,noDevices()).WillRepeatedly(Return(2));
	EXPECT_CALL(devices,queryDevice(1)).Times(1).WillOnce(Return(true));
	
	AOCoreAudioGetCoreAudioDeviceTest audio;
    EXPECT_CALL(audio,currentOutputDeviceIndex()).WillRepeatedly(Return(1));
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&devices));
	
	QSharedPointer<AOQueryDevice::Device> pDevice = audio.getCurrentDevice();
	ASSERT_FALSE(pDevice.isNull());
	
	EXPECT_TRUE(pDevice->id()=="Device");
	EXPECT_TRUE(pDevice->name()=="SoundBlaster");
	EXPECT_TRUE(pDevice->isFrequencySupported(44100));
	EXPECT_TRUE(pDevice->isFrequencySupported(48000));
	EXPECT_FALSE(pDevice->isFrequencySupported(192000));
	ASSERT_EQ(4,pDevice->noChannels());
	EXPECT_TRUE(pDevice->channel(0).name()=="FL");
	EXPECT_TRUE(pDevice->channel(1).name()=="FR");
	EXPECT_TRUE(pDevice->channel(2).name()=="BL");
	EXPECT_TRUE(pDevice->channel(3).name()=="BR");
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,getCurrentDeviceWithDefaultAlreadyInitialized)
{
	AOQueryDevice::Device deviceA;
	deviceA.setInitialized();
	deviceA.id() = "Device";
	deviceA.name() = "SoundBlaster";
	deviceA.addFrequency(44100);
	deviceA.addFrequency(48000);
	deviceA.setNoChannels(4);
	deviceA.channel(0).name() = "FL";
	deviceA.channel(1).name() = "FR";
	deviceA.channel(2).name() = "BL";
	deviceA.channel(3).name() = "BR";

	AOQueryDeviceMock devices;
	EXPECT_CALL(devices,device(1)).WillRepeatedly(ReturnRef(deviceA));
	EXPECT_CALL(devices,noDevices()).WillRepeatedly(Return(2));

	AOCoreAudioGetCoreAudioDeviceTest audio;
    EXPECT_CALL(audio,currentOutputDeviceIndex()).WillRepeatedly(Return(1));
	EXPECT_CALL(audio,getDeviceInfo()).WillRepeatedly(Return(&devices));
	
	QSharedPointer<AOQueryDevice::Device> pDevice = audio.getCurrentDevice();
	ASSERT_FALSE(pDevice.isNull());
	
	EXPECT_TRUE(pDevice->id()=="Device");
	EXPECT_TRUE(pDevice->name()=="SoundBlaster");
	EXPECT_TRUE(pDevice->isFrequencySupported(44100));
	EXPECT_TRUE(pDevice->isFrequencySupported(48000));
	EXPECT_FALSE(pDevice->isFrequencySupported(192000));
	ASSERT_EQ(4,pDevice->noChannels());
	EXPECT_TRUE(pDevice->channel(0).name()=="FL");
	EXPECT_TRUE(pDevice->channel(1).name()=="FR");
	EXPECT_TRUE(pDevice->channel(2).name()=="BL");
	EXPECT_TRUE(pDevice->channel(3).name()=="BR");
}

//-------------------------------------------------------------------------------------------

class AOBaseGetSourceDescriptionTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(getCodec, engine::Codec *());
		MOCK_CONST_METHOD0(getFrequency,tint());
		FormatDescription getSourceDescription(tint noChannels);
};

//-------------------------------------------------------------------------------------------

FormatDescription AOBaseGetSourceDescriptionTest::getSourceDescription(tint noChannels)
{
	return AOBase::getSourceDescription(noChannels);
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, getSourceDescriptionNoCodec)
{
	AOBaseGetSourceDescriptionTest audio;
    EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(reinterpret_cast<engine::Codec *>(NULL)));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(192000));
	
	FormatDescription desc;
	desc = audio.getSourceDescription(6);
	
	EXPECT_EQ(FormatDescription::e_DataFloatDouble,desc.typeOfData());
	EXPECT_EQ(64,desc.bits());
	EXPECT_EQ(6,desc.channels());
	EXPECT_EQ(192000,desc.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, getSourceDescriptionCodecInt32)
{
	CodecMock codec;
	EXPECT_CALL(codec, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleInt32 | engine::e_SampleFloat));

	AOBaseGetSourceDescriptionTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(48000));
	
	FormatDescription desc;
	desc = audio.getSourceDescription(2);
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger, desc.typeOfData());
	EXPECT_EQ(32, desc.bits());
	EXPECT_EQ(2, desc.channels());
	EXPECT_EQ(48000, desc.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, getSourceDescriptionCodecInt24)
{
	CodecMock codec;
	EXPECT_CALL(codec, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleInt24 | engine::e_SampleFloat));

	AOBaseGetSourceDescriptionTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(48000));
	
	FormatDescription desc;
	desc = audio.getSourceDescription(2);
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger, desc.typeOfData());
	EXPECT_EQ(24, desc.bits());
	EXPECT_EQ(2, desc.channels());
	EXPECT_EQ(48000, desc.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, getSourceDescriptionCodecInt16)
{
	CodecMock codec;
	EXPECT_CALL(codec, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleInt16 | engine::e_SampleFloat));

	AOBaseGetSourceDescriptionTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(48000));
	
	FormatDescription desc;
	desc = audio.getSourceDescription(2);
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger, desc.typeOfData());
	EXPECT_EQ(16, desc.bits());
	EXPECT_EQ(2, desc.channels());
	EXPECT_EQ(48000, desc.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, getSourceDescriptionCodecNoInteger)
{
	CodecMock codec;
	EXPECT_CALL(codec, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));

	AOBaseGetSourceDescriptionTest audio;
	EXPECT_CALL(audio,getCodec()).WillRepeatedly(Return(&codec));
	EXPECT_CALL(audio,getFrequency()).WillRepeatedly(Return(48000));
	
	FormatDescription desc;
	desc = audio.getSourceDescription(2);
	
	EXPECT_EQ(FormatDescription::e_DataFloatDouble, desc.typeOfData());
	EXPECT_EQ(64, desc.bits());
	EXPECT_EQ(2, desc.channels());
	EXPECT_EQ(48000, desc.frequency());
}

//-------------------------------------------------------------------------------------------

class AOBaseSlotCallTest : public AOBaseTest
{
	public:
		MOCK_METHOD0(doTimer,void());
        MOCK_METHOD1(doCodecInit,void(void *cPtr));
        MOCK_METHOD0(doEventTimer,void());
		
		virtual bool testCanCallSlot(SlotType type);
		virtual bool testCanCallSlot(SlotType type,void *cData);
		virtual void testSlotComplete();
};

//-------------------------------------------------------------------------------------------

bool AOBaseSlotCallTest::testCanCallSlot(SlotType type)
{
	return canCallSlot(type);
}

//-------------------------------------------------------------------------------------------

bool AOBaseSlotCallTest::testCanCallSlot(SlotType type,void *cData)
{
	return canCallSlot(type,cData);
}

//-------------------------------------------------------------------------------------------

void AOBaseSlotCallTest::testSlotComplete()
{
	slotComplete();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,slotCallSequential)
{
	char mem[1];

	AOBaseSlotCallTest audio;
	
	ASSERT_TRUE(audio.testCanCallSlot(AOBase::e_onTimer));
	audio.testSlotComplete();
	ASSERT_TRUE(audio.testCanCallSlot(AOBase::e_onCodecInit,reinterpret_cast<void *>(mem)));
	audio.testSlotComplete();
	ASSERT_TRUE(audio.testCanCallSlot(AOBase::e_onEventTimer));
	audio.testSlotComplete();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,slotCallRecursiveSingle)
{
	AOBaseSlotCallTest audio;
	EXPECT_CALL(audio,doTimer()).Times(1);
	
	ASSERT_TRUE(audio.testCanCallSlot(AOBase::e_onTimer));
	ASSERT_FALSE(audio.testCanCallSlot(AOBase::e_onTimer));
	audio.testSlotComplete();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase,slotCallRecursiveSequence)
{
	char mem[1];

	AOBaseSlotCallTest audio;

    using ::testing::InSequence;
	{
        InSequence slotSequence;
        EXPECT_CALL(audio,doCodecInit(mem)).Times(1);
        EXPECT_CALL(audio,doTimer()).Times(1);
        EXPECT_CALL(audio,doEventTimer()).Times(1);
	}
	
	ASSERT_TRUE(audio.testCanCallSlot(AOBase::e_onTimer));
	ASSERT_FALSE(audio.testCanCallSlot(AOBase::e_onCodecInit,reinterpret_cast<void *>(mem)));
	ASSERT_FALSE(audio.testCanCallSlot(AOBase::e_onTimer));
	ASSERT_FALSE(audio.testCanCallSlot(AOBase::e_onEventTimer));
	audio.testSlotComplete();
}

//-------------------------------------------------------------------------------------------

class AOBaseNextCodecSeamless : public AOBaseTest
{
	public:
		virtual void setCodec(engine::Codec *c);
		virtual void setNextCodec(engine::Codec *c);
		virtual void setNoInChannels(tint n);
		virtual bool isNextCodecSeamless();
};

//-------------------------------------------------------------------------------------------

void AOBaseNextCodecSeamless::setCodec(engine::Codec *c)
{
	AOBase::setCodec(c);
}

//-------------------------------------------------------------------------------------------

void AOBaseNextCodecSeamless::setNextCodec(engine::Codec *c)
{
	AOBase::setNextCodec(c);
}

//-------------------------------------------------------------------------------------------

void AOBaseNextCodecSeamless::setNoInChannels(tint n)
{
	AOBase::setNoInChannels(n);
}

//-------------------------------------------------------------------------------------------

bool AOBaseNextCodecSeamless::isNextCodecSeamless()
{
	return AOBase::isNextCodecSeamless();
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessWhenNoCurrentCodec)
{
	CodecMock codecNext;
	
	AOBaseNextCodecSeamless audio;
	audio.setCodec(NULL);
	audio.setNextCodec(&codecNext);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessWhenNoNextCodec)
{
	CodecMock codecCurrent;
	
	AOBaseNextCodecSeamless audio;
	audio.setCodec(&codecCurrent);
	audio.setNextCodec(NULL);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessWhenDifferentFrequencies)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));

	CodecMock codecN;
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(48000));
	
	AOBaseNextCodecSeamless audio;
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessWhenDifferentChannels)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(4));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsSeamlessGivenBothCodecsUseFloat)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_TRUE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessGivenCurrentFloatAndNextInteger16)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt16));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsSeamlessGivenBothCodecsUseInteger16)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt16));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt16));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_TRUE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessGivenCurrentInteger16AndNextFloat)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt16));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessGivenCurrentInteger16AndNextInteger24)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt16));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt24));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsSeamlessGivenBothCodecsUseInteger24)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt24));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt24));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_TRUE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessGivenCurrentInteger24AndNextFloat)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt24));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessGivenCurrentInteger24AndNextInteger16)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt24));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt16));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsSeamlessGivenBothCodecsUseInteger32)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt32));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt32));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_TRUE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessGivenCurrentInteger32AndNextFloat)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt32));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------

TEST(AOBase, nextCodecIsNotSeamlessGivenCurrentInteger32AndNextInteger24)
{
	CodecMock codecC;
	EXPECT_CALL(codecC, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecC, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt32));

	CodecMock codecN;
	EXPECT_CALL(codecN, noChannels()).WillRepeatedly(Return(2));
	EXPECT_CALL(codecN, frequency()).WillRepeatedly(Return(44100));
	EXPECT_CALL(codecN, dataTypesSupported()).WillRepeatedly(Return(engine::e_SampleFloat | engine::e_SampleInt24));
	
	AOBaseNextCodecSeamless audio;
	audio.setNoInChannels(2);
	audio.setCodec(&codecC);
	audio.setNextCodec(&codecN);
	
	EXPECT_FALSE(audio.isNextCodecSeamless());
}

//-------------------------------------------------------------------------------------------
