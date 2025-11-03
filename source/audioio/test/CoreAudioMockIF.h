//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_COREAUDIOMOCKIF_H
#define __OMEGA_AUDIOIO_COREAUDIOMOCKIF_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/CoreAudioIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class CoreAudioMockIF : public CoreAudioIF
{
    public:
        CoreAudioMockIF();
        virtual ~CoreAudioMockIF();

        MOCK_METHOD1(AudioObjectShow,void(AudioObjectID inObjectID));

        MOCK_METHOD2(AudioObjectHasProperty,Boolean(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress));
        MOCK_METHOD3(AudioObjectIsPropertySettable,OSStatus(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,Boolean *outIsSettable));
        MOCK_METHOD5(AudioObjectGetPropertyDataSize,OSStatus(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 *outDataSize));
        MOCK_METHOD6(AudioObjectGetPropertyData,OSStatus(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 *ioDataSize,void *outData));
        MOCK_METHOD6(AudioObjectSetPropertyData,OSStatus(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,UInt32 inQualifierDataSize,const void *inQualifierData,UInt32 inDataSize,const void *inData));

        MOCK_METHOD4(AudioObjectAddPropertyListener,OSStatus(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,AudioObjectPropertyListenerProc inListener,void *inClientData));
        MOCK_METHOD4(AudioObjectRemovePropertyListener,OSStatus(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,AudioObjectPropertyListenerProc inListener,void *inClientData));

        MOCK_METHOD4(AudioObjectAddPropertyListenerBlock,OSStatus(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,dispatch_queue_t inDispatchQueue,AudioObjectPropertyListenerBlock inListener));
        MOCK_METHOD4(AudioObjectRemovePropertyListenerBlock,OSStatus(AudioObjectID inObjectID,const AudioObjectPropertyAddress *inAddress,dispatch_queue_t inDispatchQueue,AudioObjectPropertyListenerBlock inListener));

        MOCK_METHOD4(AudioDeviceCreateIOProcID,OSStatus(AudioObjectID inDevice,AudioDeviceIOProc inProc,void *inClientData,AudioDeviceIOProcID *outIOProcID));
        MOCK_METHOD2(AudioDeviceDestroyIOProcID,OSStatus(AudioObjectID inDevice,AudioDeviceIOProcID inIOProcID));

        MOCK_METHOD2(AudioDeviceStart,OSStatus(AudioObjectID inDevice,AudioDeviceIOProcID inProcID));
        MOCK_METHOD2(AudioDeviceStop,OSStatus(AudioObjectID inDevice,AudioDeviceIOProcID inProcID));
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
