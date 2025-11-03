//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_COREAUDIOIF_H
#define __OMEGA_AUDIOIO_COREAUDIOIF_H
//-------------------------------------------------------------------------------------------

#include <CoreServices/CoreServices.h>
#include <CoreFoundation/CoreFoundation.h>

#include <CoreAudio/AudioHardware.h>
#include <CoreAudio/CoreAudio.h>
#include <CoreAudio/CoreAudioTypes.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>

#include <AvailabilityMacros.h>

#include <QSharedPointer>

#include "common/inc/Factory.h"
#include "audioio/inc/AudioIODLL.h"
#include "audioio/inc/AOQueryDevice.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT CoreAudioIF
{
    public:
        CoreAudioIF();
        virtual ~CoreAudioIF();

        static QSharedPointer<CoreAudioIF> instance(const tchar *factoryName);
        static QSharedPointer<CoreAudioIF> instance();
        static void release();

        virtual void AudioObjectShow(AudioObjectID inObjectID) = 0;

        virtual Boolean AudioObjectHasProperty(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress) = 0;
        virtual OSStatus AudioObjectIsPropertySettable(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,Boolean *outIsSettable) = 0;
        virtual OSStatus AudioObjectGetPropertyDataSize(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 *outDataSize) = 0;
        virtual OSStatus AudioObjectGetPropertyData(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 *ioDataSize,void *outData) = 0;
        virtual OSStatus AudioObjectSetPropertyData(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 inDataSize,const void *inData) = 0;

        virtual OSStatus AudioObjectAddPropertyListener(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,AudioObjectPropertyListenerProc inListener,void *inClientData) = 0;
        virtual OSStatus AudioObjectRemovePropertyListener(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,AudioObjectPropertyListenerProc inListener,void *inClientData) = 0;

        virtual OSStatus AudioObjectAddPropertyListenerBlock(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,dispatch_queue_t inDispatchQueue,AudioObjectPropertyListenerBlock inListener) = 0;
        virtual OSStatus AudioObjectRemovePropertyListenerBlock(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,dispatch_queue_t inDispatchQueue,AudioObjectPropertyListenerBlock inListener) = 0;

        virtual OSStatus AudioDeviceCreateIOProcID(AudioObjectID inDevice,AudioDeviceIOProc inProc,void *inClientData,AudioDeviceIOProcID *outIOProcID) = 0;
        virtual OSStatus AudioDeviceDestroyIOProcID(AudioObjectID inDevice,AudioDeviceIOProcID inIOProcID) = 0;

        virtual OSStatus AudioDeviceStart(AudioObjectID inDevice,AudioDeviceIOProcID inProcID) = 0;
        virtual OSStatus AudioDeviceStop(AudioObjectID inDevice,AudioDeviceIOProcID inProcID) = 0;

    protected:

        static QSharedPointer<CoreAudioIF> m_instance;
};

typedef QSharedPointer<CoreAudioIF> CoreAudioIFSPtr;

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(AUDIOIO_EXPORT,CoreAudioIFFactory,CoreAudioIF)

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
