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
				QSet<int> freqSet = dev->deviceInterface()->queryFrequencyCapabilities();
				for(QSet<int>::iterator ppI=freqSet.begin();ppI!=freqSet.end();ppI++)
				{
					dev->addFrequency(*ppI);
				}
/* TODO Refactor */
				QVector<AOQueryDevice::Channel> channelList = dev->deviceInterface()->queryChannelCapabilities();
				dev->setNoChannels(channelList.size());
				for(int i=0;i<channelList.size();i++)
				{
					dev->channel(i) = channelList.at(i);
				}

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
	m_pDeviceInterface()
{
	m_type = AOQueryDevice::Device::e_deviceWasAPI;
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
	AOQueryDevice::Device::copy(rhs);
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::print() const
{
	int i,j,k ;
	QList<int> sFreq;
	QSet<int>::const_iterator ppI;
	
	for(ppI=m_frequencySet.begin();ppI!=m_frequencySet.end();ppI++)
	{
		sFreq.append(*ppI);
	}
	std::sort(sFreq.begin(),sFreq.end());
	
	common::Log::g_Log.print("Device UUID : %s\n",m_id.toUtf8().constData());
	common::Log::g_Log.print("Device Name : %s\n",m_name.toUtf8().constData());
	common::Log::g_Log.print("Channels :");
	for(i=0;i<m_channels.size();i++)
	{
		common::Log::g_Log.print(" %d %s,",i,m_channels.at(i).name().toUtf8().constData());
	}
	common::Log::g_Log.print("\n");
	common::Log::g_Log.print("Frequencies : ");
	for(i=0;i<sFreq.size();i++)
	{
		common::Log::g_Log.print(" %d",sFreq.at(i));
	}
	common::Log::g_Log.print("\n");

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

void AOQueryWasAPI::DeviceWasAPI::updateExclusive()
{
	QSet<int> freqSet = deviceInterface()->queryFrequencyCapabilities();
	m_frequencySet.clear();
	for(QSet<int>::iterator ppI=freqSet.begin();ppI!=freqSet.end();ppI++)
	{
		m_frequencySet.insert(*ppI);
	}
	
	QVector<AOQueryDevice::Channel> channelList = deviceInterface()->queryChannelCapabilities();
	setNoChannels(channelList.size());
	for(int i=0;i<channelList.size();i++)
	{
		channel(i) = channelList.at(i);
	}
}

//-------------------------------------------------------------------------------------------

void AOQueryWasAPI::DeviceWasAPI::setInitialized()
{
	AOQueryDevice::Device::setInitialized();
	setHasExclusive(true);
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
