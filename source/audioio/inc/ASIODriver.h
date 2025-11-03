//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_ASIODRIVER_H
#define __OMEGA_AUDIOIO_ASIODRIVER_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"
#include "audioio/inc/AudioIODLL.h"
#include "audioio/inc/ASIO.h"

#include <QString>
#include <QMap>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class ASIODriverService;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT ASIODriver
{
    public:
        ASIODriver(ASIODriverService *srv,IASIO *drv);
        ~ASIODriver();
        
        virtual ASIOError ASIOInit(ASIODriverInfo *info);
        virtual ASIOError ASIOExit();

        virtual ASIOError ASIOStart();
        virtual ASIOError ASIOStop();

        virtual ASIOError ASIOGetChannels(long *numInputChannels, long *numOutputChannels);
        virtual ASIOError ASIOGetLatencies(long *inputLatency, long *outputLatency);
        virtual ASIOError ASIOGetBufferSize(long *minSize, long *maxSize, long *preferredSize, long *granularity);
        virtual ASIOError ASIOCanSampleRate(ASIOSampleRate sampleRate);
        virtual ASIOError ASIOGetSampleRate(ASIOSampleRate *currentRate);
        virtual ASIOError ASIOSetSampleRate(ASIOSampleRate sampleRate);
        virtual ASIOError ASIOGetClockSources(ASIOClockSource *clocks, long *numSources);
        virtual ASIOError ASIOSetClockSource(long reference);
        virtual ASIOError ASIOGetSamplePosition (ASIOSamples *sPos, ASIOTimeStamp *tStamp);
        virtual ASIOError ASIOGetChannelInfo(ASIOChannelInfo *info);
        
        virtual ASIOError ASIOCreateBuffers(ASIOBufferInfo *bufferInfos, long numChannels,long bufferSize, ASIOCallbacks *callbacks);
        
        virtual ASIOError ASIODisposeBuffers();
        virtual ASIOError ASIOControlPanel();

        virtual ASIOError ASIOFuture(long selector, void *params);

        virtual ASIOError ASIOOutputReady();
        
        virtual ASIODriverService *service();
        virtual IASIO *driver();
        
    protected:
    
        ASIODriverService *m_service;
        IASIO *m_driver;
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT ASIODriverService
{
    public:
        class DriverInfo;

    public:
        ~ASIODriverService();
        
        static ASIODriverService& instance();
        
        static bool start();
        static void stop();
        
        const DriverInfo& driverInfo(tint index) const;
        tint noDrivers() const;
        
        bool open(tint index);
        void close(tint index);
        
        ASIODriver& driver(tint index);
        ASIODriver *driverPtr(tint index);
        
        static tint getSampleSize(ASIOSampleType t);
        
    protected:
    
        static ASIODriverService *m_instance;
        
        HRESULT m_comSystem;
        QMap<tint,DriverInfo> m_driverMap;
        QMap<tint,ASIODriver *> m_asioMap;
        
        ASIODriverService();
        
        void printError(const tchar *strR,const tchar *strE) const;
        
        bool init();
        void shutdown();
        
        bool getDriverInfo(HKEY hkey,DriverInfo& info);
        bool findDriverPath(const QString& comID,DriverInfo& info);
        bool canOpenDevice(DriverInfo& info);
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT ASIODriverService::DriverInfo
{
public:
    DriverInfo();
    DriverInfo(const DriverInfo& rhs);

    const DriverInfo& operator = (const DriverInfo& rhs);

    tint& id();
    const tint& id() const;
    CLSID& clsid();
    const CLSID& clsid() const;
    QString& path();
    const QString& path() const;
    QString& name();
    const QString& name() const;

protected:

    tint m_id;
    CLSID m_clsid;
    QString m_path;
    QString m_name;

    void copy(const DriverInfo& rhs);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

