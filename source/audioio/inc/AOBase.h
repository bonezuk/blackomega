//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOBASE_H
#define __OMEGA_AUDIOIO_AOBASE_H
//-------------------------------------------------------------------------------------------

// Qt
#include <QObject>
#include <QThread>
#include <QEvent>
#include <QWaitCondition>
#include <QSettings>
#include <QCoreApplication>
#include <QTimer>
#include <QStringList>
#include <QSharedPointer>

// Omega
#include "common/inc/TimeStamp.h"
#include "common/inc/Factory.h"
#include "common/inc/ServiceEvent.h"
#include "engine/inc/RData.h"
#include "engine/inc/Codec.h"
#include "engine/inc/Resample.h"
#include "audioio/inc/AudioIODLL.h"
#include "audioio/inc/AOQueryDevice.h"
#include "audioio/inc/IOTimeStamp.h"
#include "audioio/inc/AbstractAudioHardwareBuffer.h"
#include "engine/inc/RemezFIR.h"
#include "engine/inc/FIRFilter.h"
#include "engine/inc/FIRFilter200HzLowPass.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AudioThread;
class AudioItem;
class AudioEvent;
#if defined(DEBUG_LOG_AUDIOOUTPUT)
class AODebugItem;
#endif

//-------------------------------------------------------------------------------------------

const tint c_kMaxOutputChannels = 32;

