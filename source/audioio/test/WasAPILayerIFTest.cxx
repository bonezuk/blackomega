#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/WasAPILayerIF.h"

#include "audioio/test/WasAPICOMInterfacesMock.h"
#include "audioio/test/WasAPIIFMock.h"

#include "common/inc/Random.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class WasAPILayerIFTest : public WasAPILayerIF
{
    public:
        WasAPILayerIFTest();
        WasAPILayerIFTest(IMMDeviceEnumeratorIFSPtr pEnumerator);
        virtual ~WasAPILayerIFTest();
};

//-------------------------------------------------------------------------------------------

WasAPILayerIFTest::WasAPILayerIFTest() : WasAPILayerIF()
{}

//-------------------------------------------------------------------------------------------

WasAPILayerIFTest::WasAPILayerIFTest(IMMDeviceEnumeratorIFSPtr pEnumerator)  : WasAPILayerIF()
{
    m_pEnumerator = pEnumerator;
}

//-------------------------------------------------------------------------------------------

WasAPILayerIFTest::~WasAPILayerIFTest()
{}

//-------------------------------------------------------------------------------------------

LPWSTR WasAPILayerIFTestHelperBuildCOMString(const QString& txt)
{
    tint i;
    tuint16 *mem = reinterpret_cast<tuint16 *>(CoTaskMemAlloc((txt.length() + 1) * sizeof(tuint16)));
    
    for(i=0;i<txt.length();i++)
    {
        mem[i] = txt.at(i).unicode();
    }
    mem[txt.length()] = 0;
    return reinterpret_cast<LPWSTR>(mem);
}

//-------------------------------------------------------------------------------------------

class WasAPILayerIFEnumerateDeviceIdsTest : public WasAPILayerIFTest
{
    public:
        WasAPILayerIFEnumerateDeviceIdsTest(IMMDeviceEnumeratorIFSPtr pEnumerator);
        MOCK_METHOD1(createDeviceCollectionIF,IMMDeviceCollectionIFSPtr(IMMDeviceCollection *pCollection));
        MOCK_METHOD1(createDeviceIF,IMMDeviceIFSPtr(IMMDevice *pDevice));
};

//-------------------------------------------------------------------------------------------

