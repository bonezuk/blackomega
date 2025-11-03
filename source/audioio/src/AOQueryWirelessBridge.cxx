//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYWIRELESSBRIDGE_H
#define __OMEGA_AUDIOIO_AOQUERYWIRELESSBRIDGE_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryDevice.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryWirelessBridge : public AOQueryDevice
{
    public:
        AOQueryWirelessBridge();
        virtual ~AOQueryWirelessBridge();
        
        virtual bool queryNames();
        virtual bool queryDevice(int idx);
        
        virtual int defaultDeviceIndex();
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AOQueryWirelessBridge::AOQueryWirelessBridge() : AOQueryDevice()
{}

//-------------------------------------------------------------------------------------------

AOQueryWirelessBridge::~AOQueryWirelessBridge()
{}

//-------------------------------------------------------------------------------------------

bool AOQueryWirelessBridge::queryNames()
{
    
}

//-------------------------------------------------------------------------------------------

bool AOQueryWirelessBridge::queryDevice(int idx)

//-------------------------------------------------------------------------------------------

int AOQueryWirelessBridge::defaultDeviceIndex()

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
