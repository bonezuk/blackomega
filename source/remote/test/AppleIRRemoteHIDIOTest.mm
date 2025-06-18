//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/AppleIRRemoteHIDIO.h"
#include "remote/test/HIDDeviceIFMock.h"
#include "remote/test/RemoteIFMock.h"

using namespace omega::remote;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AppleIRRemoteHIDIOTest : public AppleIRRemoteHIDIO
{
    public:
        AppleIRRemoteHIDIOTest();
        virtual ~AppleIRRemoteHIDIOTest();
        
        MOCK_CONST_METHOD0(getExclusive,bool());
        MOCK_CONST_METHOD0(getHIDManager,IOHIDManagerRef());
        MOCK_CONST_METHOD0(getHIDDevice,IOHIDDeviceRef());
        MOCK_CONST_METHOD0(getHIDQueue,IOHIDQueueRef());
        MOCK_CONST_METHOD0(getKeyParser,const KeyStateParser&());
        MOCK_CONST_METHOD0(getButtonStatesMap,const QMap<int,Button>&());
        MOCK_CONST_METHOD0(getRemoteIF,RemoteIF*());
        
        MOCK_METHOD1(setExclusive,void(bool exclusive));
        MOCK_METHOD1(setHIDManager,void(IOHIDManagerRef manager));
        MOCK_METHOD1(setHIDDevice,void(IOHIDDeviceRef device));
        MOCK_METHOD1(setHIDQueue,void(IOHIDQueueRef));

        bool testOpenManagerForDeviceClass(const char *className);
        void testCloseManager();
        int testOpenDevice(IOHIDDeviceRef deviceRef);
        void testCloseDevice();
        bool testOpenQueue();
        void testCloseQueue();
        void testQueueValueCallbackImpl(IOReturn inResult,void *inSender);
        void testProcessQueue(const QVector<int>& cookieList,const QVector<int>& indexList);
                
        const QMap<int,Button>& getStateMap() const;
        const KeyStateParser& getParser() const;
};

//-------------------------------------------------------------------------------------------

AppleIRRemoteHIDIOTest::AppleIRRemoteHIDIOTest() : AppleIRRemoteHIDIO()
{}

//-------------------------------------------------------------------------------------------

AppleIRRemoteHIDIOTest::~AppleIRRemoteHIDIOTest()
{}

//-------------------------------------------------------------------------------------------

