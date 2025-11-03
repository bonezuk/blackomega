//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_INTEGRATION_TEST_AOCOREHALINTEGER_H
#define __OMEGA_INTEGRATION_TEST_AOCOREHALINTEGER_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QCoreApplication>
#include <QTimer>
#include <QVector>
#include <QMutex>
#include <QWaitCondition>

#include "audioio/inc/AOCoreAudioMacOS.h"
#include "audioio/inc/AOQueryCoreAudio.h"
#include "audioio/inc/FormatDescription.h"
#include "audioio/inc/FormatDescriptionUtils.h"
#include "audioio/inc/SampleConverter.h"
#include "engine/inc/FormatTypeFromFloat.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class HALSignalGenerator : public QCoreApplication
{
    public:
        Q_OBJECT

    public:
        HALSignalGenerator(tint deviceIdx,int& argc,char **argv);
        virtual ~HALSignalGenerator();

        void runToneSignal();
        void runIntegerToneSignal();
        void queryDeviceFormat();
        void printFormatsAvailableToDevice();
        void printFormatsAvailableToAllDevices();

        static OSStatus IOProc(AudioDeviceID ioDevice,
                               const AudioTimeStamp *inNow,
                               const AudioBufferList *inInputData,
                               const AudioTimeStamp *inInputTime,
                               AudioBufferList *outOutputData,
                               const AudioTimeStamp *inOutputTime,
                               void *inClientData);

        static OSStatus IOProcInteger(AudioDeviceID ioDevice,
                                     const AudioTimeStamp *inNow,
                                     const AudioBufferList *inInputData,
                                     const AudioTimeStamp *inInputTime,
                                     AudioBufferList *outOutputData,
                                     const AudioTimeStamp *inOutputTime,
                                     void *inClientData);
    protected:

        tint m_deviceIndex;
        audioio::AOQueryCoreAudio *m_deviceInfo;

        bool m_exclusive;

        tint m_noChannels;

        tfloat64 m_frequencyPlayback;
        tfloat64 m_frequencySignal;
        tfloat64 m_phase;

        bool m_flagStart;
        AudioDeviceIOProcID m_deviceIOProcID;

        SampleConverter *m_converter;
        QMutex m_mutex;
        QWaitCondition m_condition;

        bool m_enableMixing;

        void printError(const tchar *strR,const tchar *strE) const;
        void printError(const tchar *strR,const tchar *strE,OSStatus err) const;
        void log(const tchar *strR);
        void log(const QString& strR);

        bool openAudio();
        void closeAudio();

        void deallocate();
        bool queryAudioDevices();
        AudioDeviceID getDeviceID();
        QString getDeviceName();
        bool useExclusiveModeIfAvailable(AudioDeviceID devId);
        bool setSampleRate(AudioDeviceID devId,int sampleRate);
        bool setAudioStreamFormat(AudioDeviceID devId,int noChannels,int sampleRate);
        void stopAudioDevice();
        bool isDeviceRunning(AudioDeviceID devId);
        void exclusiveModeRelease(AudioDeviceID devId);

        void queryDeviceStreamFormat(AudioDeviceID devId);

        OSStatus IOProcImpl(AudioDeviceID ioDevice,
                            const AudioTimeStamp *inNow,
                            const AudioBufferList *inInputData,
                            const AudioTimeStamp *inInputTime,
                            AudioBufferList *outOutputData,
                            const AudioTimeStamp *inOutputTime);


        QString formatIDToString(UInt32 formatID);
        QString formatFlagsToString(UInt32 flags);
        void printFormat(AudioStreamBasicDescription *format);
        QVector<AudioStreamID> getAudioStreamsForDevice(AudioDeviceID devID);
        QVector<AudioStreamRangedDescription> getAudioStreamDescriptions(AudioStreamID streamID);

        QPair<AudioStreamID,AudioStreamBasicDescription *> findClosestStream(const FormatDescription& sourceDesc,const QVector<AudioStreamID>& streamIDList);
        bool generateFormatDescription(const AudioStreamBasicDescription& pFormat,FormatDescription& desc);
        bool isFormatDataTypeCorrisponding(const AudioStreamBasicDescription& format,const FormatDescription& desc);
        bool isDescriptionsEqual(const AudioStreamBasicDescription& format,const FormatDescription& desc);
        bool openAudioStream();
        void closeAudioStream();
        FormatDescription getSourceDescription();
        bool getDescription(const AudioStreamBasicDescription& format,FormatDescription& desc);
        SampleConverter getSampleConverterFromDescription(const AudioStreamBasicDescription& format);
        bool audioRunLoopSetup();

        static OSStatus audioStreamChangeListener(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddress[],void *inClientData);
        OSStatus audioStreamChangeListenerImpl(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddress[]);
        bool setAudioStream(AudioStreamID streamID,AudioStreamBasicDescription format);

        bool disableMixingIfPossible(AudioDeviceID devId);
        void reenableMixing(AudioDeviceID devId);

        OSStatus IntegerIOProcImpl(AudioDeviceID ioDevice,
                                   const AudioTimeStamp *inNow,
                                   const AudioBufferList *inInputData,
                                   const AudioTimeStamp *inInputTime,
                                   AudioBufferList *outOutputData,
                                   const AudioTimeStamp *inOutputTime);


    protected slots:

        void onSignalStart();
        void onSignalStop();
        void onIntegerSignalStart();
        void onIntegerSignalStop();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

