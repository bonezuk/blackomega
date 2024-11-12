//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_TEST_AOQUERYDEVICEMOCK_H
#define __OMEGA_AUDIOIO_TEST_AOQUERYDEVICEMOCK_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AOQueryDevice.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AOQueryDeviceMock : public AOQueryDevice
{
	public:
		MOCK_METHOD0(queryNames,bool());
		MOCK_METHOD1(queryDevice,bool(int idx));
		
        MOCK_METHOD0(defaultDeviceIndex,int());
		
		MOCK_CONST_METHOD0(noDevices,int());
		MOCK_CONST_METHOD1(device,const AOQueryDevice::Device&(int idx));
		
		MOCK_CONST_METHOD0(print,void());
};

//-------------------------------------------------------------------------------------------

class AOQueryDeviceDeviceMock : public AOQueryDevice::Device
{
	public:
		MOCK_CONST_METHOD0(isInitialized,bool());
		MOCK_METHOD0(setInitialized,void());
		
		MOCK_METHOD0(id,QString&());
		MOCK_CONST_METHOD0(id,const QString&());
		
		MOCK_METHOD0(name,QString&());
		MOCK_CONST_METHOD0(name,const QString&());

		MOCK_CONST_METHOD1(isFrequencySupported,bool(int freq));
		MOCK_METHOD1(addFrequency,void(int freq));
		MOCK_CONST_METHOD0(frequencies,const QSet<int>&());

		MOCK_CONST_METHOD0(noChannels,int());
		MOCK_METHOD1(setNoChannels,void(int noCh));

		MOCK_CONST_METHOD0(print,void());
};
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