bool AppleIRRemoteHIDIOTest::testOpenManagerForDeviceClass(const char *className)
{
    return openManagerForDeviceClass(className);
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteHIDIOTest::testCloseManager()
{
    closeManager();
}

//-------------------------------------------------------------------------------------------

int AppleIRRemoteHIDIOTest::testOpenDevice(IOHIDDeviceRef deviceRef)
{
    return openDevice(deviceRef);
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteHIDIOTest::testCloseDevice()
{
    closeDevice();
}

//-------------------------------------------------------------------------------------------

bool AppleIRRemoteHIDIOTest::testOpenQueue()
{
    return openQueue();
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteHIDIOTest::testCloseQueue()
{
    closeQueue();
}

//-------------------------------------------------------------------------------------------

const QMap<int,AppleIRRemoteHIDIOTest::Button>& AppleIRRemoteHIDIOTest::getStateMap() const
{
    return m_states;
}

//-------------------------------------------------------------------------------------------

const KeyStateParser& AppleIRRemoteHIDIOTest::getParser() const
{
    return m_parser;
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteHIDIOTest::testQueueValueCallbackImpl(IOReturn inResult,void *inSender)
{
    queueValueCallbackImpl(inResult,inSender);
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteHIDIOTest::testProcessQueue(const QVector<int>& cookieList,const QVector<int>& indexList)
{
    processQueue(cookieList,indexList);
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openManagerForDeviceClassFailsToCreateManager)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
        
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(pAPI,IOHIDManagerCreate(kCFAllocatorDefault,kIOHIDOptionsTypeNone)).Times(1).WillOnce(Return((IOHIDManagerRef)0));
    
    EXPECT_FALSE(remote.testOpenManagerForDeviceClass("MockAppleIRRemote"));
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openManagerForDeviceClassFailsToFindMatchingDictionary)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockManager = 1;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(pAPI,IOHIDManagerCreate(kCFAllocatorDefault,kIOHIDOptionsTypeNone)).Times(1)
        .WillOnce(Return((IOHIDManagerRef)&mockManager));
    EXPECT_CALL(pAPI,IOServiceMatching("MockAppleIRRemote")).Times(1).WillOnce(Return((CFMutableDictionaryRef)0));
    EXPECT_CALL(pAPI,CFRelease((IOHIDManagerRef)&mockManager)).Times(1);
    
    EXPECT_FALSE(remote.testOpenManagerForDeviceClass("MockAppleIRRemote"));
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openManagerForDeviceClassFailsToOpenManager)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockManager = 1;
    int mockDictionary = 2;
    int mockRunLoop = 3;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(pAPI,IOHIDManagerCreate(kCFAllocatorDefault,kIOHIDOptionsTypeNone)).Times(1)
        .WillOnce(Return((IOHIDManagerRef)&mockManager));
    EXPECT_CALL(pAPI,IOServiceMatching("MockAppleIRRemote")).Times(1).WillOnce(Return((CFMutableDictionaryRef)&mockDictionary));
    EXPECT_CALL(pAPI,IOHIDManagerSetDeviceMatching((IOHIDManagerRef)&mockManager,(CFDictionaryRef)&mockDictionary)).Times(1);
    EXPECT_CALL(pAPI,IOHIDManagerRegisterDeviceMatchingCallback((IOHIDManagerRef)&mockManager, AppleIRRemoteHIDIO::registerDevice,(void *)&remote)).Times(1);
    EXPECT_CALL(pAPI,IOHIDManagerRegisterDeviceRemovalCallback((IOHIDManagerRef)&mockManager, AppleIRRemoteHIDIO::unregisterDevice,(void *)&remote)).Times(1);
    EXPECT_CALL(pAPI,CFRunLoopGetCurrent()).Times(2).WillRepeatedly(Return((CFRunLoopRef)&mockRunLoop));
    EXPECT_CALL(pAPI,IOHIDManagerScheduleWithRunLoop((IOHIDManagerRef)&mockManager,(CFRunLoopRef)&mockRunLoop,kCFRunLoopDefaultMode)).Times(1);
    EXPECT_CALL(pAPI,IOHIDManagerOpen((IOHIDManagerRef)&mockManager,kIOHIDOptionsTypeNone)).Times(1)
        .WillOnce(Return(kIOReturnError));
    EXPECT_CALL(pAPI,IOHIDManagerUnscheduleFromRunLoop((IOHIDManagerRef)&mockManager,(CFRunLoopRef)&mockRunLoop,kCFRunLoopDefaultMode)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((CFDictionaryRef)&mockDictionary)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDManagerRef)&mockManager)).Times(1);
    
    EXPECT_FALSE(remote.testOpenManagerForDeviceClass("MockAppleIRRemote"));
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openManagerForDeviceClassSuccess)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockManager = 1;
    int mockDictionary = 2;
    int mockRunLoop = 3;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(pAPI,IOHIDManagerCreate(kCFAllocatorDefault,kIOHIDOptionsTypeNone)).Times(1)
        .WillOnce(Return((IOHIDManagerRef)&mockManager));
    EXPECT_CALL(pAPI,IOServiceMatching("MockAppleIRRemote")).Times(1).WillOnce(Return((CFMutableDictionaryRef)&mockDictionary));
    EXPECT_CALL(pAPI,IOHIDManagerSetDeviceMatching((IOHIDManagerRef)&mockManager,(CFDictionaryRef)&mockDictionary)).Times(1);
    EXPECT_CALL(pAPI,IOHIDManagerRegisterDeviceMatchingCallback((IOHIDManagerRef)&mockManager, AppleIRRemoteHIDIO::registerDevice,(void *)&remote)).Times(1);
    EXPECT_CALL(pAPI,IOHIDManagerRegisterDeviceRemovalCallback((IOHIDManagerRef)&mockManager, AppleIRRemoteHIDIO::unregisterDevice,(void *)&remote)).Times(1);
    EXPECT_CALL(pAPI,CFRunLoopGetCurrent()).Times(1).WillOnce(Return((CFRunLoopRef)&mockRunLoop));
    EXPECT_CALL(pAPI,IOHIDManagerScheduleWithRunLoop((IOHIDManagerRef)&mockManager,(CFRunLoopRef)&mockRunLoop,kCFRunLoopDefaultMode)).Times(1);
    EXPECT_CALL(pAPI,IOHIDManagerOpen((IOHIDManagerRef)&mockManager,kIOHIDOptionsTypeNone)).Times(1)
        .WillOnce(Return(kIOReturnSuccess));
    EXPECT_CALL(pAPI,CFRetain((IOHIDManagerRef)&mockManager)).Times(1)
        .WillOnce(Return((IOHIDManagerRef)&mockManager));
    EXPECT_CALL(remote,setHIDManager((IOHIDManagerRef)&mockManager)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((CFDictionaryRef)&mockDictionary)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDManagerRef)&mockManager)).Times(1);
    
    EXPECT_TRUE(remote.testOpenManagerForDeviceClass("MockAppleIRRemote"));
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,closeManagerNoManager)
{
    AppleIRRemoteHIDIOTest remote;
    EXPECT_CALL(remote,getHIDManager()).Times(1).WillOnce(Return((IOHIDManagerRef)0));
    remote.testCloseManager();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,closeManagerWithManager)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockManager = 1;
    int mockRunLoop = 3;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getHIDManager()).Times(4).WillRepeatedly(Return((IOHIDManagerRef)&mockManager));
    EXPECT_CALL(pAPI,IOHIDManagerClose((IOHIDManagerRef)&mockManager,kIOHIDOptionsTypeNone)).Times(1);
    EXPECT_CALL(pAPI,CFRunLoopGetCurrent()).Times(1).WillOnce(Return((CFRunLoopRef)&mockRunLoop));
    EXPECT_CALL(pAPI,IOHIDManagerUnscheduleFromRunLoop((IOHIDManagerRef)&mockManager,(CFRunLoopRef)&mockRunLoop,kCFRunLoopDefaultMode)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDManagerRef)&mockManager)).Times(1);
    EXPECT_CALL(remote,setHIDManager(0)).Times(1);
    
    remote.testCloseManager();
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openDeviceFailure)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockDeviceRef = 1;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getExclusive()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,IOHIDDeviceOpen((IOHIDDeviceRef)&mockDeviceRef,kIOHIDOptionsTypeSeizeDevice)).Times(1).WillOnce(Return(kIOReturnIOError));

    EXPECT_TRUE(remote.testOpenDevice((IOHIDDeviceRef)&mockDeviceRef)<0);

    HIDDeviceIF::release();        
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openDeviceExclusiveLockedByOtherProcess)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockDeviceRef = 1;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getExclusive()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,IOHIDDeviceOpen((IOHIDDeviceRef)&mockDeviceRef,kIOHIDOptionsTypeSeizeDevice)).Times(1).WillOnce(Return(kIOReturnExclusiveAccess));

    EXPECT_TRUE(remote.testOpenDevice((IOHIDDeviceRef)&mockDeviceRef)==0);

    HIDDeviceIF::release();    
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openDeviceExclusiveSuccess)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockDeviceRef = 1;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getExclusive()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,IOHIDDeviceOpen((IOHIDDeviceRef)&mockDeviceRef,kIOHIDOptionsTypeSeizeDevice)).Times(1).WillOnce(Return(kIOReturnSuccess));
    EXPECT_CALL(remote,setHIDDevice((IOHIDDeviceRef)&mockDeviceRef)).Times(1);

    EXPECT_TRUE(remote.testOpenDevice((IOHIDDeviceRef)&mockDeviceRef)>0);

    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openDeviceSharedSuccess)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockDeviceRef = 1;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getExclusive()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,IOHIDDeviceOpen((IOHIDDeviceRef)&mockDeviceRef,kIOHIDOptionsTypeNone)).Times(1).WillOnce(Return(kIOReturnSuccess));
    EXPECT_CALL(remote,setHIDDevice((IOHIDDeviceRef)&mockDeviceRef)).Times(1);

    EXPECT_TRUE(remote.testOpenDevice((IOHIDDeviceRef)&mockDeviceRef)==1);

    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,closeDeviceNoDeviceOpen)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    AppleIRRemoteHIDIOTest remote;
    EXPECT_CALL(remote,getHIDDevice()).Times(1).WillOnce(Return((IOHIDDeviceRef)0));
    remote.testCloseDevice();
    
    HIDDeviceIF::release();        
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,closeDeviceGivenOpenDevice)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
        
    int mockDevice = 1;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getHIDDevice()).Times(2).WillRepeatedly(Return((IOHIDDeviceRef)&mockDevice));
    EXPECT_CALL(pAPI,IOHIDDeviceClose((IOHIDDeviceRef)&mockDevice,kIOHIDOptionsTypeNone)).Times(1);
    EXPECT_CALL(remote,setHIDDevice(0)).Times(1);
    
    remote.testCloseDevice();
    
    HIDDeviceIF::release();    
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openQueueFailToCreateQueue)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockDevice = 1;

    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getHIDDevice()).Times(1).WillOnce(Return((IOHIDDeviceRef)&mockDevice));
    EXPECT_CALL(pAPI,IOHIDQueueCreate(kCFAllocatorDefault,(IOHIDDeviceRef)&mockDevice,12,kIOHIDOptionsTypeNone)).Times(1).WillOnce(Return((IOHIDQueueRef)0));
    
    EXPECT_FALSE(remote.testOpenQueue());
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openQueueFailToGetElements)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockQueue = 1;
    int mockDevice = 2;
        
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getHIDDevice()).Times(2).WillRepeatedly(Return((IOHIDDeviceRef)&mockDevice));
    EXPECT_CALL(pAPI,IOHIDQueueCreate(kCFAllocatorDefault,(IOHIDDeviceRef)&mockDevice,12,kIOHIDOptionsTypeNone)).Times(1).WillOnce(Return((IOHIDQueueRef)&mockQueue));
    EXPECT_CALL(pAPI,IOHIDDeviceCopyMatchingElements((IOHIDDeviceRef)&mockDevice,0,kIOHIDOptionsTypeNone)).Times(1).WillOnce(Return((CFArrayRef)0));
    EXPECT_CALL(pAPI,CFRelease((IOHIDQueueRef)&mockQueue)).Times(1);
    
    EXPECT_FALSE(remote.testOpenQueue());
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openQueueSuccessNoElements)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockQueue = 1;
    int mockDevice = 2;
    int mockRunLoop = 3;
    int mockArray = 4;
    int mockElementA = 5;
    int mockElementB = 6;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getHIDDevice()).Times(2).WillRepeatedly(Return((IOHIDDeviceRef)&mockDevice));
    EXPECT_CALL(pAPI,IOHIDQueueCreate(kCFAllocatorDefault,(IOHIDDeviceRef)&mockDevice,12,kIOHIDOptionsTypeNone)).Times(1).WillOnce(Return((IOHIDQueueRef)&mockQueue));
    EXPECT_CALL(pAPI,IOHIDDeviceCopyMatchingElements((IOHIDDeviceRef)&mockDevice,0,kIOHIDOptionsTypeNone)).Times(1).WillOnce(Return((CFArrayRef)&mockArray));
    EXPECT_CALL(pAPI,CFArrayGetCount((CFArrayRef)&mockArray)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(pAPI,CFRelease((CFArrayRef)&mockArray)).Times(1);
    EXPECT_CALL(pAPI,CFRunLoopGetCurrent()).Times(1).WillOnce(Return((CFRunLoopRef)&mockRunLoop));
    EXPECT_CALL(pAPI,IOHIDQueueScheduleWithRunLoop((IOHIDQueueRef)&mockQueue,(CFRunLoopRef)&mockRunLoop,kCFRunLoopDefaultMode)).Times(1);
    EXPECT_CALL(pAPI,IOHIDQueueRegisterValueAvailableCallback((IOHIDQueueRef)&mockQueue,AppleIRRemoteHIDIO::queueValueCallback,(void *)&remote)).Times(1);
    EXPECT_CALL(pAPI,IOHIDQueueStart((IOHIDQueueRef)&mockQueue)).Times(1);
    EXPECT_CALL(pAPI,CFRetain((IOHIDQueueRef)&mockQueue)).Times(1).WillOnce(Return((IOHIDQueueRef)&mockQueue));
    EXPECT_CALL(remote,setHIDQueue((IOHIDQueueRef)&mockQueue)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDQueueRef)&mockQueue)).Times(1);
    
    EXPECT_TRUE(remote.testOpenQueue());
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,openQueueSuccess)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockQueue = 1;
    int mockDevice = 2;
    int mockRunLoop = 3;
    int mockArray = 4;
    int mockElementA = 5;
    int mockElementB = 6;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getHIDDevice()).Times(2).WillRepeatedly(Return((IOHIDDeviceRef)&mockDevice));
    EXPECT_CALL(pAPI,IOHIDQueueCreate(kCFAllocatorDefault,(IOHIDDeviceRef)&mockDevice,12,kIOHIDOptionsTypeNone)).Times(1).WillOnce(Return((IOHIDQueueRef)&mockQueue));
    EXPECT_CALL(pAPI,IOHIDDeviceCopyMatchingElements((IOHIDDeviceRef)&mockDevice,0,kIOHIDOptionsTypeNone)).Times(1).WillOnce(Return((CFArrayRef)&mockArray));
    EXPECT_CALL(pAPI,CFArrayGetCount((CFArrayRef)&mockArray)).Times(1).WillOnce(Return(2));
    EXPECT_CALL(pAPI,CFArrayGetValueAtIndex((CFArrayRef)&mockArray,0)).Times(1).WillOnce(Return((IOHIDElementRef)&mockElementA));
    EXPECT_CALL(pAPI,IOHIDQueueAddElement((IOHIDQueueRef)&mockQueue,(IOHIDElementRef)&mockElementA)).Times(1);
    EXPECT_CALL(pAPI,CFArrayGetValueAtIndex((CFArrayRef)&mockArray,1)).Times(1).WillOnce(Return((IOHIDElementRef)&mockElementB));
    EXPECT_CALL(pAPI,IOHIDQueueAddElement((IOHIDQueueRef)&mockQueue,(IOHIDElementRef)&mockElementB)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((CFArrayRef)&mockArray)).Times(1);
    EXPECT_CALL(pAPI,CFRunLoopGetCurrent()).Times(1).WillOnce(Return((CFRunLoopRef)&mockRunLoop));
    EXPECT_CALL(pAPI,IOHIDQueueScheduleWithRunLoop((IOHIDQueueRef)&mockQueue,(CFRunLoopRef)&mockRunLoop,kCFRunLoopDefaultMode)).Times(1);
    EXPECT_CALL(pAPI,IOHIDQueueRegisterValueAvailableCallback((IOHIDQueueRef)&mockQueue,AppleIRRemoteHIDIO::queueValueCallback,(void *)&remote)).Times(1);
    EXPECT_CALL(pAPI,IOHIDQueueStart((IOHIDQueueRef)&mockQueue)).Times(1);
    EXPECT_CALL(pAPI,CFRetain((IOHIDQueueRef)&mockQueue)).Times(1).WillOnce(Return((IOHIDQueueRef)&mockQueue));
    EXPECT_CALL(remote,setHIDQueue((IOHIDQueueRef)&mockQueue)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDQueueRef)&mockQueue)).Times(1);
    
    EXPECT_TRUE(remote.testOpenQueue());
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,closeQueueWhenNoQueue)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    AppleIRRemoteHIDIOTest remote;    
    EXPECT_CALL(remote,getHIDQueue()).Times(1).WillOnce(Return((IOHIDQueueRef)0));
    
    remote.testCloseQueue();
    
    HIDDeviceIF::release();        
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,closeQueueWhenQueueExists)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockQueue = 4;
    int mockRunLoop = 3;
    
    AppleIRRemoteHIDIOTest remote;
    
    EXPECT_CALL(remote,getHIDQueue()).Times(4).WillRepeatedly(Return((IOHIDQueueRef)&mockQueue));
    EXPECT_CALL(pAPI,IOHIDQueueStop((IOHIDQueueRef)&mockQueue)).Times(1);
    EXPECT_CALL(pAPI,CFRunLoopGetCurrent()).Times(1).WillOnce(Return((CFRunLoopRef)&mockRunLoop));
    EXPECT_CALL(pAPI,IOHIDQueueUnscheduleFromRunLoop((IOHIDQueueRef)&mockQueue,(CFRunLoopRef)&mockRunLoop,kCFRunLoopDefaultMode)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDQueueRef)&mockQueue)).Times(1);
    EXPECT_CALL(remote,setHIDQueue(0)).Times(1);
    
    remote.testCloseQueue();
    
    HIDDeviceIF::release();    
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,defineIRParser)
{
    const int c_remoteButtonCentre[]       = { 33, 21, 20,  3,  2, 33, 21, 20,  3, 2 }; // 10
    const int c_remoteButtonCentreHold[]   = { 33, 21, 20, 11,  2, 33, 21, 20, 11, 2 }; // 10
    const int c_remoteButtonPlus[]         = { 33, 31, 30, 21, 20,  2 }; // 6
    const int c_remoteButtonMinus[]        = { 33, 32, 30, 21, 20,  2 }; // 6
    const int c_remoteButtonLeftTouch[]    = { 33, 25, 21, 20,  2, 33, 25, 21, 20, 2 }; // 10
    const int c_remoteButtonLeft[]         = { 33, 21, 20, 13, 12,  2 }; // 6
    const int c_remoteButtonRightTouch[]   = { 33, 24, 21, 20,  2, 33, 24, 21, 20, 2 }; // 10
    const int c_remoteButtonRight[]        = { 33, 21, 20, 14, 12,  2 }; // 6
    const int c_remoteButtonPlay[]         = { 33, 21, 20,  8,  2, 33, 21, 20,  8, 2 }; // 10
    const int c_remoteButtonPlayHold[]     = { 37, 33, 21, 20,  2, 37, 33, 21, 20, 2 }; // 10
    const int c_remoteButtonMenu[]         = { 33, 22, 21, 20,  2, 33, 22, 21, 20, 2 }; // 10
    const int c_remoteButtonMenuHold[]     = { 33, 21, 20,  2, 33, 21, 20,  2 }; // 8
    const int c_remoteButtonPlaySecond[]   = { 33, 23, 21, 20,  2, 33, 23, 21, 20, 2 }; // 10
    const int c_remoteControlSwitched[]    = { 19 }; // 1
    
    AppleIRRemoteHIDIOTest remote;
    
    int offset;
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonCentre,10,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonCentre);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonCentreHold,10,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonCentreHold);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonPlus,6,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonPlusPress);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonMinus,6,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonMinusPress);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonLeftTouch,10,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonLeftTouch);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonLeft,6,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonLeftPress);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonRightTouch,10,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonRightTouch);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonRight,6,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonRightPress);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonPlay,10,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonPlay);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonPlayHold,10,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonPlayHold);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonMenu,10,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonMenu);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonMenuHold,8,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonMenuHold);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteButtonPlaySecond,10,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonPlay);
    offset = 0;
    EXPECT_TRUE(remote.getStateMap().find(remote.getParser().find(c_remoteControlSwitched,1,offset)).value()==AppleIRRemoteHIDIO::e_remoteButtonUnknown);
}

