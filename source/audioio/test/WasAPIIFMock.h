//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_WASAPIIFMOCK_H
#define __OMEGA_AUDIOIO_WASAPIIFMOCK_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/WasAPIIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class WasAPIIFMock : public WasAPIIF
{
	public:
		WasAPIIFMock();
		virtual ~WasAPIIFMock();
		
		MOCK_METHOD0(enumerateDeviceIds,QStringList());
		MOCK_METHOD1(getDevice,QSharedPointer<WasAPIDevice>(const QString& devID));
		
	protected:
	
		virtual bool init();
		virtual void done();
};

//-------------------------------------------------------------------------------------------

class WasAPIDeviceMock : public WasAPIDevice
{
	public:
		WasAPIDeviceMock();
		virtual ~WasAPIDeviceMock();

		MOCK_CONST_METHOD0(id,QString());
		MOCK_CONST_METHOD0(name,QString());

		MOCK_CONST_METHOD0(isExclusive, bool());
		MOCK_METHOD1(setExclusive, void(bool));

		MOCK_METHOD0(queryFrequencyCapabilities,QSet<int>());
		MOCK_METHOD0(queryChannelCapabilities, int());

		MOCK_METHOD1(findClosestSupportedFormat,WAVEFORMATEX *(const FormatDescription& sourceDesc));

		MOCK_METHOD0(getAudioClient,IAudioClientIFSPtr());
		MOCK_METHOD0(releaseAudioClient,void());

		MOCK_METHOD0(isDeviceVolume, bool());
		MOCK_METHOD0(getVolume, sample_t());
		MOCK_METHOD1(setVolume, bool(sample_t vol));

		MOCK_METHOD2(setupVolumeNotification, bool(VolumeChangeNotifier pNotifier, LPVOID pVInstance));
		MOCK_METHOD0(shutdownVolumeNotification, void());

	protected:
	
		virtual bool init(const QString& devID);
		virtual void done();
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
