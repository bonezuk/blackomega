//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AOQUERYASIO_H
#define __OMEGA_AUDIOIO_AOQUERYASIO_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryDevice.h"
#include "audioio/inc/ASIODriver.h"
#include "audioio/inc/ASIOData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryASIO : public AOQueryDevice
{
	public:
		class DeviceASIO;

	public:
		AOQueryASIO();
		virtual ~AOQueryASIO();
		
		virtual bool queryNames();
		virtual bool queryDevice(int idx);
		
		virtual int defaultDeviceIndex();
};

//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AOQueryASIO::DeviceASIO : public AOQueryDevice::Device
{
	public:
		DeviceASIO();
		DeviceASIO(const AOQueryDevice::Device& rhs);
		virtual ~DeviceASIO();

		virtual bool isAPIExclusive() const;

		virtual bool isDSDNative() const;
		virtual bool isDSDFrequencySupported(int freq, bool isNative) const;

		virtual void queryDSDCapabilities(ASIODriver *driver);
		
	protected:
		
		QSet<int> m_nativeDSDFrequencies;
		
		virtual void copy(const AOQueryDevice::Device& rhs);
		
		virtual int dsdOverPCMSampleType(ASIOSampleType type) const;
		virtual void queryDSDNativeCapabilities(ASIODriver *driver);
		virtual void queryDSDOverPCMCapabilities(ASIODriver *driver);
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
