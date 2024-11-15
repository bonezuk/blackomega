#include "audioio/inc/AOQueryDevice.h"

#include <QString>
#include <QSettings>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AOChannelMap::AOChannelMap(const AOQueryDevice::Device& dev) : m_device(dev),
	m_settingsKey(),
	m_channelMap(),
	m_noMappedChannels(-1),
	m_noDeviceChannels(2),
	m_stereoType(e_Front),
	m_isStereoCenter(true),
	m_isStereoLFE(true)
{
	AOChannelMap::defaultValues();
}

//-------------------------------------------------------------------------------------------

AOChannelMap::AOChannelMap(const AOQueryDevice::Device& dev, const QString& settingsKey) : m_device(dev),
	m_settingsKey(settingsKey),
	m_channelMap(),
	m_noMappedChannels(-1),
	m_noDeviceChannels(2),
	m_stereoType(e_Front),
	m_isStereoCenter(true),
	m_isStereoLFE(true)
{
	AOChannelMap::defaultValues();
}

//-------------------------------------------------------------------------------------------

AOChannelMap::~AOChannelMap()
{}

//-------------------------------------------------------------------------------------------

AOChannelMap::StereoType AOChannelMap::stereoType() const
{
	return m_stereoType;
}

//-------------------------------------------------------------------------------------------

