//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYASIO_H
#define __OMEGA_AUDIOIO_AOQUERYASIO_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryDevice.h"
#include "audioio/inc/ASIODriver.h"
#include "audioio/inc/ASIOData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryASIO : public AOQueryDevice
{
    public:
        class DeviceASIO;

    public:
        AOQueryASIO();
        virtual ~AOQueryASIO();
        
        virtual bool queryNames();
        virtual bool queryDevice(int idx);
        
        virtual int defaultDeviceIndex();
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryASIO::DeviceASIO : public AOQueryDevice::Device
{
    public:
        DeviceASIO();
        DeviceASIO(const AOQueryDevice::Device& rhs);
        virtual ~DeviceASIO();

        virtual bool isAPIExclusive() const;
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
