//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOLINUXALSA_H
#define __OMEGA_AUDIOIO_AOLINUXALSA_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOBase.h"
#include "audioio/inc/AOQueryALSA.h"
#include "audioio/inc/AudioHardwareBufferALSA.h"
#include "audioio/inc/SampleConverter.h"
#include "audioio/inc/FormatDescriptionUtils.h"

#include <QQueue>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOLinuxALSA : public AOBase
{
    public:
        Q_OBJECT

    public:
        AOLinuxALSA(QObject *parent = 0);
        virtual ~AOLinuxALSA();

    protected:
        snd_pcm_t *m_handleALSA;
        snd_pcm_hw_params_t *m_hwParamsALSA;
        snd_pcm_sw_params_t *m_swParamsALSA;
        tint m_formatTypeALSA;
        snd_pcm_uframes_t m_noSamplesInPeriodALSA;
        snd_pcm_uframes_t m_noSamplesInBufferALSA;
        snd_async_handler_t *m_pCallback;

        SampleConverter *m_pSampleConverter;

        bool m_flagInit;
        bool m_flagStart;

        QQueue<tbyte *> m_playbackALSAMemoryBuffers;
        tint m_playbackALSAMemoryBufferSize;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printErrorOS(const tchar *strR,const tchar *strE,tint errCode) const;

        virtual bool init();

        virtual engine::AData *allocateData(tint len,tint inChannel,tint outChannel);

        virtual bool openAudio();
        virtual void closeAudio();
        virtual bool startAudioDevice();
        virtual void stopAudioDevice();
        virtual bool isAudio() const;

        virtual QSharedPointer<AOQueryALSA::DeviceALSA> getCurrentALSAAudioDevice();
        virtual int formatFromDescription(snd_pcm_t *handle,QSharedPointer<AOQueryALSA::DeviceALSA>& pDeviceALSA,const FormatDescription& desc);
        virtual bool setBufferLength();
        virtual bool queryBufferSize();
        virtual void setResamplerAsRequired(tint codecFrequency,tint deviceFrequency);

        virtual bool setupHardwareParameters(int fType,const FormatDescription& desc);
        virtual bool setupSoftwareParameters();

        virtual SampleConverter *getSampleConverter();

        virtual SampleConverter *createSampleConverter(tint formatType);

        static void writeAudioALSA(snd_async_handler_t *pCallback);
        virtual void writeAudioALSAImpl(snd_async_handler_t *pCallback);
        virtual void writeAudioToALSA(snd_pcm_t *handle,tint noFrames);
        virtual IOTimeStamp createIOTimeStamp(snd_pcm_t *handle) const;

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

        virtual void processMessages();
        virtual void processMessagesForStop();
        virtual void processMessagesForStopSetTimer(tint delay);

        virtual bool getFlagInit() const;
        virtual void setFlagInit(bool v);
        virtual bool getFlagStart() const;
        virtual void setFlagStart(bool v);

        virtual void allocALSAPlaybackBuffers(tint formatType, tint noChannels);
        virtual void freeALSAPlaybackBuffers();

        virtual void setCodecSampleFormatType(engine::Codec *codec, engine::RData *item);

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
