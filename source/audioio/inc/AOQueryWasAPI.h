//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYWASAPI_H
#define __OMEGA_AUDIOIO_AOQUERYWASAPI_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryDevice.h"
#include "audioio/inc/WasAPIIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryWasAPI : public AOQueryDevice
{
	public:
		class DeviceWasAPI;

	public:
		AOQueryWasAPI();
		virtual ~AOQueryWasAPI();
		
		virtual bool queryNames();
		virtual bool queryDevice(int idx);
		
		virtual int defaultDeviceIndex();
};

//------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryWasAPI::DeviceWasAPI : public AOQueryDevice::Device
{
	public:
		DeviceWasAPI();
		DeviceWasAPI(const DeviceWasAPI& rhs);
		virtual ~DeviceWasAPI();

		WasAPIDeviceSPtr deviceInterface() const;
		void setDeviceInterface(WasAPIDeviceSPtr pDeviceIF);
		
		virtual AccessModeWasAPI accessMode() const;
		virtual void setAccessMode(AccessModeWasAPI mode);

		virtual bool isFrequencySupported(int freq) const;
		virtual void addFrequency(int freq);
		virtual const QSet<int>& frequencies() const;

		virtual int noChannels() const;
		virtual void setNoChannels(int noCh);
		virtual AOChannelMap *channelMap();

		virtual void loadChannelMap(bool mapChannelFromSettings = false);
		virtual void saveChannelMap();

		virtual void print() const;

		virtual void setInitialized();

	protected:
		AccessMode m_accessMode;
		QSet<int> m_frequencySetShared;
		AOChannelMap *m_channelMapShared;

		WasAPIDeviceSPtr m_pDeviceInterface;

		virtual void copy(const AOQueryDevice::Device& rhs);
		virtual bool isExclusiveFromAM() const;
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