WasAPILayerIFEnumerateDeviceIdsTest::WasAPILayerIFEnumerateDeviceIdsTest(IMMDeviceEnumeratorIFSPtr pEnumerator) : WasAPILayerIFTest(pEnumerator)
{}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,enumerateDeviceIdsGivenErrorEnumeratingActiveAudioEndPoints)
{
    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceCollection *iCollection = reinterpret_cast<IMMDeviceCollection *>(6);
    
    EXPECT_CALL(mockEnumerator,EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,A<IMMDeviceCollection **>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(iCollection),Return(S_FALSE)));
        
    WasAPILayerIFEnumerateDeviceIdsTest layer(pEnumerator);
    
    QStringList deviceList = layer.enumerateDeviceIds();
    
    ASSERT_EQ(0,deviceList.size());
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,enumerateDeviceIdsGivenNoDeviceCollection)
{
    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceCollection *iCollection = reinterpret_cast<IMMDeviceCollection *>(0);
    
    EXPECT_CALL(mockEnumerator,EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,A<IMMDeviceCollection **>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(iCollection),Return(S_OK)));
        
    WasAPILayerIFEnumerateDeviceIdsTest layer(pEnumerator);
    
    QStringList deviceList = layer.enumerateDeviceIds();
    
    ASSERT_EQ(0,deviceList.size());
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,enumerateDeviceIdsGivenErrorCountingDevices)
{
    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceCollectionIFSPtr pCollection(new IMMDeviceCollectionIFMock());
    IMMDeviceCollectionIFMock& mockCollection = dynamic_cast<IMMDeviceCollectionIFMock&>(*(pCollection.data()));
    IMMDeviceCollection *iCollection = reinterpret_cast<IMMDeviceCollection *>(4);
    
    EXPECT_CALL(mockEnumerator,EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,A<IMMDeviceCollection **>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(iCollection),Return(S_OK)));
    
    EXPECT_CALL(mockCollection,GetCount(A<UINT *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(4),Return(S_FALSE)));
    
    WasAPILayerIFEnumerateDeviceIdsTest layer(pEnumerator);
    
    EXPECT_CALL(layer,createDeviceCollectionIF(iCollection)).Times(1).WillOnce(Return(pCollection));
    
    QStringList deviceList = layer.enumerateDeviceIds();
    
    ASSERT_EQ(0,deviceList.size());
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,enumerateDeviceIdsGivenNoDevices)
{
    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceCollectionIFSPtr pCollection(new IMMDeviceCollectionIFMock());
    IMMDeviceCollectionIFMock& mockCollection = dynamic_cast<IMMDeviceCollectionIFMock&>(*(pCollection.data()));
    IMMDeviceCollection *iCollection = reinterpret_cast<IMMDeviceCollection *>(4);
    
    EXPECT_CALL(mockEnumerator,EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,A<IMMDeviceCollection **>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(iCollection),Return(S_OK)));
    
    EXPECT_CALL(mockCollection,GetCount(A<UINT *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(0),Return(S_OK)));
    
    WasAPILayerIFEnumerateDeviceIdsTest layer(pEnumerator);
    
    EXPECT_CALL(layer,createDeviceCollectionIF(iCollection)).Times(1).WillOnce(Return(pCollection));
    
    QStringList deviceList = layer.enumerateDeviceIds();
    
    ASSERT_EQ(0,deviceList.size());
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,enumerateDeviceIdsGiven1ActiveDevice)
{
    QString nameA = "ASUS Xonar DS2";
    LPWSTR deviceNameA = WasAPILayerIFTestHelperBuildCOMString(nameA);
    
    IMMDeviceIFSPtr pDeviceA(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceA = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceA.data()));
    IMMDevice *iDeviceA = reinterpret_cast<IMMDevice *>(5);
    EXPECT_CALL(mockDeviceA,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameA),Return(S_OK)));

    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceCollectionIFSPtr pCollection(new IMMDeviceCollectionIFMock());
    IMMDeviceCollectionIFMock& mockCollection = dynamic_cast<IMMDeviceCollectionIFMock&>(*(pCollection.data()));
    IMMDeviceCollection *iCollection = reinterpret_cast<IMMDeviceCollection *>(4);
    
    EXPECT_CALL(mockEnumerator,EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,A<IMMDeviceCollection **>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(iCollection),Return(S_OK)));
    
    EXPECT_CALL(mockCollection,GetCount(A<UINT *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(1),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(0,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceA),Return(S_OK)));
    
    WasAPILayerIFEnumerateDeviceIdsTest layer(pEnumerator);
    
    EXPECT_CALL(layer,createDeviceCollectionIF(iCollection)).Times(1).WillOnce(Return(pCollection));
    EXPECT_CALL(layer,createDeviceIF(iDeviceA)).Times(1).WillOnce(Return(pDeviceA));
    
    QStringList deviceList = layer.enumerateDeviceIds();
    
    ASSERT_EQ(1,deviceList.size());
    EXPECT_TRUE(deviceList.at(0)==nameA);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,enumerateDeviceIdsGiven3ActiveDevicesButErrorGivenWhenGettingFirstDevice)
{
    QString nameB = "iFi DSD USB";
    QString nameC = "AirPlay";
    
    LPWSTR deviceNameB = WasAPILayerIFTestHelperBuildCOMString(nameB);
    LPWSTR deviceNameC = WasAPILayerIFTestHelperBuildCOMString(nameC);
    
    IMMDevice *iDeviceA = reinterpret_cast<IMMDevice *>(5);

    IMMDeviceIFSPtr pDeviceB(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceB = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceB.data()));
    IMMDevice *iDeviceB = reinterpret_cast<IMMDevice *>(6);
    EXPECT_CALL(mockDeviceB,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameB),Return(S_OK)));

    IMMDeviceIFSPtr pDeviceC(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceC = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceC.data()));
    IMMDevice *iDeviceC = reinterpret_cast<IMMDevice *>(7);
    EXPECT_CALL(mockDeviceC,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameC),Return(S_OK)));

    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceCollectionIFSPtr pCollection(new IMMDeviceCollectionIFMock());
    IMMDeviceCollectionIFMock& mockCollection = dynamic_cast<IMMDeviceCollectionIFMock&>(*(pCollection.data()));
    IMMDeviceCollection *iCollection = reinterpret_cast<IMMDeviceCollection *>(4);
    
    EXPECT_CALL(mockEnumerator,EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,A<IMMDeviceCollection **>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(iCollection),Return(S_OK)));
    
    EXPECT_CALL(mockCollection,GetCount(A<UINT *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(3),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(0,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceA),Return(S_FALSE)));
    EXPECT_CALL(mockCollection,Item(1,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceB),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(2,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceC),Return(S_OK)));
    
    WasAPILayerIFEnumerateDeviceIdsTest layer(pEnumerator);
    
    EXPECT_CALL(layer,createDeviceCollectionIF(iCollection)).Times(1).WillOnce(Return(pCollection));
    EXPECT_CALL(layer,createDeviceIF(iDeviceB)).Times(1).WillOnce(Return(pDeviceB));
    EXPECT_CALL(layer,createDeviceIF(iDeviceC)).Times(1).WillOnce(Return(pDeviceC));
    
    QStringList deviceList = layer.enumerateDeviceIds();
    
    ASSERT_EQ(2,deviceList.size());
    EXPECT_TRUE(deviceList.at(0)==nameB);
    EXPECT_TRUE(deviceList.at(1)==nameC);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,enumerateDeviceIdsGiven3ActiveDevicesButFailToGetFirstDevice)
{
    QString nameB = "iFi DSD USB";
    QString nameC = "AirPlay";
    
    LPWSTR deviceNameB = WasAPILayerIFTestHelperBuildCOMString(nameB);
    LPWSTR deviceNameC = WasAPILayerIFTestHelperBuildCOMString(nameC);

    IMMDevice *iDeviceA = 0;

    IMMDeviceIFSPtr pDeviceB(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceB = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceB.data()));
    IMMDevice *iDeviceB = reinterpret_cast<IMMDevice *>(6);
    EXPECT_CALL(mockDeviceB,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameB),Return(S_OK)));

    IMMDeviceIFSPtr pDeviceC(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceC = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceC.data()));
    IMMDevice *iDeviceC = reinterpret_cast<IMMDevice *>(7);
    EXPECT_CALL(mockDeviceC,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameC),Return(S_OK)));

    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceCollectionIFSPtr pCollection(new IMMDeviceCollectionIFMock());
    IMMDeviceCollectionIFMock& mockCollection = dynamic_cast<IMMDeviceCollectionIFMock&>(*(pCollection.data()));
    IMMDeviceCollection *iCollection = reinterpret_cast<IMMDeviceCollection *>(4);
    
    EXPECT_CALL(mockEnumerator,EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,A<IMMDeviceCollection **>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(iCollection),Return(S_OK)));
    
    EXPECT_CALL(mockCollection,GetCount(A<UINT *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(3),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(0,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceA),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(1,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceB),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(2,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceC),Return(S_OK)));
    
    WasAPILayerIFEnumerateDeviceIdsTest layer(pEnumerator);
    
    EXPECT_CALL(layer,createDeviceCollectionIF(iCollection)).Times(1).WillOnce(Return(pCollection));
    EXPECT_CALL(layer,createDeviceIF(iDeviceB)).Times(1).WillOnce(Return(pDeviceB));
    EXPECT_CALL(layer,createDeviceIF(iDeviceC)).Times(1).WillOnce(Return(pDeviceC));
    
    QStringList deviceList = layer.enumerateDeviceIds();
    
    ASSERT_EQ(2,deviceList.size());
    EXPECT_TRUE(deviceList.at(0)==nameB);
    EXPECT_TRUE(deviceList.at(1)==nameC);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,enumerateDeviceIdsGiven3ActiveDevicesButErrorGettingIdOfSecondDevice)
{
    QString nameA = "ASUS Xonar DS2";
    QString nameC = "AirPlay";
    
    LPWSTR deviceNameA = WasAPILayerIFTestHelperBuildCOMString(nameA);
    LPWSTR deviceNameC = WasAPILayerIFTestHelperBuildCOMString(nameC);
    
    IMMDeviceIFSPtr pDeviceA(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceA = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceA.data()));
    IMMDevice *iDeviceA = reinterpret_cast<IMMDevice *>(5);
    EXPECT_CALL(mockDeviceA,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameA),Return(S_OK)));

    IMMDevice *iDeviceB = reinterpret_cast<IMMDevice *>(6);

    IMMDeviceIFSPtr pDeviceC(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceC = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceC.data()));
    IMMDevice *iDeviceC = reinterpret_cast<IMMDevice *>(7);
    EXPECT_CALL(mockDeviceC,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameC),Return(S_OK)));

    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceCollectionIFSPtr pCollection(new IMMDeviceCollectionIFMock());
    IMMDeviceCollectionIFMock& mockCollection = dynamic_cast<IMMDeviceCollectionIFMock&>(*(pCollection.data()));
    IMMDeviceCollection *iCollection = reinterpret_cast<IMMDeviceCollection *>(4);
    
    EXPECT_CALL(mockEnumerator,EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,A<IMMDeviceCollection **>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(iCollection),Return(S_OK)));
    
    EXPECT_CALL(mockCollection,GetCount(A<UINT *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(3),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(0,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceA),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(1,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceB),Return(S_FALSE)));
    EXPECT_CALL(mockCollection,Item(2,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceC),Return(S_OK)));
    
    WasAPILayerIFEnumerateDeviceIdsTest layer(pEnumerator);
    
    EXPECT_CALL(layer,createDeviceCollectionIF(iCollection)).Times(1).WillOnce(Return(pCollection));
    EXPECT_CALL(layer,createDeviceIF(iDeviceA)).Times(1).WillOnce(Return(pDeviceA));
    EXPECT_CALL(layer,createDeviceIF(iDeviceC)).Times(1).WillOnce(Return(pDeviceC));
    
    QStringList deviceList = layer.enumerateDeviceIds();
    
    ASSERT_EQ(2,deviceList.size());
    EXPECT_TRUE(deviceList.at(0)==nameA);
    EXPECT_TRUE(deviceList.at(1)==nameC);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,enumerateDeviceIdsGiven3ActiveDevicesButNoNameGivenForSecondDevice)
{
    QString nameA = "ASUS Xonar DS2";
    QString nameC = "AirPlay";
    
    LPWSTR deviceNameA = WasAPILayerIFTestHelperBuildCOMString(nameA);
    LPWSTR deviceNameB = 0;
    LPWSTR deviceNameC = WasAPILayerIFTestHelperBuildCOMString(nameC);
    
    IMMDeviceIFSPtr pDeviceA(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceA = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceA.data()));
    IMMDevice *iDeviceA = reinterpret_cast<IMMDevice *>(5);
    EXPECT_CALL(mockDeviceA,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameA),Return(S_OK)));

    IMMDeviceIFSPtr pDeviceB(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceB = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceB.data()));
    IMMDevice *iDeviceB = reinterpret_cast<IMMDevice *>(6);
    EXPECT_CALL(mockDeviceB,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameB),Return(S_OK)));

    IMMDeviceIFSPtr pDeviceC(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceC = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceC.data()));
    IMMDevice *iDeviceC = reinterpret_cast<IMMDevice *>(7);
    EXPECT_CALL(mockDeviceC,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameC),Return(S_OK)));

    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceCollectionIFSPtr pCollection(new IMMDeviceCollectionIFMock());
    IMMDeviceCollectionIFMock& mockCollection = dynamic_cast<IMMDeviceCollectionIFMock&>(*(pCollection.data()));
    IMMDeviceCollection *iCollection = reinterpret_cast<IMMDeviceCollection *>(4);
    
    EXPECT_CALL(mockEnumerator,EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,A<IMMDeviceCollection **>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(iCollection),Return(S_OK)));
    
    EXPECT_CALL(mockCollection,GetCount(A<UINT *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(3),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(0,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceA),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(1,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceB),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(2,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceC),Return(S_OK)));
    
    WasAPILayerIFEnumerateDeviceIdsTest layer(pEnumerator);
    
    EXPECT_CALL(layer,createDeviceCollectionIF(iCollection)).Times(1).WillOnce(Return(pCollection));
    EXPECT_CALL(layer,createDeviceIF(iDeviceA)).Times(1).WillOnce(Return(pDeviceA));
    EXPECT_CALL(layer,createDeviceIF(iDeviceB)).Times(1).WillOnce(Return(pDeviceB));
    EXPECT_CALL(layer,createDeviceIF(iDeviceC)).Times(1).WillOnce(Return(pDeviceC));
    
    QStringList deviceList = layer.enumerateDeviceIds();

    ASSERT_EQ(2,deviceList.size());
    EXPECT_TRUE(deviceList.at(0)==nameA);
    EXPECT_TRUE(deviceList.at(1)==nameC);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,enumerateDeviceIdsGiven3ActiveDevices)
{
    QString nameA = "ASUS Xonar DS2";
    QString nameB = "iFi DSD USB";
    QString nameC = "AirPlay";
    
    LPWSTR deviceNameA = WasAPILayerIFTestHelperBuildCOMString(nameA);
    LPWSTR deviceNameB = WasAPILayerIFTestHelperBuildCOMString(nameB);
    LPWSTR deviceNameC = WasAPILayerIFTestHelperBuildCOMString(nameC);
    
    IMMDeviceIFSPtr pDeviceA(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceA = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceA.data()));
    IMMDevice *iDeviceA = reinterpret_cast<IMMDevice *>(5);
    EXPECT_CALL(mockDeviceA,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameA),Return(S_OK)));

    IMMDeviceIFSPtr pDeviceB(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceB = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceB.data()));
    IMMDevice *iDeviceB = reinterpret_cast<IMMDevice *>(6);
    EXPECT_CALL(mockDeviceB,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameB),Return(S_OK)));

    IMMDeviceIFSPtr pDeviceC(new IMMDeviceIFMock);
    IMMDeviceIFMock& mockDeviceC = dynamic_cast<IMMDeviceIFMock&>(*(pDeviceC.data()));
    IMMDevice *iDeviceC = reinterpret_cast<IMMDevice *>(7);
    EXPECT_CALL(mockDeviceC,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(deviceNameC),Return(S_OK)));

    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceCollectionIFSPtr pCollection(new IMMDeviceCollectionIFMock());
    IMMDeviceCollectionIFMock& mockCollection = dynamic_cast<IMMDeviceCollectionIFMock&>(*(pCollection.data()));
    IMMDeviceCollection *iCollection = reinterpret_cast<IMMDeviceCollection *>(4);
    
    EXPECT_CALL(mockEnumerator,EnumAudioEndpoints(eRender,DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED,A<IMMDeviceCollection **>())).Times(1)
        .WillOnce(DoAll(SetArgPointee<2>(iCollection),Return(S_OK)));
    
    EXPECT_CALL(mockCollection,GetCount(A<UINT *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(3),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(0,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceA),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(1,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceB),Return(S_OK)));
    EXPECT_CALL(mockCollection,Item(2,A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDeviceC),Return(S_OK)));
    
    WasAPILayerIFEnumerateDeviceIdsTest layer(pEnumerator);
    
    EXPECT_CALL(layer,createDeviceCollectionIF(iCollection)).Times(1).WillOnce(Return(pCollection));
    EXPECT_CALL(layer,createDeviceIF(iDeviceA)).Times(1).WillOnce(Return(pDeviceA));
    EXPECT_CALL(layer,createDeviceIF(iDeviceB)).Times(1).WillOnce(Return(pDeviceB));
    EXPECT_CALL(layer,createDeviceIF(iDeviceC)).Times(1).WillOnce(Return(pDeviceC));
    
    QStringList deviceList = layer.enumerateDeviceIds();
    
    ASSERT_EQ(3,deviceList.size());
    EXPECT_TRUE(deviceList.at(0)==nameA);
    EXPECT_TRUE(deviceList.at(1)==nameB);
    EXPECT_TRUE(deviceList.at(2)==nameC);
}