bool AOChannelMap::setStereoType(StereoType t)
{
	bool isFrontValid = true, isSurroundValid = true, isRearValid = true;
	bool res = false;
	
	if(t == e_Front || t == e_FrontSurround || t == e_FrontRear || t == e_FrontSurroundRear)
	{
		if((!isValidChannel(e_FrontLeft) || !isValidChannel(e_FrontRight)) && !isValidChannel(e_Center))
		{
			isFrontValid = false;
		}
	}
	if(t == e_Surround || t ==  e_FrontSurround || t == e_SurroundRear || t == e_FrontSurroundRear)
	{
		if(!isValidChannel(e_SurroundLeft) || !isValidChannel(e_SurroundRight))
		{
			isSurroundValid = false;
		}
	}
	if(t == e_Rear || t == e_FrontRear || t == e_FrontRear || t == e_FrontSurroundRear)
	{
		if(!isValidChannel(e_RearLeft) || !isValidChannel(e_RearRight))
		{
			isRearValid = false;
		}
	}
	if(isFrontValid && isSurroundValid && isRearValid)
	{
		m_stereoType = t;
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

int AOChannelMap::noMappedChannels() const
{
	return (m_noMappedChannels >= 0) ? m_noMappedChannels : m_noDeviceChannels;
}

//-------------------------------------------------------------------------------------------

void AOChannelMap::setNoMappedChannels(int noChs)
{
	if(noChs > 0 && noChs <= m_noDeviceChannels)
	{
		saveChannels();
		m_noMappedChannels = noChs;
		defaultValues();
		loadChannels(noChs);
	}
}

//-------------------------------------------------------------------------------------------

int AOChannelMap::noDeviceChannels() const
{
	return m_noDeviceChannels;
}

//-------------------------------------------------------------------------------------------

void AOChannelMap::setNoDeviceChannels(int noChs)
{
	if(noChs > 0)
	{
		m_noDeviceChannels = noChs;
		m_noMappedChannels = -1;
		defaultValues();
	}	
}

//-------------------------------------------------------------------------------------------

int AOChannelMap::defaultChannelIndex(ChannelType t) const
{
	static const int c_defaultChannel[8][8] = {
		{ -1, -1,  0, -1, -1, -1, -1, -1 }, // 1 Speaker : Center
		{  0,  1, -1, -1, -1, -1, -1, -1 }, // 2 Speaker : Stereo
		{  0,  1,  2, -1, -1, -1, -1, -1 }, // 3 Speakers : Stereo + Center
		{  0,  1, -1, -1, -1, -1,  2,  3 }, // 4 Speakers : Quadrophonic
		{  0,  1,  2, -1, -1, -1,  3,  4 }, // 5 Speakers : Surround
		{  0,  1,  2,  3, -1, -1,  4,  5 }, // 5.1 Speakers : Surround + Subwoofer
		{  0,  1,  7, -1,  2,  3,  4,  5 }, // 7 Speakers : Full Surround
		{  0,  1,  2,  3,  6,  7,  4,  5 }, // 7.1 Speakers : Full Surround + Subwoofer
	};
	
	int idx;
	if(t >= e_FrontLeft && t < e_UnknownChannel)
	{
		idx = c_defaultChannel[noMappedChannels() - 1][static_cast<int>(t)];
	}
	else
	{
		idx = -1;
	}
	return idx;
}

//-------------------------------------------------------------------------------------------

bool AOChannelMap::isValidChannel(ChannelType t) const
{
	return (defaultChannelIndex(t) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

void AOChannelMap::defaultValues()
{
	bool isSurround = false, isRear = false;
	int chType;
	
	m_channelMap.clear();
	
	for(chType = 0; chType < 8; chType++)
	{
		ChannelType type = static_cast<ChannelType>(chType);
		int idx = defaultChannelIndex(type);
		if(idx >= 0)
		{
			m_channelMap.insert(type, idx);
		}
	}
	
	if(isValidChannel(e_SurroundLeft) && isValidChannel(e_SurroundRight))
	{
		isSurround = true;
	}
	if(isValidChannel(e_RearLeft) && isValidChannel(e_RearRight))
	{
		isRear = true;
	}
	if(isSurround && isRear)
	{
		m_stereoType = e_FrontSurroundRear;
	}
	else if(isSurround)
	{
		m_stereoType = e_FrontSurround;
	}
	else if(isRear)
	{
		m_stereoType = e_FrontRear;
	}
	else
	{
		m_stereoType = e_Front;
	}
	
	m_isStereoCenter = isValidChannel(e_Center);
	m_isStereoLFE = isValidChannel(e_LFE);
}

//-------------------------------------------------------------------------------------------

int AOChannelMap::channel(ChannelType t) const
{
	int idx;
	QMap<ChannelType, int>::const_iterator ppI = m_channelMap.find(t);
	if(ppI != m_channelMap.end())
	{
		idx = ppI.value();
	}
	else
	{
		idx = -1;
	}
	return idx;
}

//-------------------------------------------------------------------------------------------

bool AOChannelMap::setChannel(ChannelType t, int chIdx)
{
	bool res = false;

	if(isValidChannel(t) && chIdx < noDeviceChannels())
	{
		QMap<ChannelType, int>::iterator ppI, ppJ;
		ChannelType swapChannel, cType;
		int oldChIdx = -1;
			
		ppI = m_channelMap.find(t);
		if (ppI != m_channelMap.end())
		{
			oldChIdx = ppI.value();
			m_channelMap.erase(ppI);
		}
		if(chIdx >= 0)
		{
			swapChannel = indexAtChannel(chIdx);
			if(swapChannel < e_UnknownChannel && isValidChannel(swapChannel))
			{
				ppI = m_channelMap.find(swapChannel);
				if(ppI != m_channelMap.end())
				{
					if(oldChIdx < 0)
					{
						oldChIdx = defaultChannelIndex(swapChannel);
						cType = indexAtChannel(oldChIdx);
						if(oldChIdx == chIdx || cType != swapChannel)
						{
							oldChIdx = defaultChannelIndex(t);
							if(oldChIdx == chIdx || cType != swapChannel)
							{
								for (int c = 0; c < noMappedChannels(); c++)
								{
									if (indexAtChannel(c) == e_UnknownChannel)
									{
										oldChIdx = c;
										break;
									}
								}
							}
						}
					}
					m_channelMap.erase(ppI);
					m_channelMap.insert(swapChannel, oldChIdx);
				}
			}
		}
		m_channelMap.insert(t, chIdx);
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

ChannelType AOChannelMap::indexAtChannel(int idx) const
{
	ChannelType t = e_UnknownChannel;
	
	if(idx >= 0 && idx < noMappedChannels())
	{
		for(tint chIdx = 0; chIdx < static_cast<ChannelType>(e_UnknownChannel); chIdx++)
		{
			ChannelType type = static_cast<ChannelType>(chIdx);
			if(isValidChannel(type))
			{
				QMap<ChannelType,int>::const_iterator ppI = m_channelMap.find(type);
				if(ppI != m_channelMap.end() && ppI.value() == idx)
				{
					t = type;
					break;
				}
			}
		}
	}
	return t;
}

//-------------------------------------------------------------------------------------------

bool AOChannelMap::isStereoLFE() const
{
	return (isValidChannel(e_LFE)) ? m_isStereoLFE : false;
}

//-------------------------------------------------------------------------------------------

bool AOChannelMap::setStereoLFE(bool flag)
{
	bool res = false;
	if(isValidChannel(e_LFE))
	{
		m_isStereoLFE = flag;
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOChannelMap::isStereoCenter() const
{
	return (isValidChannel(e_Center)) ? m_isStereoCenter : false;
}

//-------------------------------------------------------------------------------------------

bool AOChannelMap::setStereoCenter(bool flag)
{
	bool res = false;
	if(isValidChannel(e_Center))
	{
		m_isStereoCenter = flag;
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void AOChannelMap::load(bool mapChannelFromSettings)
{
	int noMChs = -1;
	QSettings settings;
	QString groupName = settingsBaseName();
	
	if(m_noMappedChannels < 0 || mapChannelFromSettings)
	{
		settings.beginGroup(groupName);
		if (settings.contains("noMapChannels"))
		{
			noMChs = settings.value("noMapChannels", QVariant(-1)).toInt();
		}
		settings.endGroup();
	}
	else
	{
		noMChs = m_noMappedChannels;
	}
	
	if(noMChs > 0)
	{
		bool isValid;
		
		m_noMappedChannels = noMChs;
		defaultValues();
		isValid = loadChannels(noMChs);
		if(!isValid)
		{
			defaultValues();
		}
	}
}

//-------------------------------------------------------------------------------------------

QString AOChannelMap::settingsBaseName() const
{
	QString n = "audio" + m_device.name();
	if(!m_settingsKey.isEmpty())
	{
		n += "_" + m_settingsKey;
	}
	return n;
}

//-------------------------------------------------------------------------------------------

bool AOChannelMap::loadChannels(int noMChs)
{
	bool isValid = true;
	QSettings settings;
	QString groupName = settingsBaseName();

	groupName += "_" + QString::number(noMChs);
	settings.beginGroup(groupName);
	for(int chIdx = 0; chIdx < static_cast<int>(e_UnknownChannel) && isValid; chIdx++)
	{
		ChannelType chType = static_cast<ChannelType>(chIdx);

		if(isValidChannel(chType))
		{
			QString name = channelSettingsName(chType);
			if(settings.contains(name))
			{
				int idx = settings.value(name, QVariant(defaultChannelIndex(chType))).toInt();
				isValid = setChannel(chType, idx);
			}
		}
	}
	if(isValid)
	{
		if(settings.contains("stereoType"))
		{
			StereoType sType = static_cast<StereoType>(settings.value("stereoType", QVariant(static_cast<int>(m_stereoType))).toInt());
			isValid = setStereoType(sType);
		}
		if(settings.contains("isStereoCenter"))
		{
			m_isStereoCenter = settings.value("isStereoCenter", QVariant(m_isStereoCenter)).toBool();
		}
		if(settings.contains("isStereoLFE"))
		{
			m_isStereoLFE = settings.value("isStereoLFE", QVariant(m_isStereoLFE)).toBool();
		}
	}
	settings.endGroup();

	return isValid;
}

//-------------------------------------------------------------------------------------------

void AOChannelMap::saveChannels()
{
	QSettings settings;
	QString groupName = settingsBaseName();

	groupName += "_" + QString::number(noMappedChannels());
	settings.remove(groupName);

	settings.beginGroup(groupName);
	for(int chIdx = 0; chIdx < static_cast<int>(e_UnknownChannel); chIdx++)
	{
		ChannelType chType = static_cast<ChannelType>(chIdx);
		if(isValidChannel(chType))
		{
			QString name = channelSettingsName(chType);
			if(!name.isEmpty())
			{
				settings.setValue(name, QVariant(channel(chType)));
			}
		}
	}
	settings.setValue("stereoType", QVariant(static_cast<int>(m_stereoType)));
	settings.setValue("isStereoCenter", QVariant(m_isStereoCenter));
	settings.setValue("isStereoLFE", QVariant(m_isStereoLFE));
	settings.endGroup();
}

//-------------------------------------------------------------------------------------------

void AOChannelMap::save()
{
	QSettings settings;
	QString groupName = settingsBaseName();

	settings.beginGroup(groupName);
	settings.setValue("noMapChannels", QVariant(noMappedChannels()));
	settings.endGroup();

	saveChannels();
}

//-------------------------------------------------------------------------------------------

QString AOChannelMap::channelSettingsName(ChannelType t)
{
	QString name;
	
	switch(t)
	{
		case e_FrontLeft:
			name = "FrontLeft";
			break;
		case e_FrontRight:
			name = "FrontRight";
			break;
		case e_Center:
			name = "Center";
			break;
		case e_LFE:
			name = "LFE";
			break;
		case e_SurroundLeft:
			name = "SurroundLeft";
			break;
		case e_SurroundRight:
			name = "SurroundRight";
			break;
		case e_RearLeft:
			name = "RearLeft";
			break;
		case e_RearRight:
			name = "RearRight";
			break;
		default:
			break;
	}
	return name;
}

//-------------------------------------------------------------------------------------------

void AOChannelMap::copyForDevice(const AOChannelMap *pSource)
{
	m_settingsKey = pSource->m_settingsKey;
	setNoDeviceChannels(pSource->noDeviceChannels());
	m_noMappedChannels = pSource->m_noMappedChannels;
	for(int chIdx = 0; chIdx < static_cast<int>(e_UnknownChannel); chIdx++)
	{
		ChannelType chType = static_cast<ChannelType>(chIdx);
		int sIdx = pSource->channel(chType);
		setChannel(chType, sIdx);
	}
	setStereoType(pSource->stereoType());
	setStereoLFE(pSource->isStereoLFE());
	setStereoCenter(pSource->isStereoCenter());
}

//-------------------------------------------------------------------------------------------

void AOChannelMap::print()
{
	common::Log::g_Log.print("No. Device Channels: %d\n", noDeviceChannels());
	common::Log::g_Log.print("No. Mapped Channels: %d\n", noMappedChannels());
	common::Log::g_Log.print("Channels: ");
	for(int chIdx = 0; chIdx < static_cast<int>(e_UnknownChannel); chIdx++)
	{
		ChannelType chType = static_cast<ChannelType>(chIdx);
		if(isValidChannel(chType))
		{
			int sIdx = channel(chType);
			common::Log::g_Log.print("%s=%d, ", channelSettingsName(chType).toUtf8().constData(), sIdx);
		}
	}
	common::Log::g_Log.print("\n");
	
	QString stName;
	StereoType t = stereoType();
	if(t == e_Front || t == e_FrontSurround || t == e_FrontRear || t == e_FrontSurroundRear)
	{
		stName = "Front ";
	}
	if(t == e_Surround || t ==  e_FrontSurround || t == e_SurroundRear || t == e_FrontSurroundRear)
	{
		stName = "Surround ";
	}
	if(t == e_Rear || t == e_FrontRear || t == e_FrontRear || t == e_FrontSurroundRear)
	{
		stName = "Rear";
	}
	common::Log::g_Log.print("Stereo Map: %s\n", stName.toUtf8().constData());
	common::Log::g_Log.print("Stereo LFE: %s, Stereo Center: %s\n", isStereoLFE() ? "on" : "off", isStereoCenter() ? "on" : "off");
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