//-------------------------------------------------------------------------------------------

class AppleIRRemoteHIDQueueCallbackTest : public AppleIRRemoteHIDIOTest
{
    public:
        MOCK_METHOD2(processQueue,void(const QVector<int>& cookieList,const QVector<int>& indexList));
};

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,queueValueCallbackImplCalledWithErrorCondition)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockQueueRef = 1;
    
    AppleIRRemoteHIDQueueCallbackTest remote;
    
    EXPECT_CALL(remote,getHIDQueue()).Times(1).WillOnce(Return((IOHIDQueueRef)&mockQueueRef));

    remote.testQueueValueCallbackImpl(kIOReturnIOError,(void *)&mockQueueRef);

    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,queueValueCallbackImplQueueIsNotDeviceQueue)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockQueueRef = 1;
    int deviceQueueRef = 2;
    
    AppleIRRemoteHIDQueueCallbackTest remote;
    
    EXPECT_CALL(remote,getHIDQueue()).Times(1).WillOnce(Return((IOHIDQueueRef)&deviceQueueRef));

    remote.testQueueValueCallbackImpl(kIOReturnSuccess,(void *)&mockQueueRef);

    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,queueValueCallbackImplQueueHasNoData)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockQueueRef = 1;
    
    AppleIRRemoteHIDQueueCallbackTest remote;
    
    EXPECT_CALL(remote,getHIDQueue()).Times(1).WillOnce(Return((IOHIDQueueRef)&mockQueueRef));
    EXPECT_CALL(pAPI,IOHIDQueueCopyNextValueWithTimeout((IOHIDQueueRef)&mockQueueRef,0.0))
        .WillRepeatedly(Return((IOHIDValueRef)0));
    
    QVector<int> cookieList,indexList;
    
    EXPECT_CALL(remote,processQueue(A<const QVector<int>&>(),A<const QVector<int>&>())).Times(1)
        .WillOnce(DoAll(SaveArg<0>(&cookieList),SaveArg<1>(&indexList)));

    remote.testQueueValueCallbackImpl(kIOReturnSuccess,(void *)&mockQueueRef);
    
    EXPECT_TRUE(cookieList.size()==0);
    EXPECT_TRUE(indexList.size()==0);
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,queueValueCallbackImplQueueHasDataButNoElement)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockQueueRef = 1;
    int mockValueA = 2;
    
    AppleIRRemoteHIDQueueCallbackTest remote;
    
    EXPECT_CALL(remote,getHIDQueue()).Times(1).WillOnce(Return((IOHIDQueueRef)&mockQueueRef));
    EXPECT_CALL(pAPI,IOHIDQueueCopyNextValueWithTimeout((IOHIDQueueRef)&mockQueueRef,0.0))
        .WillOnce(Return((IOHIDValueRef)&mockValueA))
        .WillRepeatedly(Return((IOHIDValueRef)0));
    EXPECT_CALL(pAPI,IOHIDValueGetElement((IOHIDValueRef)&mockValueA)).Times(1)
        .WillOnce(Return((IOHIDElementRef)0));
    EXPECT_CALL(pAPI,CFRelease((IOHIDValueRef)&mockValueA)).Times(1);
    
    QVector<int> cookieList,indexList;
    
    EXPECT_CALL(remote,processQueue(A<const QVector<int>&>(),A<const QVector<int>&>())).Times(1)
        .WillOnce(DoAll(SaveArg<0>(&cookieList),SaveArg<1>(&indexList)));

    remote.testQueueValueCallbackImpl(kIOReturnSuccess,(void *)&mockQueueRef);
    
    EXPECT_TRUE(cookieList.size()==0);
    EXPECT_TRUE(indexList.size()==0);
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,queueValueCallbackImplQueueHasDataWithIgnoreMarkers)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockQueueRef = 1;
    int mockValueA = 2, mockValueB = 3, mockValueC = 4;
    int mockElementA = 5, mockElementB = 6, mockElementC = 7;
    int indexA = 8, indexB = 9, indexC = 10;
    int cookieA = 5, cookieB = 12, cookieC = 5;
    
    AppleIRRemoteHIDQueueCallbackTest remote;
    
    EXPECT_CALL(remote,getHIDQueue()).Times(1).WillOnce(Return((IOHIDQueueRef)&mockQueueRef));

    EXPECT_CALL(pAPI,IOHIDQueueCopyNextValueWithTimeout((IOHIDQueueRef)&mockQueueRef,0.0))
        .WillOnce(Return((IOHIDValueRef)&mockValueA))
        .WillOnce(Return((IOHIDValueRef)&mockValueB))
        .WillOnce(Return((IOHIDValueRef)&mockValueC))
        .WillRepeatedly(Return((IOHIDValueRef)0));

    EXPECT_CALL(pAPI,IOHIDValueGetElement((IOHIDValueRef)&mockValueA)).Times(1)
        .WillOnce(Return((IOHIDElementRef)&mockElementA));
    EXPECT_CALL(pAPI,IOHIDValueGetElement((IOHIDValueRef)&mockValueB)).Times(1)
        .WillOnce(Return((IOHIDElementRef)&mockElementB));
    EXPECT_CALL(pAPI,IOHIDValueGetElement((IOHIDValueRef)&mockValueC)).Times(1)
        .WillOnce(Return((IOHIDElementRef)&mockElementC));

    EXPECT_CALL(pAPI,IOHIDValueGetIntegerValue((IOHIDValueRef)&mockValueA)).Times(1)
        .WillOnce(Return((CFIndex)indexA));
    EXPECT_CALL(pAPI,IOHIDValueGetIntegerValue((IOHIDValueRef)&mockValueB)).Times(1)
        .WillOnce(Return((CFIndex)indexB));
    EXPECT_CALL(pAPI,IOHIDValueGetIntegerValue((IOHIDValueRef)&mockValueC)).Times(1)
        .WillOnce(Return((CFIndex)indexC));
    
    EXPECT_CALL(pAPI,IOHIDElementGetCookie((IOHIDElementRef)&mockElementA)).Times(1)
        .WillOnce(Return((IOHIDElementCookie)cookieA));
    EXPECT_CALL(pAPI,IOHIDElementGetCookie((IOHIDElementRef)&mockElementB)).Times(1)
        .WillOnce(Return((IOHIDElementCookie)cookieB));
    EXPECT_CALL(pAPI,IOHIDElementGetCookie((IOHIDElementRef)&mockElementC)).Times(1)
        .WillOnce(Return((IOHIDElementCookie)cookieC));
    
    EXPECT_CALL(pAPI,CFRelease((IOHIDElementRef)&mockElementA)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDElementRef)&mockElementB)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDElementRef)&mockElementC)).Times(1);
    
    EXPECT_CALL(pAPI,CFRelease((IOHIDValueRef)&mockValueA)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDValueRef)&mockValueB)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDValueRef)&mockValueC)).Times(1);
    
    QVector<int> cookieList,indexList;
    
    EXPECT_CALL(remote,processQueue(A<const QVector<int>&>(),A<const QVector<int>&>())).Times(1)
        .WillOnce(DoAll(SaveArg<0>(&cookieList),SaveArg<1>(&indexList)));

    remote.testQueueValueCallbackImpl(kIOReturnSuccess,(void *)&mockQueueRef);
    
    EXPECT_TRUE(cookieList.size()==1);
    EXPECT_TRUE(cookieList.at(0)==12);
    
    EXPECT_TRUE(indexList.size()==1);
    EXPECT_TRUE(indexList.at(0)==9);
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,queueValueCallbackImplWithQueueHasData)
{
    HIDDeviceIFSPtr pMockAPI = HIDDeviceIF::instance("mock");
    HIDDeviceIFMock& pAPI = dynamic_cast<HIDDeviceIFMock&>(*(pMockAPI.data()));
    
    int mockQueueRef = 1;
    int mockValueA = 2, mockValueB = 3, mockValueC = 4;
    int mockElementA = 5, mockElementB = 6, mockElementC = 7;
    int indexA = 8, indexB = 9, indexC = 10;
    int cookieA = 11, cookieB = 12, cookieC = 13;
    
    AppleIRRemoteHIDQueueCallbackTest remote;
    
    EXPECT_CALL(remote,getHIDQueue()).Times(1).WillOnce(Return((IOHIDQueueRef)&mockQueueRef));

    EXPECT_CALL(pAPI,IOHIDQueueCopyNextValueWithTimeout((IOHIDQueueRef)&mockQueueRef,0.0))
        .WillOnce(Return((IOHIDValueRef)&mockValueA))
        .WillOnce(Return((IOHIDValueRef)&mockValueB))
        .WillOnce(Return((IOHIDValueRef)&mockValueC))
        .WillRepeatedly(Return((IOHIDValueRef)0));

    EXPECT_CALL(pAPI,IOHIDValueGetElement((IOHIDValueRef)&mockValueA)).Times(1)
        .WillOnce(Return((IOHIDElementRef)&mockElementA));
    EXPECT_CALL(pAPI,IOHIDValueGetElement((IOHIDValueRef)&mockValueB)).Times(1)
        .WillOnce(Return((IOHIDElementRef)&mockElementB));
    EXPECT_CALL(pAPI,IOHIDValueGetElement((IOHIDValueRef)&mockValueC)).Times(1)
        .WillOnce(Return((IOHIDElementRef)&mockElementC));

    EXPECT_CALL(pAPI,IOHIDValueGetIntegerValue((IOHIDValueRef)&mockValueA)).Times(1)
        .WillOnce(Return((CFIndex)indexA));
    EXPECT_CALL(pAPI,IOHIDValueGetIntegerValue((IOHIDValueRef)&mockValueB)).Times(1)
        .WillOnce(Return((CFIndex)indexB));
    EXPECT_CALL(pAPI,IOHIDValueGetIntegerValue((IOHIDValueRef)&mockValueC)).Times(1)
        .WillOnce(Return((CFIndex)indexC));
    
    EXPECT_CALL(pAPI,IOHIDElementGetCookie((IOHIDElementRef)&mockElementA)).Times(1)
        .WillOnce(Return((IOHIDElementCookie)cookieA));
    EXPECT_CALL(pAPI,IOHIDElementGetCookie((IOHIDElementRef)&mockElementB)).Times(1)
        .WillOnce(Return((IOHIDElementCookie)cookieB));
    EXPECT_CALL(pAPI,IOHIDElementGetCookie((IOHIDElementRef)&mockElementC)).Times(1)
        .WillOnce(Return((IOHIDElementCookie)cookieC));
    
    EXPECT_CALL(pAPI,CFRelease((IOHIDElementRef)&mockElementA)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDElementRef)&mockElementB)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDElementRef)&mockElementC)).Times(1);
    
    EXPECT_CALL(pAPI,CFRelease((IOHIDValueRef)&mockValueA)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDValueRef)&mockValueB)).Times(1);
    EXPECT_CALL(pAPI,CFRelease((IOHIDValueRef)&mockValueC)).Times(1);
    
    QVector<int> cookieList,indexList;
    
    EXPECT_CALL(remote,processQueue(A<const QVector<int>&>(),A<const QVector<int>&>())).Times(1)
        .WillOnce(DoAll(SaveArg<0>(&cookieList),SaveArg<1>(&indexList)));

    remote.testQueueValueCallbackImpl(kIOReturnSuccess,(void *)&mockQueueRef);
    
    EXPECT_TRUE(cookieList.size()==3);
    EXPECT_TRUE(cookieList.at(0)==11);
    EXPECT_TRUE(cookieList.at(1)==12);
    EXPECT_TRUE(cookieList.at(2)==13);
    
    EXPECT_TRUE(indexList.size()==3);
    EXPECT_TRUE(indexList.at(0)==8);
    EXPECT_TRUE(indexList.at(1)==9);
    EXPECT_TRUE(indexList.at(2)==10);
    
    HIDDeviceIF::release();
}