//-------------------------------------------------------------------------------------------

class WasAPIDeviceLayerTest : public WasAPIDeviceLayer
{
    public:
        WasAPIDeviceLayerTest();
        WasAPIDeviceLayerTest(IMMDeviceIFSPtr pDevice,IAudioClientIFSPtr pAudioClient);
        virtual ~WasAPIDeviceLayerTest();
        
        void testDefaultWaveFormat(WAVEFORMATEX& format) const;
        void testSetWaveFormat(int noChannels,int noBits,int frequency,WAVEFORMATEX& format) const;
        
        int testGetNumberOfBitsFromIndex(int idx) const;
        int testGetNumberOfChannelsFromIndex(int idx) const;
        int testGetFrequencyFromIndex(int idx) const;

        tint testGetNoOfBitsFromWaveFormat(const WAVEFORMATEX *pFormat) const;
        tint testGetNoOfChannelsFromWaveFormat(const WAVEFORMATEX *pFormat) const;
        tint testGetFrequencyFromWaveFormat(const WAVEFORMATEX *pFormat) const;

        tint format(int i, int j, int k, bool exclusive);
        tint formatExclusive(int i,int j,int k);
        tint formatShared(int i,int j,int k);

        void setFormat(int i, int j, int k, bool exclusive, int value);
        void setFormatExclusive(int i,int j,int k,int value);
        void setFormatShared(int i,int j,int k,int value);
        
        void testBlank();
        bool testLoadFormats();
        bool testSaveFormats();
        
        int testGetIndexOfBits(const WAVEFORMATEX *pFormat) const;
        int testGetIndexOfChannels(const WAVEFORMATEX *pFormat) const;
        int testGetIndexOfFrequency(const WAVEFORMATEX *pFormat) const;
        
        void testSetFrequencyInWaveFormat(int frequency,WAVEFORMATEX *pFormat) const;
        void testSetChannelsInWaveFormat(int channel,WAVEFORMATEX *pFormat) const;
};

//-------------------------------------------------------------------------------------------

WasAPIDeviceLayerTest::WasAPIDeviceLayerTest() : WasAPIDeviceLayer()
{}

//-------------------------------------------------------------------------------------------

