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

AOQueryASIO::DeviceASIO::DeviceASIO() : AOQueryDevice::Device(AOQueryDevice::Device::e_deviceASIO),
	m_nativeDSDFrequencies()
{}

//-------------------------------------------------------------------------------------------

AOQueryASIO::DeviceASIO::DeviceASIO(const AOQueryDevice::Device& rhs) : AOQueryDevice::Device(rhs)
{
	AOQueryASIO::DeviceASIO::copy(rhs);
}

//-------------------------------------------------------------------------------------------

AOQueryASIO::DeviceASIO::~DeviceASIO()
{}

//-------------------------------------------------------------------------------------------

bool AOQueryASIO::DeviceASIO::isAPIExclusive() const
{
	AOQueryASIO::DeviceASIO::return true;
}

//-------------------------------------------------------------------------------------------

bool AOQueryASIO::DeviceASIO::isDSDNative() const
{
	return (!m_nativeDSDFrequencies.isEmpty());
}

//-------------------------------------------------------------------------------------------

tint AOQueryASIO::DeviceASIO::isDSDFrequencySupported(int freq) const
{
	return (m_nativeDSDFrequencies.find(freq) != m_nativeDSDFrequencies.end());
}

//-------------------------------------------------------------------------------------------

void AOQueryASIO::DeviceASIO::queryDSDCapabilities(ASIODriver *driver)
{
	static const tint dsdrates[5] = {
		 2822400, // DSD-64 (Single-rate)
		 5644800, // DSD-128 (Double-rate)
		11289600, // DSD-256 (Quad-rate)
		22579200, // DSD-512 (Octuple-rate)
		45158400  // DSD-1024 (Sexdecuple-rate)
	};

	ASIOIoFormat dsdFormat = { kASIODSDFormat };
	
	m_nativeDSDFrequencies.clear();
	if(driver->ASIOFuture(kAsioSetIoFormat, &dsdFormat) == ASE_SUCCESS)
	{
		for(tint i = 0; i < 5; i++)
		{
			if(driver->ASIOCanSampleRate(static_cast<ASIOSampleRate>(dsdrates[i])) == ASE_OK)
			{
				m_nativeDSDFrequencies.add(dsdrates[i]);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOQueryASIO::DeviceASIO::copy(const AOQueryDevice::Device& rhs)
{
	Q_ASSERT(rhs.type() == AOQueryDevice::Device::e_deviceASIO);
	AOQueryASIO::DeviceASIO& asioRhs = dynamic_cast<AOQueryASIO::DeviceASIO &>(rhs);
	AOQueryDevice::Device::copy(rhs);
	m_nativeDSDFrequencies = asioRhs.m_nativeDSDFrequencies;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
