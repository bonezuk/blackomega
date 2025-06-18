//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOCoreAudioMacOS.h"
#include "audioio/inc/AOQueryCoreAudio.h"
#include "engine/inc/FormatTypeFromFloat.h"
#include "audioio/inc/AudioHardwareBufferCoreAudio.h"

#include <QSet>
#include <QVector>
#include <QPair>
#include <QCoreApplication>

#import <CoreFoundation/CoreFoundation.h>
#import <Cocoa/Cocoa.h>

#ifndef kAudioObjectPropertyElementMain
#define kAudioObjectPropertyElementMain kAudioObjectPropertyElementMaster
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(AOFactory,AOBase, \
                            AOCoreAudioMacOSFactory,AOCoreAudioMacOS, \
                            "coreaudio",false)

//-------------------------------------------------------------------------------------------

AOCoreAudioMacOS::AOCoreAudioMacOS(QObject *parent) : AOCoreAudio(parent),
    m_outputUnit(0),
    m_flagInit(false),
    m_flagStart(false),
    m_audioStreamMutex(),
    m_audioStreamCondition(),
    m_originalSampleRate(-1),
    m_isIntegerMode(false),
    m_hasExclusiveMode(false),
    m_hasMixingBeenDisabled(false),
    m_pSampleConverter(),
    m_oldStreamID(kAudioObjectUnknown),
    m_oldStreamDescription(0),
    m_integerDeviceID(kAudioObjectUnknown),
    m_pIntegerDeviceIOProcID(0),
    m_isDeviceVolume(false)
{}

//-------------------------------------------------------------------------------------------

