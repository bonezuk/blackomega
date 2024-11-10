#include "audioio/inc/AOQueryASIO.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AOQueryASIO::AOQueryASIO() : AOQueryDevice()
{}

//-------------------------------------------------------------------------------------------

AOQueryASIO::~AOQueryASIO()
{}

//-------------------------------------------------------------------------------------------

int AOQueryASIO::defaultDeviceIndex()
{
	return 0;
}

//-------------------------------------------------------------------------------------------

bool AOQueryASIO::queryNames()
{
	int i;
	ASIODriverService& asioService = ASIODriverService::instance();
	bool res = false;
	
	for(i=0;i<asioService.noDrivers();i++)
	{
		if(asioService.open(i))
		{
			ASIODriver *driver = asioService.driverPtr(i);
			if(driver!=0)
			{
				AOQueryDevice::Device *dev = new DeviceASIO();
				ASIODriverInfo dInfo;
								
				if(driver->ASIOInit(&dInfo)==ASE_OK)
				{
					dev->id() = QString::number(asioService.driverInfo(i).id());
					dev->name() = dInfo.name;
					m_devices.append(dev);
					res = true;
				}
			}
			asioService.close(i);
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOQueryASIO::queryDevice(int idx)
{
	static const tint rates[18] = {
		768000, 705600, 384000, 352800,
		192000, 176400, 96000,
		 88200,  64000, 48000, 
		 44100,  32000, 24000, 
		 22050,  16000, 12000, 
		 11025,   8000		
	};	
	
	int j;
	ASIODriverService& asioService = ASIODriverService::instance();
	bool res = false;
	
	if(idx>=0 && idx<m_devices.size())
	{
		if(asioService.open(idx))
		{
			ASIODriver *driver = asioService.driverPtr(idx);
			if(driver!=0)
			{
				long noInputChs = 0,noOutputChs = 0;
				AOQueryDevice::Device& dev = *(m_devices[idx]);
				ASIODriverInfo dInfo;
				ASIOChannelInfo chInfo[32];
				
				if(driver->ASIOInit(&dInfo)==ASE_OK)
				{
					dev.name() = dInfo.name;
					if(driver->ASIOGetChannels(&noInputChs,&noOutputChs)==ASE_OK)
					{
						if(noOutputChs>=2)
						{
							dev.setNoChannels(noOutputChs);
						
							for(j=0;j<18;j++)
							{
								if(driver->ASIOCanSampleRate(static_cast<ASIOSampleRate>(rates[j]))==ASE_OK)
								{
									dev.addFrequency(rates[j]);
								}
							}
						}
					}
					dev.loadChannelMap();
					dev.setInitialized();
					res = true;
				}
			}
			asioService.close(idx);
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------
// AOQueryASIO::DeviceASIO
//-------------------------------------------------------------------------------------------

AOQueryASIO::DeviceASIO::DeviceASIO() : AOQueryDevice::Device(AOQueryDevice::Device::e_deviceASIO)
{}

//-------------------------------------------------------------------------------------------

AOQueryASIO::DeviceASIO::DeviceASIO(const AOQueryDevice::Device& rhs) : AOQueryDevice::Device(rhs)
{
	Q_ASSERT(rhs.type() == AOQueryDevice::Device::e_deviceASIO);
}

//-------------------------------------------------------------------------------------------

AOQueryASIO::DeviceASIO::~DeviceASIO()
{}

//-------------------------------------------------------------------------------------------

bool AOQueryASIO::DeviceASIO::isAPIExclusive() const
{
	return true;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