//-------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOResampleInfo
{
    public:
        AOResampleInfo();
        AOResampleInfo(const AOResampleInfo& rhs);

        const AOResampleInfo& operator = (const AOResampleInfo& rhs);

        common::TimeStamp& start();
        const common::TimeStamp& start() const;
        common::TimeStamp& end();
        const common::TimeStamp& end() const;
        bool& complete();
        const bool& complete() const;

    protected:
        common::TimeStamp m_start;
        common::TimeStamp m_end;
        bool m_complete;
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOBase : public QObject
{
    public:
        Q_OBJECT

    public:

        typedef enum
        {
            e_stateNoCodec = 0,
            e_statePreBuffer,
            e_statePlay,
            e_statePause,
            e_stateStop,
            e_stateCrossFade
        } States;

        typedef enum
        {
            e_codecNoPlay = 0,
            e_codecSingle,
            e_codecNextQueued,
            e_codecCurrentFinish,
            e_codecSingleFinish
        } CodecState;

        typedef enum
        {
            e_onTimer = 0,
            e_onCodecInit,
            e_onEventTimer,
            e_onAudioEvent
        } SlotType;

        static const tint c_noSamplesPerAudioItem = 2048;

        friend class AudioThread;
        friend class AudioItem;

    public:
        AOBase(QObject *parent = 0);
        virtual ~AOBase();

        static bool startAudioService();
        static void stopAudioService();

        static QSharedPointer<AOBase> get(const QString& type);
        static void end(QSharedPointer<AOBase> audioOutput);

        virtual void open(const QString& url);
        virtual void open(const QString& url,const common::TimeStamp& t);
        virtual void open(const QString& url,const common::TimeStamp& t,const common::TimeStamp& len);
        virtual void next(const QString& url);
        virtual void next(const QString& url,const common::TimeStamp& t);
        virtual void next(const QString& url,const common::TimeStamp& t,const common::TimeStamp& len);
        virtual void nextCrossfade(const QString& url);
        virtual void nextCrossfade(const QString& url,const common::TimeStamp& t);
        virtual void nextCrossfade(const QString& url,const common::TimeStamp& t,const common::TimeStamp& len);

        virtual void play();
        virtual void pause();
        virtual void stop();
        virtual void resetPlay();

        virtual void seek(const common::TimeStamp& t);

        virtual void setVolume(sample_t vol);
        virtual sample_t getVolume();

        virtual bool isCrossfade() const;
        virtual common::TimeStamp getCrossfade() const;
        virtual void setCrossfade(const common::TimeStamp& t);

        virtual int noDevices();
        virtual QString deviceName(int devIdx);
        virtual QSharedPointer<AOQueryDevice::Device> device(int devIdx);

        virtual int currentOutputDeviceIndex();
        virtual void setOutputDevice(int devIdx);

        virtual void updateChannelMap(int devIdx);

        virtual Qt::HANDLE threadId();

        virtual bool isExclusive();
        virtual bool isExclusive(int devIdx);

        virtual void forceBitsPerSample(tint noBits);

    protected:

        // Default device index references.
        static tint m_audioStartCount;
        static tint m_defaultDeviceIndex;
        static AOQueryDevice *m_deviceInfo;
        static QRecursiveMutex m_deviceInfoMutex;

        States m_state;

        // number of items currently in the cyclic buffer, set when the cyclic buffer is initialised.
        tint m_noOfCyclicBufferItems;

        // number of channels from codec
        tint m_noInChannels;
        // number of output channels for current speaker configuration
        tint m_noOutChannels;
        // number of output channels on audio device
        tint m_noChannels;
        tint m_frequency;
        // the frequency of the playing codec
        tint m_codecFrequency;

        AudioThread *m_thread;
        Qt::HANDLE m_threadId;

        QTimer *m_timer;

        // State of the current set of codecs.
        CodecState m_codecState;

        // The current codec that is being played.
        engine::Codec *m_codec;
        // The next codec that is to be played.
        engine::Codec *m_nextCodec;
        // Complete codec next to be freed.
        engine::Codec *m_completeCodec;

        // Audio item to be processed by callback.
        AudioItem *m_callbackAudioItem;
        // Audio item to be processed by codec.
        AudioItem *m_codecAudioItem;

        // The audio device index reference
        tint m_deviceIdx;

        // The current play time as updated by audio callback.
        volatile common::TimeStamp m_currentPlayTime;

        // The time stamp for the last sample processed by the callback.
        volatile common::TimeStamp m_currentCallbackTime;

        // Wall clock time when the first sample in the output audio stream is sent to
        // the sound card hardware.
        common::TimeStamp m_audioStartClock;

        // The playback time for the last sample, belonging to the current stream, that
        // has been processed by the callback.
        common::TimeStamp m_currentOutTime;

        // Stream time of playback was last paused.
        common::TimeStamp m_pauseTime;

        // The time stamp of the last processed codec package.
        common::TimeStamp m_codecCurrentTime;

        // The time length of the current codec.
        common::TimeStamp m_codecTimeLength;

        // The time position in the current codec at which playback is complete. If zero then does not apply.
        common::TimeStamp m_codecTimePositionComplete;

        // The codec time when to initialize the next codec
        common::TimeStamp m_nextCodecTime;

        // The time length to play the next codec for. If zero then does not apply.
        common::TimeStamp m_nextCodecTimeLengthComplete;

        // The codec time when to start the cross fade effect.
        common::TimeStamp m_crossFadeTime;

        // The time of the next frame in cross fade effect.
        common::TimeStamp m_frameFadeTime;

        // The length of the cross fade operation
        common::TimeStamp m_crossFadeTimeLen;

        // Length of time to set cross-fade operation
        common::TimeStamp m_progFadeTime;

        // The length of time between return from bufferSwitch callback and
        // the first sample being physically played.
        common::TimeStamp m_outputLatencyTime;

        // Flag indicating that the output callback has been called to start processing
        // the output audio stream.
        volatile bool m_audioStartFlag;

        // Mutex counter to ensure sync between callback and process.
        volatile tint m_mutexCount;

        // State variable, ensure codec data is in time to be queued once playback starts.
        volatile tint m_trackTimeState;
        // In case we don't want to track time state after unpause then check flag
        volatile bool m_trackTimeStateFlag;

        // The wall clock reference time when audio playback is to stop
        volatile common::TimeStamp m_stopTimeClock;

        // Flag to indicate that the stop clock has been set
        volatile bool m_stopTimeFlag;

        // To set or not to set the cross-fade time.
        tint m_progFadeState;

        // Audio process type, used to track relationship between codec and item stream.
        // m_audioProcessType: Initially set to 0.
        // When 0:
        //  - At start of loop in processCodec the cyclic buffer item being populated with audio data is
        //    marked as processed by the codec and the data in the item is reset.
        //
        // When 1:
        //  - The call to populate the cyclic buffer item with audio data is not made when in this state.
        //  - After the onReadForNext signal emitted, if crossfade option is used and crossfade position
        //    has been reached in the current track then m_audioProcessType is set to 1.
        //  - No postProcess is carried out on the unpacked audio data.
        //
        // When 2:
        //  - Once populated with audio data the last item part is marked as bein the end of the current
        //    audio track. This information is in turn used by the rendering callback to synchronise the associated
        //    rendering clocks and timers.
        //  - The processing for detection of end of audio track is not done. This was added in as an additional
        //    validation check during refactoring, but it has no effect because immediately before doing end of
        //    track detection the m_audioProcessType value is set to 0.
        //  - When the actual track is complete and the current cyclic buffer item has been populated with the
        //    last samples in the track, the stopCodecDoNext method is called. At this point m_audioProcessType is
        //    set to 2.
        //
        // Meanings:
        //   0 : Process and decode data as normal.
        //   1 : The current track has reached the start of crossfade mixing.
        //   2 : The cyclic buffer item being populated holds the last samples of the current track and at the next
        //       loop the subsequent part will be the start of the next audio track.
        tint m_audioProcessType;

        // Indicates the state of the next process stream in relation to the current.
        // Used for tracking playback of next stream when cross-fade operation is in effect.
        // m_nextOutState: Initially set to 0.
        //
        // When 0:
        //  - For part of decoded audio data, while processing the codec, the next codec timestamp is checked with the
        //    decoded part's start timestamp. If the start of the decoded part occurs after the next codec timestamp
        //    then onReadyForNext() signal is emitted to the player and subsequently set to 1.
        //  - If crossfade not active and processing phase then the m_nextOutState is set to 0 while being unpaused.
        //    This same logic is applied while resetting playback.
        //  - The m_nextOutState is reset to 0 when the next codec timestamp is being calculated.
        //
        // When 1:
        //  - In startNextCodec given that the next codec is compatible with the current, same channel and frequency
        //    requirements, and cross fading the m_nextOutState is set to 1.
        //  - If crossfade is active then this checks the decoded part to see when to start the crossfade mixing process.
        //  - When unpausing and is in crossfade processing phase then m_nextOutState is set to 1.
        //    This same logic is applied while resetting playback.
        //
        // When 2:
        //  - In startNextCodec, when the next codec is not compatible with the current codec that is being played
        //    then the m_nextOutState is set to 2 and the onNoNext signal is emitted to the player.
        //  - No action is taken while processing the decoding of the codec when in this state.
        //
        // Meanings:
        //   0 : The time position in the current codec when to signal to the player it is ready to open and initialise
        //       the next codec in the playlist has been calculated. When this time position is encountered in the playback
        //       of the current codec the onReadyForNext signal is emitted and the state set to 1.
        //   1 : The onReadyForNext signal has been emitted and if crossfade is active then is initialised at the crossfade
        //       position of the current codec.
        //   2 : The next codec in sequence requires the audio device to be closed and reopened for playback of the next
        //       track, as it is not compatable for seamless transition. The onNoNext signal is emitted to the player and
        //       the state is set to 2 such that processing of the next codec is done by reinitialising the audio layer
        //       accordingly.
        tint m_nextOutState;

        // The name of the next codec
        QString m_nextName;

        // Level of volume
        sample_t m_volume;

        // Set to true on pause and false when unpaused.
        bool m_pauseAudioFlag;

        // Update m_codecTimeLength of not.
        bool m_codecTimeLengthUpdate;

        // Holds the volume levels for the cross-fade effect.
        sample_t *m_crossFadeAWin;
        sample_t *m_crossFadeBWin;

        // Used to layout the samples for the cross fade channel effect.
        // The length of each is equal to the length by frameLength * frameSize.
        sample_t *m_crossASample;
        sample_t *m_crossBSample;

        // The audio item used for receiving output from the next codec during
        // a cross fade effect operation.
        AudioItem *m_crossFadeItem;

        // true if cross fade on else false.
        bool m_crossFadeFlag;

        // Flag to transmit onStart signal once next song is actually playing
        bool m_startNextTrackFlag;

        // The reference time to start audio playback for remote codec
        common::TimeStamp m_refStartAudioTime;

        // relay fade flag for remote codec from startNextCodec to onCodecInit
        bool m_nrCrossfadeFlag;

        // When the audio loop prematurely reads ahead of the decoded codec how and where to resume
        // the audio signal after writing silence is controlled by this flag. If true the audio written
        // to the sound card is synchronised to the timestamp on the codec. This is required when the
        // codec is in sync with other playback streams. If false then the audio resumes at the next
        // sample and the output time is adjusted accordingly.
        bool m_syncAudioToTimestamp;

        // Used in writeToAudio when tracking that silence has been written and to resume on the next
        // valid audio data item.
        bool m_silenceIsWritten;

        // Resampling variables
        bool m_resampleFlag;
        tfloat64 m_resampleRatio;
        AudioItem *m_resampleItem;
        engine::Resample *m_resample[8];
        int m_rInFrequency;
        tint m_rUsedI;
        tint m_rUsedO;
        tint m_rSrcLen;
        tint m_rDstLen;
        tint m_rOutNo;
        sample_t *m_rIn[8];
        sample_t *m_rOut[8];
        tfloat64 m_rOutDrift;
        bool m_rCodecCompleteFlag;
        QList<AOResampleInfo> m_resampleList;

        // Time to seek the next codec when it starts to play.
        common::TimeStamp m_nextCodecSeekTime;
        // Time to seek the current codec when it starts to play.
        common::TimeStamp m_startCodecSeekTime;

        // Output channel map from AData::outData to m_bufferInfos output
        int m_outputChannelArray[c_kMaxOutputChannels];

        // Codec stream used to merge into output stream to playback
        // Original functionality was to play unlicensed message, deprecated
        engine::Codec *m_mergeCodec;
        // Audio item used for merging to output stream.
        AudioItem *m_mergeAudioItem;
        // Current time of merge same.
        common::TimeStamp m_mergeCodeTime;
        // Running time of playback for merge.
        common::TimeStamp m_mergeCurrentPlayTime;
        // The next time that the merge codec is to dubbed in.
        common::TimeStamp m_mergeNextPlayTime;

        QTimer *m_eventQueueTimer;
        QMutex m_eventQueueMutex;
        QList<QEvent *> m_eventQueue;

        // Prevent slot being recursively called from internal QProcessEvent calls
        QList<QPair<SlotType,void *> > m_recursiveSlotList;

        // Debug hack to set the number of bits per sample on output DAC.
        tint m_forceBitsPerSample;

        // Low band pass FIR Filter for generation of LFE channel
        QSharedPointer<engine::FIRFilter> m_lfeFilter;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual void printToLog(const tchar *msg) const;

        virtual bool init();
        virtual void reset();

        virtual void initCyclicBuffer();
        virtual void freeCyclicBuffer();
        virtual void flushCyclicBuffer();

        virtual common::TimeStamp getCacheTimeLength() const;

        virtual QString getActiveDeviceName();
        virtual QString getDeviceName(tint devIdx);

        virtual engine::AData *allocateData(tint len,tint inChannel,tint outChannel) = 0;

        virtual bool startCodec(const QString& url,const common::TimeStamp& t,const common::TimeStamp& tLen);
        virtual void stopCodec(bool eFlag = true);

        // startNextCodec: Called in response to player handling the onReadyForNext signal where the player gives the url
        // resource of the next track to be queued. Handled in the audio thread, it works to open and initialise the next
        // track that is to be played. The codec has to have the same number of channels and frequency in order to be
        // queued such that seamless playback can occur.
        //  - If the current codec has finished being decoded into the cyclic buffer then it immediately put in place as
        //    the current codec such that it can be decoded on the next call to processCodec. Otherwise it put in place
        //    as the next codec ready for playback.
        //  - Where the given codec is not compatible with the currently open audio DAC settings used by the codec being
        //    played, or actually fails to open, then it is not queued and the onNoNext signal is emitted to the player.
        virtual bool startNextCodec(const QString& url,const common::TimeStamp& nT,const common::TimeStamp& nTLen,bool fade = false);

        virtual bool stopCodecDoNext();
        virtual void stopNextCodec();

        virtual void setCodecCompletePositionFromNext();

        virtual engine::Codec *createNewCodecFromUrl(const QString& url) const;
        virtual void connectPreBufferedRemoteCodec();
        virtual void startPlayTimeWithSingleCodec();
        virtual void startInternalTimer(int period);
        virtual void emitOnStart(const QString& url);

        virtual void connectPreBufferedNextRemoteCodec(const QString& url,bool fade);
        virtual void calculateNextCodecCrossFadeTime();
        virtual void emitOnNoNext();

        virtual bool startAudio(const QString& url);
        virtual void stopAudio(bool eFlag = true);

        virtual bool openAudio() = 0;
        virtual void closeAudio() = 0;
        virtual bool startAudioDevice() = 0;
        virtual void stopAudioDevice() = 0;
        virtual bool isAudio() const = 0;

        virtual void processCodec(bool initF=false);
        virtual AudioItem *processCodecLoop(AudioItem *item,bool& initF,bool& loop);

        virtual bool processCodecState(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF);
        virtual bool processCodecPlay(AudioItem **pItem,const common::TimeStamp& currentT,bool& initF);
        virtual bool processCodecPlayDecode(AudioItem* item,const common::TimeStamp& currentT,bool& initF);
        virtual bool processCodecPlayDecodeInTime(AudioItem *item,const common::TimeStamp& currentT,bool& initF);
        virtual void processCodecPlayTagPartAsRequired(engine::RData *data);
        virtual bool processCodecEndForTimePositionComplete(AudioItem *item,bool decodeFlag);

        virtual void processCodecReadyForNext(AudioItem *item,bool completeFlag,tint iFrom);
        virtual void processCodecPlayNextEndInParts(engine::RData *data,bool completeFlag,tint iFrom);
        virtual void processCodecPlayNextOutStateZero(engine::RData::Part& part);
        virtual void processCodecPlayNextOutStateOne(engine::RData::Part& part,engine::RData *data);

        virtual bool processCodecPlayPostProcess(AudioItem **pItem,const common::TimeStamp& currentT,bool completeFlag);
        virtual bool processCodecPlayPostProcessComplete(AudioItem **pItem,const common::TimeStamp& currentT);
        virtual bool processCodecPlayPostProcessCompleteRemote(AudioItem **pItem,const common::TimeStamp& currentT);
        virtual bool processCodecPlayPostProcessCompleteLocal(AudioItem **pItem);
        virtual bool processCodecPlayPostProcessRunning(AudioItem **pItem);
        virtual bool processCodecPlayPostProcessCheckBufferedState(AudioItem **pItem);
        virtual void processCodecPlayPostProcessRunningWithNext(AudioItem **pItem);
        virtual void processCodecPlayPostProcessRunningWithNoNext(AudioItem **pItem);

        virtual bool processCodecCrossFade(AudioItem* item,const common::TimeStamp& currentT,bool& initF);
        virtual bool processCodecPreBuffer();

        virtual void processComplete();
        virtual void processMessages() = 0;

        virtual void postProcess(engine::RData *data);

        virtual void doTimer();

        virtual void emitOnReadyForNext();
        virtual void emitOnCrossfade();

        virtual bool pausePlayback(bool shutdown = false,bool signalFlag = true);

        virtual bool unpausePlayback(bool signalFlag = true);
        virtual bool unpausePlaybackCodecStateNoPlay();

        virtual bool unpausePlaybackCodecStateSingle();
        virtual bool unpausePlaybackCodecStateSingleTiming();
        virtual bool unpausePlaybackCodecStateSingleSeekToPauseTime();
        virtual bool unpausePlaybackCodecStateSingleBufferOnRemote(bool process);
        virtual void unpausePlaybackCodecStateSinglePlayState(bool process);
        virtual void unpausePlaybackCodecStateSinglePlayToCrossfadeState();
        virtual void unpausePlaybackCodecStateSingleSetPlayState(bool process);

        virtual bool unpausePlaybackCodecStateFinish();
        virtual bool unpausePlaybackProcess(bool signalFlag);
        virtual void unpausePlaybackProcessSetTimeAndState();
        virtual void unpausePlaybackProcessOpenAudio();
        virtual bool unpausePlaybackProcessRestartPlayback(bool signalFlag);

        virtual bool resetPlayback();
        virtual bool seekPlayback(const common::TimeStamp& t);

        virtual void emitOnPause();
        virtual void emitOnPlay();

        virtual common::TimeStamp currentPlayTime();
        virtual common::TimeStamp currentCallbackTime();
        virtual common::TimeStamp getRemoteTimeSync();

        virtual void setDeviceID(tint idIndex);
        virtual void doUpdateChannelMap(tint devId);
        virtual void doSetVolume(sample_t vol, bool isCallback);
        virtual void doSetCrossFade(const common::TimeStamp& t);

        virtual void calcNextCodecTime();
        virtual void calcCrossFadeTime();
        virtual void calcCrossFadeTimeSetLength();
        virtual void calcCrossFadeTimeAdjustToCodecLength();
        virtual common::TimeStamp getCodecTimeLengthAdjustedForEndPosition();

        virtual common::TimeStamp timeFromEndOfItemBeingPlayed(AudioItem *item);
        virtual common::TimeStamp timeFromEndOfItemBeingPlayedCallbackEndPart(AudioItem *item,bool endFlag,const common::TimeStamp& playT,bool& isEnd,common::TimeStamp& markT,AudioItem *targetItem);
        virtual common::TimeStamp timeFromEndOfItemBeingPlayedCallbackPart(AudioItem *item,int index,const common::TimeStamp& playT,bool& isEnd,common::TimeStamp& markT);
        virtual common::TimeStamp timeFromEndOfItemBeingPlayedItemTime(AudioItem *item,AudioItem *targetItem);
        virtual common::TimeStamp timeFromEndOfItemBeingPlayedDiff(const common::TimeStamp& a,const common::TimeStamp& b);
        virtual bool timeFromEndOfItemBeingPlayedHasData(AudioItem *item,AudioItem *targetItem);

        virtual void initCrossFadeWindow();
        virtual void crossFade(engine::RData *dataA,engine::RData *dataB,common::TimeStamp& tStart);
        virtual void resetNextCrossData(engine::RData *fromD,const common::TimeStamp& tEnd);

        virtual bool event(QEvent *e);
        virtual void logAudioEvent(const tchar *strR,AudioEvent *audioE);

        virtual void audioEventMain(AudioEvent *e);
        virtual void audioEventStopState(AudioEvent *e);
        virtual void audioEventPlayState(AudioEvent *e);
        virtual void audioEventPauseState(AudioEvent *e);
        virtual void audioEventNoCodecState(AudioEvent *e);
        virtual void audioEventPreBufferState(AudioEvent *e);
        virtual void audioEventCrossFadeState(AudioEvent *e);

        virtual bool initResampler(int iFreq,int oFreq);
        virtual void closeResampler();
        virtual void resetResampler();
        virtual void resetResampler(int iFreq,int oFreq);

        virtual bool decodeAndResample(engine::Codec *c,AudioItem *outputItem,bool& initF);
        virtual tint decodeAndResampleInterleaveOutputChannels(sample_t *out,tint dLen,tint rLen);
        virtual tint decodeAndResampleInterleaveOutputChannels(sample_t *out,sample_t **in,tint dLen,tint rLen);
        virtual bool decodeAndResampleSetCompletePartTiming(engine::AData& dData,engine::RData::Part& p,tint idx,tint dLen);
        virtual tint decodeAndResampleCalculateOutputLength();

        virtual void buildChannelMapArray();

        virtual bool openMergeCodec(const QString& fileName);
        virtual void closeMergeCodec();
        virtual bool mergeAudioWithCodec(engine::Codec *mCodec,AudioItem *oItem);
        virtual void mergeAudioStreams(AudioItem *oItem,AudioItem *mItem);

        virtual void resyncAudioOutputTimeToItem(AudioItem *item);

#if defined(DEBUG_LOG_AUDIOOUTPUT)
        volatile AODebugItem *m_debugList;

        void appendDebugLog(const AODebugItem& item);
        AODebugItem *getDebugLog();
        void printDebugLog();
#endif

        virtual void postAudioEvent(QEvent *e);

        virtual void audioDeviceChange();

        virtual common::TimeStamp getReferenceClockTime() const;
        virtual void incrementMutexCount();
        virtual tint getMutexCount() const;
        virtual void updateCurrentPlayTimeFromStreamTime(const IOTimeStamp& systemTime);
        virtual void setItemStateToCallbackAsApplicable(AudioItem *item);
        virtual tint remainingSamplesInBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex);
        virtual common::TimeStamp lengthOfTime(tint noSamples) const;
        virtual tint partBufferIndexForChannel(tint channelIndex) const;
        virtual void playbackOfNextTrackIsStarting(const engine::RData::Part& part,const IOTimeStamp& systemTime,tint sIndex);
        virtual tint numberOfSamplesInTime(common::TimeStamp& t) const;
        virtual tint numberOfSamplesInFixedTime(const common::TimeStamp& t) const;
        virtual common::TimeStamp timeForNumberOfSamples(tint numberOfSamples) const;
        virtual void syncAudioTimeToPartReferenceLatencyDelay(engine::RData::Part& part,const IOTimeStamp& systemTime,const common::TimeStamp& referenceTime);
        virtual void writeSilenceForSynchronizedLatencyDelay(AbstractAudioHardwareBuffer *pBuffer,engine::RData::Part& part,const common::TimeStamp& referenceTime,tint& outputSampleIndex);
        virtual void syncAudioToPartReferenceLatencyDelay(AbstractAudioHardwareBuffer *pBuffer,engine::RData::Part& part,const IOTimeStamp& systemTime,tint& outputSampleIndex);
        virtual void writeToAudioSilenceForGivenRange(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex,tint numberOfSamples);
        virtual void writeToAudioSilenceToEndOfBuffer(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex);
        virtual void writeToAudioSilenceForRemainder(AbstractAudioHardwareBuffer *pBuffer,tint fromIndex);
        virtual void writeToAudioOutputBuffer(AbstractAudioHardwareBuffer *pBuffer,engine::RData *data,tint partNumber,tint inputSampleIndex,tint outputSampleIndex,tint amount);
        virtual void writeToAudioOutputBufferSilence(AbstractAudioHardwareBuffer *pBuffer,tint bufferIndex,tint outChannelIndex,tint outputSampleIndex,tint amount);
        virtual tint writeToAudioProcessPart(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,int outputSampleIndex);

        virtual void writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,
                                                          const engine::RData *data,
                                                          tint partNumber,
                                                          tint inChannelIndex,
                                                          tint bufferIndex,
                                                          tint outChannelIndex,
                                                          tint inputSampleIndex,
                                                          tint outputSampleIndex,
                                                          tint amount) = 0;


        virtual tint partNumberFromAudioItem(AudioItem *item) const;
        virtual engine::RData::Part& partFromAudioItem(AudioItem *item) const;
        virtual tint offsetFromAudioItem(AudioItem *item) const;
        virtual void setOffsetAndPartToAudioItem(AudioItem *item,tint offset,tint partNumber) const;
        virtual AudioItem *audioItemCallbackIsDone(AudioItem *item,tint outputSampleIndex,bool& loop,bool& loopFlag);
        virtual tint writeToAudioSilenceUntilStartOfNextPart(AbstractAudioHardwareBuffer *pBuffer,const engine::RData::Part& part,tint outputSampleIndex);
        virtual void processDataForOutput(engine::RData *data);
        virtual AudioItem *writeToAudioFromItem(AbstractAudioHardwareBuffer *pBuffer,AudioItem *item,const IOTimeStamp& systemTime,tint& outputSampleIndex,bool& loop,bool& loopFlag);
        virtual void writeToAudio(AbstractAudioHardwareBuffer *pBuffer,const IOTimeStamp& systemTime);
        virtual void writeToAudioIOCallback(AbstractAudioHardwareBuffer *pBuffer,const IOTimeStamp& systemTime);
        virtual void writeToAudioPostProcess();

        virtual QSharedPointer<AOQueryDevice::Device> copyDeviceInformation(const AOQueryDevice::Device& iDevice) = 0;
        virtual QSharedPointer<AOQueryDevice::Device> getCurrentDevice();
        virtual FormatDescription getSourceDescription(tint noChannels);

        virtual bool isNextCodecSeamless();
        virtual void setCodecSampleFormatType(engine::Codec *codec, engine::RData *item);

        virtual bool isChannelGenerated(tint inChannelIdx) const;
        virtual bool isCenterChannelGenerated() const;
        virtual bool isLFEChannelGenerated() const;
        virtual void resetLFEChannel();

        virtual QSharedPointer<engine::FIRFilter> createLFEBandPassFilter(int frequency);

        virtual bool isChannelMapShared(tint deviceIdx) const;
        virtual void reloadChannelSettings();

        // Member variable setters and getters
        virtual States getState() const;
        virtual void setState(States s);
        virtual CodecState getCodecState() const;
        virtual void setCodecState(CodecState s);
        virtual const common::TimeStamp& getStartCodecSeekTime() const;
        virtual void setStartCodecSeekTime(const common::TimeStamp& t);
        virtual engine::Codec *getCodec();
        virtual void setCodec(engine::Codec *c);
        virtual const common::TimeStamp& getNextCodecSeekTime() const;
        virtual void setNextCodecSeekTime(const common::TimeStamp& t);
        virtual engine::Codec *getNextCodec();
        virtual void setNextCodec(engine::Codec *c);
        virtual tint getFrequency() const;
        virtual void setFrequency(tint f);
        virtual tint getNoInChannels() const;
        virtual void setNoInChannels(tint n);
        virtual const QString& getNextName() const;
        virtual void setNextName(const QString& n);
        virtual const common::TimeStamp& getCrossFadeTime() const;
        virtual void setCrossFadeTime(const common::TimeStamp& t);
        virtual const common::TimeStamp& getCodecCurrentTime() const;
        virtual void setCodecCurrentTime(const common::TimeStamp& t);
        virtual const common::TimeStamp& getProgFadeTime() const;
        virtual void setProgFadeTime(const common::TimeStamp& t);
        virtual const common::TimeStamp& getCodecTimeLength() const;
        virtual void setCodecTimeLength(const common::TimeStamp& t);
        virtual bool getCodecTimeLengthUpdate() const;
        virtual void setCodecTimeLengthUpdate(bool f);
        virtual tint getNextOutState() const;
        virtual void setNextOutState(tint s);

        virtual AudioItem *getCodecAudioItem();
        virtual void setCodecAudioItem(AudioItem *item);
        virtual AudioItem *getCallbackAudioItem();
        virtual void setCallbackAudioTime(AudioItem *item);
        virtual tint getAudioProcessType();
        virtual void setAudioProcessType(tint pType);
        virtual tint getTrackTimeState() const;
        virtual void setTrackTimeState(tint state);
        virtual bool getTrackTimeStateFlag() const;
        virtual void setTrackTimeStateFlag(bool flag);
        virtual common::TimeStamp getRefStartAudioTime() const;
        virtual void setRefStartAudioTime(const common::TimeStamp& t);

        virtual common::TimeStamp getNextCodecTime();
        virtual void setNextCodecTime(const common::TimeStamp& t);
        virtual bool getCrossFadeFlag();
        virtual void setCrossFadeFlag(bool flag);
        virtual AudioItem *getCrossFadeItem();
        virtual void setCrossFadeItem(AudioItem *item);
        virtual common::TimeStamp getFrameFadeTime();
        virtual void setFrameFadeTime(const common::TimeStamp& t);

        virtual bool getAudioStartFlag();
        virtual void setAudioStartFlag(bool f);

        virtual const tint& getNoOutChannels() const;
        virtual void setNoOutChannels(tint noChs);

        virtual const bool& getResampleFlag() const;
        virtual void setResampleFlag(bool flag);
        virtual const tfloat64& getResampleRatio() const;
        virtual void setResampleRatio(tfloat64 v);
        virtual AudioItem *getResampleItem();
        virtual void setResampleItem(AudioItem *item);
        virtual engine::Resample *getResampler(tint ch);
        virtual void setResampler(engine::Resample *resampler,tint ch);
        virtual const tint& getRInFrequency() const;
        virtual void setRInFrequency(tint freq);
        virtual const tint& getRUsedI() const;
        virtual void setRUsedI(tint usedI);
        virtual const tint& getRUsedO() const;
        virtual void setRUsedO(tint usedO);
        virtual const tint& getRSrcLen() const;
        virtual void setRSrcLen(tint len);
        virtual const tint& getRDstLen() const;
        virtual void setRDstLen(tint len);
        virtual const tint& getROutNo() const;
        virtual void setROutNo(tint no);
        virtual sample_t *getRIn(tint ch);
        virtual void setRIn(sample_t *in,tint ch);
        virtual sample_t *getROut(tint ch);
        virtual void setROut(sample_t *out,tint ch);
        virtual const tfloat64& getROutDrift() const;
        virtual void setROutDrift(tfloat64 drift);
        virtual const bool& getRCodecCompleteFlag() const;
        virtual void setRCodecCompleteFlag(bool flag);
        virtual QList<AOResampleInfo>& resampleList();
        virtual const QList<AOResampleInfo>& resampleListConst() const;

        virtual common::TimeStamp getPauseTime() const;
        virtual void setPauseTime(const common::TimeStamp& t);
        virtual bool getPauseAudioFlag() const;
        virtual void setPauseAudioFlag(bool f);

        virtual engine::Codec *getCompleteCodec();
        virtual void setCompleteCodec(engine::Codec *c);
        virtual const common::TimeStamp& getCurrentOutTime() const;
        virtual void setCurrentOutTime(const common::TimeStamp& v);
        virtual void setCurrentPlayTime(const common::TimeStamp& v);
        virtual void setCurrentCallbackTime(const common::TimeStamp& v);

        virtual const common::TimeStamp& getCodecTimePositionComplete() const;
        virtual void setCodecTimePositionComplete(const common::TimeStamp& t);
        virtual const common::TimeStamp& getNextCodecTimeLengthComplete() const;
        virtual void setNextCodecTimeLengthComplete(const common::TimeStamp& t);

        virtual tint getProgFadeState() const;
        virtual void setProgFadeState(tint state);
        virtual const common::TimeStamp& getCrossFadeTimeLen() const;
        virtual void setCrossFadeTimeLen(const common::TimeStamp& t);

        virtual const common::TimeStamp& getAudioStartClock() const;
        virtual void setAudioStartClock(const common::TimeStamp& t);
        virtual bool getStartNextTrackFlag() const;
        virtual void setStartNextTrackFlag(bool f);
        virtual const common::TimeStamp& getOutputLatencyTime() const;
        virtual void setOutputLatencyTime(const common::TimeStamp& t);
        virtual common::TimeStamp getStopTimeClock() const;
        virtual void setStopTimeClock(const common::TimeStamp& t);
        virtual bool getStopTimeFlag() const;
        virtual void setStopTimeFlag(bool f);

        virtual AOQueryDevice *getDeviceInfo();
        virtual void setDeviceIndex(int idx);

        virtual bool isLive() const;

        virtual bool canCallSlot(SlotType type);
        virtual bool canCallSlot(SlotType type,void *cData);
        virtual void slotComplete();

        virtual void doCodecInit(void *cPtr);
        virtual void doEventTimer();

        virtual int getNoChannelsMapped();

        virtual void emitOnVolumeChanged(tfloat64 vol);

    protected slots:

        void onTimer();

        // onCodecInit: There is corrisponding logic like startNextCodec in handling the transition to seamless audio.
        // Possible refactor to reduce this duplication of logic. The onCodecInit is used for remote based codec streams
        // and startNextCodec is for local codec streams.
        void onCodecInit(void *cPtr);

        void onEventTimer();

    signals:

        void onStart(const QString&);
        void onPlay();
        void onStop();
        void onPause();
        void onTime(quint64 t);
        void onBuffer(tfloat32 percent);
        void onReadyForNext();
        void onNoNext();
        void onCrossfade();

        // Emitted when the volume is changed by audio system or OS from callback
        void onVolumeChanged(tfloat64 vol);
};

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(AUDIOIO_EXPORT,AOFactory,AOBase)

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AudioThread : public QThread
{
    public:
        Q_OBJECT


        public:
            AudioThread(const QString& name,QObject *parent=0);
            virtual ~AudioThread();

            virtual bool ignite();

            QSharedPointer<AOBase> audio();

        protected:

            QString m_name;

            QMutex m_mutex;
            QWaitCondition m_condition;

            QSharedPointer<AOBase> m_audioOutput;

            virtual void run();
};


