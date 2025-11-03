//-------------------------------------------------------------------------------------------
#if defined(OMEGA_IOS)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYCOREAUDIOIOS_H
#define __OMEGA_AUDIOIO_AOQUERYCOREAUDIOIOS_H
//-------------------------------------------------------------------------------------------

#import <AudioToolbox/AudioToolbox.h>
#import <AudioUnit/AudioUnit.h>
#import <CoreAudio/CoreAudioTypes.h>

#include "audioio/inc/AOQueryDevice.h"
#include "audioio/inc/FormatDescription.h"
#include "audioio/inc/FormatsSupported.h"
#include "audioio/inc/FormatDescriptionUtils.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

typedef enum
{
    e_audioOutputFloat = 1,
    e_audioOutputInt16,
    e_audioOutputInt24,
    e_audioOutputInt32,
    e_audioOutputUnknown
} AOIOSAudioFormats;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryCoreAudioIOS : public AOQueryDevice
{
    public:
        class IOSDevice;

    public:
        AOQueryCoreAudioIOS();
        virtual ~AOQueryCoreAudioIOS();

        virtual bool queryNames();
        virtual bool queryDevice(int idx);

        virtual int defaultDeviceIndex();

        static int currentFrequency();
        static bool setFrequency(int frequency);
        static QString idCurrentRoute();

        virtual void rebuild();

    protected:

        int m_bytesPerSample;

        virtual void printError(const tchar *strR, const tchar *strE) const;
        virtual void printError(const tchar *strR, const tchar *strE, OSStatus err) const;

        static OSStatus iosOmegaAudioCallbackIOProcQuery(void *inRegCon, \
                                  AudioUnitRenderActionFlags *ioActionFlags, \
                                  const AudioTimeStamp *inTimeStamp, \
                                  UInt32 inBusNumber, \
                                  UInt32 inNumberFrames, \
                                  AudioBufferList *ioData);

        virtual QString bitFormatName(int bitFormat) const;

        virtual bool buildDeviceMap();
        virtual bool getDescription(AOIOSAudioFormats bitFormat, int noChannels, int freq, AudioStreamBasicDescription *fmt);
        virtual bool queryDeviceCapability(AOIOSAudioFormats bitFormat, int noChannels, int freq);

        virtual void queryFrequencies(IOSDevice *dev);
        virtual bool queryCurrentRoute(IOSDevice *dev);

        static int getCurrentSampleRate();
        static bool setPrefSessionFrequency(int frequency);
        static int waitProcessForNewFrequency();
        static int waitForNewFrequency(int oldFreq);
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryCoreAudioIOS::IOSDevice : public AOQueryDevice::Device
{
    public:
        IOSDevice();
        IOSDevice(const AOQueryDevice::Device& rhs);
        virtual ~IOSDevice();

        virtual const FormatsSupported& formats() const;
        virtual void addSupportedFormat(const FormatDescription& desc);

    protected:
        FormatsSupported m_formats;

        virtual void copy(const AOQueryDevice::Device& rhs);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
