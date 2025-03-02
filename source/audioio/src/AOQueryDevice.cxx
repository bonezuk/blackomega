#include "audioio/inc/AOQueryDevice.h"
#include "common/inc/Log.h"
#include <QSettings>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------
// AOQueryDevice::Device
//-------------------------------------------------------------------------------------------

AOQueryDevice::Device::Device() : m_initFlag(false),
	m_type(e_deviceUnknown),
	m_id(),
	m_name(),
	m_frequencySet(),
	m_channelMap(0),
	m_hasExclusive(false),
	m_dsdOverPcmSupport(0)
{
	m_channelMap = new AOChannelMap(*this);
}

//-------------------------------------------------------------------------------------------

AOQueryDevice::Device::Device(Type type) : m_initFlag(false),
	m_type(type),
	m_id(),
	m_name(),
	m_frequencySet(),
	m_channelMap(0),
	m_hasExclusive(false),
	m_dsdOverPcmSupport(0)
{
	m_channelMap = new AOChannelMap(*this);
}

//-------------------------------------------------------------------------------------------

AOQueryDevice::Device::Device(const Device& rhs) : m_initFlag(false),
	m_type(e_deviceUnknown),
	m_id(),
	m_name(),
	m_frequencySet(),
	m_channelMap(0),
	m_hasExclusive(false),
	m_dsdOverPcmSupport(0)
{
	m_channelMap = new AOChannelMap(*this);
	AOQueryDevice::Device::copy(rhs);
}

//-------------------------------------------------------------------------------------------

AOQueryDevice::Device::~Device()
{
	delete m_channelMap;
}

//-------------------------------------------------------------------------------------------