//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AudioItem
{
    public:

        typedef enum
        {
            e_stateEmpty = 0,
            e_stateCodec,
            e_stateFull,
            e_stateFullEnd,
            e_stateCallback,
            e_stateCallbackEnd,
            e_stateDone
        } ItemStates;

    public:
        AudioItem();
        AudioItem(AOBase *ao,tint len,tint inChannel,tint outChannel);
        virtual ~AudioItem();

        virtual AudioItem *prev();
        virtual const AudioItem *prevConst() const;
        virtual void setPrev(AudioItem *item);

        virtual AudioItem *next();
        virtual const AudioItem *nextConst() const;
        virtual void setNext(AudioItem *item);

        virtual AudioItem::ItemStates state() const;
        virtual void setState(AudioItem::ItemStates v);

        virtual tint done() const;
        virtual void setDone(tint v);

        virtual engine::AData *data();
        virtual const engine::AData *dataConst() const;
        virtual void setData(engine::AData *d);

    protected:

        AOBase *m_audioOutput;
        AudioItem *m_prev;
        AudioItem *m_next;
        tint m_done;
        ItemStates m_state;
        engine::AData *m_data;
};

//-------------------------------------------------------------------------------------------

class AudioEvent : public QEvent
{
    public:
        typedef enum
        {
            e_startPlaybackEvent = QEvent::User + 1000,
            e_stopPlaybackEvent,
            e_playPlaybackEvent,
            e_pausePlaybackEvent,
            e_seekPlaybackEvent,
            e_setVolumeEvent,
            e_setDeviceID,
            e_updateChannelMap,
            e_nextPlaybackEvent,
            e_nextCrossPlaybackEvent,
            e_crossFadeEvent,
            e_audioDeviceChangeEvent,
            e_resetPlaybackEvent
        } AudioEventType;

