//-------------------------------------------------------------------------------------------
#if defined(OMEGA_IOS)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOCOREAUDIOIOS_H
#define __OMEGA_AUDIOIO_AOCOREAUDIOIOS_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOCoreAudio.h"
#include "audioio/inc/AOQueryCoreAudioIOS.h"
#include "audioio/inc/AudioHardwareBufferCoreAudioIOS.h"
#include "audioio/inc/SampleConverter.h"

#include <mach/mach_time.h>
#include <mach/kern_return.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOCoreAudioIOS : public AOCoreAudio
{
    public:
        Q_OBJECT

    public:
        AOCoreAudioIOS(QObject *parent = 0);
        virtual ~AOCoreAudioIOS();

        virtual bool isUpdateRequired();
        virtual void updateCurrentDevice();

    protected:

        AudioComponentInstance m_audioOutputUnit;
        QSharedPointer<SampleConverter> m_pSampleConverter;
        bool m_flagInit;
        bool m_flagStart;
        struct mach_timebase_info m_machTimeInfo;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR, const tchar *strE, OSStatus err) const;

        virtual engine::AData *allocateData(tint len,tint inChannel,tint outChannel);

        virtual bool openAudio();
        virtual void closeAudio();
        virtual bool startAudioDevice();
        virtual void stopAudioDevice();
        virtual bool isAudio() const;

        virtual void processMessages();

        virtual void addToPriorityMap(QMap<int, QList<int> >& rMap, int priority, int rate);
        virtual void setPlaybackFrequency(QSharedPointer<AOQueryCoreAudioIOS::IOSDevice> pDevice);

        virtual void writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,
                                                          const engine::RData *data,
                                                          tint partNumber,
                                                          tint inChannelIndex,
                                                          tint bufferIndex,
                                                          tint outChannelIndex,
                                                          tint inputSampleIndex,
                                                          tint outputSampleIndex,
                                                          tint amount);

        virtual QSharedPointer<AOQueryDevice::Device> copyDeviceInformation(const AOQueryDevice::Device& iDevice);
        virtual QSharedPointer<AOQueryCoreAudioIOS::IOSDevice> getCurrentCoreAudioIOSDevice();

        virtual bool getStreamDescription(const FormatDescription& desc, AudioStreamBasicDescription *fmt);
        virtual bool isConvertionAlignedHigh(const AudioStreamBasicDescription& format) const;
        virtual QSharedPointer<SampleConverter> sampleConverterFromDescription(const AudioStreamBasicDescription& format);

        static OSStatus iosAudioRender(void *inRegCon, \
                                  AudioUnitRenderActionFlags *ioActionFlags, \
                                  const AudioTimeStamp *inTimeStamp, \
                                  UInt32 inBusNumber, \
                                  UInt32 inNumberFrames, \
                                  AudioBufferList *ioData);

        virtual OSStatus iosAudioRenderImpl(AudioUnitRenderActionFlags *ioActionFlags, \
                                  const AudioTimeStamp *inTimeStamp, \
                                  UInt32 inBusNumber, \
                                  UInt32 inNumberFrames, \
                                  AudioBufferList *ioData);

        virtual IOTimeStamp createIOTimeStamp(const AudioTimeStamp *sysTime) const;

    protected slots:
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
