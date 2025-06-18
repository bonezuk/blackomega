#include "audioio/inc/CoreAudioLayerIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CoreAudioIFFactory,CoreAudioIF, \
                            CoreAudioLayerIFFactory,CoreAudioLayerIF, \
                            "coreaudio",false)

//-------------------------------------------------------------------------------------------

CoreAudioLayerIF::CoreAudioLayerIF() : CoreAudioIF()
{}

//-------------------------------------------------------------------------------------------

CoreAudioLayerIF::~CoreAudioLayerIF()
{}

//-------------------------------------------------------------------------------------------

void CoreAudioLayerIF::AudioObjectShow(AudioObjectID inObjectID)
{
    ::AudioObjectShow(inObjectID);
}

//-------------------------------------------------------------------------------------------

Boolean CoreAudioLayerIF::AudioObjectHasProperty(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress)
{
    return ::AudioObjectHasProperty(inObjectID,inAddress);
}

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioObjectIsPropertySettable(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,Boolean *outIsSettable)
{
    return ::AudioObjectIsPropertySettable(inObjectID,inAddress,outIsSettable);
}

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioObjectGetPropertyDataSize(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 *outDataSize)
{
    return ::AudioObjectGetPropertyDataSize(inObjectID,inAddress,inQualifierDataSize,inQualifierData,outDataSize);
}

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioObjectGetPropertyData(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 *ioDataSize,void *outData)
{
    return ::AudioObjectGetPropertyData(inObjectID,inAddress,inQualifierDataSize,inQualifierData,ioDataSize,outData);
}

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioObjectSetPropertyData(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 inDataSize,const void *inData)
{
    return ::AudioObjectSetPropertyData(inObjectID,inAddress,inQualifierDataSize,inQualifierData,inDataSize,inData);
}

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioObjectAddPropertyListener(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,AudioObjectPropertyListenerProc inListener,void *inClientData)
{
    return ::AudioObjectAddPropertyListener(inObjectID,inAddress,inListener,inClientData);
}

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioObjectRemovePropertyListener(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,AudioObjectPropertyListenerProc inListener,void *inClientData)
{
    return ::AudioObjectRemovePropertyListener(inObjectID,inAddress,inListener,inClientData);
}

//-------------------------------------------------------------------------------------------

#ifdef __clang__

OSStatus CoreAudioLayerIF::AudioObjectAddPropertyListenerBlock(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,dispatch_queue_t inDispatchQueue,AudioObjectPropertyListenerBlock inListener)
{
    return ::AudioObjectAddPropertyListenerBlock(inObjectID,inAddress,inDispatchQueue,inListener);
}

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioObjectRemovePropertyListenerBlock(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,dispatch_queue_t inDispatchQueue,AudioObjectPropertyListenerBlock inListener)
{
    return ::AudioObjectRemovePropertyListenerBlock(inObjectID,inAddress,inDispatchQueue,inListener);
}

#endif

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioDeviceCreateIOProcID(AudioObjectID inDevice,AudioDeviceIOProc inProc,void *inClientData,AudioDeviceIOProcID *outIOProcID)
{
    return ::AudioDeviceCreateIOProcID(inDevice,inProc,inClientData,outIOProcID);
}

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioDeviceDestroyIOProcID(AudioObjectID inDevice,AudioDeviceIOProcID inIOProcID)
{
    return ::AudioDeviceDestroyIOProcID(inDevice,inIOProcID);
}

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioDeviceStart(AudioObjectID inDevice,AudioDeviceIOProcID inProcID)
{
    return ::AudioDeviceStart(inDevice,inProcID);
}

//-------------------------------------------------------------------------------------------

OSStatus CoreAudioLayerIF::AudioDeviceStop(AudioObjectID inDevice,AudioDeviceIOProcID inProcID)
{
    return ::AudioDeviceStop(inDevice,inProcID);
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
