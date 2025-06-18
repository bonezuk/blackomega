//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_WASAPIIF_H
#define __OMEGA_AUDIOIO_WASAPIIF_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Factory.h"
#include "audioio/inc/AudioIODLL.h"
#include "audioio/inc/AOQuerySharedDevice.h"
#include "audioio/inc/WasAPICOMInterfaces.h"
#include "audioio/inc/FormatDescription.h"
#include "audioio/inc/WasAPIVolumeEvents.h"

#include <QSharedPointer>
#include <QString>
#include <QStringList>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class WasAPIDevice;
class WasAPIIF;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT WasAPIIF : public QObject
{
    public:
        Q_OBJECT
        
    public:
        WasAPIIF();
        virtual ~WasAPIIF();
        
        static QSharedPointer<WasAPIIF> instance(const tchar *factoryName);
        static QSharedPointer<WasAPIIF> instance();
        static void release();
        
        virtual QStringList enumerateDeviceIds() = 0;
        
        virtual QSharedPointer<WasAPIDevice> getDevice(const QString& devID) = 0;
        
    protected:
    
        static QSharedPointer<WasAPIIF> m_instance;
        static bool m_exclusiveAccess;
        
        virtual bool init() = 0;
        virtual void done() = 0;
};

ABSTRACT_FACTORY_CLASS(AUDIOIO_EXPORT,WasAPIIFFactory,WasAPIIF)

//-------------------------------------------------------------------------------------------

typedef QSharedPointer<WasAPIIF> WasAPIIFSPtr;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT WasAPIDevice : public QObject
{
    public:
        Q_OBJECT
    
    public:
        friend class WasAPIIF;

    public:
        WasAPIDevice();
        virtual ~WasAPIDevice();

        virtual QString id() const = 0;
        virtual QString name() const = 0;

        virtual bool isExclusive() const = 0;
        virtual void setExclusive(bool flag) = 0;

        virtual QSet<int> queryFrequencyCapabilities(AccessModeSharedDevice accessMode = e_Settings) = 0;
        virtual int queryChannelCapabilities(AccessModeSharedDevice accessMode = e_Settings) = 0;

        virtual WAVEFORMATEX *findClosestSupportedFormat(const FormatDescription& sourceDesc) = 0;

        virtual IAudioClientIFSPtr getAudioClient() = 0;
        virtual void releaseAudioClient() = 0;

        virtual bool isDeviceVolume() = 0;
        virtual sample_t getVolume() = 0;
        virtual bool setVolume(sample_t vol) = 0;

        virtual bool setupVolumeNotification(VolumeChangeNotifier pNotifier, LPVOID pVInstance) = 0;
        virtual void shutdownVolumeNotification() = 0;

    protected:
    
        virtual bool init(const QString& devID) = 0;
        virtual void done() = 0;
};

//-------------------------------------------------------------------------------------------

typedef QSharedPointer<WasAPIDevice> WasAPIDeviceSPtr;

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
