//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX) || defined(OMEGA_IOS)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOCOREAUDIO_H
#define __OMEGA_AUDIOIO_AOCOREAUDIO_H
//-------------------------------------------------------------------------------------------

#import <AudioToolbox/AudioToolbox.h>
#import <AudioUnit/AudioUnit.h>
#import <CoreAudio/CoreAudioTypes.h>

#include "audioio/inc/AOBase.h"
#include "audioio/inc/SampleConverter.h"
#include "audioio/inc/FormatsSupported.h"
#include "audioio/inc/FormatDescriptionUtils.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOCoreAudio : public AOBase
{
    public:
        AOCoreAudio(QObject *parent = 0);
        virtual ~AOCoreAudio();
    
    protected:
    
        virtual int getDeviceFrequency();
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