const AOQueryDevice::Device& AOQueryDevice::Device::operator = (const Device& rhs)
{
	if(this!=&rhs)
	{
		copy(rhs);
	}
	return *this;
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::copy(const Device& rhs)
{
	m_initFlag = rhs.m_initFlag;
	m_type = rhs.m_type;
	m_id = rhs.m_id;
	m_name = rhs.m_name;
	m_frequencySet = rhs.m_frequencySet;
	m_hasExclusive = rhs.m_hasExclusive;
	m_dsdOverPcmSupport = rhs.m_dsdOverPcmSupport;
	m_channelMap->copyForDevice(rhs.m_channelMap);
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::clear()
{
	m_initFlag = false;
	m_id = "";
	m_name = "";
	m_frequencySet.clear();
	m_hasExclusive = false;
	m_channelMap->setNoDeviceChannels(2);
}

//-------------------------------------------------------------------------------------------

bool AOQueryDevice::Device::isInitialized() const
{
	return m_initFlag;
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::setInitialized()
{
	m_initFlag = true;
}

//-------------------------------------------------------------------------------------------

const AOQueryDevice::Device::Type& AOQueryDevice::Device::type() const
{
	return m_type;
}

//-------------------------------------------------------------------------------------------

QString& AOQueryDevice::Device::id()
{
	return m_id;
}

//-------------------------------------------------------------------------------------------

const QString& AOQueryDevice::Device::idConst() const
{
	return m_id;
}

//-------------------------------------------------------------------------------------------

QString& AOQueryDevice::Device::name()
{
	return m_name;
}

//-------------------------------------------------------------------------------------------

const QString& AOQueryDevice::Device::name() const
{
	return m_name;
}

//-------------------------------------------------------------------------------------------

bool AOQueryDevice::Device::isFrequencySupported(int freq) const
{
	return (m_frequencySet.find(freq) != m_frequencySet.end());
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::addFrequency(int freq)
{
	m_frequencySet.insert(freq);
}

//-------------------------------------------------------------------------------------------

const QSet<int>& AOQueryDevice::Device::frequencies() const
{
	return m_frequencySet;
}

//-------------------------------------------------------------------------------------------

int AOQueryDevice::Device::noChannels() const
{
	return m_channelMap->noDeviceChannels();
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::setNoChannels(int noCh)
{
	m_channelMap->setNoDeviceChannels(noCh);
}

//-------------------------------------------------------------------------------------------

bool AOQueryDevice::Device::isAPIExclusive() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

bool AOQueryDevice::Device::hasExclusive() const
{
	return m_hasExclusive;
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::setHasExclusive(bool flag)
{
	m_hasExclusive = flag;
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::loadChannelMap(bool mapChannelFromSettings)
{
	loadCM(m_channelMap, mapChannelFromSettings);
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::saveChannelMap()
{
	saveCM(m_channelMap);
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::loadCM(AOChannelMap *chMap, bool mapChannelFromSettings)
{
	chMap->load(mapChannelFromSettings);
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::saveCM(AOChannelMap *chMap)
{
	chMap->save();
}

//-------------------------------------------------------------------------------------------

AOChannelMap *AOQueryDevice::Device::channelMap()
{
	return m_channelMap;
}

//-------------------------------------------------------------------------------------------

bool AOQueryDevice::Device::isDSDNative() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

int AOQueryDevice::Device::isDSDOverPCM() const
{
	return m_dsdOverPcmSupport;
}

//-------------------------------------------------------------------------------------------

bool AOQueryDevice::Device::isDSDFrequencySupported(int freq, bool isNative)
{
	bool res = false;
	
	if(!isNative && isDSDOverPCM())
	{
		switch(freq)
		{
			// DSD64
			case 2822400:
			case 3072000:
			// DSD128
			case 5644800:
			case 6144000:
			// DSD256
			case 11289600:
			case 12288000:
			// DSD512
			case 22579200:
			case 24576000:
			// DSD1024
			case 45158400:
			case 49152000:
				res = isFrequencySupported(freq / 16);
				break;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

AOQueryDevice::Device::DSDPlaybackMode AOQueryDevice::Device::validateDSDPlaybackMode(DSDPlaybackMode mode) const
{
	if(mode == e_DSDNative && !isDSDNative())
	{
		mode = e_DSDToPCM;
	}
	else if(mode == e_DSDOverPCM && !isDSDOverPCM())
	{
		mode = e_DSDToPCM;
	}
	return mode;
}

//-------------------------------------------------------------------------------------------

AOQueryDevice::Device::DSDPlaybackMode AOQueryDevice::Device::playbackModeOfDSDGroup(const QString& groupName) const
{
	DSDPlaybackMode mode = e_DSDToPCM;
	QSettings settings;
	
	settings.beginGroup(groupName);
	if(settings.contains("dsdmode"))
	{
		mode = static_cast<DSDPlaybackMode>(settings.value("dsdmode", QVariant(static_cast<int>(e_DSDToPCM))).toInt());
	}
	else
	{
		if(isDSDNative())
		{
			mode = e_DSDNative;
		}
		else if(isDSDOverPCM())
		{
			mode = e_DSDOverPCM;
		}
	}
	settings.endGroup();
	return validateDSDPlaybackMode(mode);
}

//-------------------------------------------------------------------------------------------

AOQueryDevice::Device::DSDPlaybackMode AOQueryDevice::Device::playbackModeOfDSD() const
{
	QString name = "audio" + m_name;
	return playbackModeOfDSDGroup(name);
}

//-------------------------------------------------------------------------------------------

bool AOQueryDevice::Device::setPlaybackModeOfDSDGroup(DSDPlaybackMode mode, const QString& groupName)
{
	bool res = false;
	
	if((mode == e_DSDNative && isDSDNative()) || (mode == e_DSDOverPCM && isDSDOverPCM()) || mode == e_DSDToPCM)
	{
		QSettings settings;
		
		settings.beginGroup(groupName);
		settings.setValue("dsdmode", QVariant(static_cast<int>(mode)));
		settings.endGroup();
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOQueryDevice::Device::setPlaybackModeOfDSD(DSDPlaybackMode mode)
{
	QString name = "audio" + m_name;
	return setPlaybackModeOfDSDGroup(mode, name);
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::Device::print() const
{
	int i;
	QList<int> sFreq;
	QSet<int>::const_iterator ppI;
	
	for(ppI=m_frequencySet.begin();ppI!=m_frequencySet.end();++ppI)
	{
		sFreq.append(*ppI);
	}
	std::sort(sFreq.begin(),sFreq.end());
	
	common::Log::g_Log.print("Device UUID : %s\n",m_id.toUtf8().constData());
	common::Log::g_Log.print("Device Name : %s\n",m_name.toUtf8().constData());
	m_channelMap->print();
	common::Log::g_Log.print("Frequencies : ");
	for(i=0;i<sFreq.size();i++)
	{
		common::Log::g_Log.print(" %d",sFreq.at(i));
	}
	common::Log::g_Log.print("\n");
}

//-------------------------------------------------------------------------------------------
// AOQueryDevice
//-------------------------------------------------------------------------------------------

AOQueryDevice::AOQueryDevice() : m_devices()
{}

//-------------------------------------------------------------------------------------------

AOQueryDevice::~AOQueryDevice()
{
	for(int i=0;i<m_devices.size();i++)
	{
		Device *dev = m_devices[i];
		delete dev;
	}
	m_devices.clear();
}

//-------------------------------------------------------------------------------------------

int AOQueryDevice::noDevices() const
{
	return m_devices.size();
}

//-------------------------------------------------------------------------------------------

const AOQueryDevice::Device& AOQueryDevice::device(int idx) const
{
	return *(m_devices.at(idx));
}

//-------------------------------------------------------------------------------------------

AOQueryDevice::Device* AOQueryDevice::deviceDirect(int idx)
{
	Device* dev = NULL;
	if(idx >= 0 && idx < m_devices.size())
	{
		dev = m_devices.at(idx);
	}
	return dev;
}

//-------------------------------------------------------------------------------------------

void AOQueryDevice::print() const
{
	int i;

	common::Log::g_Log.print("Number of Devices : %d\n\n",noDevices());
	for(i=0;i<noDevices();i++)
	{
		device(i).print();
	}
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
