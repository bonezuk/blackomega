#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AOQueryWasAPI.h"
#include "audioio/inc/WasAPILayerIF.h"
#include "audioio/test/WasAPICOMInterfacesMock.h"
#include "audioio/test/WasAPIIFMock.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(AOQueryWasAPI,queryNamesGivenNoDevices)
{
    QStringList deviceNames;

    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,enumerateDeviceIds()).Times(1).WillOnce(Return(deviceNames));

    AOQueryWasAPI query;

    ASSERT_FALSE(query.queryNames());

    WasAPIIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryWasAPI,queryNamesGivenOneDeviceThatIsInvalid)
{
    QStringList deviceNames;
    QString deviceNameA = "Device A";

    deviceNames.append(deviceNameA);

    WasAPIDeviceSPtr pDeviceA;
    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,enumerateDeviceIds()).Times(1).WillOnce(Return(deviceNames));
    EXPECT_CALL(apiMock,getDevice(deviceNameA)).Times(1).WillOnce(Return(pDeviceA));

    AOQueryWasAPI query;

    ASSERT_FALSE(query.queryNames());

    WasAPIIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryWasAPI,queryNamesGivenOneValidDevice)
{
    QStringList deviceNames;
    QString deviceNameA = "Device A";
    QString friendlyNameA = "Friends A";

    deviceNames.append(deviceNameA);

    WasAPIDeviceSPtr pDeviceA(new WasAPIDeviceMock);
    WasAPIDeviceMock& deviceA = dynamic_cast<WasAPIDeviceMock&>(*(pDeviceA.data()));
    EXPECT_CALL(deviceA,id()).Times(1).WillOnce(Return(deviceNameA));
    EXPECT_CALL(deviceA,name()).Times(1).WillOnce(Return(friendlyNameA));

    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,enumerateDeviceIds()).Times(1).WillOnce(Return(deviceNames));
    EXPECT_CALL(apiMock,getDevice(deviceNameA)).Times(1).WillOnce(Return(pDeviceA));

    AOQueryWasAPI query;

    ASSERT_TRUE(query.queryNames());

    ASSERT_EQ(1,query.noDevices());
    EXPECT_TRUE(query.device(0).idConst()==deviceNameA);
    EXPECT_TRUE(query.device(0).name()==friendlyNameA);

    WasAPIIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryWasAPI,queryNamesGivenThreeValidDevices)
{
    QStringList deviceNames;
    QString deviceNameA = "Device A";
    QString deviceNameB = "Device B";
    QString deviceNameC = "Device C";
    QString friendlyNameA = "Friends A";
    QString friendlyNameB = "Friends B";
    QString friendlyNameC = "Friends C";

    deviceNames.append(deviceNameA);
    deviceNames.append(deviceNameB);
    deviceNames.append(deviceNameC);

    WasAPIDeviceSPtr pDeviceA(new WasAPIDeviceMock);
    WasAPIDeviceMock& deviceA = dynamic_cast<WasAPIDeviceMock&>(*(pDeviceA.data()));
    EXPECT_CALL(deviceA,id()).Times(1).WillOnce(Return(deviceNameA));
    EXPECT_CALL(deviceA,name()).Times(1).WillOnce(Return(friendlyNameA));

    WasAPIDeviceSPtr pDeviceB(new WasAPIDeviceMock);
    WasAPIDeviceMock& deviceB = dynamic_cast<WasAPIDeviceMock&>(*(pDeviceB.data()));
    EXPECT_CALL(deviceB,id()).Times(1).WillOnce(Return(deviceNameB));
    EXPECT_CALL(deviceB,name()).Times(1).WillOnce(Return(friendlyNameB));

    WasAPIDeviceSPtr pDeviceC(new WasAPIDeviceMock);
    WasAPIDeviceMock& deviceC = dynamic_cast<WasAPIDeviceMock&>(*(pDeviceC.data()));
    EXPECT_CALL(deviceC,id()).Times(1).WillOnce(Return(deviceNameC));
    EXPECT_CALL(deviceC,name()).Times(1).WillOnce(Return(friendlyNameC));

    WasAPIIFSPtr pAPI = WasAPIIF::instance("mock");
    WasAPIIFMock& apiMock = dynamic_cast<WasAPIIFMock&>(*(pAPI.data()));

    EXPECT_CALL(apiMock,enumerateDeviceIds()).Times(1).WillOnce(Return(deviceNames));
    EXPECT_CALL(apiMock,getDevice(deviceNameA)).Times(1).WillOnce(Return(pDeviceA));
    EXPECT_CALL(apiMock,getDevice(deviceNameB)).Times(1).WillOnce(Return(pDeviceB));
    EXPECT_CALL(apiMock,getDevice(deviceNameC)).Times(1).WillOnce(Return(pDeviceC));

    AOQueryWasAPI query;

    ASSERT_TRUE(query.queryNames());

    ASSERT_EQ(3,query.noDevices());
    EXPECT_TRUE(query.device(0).idConst()==deviceNameA);
    EXPECT_TRUE(query.device(0).name()==friendlyNameA);
    EXPECT_TRUE(query.device(1).idConst()==deviceNameB);
    EXPECT_TRUE(query.device(1).name()==friendlyNameB);
    EXPECT_TRUE(query.device(2).idConst()==deviceNameC);
    EXPECT_TRUE(query.device(2).name()==friendlyNameC);

    WasAPIIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AOQueryWasAPI,deviceHasExpectedType)
{
    AOQueryWasAPI::DeviceWasAPI deviceA;
    ASSERT_EQ(AOQueryDevice::Device::e_deviceWasAPI,deviceA.type());

    AOQueryWasAPI::DeviceWasAPI deviceB(deviceA);
    ASSERT_EQ(AOQueryDevice::Device::e_deviceWasAPI,deviceB.type());
}

//-------------------------------------------------------------------------------------------
