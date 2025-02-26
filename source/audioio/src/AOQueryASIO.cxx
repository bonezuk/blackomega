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
	static const tint rates[22] = {
		3072000, 2822400, 1536000, 1411200,
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
				AOQueryASIO::DeviceASIO& dev = dynamic_cast<AOQueryASIO::DeviceASIO&>(*(m_devices[idx]));
				ASIODriverInfo dInfo;
				
				if(driver->ASIOInit(&dInfo)==ASE_OK)
				{
					dev.name() = dInfo.name;
					if(driver->ASIOGetChannels(&noInputChs,&noOutputChs)==ASE_OK)
					{
						if(noOutputChs>=2)
						{
							dev.setNoChannels(noOutputChs);
						
							for(j=0;j<22;j++)
							{
								if(driver->ASIOCanSampleRate(static_cast<ASIOSampleRate>(rates[j]))==ASE_OK)
								{
									dev.addFrequency(rates[j]);
								}
							}
						}
					}
					dev.queryDSDCapabilities(driver);
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
	return true;
}

//-------------------------------------------------------------------------------------------

bool AOQueryASIO::DeviceASIO::isDSDNative() const
{
	return (!m_nativeDSDFrequencies.isEmpty());
}

//-------------------------------------------------------------------------------------------

bool AOQueryASIO::DeviceASIO::isDSDFrequencySupported(int freq, bool isNative)
{
	bool res;

	if(isNative)
	{
		res = (m_nativeDSDFrequencies.find(freq) != m_nativeDSDFrequencies.end());
	}
	else
	{
		res = AOQueryDevice::Device::isDSDFrequencySupported(freq, isNative);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

int AOQueryASIO::DeviceASIO::dsdOverPCMSampleType(ASIOSampleType type) const
{
	int dopFlag;
	
	switch(type)
	{
		case ASIOSTInt24LSB:
		case ASIOSTInt24MSB:
		case ASIOSTInt32LSB24:
		case ASIOSTInt32MSB24:
			dopFlag = AOQueryDevice::Device::e_dopInt24;
			break;
		case ASIOSTInt32LSB:
		case ASIOSTInt32MSB:
			dopFlag = AOQueryDevice::Device::e_dopInt32;
			break;
		default:
			dopFlag = 0;
			break;
	}
	return dopFlag;
}

//-------------------------------------------------------------------------------------------

void AOQueryASIO::DeviceASIO::queryDSDNativeCapabilities(ASIODriver *driver)
{
	static const tint dsdrates[10] = {
		 2822400, // DSD-64 (Single-rate) 44.1kHz
		 3072000, // DSD-64 (Single-rate) 48kHz
		 5644800, // DSD-128 (Double-rate) 44.1kHz
		 6144000, // DSD-128 (Double-rate) 48kHz
		11289600, // DSD-256 (Quad-rate) 44.1kHz
		12288000, // DSD-256 (Quad-rate) 48kHz
		22579200, // DSD-512 (Octuple-rate) 44.1kHz
		24576000, // DSD-512 (Octuple-rate) 48kHz
		45158400,  // DSD-1024 (Sexdecuple-rate) 44.1kHz
		49152000   // DSD-1024 (Sexdecuple-rate) 48kHz
	};

	ASIOIoFormat dsdFormat = { kASIODSDFormat };
	
	m_nativeDSDFrequencies.clear();
	if(driver->ASIOFuture(kAsioSetIoFormat, &dsdFormat) == ASE_SUCCESS)
	{
		for(tint i = 0; i < 10; i++)
		{
			if(driver->ASIOCanSampleRate(static_cast<ASIOSampleRate>(dsdrates[i])) == ASE_OK)
			{
				m_nativeDSDFrequencies.insert(dsdrates[i]);
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOQueryASIO::DeviceASIO::queryDSDOverPCMCapabilities(ASIODriver *driver)
{
	static const tint pcmrates[10] = {
		176400,  // DSD64   -  2822400Hz =  176400Hz PCM
		192000,  // DSD64   -  3072000Hz =  192000Hz PCM
		352800,  // DSD128  -  5644800Hz =  352800Hz PCM
		384000,  // DSD128  -  6144000Hz =  384000Hz PCM
		705600,  // DSD256  - 11289600Hz =  705600Hz PCM
		768000,  // DSD256  - 12288000Hz =  768000Hz PCM
		1411200, // DSD512  - 22579200Hz = 1411200Hz PCM
		1536000, // DSD512  - 24576000Hz = 1536000Hz PCM
		2822400, // DSD1024 - 45158400Hz = 2822400Hz PCM
		3072000  // DSD1024 - 49152000Hz = 3072000Hz PCM
	};
	
	ASIOError res;
	ASIOChannelInfo channelInfo;

	m_dsdOverPcmSupport = 0;
	for(tint i = 0; i < 10; i++)
	{
		res = driver->ASIOCanSampleRate(static_cast<ASIOSampleRate>(pcmrates[i]));
		if(res == ASE_OK)
		{
			res = driver->ASIOSetSampleRate(pcmrates[i]);
			if(res == ASE_OK)
			{
				channelInfo.channel = 0;
				channelInfo.isInput = ASIOFalse;
				res = driver->ASIOGetChannelInfo(&channelInfo);
				if(res == ASE_OK)
				{
					m_dsdOverPcmSupport |= dsdOverPCMSampleType(channelInfo.type);
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void AOQueryASIO::DeviceASIO::queryDSDCapabilities(ASIODriver *driver)
{
	queryDSDOverPCMCapabilities(driver);
	queryDSDNativeCapabilities(driver);
}

//-------------------------------------------------------------------------------------------

void AOQueryASIO::DeviceASIO::copy(const AOQueryDevice::Device& rhs)
{
	Q_ASSERT(rhs.type() == AOQueryDevice::Device::e_deviceASIO);
	const AOQueryASIO::DeviceASIO& asioRhs = dynamic_cast<const AOQueryASIO::DeviceASIO &>(rhs);
	AOQueryDevice::Device::copy(rhs);
	m_nativeDSDFrequencies = asioRhs.m_nativeDSDFrequencies;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
