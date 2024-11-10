//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYDEVICE_H
#define __OMEGA_AUDIOIO_AOQUERYDEVICE_H
//-------------------------------------------------------------------------------------------

#include <QSet>

#include "engine/inc/RData.h"
#include "engine/inc/Codec.h"
#include "audioio/inc/AudioIODLL.h"
#include "audioio/inc/AOChannelMap.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
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
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

