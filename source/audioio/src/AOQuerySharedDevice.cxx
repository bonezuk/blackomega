#include "audioio/inc/AOQuerySharedDevice.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AOQuerySharedDevice::AOQuerySharedDevice() : AOQueryDevice::Device(),
	m_accessMode(e_Settings),
	m_frequencySetShared(),
	m_channelMapShared(0),
	m_dsdOverPcmSupportShared(0)
{
	m_channelMapShared = new AOChannelMap(dynamic_cast<const AOQueryDevice::Device&>(*this), "shared");
}

//-------------------------------------------------------------------------------------------

AOQuerySharedDevice::AOQuerySharedDevice(const AOQuerySharedDevice& rhs) : AOQueryDevice::Device(),
	m_accessMode(e_Settings),
	m_frequencySetShared(),
	m_channelMapShared(0),
	m_dsdOverPcmSupportShared(0)
{
	m_channelMapShared = new AOChannelMap(dynamic_cast<const AOQueryDevice::Device&>(*this), "shared");
	copy(rhs);
}

//-------------------------------------------------------------------------------------------

AOQuerySharedDevice::~AOQuerySharedDevice()
{
	delete m_channelMapShared;
	m_channelMapShared = 0;
}

//-------------------------------------------------------------------------------------------

void AOQuerySharedDevice::copy(const AOQueryDevice::Device& rhs)
{
	const AOQuerySharedDevice& sDevice = dynamic_cast<const AOQuerySharedDevice&>(rhs);
	m_accessMode = sDevice.m_accessMode;
	m_frequencySetShared = sDevice.m_frequencySetShared;
	m_dsdOverPcmSupportShared = sDevice.m_dsdOverPcmSupportShared;
	m_channelMapShared->copyForDevice(sDevice.m_channelMapShared);
	AOQueryDevice::Device::copy(rhs);
}

//-------------------------------------------------------------------------------------------

AccessModeSharedDevice AOQuerySharedDevice::accessMode() const
{
	return m_accessMode;
}

//-------------------------------------------------------------------------------------------

void AOQuerySharedDevice::setAccessMode(AccessModeSharedDevice mode)
{
	m_accessMode = mode;
}

//-------------------------------------------------------------------------------------------

bool AOQuerySharedDevice::isExclusiveFromAM() const
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

bool AOQuerySharedDevice::isFrequencySupported(int freq) const
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

void AOQuerySharedDevice::addFrequency(int freq)
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

const QSet<int>& AOQuerySharedDevice::frequencies() const
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

int AOQuerySharedDevice::noChannels() const
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

void AOQuerySharedDevice::setNoChannels(int noCh)
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

AOChannelMap *AOQuerySharedDevice::channelMap()
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

void AOQuerySharedDevice::loadChannelMap(bool mapChannelFromSettings)
{
	loadCM(m_channelMap, mapChannelFromSettings);
	loadCM(m_channelMapShared, mapChannelFromSettings);
}

//-------------------------------------------------------------------------------------------

void AOQuerySharedDevice::saveChannelMap()
{
	saveCM(m_channelMap);
	saveCM(m_channelMapShared);
}

//-------------------------------------------------------------------------------------------

int AOQuerySharedDevice::isDSDOverPCM() const
{
	if(isExclusiveFromAM())
	{
		return m_dsdOverPcmSupport;
	}
	else
	{
		return m_dsdOverPcmSupportShared;
	}
}

//-------------------------------------------------------------------------------------------

AOQueryDevice::Device::DSDPlaybackMode AOQuerySharedDevice::playbackModeOfDSD() const
{
	QString name = "audio" + m_name + ((isExclusiveFromAM()) ? "exclusive" : "shared");
	return playbackModeOfDSDGroup(name);
}

//-------------------------------------------------------------------------------------------

bool AOQuerySharedDevice::setPlaybackModeOfDSD(AOQueryDevice::Device::DSDPlaybackMode mode)
{
	QString name = "audio" + m_name + ((isExclusiveFromAM()) ? "exclusive" : "shared");
	return setPlaybackModeOfDSDGroup(mode, name);
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
