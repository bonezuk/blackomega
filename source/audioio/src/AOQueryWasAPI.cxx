#include "audioio/inc/AOQueryWasAPI.h"
#include "audioio/inc/WasAPIIF.h"
#include "audioio/inc/WasAPILayerIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AOQueryWasAPI::AOQueryWasAPI() : AOQueryDevice()
{}

//-------------------------------------------------------------------------------------------

AOQueryWasAPI::~AOQueryWasAPI()
{}

//-------------------------------------------------------------------------------------------

bool AOQueryWasAPI::queryNames()
{
	QStringList deviceList = WasAPIIF::instance()->enumerateDeviceIds();
	bool res = false;
	
	for(QStringList::iterator ppI=deviceList.begin();ppI!=deviceList.end();++ppI)
	{
		WasAPIDeviceSPtr pDevice = WasAPIIF::instance()->getDevice(*ppI);
		
		if(!pDevice.isNull())
		{
			AOQueryWasAPI::DeviceWasAPI *dev = new AOQueryWasAPI::DeviceWasAPI;

			dev->id() = pDevice->id();
			dev->setDeviceInterface(pDevice);
			if(!dev->id().isEmpty())
			{
				dev->name() = pDevice->name();
				m_devices.append(dev);
				res = true;
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOQueryWasAPI::queryDevice(int idx)
{
	bool res = false;
	
	if(idx>=0 && idx<m_devices.size())
	{
		DeviceWasAPI *dev = dynamic_cast<DeviceWasAPI *>(m_devices[idx]);
		if(dev!=0)
		{
			if(!dev->isInitialized())
			{
				for(int i = 0; i < 2; i++)
				{
					AccessModeWasAPI mode = (!i) ? e_Exclusive : e_Shared;
					
					dev->setAccessMode(mode);

					QSet<int> freqSet = dev->deviceInterface()->queryFrequencyCapabilities(mode);
					for(QSet<int>::iterator ppI=freqSet.begin();ppI!=freqSet.end();ppI++)
					{
						dev->addFrequency(*ppI);
					}
					
					dev->setNoChannels(dev->deviceInterface()->queryChannelCapabilities(mode));
				}
				dev->setAccessMode(e_Settings);
				dev->loadChannelMap();
				dev->setInitialized();
			}
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

int AOQueryWasAPI::defaultDeviceIndex()
{
	return -1;
}

//-------------------------------------------------------------------------------------------
// AOQueryWasAPI::DeviceWasAPI
//-------------------------------------------------------------------------------------------

AOQueryWasAPI::DeviceWasAPI::DeviceWasAPI() : AOQueryDevice::Device(),
	m_accessMode(AOQueryWasAPI::DeviceWasAPI::e_Settings),
	m_frequencySetShared(),
	m_channelMapShared(0),
	m_pDeviceInterface()
{
	m_type = AOQueryDevice::Device::e_deviceWasAPI;
	m_channelMapShared = new AOChannelMap(*this, "shared");
}

//-------------------------------------------------------------------------------------------

AOQueryWasAPI::DeviceWasAPI::DeviceWasAPI(const DeviceWasAPI& rhs)
{
	AOQueryWasAPI::DeviceWasAPI::copy(rhs);
}

//-------------------------------------------------------------------------------------------

AOQueryWasAPI::DeviceWasAPI::~DeviceWasAPI()
{}

//-------------------------------------------------------------------------------------------

WasAPIDeviceSPtr AOQueryWasAPI::DeviceWasAPI::deviceInterface() const
{
	return m_pDeviceInterface;
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::setDeviceInterface(WasAPIDeviceSPtr pDeviceIF)
{
	m_pDeviceInterface = pDeviceIF;
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::copy(const AOQueryDevice::Device& rhs)
{
	const AOQueryWasAPI::DeviceWasAPI& wasDevice = dynamic_cast<const AOQueryWasAPI::DeviceWasAPI&>(rhs);
	setDeviceInterface(wasDevice.deviceInterface());
	m_accessMode = rhs.m_accessMode;
	m_frequencySetShared = rhs.m_frequencySetShared;
	m_channelMapShared->copyForDevice(rhs.m_channelMapShared);
	AOQueryDevice::Device::copy(rhs);
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::print() const
{
	int i,j,k ;

	AOQueryDevice::Device::print();

	WasAPIDeviceSPtr pDevice = WasAPIIF::instance()->getDevice(m_id);
	if(!pDevice.isNull())
	{
		QSharedPointer<WasAPIDeviceLayer> pLDevice = pDevice.dynamicCast<WasAPIDeviceLayer>();

		common::Log::g_Log.print("Supported Formats:\n");
		for(i=0;i<NUMBER_WASAPI_MAXCHANNELS;i++)
		{
			for(j=0;j<NUMBER_WASAPI_MAXBITS;j++)
			{
				for(k=0;k<NUMBER_WASAPI_MAXFREQUENCIES;k++)
				{
					if(pLDevice->isFormat(i,j,k))
					{
						common::Log::g_Log.print("  channels=%d, frequency=%d, bits=%d\n",pLDevice->getNumberOfChannelsFromIndex(i),pLDevice->getFrequencyFromIndex(k),pLDevice->getNumberOfBitsFromIndex(j));
					}
				}
			}
		}
	}
	common::Log::g_Log.print("\n\n");
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::setInitialized()
{
	AOQueryDevice::Device::setInitialized();
	setHasExclusive(true);
}

//-------------------------------------------------------------------------------------------

AccessModeWasAPI AOQueryWasAPI::DeviceWasAPI::accessMode() const
{
	return m_accessMode;
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::setAccessMode(AccessModeWasAPI mode)
{
	m_accessMode = mode;
}

//-------------------------------------------------------------------------------------------

bool AOQueryWasAPI::DeviceWasAPI::isExclusiveFromAM() const
{
	bool isExcl;
	
	if(m_accessMode == e_Settings)
	{
		isExcl = AudioSettings::instance(name())->isExclusive();
	}
	else
	{
		isExcl = (m_accessMode == e_Exclusive);
	}
	return isExcl;
}

//-------------------------------------------------------------------------------------------

bool AOQueryWasAPI::DeviceWasAPI::isFrequencySupported(int freq) const
{
	if(isExclusiveFromAM())
	{
		return (m_frequencySet.find(freq) != m_frequencySet.end());
	}
	else
	{
		return (m_frequencySetShared.find(freq) != m_frequencySetShared.end());
	}
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::addFrequency(int freq)
{
	if(isExclusiveFromAM())
	{
		m_frequencySet.insert(freq);
	}
	else
	{
		m_frequencySetShared.insert(freq);
	}
}

//-------------------------------------------------------------------------------------------

const QSet<int>& AOQueryWasAPI::DeviceWasAPI::frequencies() const
{
	if(isExclusiveFromAM())
	{
		return m_frequencySet;
	}
	else
	{
		return m_frequencySetShared;
	}
}

//-------------------------------------------------------------------------------------------

int AOQueryWasAPI::DeviceWasAPI::noChannels() const
{
	if(isExclusiveFromAM())
	{
		return m_channelMap->noDeviceChannels();
	}
	else
	{
		return m_channelMapShared->noDeviceChannels();
	}
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::setNoChannels(int noCh)
{
	if(isExclusiveFromAM())
	{
		m_channelMap->setNoDeviceChannels(noCh);
	}
	else
	{
		m_channelMapShared->setNoDeviceChannels(noCh);
	}
}

//-------------------------------------------------------------------------------------------

AOChannelMap *AOQueryWasAPI::DeviceWasAPI::channelMap()
{
	if(isExclusiveFromAM())
	{
		return m_channelMap;
	}
	else
	{
		return m_channelMapShared;
	}
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::loadChannelMap(bool mapChannelFromSettings)
{
	loadCM(m_channelMap, mapChannelFromSettings);
	loadCM(m_channelMapShared, mapChannelFromSettings);
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::saveChannelMap()
{
	saveCM(m_channelMap);
	saveCM(m_channelMapShared);
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
