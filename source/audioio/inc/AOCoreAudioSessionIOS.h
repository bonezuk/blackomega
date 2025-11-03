//-------------------------------------------------------------------------------------------
#if defined(OMEGA_IOS)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOCOREAUDIOSESSIONIOS_H
#define __OMEGA_AUDIOIO_AOCOREAUDIOSESSIONIOS_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOCoreAudio.h"
#include "audioio/inc/AOQueryCoreAudioIOS.h"
#include "audioio/inc/AudioHardwareBufferCoreAudioIOS.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOCoreAudioSessionIOS : public QObject
{
    public:
        virtual ~AOCoreAudioSessionIOS();
        
        static QSharedPointer<AOCoreAudioSessionIOS> audioInstance();
        
        virtual void logOutput();
        
        virtual bool loadDevice(AOQueryDevice::Device& dev) = 0;
        virtual bool saveDevice(AOQueryDevice::Device& dev) = 0;
        
    protected:
        static QSharedPointer<AOCoreAudioSessionIOS> m_instance;
        
        AOCoreAudioSessionIOS(QObject *parent = 0);
        
        virtual void printError(const char *strR, const char *strE) const;
        virtual void printError(const char *strR, const char *strE, void *pE) const;
        
        virtual bool init();
        virtual void close();
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
