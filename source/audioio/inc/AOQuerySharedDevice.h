//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYSHAREDDEVICE_H
#define __OMEGA_AUDIOIO_AOQUERYSHAREDDEVICE_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryDevice.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

typedef enum
{
	e_Settings = 0,
	e_Exclusive,
	e_Shared
} AccessModeSharedDevice;

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQuerySharedDevice : public AOQueryDevice::Device
{
	public:
		AOQuerySharedDevice();
		AOQuerySharedDevice(const AOQuerySharedDevice& rhs);
		virtual ~AOQuerySharedDevice();

		virtual AccessModeSharedDevice accessMode() const;
		virtual void setAccessMode(AccessModeSharedDevice mode);

		virtual bool isFrequencySupported(int freq) const;
		virtual void addFrequency(int freq);
		virtual const QSet<int>& frequencies() const;

		virtual int noChannels() const;
		virtual void setNoChannels(int noCh);
		virtual AOChannelMap *channelMap();

		virtual void loadChannelMap(bool mapChannelFromSettings = false);
		virtual void saveChannelMap();

	protected:
		AccessModeSharedDevice m_accessMode;
		QSet<int> m_frequencySetShared;
		AOChannelMap *m_channelMapShared;

		virtual void copy(const AOQueryDevice::Device& rhs);
		virtual bool isExclusiveFromAM() const;
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
