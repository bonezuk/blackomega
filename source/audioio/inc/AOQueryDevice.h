//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYDEVICE_H
#define __OMEGA_AUDIOIO_AOQUERYDEVICE_H
//-------------------------------------------------------------------------------------------

#include <QSet>

#include "engine/inc/RData.h"
#include "engine/inc/Codec.h"
#include "audioio/inc/AudioIODLL.h"
#include "audioio/inc/AudioSettings.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AOChannelMap;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryDevice
{
	public:
	
		class Channel;
		class Device;
	
	public:
		AOQueryDevice();
		virtual ~AOQueryDevice();
		
		virtual bool queryNames() = 0;
		virtual bool queryDevice(int idx) = 0;
		
		virtual int defaultDeviceIndex() = 0;
		
		virtual int noDevices() const;
		virtual const Device& device(int idx) const;
		virtual Device* deviceDirect(int idx);
		
		virtual void print() const;

	protected:
		
		QVector<Device *> m_devices;
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryDevice::Device
{
	public:
		
		typedef enum
		{
			e_deviceASIO = 1,
			e_deviceWasAPI,
			e_deviceCoreAudio,
			e_deviceALSA,
			e_deviceIOS,
			e_deviceUnknown = 0
		} Type;
		
	public:
		Device();
		Device(Type type);
		Device(const Device& rhs);
		virtual ~Device();

		const Device& operator = (const Device& rhs);
		
		virtual bool isInitialized() const;
		virtual void setInitialized();
		
        virtual const Type& type() const;
		
		virtual void clear();
		
		virtual QString& id();
		virtual const QString& idConst() const;
		
		virtual QString& name();
		virtual const QString& name() const;

		virtual bool isFrequencySupported(int freq) const;
		virtual void addFrequency(int freq);
		virtual const QSet<int>& frequencies() const;

		virtual int noChannels() const;
		virtual void setNoChannels(int noCh);
		virtual AOChannelMap *channelMap();
		virtual void loadChannelMap();
		virtual void saveChannelMap();
		
		// An audio device can be either a shared resource or exclusively used
		virtual bool hasExclusive() const;
        virtual void setHasExclusive(bool flag);

		// The API of an audio device is exclusive by its very nature (e.g. ASIO)
		virtual bool isAPIExclusive() const;
		
		virtual void print() const;

	protected:
	
		bool m_initFlag;
		Type m_type;
		QString m_id;
		QString m_name;
		QSet<int> m_frequencySet;
		AOChannelMap *m_channelMap;
		bool m_hasExclusive;

		virtual void copy(const Device& rhs);
};

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

class AUDIOIO_EXPORT AOChannelMap
{
	public:
		friend class AOQueryDevice::Device;
		
		typedef enum
		{
			e_Front = 0,
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
		
		virtual StereoType stereoType() const;
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
		virtual void copyForDevice(const AOChannelMap *pSource);
		
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