    public:
        AudioEvent(AudioEventType t);

        static AudioEvent *clone(AudioEvent *pEvent);

        const QString& uri() const;
        QString& uri();

        const tint& device() const;
        tint& device();

        const common::TimeStamp& time() const;
        common::TimeStamp& time();

        const common::TimeStamp& timeLength() const;
        common::TimeStamp& timeLength();

        const sample_t& volume() const;
        sample_t& volume();

        const bool& exclusive() const;
        bool& exclusive();

        const bool& isCallback() const;
        bool& isCallback();

    protected:

        tint m_device;
        QString m_url;
        common::TimeStamp m_time;
        sample_t m_volume;
        common::TimeStamp m_timeLength;
        bool m_exclusive;
        bool m_isCallback;
};

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AOResampleInfo::start()
{
    return m_start;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AOResampleInfo::start() const
{
    return m_start;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AOResampleInfo::end()
{
    return m_end;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AOResampleInfo::end() const
{
    return m_end;
}

//-------------------------------------------------------------------------------------------

inline bool& AOResampleInfo::complete()
{
    return m_complete;
}

//-------------------------------------------------------------------------------------------

inline const bool& AOResampleInfo::complete() const
{
    return m_complete;
}

//-------------------------------------------------------------------------------------------
#if defined(DEBUG_LOG_AUDIOOUTPUT)
//-------------------------------------------------------------------------------------------

class AODebugItem
{
    public:

        typedef enum
        {
            e_clockSkew,
            e_referenceTimeStart,
            e_referenceFill,
            e_partOutputA,
            e_partOutputB,
            e_partOutputC,
            e_partOutputD,
            e_silenceOutputA,
            e_silenceOutputB
        } Type;

    public:
        AODebugItem(Type t);
        AODebugItem(const AODebugItem& rhs);

        const AODebugItem& operator = (const AODebugItem& rhs);

        bool isMarked() const;
        void mark();
        AODebugItem *next();
        void setNext(AODebugItem *nItem);

        Type& type();
        const Type& type() const;

        common::TimeStamp& nowClock();
        const common::TimeStamp& nowClock() const;
        common::TimeStamp& referenceClock();
        const common::TimeStamp& referenceClock() const;
        common::TimeStamp& audioClock();
        const common::TimeStamp& audioClock() const;

        common::TimeStamp& playTime();
        const common::TimeStamp& playTime() const;
        common::TimeStamp& outTime();
        const common::TimeStamp& outTime() const;
        common::TimeStamp& audioStartClock();
        const common::TimeStamp& audioStartClock() const;

        common::TimeStamp& timeA();
        const common::TimeStamp& timeA() const;
        common::TimeStamp& timeB();
        const common::TimeStamp& timeB() const;
        common::TimeStamp& timeC();
        const common::TimeStamp& timeC() const;

        tint& amount();
        const tint& amount() const;
        tint& offset();
        const tint& offset() const;
        tint& total();
        const tint& total() const;

    protected:

        AODebugItem *m_next;
        bool m_marker;

        Type m_type;
        common::TimeStamp m_nowClock;
        common::TimeStamp m_referenceClock;
        common::TimeStamp m_audioClock;
        common::TimeStamp m_playTime;
        common::TimeStamp m_outTime;
        common::TimeStamp m_audioStartClock;
        common::TimeStamp m_timeA;
        common::TimeStamp m_timeB;
        common::TimeStamp m_timeC;
        tint m_amount;
        tint m_offset;
        tint m_total;

        void copy(const AODebugItem& rhs);
};

//-------------------------------------------------------------------------------------------

inline bool AODebugItem::isMarked() const
{
    return m_marker;
}

//-------------------------------------------------------------------------------------------

inline void AODebugItem::mark()
{
    m_marker = true;
}

//-------------------------------------------------------------------------------------------

inline AODebugItem *AODebugItem::next()
{
    return m_next;
}

//-------------------------------------------------------------------------------------------

inline void AODebugItem::setNext(AODebugItem *nItem)
{
    m_next = nItem;
}

//-------------------------------------------------------------------------------------------

inline AODebugItem::Type& AODebugItem::type()
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

inline const AODebugItem::Type& AODebugItem::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AODebugItem::nowClock()
{
    return m_nowClock;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AODebugItem::nowClock() const
{
    return m_nowClock;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AODebugItem::referenceClock()
{
    return m_referenceClock;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AODebugItem::referenceClock() const
{
    return m_referenceClock;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AODebugItem::audioClock()
{
    return m_audioClock;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AODebugItem::audioClock() const
{
    return m_audioClock;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AODebugItem::playTime()
{
    return m_playTime;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AODebugItem::playTime() const
{
    return m_playTime;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AODebugItem::outTime()
{
    return m_outTime;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AODebugItem::outTime() const
{
    return m_outTime;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AODebugItem::audioStartClock()
{
    return m_audioStartClock;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AODebugItem::audioStartClock() const
{
    return m_audioStartClock;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AODebugItem::timeA()
{
    return m_timeA;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AODebugItem::timeA() const
{
    return m_timeA;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AODebugItem::timeB()
{
    return m_timeB;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AODebugItem::timeB() const
{
    return m_timeB;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& AODebugItem::timeC()
{
    return m_timeC;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& AODebugItem::timeC() const
{
    return m_timeC;
}

//-------------------------------------------------------------------------------------------

inline tint& AODebugItem::amount()
{
    return m_amount;
}

//-------------------------------------------------------------------------------------------

inline const tint& AODebugItem::amount() const
{
    return m_amount;
}

//-------------------------------------------------------------------------------------------

inline tint& AODebugItem::offset()
{
    return m_offset;
}

//-------------------------------------------------------------------------------------------

inline const tint& AODebugItem::offset() const
{
    return m_offset;
}

//-------------------------------------------------------------------------------------------

inline tint& AODebugItem::total()
{
    return m_total;
}

//-------------------------------------------------------------------------------------------

inline const tint& AODebugItem::total() const
{
    return m_total;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