//-------------------------------------------------------------------------------------------

class AppleIRRRemoteHIDIOProcessQueueTest : public AppleIRRemoteHIDIOTest
{
    public:
        MOCK_METHOD2(processButton,void(AppleIRRemoteHIDIO::Button button,int indexState));
};

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,processQueueGivenNoQueue)
{
    QVector<int> cookieList,indexList;
    AppleIRRRemoteHIDIOProcessQueueTest remote;
    remote.testProcessQueue(cookieList,indexList);
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,processQueueGivenUnequalQueueLengths)
{
    const int testKeySequence[] = {2, 2, 3, 1};
    const int testIdxSequence[] = {1, 1, 1};
    
    QVector<int> cookieList,indexList;
    for(int i=0;i<4;i++)
    {
        cookieList.append(testKeySequence[i]);
    }
    for(int i=0;i<3;i++)
    {
        indexList.append(testIdxSequence[i]);
    }

    AppleIRRRemoteHIDIOProcessQueueTest remote;
    remote.testProcessQueue(cookieList,indexList);
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,processQueueGivenNoButtonCodes)
{
    const int c_SeqA[] = {1, 2};
    const int c_SeqB[] = {2, 3};
    const int c_SeqC[] = {1, 3};
    const int c_SeqD[] = {1, 1};

    QMap<int,AppleIRRemoteHIDIO::Button> states;
    KeyStateParser parser;
    states.insert(parser.sequence(c_SeqA,2),AppleIRRemoteHIDIO::e_remoteButtonPlusPress);
    states.insert(parser.sequence(c_SeqB,2),AppleIRRemoteHIDIO::e_remoteButtonPlusRelease);
    states.insert(parser.sequence(c_SeqC,2),AppleIRRemoteHIDIO::e_remoteButtonMinusPress);
    states.insert(parser.sequence(c_SeqD,2),AppleIRRemoteHIDIO::e_remoteButtonMinusRelease);

    const int testKeySequence[] = {2, 4, 3, 1};
    const int testIdxSequence[] = {1, 1, 1, 0};
    
    QVector<int> cookieList,indexList;
    for(int i=0;i<4;i++)
    {
        cookieList.append(testKeySequence[i]);
        indexList.append(testIdxSequence[i]);
    }

    AppleIRRRemoteHIDIOProcessQueueTest remote;
    EXPECT_CALL(remote,getKeyParser()).WillRepeatedly(ReturnRef(parser));
    EXPECT_CALL(remote,getButtonStatesMap()).WillRepeatedly(ReturnRef(states));
    
    remote.testProcessQueue(cookieList,indexList);
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,processQueueGivenOneButtonCode)
{
    const int c_SeqA[] = {1, 2};
    const int c_SeqB[] = {2, 3};
    const int c_SeqC[] = {1, 3};
    const int c_SeqD[] = {1, 1};

    QMap<int,AppleIRRemoteHIDIO::Button> states;
    KeyStateParser parser;
    states.insert(parser.sequence(c_SeqA,2),AppleIRRemoteHIDIO::e_remoteButtonPlusPress);
    states.insert(parser.sequence(c_SeqB,2),AppleIRRemoteHIDIO::e_remoteButtonPlusRelease);
    states.insert(parser.sequence(c_SeqC,2),AppleIRRemoteHIDIO::e_remoteButtonMinusPress);
    states.insert(parser.sequence(c_SeqD,2),AppleIRRemoteHIDIO::e_remoteButtonMinusRelease);

    const int testKeySequence[] = {2, 1, 3, 1};
    const int testIdxSequence[] = {1, 1, 1, 0};
    
    QVector<int> cookieList,indexList;
    for(int i=0;i<4;i++)
    {
        cookieList.append(testKeySequence[i]);
        indexList.append(testIdxSequence[i]);
    }

    AppleIRRRemoteHIDIOProcessQueueTest remote;
    EXPECT_CALL(remote,getKeyParser()).WillRepeatedly(ReturnRef(parser));
    EXPECT_CALL(remote,getButtonStatesMap()).WillRepeatedly(ReturnRef(states));
    EXPECT_CALL(remote,processButton(AppleIRRemoteHIDIO::e_remoteButtonMinusPress,2)).Times(1);
    
    remote.testProcessQueue(cookieList,indexList);
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,processQueueGivenThreeSequenceTwoButtonCodes)
{
    const int c_SeqA[] = {1, 2};
    const int c_SeqB[] = {2, 3};
    const int c_SeqC[] = {1, 3};
    const int c_SeqD[] = {1, 1};

    QMap<int,AppleIRRemoteHIDIO::Button> states;
    KeyStateParser parser;
    states.insert(parser.sequence(c_SeqA,2),AppleIRRemoteHIDIO::e_remoteButtonPlusPress);
    states.insert(parser.sequence(c_SeqB,2),AppleIRRemoteHIDIO::e_remoteButtonPlusRelease);
    parser.sequence(c_SeqC,2);
    states.insert(parser.sequence(c_SeqD,2),AppleIRRemoteHIDIO::e_remoteButtonMinusRelease);

    const int testKeySequence[] = {1, 2, 1, 3, 1, 1};
    const int testIdxSequence[] = {0, 1, 1, 0, 1, 1};
    
    QVector<int> cookieList,indexList;
    for(int i=0;i<6;i++)
    {
        cookieList.append(testKeySequence[i]);
        indexList.append(testIdxSequence[i]);
    }

    AppleIRRRemoteHIDIOProcessQueueTest remote;
    EXPECT_CALL(remote,getKeyParser()).WillRepeatedly(ReturnRef(parser));
    EXPECT_CALL(remote,getButtonStatesMap()).WillRepeatedly(ReturnRef(states));
    {
        InSequence s1;
        EXPECT_CALL(remote,processButton(AppleIRRemoteHIDIO::e_remoteButtonPlusPress,1)).Times(1);
        EXPECT_CALL(remote,processButton(AppleIRRemoteHIDIO::e_remoteButtonMinusRelease,2)).Times(1);
    }
    
    remote.testProcessQueue(cookieList,indexList);
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,processQueueGivenThreeSequenceButtonCodesConsequetive)
{
    const int c_SeqA[] = {1, 2};
    const int c_SeqB[] = {2, 3};
    const int c_SeqC[] = {1, 3};
    const int c_SeqD[] = {1, 1};

    QMap<int,AppleIRRemoteHIDIO::Button> states;
    KeyStateParser parser;
    states.insert(parser.sequence(c_SeqA,2),AppleIRRemoteHIDIO::e_remoteButtonPlusPress);
    states.insert(parser.sequence(c_SeqB,2),AppleIRRemoteHIDIO::e_remoteButtonPlusRelease);
    states.insert(parser.sequence(c_SeqC,2),AppleIRRemoteHIDIO::e_remoteButtonMinusPress);
    states.insert(parser.sequence(c_SeqD,2),AppleIRRemoteHIDIO::e_remoteButtonMinusRelease);

    const int testKeySequence[] = {1, 2, 4, 1, 3, 4, 1, 1};
    const int testIdxSequence[] = {0, 1, 1, 1, 0, 1, 1, 1};
    
    QVector<int> cookieList,indexList;
    for(int i=0;i<8;i++)
    {
        cookieList.append(testKeySequence[i]);
        indexList.append(testIdxSequence[i]);
    }

    AppleIRRRemoteHIDIOProcessQueueTest remote;
    EXPECT_CALL(remote,getKeyParser()).WillRepeatedly(ReturnRef(parser));
    EXPECT_CALL(remote,getButtonStatesMap()).WillRepeatedly(ReturnRef(states));
    {
        InSequence s1;
        EXPECT_CALL(remote,processButton(AppleIRRemoteHIDIO::e_remoteButtonPlusPress,1)).Times(1);
        EXPECT_CALL(remote,processButton(AppleIRRemoteHIDIO::e_remoteButtonMinusPress,1)).Times(1);
        EXPECT_CALL(remote,processButton(AppleIRRemoteHIDIO::e_remoteButtonMinusRelease,2)).Times(1);
    }
    
    remote.testProcessQueue(cookieList,indexList);
}

