//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOCOREAUDIOMACOS_H
#define __OMEGA_AUDIOIO_AOCOREAUDIOMACOS_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOBase.h"
#include "audioio/inc/CoreAudioIF.h"

#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#include <CoreAudio/CoreAudio.h>

#include "audioio/inc/FormatsSupported.h"
#include "audioio/inc/FormatDescriptionUtils.h"
#include "audioio/inc/SampleConverter.h"
#include "audioio/inc/AOQueryCoreAudio.h"
#include "audioio/inc/AOCoreAudio.h"

#ifndef AudioFormatID
typedef UInt32 AudioFormatID;
#endif
#ifndef AudioFormatFlags
typedef UInt32 AudioFormatFlags;
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOCoreAudioMacOS : public AOCoreAudio
{
    public:
        Q_OBJECT

    public:
        AOCoreAudioMacOS(QObject *parent = 0);
        virtual ~AOCoreAudioMacOS();

        static void calcAudioLatency();

    protected:
        static volatile bool m_outputLatencyCalcFlag;
        static volatile common::TimeStamp m_outputLatencyTimeCore;
        static tint m_outputLatencyBufferSize;

        AudioComponentInstance m_outputUnit;
        AudioStreamBasicDescription m_streamFormat;
        AURenderCallbackStruct m_renderCallback;

        bool m_flagInit;
        bool m_flagStart;

        QMutex m_audioStreamMutex;
        QWaitCondition m_audioStreamCondition;

        tint m_originalSampleRate;

        bool m_isIntegerMode;
        bool m_hasExclusiveMode;
        bool m_hasMixingBeenDisabled;
        QSharedPointer<SampleConverter> m_pSampleConverter;
        AudioStreamID m_oldStreamID;
        AudioStreamBasicDescription *m_oldStreamDescription;
        AudioDeviceID m_integerDeviceID;
        AudioDeviceIOProcID *m_pIntegerDeviceIOProcID;

        bool m_isDeviceVolume;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printErrorOS(const tchar *strR,const tchar *strE,OSStatus err) const;

        virtual bool init();
        virtual void reset();

        virtual bool openAudio();
        virtual void closeAudio();
        virtual bool startAudioDevice();
        virtual void stopAudioDevice();
        virtual bool isAudio() const;

        virtual bool openAudioCoreAudio(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pDevice);

        virtual QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> getCurrentCoreAudioDevice();
        virtual void setSampleRateWhileOpeningCoreDevice(AudioDeviceID devID);
        virtual void resetSampleRateToOriginalWhenClosing(AudioDeviceID devID);
        virtual void setDeviceID(tint id);

        static OSStatus callbackIOProc(void *inRegCon, \
                                       AudioUnitRenderActionFlags *ioActionFlags, \
                                       const AudioTimeStamp *inTimeStamp, \
                                       UInt32 inBusNumber, \
                                       UInt32 inNumberFrames, \
                                       AudioBufferList *ioData);

        virtual OSStatus callbackIOProcI(AudioUnitRenderActionFlags *ioActionFlags, \
                                        const AudioTimeStamp *inTimeStamp, \
                                        UInt32 inBusNumber, \
                                        UInt32 inNumberFrames, \
                                        AudioBufferList *ioData);

        virtual engine::AData *allocateData(tint len,tint inChannel,tint outChannel);

        virtual void processMessages();

        static OSStatus callbackLatencyProc(AudioDeviceID id, \
                                       const AudioTimeStamp *inNow, \
                                       const AudioBufferList *inInputData, \
                                       const AudioTimeStamp *inInputTime, \
                                       AudioBufferList *outOutputData, \
                                       const AudioTimeStamp *inOutputTime, \
                                       void *inClientData);

        static OSStatus corePropertyChangeProc(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddresses[],void *inClientData);
        OSStatus corePropertyChangeProcImpl(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddresses[]);

        void addListenerDevices();
        void removeListenerDevices();
        void addListenerJackConnection(AudioDeviceID devID);
        void removeListenerJackConnection(AudioDeviceID devID);

        virtual void audioDeviceChange();

        virtual IOTimeStamp createIOTimeStamp(const AudioTimeStamp *sysTime) const;

        virtual void writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,
                                                          const engine::RData *data,
                                                          tint partNumber,
                                                          tint inChannelIndex,
                                                          tint bufferIndex,
                                                          tint outChannelIndex,
                                                          tint inputSampleIndex,
                                                          tint outputSampleIndex,
                                                          tint amount);

        virtual void writeToAudioFromFloat(const sample_t *input, tint iIdx, tfloat32 *output, tint oIdx, tint tAmount, tint noInputChannels, tint noOutputChannels);
        virtual void writeToAudioFromInt16(const sample_t *in, tint iIdx, tfloat32 *output, tint oIdx, tint tAmount, tint noInputChannels, tint noOutputChannels);
        virtual void writeToAudioFromInt24(const sample_t *in, tint iIdx, tfloat32 *output, tint oIdx, tint tAmount, tint noInputChannels, tint noOutputChannels);
        virtual void writeToAudioFromInt32(const sample_t *in, tint iIdx, tfloat32 *output, tint oIdx, tint tAmount, tint noInputChannels, tint noOutputChannels);

        virtual pid_t getCurrentProcessID() const;

        bool isExclusiveModeIfAvailable(AudioDeviceID devId);
        bool setExclusiveMode(AudioDeviceID devID, bool isExcl);

        virtual bool setupPropertyRunLoop();
        virtual bool disableMixingIfPossible(AudioDeviceID devID);
        virtual void reEnableMixing(AudioDeviceID devID);
        virtual QVector<AudioStreamID> getAudioStreamsForDevice(AudioDeviceID devID);
        virtual QVector<AudioStreamRangedDescription> getAudioStreamDescriptions(AudioStreamID streamID);
        virtual bool isFormatDataTypeCorrisponding(const AudioStreamBasicDescription& format,const FormatDescription& desc);
        virtual QVector<int> findFrequenciesFromRange(const AudioStreamRangedDescription& range,bool useRange);
        virtual QVector<FormatDescription> formatDescriptionsFromRanged(const AudioStreamRangedDescription& range,bool useRange);
        virtual bool areDescriptionsEquivalent(const AudioStreamBasicDescription& format,const FormatDescription& desc);
        virtual FormatsSupported getSupportedFormatsForStreams(const QVector<AudioStreamID>& streamIDs);
        virtual QPair<AudioStreamID,AudioStreamBasicDescription *> findClosestStream(const FormatDescription& sourceDesc,const QVector<AudioStreamID>& streamIDs);
        virtual bool isConvertionAlignedHigh(const AudioStreamBasicDescription& format) const;
        virtual QSharedPointer<SampleConverter> sampleConverterFromDescription(const AudioStreamBasicDescription& format);
        virtual bool formatFromStreamDescription(const AudioStreamBasicDescription& format,FormatDescription& desc);
        virtual tint audioStreamWaitTimeout() const;
        static OSStatus audioStreamChangeListener(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddress[],void *inClientData);
        virtual OSStatus audioStreamChangeListenerImpl(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddress[]);
        virtual bool setAudioStream(AudioStreamID streamID,const AudioStreamBasicDescription& format);
        virtual AudioStreamBasicDescription *saveAudioDescriptionForStream(AudioStreamID streamID);
        virtual tint setSampleRateIfPossible(AudioDeviceID devID,int sampleRate);
        virtual bool isDeviceAlive(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pDevice);

        virtual QSharedPointer<AOQueryDevice::Device> copyDeviceInformation(const AOQueryDevice::Device& iDevice);

        virtual bool openIntegerAudio(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pDevice);
        virtual void closeIntegerAudio();
        virtual bool startIntegerAudio();
        virtual void stopIntegerAudio();

        static OSStatus IOProcInteger(AudioDeviceID ioDevice,
                                        const AudioTimeStamp *inNow,
                                      const AudioBufferList *inInputData,
                                      const AudioTimeStamp *inInputTime,
                                      AudioBufferList *outOutputData,
                                      const AudioTimeStamp *inOutputTime,
                                      void *inClientData);

        virtual OSStatus IntegerIOProcImpl(AudioDeviceID ioDevice,
                                           const AudioTimeStamp *inNow,
                                           const AudioBufferList *inInputData,
                                           const AudioTimeStamp *inInputTime,
                                           AudioBufferList *outOutputData,
                                           const AudioTimeStamp *inOutputTime);

        virtual bool canDeviceSupportIntegerMode(AudioDeviceID devID);
        virtual void updateExclusiveAndIntegerModeOnDevices();

        virtual void setCodecSampleFormatType(engine::Codec *codec, engine::RData *item);

        virtual bool isDeviceVolume();
        virtual bool isDeviceVolumeSettable();
        virtual bool isDeviceMuted();
        virtual void setDeviceMuted(bool isMute);
        virtual sample_t getDeviceVolume();
        virtual bool setDeviceVolume(sample_t vol);

        virtual void doSetVolume(sample_t vol, bool isCallback);

        static OSStatus volumePropertyChangeProc(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddresses[],void *inClientData);
        OSStatus volumeChangeProcImpl(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddresses[]);
        void addVolumeChangeNotification(AudioDeviceID devID);
        void removeVolumeChangeNotification(AudioDeviceID devID);

        void printAudioStreamRangedDescription(AudioStreamRangedDescription desc) const;
        QString formatIDString(AudioFormatID formatID) const;
        QString formatFlagString(AudioFormatFlags flag) const;

    protected Q_SLOTS:
        void onStopProcess();
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
