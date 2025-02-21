//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYCOREAUDIO_H
#define __OMEGA_AUDIOIO_AOQUERYCOREAUDIO_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryDevice.h"
#include "audioio/inc/CoreAudioIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryCoreAudio : public AOQueryDevice
{
	public:
		class DeviceCoreAudio;

	public:
		AOQueryCoreAudio();
		virtual ~AOQueryCoreAudio();
		
		virtual bool queryNames();
		virtual bool queryDevice(int idx);

		virtual int defaultDeviceIndex();
		
	protected:
		
		virtual void printError(const tchar *strR,const tchar *strE,OSStatus err) const;
		
		virtual bool queryDevice(AudioDeviceID devID,Device& dev);
		virtual bool setupChannelLayout(AudioDeviceID devID,Device& dev);
		
		// virtual AudioChannelLayout *expandChannelLayout(AudioChannelLayout *ioLayout,UInt32 ioLayoutSize);
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryCoreAudio::DeviceCoreAudio : public AOQueryDevice::Device
{
	public:
		DeviceCoreAudio();
		DeviceCoreAudio(const AOQueryDevice::Device& rhs);
		virtual ~DeviceCoreAudio();
		
		virtual AudioDeviceID deviceID() const;
		virtual void setDeviceID(AudioDeviceID devID);

		virtual bool isIntegerMode() const;
		virtual void setIntegerMode(bool isInt);

		virtual bool isDSDFrequencySupported(int freq, bool isNative);
		virtual void setDSDOverPCM(int dsdFrequency, bool is24Bit);

		virtual void print() const;

	protected:
	
		AudioDeviceID m_deviceID;
		bool m_isIntegerMode;
		QMap<int, int> m_dsdFrequencies;
		
		virtual void copy(const AOQueryDevice::Device& rhs);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

