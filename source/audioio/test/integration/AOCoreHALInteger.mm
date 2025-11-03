#include "audioio/test/integration/AOCoreHALInteger.h"

#import <CoreFoundation/CoreFoundation.h>
#import <Cocoa/Cocoa.h>

#ifndef kAudioObjectPropertyElementMain
#define kAudioObjectPropertyElementMain kAudioObjectPropertyElementMaster
#endif

//-------------------------------------------------------------------------------------------

HALSignalGenerator::HALSignalGenerator(tint deviceIdx,int& argc,char **argv) : QCoreApplication(argc,argv),
    m_deviceIndex(deviceIdx),
    m_deviceInfo(0),
    m_exclusive(true),
    m_noChannels(2),
    m_frequencyPlayback(192000.0),
    m_frequencySignal(500.0),
    m_phase(0.0),
    m_flagStart(false),
    m_deviceIOProcID(0),
    m_enableMixing(false)
{}

//-------------------------------------------------------------------------------------------

HALSignalGenerator::~HALSignalGenerator()
{}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log.print("HALSignalGenerator::%s - %s\n",strR,strE);
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::printError(const tchar *strR,const tchar *strE,OSStatus err) const
{
    NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
    QString desc = QString::fromUtf8(error.localizedDescription.UTF8String);
    QString reason = QString::fromUtf8(error.localizedFailureReason.UTF8String);
    QString recovery = QString::fromUtf8(error.localizedRecoverySuggestion.UTF8String);
    QString eStr = "HALSignalGenerator::";
    eStr += strE;
    eStr += "\n\tDescription - " + desc;
    eStr += "\n\tReason - " + reason;
    eStr += "\n\tSuggestion - " + recovery + "\n";
    common::Log::g_Log.print("%s",eStr.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::log(const tchar *strR)
{
    common::Log::g_Log.print("%s\n",strR);
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::log(const QString& strR)
{
    log(strR.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::runToneSignal()
{
    QTimer::singleShot(10,this,SLOT(onSignalStart()));
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::onSignalStart()
{
    if(openAudio())
    {
        QTimer::singleShot(5000,this,SLOT(onSignalStop()));
    }
    else
    {
        onSignalStop();
    }
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::onSignalStop()
{
    closeAudio();
    quit();
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::openAudio()
{
    bool res = false;

    m_phase = 0.0;

    log("Opening Audio Device");
    if(queryAudioDevices())
    {
        log("Get audio device ID");
        AudioDeviceID devID = getDeviceID();

        if(devID!=kAudioDeviceUnknown)
        {
            res = true;

            {
                QString msg = "Using Audio Device : " + getDeviceName();
                log(msg);
            }

            if(m_exclusive)
            {
                log("Obtaining exclusive access to audio device");
                res = useExclusiveModeIfAvailable(devID);
            }

            if(res)
            {
                res = false;
                log("Setting sample rate");
                setSampleRate(devID,static_cast<tint>(m_frequencyPlayback));
                {
                    log("Setting audio stream format");
                    if(setAudioStreamFormat(devID,m_noChannels,static_cast<tint>(m_frequencyPlayback)))
                    {
                        OSStatus err;

                        log("Create I/O procedure");
                        err = AudioDeviceCreateIOProcID(devID,HALSignalGenerator::IOProc,(void *)this,&m_deviceIOProcID);
                        if(err==noErr)
                        {
                            log("Starting audio device");
                            err = AudioDeviceStart(devID,m_deviceIOProcID);
                            if(err==noErr)
                            {
                                log("Audio playback started");
                                m_flagStart = true;
                                res = true;
                            }
                            else
                            {
                                printError("openAudio","Failed to start audio playback",err);
                            }
                        }
                        else
                        {
                            printError("openAudio","Failed to create I/O playback callback",err);
                        }
                    }
                    else
                    {
                        printError("openAudio","Error setting the audio stream format");
                    }
                }
            }
            else
            {
                printError("openAudio","Error getting exclusive access to audio device");
            }
        }
        else
        {
            printError("openAudio","Unknown audio device is being referenced");
        }
    }
    else
    {
        printError("openAudio","Failed to query audio devices");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::closeAudio()
{
    if(m_deviceInfo!=0)
    {
        stopAudioDevice();
        deallocate();
    }
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::deallocate()
{
    if(m_deviceInfo!=0)
    {
        log("Releasing queried audio device information");
        delete m_deviceInfo;
        m_deviceInfo = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::queryAudioDevices()
{
    bool res = true;

    if(m_deviceInfo==0)
    {
        m_deviceInfo = new audioio::AOQueryCoreAudio();
        if(m_deviceInfo->queryNames())
        {
            for(tint i=0;i<m_deviceInfo->noDevices() && res;i++)
            {
                if(!m_deviceInfo->queryDevice(i))
                {
                    res = false;
                }
            }
            m_deviceInfo->print();
        }
        else
        {
            res = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

AudioDeviceID HALSignalGenerator::getDeviceID()
{
    AudioDeviceID devID;

    if(m_deviceIndex>=0 && m_deviceIndex<m_deviceInfo->noDevices())
    {
        const audioio::AOQueryCoreAudio::DeviceCoreAudio& dev = dynamic_cast<const audioio::AOQueryCoreAudio::DeviceCoreAudio &>(m_deviceInfo->device(m_deviceIndex));
        devID = dev.deviceID();
    }
    else
    {
        devID = kAudioDeviceUnknown;
    }
    return devID;
}

//-------------------------------------------------------------------------------------------

QString HALSignalGenerator::getDeviceName()
{
    QString name;

    if(m_deviceIndex>=0 && m_deviceIndex<m_deviceInfo->noDevices())
    {
        const audioio::AOQueryCoreAudio::DeviceCoreAudio& dev = dynamic_cast<const audioio::AOQueryCoreAudio::DeviceCoreAudio &>(m_deviceInfo->device(m_deviceIndex));
        name = dev.name();
    }
    else
    {
        name = "Unknown Device";
    }
    return name;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::useExclusiveModeIfAvailable(AudioDeviceID devId)
{
    UInt32 propSize;
    pid_t processID,cProcessID;
    AudioObjectPropertyAddress propAddr;
    OSStatus err;
    bool res = false;

    propAddr.mSelector = kAudioDevicePropertyHogMode;
    propAddr.mScope = kAudioDevicePropertyScopeOutput;
    propAddr.mElement = kAudioObjectPropertyElementMain;
    propSize = sizeof(pid_t);

    cProcessID = ::getpid();

    if(AudioObjectHasProperty(devId,&propAddr))
    {
        Boolean settableFlag = false;

        err = AudioObjectIsPropertySettable(devId,&propAddr,&settableFlag);
        if(err==noErr)
        {
            if(settableFlag)
            {
                err = AudioObjectGetPropertyData(devId,&propAddr,0,0,&propSize,&processID);
                if(err==noErr)
                {
                    if(processID==-1)
                    {
                        err = AudioObjectSetPropertyData(devId,&propAddr,0,0,propSize,(const void *)&cProcessID);
                        if(err==noErr)
                        {
                            log("Exclusive mode set for audio device");
                            res = true;
                        }
                        else
                        {
                            printError("useExclusiveModeIfAvailable","Failed to set exclusive mode for audio device",err);
                            res = false;
                        }
                    }
                    else
                    {
                        if(processID==cProcessID)
                        {
                            log("Exclusive mode already set for process");
                            res = true;
                        }
                        else
                        {
                            log("Exclusive mode could not be set as another process already has it");
                            res = false;
                        }
                    }
                }
                else
                {
                    printError("useExclusiveModeIfAvailable","Failed to get exclusive mode state",err);
                    res = false;
                }
            }
            else
            {
                log("Exclusive mode cannot be set for audio device");
                res = true;
            }
        }
        else
        {
            printError("useExclusiveModeIfAvailable","Error querying if exclusive mode can be set",err);
            res = false;
        }
    }
    else
    {
        log("Audio device does not have exclusive mode");
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::setAudioStreamFormat(AudioDeviceID devId,int noChannels,int sampleRate)
{
    UInt32 propSize;
    AudioStreamBasicDescription deviceFormat;
    AudioObjectPropertyAddress propAddr;
    OSStatus err;
    bool res = false;

    propAddr.mSelector = kAudioDevicePropertyStreamFormat;
    propAddr.mScope = kAudioDevicePropertyScopeOutput;
    propAddr.mElement = kAudioObjectPropertyElementMain;
    propSize = sizeof(AudioStreamBasicDescription);

    ::memset(&deviceFormat,0,sizeof(AudioStreamBasicDescription));
    deviceFormat.mSampleRate = static_cast<Float64>(sampleRate);
    deviceFormat.mFormatID = kAudioFormatLinearPCM;
    deviceFormat.mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
    deviceFormat.mFramesPerPacket = 1;
    deviceFormat.mBitsPerChannel = sizeof(float) * 8;
    deviceFormat.mBytesPerPacket = static_cast<UInt32>(sizeof(float)) * noChannels;
    deviceFormat.mBytesPerFrame = static_cast<UInt32>(sizeof(float)) * noChannels;
    deviceFormat.mChannelsPerFrame = static_cast<UInt32>(noChannels);

    err = AudioObjectSetPropertyData(devId,&propAddr,0,0,propSize,(const void *)&deviceFormat);
    if(err==noErr)
    {
        log("Stream format set");
        res = true;
    }
    else
    {
        printError("setAudioStreamFormat","Failed to set stream format",err);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::setSampleRate(AudioDeviceID devId,int sampleRate)
{
    Float64 sRate;
    UInt32 propSize;
    AudioObjectPropertyAddress propAddr;
    Boolean settableFlag = false;
    OSStatus err;
    bool res;

    propAddr.mSelector = kAudioDevicePropertyNominalSampleRate;
    propAddr.mScope = kAudioDevicePropertyScopeOutput;
    propAddr.mElement = kAudioObjectPropertyElementMain;
    propSize = sizeof(Float64);

    err = AudioObjectIsPropertySettable(devId,&propAddr,&settableFlag);
    if(err==noErr)
    {
        if(settableFlag)
        {
            sRate = static_cast<Float64>(sampleRate);
            err = AudioObjectSetPropertyData(devId,&propAddr,0,0,propSize,(const void *)&sRate);
            if(err==noErr)
            {
                QString msg = QString("Sample rate set to %1").arg(sampleRate);
                log(msg);
                res = true;
            }
            else
            {
                QString msg = QString("Error setting sample rate to %1").arg(sampleRate);
                printError("setSampleRate",msg.toUtf8().constData(),err);
                res = false;
            }
        }
        else
        {
            log("Sample rate cannot be set for audio device");
            res = true;
        }
    }
    else
    {
        printError("setSampleRate","Error querying if sample rate can be set",err);
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::stopAudioDevice()
{
    AudioDeviceID devID = getDeviceID();

    if(devID!=kAudioDeviceUnknown)
    {
        log("Closing audio device");

        if(m_deviceIOProcID!=0)
        {
            if(isDeviceRunning(devID))
            {
                log("Stopping audio device");
                AudioDeviceStop(devID,m_deviceIOProcID);
                log("Audio device stopped");
            }
            AudioDeviceDestroyIOProcID(devID,m_deviceIOProcID);
            m_deviceIOProcID = 0;

            if(m_enableMixing)
            {
                log("Re-enable mixing");
                reenableMixing(devID);
            }
        }
        log("Releasing exclusive device access");
        exclusiveModeRelease(devID);
        log("Audio device closed");
        m_flagStart = false;
    }
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::isDeviceRunning(AudioDeviceID devId)
{
    UInt32 runValue = 0;
    UInt32 propSize;
    AudioObjectPropertyAddress propAddr;
    OSStatus err;
    bool res = false;

    propAddr.mSelector = kAudioDevicePropertyDeviceIsRunning;
    propAddr.mScope = kAudioDevicePropertyScopeOutput;
    propAddr.mElement = kAudioObjectPropertyElementMain;
    propSize = sizeof(UInt32);

    err = AudioObjectGetPropertyData(devId,&propAddr,0,0,&propSize,&runValue);
    if(err==noErr)
    {
        if(runValue)
        {
            log("Device is running");
            res = true;
        }
        else
        {
            log("Device is not running");
        }
    }
    else
    {
        printError("isDeviceRunning","Error querying if device is running",err);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::exclusiveModeRelease(AudioDeviceID devId)
{
    UInt32 propSize;
    pid_t processID,cProcessID;
    AudioObjectPropertyAddress propAddr;
    OSStatus err;

    propAddr.mSelector = kAudioDevicePropertyHogMode;
    propAddr.mScope = kAudioDevicePropertyScopeOutput;
    propAddr.mElement = kAudioObjectPropertyElementMain;
    propSize = sizeof(pid_t);

    cProcessID = ::getpid();

    if(AudioObjectHasProperty(devId,&propAddr))
    {
        Boolean settableFlag = false;

        err = AudioObjectIsPropertySettable(devId,&propAddr,&settableFlag);
        if(err==noErr)
        {
            if(settableFlag)
            {
                err = AudioObjectGetPropertyData(devId,&propAddr,0,0,&propSize,&processID);
                if(err==noErr)
                {
                    if(processID==cProcessID)
                    {
                        err = AudioObjectSetPropertyData(devId,&propAddr,0,0,propSize,(const void *)&cProcessID);
                        if(err!=noErr)
                        {
                            printError("exclusiveModeRelease","Error unsetting exclusive access on device",err);
                        }
                    }
                }
                else
                {
                    printError("exclusiveModeRelease","Failed to get exclusive access state",err);
                }
            }
            else
            {
                log("Exclusive access cannot be unset for device");
            }
        }
        else
        {
            printError("exclusiveModeRelease","Error querying exclusive access mode",err);
        }
    }
    else
    {
        log("Audio device cannot be exclusively accessed");
    }
}

//-------------------------------------------------------------------------------------------

OSStatus HALSignalGenerator::IOProc(AudioDeviceID ioDevice,
                                    const AudioTimeStamp *inNow,
                                    const AudioBufferList *inInputData,
                                    const AudioTimeStamp *inInputTime,
                                    AudioBufferList *outOutputData,
                                    const AudioTimeStamp *inOutputTime,
                                    void *inClientData)
{
    HALSignalGenerator *ioAudio = reinterpret_cast<HALSignalGenerator *>(inClientData);
    return ioAudio->IOProcImpl(ioDevice,inNow,inInputData,inInputTime,outOutputData,inOutputTime);
}

//-------------------------------------------------------------------------------------------

OSStatus HALSignalGenerator::IOProcImpl(AudioDeviceID ioDevice,
                                        const AudioTimeStamp *inNow,
                                        const AudioBufferList *inInputData,
                                        const AudioTimeStamp *inInputTime,
                                        AudioBufferList *outOutputData,
                                        const AudioTimeStamp *inOutputTime)
{
    tint j,k;
    tfloat64 phase;
    tfloat64 phaseInc = (2.0 * c_PI_D * m_frequencySignal) / m_frequencyPlayback;

    j = 0;
    k = 0;
    while(j<m_noChannels && k<outOutputData->mNumberBuffers)
    {
        tfloat32 *out = reinterpret_cast<tfloat32 *>(outOutputData->mBuffers[k].mData);
        tint noChs = static_cast<tint>(outOutputData->mBuffers[k].mNumberChannels);
        tint len = outOutputData->mBuffers[k].mDataByteSize / (noChs * sizeof(tfloat32));

        phase = m_phase;
        for(tint i=0;i<len;i++,phase+=phaseInc)
        {
            for(tint m=0;m<noChs;m++)
            {
                *out = engine::sampleToFloat32(sin(phase));
                out++;
            }
        }
        j += noChs;
        k++;
    }
    m_phase = phase;

    return noErr;
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::queryDeviceStreamFormat(AudioDeviceID devId)
{
    UInt32 propSize;
    AudioStreamBasicDescription deviceFormat;
    AudioObjectPropertyAddress propAddr;
    OSStatus err;

    propAddr.mSelector = kAudioDevicePropertyStreamFormat;
    propAddr.mScope = kAudioDevicePropertyScopeOutput;
    propAddr.mElement = kAudioObjectPropertyElementMain;
    propSize = sizeof(AudioStreamBasicDescription);

    ::memset(&deviceFormat,0,sizeof(AudioStreamBasicDescription));

    err = AudioObjectGetPropertyData(devId,&propAddr,0,0,&propSize,(void *)&deviceFormat);
    if(err==noErr)
    {
        log("Stream format get");
    }
    else
    {
        printError("queryDeviceStreamFormat","Failed to get stream format",err);
    }
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::queryDeviceFormat()
{
    if(queryAudioDevices())
    {
        log("Get audio device ID");
        AudioDeviceID devID = getDeviceID();

        if(devID!=kAudioDeviceUnknown)
        {
            log(QString("Using Audio Device : ").arg(getDeviceName()));
            if(m_exclusive)
            {
                log("Obtaining exclusive access to audio device");
                useExclusiveModeIfAvailable(devID);
            }

            queryDeviceStreamFormat(devID);

            exclusiveModeRelease(devID);
        }
        deallocate();
    }
}

//-------------------------------------------------------------------------------------------

QString HALSignalGenerator::formatIDToString(UInt32 formatID)
{
    QString txt;

    switch(formatID)
    {
        case kAudioFormatLinearPCM:
            txt = "Linear PCM"; break;
        case kAudioFormatAC3:
            txt = "AC-3"; break;
        case kAudioFormat60958AC3:
            txt = "AC-3 packaged for transport over an IEC 60958 compliant digital audio interface"; break;
        case kAudioFormatAppleIMA4:
            txt = "Apples implementation of IMA 4:1 ADPCM"; break;
        case kAudioFormatMPEG4AAC:
            txt = "MPEG-4 Low Complexity AAC audio object"; break;
        case kAudioFormatMPEG4CELP:
            txt = "MPEG-4 CELP audio object"; break;
        case kAudioFormatMPEG4HVXC:
            txt = "MPEG-4 HVXC audio object"; break;
        case kAudioFormatMPEG4TwinVQ:
            txt = "MPEG-4 TwinVQ audio object type"; break;
        case kAudioFormatMACE3:
            txt = "MACE 3:1"; break;
        case kAudioFormatMACE6:
            txt = "MACE 6:1"; break;
        case kAudioFormatULaw:
            txt = "µLaw 2:1"; break;
        case kAudioFormatALaw:
            txt = "aLaw 2:1"; break;
        case kAudioFormatQDesign:
            txt = "QDesign music"; break;
        case kAudioFormatQDesign2:
            txt = "QDesign2 music"; break;
        case kAudioFormatQUALCOMM:
            txt = "QUALCOMM PureVoice"; break;
        case kAudioFormatMPEGLayer1:
            txt = "MPEG-1/2, Layer 1 audio"; break;
        case kAudioFormatMPEGLayer2:
            txt = "MPEG-1/2, Layer 2 audio"; break;
        case kAudioFormatMPEGLayer3:
            txt = "MPEG-1/2, Layer 3 audio"; break;
        case kAudioFormatTimeCode:
            txt = "A stream of IOAudioTimeStamps"; break;
        case kAudioFormatMIDIStream:
            txt = "A stream of MIDIPacketLists where the time stamps in the MIDIPacketList are sample offsets in the stream."; break;
        case kAudioFormatParameterValueStream:
            txt = "A \"side-chain\" of Float32 data that can be fed or generated by an AudioUnit and is used to send a high density of parameter value control information."; break;
        case kAudioFormatAppleLossless:
            txt = "Apple Lossless"; break;
        case kAudioFormatMPEG4AAC_HE:
            txt = "MPEG-4 High Efficiency AAC audio object"; break;
        case kAudioFormatMPEG4AAC_LD:
            txt = "MPEG-4 AAC Low Delay audio object"; break;
        case kAudioFormatMPEG4AAC_ELD:
            txt = "MPEG-4 AAC Enhanced Low Delay audio object"; break;
        case kAudioFormatMPEG4AAC_ELD_SBR:
            txt = "MPEG-4 AAC Enhanced Low Delay audio object with SBR extension layer"; break;
        case kAudioFormatMPEG4AAC_HE_V2:
            txt = "MPEG-4 High Efficiency AAC Version 2 audio object"; break;
        case kAudioFormatMPEG4AAC_Spatial:
            txt = "MPEG-4 Spatial Audio audio object"; break;
        case kAudioFormatAMR:
            txt = "The AMR Narrow Band speech codec."; break;
        case kAudioFormatAudible:
            txt = "The format used for Audible audio books"; break;
        case kAudioFormatiLBC:
            txt = "The iLBC narrow band speech codec"; break;
        case kAudioFormatDVIIntelIMA:
            txt = "DVI/Intel IMA ADPCM"; break;
        case kAudioFormatMicrosoftGSM:
            txt = "Microsoft GSM 6.10 - ACM code 49."; break;
        case kAudioFormatAES3:
            txt = "This format is defined by AES3-2003, and adopted into MXF and MPEG-2 containers and SDTI transport streams with SMPTE specs 302M-2002 and 331M-2000"; break;
        default:
            txt = "Unknown format"; break;
    }
    return txt;
}

//-------------------------------------------------------------------------------------------

QString HALSignalGenerator::formatFlagsToString(UInt32 flags)
{
    QString txt = "";

    if(flags & kAudioFormatFlagIsFloat)
    {
        txt += "Floating point, ";
    }
    else
    {
        if(flags & kAudioFormatFlagIsSignedInteger)
        {
            txt += "Signed integer, ";
        }
        else
        {
            txt += "Unsigned integer, ";
        }
    }

    if(flags & kAudioFormatFlagIsBigEndian)
    {
        txt += "Big Endian, ";
    }
    else
    {
        txt += "Little Endian, ";
    }

    if(flags & kAudioFormatFlagIsPacked)
    {
        txt += "All bits used, ";
    }
    else
    {
        if(flags & kAudioFormatFlagIsAlignedHigh)
        {
            txt += "Bits aligned high, ";
        }
        else
        {
            txt += "Bits aligned low, ";
        }
    }

    if(flags & kAudioFormatFlagIsNonInterleaved)
    {
        txt += "Non-Interleaved, ";
    }
    else
    {
        txt += "Interleaved, ";
    }

    if(flags & kAudioFormatFlagIsNonMixable)
    {
        txt += "Non-mixable";
    }
    else
    {
        txt += "Mixable";
    }

    return txt;
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::printFormat(AudioStreamBasicDescription *format)
{
    common::Log::g_Log.print("Sample Rate : %d\n",static_cast<tint>(format->mSampleRate));
    common::Log::g_Log.print("Format ID : %s\n",formatIDToString(format->mFormatID).toUtf8().constData());
    common::Log::g_Log.print("Format Flags : %s\n",formatFlagsToString(format->mFormatFlags).toUtf8().constData());
    common::Log::g_Log.print("Frames Per Packet : %d\n",format->mFramesPerPacket);
    common::Log::g_Log.print("Bits Per Channel : %d\n",format->mBitsPerChannel);
    common::Log::g_Log.print("Bytes Per Packet : %d\n",format->mBytesPerPacket);
    common::Log::g_Log.print("Bytes Per Frame : %d\n",format->mBytesPerFrame);
    common::Log::g_Log.print("Channels Per Frame : %d\n\n",format->mChannelsPerFrame);
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::printFormatsAvailableToAllDevices()
{
    if(queryAudioDevices())
    {
        for(m_deviceIndex=0;m_deviceIndex<m_deviceInfo->noDevices();m_deviceIndex++)
        {
            log("Get audio device ID");
            AudioDeviceID devID = getDeviceID();

            if(devID!=kAudioDeviceUnknown)
            {
                log(QString("Using Audio Device : %1").arg(getDeviceName()));
                if(m_exclusive)
                {
                    log("Obtaining exclusive access to audio device");
                    useExclusiveModeIfAvailable(devID);
                }
                log("");

                QVector<AudioStreamID> streams = getAudioStreamsForDevice(devID);
                QVector<AudioStreamID>::iterator ppI;

                for(ppI=streams.begin();ppI!=streams.end();ppI++)
                {
                    AudioStreamID streamID = *ppI;
                    QVector<AudioStreamRangedDescription> formats;
                    QVector<AudioStreamRangedDescription>::iterator ppJ;

                    formats = getAudioStreamDescriptions(streamID);
                    for(ppJ=formats.begin();ppJ!=formats.end();ppJ++)
                    {
                        AudioStreamRangedDescription desc = *ppJ;
                        printFormat(&desc.mFormat);
                    }
                }

                exclusiveModeRelease(devID);
            }
            log("");
        }
        deallocate();
    }
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::printFormatsAvailableToDevice()
{
    if(queryAudioDevices())
    {
        log("Get audio device ID");
        AudioDeviceID devID = getDeviceID();

        if(devID!=kAudioDeviceUnknown)
        {
            log(QString("Using Audio Device : %1").arg(getDeviceName()));
            if(m_exclusive)
            {
                log("Obtaining exclusive access to audio device");
                useExclusiveModeIfAvailable(devID);
            }
            log("");

            QVector<AudioStreamID> streams = getAudioStreamsForDevice(devID);
            QVector<AudioStreamID>::iterator ppI;

            for(ppI=streams.begin();ppI!=streams.end();ppI++)
            {
                AudioStreamID streamID = *ppI;
                QVector<AudioStreamRangedDescription> formats;
                QVector<AudioStreamRangedDescription>::iterator ppJ;

                formats = getAudioStreamDescriptions(streamID);
                for(ppJ=formats.begin();ppJ!=formats.end();ppJ++)
                {
                    AudioStreamRangedDescription desc = *ppJ;
                    printFormat(&desc.mFormat);
                }
            }

            exclusiveModeRelease(devID);
        }
        deallocate();
    }
}

//-------------------------------------------------------------------------------------------

QVector<AudioStreamID> HALSignalGenerator::getAudioStreamsForDevice(AudioDeviceID devID)
{
    UInt32 paramSize = 0;
    QVector<AudioStreamID> streamIDs;
    AudioObjectPropertyAddress propAddr;
    OSStatus err;

    propAddr.mSelector = kAudioDevicePropertyStreams;
    propAddr.mScope = kAudioDevicePropertyScopeOutput;
    propAddr.mElement = kAudioObjectPropertyElementMain;

    err = AudioObjectGetPropertyDataSize(devID,&propAddr,0,0,&paramSize);
    if(err==noErr)
    {
        tint noStreams = static_cast<tint>(paramSize) / sizeof(AudioStreamID);

        if(noStreams > 0)
        {
            AudioStreamID *pStreams = new AudioStreamID [noStreams];

            err = AudioObjectGetPropertyData(devID,&propAddr,0,0,&paramSize,pStreams);
            if(err==noErr)
            {
                for(tint i=0;i<noStreams;i++)
                {
                    streamIDs.append(pStreams[i]);
                }
            }
            else
            {
                printError("getAudioStreamsForDevice","Failed to get audio streams",err);
            }
            delete [] pStreams;
        }
        else
        {
            printError("getAudioStreamsForDevice","No audio streams available");
        }
    }
    else
    {
        printError("getAudioStreamsForDevice","Error getting size of audio streams",err);
    }
    return streamIDs;
}

//-------------------------------------------------------------------------------------------

QVector<AudioStreamRangedDescription> HALSignalGenerator::getAudioStreamDescriptions(AudioStreamID streamID)
{
    UInt32 paramSize = 0;
    QVector<AudioStreamRangedDescription> streams;
    AudioObjectPropertyAddress propAddr;
    OSStatus err;

    propAddr.mSelector = kAudioStreamPropertyAvailablePhysicalFormats;
    propAddr.mScope = kAudioObjectPropertyScopeGlobal;
    propAddr.mElement = 0;

    err = AudioObjectGetPropertyDataSize(streamID,&propAddr,0,0,&paramSize);
    if(err==noErr)
    {
        tint noFormats = paramSize / sizeof(AudioStreamRangedDescription);
        if(noFormats>0)
        {
            AudioStreamRangedDescription *descriptions = new AudioStreamRangedDescription [noFormats];

            err = AudioObjectGetPropertyData(streamID,&propAddr,0,0,&paramSize,descriptions);
            if(err==noErr)
            {
                for(tint i=0;i<noFormats;i++)
                {
                    streams.append(descriptions[i]);
                }
            }
            else
            {
                printError("getAudioStreamDescriptions","Failed to get formats for stream ID",err);
            }

            delete [] descriptions;
        }
        else
        {
            printError("getAudioStreamDescriptions","No formats available for stream ID");
        }
    }
    else
    {
        printError("getAudioStreamDescriptions","Error getting number of stream formats",err);
    }
    return streams;
}

//-------------------------------------------------------------------------------------------

QPair<AudioStreamID,AudioStreamBasicDescription *> HALSignalGenerator::findClosestStream(const FormatDescription& sourceDesc,const QVector<AudioStreamID>& streamIDList)
{
    QPair<AudioStreamID,AudioStreamBasicDescription *> streamID(kAudioObjectUnknown,0);
    FormatsSupported supported;
    FormatDescription closeDesc;

    streamID.first = kAudioObjectUnknown;

    for(QVector<AudioStreamID>::const_iterator ppI=streamIDList.constBegin();ppI!=streamIDList.constEnd();ppI++)
    {
        AudioStreamID ID = *ppI;
        QVector<AudioStreamRangedDescription> streamList;

        streamList = getAudioStreamDescriptions(ID);
        for(QVector<AudioStreamRangedDescription>::const_iterator ppJ=streamList.constBegin();ppJ!=streamList.constEnd();ppJ++)
        {
            const AudioStreamRangedDescription& rangeDesc = *ppJ;

            if(rangeDesc.mFormat.mFormatFlags & kAudioFormatFlagIsNonMixable)
            {
                FormatDescription desc;

                if(generateFormatDescription(rangeDesc.mFormat,desc))
                {
                    supported.add(desc);
                }
            }
        }
    }

    if(FormatDescriptionUtils::findClosestFormatType(sourceDesc,supported,closeDesc))
    {
        for(QVector<AudioStreamID>::const_iterator ppI=streamIDList.constBegin();ppI!=streamIDList.constEnd() && streamID.first==kAudioObjectUnknown;ppI++)
        {
            AudioStreamID ID = *ppI;
            QVector<AudioStreamRangedDescription> streamList;

            streamList = getAudioStreamDescriptions(ID);
            for(QVector<AudioStreamRangedDescription>::const_iterator ppJ=streamList.constBegin();ppJ!=streamList.constEnd() && streamID.first==kAudioObjectUnknown;ppJ++)
            {
                const AudioStreamRangedDescription& rangeDesc = *ppJ;
                FormatDescription desc;

                if(isDescriptionsEqual(rangeDesc.mFormat,closeDesc) && (rangeDesc.mFormat.mFormatFlags & kAudioFormatFlagIsNonMixable))
                {
                    streamID.first = ID;
                    streamID.second = new AudioStreamBasicDescription;
                    memcpy(streamID.second,&rangeDesc.mFormat,sizeof(AudioStreamBasicDescription));
                }
            }
        }
    }
    return streamID;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::generateFormatDescription(const AudioStreamBasicDescription& format,FormatDescription& desc)
{
    bool res = false;

    if(format.mFormatID==kAudioFormatLinearPCM)
    {
        if(format.mFormatFlags & kAudioFormatFlagIsFloat)
        {
            desc.setTypeOfData(FormatDescription::e_DataFloatSingle);
        }
        else if(format.mFormatFlags & kAudioFormatFlagIsSignedInteger)
        {
            desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
        }
        else
        {
            desc.setTypeOfData(FormatDescription::e_DataUnsignedInteger);
        }

        if(desc.setNumberOfBits(format.mBitsPerChannel))
        {
            if(desc.setNumberOfChannels(format.mChannelsPerFrame))
            {
                if(desc.setFrequency(format.mSampleRate))
                {
                    res = isFormatDataTypeCorrisponding(format,desc);
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::isFormatDataTypeCorrisponding(const AudioStreamBasicDescription& format,const FormatDescription& desc)
{
    bool res = false;

    if(format.mFormatID==kAudioFormatLinearPCM)
    {
        if((format.mFormatFlags & kAudioFormatFlagIsFloat) && desc.typeOfData()==FormatDescription::e_DataFloatSingle)
        {
            res = true;
        }
        else if((format.mFormatFlags & kAudioFormatFlagIsSignedInteger) && desc.typeOfData()==FormatDescription::e_DataSignedInteger)
        {
            res = true;
        }
        else if(desc.typeOfData()==FormatDescription::e_DataUnsignedInteger)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::isDescriptionsEqual(const AudioStreamBasicDescription& format,const FormatDescription& desc)
{
    bool res = false;

    if(format.mFormatID==kAudioFormatLinearPCM)
    {
        if(isFormatDataTypeCorrisponding(format,desc))
        {
            if(desc.bits()==format.mBitsPerChannel && desc.channels()==format.mChannelsPerFrame && desc.frequency()==format.mSampleRate)
            {
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::openAudioStream()
{
    bool res = false;

    log("Setup audio run loop");
    if(audioRunLoopSetup())
    {
        log("Opening Audio Device");
        if(queryAudioDevices())
        {
            log("Get audio device ID");
            AudioDeviceID devID = getDeviceID();
            if(devID!=kAudioDeviceUnknown)
            {
                QVector<AudioStreamID> streamList;

                {
                    QString msg = "Using Audio Device : " + getDeviceName();
                    log(msg);
                }

                if(m_exclusive)
                {
                    log("Obtaining exclusive access to audio device");
                    res = useExclusiveModeIfAvailable(devID);
                }

                log("Try to disable mixing");
                m_enableMixing = disableMixingIfPossible(devID);
                if(m_enableMixing)
                {
                    log("Mixing disabled");
                }

                log("Get audio streams for device");
                streamList = getAudioStreamsForDevice(devID);
                if(!streamList.isEmpty())
                {
                    QPair<AudioStreamID,AudioStreamBasicDescription *> closestStream;

                    log("Finding closest desired format");
                    closestStream = findClosestStream(getSourceDescription(),streamList);
                    if(closestStream.first!=kAudioObjectUnknown && closestStream.second!=0)
                    {
                        if(closestStream.second->mFormatFlags & kAudioFormatFlagIsSignedInteger)
                        {
                            m_converter = new SampleConverter(getSampleConverterFromDescription(*(closestStream.second)));

                            log("Creating sample converter");
                            if(m_converter->isSupported())
                            {
                                log("Setting physical device stream format");
                                if(setAudioStream(closestStream.first,*(closestStream.second)))
                                {
                                    OSStatus err;

//                                    log("Setting sample rate");
//                                    setSampleRate(devID,static_cast<tint>(m_frequencyPlayback));

                                    log("Create I/O procedure");
                                    err = AudioDeviceCreateIOProcID(devID,HALSignalGenerator::IOProcInteger,(void *)this,&m_deviceIOProcID);
//                                    err = AudioDeviceCreateIOProcID(devID,HALSignalGenerator::IOProc,(void *)this,&m_deviceIOProcID);
                                    if(err==noErr)
                                    {
                                        log("Starting audio device");
                                        err = AudioDeviceStart(devID,m_deviceIOProcID);
                                        if(err==noErr)
                                        {
                                            log("Audio playback started");
                                            m_flagStart = true;
                                            res = true;
                                        }
                                        else
                                        {
                                            printError("openAudio","Failed to start audio playback",err);
                                        }
                                    }
                                    else
                                    {
                                        printError("openAudio","Failed to create I/O playback callback",err);
                                    }
                                }
                                else
                                {
                                    printError("openAudio","Failed to setup physical format on audio stream");
                                }
                            }
                            else
                            {
                                printError("openAudio","Closest audio format not supported");
                            }
                        }
                        else
                        {
                            printError("openAudio","Audio format is not integer mode");
                        }
                        delete closestStream.second;
                    }
                    else
                    {
                        printError("openAudio","Failed to find format for device");
                    }
                }
                else
                {
                    printError("openAudio","Failed to get list of audio streams for device");
                }
            }
            else
            {
                printError("openAudio","Unknown audio device is being referenced");
            }
        }
        else
        {
            printError("openAudio","Failed to query audio devices");
        }
    }
    else
    {
        printError("openAudio","Failed to setup audio run loop");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::closeAudioStream()
{
    if(m_deviceInfo!=0)
    {
        stopAudioDevice();
        if(m_converter!=0)
        {
            delete m_converter;
            m_converter = 0;
        }
        deallocate();
    }
}

//-------------------------------------------------------------------------------------------

FormatDescription HALSignalGenerator::getSourceDescription()
{
    FormatDescription desc(FormatDescription::e_DataFloatDouble,64,2,m_frequencyPlayback);
    return desc;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::getDescription(const AudioStreamBasicDescription& format,FormatDescription& desc)
{
    bool res = false;

    if(format.mFormatID==kAudioFormatLinearPCM)
    {
        FormatDescription::DataType type;

        if(format.mFormatFlags & kAudioFormatFlagIsFloat)
        {
            type = FormatDescription::e_DataFloatSingle;
        }
        else
        {
            if(format.mFormatFlags & kAudioFormatFlagIsSignedInteger)
            {
                type = FormatDescription::e_DataSignedInteger;
            }
            else
            {
                type = FormatDescription::e_DataUnsignedInteger;
            }
        }

        desc.setTypeOfData(type);
        desc.setNumberOfBits(format.mBitsPerChannel);
        desc.setNumberOfChannels(format.mChannelsPerFrame);
        desc.setFrequency(format.mSampleRate);

        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

SampleConverter HALSignalGenerator::getSampleConverterFromDescription(const AudioStreamBasicDescription& format)
{
    tint noBits = format.mBitsPerChannel;
    tint bytesPerSample = format.mBytesPerFrame / format.mChannelsPerFrame;
    bool littleEndian = (format.mFormatFlags & kAudioFormatFlagIsBigEndian) ? false : true;
    bool alignHigh = (format.mFormatFlags & kAudioFormatFlagIsAlignedHigh) ? true : false;
    SampleConverter converter(noBits,bytesPerSample,littleEndian,alignHigh,true);
    return converter;
}

//-------------------------------------------------------------------------------------------

OSStatus HALSignalGenerator::IOProcInteger(AudioDeviceID ioDevice,
                                           const AudioTimeStamp *inNow,
                                           const AudioBufferList *inInputData,
                                           const AudioTimeStamp *inInputTime,
                                           AudioBufferList *outOutputData,
                                           const AudioTimeStamp *inOutputTime,
                                           void *inClientData)
{
    HALSignalGenerator *signalGenerator = reinterpret_cast<HALSignalGenerator *>(inClientData);
    return signalGenerator->IntegerIOProcImpl(ioDevice,inNow,inInputData,inInputTime,outOutputData,inOutputTime);
}

//-------------------------------------------------------------------------------------------

OSStatus HALSignalGenerator::IntegerIOProcImpl(AudioDeviceID ioDevice,
                                               const AudioTimeStamp *inNow,
                                               const AudioBufferList *inInputData,
                                               const AudioTimeStamp *inInputTime,
                                               AudioBufferList *outOutputData,
                                               const AudioTimeStamp *inOutputTime)
{
    const tint c_bufferLength = 512;

    tint j,k;
    tfloat64 phase = m_phase;
    sample_t dBuffer[c_bufferLength];
    tfloat64 phaseInc = (2.0 * c_PI_D * m_frequencySignal) / m_frequencyPlayback;

    j = 0;
    k = 0;
    while(j<m_noChannels && k<outOutputData->mNumberBuffers)
    {
        tint i;
        tbyte *out = reinterpret_cast<tbyte *>(outOutputData->mBuffers[k].mData);
        tint noChs = static_cast<tint>(outOutputData->mBuffers[k].mNumberChannels);
        tint len = outOutputData->mBuffers[k].mDataByteSize / (noChs * m_converter->bytesPerSample());

        phase = m_phase;

        i = 0;
        while(i<len)
        {
            tint bPos,pos;

            bPos = i;
            pos = 0;
            while(bPos<len && pos<c_bufferLength)
            {
                for(tint m=0;m<noChs && pos<c_bufferLength;m++,pos++)
                {
                    dBuffer[pos] = static_cast<sample_t>(sin(phase));
                }

                phase += phaseInc;
                if(phase > 2.0 * c_PI_D)
                {
                    phase -= 2.0 * c_PI_D;
                }

                bPos++;
            }

            m_converter->convert(dBuffer,&out[i * noChs * m_converter->bytesPerSample()],pos);
            i = bPos;
        }

        j += noChs;
        k++;
    }
    m_phase = phase;

    return noErr;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::audioRunLoopSetup()
{
    bool res;
    AudioObjectPropertyAddress propAddr;
    CFRunLoopRef runLoop = 0;
    OSStatus err;

    propAddr.mSelector = kAudioHardwarePropertyRunLoop;
    propAddr.mScope = kAudioObjectPropertyScopeGlobal;
    propAddr.mElement = kAudioObjectPropertyElementMain;

    err = AudioObjectSetPropertyData(kAudioObjectSystemObject,&propAddr,0,0,sizeof(CFRunLoopRef),&runLoop);
    if(err==noErr)
    {
        res = true;
    }
    else
    {
        printError("audioRunLoopSetup","Error setting run loop",err);
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

OSStatus HALSignalGenerator::audioStreamChangeListener(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddress[],void *inClientData)
{
    OSStatus err = noErr;
    HALSignalGenerator *signalGenerator = reinterpret_cast<HALSignalGenerator *>(inClientData);

    if(signalGenerator!=0)
    {
        err = signalGenerator->audioStreamChangeListenerImpl(inObjectID,inNumberAddresses,inAddress);
    }
    return err;
}

//-------------------------------------------------------------------------------------------

OSStatus HALSignalGenerator::audioStreamChangeListenerImpl(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddress[])
{
    for(UInt32 i=0;i<inNumberAddresses;i++)
    {
        if(inAddress[i].mSelector == kAudioStreamPropertyPhysicalFormat)
        {
            m_condition.wakeAll();
        }
    }
    return noErr;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::setAudioStream(AudioStreamID streamID,AudioStreamBasicDescription format)
{
    OSStatus err;
    FormatDescription desc;
    AudioObjectPropertyAddress propAddr;
    bool res = false;

    m_mutex.lock();

    propAddr.mSelector = kAudioStreamPropertyPhysicalFormat;
    propAddr.mScope = kAudioObjectPropertyScopeGlobal;
    propAddr.mElement = kAudioObjectPropertyElementMain;

    if(getDescription(format,desc))
    {
        err = AudioObjectAddPropertyListener(streamID,&propAddr,HALSignalGenerator::audioStreamChangeListener,(void *)this);
        if(err==noErr)
        {
            err = AudioObjectSetPropertyData(streamID,&propAddr,0,0,sizeof(AudioStreamBasicDescription),&format);
            if(err==noErr)
            {
                AudioStreamBasicDescription actualFormat;
                UInt32 paramSize = sizeof(AudioStreamBasicDescription);

                for(tint i=0;i<9 && !res;i++)
                {
                    if(i > 0)
                    {
                        m_condition.wait(&m_mutex,500);
                    }
                    err = AudioObjectGetPropertyData(streamID,&propAddr,0,0,&paramSize,&actualFormat);
                    if(err==noErr)
                    {
                        FormatDescription actualDesc;

                        log("Current Set Format");
                        printFormat(&actualFormat);

                        if(getDescription(actualFormat,actualDesc))
                        {
                            if(desc==actualDesc)
                            {
                                res = true;
                            }
                        }
                    }
                    else
                    {
                        printError("setAudioStream","Error getting physical format for device",err);
                    }
                }
            }
            else
            {
                printError("setAudioStream","Could not set the stream format",err);
            }
        }
        else
        {
            printError("setAudioStream","Failed to attach property listener",err);
        }
    }
    else
    {
        printError("setAudioStream","Incompatible audio format");
    }

    err = AudioObjectRemovePropertyListener(streamID,&propAddr,HALSignalGenerator::audioStreamChangeListener,(void *)this);
    if(err!=noErr)
    {
        printError("setAudioStream","Failed to remove property listener",err);
        res = false;
    }

    m_mutex.unlock();
    return res;
}

//-------------------------------------------------------------------------------------------

bool HALSignalGenerator::disableMixingIfPossible(AudioDeviceID devId)
{
    AudioObjectPropertyAddress propAddr;
    bool res = false;

    propAddr.mSelector = kAudioDevicePropertySupportsMixing;
    propAddr.mScope = kAudioObjectPropertyScopeGlobal;
    propAddr.mElement = kAudioObjectPropertyElementMain;

    if(AudioObjectHasProperty(devId,&propAddr))
    {
        UInt32 paramSize = 0,mix = 0;
        Boolean writeable = false;
        OSStatus err;

        err = AudioObjectIsPropertySettable(devId,&propAddr,&writeable);
        if(err==noErr)
        {
            err = AudioObjectGetPropertyDataSize(devId,&propAddr,0,0,&paramSize);
            if(err==noErr)
            {
                err = AudioObjectGetPropertyData(devId,&propAddr,0,0,&paramSize,&mix);
                if(err==noErr)
                {
                    if(writeable && mix)
                    {
                        mix = 0;
                        err = AudioObjectSetPropertyData(devId,&propAddr,0,0,paramSize,&mix);
                        if(err==noErr)
                        {
                            res = true;
                        }
                        else
                        {
                            printError("disableMixingIfPossible","Failed to set disable mixing property",err);
                        }
                    }
                }
                else
                {
                    printError("disableMixingIfPossible","Failed to get disable mixing property",err);
                }
            }
            else
            {
                printError("disableMixingIfPossible","Failed to get disable mixing property size",err);
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::reenableMixing(AudioDeviceID devId)
{
    AudioObjectPropertyAddress propAddr;

    propAddr.mSelector = kAudioDevicePropertySupportsMixing;
    propAddr.mScope = kAudioObjectPropertyScopeGlobal;
    propAddr.mElement = kAudioObjectPropertyElementMain;

    if(AudioObjectHasProperty(devId,&propAddr))
    {
        UInt32 paramSize = 0,mix = 0;
        Boolean writeable = false;
        OSStatus err;

        err = AudioObjectIsPropertySettable(devId,&propAddr,&writeable);
        if(err==noErr)
        {
            err = AudioObjectGetPropertyDataSize(devId,&propAddr,0,0,&paramSize);
            if(err==noErr)
            {
                err = AudioObjectGetPropertyData(devId,&propAddr,0,0,&paramSize,&mix);
                if(err==noErr)
                {
                    if(writeable)
                    {
                        mix = 1;
                        err = AudioObjectSetPropertyData(devId,&propAddr,0,0,paramSize,&mix);
                        if(err!=noErr)
                        {
                            printError("reenableMixing","Failed to set disable mixing property",err);
                        }
                    }
                }
                else
                {
                    printError("reenableMixing","Failed to get disable mixing property",err);
                }
            }
            else
            {
                printError("reenableMixing","Failed to get disable mixing property size",err);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::runIntegerToneSignal()
{
    QTimer::singleShot(10,this,SLOT(onIntegerSignalStart()));
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::onIntegerSignalStart()
{
    if(openAudioStream())
    {
        QTimer::singleShot(5000,this,SLOT(onIntegerSignalStop()));
    }
    else
    {
        onIntegerSignalStop();
    }
}

//-------------------------------------------------------------------------------------------

void HALSignalGenerator::onIntegerSignalStop()
{
    closeAudioStream();
    quit();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreHALInteger,signal)
{
    int argc = 0;
    HALSignalGenerator signalApp(2,argc,0);
    signalApp.runToneSignal();
    signalApp.exec();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreHALInteger,signalInteger)
{
    int argc = 0;
    HALSignalGenerator signalApp(2,argc,0);
    signalApp.runIntegerToneSignal();
    signalApp.exec();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreHALInteger,queryDeviceFormat)
{
    int argc = 0;
    HALSignalGenerator signalApp(2,argc,0);
    signalApp.queryDeviceFormat();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreHALInteger,printDeviceFormats)
{
    int argc = 0;
    HALSignalGenerator signalApp(2,argc,0);
    signalApp.printFormatsAvailableToDevice();
}

//-------------------------------------------------------------------------------------------

TEST(AOCoreHALInteger,printAllDeviceFormats)
{
    int argc = 0;
    HALSignalGenerator signalApp(2,argc,0);
    signalApp.printFormatsAvailableToAllDevices();
}

//-------------------------------------------------------------------------------------------
