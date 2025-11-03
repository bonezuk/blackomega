//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_INTEGRATION_TEST_ALSATEST_H
#define __OMEGA_INTEGRATION_TEST_ALSATEST_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QCoreApplication>
#include <QTimer>
#include <QVector>
#include <QMutex>
#include <QWaitCondition>

#include "audioio/inc/AOQueryALSA.h"
#include "audioio/inc/FormatDescription.h"
#include "audioio/inc/FormatDescriptionUtils.h"
#include "audioio/inc/SampleConverter.h"
#include "engine/inc/FormatTypeFromFloat.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class ALSAPlaybackTester : public QCoreApplication
{
    public:
        Q_OBJECT

    public:
        ALSAPlaybackTester(int& argc,char **argv);
        virtual ~ALSAPlaybackTester();

    protected:

        snd_pcm_t *m_handle;
        snd_pcm_hw_params_t *m_hwParams;
        snd_pcm_sw_params_t *m_swParams;

        tint m_frequency;

        snd_pcm_uframes_t m_noSamplesInPeriod;
        snd_pcm_uframes_t m_noSamplesInBuffer;

        tfloat64 m_phase;
        tint m_frequencySignal;

        tint16 *m_audioWriteBuffer;

        snd_async_handler_t *m_pCallback;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printErrorOS(const tchar *strR,const tchar *strE,int errAlsa) const;

        virtual bool openAudio();
        virtual void closeAudio();

        virtual bool setupHardwareParameters();
        virtual bool setupSoftwareParameters();
        virtual bool setBufferLength();
        virtual bool queryBufferSize();

        static void writeAudio(snd_async_handler_t *pCallback);
        virtual void writeAudioImpl(snd_async_handler_t *pCallback);
        virtual void generateAndWriteAudio(snd_pcm_t *handle,tint len);

        virtual bool startAudio();
        virtual void stopAudio();

    protected slots:
        void onStart();
        void onStop();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