//-------------------------------------------------------------------------------------------

class AppleIRRRemoteHIDIOProcessButtonTest : public AppleIRRemoteHIDIOTest
{
    public:
        AppleIRRRemoteHIDIOProcessButtonTest(AppleIRRemoteHIDIO::Button expect);
        MOCK_METHOD1(printButton,void(AppleIRRemoteHIDIO::Button button));
        void testProcessButton(AppleIRRemoteHIDIO::Button button,int indexState);
        void remoteEvent(RemoteEvent *e);
    protected:
        AppleIRRemoteHIDIO::Button m_expect;
};

//-------------------------------------------------------------------------------------------

AppleIRRRemoteHIDIOProcessButtonTest::AppleIRRRemoteHIDIOProcessButtonTest(AppleIRRemoteHIDIO::Button expect) : m_expect(expect)
{}

//-------------------------------------------------------------------------------------------

void AppleIRRRemoteHIDIOProcessButtonTest::testProcessButton(AppleIRRemoteHIDIO::Button button,int indexState)
{
    processButton(button,indexState);
}

//-------------------------------------------------------------------------------------------

void AppleIRRRemoteHIDIOProcessButtonTest::remoteEvent(RemoteEvent *e)
{
    AppleIRRemoteEvent *pIREvent = dynamic_cast<AppleIRRemoteEvent *>(e);
    EXPECT_TRUE(pIREvent!=0);
    EXPECT_TRUE(pIREvent->button()==m_expect);
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,processButtonNoPress)
{
    for(int buttonIdx=0;buttonIdx<static_cast<int>(AppleIRRemoteHIDIO::e_remoteButtonUnknown);buttonIdx++)
    {
        AppleIRRemoteHIDIO::Button button = static_cast<AppleIRRemoteHIDIO::Button>(buttonIdx);
        
        RemoteIFMock remoteIF;
        AppleIRRRemoteHIDIOProcessButtonTest remote(button);
        
        EXPECT_CALL(remoteIF,remoteEvent(A<RemoteEvent *>())).Times(1).WillOnce(Invoke(&remote,&AppleIRRRemoteHIDIOProcessButtonTest::remoteEvent));
        EXPECT_CALL(remote,getRemoteIF()).WillRepeatedly(Return(&remoteIF));
        
        remote.testProcessButton(button,1);
    }
}