WasAPIDeviceLayerTest::WasAPIDeviceLayerTest(IMMDeviceIFSPtr pDevice,IAudioClientIFSPtr pAudioClient) : WasAPIDeviceLayer()
{
    m_pDevice = pDevice;
    m_pAudioClient = pAudioClient;
}

//-------------------------------------------------------------------------------------------

WasAPIDeviceLayerTest::~WasAPIDeviceLayerTest()
{}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayerTest::testDefaultWaveFormat(WAVEFORMATEX& format) const
{
    defaultWaveFormat(format);
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayerTest::testSetWaveFormat(int noChannels,int noBits,int frequency,WAVEFORMATEX& format) const
{
    setWaveFormat(noChannels,noBits,frequency,format);
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayerTest::testGetNumberOfBitsFromIndex(int idx) const
{
    return getNumberOfBitsFromIndex(idx);
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayerTest::testGetNumberOfChannelsFromIndex(int idx) const
{
    return getNumberOfChannelsFromIndex(idx);
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayerTest::testGetFrequencyFromIndex(int idx) const
{
    return getFrequencyFromIndex(idx);
}

//-------------------------------------------------------------------------------------------

tint WasAPIDeviceLayerTest::testGetNoOfBitsFromWaveFormat(const WAVEFORMATEX *pFormat) const
{
    return getNoOfBitsFromWaveFormat(pFormat);
}

//-------------------------------------------------------------------------------------------

tint WasAPIDeviceLayerTest::testGetNoOfChannelsFromWaveFormat(const WAVEFORMATEX *pFormat) const
{
    return getNoOfChannelsFromWaveFormat(pFormat);
}

//-------------------------------------------------------------------------------------------

tint WasAPIDeviceLayerTest::testGetFrequencyFromWaveFormat(const WAVEFORMATEX *pFormat) const
{
    return getFrequencyFromWaveFormat(pFormat);
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayerTest::setFormatExclusive(int i,int j,int k,int value)
{
    m_formatsExclusive[i][j][k] = value;
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayerTest::setFormatShared(int i,int j,int k,int value)
{
    m_formatsShared[i][j][k] = value;
}

//-------------------------------------------------------------------------------------------

tint WasAPIDeviceLayerTest::format(int i, int j, int k, bool exclusive)
{
    return (exclusive) ? formatExclusive(i, j, k) : formatShared(i, j, k);
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayerTest::setFormat(int i, int j, int k, bool exclusive, int value)
{
    if(exclusive)
    {
        setFormatExclusive(i, j, k, value);
    }
    else
    {
        setFormatShared(i, j, k, value);
    }
}

//-------------------------------------------------------------------------------------------

tint WasAPIDeviceLayerTest::formatExclusive(int i,int j,int k)
{
    return m_formatsExclusive[i][j][k];
}

//-------------------------------------------------------------------------------------------

tint WasAPIDeviceLayerTest::formatShared(int i,int j,int k)
{
    return m_formatsShared[i][j][k];
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayerTest::testBlank()
{
    blank();
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayerTest::testLoadFormats()
{
    return loadFormats();
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayerTest::testSaveFormats()
{
    return saveFormats();
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayerTest::testGetIndexOfBits(const WAVEFORMATEX *pFormat) const
{
    return getIndexOfBits(pFormat);
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayerTest::testGetIndexOfChannels(const WAVEFORMATEX *pFormat) const
{
    return getIndexOfChannels(pFormat);
}

//-------------------------------------------------------------------------------------------

int WasAPIDeviceLayerTest::testGetIndexOfFrequency(const WAVEFORMATEX *pFormat) const
{
    return getIndexOfFrequency(pFormat);
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayerTest::testSetFrequencyInWaveFormat(int frequency,WAVEFORMATEX *pFormat) const
{
    setFrequencyInWaveFormat(frequency,pFormat);
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayerTest::testSetChannelsInWaveFormat(int channel,WAVEFORMATEX *pFormat) const
{
    setChannelsInWaveFormat(channel,pFormat);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,defaultWaveFormat)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    
    EXPECT_EQ(static_cast<int>(WasAPIDeviceLayer::e_formatPCM),static_cast<int>(format.wFormatTag));
    EXPECT_EQ(2,format.nChannels);
    EXPECT_EQ(44100,format.nSamplesPerSec);
    EXPECT_EQ(44100 * 2 * 2,format.nAvgBytesPerSec);
    EXPECT_EQ(4,format.nBlockAlign);
    EXPECT_EQ(16,format.wBitsPerSample);
    EXPECT_EQ(0,format.cbSize);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,setWaveFormat)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,192000,format);
    
    EXPECT_EQ(static_cast<int>(WasAPIDeviceLayer::e_formatPCM),static_cast<int>(format.wFormatTag));
    EXPECT_EQ(8,format.nChannels);
    EXPECT_EQ(192000,format.nSamplesPerSec);
    EXPECT_EQ(192000 * 3 * 8,format.nAvgBytesPerSec);
    EXPECT_EQ(3 * 8,format.nBlockAlign);
    EXPECT_EQ(24,format.wBitsPerSample);
    EXPECT_EQ(0,format.cbSize);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNumberOfBitsFromIndex)
{
    WasAPIDeviceLayerTest device;
    
    EXPECT_EQ(16,device.testGetNumberOfBitsFromIndex(0));
    EXPECT_EQ(20,device.testGetNumberOfBitsFromIndex(1));
    EXPECT_EQ(24,device.testGetNumberOfBitsFromIndex(2));
    EXPECT_EQ(28,device.testGetNumberOfBitsFromIndex(3));
    EXPECT_EQ(32, device.testGetNumberOfBitsFromIndex(4));
    EXPECT_EQ(-1,device.testGetNumberOfBitsFromIndex(5));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNumberOfChannelsFromIndex)
{
    WasAPIDeviceLayerTest device;
    
    EXPECT_EQ(1,device.testGetNumberOfChannelsFromIndex(0));
    EXPECT_EQ(2,device.testGetNumberOfChannelsFromIndex(1));
    EXPECT_EQ(3,device.testGetNumberOfChannelsFromIndex(2));
    EXPECT_EQ(4,device.testGetNumberOfChannelsFromIndex(3));
    EXPECT_EQ(5,device.testGetNumberOfChannelsFromIndex(4));
    EXPECT_EQ(6,device.testGetNumberOfChannelsFromIndex(5));
    EXPECT_EQ(7,device.testGetNumberOfChannelsFromIndex(6));
    EXPECT_EQ(8,device.testGetNumberOfChannelsFromIndex(7));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromIndex)
{
    WasAPIDeviceLayerTest device;
    
    EXPECT_EQ(8000,device.testGetFrequencyFromIndex(0));
    EXPECT_EQ(11025,device.testGetFrequencyFromIndex(1));
    EXPECT_EQ(12000,device.testGetFrequencyFromIndex(2));
    EXPECT_EQ(16000,device.testGetFrequencyFromIndex(3));
    EXPECT_EQ(22050,device.testGetFrequencyFromIndex(4));
    EXPECT_EQ(24000,device.testGetFrequencyFromIndex(5));
    EXPECT_EQ(32000,device.testGetFrequencyFromIndex(6));
    EXPECT_EQ(44100,device.testGetFrequencyFromIndex(7));
    EXPECT_EQ(48000,device.testGetFrequencyFromIndex(8));
    EXPECT_EQ(64000,device.testGetFrequencyFromIndex(9));
    EXPECT_EQ(88200,device.testGetFrequencyFromIndex(10));
    EXPECT_EQ(96000,device.testGetFrequencyFromIndex(11));
    EXPECT_EQ(176400,device.testGetFrequencyFromIndex(12));
    EXPECT_EQ(192000,device.testGetFrequencyFromIndex(13));
    EXPECT_EQ(352800,device.testGetFrequencyFromIndex(14));
    EXPECT_EQ(384000,device.testGetFrequencyFromIndex(15));
    EXPECT_EQ(705600,device.testGetFrequencyFromIndex(16));
    EXPECT_EQ(768000,device.testGetFrequencyFromIndex(17));    
}

//-------------------------------------------------------------------------------------------

class WasAPIDeviceLayerInitTest : public WasAPIDeviceLayerTest
{
    public:
        MOCK_METHOD0(deviceEnumerator,IMMDeviceEnumeratorIFSPtr());
        MOCK_METHOD1(createDeviceIF,IMMDeviceIFSPtr(IMMDevice *pDevice));
        MOCK_METHOD1(createAudioClientIF,IAudioClientIFSPtr(IAudioClient *pAudioClient));
        MOCK_METHOD0(loadFormats,bool());
        MOCK_METHOD0(blank,void());
        MOCK_METHOD0(queryDeviceFormatCapabilities,void());
        
        IMMDeviceIFSPtr getDeviceInterface();
        IAudioClientIFSPtr getAudioClientInterface();

        bool testInit(const QString& devID);
};

//-------------------------------------------------------------------------------------------

IMMDeviceIFSPtr WasAPIDeviceLayerInitTest::getDeviceInterface()
{
    return m_pDevice;
}

//-------------------------------------------------------------------------------------------

IAudioClientIFSPtr WasAPIDeviceLayerInitTest::getAudioClientInterface()
{
    return m_pAudioClient;
}

//-------------------------------------------------------------------------------------------

bool WasAPIDeviceLayerInitTest::testInit(const QString& devID)
{
    return init(devID);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,initGivenNoDevice)
{
    QString name = "ASUS Xonar DS2";
    
    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    EXPECT_CALL(mockEnumerator,GetDevice(A<LPCWSTR>(),A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(reinterpret_cast<IMMDevice *>(0)),Return(S_OK)));
    
    WasAPIDeviceLayerInitTest deviceLayer;
    
    EXPECT_CALL(deviceLayer,deviceEnumerator()).Times(1).WillOnce(Return(pEnumerator));
    
    ASSERT_FALSE(deviceLayer.testInit(name));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,initGivenErrorGettingDevice)
{
    QString name = "ASUS Xonar DS2";
    
    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDevice *iDevice = reinterpret_cast<IMMDevice *>(5);
    
    EXPECT_CALL(mockEnumerator,GetDevice(A<LPCWSTR>(),A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDevice),Return(S_FALSE)));
    
    WasAPIDeviceLayerInitTest deviceLayer;
    
    EXPECT_CALL(deviceLayer,deviceEnumerator()).Times(1).WillOnce(Return(pEnumerator));
    
    ASSERT_FALSE(deviceLayer.testInit(name));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,initGivenErrorActivatingDevice)
{
    const IID IDOF_IAudioClient = __uuidof(IAudioClient);

    QString name = "ASUS Xonar DS2";
    
    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceIFSPtr pDevice(new IMMDeviceIFMock());
    IMMDeviceIFMock& mockDevice = dynamic_cast<IMMDeviceIFMock&>(*(pDevice.data()));
    IMMDevice *iDevice = reinterpret_cast<IMMDevice *>(5);
    
    IAudioClient *iAudio = reinterpret_cast<IAudioClient *>(6);
    
    EXPECT_CALL(mockEnumerator,GetDevice(A<LPCWSTR>(),A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDevice),Return(S_OK)));
    
    EXPECT_CALL(mockDevice,Activate(IDOF_IAudioClient,CLSCTX_ALL,0,A<void **>())).Times(1).WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<void *>(iAudio)),Return(S_FALSE)));
    
    WasAPIDeviceLayerInitTest deviceLayer;
    
    EXPECT_CALL(deviceLayer,deviceEnumerator()).Times(1).WillOnce(Return(pEnumerator));
    EXPECT_CALL(deviceLayer,createDeviceIF(iDevice)).Times(1).WillOnce(Return(pDevice));
    
    ASSERT_FALSE(deviceLayer.testInit(name));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,initGivenNoAudioClientInterface)
{
    const IID IDOF_IAudioClient = __uuidof(IAudioClient);

    QString name = "ASUS Xonar DS2";
    
    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceIFSPtr pDevice(new IMMDeviceIFMock());
    IMMDeviceIFMock& mockDevice = dynamic_cast<IMMDeviceIFMock&>(*(pDevice.data()));
    IMMDevice *iDevice = reinterpret_cast<IMMDevice *>(5);
    
    EXPECT_CALL(mockEnumerator,GetDevice(A<LPCWSTR>(),A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDevice),Return(S_OK)));
    
    EXPECT_CALL(mockDevice,Activate(IDOF_IAudioClient,CLSCTX_ALL,0,A<void **>())).Times(1).WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<void *>(0)),Return(S_OK)));
    
    WasAPIDeviceLayerInitTest deviceLayer;
    
    EXPECT_CALL(deviceLayer,deviceEnumerator()).Times(1).WillOnce(Return(pEnumerator));
    EXPECT_CALL(deviceLayer,createDeviceIF(iDevice)).Times(1).WillOnce(Return(pDevice));
    
    ASSERT_FALSE(deviceLayer.testInit(name));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,initGivenSuccessWithUnsuccessfulLoadFormats)
{
    const IID IDOF_IAudioClient = __uuidof(IAudioClient);

    QString name = "ASUS Xonar DS2";
    
    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));
    
    IMMDeviceIFSPtr pDevice(new IMMDeviceIFMock());
    IMMDeviceIFMock& mockDevice = dynamic_cast<IMMDeviceIFMock&>(*(pDevice.data()));
    IMMDevice *iDevice = reinterpret_cast<IMMDevice *>(5);
    
    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());
    IAudioClient *iAudio = reinterpret_cast<IAudioClient *>(6);
    
    EXPECT_CALL(mockEnumerator,GetDevice(A<LPCWSTR>(),A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDevice),Return(S_OK)));
    
    EXPECT_CALL(mockDevice, Activate(IDOF_IAudioClient, CLSCTX_ALL, 0, A<void **>())).Times(1).WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<void *>(iAudio)), Return(S_OK)));
    
    WasAPIDeviceLayerInitTest deviceLayer;
    
    EXPECT_CALL(deviceLayer,deviceEnumerator()).Times(1).WillOnce(Return(pEnumerator));
    EXPECT_CALL(deviceLayer,createDeviceIF(iDevice)).Times(1).WillOnce(Return(pDevice));
    EXPECT_CALL(deviceLayer,createAudioClientIF(iAudio)).Times(1).WillOnce(Return(pAudioClient));
    EXPECT_CALL(deviceLayer,loadFormats()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(deviceLayer,blank()).Times(1);
    EXPECT_CALL(deviceLayer,queryDeviceFormatCapabilities()).Times(1);

    ASSERT_TRUE(deviceLayer.testInit(name));
    
    EXPECT_EQ(pDevice.data(),deviceLayer.getDeviceInterface().data());
    EXPECT_TRUE(deviceLayer.getAudioClientInterface().isNull());
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,initGivenSuccessWithSuccessfulLoadFormats)
{
    const IID IDOF_IAudioClient = __uuidof(IAudioClient);

    QString name = "ASUS Xonar DS2";

    IMMDeviceEnumeratorIFSPtr pEnumerator(new IMMDeviceEnumeratorIFMock());
    IMMDeviceEnumeratorIFMock& mockEnumerator = dynamic_cast<IMMDeviceEnumeratorIFMock&>(*(pEnumerator.data()));

    IMMDeviceIFSPtr pDevice(new IMMDeviceIFMock());
    IMMDeviceIFMock& mockDevice = dynamic_cast<IMMDeviceIFMock&>(*(pDevice.data()));
    IMMDevice *iDevice = reinterpret_cast<IMMDevice *>(5);

    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());
    IAudioClient *iAudio = reinterpret_cast<IAudioClient *>(6);

    EXPECT_CALL(mockEnumerator,GetDevice(A<LPCWSTR>(),A<IMMDevice **>())).Times(1).WillOnce(DoAll(SetArgPointee<1>(iDevice),Return(S_OK)));

    EXPECT_CALL(mockDevice,Activate(IDOF_IAudioClient,CLSCTX_ALL,0,A<void **>())).Times(1).WillOnce(DoAll(SetArgPointee<3>(reinterpret_cast<void *>(iAudio)),Return(S_OK)));

    WasAPIDeviceLayerInitTest deviceLayer;

    EXPECT_CALL(deviceLayer,deviceEnumerator()).Times(1).WillOnce(Return(pEnumerator));
    EXPECT_CALL(deviceLayer,createDeviceIF(iDevice)).Times(1).WillOnce(Return(pDevice));
    EXPECT_CALL(deviceLayer,createAudioClientIF(iAudio)).Times(1).WillOnce(Return(pAudioClient));
    EXPECT_CALL(deviceLayer,loadFormats()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(deviceLayer,queryDeviceFormatCapabilities()).Times(1);

    ASSERT_TRUE(deviceLayer.testInit(name));

    EXPECT_EQ(pDevice.data(),deviceLayer.getDeviceInterface().data());
    EXPECT_TRUE(deviceLayer.getAudioClientInterface().isNull());
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfBitsFromWaveFormatFor8Bits)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,8000,format);
    EXPECT_EQ(8,device.testGetNoOfBitsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,8,41000,format);
    EXPECT_EQ(8,device.testGetNoOfBitsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,8,192000,format);
    EXPECT_EQ(8,device.testGetNoOfBitsFromWaveFormat(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfBitsFromWaveFormatFor16Bits)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,16,8000,format);
    EXPECT_EQ(16,device.testGetNoOfBitsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,41000,format);
    EXPECT_EQ(16,device.testGetNoOfBitsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,16,192000,format);
    EXPECT_EQ(16,device.testGetNoOfBitsFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfBitsFromWaveFormatFor24Bits)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,24,8000,format);
    EXPECT_EQ(24,device.testGetNoOfBitsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,24,41000,format);
    EXPECT_EQ(24,device.testGetNoOfBitsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,192000,format);
    EXPECT_EQ(24,device.testGetNoOfBitsFromWaveFormat(&format));        
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfBitsFromWaveFormatFor32Bits)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,32,8000,format);
    EXPECT_EQ(32,device.testGetNoOfBitsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,32,41000,format);
    EXPECT_EQ(32,device.testGetNoOfBitsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,32,192000,format);
    EXPECT_EQ(32,device.testGetNoOfBitsFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfChannelsFromWaveFormatFor1Channel)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,8000,format);
    EXPECT_EQ(1,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,16,41000,format);
    EXPECT_EQ(1,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,24,192000,format);
    EXPECT_EQ(1,device.testGetNoOfChannelsFromWaveFormat(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfChannelsFromWaveFormatFor2Channels)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,8,8000,format);
    EXPECT_EQ(2,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,41000,format);
    EXPECT_EQ(2,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,24,192000,format);
    EXPECT_EQ(2,device.testGetNoOfChannelsFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfChannelsFromWaveFormatFor3Channels)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(3,8,8000,format);
    EXPECT_EQ(3,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(3,16,41000,format);
    EXPECT_EQ(3,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(3,24,192000,format);
    EXPECT_EQ(3,device.testGetNoOfChannelsFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfChannelsFromWaveFormatFor4Channels)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(4,8,8000,format);
    EXPECT_EQ(4,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(4,16,41000,format);
    EXPECT_EQ(4,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(4,24,192000,format);
    EXPECT_EQ(4,device.testGetNoOfChannelsFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfChannelsFromWaveFormatFor5Channels)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(5,8,8000,format);
    EXPECT_EQ(5,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(5,16,41000,format);
    EXPECT_EQ(5,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(5,24,192000,format);
    EXPECT_EQ(5,device.testGetNoOfChannelsFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfChannelsFromWaveFormatFor6Channels)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(6,8,8000,format);
    EXPECT_EQ(6,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(6,16,41000,format);
    EXPECT_EQ(6,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(6,24,192000,format);
    EXPECT_EQ(6,device.testGetNoOfChannelsFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfChannelsFromWaveFormatFor7Channels)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(7,8,8000,format);
    EXPECT_EQ(7,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(7,16,41000,format);
    EXPECT_EQ(7,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(7,24,192000,format);
    EXPECT_EQ(7,device.testGetNoOfChannelsFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getNoOfChannelsFromWaveFormatFor8Channels)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,8,8000,format);
    EXPECT_EQ(8,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,16,41000,format);
    EXPECT_EQ(8,device.testGetNoOfChannelsFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,192000,format);
    EXPECT_EQ(8,device.testGetNoOfChannelsFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor8000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,8000,format);
    EXPECT_EQ(8000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,8000,format);
    EXPECT_EQ(8000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,8000,format);
    EXPECT_EQ(8000,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor11025Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,11025,format);
    EXPECT_EQ(11025,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,11025,format);
    EXPECT_EQ(11025,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,11025,format);
    EXPECT_EQ(11025,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor12000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,12000,format);
    EXPECT_EQ(12000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,12000,format);
    EXPECT_EQ(12000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,12000,format);
    EXPECT_EQ(12000,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor16000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,16000,format);
    EXPECT_EQ(16000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,16000,format);
    EXPECT_EQ(16000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,16000,format);
    EXPECT_EQ(16000,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor22050Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,22050,format);
    EXPECT_EQ(22050,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,22050,format);
    EXPECT_EQ(22050,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,22050,format);
    EXPECT_EQ(22050,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor24000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,24000,format);
    EXPECT_EQ(24000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,24000,format);
    EXPECT_EQ(24000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,24000,format);
    EXPECT_EQ(24000,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor32000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,32000,format);
    EXPECT_EQ(32000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,32000,format);
    EXPECT_EQ(32000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,32000,format);
    EXPECT_EQ(32000,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor44100Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,44100,format);
    EXPECT_EQ(44100,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,44100,format);
    EXPECT_EQ(44100,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,44100,format);
    EXPECT_EQ(44100,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor48000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,48000,format);
    EXPECT_EQ(48000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,48000,format);
    EXPECT_EQ(48000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,48000,format);
    EXPECT_EQ(48000,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor64000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,64000,format);
    EXPECT_EQ(64000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,64000,format);
    EXPECT_EQ(64000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,64000,format);
    EXPECT_EQ(64000,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor88200Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,88200,format);
    EXPECT_EQ(88200,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,88200,format);
    EXPECT_EQ(88200,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,88200,format);
    EXPECT_EQ(88200,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor96000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,96000,format);
    EXPECT_EQ(96000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,96000,format);
    EXPECT_EQ(96000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,96000,format);
    EXPECT_EQ(96000,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getFrequencyFromWaveFormatFor192000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,8,192000,format);
    EXPECT_EQ(192000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,192000,format);
    EXPECT_EQ(192000,device.testGetFrequencyFromWaveFormat(&format));

    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,24,192000,format);
    EXPECT_EQ(192000,device.testGetFrequencyFromWaveFormat(&format));    
}

//-------------------------------------------------------------------------------------------

class WasAPIDeviceLayerFindDefaultFormatTypeInvoker
{
    public:
        WasAPIDeviceLayerFindDefaultFormatTypeInvoker();
        virtual ~WasAPIDeviceLayerFindDefaultFormatTypeInvoker();
        virtual void append(WAVEFORMATEX *format);
        virtual HRESULT IsFormatSupported(AUDCLNT_SHAREMODE shareMode,const WAVEFORMATEX *pFormat,WAVEFORMATEX **ppClosestMatch);
        
    protected:
        
        QList<WAVEFORMATEX *> m_formats;
};

//-------------------------------------------------------------------------------------------

WasAPIDeviceLayerFindDefaultFormatTypeInvoker::WasAPIDeviceLayerFindDefaultFormatTypeInvoker() : m_formats()
{}

//-------------------------------------------------------------------------------------------

WasAPIDeviceLayerFindDefaultFormatTypeInvoker::~WasAPIDeviceLayerFindDefaultFormatTypeInvoker()
{
    for(QList<WAVEFORMATEX *>::iterator ppI=m_formats.begin();ppI!=m_formats.end();++ppI)
    {
        delete *ppI;
    }
    m_formats.clear();
}

//-------------------------------------------------------------------------------------------

void WasAPIDeviceLayerFindDefaultFormatTypeInvoker::append(WAVEFORMATEX *format)
{
    WAVEFORMATEX *pNFormat = new WAVEFORMATEX;
    memcpy(pNFormat,format,sizeof(WAVEFORMATEX));
    m_formats.append(pNFormat);
}

//-------------------------------------------------------------------------------------------

HRESULT WasAPIDeviceLayerFindDefaultFormatTypeInvoker::IsFormatSupported(AUDCLNT_SHAREMODE ,const WAVEFORMATEX *pFormat,WAVEFORMATEX **)
{
    QList<WAVEFORMATEX *>::const_iterator ppI;
    HRESULT res = S_FALSE;
    
    for(ppI=m_formats.begin();ppI!=m_formats.end() && res==S_FALSE;++ppI)
    {
        const WAVEFORMATEX *f = *ppI;
        
        if(pFormat->wFormatTag==f->wFormatTag && pFormat->nChannels==f->nChannels && pFormat->nSamplesPerSec==f->nSamplesPerSec &&
           pFormat->nAvgBytesPerSec==f->nAvgBytesPerSec && pFormat->nBlockAlign==f->nBlockAlign && pFormat->wBitsPerSample==f->wBitsPerSample)
        {
            res = S_OK;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

class WasAPILayerIFGetDeviceTest : public WasAPILayerIFTest
{
    public:
        MOCK_METHOD0(createDeviceInstance, WasAPIDeviceLayer *());
};

//-------------------------------------------------------------------------------------------

class WasAPIDeviceLayerInitMock : public WasAPIDeviceLayer
{
    public:
        MOCK_METHOD1(init,bool(const QString& devID));
};

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,getDeviceNoneGivenForEmptyDeviceID)
{
    QString devId = "";
    WasAPILayerIFGetDeviceTest layer;
    WasAPIDeviceSPtr pDevice = layer.getDevice(devId);
    ASSERT_TRUE(pDevice.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,getDeviceNoDeviceGivenWhenInitFails)
{
    QString devId = "Device ID";

    WasAPIDeviceLayerInitMock *pExpectDevice = new WasAPIDeviceLayerInitMock();
    EXPECT_CALL(*pExpectDevice,init(devId)).Times(1).WillOnce(Return(false));
    
    WasAPILayerIFGetDeviceTest layer;
    EXPECT_CALL(layer,createDeviceInstance()).Times(1).WillOnce(Return(pExpectDevice));
    
    WasAPIDeviceSPtr pDevice = layer.getDevice(devId);
    
    ASSERT_TRUE(pDevice.isNull());
}

//-------------------------------------------------------------------------------------------

TEST(WasAPILayerIF,getDeviceNoDeviceGivenWhenInitSucceeds)
{
    QString devId = "Device ID";

    WasAPIDeviceLayerInitMock *pExpectDevice = new WasAPIDeviceLayerInitMock();
    EXPECT_CALL(*pExpectDevice,init(devId)).Times(1).WillOnce(Return(true));
    
    WasAPILayerIFGetDeviceTest layer;
    EXPECT_CALL(layer,createDeviceInstance()).Times(1).WillOnce(Return(pExpectDevice));
    
    WasAPIDeviceSPtr pDevice = layer.getDevice(devId);
    
    ASSERT_TRUE(pDevice.data()==pExpectDevice);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,idWhenDeviceFailsWithNoName)
{
    LPWSTR pEName = 0;

    IMMDeviceIFSPtr pDevice(new IMMDeviceIFMock());
    IMMDeviceIFMock& mockDevice = dynamic_cast<IMMDeviceIFMock&>(*(pDevice.data()));
    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());

    EXPECT_CALL(mockDevice,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(pEName),Return(S_FALSE)));

    WasAPIDeviceLayerTest device(pDevice,pAudioClient);
    
    ASSERT_TRUE(device.id().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,idWhenDeviceSucceedsWithNoName)
{
    LPWSTR pEName = 0;

    IMMDeviceIFSPtr pDevice(new IMMDeviceIFMock());
    IMMDeviceIFMock& mockDevice = dynamic_cast<IMMDeviceIFMock&>(*(pDevice.data()));
    IAudioClientIFSPtr pAudioClient(new IAudioClientIFMock());

    EXPECT_CALL(mockDevice,GetId(A<LPWSTR *>())).Times(1).WillOnce(DoAll(SetArgPointee<0>(pEName),Return(S_OK)));

    WasAPIDeviceLayerTest device(pDevice,pAudioClient);
    
    ASSERT_TRUE(device.id().isEmpty());
}

//-------------------------------------------------------------------------------------------

class WasAPIDeviceLayerSaveAndLoadFormatsInvoker
{
    public:
        WasAPIDeviceLayerSaveAndLoadFormatsInvoker(const QString& devID);
        HRESULT GetId(LPWSTR *ppstrId);
    protected:
        QString m_deviceID;
};

//-------------------------------------------------------------------------------------------

WasAPIDeviceLayerSaveAndLoadFormatsInvoker::WasAPIDeviceLayerSaveAndLoadFormatsInvoker(const QString& devID) : m_deviceID(devID)
{}

//-------------------------------------------------------------------------------------------

HRESULT WasAPIDeviceLayerSaveAndLoadFormatsInvoker::GetId(LPWSTR *ppstrId)
{
    LPWSTR pEName = reinterpret_cast<LPWSTR>(CoTaskMemAlloc((m_deviceID.length() + 1) * sizeof(tuint16)));
    memcpy(pEName,m_deviceID.utf16(),m_deviceID.length() * sizeof(tuint16));
    pEName[m_deviceID.length()] = 0;
    *ppstrId = pEName;
    return S_OK;
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfBitsFor8BitsInteger)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,8,44100,format);
    ASSERT_EQ(0,device.testGetIndexOfBits(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfBitsFor16BitsInteger)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,44100,format);
    ASSERT_EQ(1,device.testGetIndexOfBits(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfBitsFor24BitsInteger)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,24,44100,format);
    ASSERT_EQ(2,device.testGetIndexOfBits(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfBitsFor32BitsInteger)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,32,44100,format);
    ASSERT_EQ(3,device.testGetIndexOfBits(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfBitsFor32BitsFloat)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    format.wFormatTag = WasAPIDeviceLayer::e_formatFloat;
    device.testSetWaveFormat(2,32,44100,format);
    ASSERT_EQ(4,device.testGetIndexOfBits(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfChannelsFor1Channel)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(1,16,44100,format);
    ASSERT_EQ(0,device.testGetIndexOfChannels(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfChannelsFor2Channels)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,44100,format);
    ASSERT_EQ(1,device.testGetIndexOfChannels(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfChannelsFor4Channels)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(4,16,44100,format);
    ASSERT_EQ(3,device.testGetIndexOfChannels(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfChannelsFor8Channels)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(8,16,44100,format);
    ASSERT_EQ(7,device.testGetIndexOfChannels(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAtUndefined)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,100,format);
    ASSERT_EQ(-1,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt8000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,8000,format);
    ASSERT_EQ(0,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt11025Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,11025,format);
    ASSERT_EQ(1,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt12000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,12000,format);
    ASSERT_EQ(2,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt16000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,16000,format);
    ASSERT_EQ(3,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt22050Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,22050,format);
    ASSERT_EQ(4,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt24000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,24000,format);
    ASSERT_EQ(5,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt32000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,32000,format);
    ASSERT_EQ(6,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt44100Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,44100,format);
    ASSERT_EQ(7,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt48000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,48000,format);
    ASSERT_EQ(8,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt64000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,64000,format);
    ASSERT_EQ(9,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt88200Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,88200,format);
    ASSERT_EQ(10,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt96000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,96000,format);
    ASSERT_EQ(11,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt176400Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,176400,format);
    ASSERT_EQ(12,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt192000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,192000,format);
    ASSERT_EQ(13,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt352800Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,352800,format);
    ASSERT_EQ(14,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt384000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,384000,format);
    ASSERT_EQ(15,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt705600Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,705600,format);
    ASSERT_EQ(16,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,getIndexOfFrequencyAt768000Hz)
{
    WAVEFORMATEX format;
    WasAPIDeviceLayerTest device;
    device.testDefaultWaveFormat(format);
    device.testSetWaveFormat(2,16,768000,format);
    ASSERT_EQ(17,device.testGetIndexOfFrequency(&format));
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,setFrequencyInWaveFormatEx)
{
    WAVEFORMATEX format;
    
    memset(&format,0,sizeof(WAVEFORMATEX));
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 4;
    format.nSamplesPerSec = 44100;
    format.nAvgBytesPerSec = 44100 * 4 * 2;
    format.nBlockAlign = 4 * 2;
    format.wBitsPerSample = 16;
    format.cbSize = 0;

    WasAPIDeviceLayerTest device;
    device.testSetFrequencyInWaveFormat(192000,&format);
    
    EXPECT_EQ(WAVE_FORMAT_PCM,format.wFormatTag);
    EXPECT_EQ(4,format.nChannels);
    EXPECT_EQ(192000,format.nSamplesPerSec);
    EXPECT_EQ(192000 * 4 * 2,format.nAvgBytesPerSec);
    EXPECT_EQ(4 * 2,format.nBlockAlign);
    EXPECT_EQ(16,format.wBitsPerSample);
    EXPECT_EQ(0,format.cbSize);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,setFrequencyInWaveFormatExtensible)
{
    WAVEFORMATEXTENSIBLE format;
    
    memset(&format,0,sizeof(WAVEFORMATEXTENSIBLE));
    format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    format.Format.nChannels = 4;
    format.Format.nSamplesPerSec = 44100;
    format.Format.nAvgBytesPerSec = 44100 * 4 * 2;
    format.Format.nBlockAlign = 4 * 2;
    format.Format.wBitsPerSample = 16;
    format.Format.cbSize = 22;
    format.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
    format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

    WasAPIDeviceLayerTest device;
    device.testSetFrequencyInWaveFormat(192000,reinterpret_cast<WAVEFORMATEX *>(&format));
    
    EXPECT_EQ(WAVE_FORMAT_EXTENSIBLE,format.Format.wFormatTag);
    EXPECT_EQ(4,format.Format.nChannels);
    EXPECT_EQ(192000,format.Format.nSamplesPerSec);
    EXPECT_EQ(192000 * 4 * 2,format.Format.nAvgBytesPerSec);
    EXPECT_EQ(4 * 2,format.Format.nBlockAlign);
    EXPECT_EQ(16,format.Format.wBitsPerSample);
    EXPECT_EQ(22,format.Format.cbSize);
    EXPECT_EQ(KSAUDIO_SPEAKER_QUAD,format.dwChannelMask);
    EXPECT_EQ(KSDATAFORMAT_SUBTYPE_PCM,format.SubFormat);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,setChannelsInWaveFormatEx)
{
    WAVEFORMATEX format;
    
    memset(&format,0,sizeof(WAVEFORMATEX));
    format.wFormatTag = WAVE_FORMAT_PCM;
    format.nChannels = 2;
    format.nSamplesPerSec = 44100;
    format.nAvgBytesPerSec = 44100 * 2 * 3;
    format.nBlockAlign = 2 * 3;
    format.wBitsPerSample = 24;
    format.cbSize = 0;

    WasAPIDeviceLayerTest device;
    device.testSetChannelsInWaveFormat(8,&format);
    
    EXPECT_EQ(WAVE_FORMAT_PCM,format.wFormatTag);
    EXPECT_EQ(8,format.nChannels);
    EXPECT_EQ(44100,format.nSamplesPerSec);
    EXPECT_EQ(44100 * 8 * 3,format.nAvgBytesPerSec);
    EXPECT_EQ(8 * 3,format.nBlockAlign);
    EXPECT_EQ(24,format.wBitsPerSample);
    EXPECT_EQ(0,format.cbSize);
}

//-------------------------------------------------------------------------------------------

TEST(WasAPIDeviceLayer,setChannelsInWaveFormatExtensible)
{
    WAVEFORMATEXTENSIBLE format;
    
    memset(&format,0,sizeof(WAVEFORMATEXTENSIBLE));
    format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    format.Format.nChannels = 2;
    format.Format.nSamplesPerSec = 44100;
    format.Format.nAvgBytesPerSec = 44100 * 2 * 3;
    format.Format.nBlockAlign = 2 * 3;
    format.Format.wBitsPerSample = 24;
    format.Format.cbSize = 22;
    format.dwChannelMask = KSAUDIO_SPEAKER_QUAD;
    format.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

    WasAPIDeviceLayerTest device;
    device.testSetChannelsInWaveFormat(8,reinterpret_cast<WAVEFORMATEX *>(&format));
    
    EXPECT_EQ(WAVE_FORMAT_EXTENSIBLE,format.Format.wFormatTag);
    EXPECT_EQ(8,format.Format.nChannels);
    EXPECT_EQ(44100,format.Format.nSamplesPerSec);
    EXPECT_EQ(44100 * 8 * 3,format.Format.nAvgBytesPerSec);
    EXPECT_EQ(8 * 3,format.Format.nBlockAlign);
    EXPECT_EQ(24,format.Format.wBitsPerSample);
    EXPECT_EQ(22,format.Format.cbSize);
    EXPECT_EQ(KSAUDIO_SPEAKER_QUAD,format.dwChannelMask);
    EXPECT_EQ(KSDATAFORMAT_SUBTYPE_PCM,format.SubFormat);
}

//-------------------------------------------------------------------------------------------