AOCoreAudioMacOS::~AOCoreAudioMacOS()
{}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "AOCoreAudio::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::printErrorOS(const tchar *strR,const tchar *strE,OSStatus err) const
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
    common::Log::g_Log << "AOCoreAudio::" << strR << " - " << eStr.toUtf8().constData();
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::init()
{
    bool res = AOBase::init();
    CoreAudioIF::instance("coreaudio");
    updateExclusiveAndIntegerModeOnDevices();
    addListenerDevices();
    return res;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::reset()
{
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pCoreDevice = getCurrentCoreAudioDevice();
    if(!pCoreDevice.isNull())
    {
        resetSampleRateToOriginalWhenClosing(pCoreDevice->deviceID());
    }

    removeListenerDevices();
    AOBase::reset();
    CoreAudioIF::release();
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::isAudio() const
{
    return m_flagInit;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> AOCoreAudioMacOS::getCurrentCoreAudioDevice()
{
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pCoreDevice;
    QSharedPointer<AOQueryDevice::Device> pDevice = getCurrentDevice();
    if(!pDevice.isNull())
    {
        pCoreDevice = pDevice.dynamicCast<AOQueryCoreAudio::DeviceCoreAudio>();
    }
    return pCoreDevice;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::openAudio()
{
    bool res = false;

    closeAudio();
    m_frequency = m_codec->frequency();

    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pCoreDevice = getCurrentCoreAudioDevice();
    if(pCoreDevice.isNull())
    {
        printError("openAudio","Could not find audio device");
        return false;
    }

    m_isIntegerMode = (isExclusive() && pCoreDevice->isIntegerMode()) ? true : false;

    m_isDeviceVolume = isDeviceVolume();
    if(m_isDeviceVolume)
    {
        m_volume = getDeviceVolume();
        emitOnVolumeChanged(m_volume);
    }

    if(m_isIntegerMode)
    {
        res = openIntegerAudio(pCoreDevice);
        if(!res)
        {
            printError("openAudio","Could not open audio in integer mode");
            closeAudio();
        }
    }

    if(!res)
    {
        m_isIntegerMode = false;
        res = openAudioCoreAudio(pCoreDevice);
        if(!res)
        {
            printError("openAudio","Could not open audio using CoreAudio");
            closeAudio();
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::openAudioCoreAudio(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pCoreDevice)
{
    AudioDeviceID devID;
    OSStatus err;
    bool res = false;

    devID = pCoreDevice->deviceID();
    if(devID!=kAudioDeviceUnknown)
    {
        AudioComponent comp;
        AudioComponentDescription desc;

        desc.componentType = kAudioUnitType_Output;
        desc.componentSubType = kAudioUnitSubType_HALOutput;
        desc.componentManufacturer = kAudioUnitManufacturer_Apple;
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;

        m_hasExclusiveMode = (isExclusive() && pCoreDevice->hasExclusive()) ? setExclusiveMode(pCoreDevice->deviceID(), true) : false;

        comp = AudioComponentFindNext(0,&desc);
        if(comp!=0)
        {
            err = AudioComponentInstanceNew(comp,&m_outputUnit);
            if(err==noErr)
            {
                UInt32 enableIO = 1;

                err = AudioUnitSetProperty(m_outputUnit,kAudioOutputUnitProperty_EnableIO,kAudioUnitScope_Output,0,&enableIO,sizeof(enableIO));
                if(err!=noErr)
                {
                    printErrorOS("openAudio","Error enabling output audio I/O",err);
                    return false;
                }

                err = AudioUnitSetProperty(m_outputUnit,kAudioOutputUnitProperty_CurrentDevice,kAudioUnitScope_Global,0,&devID,sizeof(devID));
                if(err==noErr)
                {
                    if(getDeviceFrequency()>0)
                    {
                        setSampleRateWhileOpeningCoreDevice(devID);

                        initCyclicBuffer();

                        m_noChannels = pCoreDevice->noChannels();
                        ::memset(&m_streamFormat,0,sizeof(AudioStreamBasicDescription));
                        m_streamFormat.mSampleRate = static_cast<Float64>(m_frequency);
                        m_streamFormat.mFormatID = kAudioFormatLinearPCM;
                        m_streamFormat.mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
                        m_streamFormat.mFramesPerPacket = 1;
                        m_streamFormat.mBitsPerChannel = sizeof(tfloat32) * 8;
                        m_streamFormat.mBytesPerPacket = static_cast<UInt32>(sizeof(tfloat32)) * m_noChannels;
                        m_streamFormat.mBytesPerFrame = static_cast<UInt32>(sizeof(tfloat32)) * m_noChannels;
                        m_streamFormat.mChannelsPerFrame = static_cast<UInt32>(m_noChannels);

                        err = AudioUnitSetProperty(m_outputUnit,kAudioUnitProperty_StreamFormat,kAudioUnitScope_Input,0,&m_streamFormat,sizeof(AudioStreamBasicDescription));
                        if(err==noErr)
                        {
                            ::memset(&m_renderCallback,0,sizeof(AURenderCallbackStruct));
                            m_renderCallback.inputProc = AOCoreAudioMacOS::callbackIOProc;
                            m_renderCallback.inputProcRefCon = reinterpret_cast<void *>(this);

                            err = AudioUnitSetProperty(m_outputUnit,kAudioUnitProperty_SetRenderCallback,kAudioUnitScope_Input,0,&m_renderCallback,sizeof(m_renderCallback));
                            if(err==noErr)
                            {
                                addListenerJackConnection(devID);
                                addVolumeChangeNotification(devID);

                                err = AudioUnitInitialize(m_outputUnit);
                                if(err==noErr)
                                {
                                    m_flagInit = true;
                                    res = true;
                                }
                                else
                                {
                                    AudioUnitUninitialize(m_outputUnit);
                                    printErrorOS("openAudio","Failed to initialize audio output device",err);
                                }
                            }
                            else
                            {
                                printErrorOS("openAudio","Error setting audio render callback",err);
                            }
                        }
                        else
                        {
                            printErrorOS("openAudio","Failed to set audio format on output device",err);
                        }
                    }
                    else
                    {
                        printError("openAudio","Failed to find supported audio frequency");
                    }
                }
                else
                {
                    printErrorOS("openAudio","Failed to set audio output device to HAL audio unit",err);
                }
            }
            else
            {
                printErrorOS("openAudio","Failed to get instance of output HAL component",err);
            }
        }
        else
        {
            printError("openAudio","Failed to find ouput HAL component");
        }
    }
    else
    {
        printError("openAudio","Failed to find output device");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::setSampleRateWhileOpeningCoreDevice(AudioDeviceID devID)
{
    tint freq = getFrequency();
    tint oldRate = setSampleRateIfPossible(devID,freq);

    if(oldRate > 0)
    {
        if(m_originalSampleRate < 0)
        {
            m_originalSampleRate = oldRate;
        }
        else if(m_originalSampleRate == freq)
        {
            m_originalSampleRate = -1;
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::resetSampleRateToOriginalWhenClosing(AudioDeviceID devID)
{
    if(m_originalSampleRate > 0)
    {
        if(setSampleRateIfPossible(devID,m_originalSampleRate) > 0)
        {
            m_originalSampleRate = -1;
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::setDeviceID(tint idIndex)
{
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pCoreDevice = getCurrentCoreAudioDevice();
    if(!pCoreDevice.isNull())
    {
        resetSampleRateToOriginalWhenClosing(pCoreDevice->deviceID());
        m_originalSampleRate = -1;
    }
    AOBase::setDeviceID(idIndex);
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::closeAudio()
{
    OSStatus err;

    if(m_isIntegerMode)
    {
        closeIntegerAudio();
    }
    else
    {
        if(m_outputUnit!=0)
        {
            const AOQueryCoreAudio::DeviceCoreAudio& dev = dynamic_cast<const AOQueryCoreAudio::DeviceCoreAudio &>(m_deviceInfo->device(m_deviceIdx));

            stopAudioDevice();

            if(m_hasExclusiveMode)
            {
                setExclusiveMode(dev.deviceID(), false);
                m_hasExclusiveMode = false;
            }

            if(m_flagInit)
            {
                err = AudioUnitUninitialize(m_outputUnit);
                if(err!=noErr)
                {
                    printErrorOS("closeAudio","Error uninitializing audio output unit",err);
                }

                removeListenerJackConnection(dev.deviceID());
                removeVolumeChangeNotification(dev.deviceID());
                m_flagInit = false;
            }

            AudioComponentInstanceDispose(m_outputUnit);
            m_outputUnit = 0;
        }
    }
    closeResampler();
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::startAudioDevice()
{
    bool res = false;

    if(m_isIntegerMode)
    {
        res = startIntegerAudio();
    }
    else
    {
        OSStatus err;

        if(m_flagInit)
        {
            err = AudioOutputUnitStart(m_outputUnit);
            if(err==noErr)
            {
                m_flagStart = true;
                res = true;
            }
            else
            {
                printErrorOS("startAudioDevice","Error starting audio output",err);
            }
        }
        else
        {
            printError("startAudioDevice","Audio output unit has not been initialized");
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::stopAudioDevice()
{
    if(m_isIntegerMode)
    {
        stopIntegerAudio();
    }
    else
    {
        if(m_flagStart)
        {
            OSStatus err;

            err = AudioOutputUnitStop(m_outputUnit);
            if(err!=noErr)
            {
                printErrorOS("stopAudioDevice","Error stopping audio output",err);
            }
            m_flagStart = false;
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::processMessages()
{
    if(m_stopTimeFlag)
    {
        if(m_flagStart)
        {
            tint delay;
            common::TimeStamp dT,cT;

            cT = common::TimeStamp::reference();
            dT = const_cast<const common::TimeStamp &>(m_stopTimeClock);
            if(dT > cT)
            {
                dT -= cT;
                delay = (dT.secondsTotal() * 1000) + dT.millisecond();
                if(delay>0)
                {
                    QTimer::singleShot(delay,this,SLOT(onStopProcess()));
                }
                else
                {
                    onStopProcess();
                }
            }
            else
            {
                onStopProcess();
            }
        }
        else
        {
            onStopProcess();
        }
        m_stopTimeFlag = false;
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::onStopProcess()
{
    stopCodec();
}

//-------------------------------------------------------------------------------------------

engine::AData *AOCoreAudioMacOS::allocateData(tint len,tint inChannel,tint outChannel)
{
    engine::RData *d = new engine::RData(len,inChannel,outChannel);
    return reinterpret_cast<engine::AData *>(d);
}

//-------------------------------------------------------------------------------------------

volatile bool AOCoreAudioMacOS::m_outputLatencyCalcFlag = false;
tint AOCoreAudioMacOS::m_outputLatencyBufferSize = 0;
volatile common::TimeStamp AOCoreAudioMacOS::m_outputLatencyTimeCore;

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::calcAudioLatency()
{
    UInt32 count,bufferSize = 0,propSize = 0;
    AudioDeviceID devId = kAudioDeviceUnknown;
    OSStatus err;

    AudioObjectPropertyAddress theAddress = { kAudioHardwarePropertyDefaultOutputDevice,
                                              kAudioObjectPropertyScopeGlobal,
                                              kAudioObjectPropertyElementMain };

    propSize = sizeof(devId);
    err = AudioObjectGetPropertyData(kAudioObjectSystemObject,&theAddress,0,0,&propSize,&devId);
    if(err==noErr)
    {
        AudioStreamBasicDescription streamFormat;

        ::memset(&streamFormat,0,sizeof(AudioStreamBasicDescription));
        streamFormat.mSampleRate = 44100.0;
        streamFormat.mFormatID = kAudioFormatLinearPCM;
        streamFormat.mFormatFlags = kAudioFormatFlagsNativeFloatPacked;
        streamFormat.mFramesPerPacket = 1;
        streamFormat.mBitsPerChannel = sizeof(tfloat32) * 8;
        streamFormat.mBytesPerPacket = sizeof(tfloat32) * 2;
        streamFormat.mBytesPerFrame  = sizeof(tfloat32) * 2;
        streamFormat.mChannelsPerFrame = 2;

        AudioObjectPropertyAddress propertyAddress;
        propertyAddress.mSelector = kAudioDevicePropertyStreamFormat;
        propertyAddress.mScope = kAudioDevicePropertyScopeOutput;
        propertyAddress.mElement = kAudioObjectPropertyElementMain;

        err = AudioObjectSetPropertyData(devId,&propertyAddress,0,0,sizeof(streamFormat),&streamFormat);
        if(err==noErr)
        {
            count = sizeof(bufferSize);

            AudioObjectPropertyAddress bufferAddress = { kAudioDevicePropertyBufferSize, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain};

            err = AudioObjectGetPropertyData(devId,&bufferAddress,0,0,&count,&bufferSize);
            if(err==noErr)
            {
                AudioDeviceIOProcID theIOProcID = 0;

                m_outputLatencyBufferSize = static_cast<tint>(bufferSize / streamFormat.mBytesPerFrame);

                err = AudioDeviceCreateIOProcID(devId,AOCoreAudioMacOS::callbackLatencyProc,0,&theIOProcID);
                if(err==kAudioHardwareNoError)
                {
                    err = AudioDeviceStart(devId,AOCoreAudioMacOS::callbackLatencyProc);
                    if(err==kAudioHardwareNoError)
                    {
                        while(!m_outputLatencyCalcFlag)
                        {
                            usleep(100);
                        }

                        AudioDeviceStop(devId,AOCoreAudioMacOS::callbackLatencyProc);
                    }
                    AudioDeviceDestroyIOProcID(devId,theIOProcID);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::callbackLatencyProc(AudioDeviceID id, \
                                          const AudioTimeStamp *inNow, \
                                          const AudioBufferList *inInputData, \
                                          const AudioTimeStamp *inInputTime, \
                                          AudioBufferList *outOutputData, \
                                          const AudioTimeStamp *inOutputTime, \
                                          void *inClientData)
{
    tint j,k,noChs;
    common::TimeStamp sT,oT;

    sT.nano64(AudioConvertHostTimeToNanos(inNow->mHostTime));
    oT.nano64(AudioConvertHostTimeToNanos(inOutputTime->mHostTime));
    if(oT > sT)
    {
        m_outputLatencyTimeCore.set(oT - sT);
    }
    else
    {
        common::TimeStamp blankT;
        m_outputLatencyTimeCore.set(blankT);
    }
    m_outputLatencyCalcFlag = true;

    for(tint i = 0; i < static_cast<int>(outOutputData->mNumberBuffers); i++)
    {
        tint noChs = static_cast<int>(outOutputData->mBuffers[i].mNumberChannels);
        ::memset(outOutputData->mBuffers[i].mData, 0 , m_outputLatencyBufferSize * noChs * sizeof(tfloat32));
    }
    return kAudioHardwareNoError;
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::corePropertyChangeProc(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddresses[],void *inClientData)
{
    AOCoreAudioMacOS *audioIO = reinterpret_cast<AOCoreAudioMacOS *>(inClientData);
    return audioIO->corePropertyChangeProcImpl(inObjectID,inNumberAddresses,inAddresses);
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::corePropertyChangeProcImpl(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddresses[])
{
    UInt32 i;

    for(i=0;i<inNumberAddresses;i++)
    {
        switch(inAddresses[i].mSelector)
        {
            case kAudioHardwarePropertyDevices:
                {
                    AudioEvent *e = new AudioEvent(AudioEvent::e_audioDeviceChangeEvent);
                    QCoreApplication::postEvent(this,e);
                }
                break;

            case kAudioDevicePropertyJackIsConnected:
                pause();
                break;

            default:
                break;
        }
    }
    return noErr;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::addListenerDevices()
{
    AudioObjectPropertyAddress propAddr;
    OSStatus err;

    if(!setupPropertyRunLoop())
    {
        printError("addListenerDevices","Error setting run loop correctly");
    }

    propAddr.mSelector = kAudioHardwarePropertyDevices;
    propAddr.mScope = kAudioObjectPropertyScopeGlobal;
    propAddr.mElement = kAudioObjectPropertyElementMain;

    err = AudioObjectAddPropertyListener(kAudioObjectSystemObject,&propAddr,AOCoreAudioMacOS::corePropertyChangeProc,this);
    if(err!=noErr)
    {
        printError("addListenerDevice","Error adding device hardware listener");
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::removeListenerDevices()
{
    AudioObjectPropertyAddress propAddr;
    OSStatus err;

    propAddr.mSelector = kAudioHardwarePropertyDevices;
    propAddr.mScope = kAudioObjectPropertyScopeGlobal;
    propAddr.mElement = kAudioObjectPropertyElementMain;

    err = AudioObjectRemovePropertyListener(kAudioObjectSystemObject,&propAddr,AOCoreAudioMacOS::corePropertyChangeProc,this);
    if(err!=noErr)
    {
        printError("addListenerDevice","Error removing device hardware listener");
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::addListenerJackConnection(AudioDeviceID devID)
{
    AudioObjectPropertyAddress propAddr;
    OSStatus err;

    propAddr.mSelector = kAudioDevicePropertyJackIsConnected;
    propAddr.mScope = kAudioDevicePropertyScopeOutput;
    propAddr.mElement = kAudioObjectPropertyElementMain;
    if(AudioObjectHasProperty(devID,&propAddr))
    {
        err = AudioObjectAddPropertyListener(devID,&propAddr,AOCoreAudioMacOS::corePropertyChangeProc,this);
        if(err!=noErr)
        {
            printError("addListenerDevice","Error adding headphone jack sensor");
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::removeListenerJackConnection(AudioDeviceID devID)
{
    AudioObjectPropertyAddress propAddr;
    OSStatus err;

    propAddr.mSelector = kAudioDevicePropertyJackIsConnected;
    propAddr.mScope = kAudioDevicePropertyScopeOutput;
    propAddr.mElement = kAudioObjectPropertyElementMain;
    if(AudioObjectHasProperty(devID,&propAddr))
    {
        err = AudioObjectRemovePropertyListener(devID,&propAddr,AOCoreAudioMacOS::corePropertyChangeProc,this);
        if(err!=noErr)
        {
            printError("addListenerDevice","Error removing headphone jack sensor");
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::audioDeviceChange()
{
    tint i,nDefaultIndex = -1;
    AOQueryCoreAudio *nDeviceInfo;
    QString defaultDeviceID,defaultDeviceName;
    bool res = false,found = false,pauseFlag = false;

    nDeviceInfo = new AOQueryCoreAudio;

    m_deviceInfoMutex.lock();
    if(nDeviceInfo->queryNames())
    {
        QSettings settings;

        for(i=0;i<nDeviceInfo->noDevices();i++)
        {
            nDeviceInfo->queryDevice(i);
        }

#if defined(OMEGA_DEBUG)
        nDeviceInfo->print();
#endif

        settings.beginGroup("audio");
        if(settings.contains(QString::fromLatin1("defaultDeviceID")))
        {
            defaultDeviceID = settings.value(QString::fromLatin1("defaultDeviceID"),QVariant(QString())).toString();
            if(!defaultDeviceID.isEmpty())
            {
                for(i=0;i<nDeviceInfo->noDevices() && !found;i++)
                {
                                        if(defaultDeviceID==nDeviceInfo->device(i).idConst())
                    {
                        nDefaultIndex = i;
                        found = true;
                    }
                }
            }
        }
        if(!found)
        {
            if(settings.contains(QString::fromLatin1("defaultDeviceName")))
            {
                defaultDeviceName = settings.value(QString::fromLatin1("defaultDeviceName"),QVariant(QString())).toString();
                if(!defaultDeviceName.isEmpty())
                {
                    for(i=0;i<nDeviceInfo->noDevices() && !found;i++)
                    {
                        nDefaultIndex = i;
                        found = true;
                    }
                }
            }
        }
        if(!found)
        {
            nDefaultIndex = nDeviceInfo->defaultDeviceIndex();
        }
        settings.endGroup();

        if(m_deviceInfo!=0)
        {
            if((m_deviceIdx>=0 && m_deviceIdx<m_deviceInfo->noDevices()) && (nDefaultIndex>=0 && nDefaultIndex<nDeviceInfo->noDevices()))
            {
                QString nDeviceID,cDeviceID;

                                cDeviceID = m_deviceInfo->device(m_deviceIdx).idConst();
                                nDeviceID = nDeviceInfo->device(nDefaultIndex).idConst();
                if(cDeviceID!=nDeviceID)
                {
                    pauseFlag = true;
                }
                res = true;
            }
        }
    }

    if(res)
    {
        if(m_deviceInfo!=0)
        {
            delete m_deviceInfo;
        }
        m_deviceInfo = nDeviceInfo;
        m_deviceIdx = nDefaultIndex;
        updateExclusiveAndIntegerModeOnDevices();
        QString devName = m_deviceInfo->device(m_deviceIdx).name();
        m_deviceInfo->deviceDirect(m_deviceIdx)->loadChannelMap();
    }
    else
    {
        delete nDeviceInfo;
    }
    m_deviceInfoMutex.unlock();

    if(res)
    {
        resetPlayback();
        if(pauseFlag)
        {
            pausePlayback(true);
        }
    }
}

//-------------------------------------------------------------------------------------------
// IOProc refactor
//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::callbackIOProc(void *inRegCon, \
                                     AudioUnitRenderActionFlags *ioActionFlags, \
                                     const AudioTimeStamp *inTimeStamp, \
                                     UInt32 inBusNumber, \
                                     UInt32 inNumberFrames, \
                                     AudioBufferList *ioData)
{
    AOCoreAudioMacOS *ao = reinterpret_cast<AOCoreAudioMacOS *>(inRegCon);
    return ao->callbackIOProcI(ioActionFlags,inTimeStamp,inBusNumber,inNumberFrames,ioData);
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::callbackIOProcI(AudioUnitRenderActionFlags *ioActionFlags, \
                                      const AudioTimeStamp *inTimeStamp, \
                                      UInt32 inBusNumber, \
                                      UInt32 inNumberFrames, \
                                      AudioBufferList *ioData)
{
    IOTimeStamp systemTime;
    AudioHardwareBufferCoreAudio buffer(ioData,inNumberFrames,sizeof(tfloat32));
    systemTime = createIOTimeStamp(inTimeStamp);
    writeToAudioIOCallback(&buffer,systemTime);
    return noErr;
}

//-------------------------------------------------------------------------------------------

IOTimeStamp AOCoreAudioMacOS::createIOTimeStamp(const AudioTimeStamp *sysTime) const
{
    common::TimeStamp tS;
    tS.nano64(AudioConvertHostTimeToNanos(sysTime->mHostTime));
    return IOTimeStamp(true,tS);
}

//-------------------------------------------------------------------------------------------
// HAL Integer
//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::setupPropertyRunLoop()
{
    bool res;
    OSStatus err;
    CFRunLoopRef runLoop = 0;
    AudioObjectPropertyAddress property = { kAudioHardwarePropertyRunLoop, kAudioObjectPropertyScopeGlobal,kAudioObjectPropertyElementMain };

    err = CoreAudioIF::instance()->AudioObjectSetPropertyData(kAudioObjectSystemObject,&property,0,0,sizeof(CFRunLoopRef),
        reinterpret_cast<const void *>(&runLoop));
    if(err==noErr)
    {
        res = true;
    }
    else
    {
        printErrorOS("setupPropertyRunLoop","Error setting run loop",err);
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

pid_t AOCoreAudioMacOS::getCurrentProcessID() const
{
    return getpid();
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::isExclusiveModeIfAvailable(AudioDeviceID devId)
{
    bool res = false;
    AudioObjectPropertyAddress property = { kAudioDevicePropertyHogMode, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };

    if(CoreAudioIF::instance()->AudioObjectHasProperty(devId,&property))
    {
        OSStatus err;
        Boolean settableFlag = false;

        err = CoreAudioIF::instance()->AudioObjectIsPropertySettable(devId,&property,&settableFlag);
        if(err==noErr && settableFlag)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::setExclusiveMode(AudioDeviceID devID, bool isExcl)
{
    bool res = false;
    AudioObjectPropertyAddress property = { kAudioDevicePropertyHogMode, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };

    if(isExclusiveModeIfAvailable(devID))
    {
            OSStatus err;
            pid_t processID, cProcessID;
            UInt32 propSize = sizeof(pid_t);

            err = CoreAudioIF::instance()->AudioObjectGetPropertyData(devID,&property,0,0,&propSize,reinterpret_cast<void *>(&processID));
            if(err==noErr)
            {
                cProcessID = getCurrentProcessID();
                if(processID==cProcessID)
                {
                    if(!isExcl)
                    {
                        cProcessID = -1;
                        err = CoreAudioIF::instance()->AudioObjectSetPropertyData(devID,&property,0,0,propSize,reinterpret_cast<const void *>(&cProcessID));
                        res = (err == noErr) ? true : false;
                    }
                }
                else if(isExcl)
                {
                    err = CoreAudioIF::instance()->AudioObjectSetPropertyData(devID,&property,0,0,propSize,reinterpret_cast<const void *>(&cProcessID));
                    res = (err == noErr) ? true : false;
                }
            }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::disableMixingIfPossible(AudioDeviceID devID)
{
    AudioObjectPropertyAddress property = { kAudioDevicePropertySupportsMixing, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };
    bool res = false;

    if(CoreAudioIF::instance()->AudioObjectHasProperty(devID,&property))
    {
        OSStatus err;
        Boolean writeable = false;

        err = CoreAudioIF::instance()->AudioObjectIsPropertySettable(devID,&property,&writeable);
        if(err==noErr)
        {
            if(writeable)
            {
                UInt32 paramSize = 0;

                err = CoreAudioIF::instance()->AudioObjectGetPropertyDataSize(devID,&property,0,0,&paramSize);
                if(err==noErr)
                {
                    UInt32 mix = 0;

                    err = CoreAudioIF::instance()->AudioObjectGetPropertyData(devID,&property,0,0,&paramSize,&mix);
                    if(err==noErr)
                    {
                        if(mix)
                        {
                            mix = 0;
                            err = CoreAudioIF::instance()->AudioObjectSetPropertyData(devID,&property,0,0,paramSize,&mix);
                            if(err==noErr)
                            {
                                res = true;
                            }
                            else
                            {
                                printErrorOS("disableMixingIfPossible","Error disabling mixing property on audio device",err);
                            }
                        }
                    }
                    else
                    {
                        printErrorOS("disableMixingIfPossible","Error getting mixing property",err);
                    }
                }
                else
                {
                    printErrorOS("disableMixingIfPossible","Error getting mixing property size",err);
                }
            }
        }
        else
        {
            printErrorOS("disableMixingIfPossible","Error getting if mixing property is writeable",err);
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::reEnableMixing(AudioDeviceID devID)
{
    AudioObjectPropertyAddress property = { kAudioDevicePropertySupportsMixing, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };

    if(CoreAudioIF::instance()->AudioObjectHasProperty(devID,&property))
    {
        OSStatus err;
        Boolean writeable = false;

        err = CoreAudioIF::instance()->AudioObjectIsPropertySettable(devID,&property,&writeable);
        if(err==noErr)
        {
            if(writeable)
            {
                UInt32 paramSize = 0;

                err = CoreAudioIF::instance()->AudioObjectGetPropertyDataSize(devID,&property,0,0,&paramSize);
                if(err==noErr)
                {
                    UInt32 mix = 1;

                    err = CoreAudioIF::instance()->AudioObjectSetPropertyData(devID,&property,0,0,paramSize,&mix);
                    if(err!=noErr)
                    {
                        printErrorOS("reEnableMixing","Failed to re-enable mixing on audio device",err);
                    }
                }
                else
                {
                    printErrorOS("reEnableMixing","Error getting mixing property size",err);
                }
            }
        }
        else
        {
            printErrorOS("reEnableMixing","Error getting if mixing property is writeable",err);
        }
    }
}

//-------------------------------------------------------------------------------------------

QVector<AudioStreamID> AOCoreAudioMacOS::getAudioStreamsForDevice(AudioDeviceID devID)
{
    OSStatus err;
    UInt32 paramSize = 0;
    QVector<AudioStreamID> streamIDs;
    AudioObjectPropertyAddress property = { kAudioDevicePropertyStreams, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };

    err = CoreAudioIF::instance()->AudioObjectGetPropertyDataSize(devID,&property,0,0,&paramSize);
    if(err==noErr)
    {
        tint noStreams = static_cast<tint>(paramSize) / sizeof(AudioStreamID);

        if(noStreams > 0)
        {
            AudioStreamID *pStreams = new AudioStreamID [noStreams];

            err = CoreAudioIF::instance()->AudioObjectGetPropertyData(devID,&property,0,0,&paramSize,pStreams);
            if(err==noErr)
            {
                for(tint i=0;i<noStreams;i++)
                {
                    streamIDs.append(pStreams[i]);
                }
            }
            else
            {
                printErrorOS("getAudioStreamsForDevice","Error getting audio streams for device",err);
            }
            delete [] pStreams;
        }
        else
        {
            printError("getAudioStreamsForDevice","No audio streams available for device");
        }
    }
    else
    {
        printErrorOS("getAudioStreamsForDevice","Error getting number of audio streams for device",err);
    }
    return streamIDs;
}

//-------------------------------------------------------------------------------------------

QVector<AudioStreamRangedDescription> AOCoreAudioMacOS::getAudioStreamDescriptions(AudioStreamID streamID)
{
    OSStatus err;
    UInt32 paramSize = 0;
    QVector<AudioStreamRangedDescription> streams;
    AudioObjectPropertyAddress property = { kAudioStreamPropertyAvailablePhysicalFormats, kAudioObjectPropertyScopeGlobal, 0 };

    err = CoreAudioIF::instance()->AudioObjectGetPropertyDataSize(streamID,&property,0,0,&paramSize);
    if(err==noErr)
    {
        tint noFormats = static_cast<tint>(paramSize) / sizeof(AudioStreamRangedDescription);
        if(noFormats>0)
        {
            AudioStreamRangedDescription *descriptions = new AudioStreamRangedDescription [noFormats];

            err = CoreAudioIF::instance()->AudioObjectGetPropertyData(streamID,&property,0,0,&paramSize,descriptions);
            if(err==noErr)
            {
                for(tint i=0;i<noFormats;i++)
                {
                    streams.append(descriptions[i]);
                }
            }
            else
            {
                printErrorOS("getAudioStreamDescriptions","Failed to get formats for stream ID",err);
            }

            delete [] descriptions;
        }
        else
        {
            printError("getAudioStreamDescriptions","No formats available for given stream");
        }
    }
    else
    {
        printErrorOS("getAudioStreamDescriptions","Error getting number of physical stream formats",err);
    }
    return streams;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::isFormatDataTypeCorrisponding(const AudioStreamBasicDescription& format,const FormatDescription& desc)
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
        else if(!((format.mFormatFlags & kAudioFormatFlagIsFloat) || (format.mFormatFlags & kAudioFormatFlagIsSignedInteger)) && desc.typeOfData()==FormatDescription::e_DataUnsignedInteger)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QVector<int> AOCoreAudioMacOS::findFrequenciesFromRange(const AudioStreamRangedDescription& range,bool useRange)
{
    QVector<int> frequencies;

    if(useRange)
    {
        m_deviceInfoMutex.lock();
        if(m_deviceIdx>=0 && m_deviceIdx<getDeviceInfo()->noDevices())
        {
            const QSet<int>& freqs = getDeviceInfo()->device(m_deviceIdx).frequencies();

            for(QSet<int>::const_iterator ppI=freqs.constBegin();ppI!=freqs.constEnd();ppI++)
            {
                tfloat64 f = static_cast<int>(*ppI);

                if(f>=range.mSampleRateRange.mMinimum && f<=range.mSampleRateRange.mMaximum)
                {
                    frequencies.append(*ppI);
                }
            }
        }
        m_deviceInfoMutex.unlock();
    }
    else
    {
        frequencies.append(range.mFormat.mSampleRate);
    }
    return frequencies;
}

//-------------------------------------------------------------------------------------------

QVector<FormatDescription> AOCoreAudioMacOS::formatDescriptionsFromRanged(const AudioStreamRangedDescription& range,bool useRange)
{
    QVector<FormatDescription> descriptions;
    QVector<int> frequencies = findFrequenciesFromRange(range,useRange);

    for(QVector<int>::iterator ppI=frequencies.begin();ppI!=frequencies.end();ppI++)
    {
        FormatDescription desc;

        if(range.mFormat.mFormatID==kAudioFormatLinearPCM)
        {
            if(range.mFormat.mFormatFlags & kAudioFormatFlagIsFloat)
            {
                desc.setTypeOfData(FormatDescription::e_DataFloatSingle);
            }
            else if(range.mFormat.mFormatFlags & kAudioFormatFlagIsSignedInteger)
            {
                desc.setTypeOfData(FormatDescription::e_DataSignedInteger);
            }
            else
            {
                desc.setTypeOfData(FormatDescription::e_DataUnsignedInteger);
            }

            if(desc.setNumberOfBits(range.mFormat.mBitsPerChannel))
            {
                if(desc.setNumberOfChannels(range.mFormat.mChannelsPerFrame))
                {
                    if(desc.setFrequency(*ppI))
                    {
                        if(isFormatDataTypeCorrisponding(range.mFormat,desc))
                        {
                            descriptions.append(desc);
                        }
                    }
                }
            }
        }
    }
    return descriptions;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::areDescriptionsEquivalent(const AudioStreamBasicDescription& format,const FormatDescription& desc)
{
    bool res = false;

    if(format.mFormatID==kAudioFormatLinearPCM)
    {
        if(isFormatDataTypeCorrisponding(format,desc))
        {
            if(desc.bits()==format.mBitsPerChannel && desc.channels()==format.mChannelsPerFrame && desc.frequency()==static_cast<tint>(format.mSampleRate))
            {
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

FormatsSupported AOCoreAudioMacOS::getSupportedFormatsForStreams(const QVector<AudioStreamID>& streamIDs)
{
    FormatsSupported supported;

    for(QVector<AudioStreamID>::const_iterator ppI=streamIDs.constBegin();ppI!=streamIDs.constEnd();ppI++)
    {
        AudioStreamID ID = *ppI;
        QVector<AudioStreamRangedDescription> streamList;

        streamList = getAudioStreamDescriptions(ID);
        for(QVector<AudioStreamRangedDescription>::const_iterator ppJ=streamList.constBegin();ppJ!=streamList.constEnd();ppJ++)
        {
            const AudioStreamRangedDescription& range = *ppJ;

            if(range.mFormat.mFormatFlags & kAudioFormatFlagIsNonMixable)
            {
                QVector<FormatDescription> descs = formatDescriptionsFromRanged(range,false);
                for(QVector<FormatDescription>::iterator ppK=descs.begin();ppK!=descs.end();ppK++)
                {
                    supported.add(*ppK);
                }
            }
        }
    }
    return supported;
}

//-------------------------------------------------------------------------------------------

QPair<AudioStreamID,AudioStreamBasicDescription *> AOCoreAudioMacOS::findClosestStream(const FormatDescription& sourceDesc,const QVector<AudioStreamID>& streamIDs)
{
    FormatsSupported supported;
    FormatDescription closestDesc;
    QPair<AudioStreamID,AudioStreamBasicDescription *> streamID(kAudioObjectUnknown,0);

    supported = getSupportedFormatsForStreams(streamIDs);

    if(FormatDescriptionUtils::findClosestFormatType(sourceDesc,supported,closestDesc))
    {
        for(QVector<AudioStreamID>::const_iterator ppI=streamIDs.constBegin();ppI!=streamIDs.constEnd();ppI++)
        {
            AudioStreamID ID = *ppI;
            QVector<AudioStreamRangedDescription> streamList;

            streamList = getAudioStreamDescriptions(ID);
            for(QVector<AudioStreamRangedDescription>::const_iterator ppJ=streamList.constBegin();ppJ!=streamList.constEnd();ppJ++)
            {
                const AudioStreamRangedDescription& range = *ppJ;

                if(areDescriptionsEquivalent(range.mFormat,closestDesc) && (range.mFormat.mFormatFlags & kAudioFormatFlagIsNonMixable))
                {
                    streamID.first = ID;
                    streamID.second = new AudioStreamBasicDescription;
                    memcpy(streamID.second,&range.mFormat,sizeof(AudioStreamBasicDescription));
                }
            }
        }
    }
    return streamID;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::isConvertionAlignedHigh(const AudioStreamBasicDescription& format) const
{
    bool alignHigh = false;
    tint packedBytesPerFrame = (format.mBitsPerChannel >> 3) * format.mChannelsPerFrame;

    if(packedBytesPerFrame < format.mBytesPerFrame)
    {
        if(format.mFormatFlags & kAudioFormatFlagIsBigEndian)
        {
            alignHigh = (format.mFormatFlags & kAudioFormatFlagIsAlignedHigh) ? true : false;
        }
        else
        {
            alignHigh = (format.mFormatFlags & kAudioFormatFlagIsAlignedHigh) ? false : true;
        }
    }
    return alignHigh;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<SampleConverter> AOCoreAudioMacOS::sampleConverterFromDescription(const AudioStreamBasicDescription& format)
{
    bool isLittleEndian,isAlignHigh;
    QSharedPointer<SampleConverter> pConverter;

    isLittleEndian = (format.mFormatFlags & kAudioFormatFlagIsBigEndian) ? false : true;
    isAlignHigh = isConvertionAlignedHigh(format);

    if(format.mFormatFlags & kAudioFormatFlagIsFloat)
    {
        QSharedPointer<SampleConverter> nConverter(new SampleConverter(true,isLittleEndian));
        pConverter = nConverter;
    }
    else
    {
        tint noBits = format.mBitsPerChannel;
        tint bytesPerSample = format.mBytesPerFrame / format.mChannelsPerFrame;
        bool isSigned = (format.mFormatFlags & kAudioFormatFlagIsSignedInteger) ? true : false;
        QSharedPointer<SampleConverter> nConverter(new SampleConverter(noBits,bytesPerSample,isLittleEndian,isAlignHigh,isSigned));
        pConverter = nConverter;
    }
    return pConverter;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::formatFromStreamDescription(const AudioStreamBasicDescription& format,FormatDescription& desc)
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

        if(desc.setTypeOfData(type))
        {
            if(desc.setNumberOfBits(format.mBitsPerChannel))
            {
                if(desc.setNumberOfChannels(format.mChannelsPerFrame))
                {
                    if(desc.setFrequency(format.mSampleRate))
                    {
                        res = true;
                    }
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint AOCoreAudioMacOS::audioStreamWaitTimeout() const
{
    return 500;
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::audioStreamChangeListener(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddress[],void *inClientData)
{
    OSStatus err = noErr;
    AOCoreAudioMacOS *pAudio = reinterpret_cast<AOCoreAudioMacOS *>(inClientData);

    if(pAudio!=0)
    {
        err = pAudio->audioStreamChangeListenerImpl(inObjectID,inNumberAddresses,inAddress);
    }
    return err;
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::audioStreamChangeListenerImpl(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddress[])
{
    for(UInt32 i=0;i<inNumberAddresses;i++)
    {
        if(inAddress[i].mSelector == kAudioStreamPropertyPhysicalFormat)
        {
            m_audioStreamCondition.wakeAll();
        }
    }
    return noErr;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::setAudioStream(AudioStreamID streamID,const AudioStreamBasicDescription& format)
{
    FormatDescription desc;
    bool res = false;

    if(formatFromStreamDescription(format,desc))
    {
        OSStatus err;
        AudioObjectPropertyAddress property = { kAudioStreamPropertyPhysicalFormat, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };

        err = CoreAudioIF::instance()->AudioObjectAddPropertyListener(streamID,&property,AOCoreAudioMacOS::audioStreamChangeListener,reinterpret_cast<void *>(this));
        if(err==noErr)
        {
            m_audioStreamMutex.lock();

            err = CoreAudioIF::instance()->AudioObjectSetPropertyData(streamID,&property,0,0,sizeof(AudioStreamBasicDescription),&format);
            if(err==noErr)
            {
                AudioStreamBasicDescription actualFormat;

                for(tint i=0;i<9 && !res;i++)
                {
                    UInt32 paramSize = sizeof(AudioStreamBasicDescription);

                    if(i > 0)
                    {
                        m_audioStreamCondition.wait(&m_audioStreamMutex,audioStreamWaitTimeout());
                    }
                    err = CoreAudioIF::instance()->AudioObjectGetPropertyData(streamID,&property,0,0,&paramSize,&actualFormat);
                    if(err==noErr)
                    {
                        FormatDescription actualDesc;

                        if(formatFromStreamDescription(actualFormat,actualDesc))
                        {
                            if(desc==actualDesc)
                            {
                                res = true;
                            }
                        }
                    }
                    else
                    {
                        printErrorOS("setAudioStream","Error getting physical format for device",err);
                    }
                }

            }
            else
            {
                printErrorOS("setAudioStream","Could not set the stream format",err);
            }

            m_audioStreamMutex.unlock();

            err = CoreAudioIF::instance()->AudioObjectRemovePropertyListener(streamID,&property,AOCoreAudioMacOS::audioStreamChangeListener,reinterpret_cast<void *>(this));
            if(err!=noErr)
            {
                printErrorOS("setAudioStream","Failed to remove property listener",err);
                res = false;
            }
        }
        else
        {
            printErrorOS("setAudioStream","Failed to attach property listener",err);
        }
    }
    else
    {
        printError("setAudioStream","Incompatible audio format");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

AudioStreamBasicDescription *AOCoreAudioMacOS::saveAudioDescriptionForStream(AudioStreamID streamID)
{
    OSStatus err;
    UInt32 paramSize = sizeof(AudioStreamBasicDescription);
    AudioObjectPropertyAddress property = { kAudioStreamPropertyPhysicalFormat, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };
    AudioStreamBasicDescription *format = new AudioStreamBasicDescription;

    memset(format,0,sizeof(AudioStreamBasicDescription));

    err = CoreAudioIF::instance()->AudioObjectGetPropertyData(streamID,&property,0,0,&paramSize,format);
    if(err!=noErr)
    {
        printErrorOS("saveAudioDescriptionForStream","Error getting current audio stream description",err);
        delete format;
        format = 0;
    }
    return format;
}

//-------------------------------------------------------------------------------------------

tint AOCoreAudioMacOS::setSampleRateIfPossible(AudioDeviceID devID,int sampleRate)
{
    tint oldRate = -1;
    OSStatus err;
    Boolean settable = false;
    AudioObjectPropertyAddress property = { kAudioDevicePropertyNominalSampleRate, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };

    if(CoreAudioIF::instance()->AudioObjectHasProperty(devID,&property))
    {
        err = CoreAudioIF::instance()->AudioObjectIsPropertySettable(devID,&property,&settable);
        if(err==noErr)
        {
            if(settable)
            {
                Float64 oRate;
                UInt32 paramSize = sizeof(Float64);

                err = CoreAudioIF::instance()->AudioObjectGetPropertyData(devID,&property,0,0,&paramSize,&oRate);
                if(err==noErr)
                {
                    oldRate = static_cast<tint>(oRate);

                    if(oldRate!=sampleRate)
                    {
                        Float64 sRate = static_cast<tint>(sampleRate);
                        err = CoreAudioIF::instance()->AudioObjectSetPropertyData(devID,&property,0,0,paramSize,reinterpret_cast<const void *>(&sRate));
                        if(err!=noErr)
                        {
                            QString eMsg;
                            eMsg = "Failed to set sample rate to " + QString::number(sampleRate);
                            printErrorOS("setSampleRateIfPossible",eMsg.toUtf8().constData(),err);
                        }
                    }
                    else
                    {
                        oldRate = -1;
                    }
                }
                else
                {
                    printErrorOS("setSampleRateIfPossible","Failed to get sample rate from device",err);
                }
            }
        }
        else
        {
            printErrorOS("setSampleRateIfPossible","Error getting if sample rate is settable for device",err);
        }
    }
    return oldRate;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<AOQueryDevice::Device> AOCoreAudioMacOS::copyDeviceInformation(const AOQueryDevice::Device& iDevice)
{
    QSharedPointer<AOQueryDevice::Device> pDevice(new AOQueryCoreAudio::DeviceCoreAudio(iDevice));
    return pDevice;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::openIntegerAudio(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pDevice)
{
    QVector<AudioStreamID> streamList;
    bool res = false;

    if(isDeviceAlive(pDevice))
    {
        m_integerDeviceID = pDevice->deviceID();

        m_hasExclusiveMode = (pDevice->hasExclusive()) ? setExclusiveMode(pDevice->deviceID(), true) : false;
        m_hasMixingBeenDisabled = disableMixingIfPossible(pDevice->deviceID());

        streamList = getAudioStreamsForDevice(pDevice->deviceID());
        if(!streamList.isEmpty())
        {
            QPair<AudioStreamID,AudioStreamBasicDescription *> closestStream;
            FormatDescription sourceDescription = getSourceDescription(pDevice->noChannels());

            closestStream = findClosestStream(sourceDescription,streamList);
            if(closestStream.first!=kAudioObjectUnknown && closestStream.second!=0)
            {
                m_pSampleConverter = sampleConverterFromDescription(*closestStream.second);
                if(!m_pSampleConverter.isNull() && m_pSampleConverter->isSupported())
                {
                    int oFreq = closestStream.second->mSampleRate;

                    // All configuration data is now obtained, so open the device.
                    setSampleRateWhileOpeningCoreDevice(pDevice->deviceID());

                    if(getFrequency() != oFreq)
                    {
                        int iFreq = getFrequency();
                        setFrequency(oFreq);
                        initResampler(iFreq,getFrequency());
                    }

                    initCyclicBuffer();

                    // Why is the old stream ID being set from the closest found stream ID?
                    m_oldStreamID = closestStream.first;
                    m_oldStreamDescription = saveAudioDescriptionForStream(closestStream.first);

                    if(setAudioStream(closestStream.first,*(closestStream.second)))
                    {
                        OSStatus err;

                        m_pIntegerDeviceIOProcID = new AudioDeviceIOProcID;
                        err = CoreAudioIF::instance()->AudioDeviceCreateIOProcID(pDevice->deviceID(),AOCoreAudioMacOS::IOProcInteger,reinterpret_cast<void *>(this),m_pIntegerDeviceIOProcID);
                        if(err==noErr)
                        {
                            addListenerJackConnection(pDevice->deviceID());
                            addVolumeChangeNotification(pDevice->deviceID());
                            m_flagInit = true;
                            res = true;
                        }
                        else
                        {
                            printErrorOS("openIntegerAudio","Failed to create I/O playback callback",err);
                            delete m_pIntegerDeviceIOProcID;
                            m_pIntegerDeviceIOProcID = 0;
                        }
                    }
                    else
                    {
                        printError("openIntegerAudio","Failed to setup physical format on audio stream");
                    }
                }
                else
                {
                    printError("openIntegerAudio","Closest audio format not supported");
                }
            }
            else
            {
                printError("openIntegerAudio","Failed to find format for device");
            }
        }
        else
        {
            printError("openIntegerAudio","Failed to get list of audio streams for device");
        }
    }
    else
    {
        printError("openIntegerAudio","Device is not alive");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::closeIntegerAudio()
{
    OSStatus err;

    if(m_integerDeviceID!=kAudioObjectUnknown)
    {
        removeListenerJackConnection(m_integerDeviceID);
        removeVolumeChangeNotification(m_integerDeviceID);

        if(m_pIntegerDeviceIOProcID!=0)
        {
            err = CoreAudioIF::instance()->AudioDeviceDestroyIOProcID(m_integerDeviceID,*m_pIntegerDeviceIOProcID);
            if(err!=noErr)
            {
                printErrorOS("closeIntegerAudio","Error removing I/O playback callback",err);
            }
            delete m_pIntegerDeviceIOProcID;
            m_pIntegerDeviceIOProcID = 0;
        }

        if(m_oldStreamID!=kAudioObjectUnknown && m_oldStreamDescription!=0)
        {
            if(!setAudioStream(m_oldStreamID,*m_oldStreamDescription))
            {
                printError("closeIntegerAudio","Error restoring original audio stream");
            }
            delete m_oldStreamDescription;
            m_oldStreamDescription = 0;
        }

        m_pSampleConverter.clear();

        if(m_hasMixingBeenDisabled)
        {
            reEnableMixing(m_integerDeviceID);
            m_hasMixingBeenDisabled = false;
        }

        if(m_hasExclusiveMode)
        {
            setExclusiveMode(m_integerDeviceID, false);
            m_hasExclusiveMode = false;
        }

        m_integerDeviceID = kAudioObjectUnknown;
        m_flagInit = false;
    }
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::startIntegerAudio()
{
    bool res = false;

    if(m_flagInit)
    {
        OSStatus err;

        err = CoreAudioIF::instance()->AudioDeviceStart(m_integerDeviceID,*m_pIntegerDeviceIOProcID);
        if(err==noErr)
        {
            m_flagStart = true;
            res = true;
        }
        else
        {
            printErrorOS("startIntegerAudio","Error starting audio integer output",err);
        }
    }
    else
    {
        printError("startIntegerAudio","Audio output device has not been setup");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::stopIntegerAudio()
{
    if(m_flagStart)
    {
        OSStatus err;

        err = CoreAudioIF::instance()->AudioDeviceStop(m_integerDeviceID,*m_pIntegerDeviceIOProcID);
        if(err!=noErr)
        {
            printErrorOS("stopIntegerAudio","Error stopping audio integer output",err);
        }
        m_flagStart = false;
    }
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::IOProcInteger(AudioDeviceID ioDevice,
                                    const AudioTimeStamp *inNow,
                                    const AudioBufferList *inInputData,
                                    const AudioTimeStamp *inInputTime,
                                    AudioBufferList *outOutputData,
                                    const AudioTimeStamp *inOutputTime,
                                    void *inClientData)
{
    AOCoreAudioMacOS *pAudio = reinterpret_cast<AOCoreAudioMacOS *>(inClientData);
    return pAudio->IntegerIOProcImpl(ioDevice,inNow,inInputData,inInputTime,outOutputData,inOutputTime);
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::IntegerIOProcImpl(AudioDeviceID ioDevice,
                                        const AudioTimeStamp *inNow,
                                        const AudioBufferList *inInputData,
                                        const AudioTimeStamp *inInputTime,
                                        AudioBufferList *outOutputData,
                                        const AudioTimeStamp *inOutputTime)
{
    IOTimeStamp systemTime;
    UInt32 inNumberFrames = outOutputData->mBuffers[0].mDataByteSize / (outOutputData->mBuffers[0].mNumberChannels * m_pSampleConverter->bytesPerSample());
    AudioHardwareBufferCoreAudio buffer(outOutputData,inNumberFrames,m_pSampleConverter->bytesPerSample());
    systemTime = createIOTimeStamp(inOutputTime);
    writeToAudioIOCallback(&buffer,systemTime);
    return noErr;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::writeToAudioOutputBufferFromPartData(AbstractAudioHardwareBuffer *pBuffer,
                                                       const engine::RData *data,
                                                       tint partNumber,
                                                       tint inChannelIndex,
                                                       tint bufferIndex,
                                                       tint outChannelIndex,
                                                       tint inputSampleIndex,
                                                       tint outputSampleIndex,
                                                       tint amount)
{
    const sample_t *input = data->partDataOutConst(partNumber);

    engine::CodecDataType dType;
    tint noInputChannels;
    tint noOutputChannels = pBuffer->numberOfChannelsInBuffer(bufferIndex);

    tint iIdx;
    tint oIdx = (outputSampleIndex * noOutputChannels) + outChannelIndex;

    if(inChannelIndex >= 0)
    {
        input = data->partDataOutConst(partNumber);
        noInputChannels = data->noOutChannels();
        iIdx = (inputSampleIndex * noInputChannels) + inChannelIndex;
        dType = data->partGetDataType(partNumber);
    }
    else
    {
        if(inChannelIndex == engine::e_lfeChannelIndex)
        {
            input = data->partFilterDataConst(partNumber, engine::e_lfeChannelIndex);
        }
        else
        {
            input = data->partDataCenterConst(partNumber);
        }
        iIdx = inputSampleIndex;
        noInputChannels = 1;
        dType = engine::e_SampleFloat;
    }

    if(m_isIntegerMode)
    {
        tbyte *out = reinterpret_cast<tbyte *>(pBuffer->buffer(bufferIndex));
        out += oIdx * m_pSampleConverter->bytesPerSample();

        m_pSampleConverter->setNumberOfInputChannels(noInputChannels);
        m_pSampleConverter->setNumberOfOutputChannels(noOutputChannels);
        if(m_isDeviceVolume)
        {
            m_pSampleConverter->setVolume(c_plusOneSample);
        }
        else
        {
            m_pSampleConverter->setVolume(m_volume);
        }
        m_pSampleConverter->convertAtIndex(input,iIdx,out,amount,dType);
    }
    else
    {
        tfloat32 *out = reinterpret_cast<tfloat32 *>(pBuffer->buffer(bufferIndex));
        tint tAmount = oIdx + (amount * noOutputChannels);

        if(dType == engine::e_SampleInt16)
        {
            writeToAudioFromInt16(input, iIdx, out, oIdx, tAmount, noInputChannels, noOutputChannels);
        }
        else if(dType == engine::e_SampleInt24)
        {
            writeToAudioFromInt24(input, iIdx, out, oIdx, tAmount, noInputChannels, noOutputChannels);
        }
        else if(dType == engine::e_SampleInt32)
        {
            writeToAudioFromInt32(input, iIdx, out, oIdx, tAmount, noInputChannels, noOutputChannels);
        }
        else
        {
            writeToAudioFromFloat(input, iIdx, out, oIdx, tAmount, noInputChannels, noOutputChannels);
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::writeToAudioFromFloat(const sample_t *input, tint iIdx, tfloat32 *output, tint oIdx, tint tAmount, tint noInputChannels, tint noOutputChannels)
{
    if(m_isDeviceVolume)
    {
        while(oIdx < tAmount)
        {
#if defined(SINGLE_FLOAT_SAMPLE)
            output[oIdx] = input[iIdx];
#else
            output[oIdx] = engine::sampleToFloat32(input[iIdx]);
#endif
            iIdx += noInputChannels;
            oIdx += noOutputChannels;
        }
    }
    else
    {
        while(oIdx < tAmount)
        {
#if defined(SINGLE_FLOAT_SAMPLE)
            output[oIdx] = input[iIdx] * m_volume;
#else
            output[oIdx] = engine::sampleToFloat32(input[iIdx] * m_volume);
#endif
            iIdx += noInputChannels;
            oIdx += noOutputChannels;
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::writeToAudioFromInt16(const sample_t *in, tint iIdx, tfloat32 *output, tint oIdx, tint tAmount, tint noInputChannels, tint noOutputChannels)
{
    const tint16 *input = reinterpret_cast<const tint16 *>(in);

    if(m_isDeviceVolume)
    {
        while(oIdx < tAmount)
        {
            output[oIdx] = engine::sample32From16Bit(input[iIdx]);
            iIdx += noInputChannels;
            oIdx += noOutputChannels;
        }
    }
    else
    {
        while(oIdx < tAmount)
        {
            output[oIdx] = engine::sample32From16Bit(input[iIdx]) * static_cast<tfloat32>(m_volume);
            iIdx += noInputChannels;
            oIdx += noOutputChannels;
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::writeToAudioFromInt24(const sample_t *in, tint iIdx, tfloat32 *output, tint oIdx, tint tAmount, tint noInputChannels, tint noOutputChannels)
{
    const tint32 *input = reinterpret_cast<const tint32 *>(in);

    if(m_isDeviceVolume)
    {
        while(oIdx < tAmount)
        {
            output[oIdx] = engine::sample32From24Bit(input[iIdx]);
            iIdx += noInputChannels;
            oIdx += noOutputChannels;
        }
    }
    else
    {
        while(oIdx < tAmount)
        {
            output[oIdx] = engine::sample32From24Bit(input[iIdx]) * static_cast<tfloat32>(m_volume);
            iIdx += noInputChannels;
            oIdx += noOutputChannels;
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::writeToAudioFromInt32(const sample_t *in, tint iIdx, tfloat32 *output, tint oIdx, tint tAmount, tint noInputChannels, tint noOutputChannels)
{
    const tint32 *input = reinterpret_cast<const tint32 *>(in);

    if(m_isDeviceVolume)
    {
        while(oIdx < tAmount)
        {
            output[oIdx] = engine::sample32From32Bit(input[iIdx]);
            iIdx += noInputChannels;
            oIdx += noOutputChannels;
        }
    }
    else
    {
        while(oIdx < tAmount)
        {
            output[oIdx] = engine::sample32From32Bit(input[iIdx]) * static_cast<tfloat32>(m_volume);
            iIdx += noInputChannels;
            oIdx += noOutputChannels;
        }
    }
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::canDeviceSupportIntegerMode(AudioDeviceID devID)
{
    QVector<AudioStreamID> streamList;
    bool res = false;

    streamList = getAudioStreamsForDevice(devID);
    if(!streamList.isEmpty())
    {
        for(QVector<AudioStreamID>::const_iterator ppI=streamList.constBegin();ppI!=streamList.constEnd() && !res;ppI++)
        {
            AudioStreamID ID = *ppI;
            QVector<AudioStreamRangedDescription> streamList;

            streamList = getAudioStreamDescriptions(ID);
            for(QVector<AudioStreamRangedDescription>::const_iterator ppJ=streamList.constBegin();ppJ!=streamList.constEnd() && !res;ppJ++)
            {
                const AudioStreamRangedDescription& range = *ppJ;

                if((range.mFormat.mFormatFlags & kAudioFormatFlagIsNonMixable) && range.mFormat.mFormatID == kAudioFormatLinearPCM)
                {
                    res = true;
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::updateExclusiveAndIntegerModeOnDevices()
{
    m_deviceInfoMutex.lock();
    for(tint i=0;i<m_deviceInfo->noDevices();i++)
    {
        const AOQueryCoreAudio::DeviceCoreAudio& cDevice = dynamic_cast<const AOQueryCoreAudio::DeviceCoreAudio&>(m_deviceInfo->device(i));
        AOQueryCoreAudio::DeviceCoreAudio& coreDevice = const_cast<AOQueryCoreAudio::DeviceCoreAudio&>(cDevice);
        coreDevice.setIntegerMode(canDeviceSupportIntegerMode(coreDevice.deviceID()));
        coreDevice.setHasExclusive(isExclusiveModeIfAvailable(coreDevice.deviceID()));
    }
    m_deviceInfoMutex.unlock();
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::isDeviceAlive(QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio>& pDevice)
{
    OSStatus err;
    bool isAlive = false;
    UInt32 paramSize = sizeof(bool);
    AudioObjectPropertyAddress property = { kAudioDevicePropertyDeviceIsAlive, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMain };

    err = CoreAudioIF::instance()->AudioObjectGetPropertyData(pDevice->deviceID(),&property,0,0,&paramSize,&isAlive);
    if(err!=noErr)
    {
        // The DAC driver may have not implemented this functionality so default to alive.
        isAlive = true;
    }

    if(!isAlive)
    {
        QString msg = "Audio device '" + pDevice->name() + "' is no longer alive and available";
        printToLog(msg.toUtf8().constData());
    }
    return isAlive;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::setCodecSampleFormatType(engine::Codec *codec, engine::RData *item)
{
    if(!item->isMixing() && !m_pSampleConverter.isNull() && !m_pSampleConverter->isFloat())
    {
        if(codec->dataTypesSupported() & engine::e_SampleInt32)
        {
            codec->setDataTypeFormat(engine::e_SampleInt32);
        }
        else if(codec->dataTypesSupported() & engine::e_SampleInt24)
        {
            codec->setDataTypeFormat(engine::e_SampleInt24);
        }
        else if(codec->dataTypesSupported() & engine::e_SampleInt16)
        {
            codec->setDataTypeFormat(engine::e_SampleInt16);
        }
        else
        {
            codec->setDataTypeFormat(engine::e_SampleFloat);
        }
    }
    else
    {
        codec->setDataTypeFormat(engine::e_SampleFloat);
    }
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::isDeviceVolume()
{
    tint i;
    AudioObjectPropertyAddress prop = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 0 };
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice = getCurrentCoreAudioDevice();
    bool isVolume = false;

    // 0 = master volume, 1 = left volume, 2 = right volume.
    for(i = 0; i < 3 && !isVolume; i++)
    {
         prop.mElement = i;
        if(CoreAudioIF::instance()->AudioObjectHasProperty(pDevice->deviceID(), &prop))
        {
            isVolume = true;
        }
    }
    return isVolume;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::isDeviceVolumeSettable()
{
    tint i;
    AudioObjectPropertyAddress prop = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 0 };
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice = getCurrentCoreAudioDevice();
    bool isVolume = false;

    // 0 = master volume, 1 = left volume, 2 = right volume.
    for(i = 0; i < 3 && !isVolume; i++)
    {
        prop.mElement = i;
        if(CoreAudioIF::instance()->AudioObjectHasProperty(pDevice->deviceID(), &prop))
        {
            OSStatus err;
            Boolean settableFlag = false;

            err = CoreAudioIF::instance()->AudioObjectIsPropertySettable(pDevice->deviceID(), &prop, &settableFlag);
            if(err == noErr && settableFlag)
            {
                isVolume = true;
            }
        }
    }
    return isVolume;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::isDeviceMuted()
{
        AudioObjectPropertyAddress prop = { kAudioDevicePropertyMute, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice = getCurrentCoreAudioDevice();
    bool isMute = false;

    if(CoreAudioIF::instance()->AudioObjectHasProperty(pDevice->deviceID(), &prop))
    {
        UInt32 mute = 0;
        UInt32 dataSize = sizeof(mute);
        OSStatus err;

        err = CoreAudioIF::instance()->AudioObjectGetPropertyData(pDevice->deviceID(), &prop, 0, 0, &dataSize, &mute);
        if(err == noErr)
        {
            isMute = (mute) ? true : false;
        }
    }
    return isMute;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::setDeviceMuted(bool isMute)
{
        AudioObjectPropertyAddress prop = { kAudioDevicePropertyMute, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice = getCurrentCoreAudioDevice();

    if(CoreAudioIF::instance()->AudioObjectHasProperty(pDevice->deviceID(), &prop))
    {
        OSStatus err;
        Boolean settableFlag = false;

        err = CoreAudioIF::instance()->AudioObjectIsPropertySettable(pDevice->deviceID(), &prop, &settableFlag);
        if(err == noErr && settableFlag)
        {
            if(isMute != isDeviceMuted())
            {
                UInt32 mute = (isMute) ? 1 : 0;
                UInt32 dataSizeM = sizeof(mute);
                CoreAudioIF::instance()->AudioObjectSetPropertyData(pDevice->deviceID(), &prop, 0, 0, dataSizeM, &mute);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

sample_t AOCoreAudioMacOS::getDeviceVolume()
{
    tint i;
    sample_t vol = 1.0f;
    AudioObjectPropertyAddress prop = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 0 };
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice = getCurrentCoreAudioDevice();

        if(!isDeviceMuted())
    {
        // 0 = master volume, 1 = left volume, 2 = right volume.
        for(i = 0; i < 3; i++)
        {
            prop.mElement = i;
            if(CoreAudioIF::instance()->AudioObjectHasProperty(pDevice->deviceID(),&prop))
            {
                Float32 volume;
                UInt32 dataSize = sizeof(volume);
                OSStatus err;

                err = CoreAudioIF::instance()->AudioObjectGetPropertyData(pDevice->deviceID(), &prop, 0, 0, &dataSize, &volume);
                if(err == noErr)
                {
                    vol = static_cast<sample_t>(volume);
                    break;
                }
            }
        }
    }
    else
    {
        vol = 0.0;
    }
    return vol;
}

//-------------------------------------------------------------------------------------------

bool AOCoreAudioMacOS::setDeviceVolume(sample_t vol)
{
    tint i, chCount;
    AudioObjectPropertyAddress prop = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, 0 };
    QSharedPointer<AOQueryCoreAudio::DeviceCoreAudio> pDevice = getCurrentCoreAudioDevice();
    bool isSet = false;

    if(vol < c_zeroSample)
    {
        vol = c_zeroSample;
    }
    else if(vol > c_plusOneSample)
    {
        vol = c_plusOneSample;
    }

    setDeviceMuted(isEqual(vol, c_zeroSample) ? true : false);

    // 0 = master volume, 1 = left volume, 2 = right volume.
    chCount = 0;
    for(i = 0; i < 3 && !isSet; i++)
    {
        prop.mElement = i;
        if(CoreAudioIF::instance()->AudioObjectHasProperty(pDevice->deviceID(), &prop))
        {
            OSStatus err;
            Boolean settableFlag = false;

            err = CoreAudioIF::instance()->AudioObjectIsPropertySettable(pDevice->deviceID(), &prop, &settableFlag);
            if(err == noErr && settableFlag)
            {
                Float32 volume = static_cast<Float32>(vol);
                UInt32 dataSize = sizeof(volume);

                err = CoreAudioIF::instance()->AudioObjectSetPropertyData(pDevice->deviceID(), &prop, 0, 0, dataSize, &volume);
                if(err == noErr)
                {
                    if(!i)
                    {
                        isSet = true;
                    }
                    else
                    {
                        chCount++;
                    }
                }
            }
        }
    }
    return (isSet || (chCount > 1));
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::doSetVolume(sample_t vol, bool isCallback)
{
    AOBase::doSetVolume(vol, isCallback);
    if(m_isDeviceVolume)
    {
        if(!setDeviceVolume(vol))
        {
            m_volume = getDeviceVolume();
            emitOnVolumeChanged(m_volume);
        }
    }
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::volumePropertyChangeProc(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddresses[],void *inClientData)
{
    OSStatus err = noErr;
    AOCoreAudioMacOS *pAudio = reinterpret_cast<AOCoreAudioMacOS *>(inClientData);

    if(pAudio != 0)
    {
        err = pAudio->volumeChangeProcImpl(inObjectID, inNumberAddresses, inAddresses);
    }
    return err;
}

//-------------------------------------------------------------------------------------------

OSStatus AOCoreAudioMacOS::volumeChangeProcImpl(AudioObjectID inObjectID,UInt32 inNumberAddresses,const AudioObjectPropertyAddress inAddresses[])
{
    UInt32 i;

    for(i=0;i<inNumberAddresses;i++)
    {
        switch(inAddresses[i].mSelector)
        {
            case kAudioDevicePropertyVolumeScalar:
            case kAudioDevicePropertyMute:
                m_volume = getDeviceVolume();
                emitOnVolumeChanged(m_volume);
                break;

            default:
                break;
        }
    }
    return noErr;
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::addVolumeChangeNotification(AudioDeviceID devID)
{
    AudioObjectPropertyAddress propVolume = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };
    AudioObjectPropertyAddress propMuted = { kAudioDevicePropertyMute, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };
    OSStatus err;

    if(CoreAudioIF::instance()->AudioObjectHasProperty(devID, &propVolume))
    {
        err = CoreAudioIF::instance()->AudioObjectAddPropertyListener(devID, &propVolume, AOCoreAudioMacOS::volumePropertyChangeProc, this);
        if(err!=noErr)
        {
            printError("addListenerDevice","Error adding volume property listener");
        }
    }
    if(CoreAudioIF::instance()->AudioObjectHasProperty(devID, &propMuted))
    {
        err = CoreAudioIF::instance()->AudioObjectAddPropertyListener(devID, &propMuted, AOCoreAudioMacOS::volumePropertyChangeProc, this);
        if(err!=noErr)
        {
            printError("addListenerDevice","Error adding volume muted property listener");
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::removeVolumeChangeNotification(AudioDeviceID devID)
{
    AudioObjectPropertyAddress propVolume = { kAudioDevicePropertyVolumeScalar, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };
    AudioObjectPropertyAddress propMuted = { kAudioDevicePropertyMute, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMain };
    OSStatus err;

    if(CoreAudioIF::instance()->AudioObjectHasProperty(devID, &propVolume))
    {
        err = CoreAudioIF::instance()->AudioObjectRemovePropertyListener(devID, &propVolume, AOCoreAudioMacOS::volumePropertyChangeProc, this);
        if(err!=noErr)
        {
            printError("addListenerDevice","Error adding volume property listener");
        }
    }
    if(CoreAudioIF::instance()->AudioObjectHasProperty(devID, &propMuted))
    {
        err = CoreAudioIF::instance()->AudioObjectRemovePropertyListener(devID, &propMuted, AOCoreAudioMacOS::volumePropertyChangeProc, this);
        if(err!=noErr)
        {
            printError("addListenerDevice","Error adding volume muted property listener");
        }
    }
}

//-------------------------------------------------------------------------------------------

void AOCoreAudioMacOS::printAudioStreamRangedDescription(AudioStreamRangedDescription desc) const
{
    QString str;

    str = "fmt=\'" + formatIDString(desc.mFormat.mFormatID) + "\', ";
    str += "ch=" + QString::number(desc.mFormat.mChannelsPerFrame) + ", ";
    str += "bits=" + QString::number(desc.mFormat.mBitsPerChannel) + ", ";
    str += "freq=" + QString::number(static_cast<tint>(desc.mFormat.mSampleRate));
    str += " (min=" + QString::number(static_cast<tint>(desc.mSampleRateRange.mMinimum)) + " max=" + QString::number(static_cast<tint>(desc.mSampleRateRange.mMaximum)) + "), ";
    //str += "bpf=" + QString::number(desc.mFormat.mBytesPerFrame) + ", ";
    //str += "bpp=" + QString::number(desc.mFormat.mBytesPerPacket) + ", ";
    //str += "fpp=" + QString::number(desc.mFormat.mFramesPerPacket) + " ";
    str += "(" + formatFlagString(desc.mFormat.mFormatFlags) + ")";
    common::Log::g_Log << str << common::c_endl;
}

//-------------------------------------------------------------------------------------------

QString AOCoreAudioMacOS::formatIDString(AudioFormatID formatID) const
{
    QString desc;
    switch(formatID)
    {
        case kAudioFormat60958AC3:
            desc = "AC-3 (IEC 60958)"; break;
        case kAudioFormatAC3:
            desc = "AC-3"; break;
        case kAudioFormatAES3:
            desc = "AES3-2003"; break;
        case kAudioFormatALaw:
            desc = "A-law"; break;
        case kAudioFormatAMR:
            desc = "Adaptive Multi-Rate (AMR)"; break;
        case kAudioFormatAMR_WB:
            desc = "AMR Wideband"; break;
        case kAudioFormatAppleIMA4:
            desc = "IMA 4:1 ADPCM"; break;
        case kAudioFormatAppleLossless:
            desc = "Apple Lossless"; break;
        case kAudioFormatAudible:
            desc = "Audible"; break;
        case kAudioFormatDVIIntelIMA:
            desc = "DVI/Intel IMA ADPCM"; break;
        case kAudioFormatEnhancedAC3:
            desc = "Enhanced AC-3"; break;
        case kAudioFormatFLAC:
            desc = "FLAC"; break;
        case kAudioFormatLinearPCM:
            desc = "Linear PCM"; break;
        case kAudioFormatMACE3:
            desc = "MACE 3:1"; break;
        case kAudioFormatMACE6:
            desc = "MACE C:1"; break;
        case kAudioFormatMIDIStream:
            desc = "MIDI"; break;
        case kAudioFormatMPEG4AAC:
            desc = "MPEG-4 AAC Low Complexity"; break;
        case kAudioFormatMPEG4AAC_ELD:
            desc = "MPEG-4 Enhanced Low Delay AAC"; break;
        case kAudioFormatMPEG4AAC_ELD_SBR:
            desc = "MPEG-4 Enhanced Low Delay AAC"; break;
        case kAudioFormatMPEG4AAC_ELD_V2:
            desc = "MPEG-4 Enhanced Low Delay AAC v2"; break;
        case kAudioFormatMPEG4AAC_HE:
            desc = "MPEG-4 High-Efficiency AAC"; break;
        case kAudioFormatMPEG4AAC_HE_V2:
            desc = "MPEG-4 High-Efficiency AAC v2"; break;
        case kAudioFormatMPEG4AAC_LD:
            desc = "MPEG-4 Low Delay AAC"; break;
        case kAudioFormatMPEG4AAC_Spatial:
            desc = "MPEG-4 Spatial Audio Coding"; break;
        case kAudioFormatMPEG4CELP:
            desc = "MPEG-4 CELP"; break;
        case kAudioFormatMPEG4HVXC:
            desc = "MPEG-4 HVXC"; break;
        case kAudioFormatMPEG4TwinVQ:
            desc = "MPEG-4 TwinVQ"; break;
        case kAudioFormatMPEGD_USAC:
            desc = "MPEG-D Unified Speech"; break;
        case kAudioFormatMPEGLayer1:
            desc = "MPEG-1/2, Layer I"; break;
        case kAudioFormatMPEGLayer2:
            desc = "MPEG-1/2, Layer II"; break;
        case kAudioFormatMPEGLayer3:
            desc = "MPEG-1/2, Layer III"; break;
        case kAudioFormatMicrosoftGSM:
            desc = "Microsoft GSM 6.10"; break;
        case kAudioFormatOpus:
            desc = "Opus"; break;
        case kAudioFormatParameterValueStream:
            desc = "ValueStream"; break;
        case kAudioFormatQDesign:
            desc = "QDesign"; break;
        case kAudioFormatQDesign2:
            desc = "QDesign v2"; break;
        case kAudioFormatQUALCOMM:
            desc = "Qualcomm PureVoice"; break;
        case kAudioFormatTimeCode:
            desc = "timestamp"; break;
        case kAudioFormatULaw:
            desc = "μ-Law"; break;
        case kAudioFormatiLBC:
            desc = "Low Bitrate Codec (iLBC)"; break;
        default:
            {
                tuint8 c[5];
                tuint32 x = static_cast<tuint32>(formatID);
                c[0] = static_cast<tuint8>((x >> 24) & 0xff);
                c[1] = static_cast<tuint8>((x >> 16) & 0xff);
                c[2] = static_cast<tuint8>((x >>  8) & 0xff);
                c[3] = static_cast<tuint8>((x      ) & 0xff);
                c[4] = 0;
                desc = reinterpret_cast<char *>(c);
            }
            break;
    }
    return desc;
}

//-------------------------------------------------------------------------------------------

QString AOCoreAudioMacOS::formatFlagString(AudioFormatFlags flag) const
{
    QString desc;

    desc = (flag & kAudioFormatFlagIsFloat) ? "DT=float, " : "DT=int, ";
    desc += (flag & kAudioFormatFlagIsBigEndian) ? "BE, " : "LE, ";
    desc += (flag & kAudioFormatFlagIsSignedInteger) ? "Signed, " : "Unsigned, ";
    if(flag & kAudioFormatFlagIsPacked)
    {
        desc += "Packed, ";
    }
    if(flag & kAudioFormatFlagIsAlignedHigh)
    {
        desc += "Align-High, ";
    }
    desc += (flag & kAudioFormatFlagIsNonMixable) ? "NON-Mixable" : "Mixable";
    return desc;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
