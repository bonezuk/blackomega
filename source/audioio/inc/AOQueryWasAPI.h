//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYWASAPI_H
#define __OMEGA_AUDIOIO_AOQUERYWASAPI_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQuerySharedDevice.h"
#include "audioio/inc/WasAPIIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryWasAPI : public AOQueryDevice
{
    public:
        class DeviceWasAPI;

    public:
        AOQueryWasAPI();
        virtual ~AOQueryWasAPI();
        
        virtual bool queryNames();
        virtual bool queryDevice(int idx);
        
        virtual int defaultDeviceIndex();
};

//------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryWasAPI::DeviceWasAPI : public AOQuerySharedDevice
{
    public:
        DeviceWasAPI();
        DeviceWasAPI(const DeviceWasAPI& rhs);
        virtual ~DeviceWasAPI();

        WasAPIDeviceSPtr deviceInterface() const;
        void setDeviceInterface(WasAPIDeviceSPtr pDeviceIF);
        
        virtual void print() const;

        virtual void setInitialized();

    protected:
        WasAPIDeviceSPtr m_pDeviceInterface;

        virtual void copy(const AOQueryDevice::Device& rhs);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
