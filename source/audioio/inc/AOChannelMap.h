//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOCHANNELMAP_H
#define __OMEGA_AUDIOIO_AOCHANNELMAP_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AudioSettings.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

typedef enum
{
	e_FrontLeft = 0,
	e_FrontRight = 1,
	e_Center = 2,
	e_LFE = 3,
	e_SurroundLeft = 4,
	e_SurroundRight = 5,
	e_RearLeft = 6,
	e_RearRight = 7,
	e_UnknownChannel = 8
} ChannelType;

//-------------------------------------------------------------------------------------------

class AOQueryDevice;
class AOQueryDevice::Device;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOChannelMap
{
	public:
		friend class AOQueryDevice::Device;
		
		typedef enum
		{c			e_Front = 0,
			e_Surround,
			e_Rear,
			e_FrontSurround,
			e_FrontRear,
			e_SurroundRear,
			e_FrontSurroundRear
		} StereoType;

	public:
		AOChannelMap(const AOQueryDevice::Device& dev);
		AOChannelMap(const AOQueryDevice::Device& dev, const QString& settingsKey);
		virtual ~AOChannelMap();
				
		virtual bool isValidChannel(ChannelType t) const;
		virtual int channel(ChannelType t) const;
		virtual bool setChannel(ChannelType t,int chIdx);
		
		// Returns the number that are actually mapped (This option can be set using the
		// ui.m_speakerCombo in SettingsAudio class from the Player interface)
		virtual int noMappedChannels() const;
		virtual void setNoMappedChannels(int noChs);
		
		// Returns the number of actual channels of the device
		virtual int noDeviceChannels() const;
		
		virtual StereoType stereoType();
		virtual bool setStereoType(StereoType t);
		
		virtual bool isStereoLFE() const;
		virtual bool setStereoLFE(bool flag);
		virtual bool isStereoCenter() const;
		virtual bool setStereoCenter(bool flag);
		
		virtual void print();

	protected:
	
		const AOQueryDevice::Device& m_device;
		QString m_settingsKey;
		QMap<ChannelType, int> m_channelMap;
		int m_noMappedChannels;
		int m_noDeviceChannels;
		StereoType m_stereoType;
		bool m_isStereoCenter;
		bool m_isStereoLFE;
		
		virtual void setNoDeviceChannels(int noChs);
		virtual void copyForDevice(QSharedPointer<AOChannelMap>& pSource);
		
		virtual int defaultChannelIndex(ChannelType t) const;
		virtual void defaultValues();

		virtual QString channelSettingsName(ChannelType t);
		virtual ChannelType indexAtChannel(int chIdx) const;
		
		virtual void load();
		virtual void save();
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------