//-------------------------------------------------------------------------------------------

TEST(AppleIRRemoteHIDIO,processButtonPressed)
{
    for(int buttonIdx=0;buttonIdx<static_cast<int>(AppleIRRemoteHIDIO::e_remoteButtonUnknown);buttonIdx++)
    {
        AppleIRRemoteHIDIO::Button button = static_cast<AppleIRRemoteHIDIO::Button>(buttonIdx);
        
        switch(button)
        {
            case AppleIRRemoteHIDIO::e_remoteButtonPlusPress:
            case AppleIRRemoteHIDIO::e_remoteButtonMinusPress:
            case AppleIRRemoteHIDIO::e_remoteButtonLeftPress:
            case AppleIRRemoteHIDIO::e_remoteButtonRightPress:
                button = static_cast<AppleIRRemoteHIDIO::Button>(buttonIdx + 1);
                break;
            default:
                break;
        }
        
        RemoteIFMock remoteIF;
        AppleIRRRemoteHIDIOProcessButtonTest remote(button);
        
        EXPECT_CALL(remoteIF,remoteEvent(A<RemoteEvent *>())).Times(1).WillOnce(Invoke(&remote,&AppleIRRRemoteHIDIOProcessButtonTest::remoteEvent));
        EXPECT_CALL(remote,getRemoteIF()).WillRepeatedly(Return(&remoteIF));
        
        remote.testProcessButton(button,0);
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

