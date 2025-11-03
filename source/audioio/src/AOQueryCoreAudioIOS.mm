//-------------------------------------------------------------------------------------------
#if defined(OMEGA_IOS)
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryCoreAudioIOS.h"
#include "audioio/inc/AOCoreAudioSessionIOS.h"

#import <Foundation/Foundation.h>
#import <AVFAudio/AVAudioSession.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------
// AOQueryCoreAudioIOS::IOSDevice
//-------------------------------------------------------------------------------------------

AOQueryCoreAudioIOS::IOSDevice::IOSDevice() : AOQueryDevice::Device(AOQueryDevice::Device::e_deviceIOS),
    m_formats()
{}

//-------------------------------------------------------------------------------------------

AOQueryCoreAudioIOS::IOSDevice::IOSDevice(const AOQueryDevice::Device& rhs) : AOQueryDevice::Device(AOQueryDevice::Device::e_deviceIOS),
    m_formats()
{
    AOQueryCoreAudioIOS::IOSDevice::copy(rhs);
}

//-------------------------------------------------------------------------------------------

AOQueryCoreAudioIOS::IOSDevice::~IOSDevice()
{}

//-------------------------------------------------------------------------------------------

const FormatsSupported& AOQueryCoreAudioIOS::IOSDevice::formats() const
{
    return m_formats;
}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudioIOS::IOSDevice::addSupportedFormat(const FormatDescription& desc)
{
    m_formats.add(desc);
}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudioIOS::IOSDevice::copy(const AOQueryDevice::Device& rhs)
{
    const IOSDevice& dev = dynamic_cast<const IOSDevice &>(rhs);
    AOQueryDevice::Device::copy(rhs);
    m_formats = dev.m_formats;
}

//-------------------------------------------------------------------------------------------
// AOQueryCoreAudioIOS
//-------------------------------------------------------------------------------------------

AOQueryCoreAudioIOS::AOQueryCoreAudioIOS() : AOQueryDevice(),
    m_bytesPerSample(0)
{}

//-------------------------------------------------------------------------------------------

