//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_COREAUDIOLAYERIF_H
#define __OMEGA_AUDIOIO_COREAUDIOLAYERIF_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/CoreAudioIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT CoreAudioLayerIF : public CoreAudioIF
{
    public:
        CoreAudioLayerIF();
        virtual ~CoreAudioLayerIF();

        virtual void AudioObjectShow(AudioObjectID inObjectID);

        virtual Boolean AudioObjectHasProperty(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress);
        virtual OSStatus AudioObjectIsPropertySettable(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,Boolean *outIsSettable);
        virtual OSStatus AudioObjectGetPropertyDataSize(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 *outDataSize);
        virtual OSStatus AudioObjectGetPropertyData(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 *ioDataSize,void *outData);
        virtual OSStatus AudioObjectSetPropertyData(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 inDataSize,const void *inData);

        virtual OSStatus AudioObjectAddPropertyListener(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,AudioObjectPropertyListenerProc inListener,void *inClientData);
        virtual OSStatus AudioObjectRemovePropertyListener(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,AudioObjectPropertyListenerProc inListener,void *inClientData);

#ifdef __clang__
        virtual OSStatus AudioObjectAddPropertyListenerBlock(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,dispatch_queue_t inDispatchQueue,AudioObjectPropertyListenerBlock inListener);
        virtual OSStatus AudioObjectRemovePropertyListenerBlock(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,dispatch_queue_t inDispatchQueue,AudioObjectPropertyListenerBlock inListener);
#endif

        virtual OSStatus AudioDeviceCreateIOProcID(AudioObjectID inDevice,AudioDeviceIOProc inProc,void *inClientData,AudioDeviceIOProcID *outIOProcID);
        virtual OSStatus AudioDeviceDestroyIOProcID(AudioObjectID inDevice,AudioDeviceIOProcID inIOProcID);

        virtual OSStatus AudioDeviceStart(AudioObjectID inDevice,AudioDeviceIOProcID inProcID);
        virtual OSStatus AudioDeviceStop(AudioObjectID inDevice,AudioDeviceIOProcID inProcID);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