AOQueryCoreAudioIOS::~AOQueryCoreAudioIOS()
{}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudioIOS::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "AOQueryCoreAudioIOS::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudioIOS::printError(const tchar *strR, const tchar *strE, OSStatus err) const
{
    NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
    QString desc = QString::fromUtf8(error.localizedDescription.UTF8String);
    QString reason = QString::fromUtf8(error.localizedFailureReason.UTF8String);
    QString recovery = QString::fromUtf8(error.localizedRecoverySuggestion.UTF8String);
    QString eStr;
    eStr  = strE;
    eStr += common::c_endl;
    eStr += "\tDescription - " + desc;
    eStr += common::c_endl;
    eStr += "\tReason - " + reason;
    eStr += common::c_endl;
    eStr += "\tSuggestion - " + recovery + common::c_endl;
    common::Log::g_Log << "AOQueryCoreAudio::" << strR << " - " << eStr.toUtf8().constData();
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudioIOS::queryNames()
{
    return buildDeviceMap();
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudioIOS::queryDevice(int idx)
{
    bool res = false;

    if(!idx && buildDeviceMap())
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

int AOQueryCoreAudioIOS::defaultDeviceIndex()
{
    return 0;
}

//-------------------------------------------------------------------------------------------

QString AOQueryCoreAudioIOS::bitFormatName(int bitFormat) const
{
    QString n;
    switch(static_cast<AOIOSAudioFormats>(bitFormat))
    {
        case e_audioOutputInt16:
            n = "Int16";
            break;
        case e_audioOutputInt24:
            n = "Int24";
            break;
        case e_audioOutputInt32:
            n = "Int32";
            break;
        case e_audioOutputFloat:
            n = "Float32";
            break;
        case e_audioOutputUnknown:
        default:
            n = "Unknown";
            break;
    }
    return n;
}

//-------------------------------------------------------------------------------------------

int AOQueryCoreAudioIOS::currentFrequency()
{
    AVAudioSession *aSession = [AVAudioSession sharedInstance];
    return static_cast<int>(aSession.sampleRate);
}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudioIOS::queryFrequencies(IOSDevice *dev)
{
    int originalFrequency = currentFrequency();
    QSet<int> freqs = FormatDescription::setOfFrequencies();
    for(QSet<int>::iterator ppI = freqs.begin(); ppI != freqs.end(); ppI++)
    {
        int freq = *ppI;
        if(setFrequency(freq))
        {
            common::Log::g_Log.print("device '%s' does support %dHz\n", dev->name().toUtf8().constData(), freq);
            dev->addFrequency(freq);
        }
        else
        {
            common::Log::g_Log.print("device '%s' does NOT support %dHz\n", dev->name().toUtf8().constData(), freq);
        }
    }
    setFrequency(originalFrequency);
}

//-------------------------------------------------------------------------------------------

int AOQueryCoreAudioIOS::getCurrentSampleRate()
{
    AVAudioSession *aSession = [AVAudioSession sharedInstance];
    return static_cast<int>(aSession.sampleRate);
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudioIOS::setPrefSessionFrequency(int frequency)
{
    double freqD = static_cast<double>(frequency);
    NSError *audioSessionError = nil;
    AVAudioSession *aSession = [AVAudioSession sharedInstance];
    bool res = false;

    res = [aSession setActive:NO error:&audioSessionError];
    if(res == YES)
    {
        res = [aSession setPreferredSampleRate:freqD error:&audioSessionError];
        if(res == YES)
        {
            res = [aSession setActive:YES error:&audioSessionError];
            if(res == YES)
            {
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

int AOQueryCoreAudioIOS::waitProcessForNewFrequency()
{
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    QThread::msleep(100);
    return getCurrentSampleRate();
}

//-------------------------------------------------------------------------------------------

int AOQueryCoreAudioIOS::waitForNewFrequency(int oldFreq)
{
    const int c_timeoutMilliSeconds = 500;
    int i, newFreq;

    for(i = 0; i < c_timeoutMilliSeconds; i += 100)
    {
        newFreq = waitProcessForNewFrequency();
        if(newFreq != oldFreq)
        {
            return newFreq;
        }
    }
    return oldFreq;
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudioIOS::setFrequency(int frequency)
{
    bool res = false;
    int actualFreq, originalFrequency = getCurrentSampleRate();

    if(frequency == originalFrequency)
    {
        return true;
    }
    if(setPrefSessionFrequency(frequency))
    {
        actualFreq = waitForNewFrequency(originalFrequency);
        NSLog(@"frequency=%d, originalFrequency=%d, actualFreq=%d\n", frequency, originalFrequency, actualFreq);
        res = (actualFreq == frequency) ? true : false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QString AOQueryCoreAudioIOS::idCurrentRoute()
{
    QString currentId;
    AVAudioSession *aSession = [AVAudioSession sharedInstance];
    if(aSession != nil)
    {
        AVAudioSessionRouteDescription *currentRoute = aSession.currentRoute;
        if(currentRoute != nil)
        {
            NSArray<AVAudioSessionPortDescription *> *outputs = currentRoute.outputs;
            if(outputs != nil && outputs.count > 0)
            {
                AVAudioSessionPortDescription *port = [outputs objectAtIndex:0];
                if(port != nil)
                {
                    QString type = QString::fromUtf8(port.portType.UTF8String);
                    QString name = QString::fromUtf8(port.portName.UTF8String);
                    currentId = name + ":" + type;
                }
            }
        }
    }
    return currentId;
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudioIOS::queryCurrentRoute(IOSDevice *dev)
{
    bool res = false;

    if(dev != 0)
    {
        QString currentId = idCurrentRoute();
        if(!currentId.isEmpty())
        {
            AVAudioSession *aSession = [AVAudioSession sharedInstance];
            if(aSession != nil)
            {
                AVAudioSessionRouteDescription *currentRoute = aSession.currentRoute;
                if(currentRoute != nil)
                {
                    NSArray<AVAudioSessionPortDescription *> *outputs = currentRoute.outputs;
                    if(outputs != nil && outputs.count > 0)
                    {
                        AVAudioSessionPortDescription *port = [outputs objectAtIndex:0];
                        if(port != nil)
                        {
                            dev->id() = currentId;
                            dev->name() = QString::fromUtf8(port.portName.UTF8String);

                            int noChannels = port.channels.count;
                            dev->setNoChannels(noChannels);
                            dev->loadChannelMap();

                            res = true;
                        }
                    }
                }
            }
        }
        else
        {
            printError("queryCurrentRoute", "No id for current audio route");
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudioIOS::buildDeviceMap()
{
    bool res = false;

    if(m_devices.isEmpty())
    {
        IOSDevice *dev = new IOSDevice();

        if(queryCurrentRoute(dev))
        {
            QSharedPointer<AOCoreAudioSessionIOS> pSession = AOCoreAudioSessionIOS::audioInstance();
            if(!pSession.isNull())
            {
                if(!pSession->loadDevice(*dev))
                {
                    queryFrequencies(dev);
                    if(pSession->saveDevice(*dev))
                    {
                        res = true;
                    }
                    else
                    {
                        printError("buildDeviceMap", "Failed to save audio device information");
                    }
                }
                else
                {
                    res = true;
                }
            }
            else
            {
                printError("buildDeviceMap", "No audio session instance defined");
            }
        }
        else
        {
            printError("buildDeviceMap", "Error querying current audio device");
        }

        if(res)
        {
            int rate = currentFrequency();

            for(int bitFormat = static_cast<int>(e_audioOutputFloat); bitFormat < static_cast<int>(e_audioOutputUnknown); bitFormat++)
            {
                int noBits;
                FormatDescription::DataType type = (bitFormat == e_audioOutputFloat) ? FormatDescription::e_DataFloatSingle : FormatDescription::e_DataSignedInteger;

                switch(static_cast<AOIOSAudioFormats>(bitFormat))
                {
                    case e_audioOutputInt16:
                        noBits = 16;
                        break;
                    case e_audioOutputInt24:
                        noBits = 24;
                        break;
                    case e_audioOutputInt32:
                    case e_audioOutputFloat:
                    case e_audioOutputUnknown:
                        noBits = 32;
                        break;
                }

                if(queryDeviceCapability(static_cast<AOIOSAudioFormats>(bitFormat), dev->noChannels(), rate))
                {
                    QSet<int> freqs = dev->frequencies();
                    for(QSet<int>::iterator ppI = freqs.begin(); ppI != freqs.end(); ppI++)
                    {
                        FormatDescription desc(type, noBits, dev->noChannels(), *ppI);
                        dev->addSupportedFormat(desc);
                    }
                }
            }

            dev->setHasExclusive(false);
            dev->setInitialized();
            m_devices.append(dynamic_cast<AOQueryDevice::Device *>(dev));
            res = (!dev->frequencies().isEmpty()) ? true : false;
        }
    }
    else
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudioIOS::getDescription(AOIOSAudioFormats bitFormat, int noChannels, int freq, AudioStreamBasicDescription *fmt)
{
    tint bytesPerSample;

    ::memset(fmt, 0, sizeof(AudioStreamBasicDescription));
    switch(bitFormat)
    {
        case e_audioOutputFloat:
            fmt->mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
            bytesPerSample = 4;
            break;
        case e_audioOutputInt16:
            fmt->mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
            bytesPerSample = 2;
            break;
        case e_audioOutputInt24:
            fmt->mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
            bytesPerSample = 3;
            break;
        case e_audioOutputInt32:
            fmt->mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
            bytesPerSample = 4;
            break;
        case e_audioOutputUnknown:
        default:
            printError("getDescription", "Undefined bit format");
            return false;
    }

    fmt->mSampleRate = static_cast<Float64>(freq);
    fmt->mFormatID = kAudioFormatLinearPCM;
    fmt->mFramesPerPacket = 1;
    fmt->mBitsPerChannel = bytesPerSample * 8;
    fmt->mBytesPerPacket = bytesPerSample * noChannels;
    fmt->mBytesPerFrame = bytesPerSample * noChannels;
    fmt->mChannelsPerFrame = noChannels;

    m_bytesPerSample = bytesPerSample;

    return true;
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudioIOS::queryDeviceCapability(AOIOSAudioFormats bitFormat, int noChannels, int freq)
{
    AudioComponent comp;
    AudioComponentDescription ioUnitDescription;
    AudioComponentInstance audioOutputUnit;
    OSStatus err;
    bool res = false;

    ::memset(&ioUnitDescription, 0, sizeof(AudioComponentDescription));
    ioUnitDescription.componentType = kAudioUnitType_Output;
    ioUnitDescription.componentSubType = kAudioUnitSubType_RemoteIO;
    ioUnitDescription.componentManufacturer = kAudioUnitManufacturer_Apple;

    comp = AudioComponentFindNext(0,&ioUnitDescription);
    if(comp != nil)
    {
        err = AudioComponentInstanceNew(comp, &audioOutputUnit);
        if(err == noErr)
        {
            UInt32 enableIO = 1;

            err = AudioUnitSetProperty(audioOutputUnit,kAudioOutputUnitProperty_EnableIO,kAudioUnitScope_Output,0,&enableIO,sizeof(enableIO));
            if(err == noErr)
            {
                AudioStreamBasicDescription streamFormat;

                if(getDescription(bitFormat, noChannels, freq, &streamFormat))
                {
                    err = AudioUnitSetProperty(audioOutputUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamFormat, sizeof(AudioStreamBasicDescription));
                    if(err == noErr)
                    {
                        AURenderCallbackStruct renderCallback;

                        ::memset(&renderCallback,0,sizeof(AURenderCallbackStruct));
                        renderCallback.inputProc = iosOmegaAudioCallbackIOProcQuery;
                        renderCallback.inputProcRefCon = (void *)(this);

                        err = AudioUnitSetProperty(audioOutputUnit,kAudioUnitProperty_SetRenderCallback,kAudioUnitScope_Input,0,&renderCallback,sizeof(renderCallback));
                        if(err == noErr)
                        {
                            err = AudioUnitInitialize(audioOutputUnit);
                            if(err == noErr)
                            {
                                Float64 sampleRate = 0.0;
                                UInt32 theSize = sizeof(sampleRate);

                                err = AudioUnitGetProperty(audioOutputUnit,kAudioUnitProperty_SampleRate,kAudioUnitScope_Output,0,&sampleRate,&theSize);
                                if(err == noErr)
                                {
                                    if(freq==static_cast<int>(sampleRate))
                                    {
                                        res = true;
                                    }
                                }
                            }
                            AudioUnitUninitialize(audioOutputUnit);
                        }
                    }
                }
            }
            AudioComponentInstanceDispose(audioOutputUnit);
        }
        else
        {
            printError("buildDeviceMap", "Failed to create instance for audio output.", err);
        }
    }
    else
    {
        printError("buildDeviceMap", "Failed to get default audio output component");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

OSStatus AOQueryCoreAudioIOS::iosOmegaAudioCallbackIOProcQuery(void *inRegCon, \
                                  AudioUnitRenderActionFlags *ioActionFlags, \
                                  const AudioTimeStamp *inTimeStamp, \
                                  UInt32 inBusNumber, \
                                  UInt32 inNumberFrames, \
                                  AudioBufferList *ioData)
{
    AOQueryCoreAudioIOS *query = reinterpret_cast<AOQueryCoreAudioIOS *>(inRegCon);
    for(tint i = 0; i < static_cast<int>(ioData->mNumberBuffers); i++)
    {
        tint noChs = static_cast<int>(ioData->mBuffers[i].mNumberChannels);
        ::memset(ioData->mBuffers[i].mData, 0 ,inNumberFrames * noChs * query->m_bytesPerSample);
    }
    return noErr;
}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudioIOS::rebuild()
{
    m_devices.clear();
    buildDeviceMap();
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------


