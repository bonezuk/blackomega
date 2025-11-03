#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/KeyCodeStateEngine.h"
#include "remote/test/KeyAssignmentMock.h"

using namespace omega::remote;
using namespace omega::common;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

class KeyCodeStateEngineTest : public KeyCodeStateEngine
{
    public:
        KeyCodeStateEngineTest();
        virtual ~KeyCodeStateEngineTest();
        virtual void testProcessRemoteCommand(const KeyCode& cmd,int repeat);
        virtual void testProcessTimerCommand(const KeyCode& cmd);
        virtual void testProcessReleaseCommand(const KeyCode& cmd);
        virtual int testGetKeyState(const KeyCode& cmd) const;
        virtual common::TimeStamp testGetKeyTime(const KeyCode& cmd) const;
        virtual void testCreateKeyState(const KeyCode& cmd,int state,const common::TimeStamp& refTime);
        virtual void testRemoveKeyState(const KeyCode& cmd);
        virtual void testUpdateKeyState(const KeyCode& cmd,int state);
        virtual void testUpdateKeyState(const KeyCode& cmd,int state,const common::TimeStamp& refTime);
        virtual bool testIsInTimeWindow(const KeyCode& cmd,double windowLength,double lowerLimit,double upperLimit) const;
        virtual bool testIsDelayWindow(const KeyCode& cmd) const;
        virtual bool testIsDelayWindowBottom(const KeyCode& cmd) const;
        virtual bool testIsDelayWindowTop(const KeyCode& cmd) const;
        virtual bool testIsRepeatWindow(const KeyCode& cmd) const;
        virtual bool testIsFastSignal(const KeyCode& cmd,int repeat) const;
        virtual void testOnProcessTimer();
        virtual void testProcessClick(const KeyCode& cmd);
        virtual void testProcessHold(const KeyCode& cmd);
        virtual void remoteEvent(RemoteEvent *e);
        virtual void onTimerImplementation();
        virtual bool start();
        virtual void stop();
};

//-------------------------------------------------------------------------------------------

KeyCodeStateEngineTest::KeyCodeStateEngineTest() : KeyCodeStateEngine()
{}

//-------------------------------------------------------------------------------------------

KeyCodeStateEngineTest::~KeyCodeStateEngineTest()
{}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::testProcessRemoteCommand(const KeyCode& cmd,int repeat)
{
    processRemoteCommand(cmd,repeat);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::testProcessTimerCommand(const KeyCode& cmd)
{
    processTimerCommand(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::testProcessReleaseCommand(const KeyCode& cmd)
{
    processReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------

int KeyCodeStateEngineTest::testGetKeyState(const KeyCode& cmd) const
{
    return getKeyState(cmd);
}

//-------------------------------------------------------------------------------------------

common::TimeStamp KeyCodeStateEngineTest::testGetKeyTime(const KeyCode& cmd) const
{
    return getKeyTime(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::testCreateKeyState(const KeyCode& cmd,int state,const common::TimeStamp& refTime)
{
    createKeyState(cmd,state,refTime);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::testRemoveKeyState(const KeyCode& cmd)
{
    removeKeyState(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::testUpdateKeyState(const KeyCode& cmd,int state)
{
    updateKeyState(cmd,state);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::testUpdateKeyState(const KeyCode& cmd,int state,const common::TimeStamp& refTime)
{
    updateKeyState(cmd,state,refTime);
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngineTest::testIsInTimeWindow(const KeyCode& cmd,double windowLength,double lowerLimit,double upperLimit) const
{
    return isInTimeWindow(cmd,windowLength,lowerLimit,upperLimit);
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngineTest::testIsDelayWindow(const KeyCode& cmd) const
{
    return isDelayWindow(cmd);
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngineTest::testIsDelayWindowBottom(const KeyCode& cmd) const
{
    return isDelayWindowBottom(cmd);
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngineTest::testIsDelayWindowTop(const KeyCode& cmd) const
{
    return isDelayWindowTop(cmd);
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngineTest::testIsRepeatWindow(const KeyCode& cmd) const
{
    return isRepeatWindow(cmd);
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngineTest::testIsFastSignal(const KeyCode& cmd,int repeat) const
{
    return isFastSignal(cmd,repeat);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::testOnProcessTimer()
{
    onProcessTimer();
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::testProcessClick(const KeyCode& cmd)
{
    processClick(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::testProcessHold(const KeyCode& cmd)
{
    processHold(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::remoteEvent(RemoteEvent *e)
{}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::onTimerImplementation()
{}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngineTest::start()
{
    return true;
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngineTest::stop()
{}

//-------------------------------------------------------------------------------------------

class KeyCodeStateEngineKeyTest : public KeyCodeStateEngineTest
{
    public:
        MOCK_CONST_METHOD0(getReferenceTime,common::TimeStamp());
        MOCK_METHOD0(getKeyStateMap,QMap<KeyCode,QPair<int,common::TimeStamp> >&());
        MOCK_CONST_METHOD0(getKeyStateMapConst,const QMap<KeyCode,QPair<int,common::TimeStamp> >&());
};

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,getKeyStateNoEntryThenStateIsZero)
{
    KeyCode cmd(1234);
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    EXPECT_TRUE(remote.testGetKeyState(cmd)==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,getKeyStateEntryGivesTheState)
{
    KeyCode cmd(1234);
    common::TimeStamp nowT = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,nowT));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    EXPECT_TRUE(remote.testGetKeyState(cmd)==3);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,getKeyTimeNoEntryThenTimeIsZero)
{
    KeyCode cmd(1234);
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    EXPECT_TRUE(remote.testGetKeyTime(cmd)==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,getKeyTimeEntryGivesTheTime)
{
    KeyCode cmd(1234);
    common::TimeStamp nowT = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,nowT));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    EXPECT_TRUE(remote.testGetKeyTime(cmd)==nowT);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,createKeyStateInsertsGivenStateAndTimeWhereNoneExists)
{
    KeyCode cmd(1234);
    common::TimeStamp nowT = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testCreateKeyState(cmd,3,nowT);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==3);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==nowT);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,createKeyStateInsertsGivenStateAndTimeWhenOtherExists)
{
    KeyCode cmd(1234);
    common::TimeStamp nowT = common::TimeStamp::now();
    common::TimeStamp newT = common::TimeStamp::now() + 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,nowT));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testCreateKeyState(cmd,4,newT);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==4);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==newT);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,updateKeyStateNoUpdateIfNoRecordForStateOnly)
{
    KeyCode cmd(1234);
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testUpdateKeyState(cmd,4);

    EXPECT_TRUE(keyStateMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,updateKeyStateWhereOnlyStateIsChanged)
{
    KeyCode cmd(1234);
    common::TimeStamp nowT = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,nowT));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testUpdateKeyState(cmd,4);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==4);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==nowT);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,updateKeyStateNoUpdateIfNoRecordForStateAndTime)
{
    KeyCode cmd(1234);
    common::TimeStamp nowT = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testUpdateKeyState(cmd,4,nowT);

    EXPECT_TRUE(keyStateMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,updateKeyStateWhereStateAndTimeIsChanged)
{
    KeyCode cmd(1234);
    common::TimeStamp nowT = common::TimeStamp::now();
    common::TimeStamp newT = common::TimeStamp::now() + 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,nowT));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testUpdateKeyState(cmd,4,newT);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==4);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==newT);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,removeKeyStateWhenDoesntExist)
{
    KeyCode cmd(1234);
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testRemoveKeyState(cmd);

    EXPECT_TRUE(keyStateMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,removeKeyStateWhenExists)
{
    KeyCode cmd(1234);
    common::TimeStamp nowT = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,nowT));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testRemoveKeyState(cmd);

    EXPECT_TRUE(keyStateMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowFullNoRecord)
{
    KeyCode cmd(1234);
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    EXPECT_FALSE(remote.testIsInTimeWindow(cmd,4.0,0.0,1.0));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowFullWhenTimeIsBeforeWindow)
{
    KeyCode cmd(1234);
    common::TimeStamp windowStart = 5.0;
    common::TimeStamp referenceTime = 4.5;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,windowStart));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(referenceTime));

    EXPECT_FALSE(remote.testIsInTimeWindow(cmd,4.0,0.0,1.0));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowFullWhenTimeIsAfterWindow)
{
    KeyCode cmd(1234);
    common::TimeStamp windowStart = 5.0;
    common::TimeStamp referenceTime = 9.5;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,windowStart));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(referenceTime));

    EXPECT_FALSE(remote.testIsInTimeWindow(cmd,4.0,0.0,1.0));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowFullWhenTimeIsInWindowNearBeginning)
{
    KeyCode cmd(1234);
    common::TimeStamp windowStart = 5.0;
    common::TimeStamp referenceTime = 5.1;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,windowStart));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(referenceTime));

    EXPECT_TRUE(remote.testIsInTimeWindow(cmd,4.0,0.0,1.0));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowFullWhenTimeIsInWindowNearEnd)
{
    KeyCode cmd(1234);
    common::TimeStamp windowStart = 5.0;
    common::TimeStamp referenceTime = 8.9;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,windowStart));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(referenceTime));

    EXPECT_TRUE(remote.testIsInTimeWindow(cmd,4.0,0.0,1.0));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowFullWhenTimeIsInWindowInMiddle)
{
    KeyCode cmd(1234);
    common::TimeStamp windowStart = 5.0;
    common::TimeStamp referenceTime = 7.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,windowStart));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(referenceTime));

    EXPECT_TRUE(remote.testIsInTimeWindow(cmd,4.0,0.0,1.0));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowPartWhenTimeIsBeforeWindow)
{
    KeyCode cmd(1234);
    common::TimeStamp windowStart = 5.0;
    common::TimeStamp referenceTime = 5.9;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,windowStart));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(referenceTime));

    EXPECT_FALSE(remote.testIsInTimeWindow(cmd,4.0,0.25,0.5));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowPartWhenTimeIsAfterWindow)
{
    KeyCode cmd(1234);
    common::TimeStamp windowStart = 5.0;
    common::TimeStamp referenceTime = 7.1;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,windowStart));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(referenceTime));

    EXPECT_FALSE(remote.testIsInTimeWindow(cmd,4.0,0.25,0.5));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowPartWhenTimeIsInWindowNearBeginning)
{
    KeyCode cmd(1234);
    common::TimeStamp windowStart = 5.0;
    common::TimeStamp referenceTime = 6.1;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,windowStart));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(referenceTime));

    EXPECT_TRUE(remote.testIsInTimeWindow(cmd,4.0,0.25,0.5));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowPartWhenTimeIsInWindowNearEnd)
{
    KeyCode cmd(1234);
    common::TimeStamp windowStart = 5.0;
    common::TimeStamp referenceTime = 6.9;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,windowStart));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(referenceTime));

    EXPECT_TRUE(remote.testIsInTimeWindow(cmd,4.0,0.25,0.5));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isInTimeWindowPartWhenTimeIsInWindowInMiddle)
{
    KeyCode cmd(1234);
    common::TimeStamp windowStart = 5.0;
    common::TimeStamp referenceTime = 6.5;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,windowStart));

    KeyCodeStateEngineKeyTest remote;
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(referenceTime));

    EXPECT_TRUE(remote.testIsInTimeWindow(cmd,4.0,0.25,0.5));
}

//-------------------------------------------------------------------------------------------

class KeyCodeStateEngineWindowTest : public KeyCodeStateEngineTest
{
    public:
        MOCK_CONST_METHOD0(getReferenceTime,common::TimeStamp());
        MOCK_METHOD0(getKeyStateMap,QMap<KeyCode,QPair<int,common::TimeStamp> >& ());
        MOCK_CONST_METHOD0(getKeyStateMapConst,const QMap<KeyCode,QPair<int,common::TimeStamp> >& ());
        MOCK_CONST_METHOD4(isInTimeWindow,bool(const KeyCode& cmd,double windowLength,double lowerLimit,double upperLimit));
        MOCK_CONST_METHOD0(getDelayTime,double());
        MOCK_CONST_METHOD0(getRepeatTime,double());
};

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isDelayWindowNoState)
{
    KeyCode cmd(1234);

    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(0.6));

    EXPECT_FALSE(remote.testIsDelayWindow(cmd));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isDelayWindowWhenInRepeatState)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(5,tOld));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(0.6));

    EXPECT_FALSE(remote.testIsDelayWindow(cmd));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isDelayWindowWhenInDelayWindowStates)
{
    for(int i=1;i<=4;i++)
    {
        KeyCode cmd(1234);

        common::TimeStamp tOld = common::TimeStamp::now();
        QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
        keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(i,tOld));

        KeyCodeStateEngineWindowTest remote;
        EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
        EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
        EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(0.6));
        EXPECT_CALL(remote,isInTimeWindow(cmd,0.6,0.0,1.0)).Times(1).WillOnce(Return(true));

        EXPECT_TRUE(remote.testIsDelayWindow(cmd));
    }
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isDelayWindowBottomNoState)
{
    KeyCode cmd(1234);

    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(0.6));

    EXPECT_FALSE(remote.testIsDelayWindowBottom(cmd));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isDelayWindowBottomWhenInRepeatState)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(5,tOld));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(0.6));

    EXPECT_FALSE(remote.testIsDelayWindowBottom(cmd));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isDelayWindowBottomWhenInDelayWindowStates)
{
    for(int i=1;i<=4;i++)
    {
        KeyCode cmd(1234);

        common::TimeStamp tOld = common::TimeStamp::now();
        QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
        keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(i,tOld));

        KeyCodeStateEngineWindowTest remote;
        EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
        EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
        EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(0.6));
        EXPECT_CALL(remote,isInTimeWindow(cmd,0.6,0.0,0.7)).Times(1).WillOnce(Return(true));

        EXPECT_TRUE(remote.testIsDelayWindowBottom(cmd));
    }
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isDelayWindowTopNoState)
{
    KeyCode cmd(1234);

    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(0.6));

    EXPECT_FALSE(remote.testIsDelayWindowTop(cmd));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isDelayWindowTopWhenInRepeatState)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(5,tOld));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(0.6));

    EXPECT_FALSE(remote.testIsDelayWindowTop(cmd));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isDelayWindowTopWhenInDelayWindowStates)
{
    for(int i=1;i<=4;i++)
    {
        KeyCode cmd(1234);

        common::TimeStamp tOld = common::TimeStamp::now();
        QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
        keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(i,tOld));

        KeyCodeStateEngineWindowTest remote;
        EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
        EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
        EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(0.6));
        EXPECT_CALL(remote,isInTimeWindow(cmd,0.6,0.7,1.0)).Times(1).WillOnce(Return(true));

        EXPECT_TRUE(remote.testIsDelayWindowTop(cmd));
    }
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isRepeatWindowNoState)
{
    KeyCode cmd(1234);

    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getRepeatTime()).WillRepeatedly(Return(0.6));

    EXPECT_FALSE(remote.testIsDelayWindowBottom(cmd));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isRepeatWindowWhenInDelayWindowStates)
{
    for(int i=1;i<=4;i++)
    {
        KeyCode cmd(1234);

        common::TimeStamp tOld = common::TimeStamp::now();
        QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
        keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(i,tOld));

        KeyCodeStateEngineWindowTest remote;
        EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
        EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
        EXPECT_CALL(remote,getRepeatTime()).WillRepeatedly(Return(0.6));

        EXPECT_FALSE(remote.testIsRepeatWindow(cmd));
    }
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isRepeatWindowWhenInRepeatState)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(5,tOld));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getRepeatTime()).WillRepeatedly(Return(0.6));
    EXPECT_CALL(remote,isInTimeWindow(cmd,0.6,0.0,1.0)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(remote.testIsRepeatWindow(cmd));
}

//-------------------------------------------------------------------------------------------
// ----- 10 - 11 - 12 - 13 - 14 - 15 - 16 - 17 x 18 x 19 x 20 , 21 , 22 , 23 -----
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignalNoState)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 16.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_FALSE(remote.testIsFastSignal(cmd,1));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignalNoRepeat)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 12.0;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_FALSE(remote.testIsFastSignal(cmd,0));
}

//-------------------------------------------------------------------------------------------
// ----- 10 - 11 - 12 - 13 - 14 - 15 - 16 - 17 x 18 x 19 x 20 , 21 , 22 , 23 -----
// ------ x    -    -    -    -    -    x    -    -    -    -    -    x    -
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignal10SecWindow6SecGap)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 16.0;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_FALSE(remote.testIsFastSignal(cmd,1));
}

//-------------------------------------------------------------------------------------------
// ----- 10 - 11 - 12 - 13 - 14 - 15 - 16 - 17 x 18 x 19 x 20 , 21 , 22 , 23 -----
// -----  x    -    -    -    -    x    -    -    -    -    x    -    -    -
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignal10SecWindowGreater5SecGap)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 15.1;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_FALSE(remote.testIsFastSignal(cmd,1));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignal10SecWindowLess5SecGap)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 14.9;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_TRUE(remote.testIsFastSignal(cmd,1));
}

//-------------------------------------------------------------------------------------------
// ----- 10 - 11 - 12 - 13 - 14 - 15 - 16 - 17 x 18 x 19 x 20 , 21 , 22 , 23 -----
// -----  x    -    -    -    x    -    -    -    x    -    -    -    x    -
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignal10SecWindow4SecGap)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 14.0;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_TRUE(remote.testIsFastSignal(cmd,1));
}

//-------------------------------------------------------------------------------------------
// ----- 10 - 11 - 12 - 13 - 14 - 15 - 16 - 17 x 18 x 19 x 20 , 21 , 22 , 23 -----
// -----  x    -    -    x    -    -    x    -    -    x    -    -    x    -
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignal10SecWindow3SecGapRepeat1)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 13.0;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_TRUE(remote.testIsFastSignal(cmd,1));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignal10SecWindow3SecGapRepeat2)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 16.0;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_TRUE(remote.testIsFastSignal(cmd,2));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignal10SecWindow3SecGapRepeat3)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 19.0;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_TRUE(remote.testIsFastSignal(cmd,3));
}

//-------------------------------------------------------------------------------------------
// ----- 10 - 11 - 12 - 13 - 14 - 15 - 16 - 17 x 18 x 19 x 20 , 21 , 22 , 23 -----
// -----  x    -    x    -    x    -    x    -    x    -    x    -    x    -
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignal10SecWindow2SecGap)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 14.0;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_TRUE(remote.testIsFastSignal(cmd,2));
}

//-------------------------------------------------------------------------------------------
// ----- 10 - 11 - 12 - 13 - 14 - 15 - 16 - 17 x 18 x 19 x 20 , 21 , 22 , 23 -----
// ------ x    x    x    x    x    x    x    x    x    x    x    x    x    x
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignal10SecWindow1SecGap)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 11.0;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_TRUE(remote.testIsFastSignal(cmd,1));
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,isFastSignal10SecWindow0SecGap)
{
    KeyCode cmd(1234);

    common::TimeStamp tRef = 10.0;
    common::TimeStamp tStart = 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tStart));

    KeyCodeStateEngineWindowTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(10.0));
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tRef));

    EXPECT_TRUE(remote.testIsFastSignal(cmd,1));
}

//-------------------------------------------------------------------------------------------

class KeyCodeStateEngineStateTest : public KeyCodeStateEngineTest
{
    public:
        MOCK_CONST_METHOD0(getReferenceTime,common::TimeStamp());
        MOCK_METHOD0(getKeyStateMap,QMap<KeyCode,QPair<int,common::TimeStamp> >&());
        MOCK_CONST_METHOD0(getKeyStateMapConst,const QMap<KeyCode,QPair<int,common::TimeStamp> >&());
        MOCK_CONST_METHOD2(isFastSignal,bool(const KeyCode& cmd,int repeat));

        MOCK_CONST_METHOD0(getDelayTime,double());
        MOCK_CONST_METHOD0(getRepeatTime,double());

        MOCK_CONST_METHOD1(isDelayWindow,bool(const KeyCode& cmd));
        MOCK_CONST_METHOD1(isDelayWindowBottom,bool(const KeyCode& cmd));
        MOCK_CONST_METHOD1(isDelayWindowTop,bool(const KeyCode& cmd));
        MOCK_CONST_METHOD1(isRepeatWindow,bool(const KeyCode& cmd));

        MOCK_METHOD1(processClick,void(const KeyCode& cmd));
        MOCK_METHOD1(processHold,void(const KeyCode& cmd));
};

//-------------------------------------------------------------------------------------------
// Given : No button press state for given key
// When  : Signal for key is received indicating that button has just been pressed (repeat = 0)
// Then  : Create state record for key, set state=1, reference time start of delay window.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalButtonNotPressedAndFirstKeySignal)
{
    KeyCode cmd(1234);
    common::TimeStamp tNow = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testProcessRemoteCommand(cmd,0);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==1);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNow);
}

//-------------------------------------------------------------------------------------------
// Condition could occur when connecting to WinLIRC server and communication is received.
// Given : No button press state for given key
// When  : Signal for key is received indicating that button has already been pressed and fast signal (repeat > 0)
// Then  : Create state record for key, set state=5, reference time start of repeat window.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalButtonNotPressedAndMultipleKeySignal)
{
    KeyCode cmd(1234);
    common::TimeStamp tNow = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testProcessRemoteCommand(cmd,1);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==5);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNow);
}

//-------------------------------------------------------------------------------------------
// Implies that button has been pressed and then quickly pressed again before state transition.
// Given : Initial button signal for key has been received
// When  : Signal for key is received indicating that button has just been pressed (repeat = 0)
// Then  : Emit click event for button, keep state=1, reference time start of delay window.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalInitialSignalReceivedAndFirstKeySignal)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(1,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,processClick(cmd)).Times(1);

    remote.testProcessRemoteCommand(cmd,0);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==1);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNow);
}

//-------------------------------------------------------------------------------------------
// At this point first and then second signal is received and time difference used to determine if given signal
// is fast or slow. This test processes the condition that the signal is determined to be fast. i.e. A repeat
// signal is expected in top part of delay window if a hold action is to be indicated from a click.
// Given : Initial button signal for key has been received
// When  : Signal for key is received indicating that button has already been pressed and fast signal (repeat > 0)
// Then  : Record of state is modified, state=2, mark signal as fast from state.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalInitialSignalReceivedAndMultipleKeySignalAndFast)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(1,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isFastSignal(cmd,3)).Times(1).WillOnce(Return(true));

    remote.testProcessRemoteCommand(cmd,3);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==2);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// At this point first and then second signal is received and time difference used to determine if given signal
// is fast or slow. This test is processes the condition that the signal is determined to be slow. i.e. A repeat
// signal is not expected to occur in top part of delay window and this repeat signal is the indication of a hold action.
// Given : Initial button signal for key has been received
// When  : Signal for key is received indicating that button has already been pressed and slow signal (repeat > 0)
// Then  : Record of state is modified, state=3, mark signal as slow from state.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalInitialSignalReceivedAndMultipleKeySignalAndSlow)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(1,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isFastSignal(cmd,3)).Times(1).WillOnce(Return(false));

    remote.testProcessRemoteCommand(cmd,3);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==3);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// The given state implies that a fast signal was given but not held long enough as to indicate a hold action
// but rather a click action.
// Given : Repeat signal has been received and determined to be fast
// When  : Signal for key is received indicating that button has just been pressed (repeat = 0)
// Then  : Click event for given key is done. Reset state information, state = 1, signal type unknown, begin delay window.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalFastRepeatSignalAndFirstKeySignal)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,processClick(cmd)).Times(1);

    remote.testProcessRemoteCommand(cmd,0);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==1);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNow);
}

//-------------------------------------------------------------------------------------------
// The next repeat signal is received in the fast signal sequence and the signal is still determined to be fast.
// The refence time is taken to determine if signal is bottom, top or post delay window. If not in the bottom of delay window
// then it will imply a hold action.
// Given : Repeat signal has been received and determined to be fast,
// When  : Signal for key is received indicating that button has already been pressed and fast signal in bottom of delay window (repeat > 0)
// Then  : No change as type fast remains the same.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalFastRepeatSignalAndMultipleKeySignalAndFastWithTimeInBottomOfDelay)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isFastSignal(cmd,3)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,isDelayWindowBottom(cmd)).Times(1).WillOnce(Return(true));

    remote.testProcessRemoteCommand(cmd,3);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==2);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// The next repeat signal is received in the fast signal sequence and the signal is still determined to be fast.
// The refence time is taken to determine if signal is bottom, top or post delay window. If not in the bottom of delay window
// then it will imply a hold action.
// Given : Repeat signal has been received and determined to be fast
// When  : Signal for key is received indicating that button has already been pressed and fast signal in top of delay window (repeat > 0)
// Then  : Modify record, state = 4.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalFastRepeatSignalAndMultipleKeySignalAndFastWithTimeNotInBottom)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isFastSignal(cmd,3)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,isDelayWindowBottom(cmd)).Times(1).WillOnce(Return(false));

    remote.testProcessRemoteCommand(cmd,3);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==4);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// The next repeat signal is received in the fast signal sequence and the signal is however determined to be slow.
// This will imply a hold action is emitted when the time delay window expires.
// Given : Repeat signal has been received and determined to be fast
// When  : Signal for key is received indicating that button has already been pressed and slow signal (repeat > 0)
// Then  : Modify record, state = 3, set type to slow via state.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalFastRepeatSignalAndMultipleKeySignalAndSlow)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isFastSignal(cmd,3)).Times(1).WillOnce(Return(false));

    remote.testProcessRemoteCommand(cmd,3);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==3);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// Current state implies a hold action but received signal determines a new button action. Depending
// on when the received signal is processed determines the action emitted for the current key state.
// If in the delay window of current action then it implies a click action if outside then implies a
// hold action.
// Given : Repeat signal has been received and determined to be slow
// When  : Signal for key is received indicating that button has just been pressed and processed in current delay window (repeat = 0)
// Then  : Emit a click event for key, set state = 1, reset the delay timer window.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalSlowRepeatSignalAndFirstKeySignalInsideDelayWindow)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isDelayWindow(cmd)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,processClick(cmd)).Times(1);

    remote.testProcessRemoteCommand(cmd,0);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==1);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNow);
}

//-------------------------------------------------------------------------------------------
// Current state implies a hold action but received signal determines a new button action. Depending
// on when the received signal is processed determines the action emitted for the current key state.
// If in the delay window of current action then it implies a click action if outside then implies a
// hold action.
// Given : Repeat signal has been received and determined to be slow
// When  : Signal for key is received indicating that button has just been pressed (repeat = 0)
// Then  : Emit a hold event for key, set state = 1, reset the delay timer window.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalSlowRepeatSignalAndFirstKeySignalAfterDelayHasExpired)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isDelayWindow(cmd)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(remote,processHold(cmd)).Times(1);

    remote.testProcessRemoteCommand(cmd,0);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==1);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNow);
}

//-------------------------------------------------------------------------------------------
// The state becomes either 2 or 4 depending on where in the delay window the signal is found.
// Given : Repeat signal has been received and determined to be slow
// When  : Signal for key is received indicating that button has already been pressed and fast signal (repeat > 0)
// Then  : Modify state = 2
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalSlowRepeatSignalAndMultipleKeySignalAndFastWithSignalInsideBottomDelayWindow)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isFastSignal(cmd,3)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,isDelayWindowBottom(cmd)).Times(1).WillOnce(Return(true));

    remote.testProcessRemoteCommand(cmd,3);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==2);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// The state becomes either 2 or 4 depending on where in the delay window the signal is found.
// Given : Repeat signal has been received and determined to be slow
// When  : Signal for key is received indicating that button has already been pressed and fast signal (repeat > 0)
// Then  : Modify state = 4
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalSlowRepeatSignalAndMultipleKeySignalAndFastWithSignalOutsideBottomDelayWindow)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isFastSignal(cmd,3)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,isDelayWindowBottom(cmd)).Times(1).WillOnce(Return(false));

    remote.testProcessRemoteCommand(cmd,3);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==4);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// No change in the signal type and so emitting hold event falls to timer when delay has expired.
// Given : Repeat signal has been received and determined to be slow
// When  : Signal for key is received indicating that button has already been pressed and slow signal (repeat > 0)
// Then  : No change in state of key
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalSlowRepeatSignalAndMultipleKeySignalAndSlow)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNow = tOld + 1.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNow));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isFastSignal(cmd,3)).Times(1).WillOnce(Return(false));

    remote.testProcessRemoteCommand(cmd,3);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==3);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// This implies that a hold action is to be processed but a new button action is being carried out.
// Given : Repeat signal has been received in the top part of delay window
// When  : Signal for key is received indicating that button has just been pressed (repeat = 0)
// Then  : Emit hold event, state = 1, and set time of delay window to current.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalRepeatInTopDelayWinowAndFirstKeySignal)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNew = tOld + 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(4,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNew));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,processHold(cmd)).Times(1);

    remote.testProcessRemoteCommand(cmd,0);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==1);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNew);
}

//-------------------------------------------------------------------------------------------
// The hold event is only emitted after the delay window is complete.
// Given : Repeat signal has been received in the top part of delay window
// When  : Signal for key is received indicating that button has already been pressed (repeat > 0)
// Then  : No change to key state
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalRepeatInTopDelayWinowAndMultipleKeySignal)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(4,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testProcessRemoteCommand(cmd,3);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==4);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// Processing for the given key is done as hold event is fired on stepping into state 5
// Given : Hold action has been determined and emitted
// When  : Signal for key is received indicating that button has just been pressed (repeat = 0)
// Then  : Set state = 1, set start of new time delay window to reference time
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalRegularHoldSignalAndFirstKeySignal)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNew = tOld + 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(5,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNew));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testProcessRemoteCommand(cmd,0);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==1);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNew);
}

//-------------------------------------------------------------------------------------------
// The key on the remote is still being held down so reset the repeat window, the factor that it is fast or slow doesn't matter now
// Given : Hold action has been determined and emitted
// When  : Signal for key is received indicating that button has already been pressed (repeat > 0)
// Then  : Set start of time for repeat window to reference time
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteSignalRegularHoldSignalAndMultipleKeySignal)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNew = tOld + 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(5,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNew));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testProcessRemoteCommand(cmd,3);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==5);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNew);
}

//-------------------------------------------------------------------------------------------
// A condition of state 0 implies that no action is present for this key hence no time information relating to delay or repeat window
// Given : No button press state for given key
// When  : Timer is called
// Then  : No record should exist before or after timer event.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerButtonNotPressed)
{
    KeyCode cmd(1234);
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.isEmpty());
}

//-------------------------------------------------------------------------------------------
// No action in this state until delay window has expired
// Given : Initial button signal for key has been received
// When  : Timer is called and bottom part delay window
// Then  : Record of state remains unchanged
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerInitialSignalReceivedAndTimeInDelayWindow)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(1,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isDelayWindow(cmd)).Times(1).WillOnce(Return(true));

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==1);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// Given that no other signal has been received for the given button then click event is emitted.
// Given : Initial button signal for key has been received
// When  : Timer is called for first time after delay window expires
// Then  : Click event for given key is done, record for key is removed.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerInitialSignalReceivedAndDelayWindowHasExpired)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(1,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isDelayWindow(cmd)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(remote,processClick(cmd)).Times(1);

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.isEmpty());
}

//-------------------------------------------------------------------------------------------
// No action taken until the delay window has actually expired.
// Given : Repeat signal has been received and determined to be fast
// When  : Timer is called and bottom part delay window
// Then  : Record of state remains unchanged
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerFastRepeatSignalAndTimeInDelayWindow)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isDelayWindow(cmd)).Times(1).WillOnce(Return(true));

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==2);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// As still in this state then condition that repeat signal happens in top of delay window is not met
// thus it is determined that click event.
// Given : Repeat signal has been received and determined to be fast
// When  : Timer is called for first time after delay window expires
// Then  : Emit a click event for key, remove record of state for given key.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerFastRepeatSignalAndDelayWindowHasExpired)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(2,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isDelayWindow(cmd)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(remote,processClick(cmd)).Times(1);

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.isEmpty());
}

//-------------------------------------------------------------------------------------------
// Given : Repeat signal has been received and determined to be slow
// When  : Timer is called and delay window
// Then  : No change in state of key
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerSlowRepeatSignalAndTimeInDelayWindow)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNew = tOld + 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNew));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isDelayWindow(cmd)).Times(1).WillOnce(Return(true));

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==3);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// Given : Repeat signal has been received and determined to be slow
// When  : Timer is called for first time after delay window expires
// Then  : Emit hold signal, set state = 6, and set repeat interval to start of delay window expiring.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerSlowRepeatSignalAndDelayWindowHasExpired)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNew = tOld + 10.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(3,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getReferenceTime()).WillRepeatedly(Return(tNew));
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isDelayWindow(cmd)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(remote,processHold(cmd)).Times(1);

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==6);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNew);
}

//-------------------------------------------------------------------------------------------
// No action until the delay window has elapsed
// Given : Repeat signal has been received in the top part of delay window
// When  : Timer is called and top part delay window
// Then  : No change to key state
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerRepeatInTopDelayWinowAndTimeInTopOfDelayWindow)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(4,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(4.0));
    EXPECT_CALL(remote,isDelayWindowTop(cmd)).Times(1).WillOnce(Return(true));

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==4);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// As delay window has expired hold event is now fired and repeat window is started
// Given : Repeat signal has been received in the top part of delay window
// When  : Timer is called for first time after delay window expires
// Then  : Emit hold event, state = 6, time set to reference end of delay window.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerRepeatInTopDelayWinowAndDelayWindowHasExpired)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    common::TimeStamp tNew = tOld + 4.0;
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(4,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getDelayTime()).WillRepeatedly(Return(4.0));
    EXPECT_CALL(remote,isDelayWindowTop(cmd)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(remote,processHold(cmd)).Times(1);

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==6);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tNew);
}

//-------------------------------------------------------------------------------------------
// Indicates that the hold button is still being pressed within the given expected time interval
// Given : Hold action has been determined and emitted
// When  : Timer is called in repeat interval
// Then  : Emit hold event, time of start of repeat window is still the same
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerRegularHoldSignalAndTimeInRepeatInterval)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(5,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isRepeatWindow(cmd)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,processHold(cmd)).Times(1);

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.find(cmd)!=keyStateMap.end());
    EXPECT_TRUE(keyStateMap.find(cmd).value().first==6);
    EXPECT_TRUE(keyStateMap.find(cmd).value().second==tOld);
}

//-------------------------------------------------------------------------------------------
// Indicates that no hold button signal has been received and thus the action of holding down the button
// has been completed.
// Given : Hold action has been determined and emitted
// When  : Timer is called after repeat interval
// Then  : Remove/reset the state for the given key. i.e. state = 0
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,remoteTimerRegularHoldSignalAndTimeAfterRepeatInterval)
{
    KeyCode cmd(1234);

    common::TimeStamp tOld = common::TimeStamp::now();
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmd,QPair<int,common::TimeStamp>(5,tOld));

    KeyCodeStateEngineStateTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,getKeyStateMapConst()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,isRepeatWindow(cmd)).Times(1).WillOnce(Return(false));

    remote.testProcessTimerCommand(cmd);

    EXPECT_TRUE(keyStateMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

class KeyCodeStateEngineOnProcessTimerTest : public KeyCodeStateEngineTest
{
    public:
        MOCK_METHOD0(getKeyStateMap,QMap<KeyCode,QPair<int,common::TimeStamp> >&());
        MOCK_METHOD1(processTimerCommand,void(const KeyCode& cmd));
};

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,onProcessTimerNoStatesToProcess)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;

    KeyCodeStateEngineOnProcessTimerTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));

    remote.testOnProcessTimer();
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,onProcessTimerProcessThreeStates)
{
    common::TimeStamp tA = 10.0, tB = 11.0, tC = 12.0;
    KeyCode cmdA(1234),cmdB(1235),cmdC(1236);
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyStateMap;
    keyStateMap.insert(cmdA,QPair<int,common::TimeStamp>(1,tA));
    keyStateMap.insert(cmdB,QPair<int,common::TimeStamp>(2,tB));
    keyStateMap.insert(cmdC,QPair<int,common::TimeStamp>(3,tC));

    KeyCodeStateEngineOnProcessTimerTest remote;
    EXPECT_CALL(remote,getKeyStateMap()).WillRepeatedly(ReturnRef(keyStateMap));
    EXPECT_CALL(remote,processTimerCommand(cmdA)).Times(1);
    EXPECT_CALL(remote,processTimerCommand(cmdB)).Times(1);
    EXPECT_CALL(remote,processTimerCommand(cmdC)).Times(1);

    remote.testOnProcessTimer();
}

//-------------------------------------------------------------------------------------------

class KeyCodeStateEngineProcessActionTest : public KeyCodeStateEngineTest
{
    public:
        MOCK_METHOD0(playPauseClickSignal,void());
        MOCK_METHOD0(previousTrackClickSignal,void());
        MOCK_METHOD0(nextTrackClickSignal,void());
        MOCK_METHOD0(seekBackSignal,void());
        MOCK_METHOD0(seekForwardSignal,void());
        MOCK_METHOD0(volumeDownIncrementSignal,void());
        MOCK_METHOD0(volumeDownHoldSignal,void());
        MOCK_METHOD0(volumeUpIncrementSignal,void());
        MOCK_METHOD0(volumeUpHoldSignal,void());
        MOCK_METHOD0(getKeyAssignment,KeyAssignmentSPtr());
};

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processClickGivenPlay)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyPlay));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,playPauseClickSignal()).Times(1);
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessClick(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processClickGivenPreviousTrack)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyPreviousTrack));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,previousTrackClickSignal()).Times(1);
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessClick(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processClickGivenNextTrack)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyNextTrack));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,nextTrackClickSignal()).Times(1);
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessClick(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processClickGivenVolumeDown)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyVolumeDown));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,volumeDownIncrementSignal()).Times(1);
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessClick(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processClickGivenVolumeUp)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyVolumeUp));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,volumeUpIncrementSignal()).Times(1);
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessClick(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processClickGivenUnassigned)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyUnassigned));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessClick(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processHoldGivenPlay)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyPlay));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessHold(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processHoldGivenPreviousTrack)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyPreviousTrack));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,seekBackSignal()).Times(1);
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessHold(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processHoldGivenNextTrack)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyNextTrack));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,seekForwardSignal()).Times(1);
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessHold(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processHoldGivenVolumeDown)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyVolumeDown));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,volumeDownIncrementSignal()).Times(1);
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessHold(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processHoldGivenVolumeUp)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyVolumeUp));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,volumeUpIncrementSignal()).Times(1);
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessHold(keyCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processHoldGivenUnassigned)
{
    KeyCode keyCode(1234);

    KeyAssignmentSPtr pKeyAssignment(new KeyAssignmentMock);
    KeyAssignmentMock& keyAssignment = dynamic_cast<KeyAssignmentMock&>(*(pKeyAssignment.data()));
    EXPECT_CALL(keyAssignment,assignedTo(Eq(keyCode))).Times(1).WillOnce(Return(KeyAssignment::e_keyUnassigned));

    KeyCodeStateEngineProcessActionTest stateEngine;
    EXPECT_CALL(stateEngine,getKeyAssignment()).Times(1).WillOnce(Return(pKeyAssignment));

    stateEngine.testProcessHold(keyCode);
}

//-------------------------------------------------------------------------------------------
//
// The distinction in processing a release event is when it occurs within the given
// time window. If it happens, given that code under consideration has a pressed state,
// within the initial delay window then a click routine. Otherwise if it occurs after
// the repeat window then it is considered to be a hold action.
//
// Unlike an initial key press which reflects the beginning of an event, interupting a
// previous event, the release event is the end of an event and thus always resets the
// given key state back to 0 (zero). Thus NO process existing event before continuing
// on to determine state of a new event as there is NO new event now happening.
//
// The different actions will be based on when the key release event occurs as opposed to
// when. Given that only the timer implementation actually calls either click once or
// hold (N number of times) the release action doesn't process the signal unless it has
// not already been given.
//-------------------------------------------------------------------------------------------

class KeyCodeStateEngineProcessReleaseButton : public KeyCodeStateEngineTest
{
    public:
        MOCK_CONST_METHOD1(getKeyState,int(const KeyCode& cmd));
        MOCK_METHOD1(removeKeyState,void(const KeyCode& cmd));
        MOCK_CONST_METHOD1(isDelayWindow,bool(const KeyCode& cmd));
        MOCK_METHOD1(processClick,void(const KeyCode& cmd));
        MOCK_METHOD1(processHold,void(const KeyCode& cmd));
};

//-------------------------------------------------------------------------------------------
// As no key press state has been given then this implies that no action can be processed.
// Given : No button press state for given key
// When  : Key release event is recieved
// Then  : Remove key state from map
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseButtonNotPressed)
{
    KeyCode key(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(key)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(keyState,removeKeyState(key)).Times(1);
    keyState.testProcessReleaseCommand(key);
}

//-------------------------------------------------------------------------------------------
// Given : Initial button signal for key has been received
// When  : Key release event is recieved and reference time is inside the delay window
// Then  : Process click command and remove key state from map.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseButtonInitalSignalAndInsideDelayWindow)
{
    KeyCode cmd(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(cmd)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(keyState,isDelayWindow(cmd)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(keyState,processClick(cmd)).Times(1);
    EXPECT_CALL(keyState,removeKeyState(cmd)).Times(1);
    keyState.testProcessReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------
// Given : Initial button signal for key has been received
// When  : Key release event is recieved and reference time is outside the delay window
// Then  : Process hold command and remove key state from map.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseButtonInitalSignalAndOutsideDelayWindow)
{
    KeyCode cmd(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(cmd)).Times(1).WillOnce(Return(1));
    EXPECT_CALL(keyState,isDelayWindow(cmd)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(keyState,processHold(cmd)).Times(1);
    EXPECT_CALL(keyState,removeKeyState(cmd)).Times(1);
    keyState.testProcessReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------
// Given : Repeat signal has been received and determined to be fast
// When  : Key release event is recieved and reference time is inside the delay window
// Then  : Process click command and remove key state from map.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseButtonFastRepeatSignalAndInsideDelayWindow)
{
    KeyCode cmd(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(cmd)).Times(1).WillOnce(Return(2));
    EXPECT_CALL(keyState,isDelayWindow(cmd)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(keyState,processClick(cmd)).Times(1);
    EXPECT_CALL(keyState,removeKeyState(cmd)).Times(1);
    keyState.testProcessReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------
// Given : Repeat signal has been received and determined to be fast
// When  : Key release event is recieved and reference time is outside the delay window
// Then  : Process hold command and remove key state from map.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseButtonFastRepeatSignalAndOutsideDelayWindow)
{
    KeyCode cmd(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(cmd)).Times(1).WillOnce(Return(2));
    EXPECT_CALL(keyState,isDelayWindow(cmd)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(keyState,processHold(cmd)).Times(1);
    EXPECT_CALL(keyState,removeKeyState(cmd)).Times(1);
    keyState.testProcessReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------
// Given : Repeat signal has been received and determined to be slow
// When  : Key release event is recieved and reference time is inside the delay window
// Then  : Process click command and remove key state from map.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseSlowRepeatSignalAndInsideDelayWindow)
{
    KeyCode cmd(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(cmd)).Times(1).WillOnce(Return(3));
    EXPECT_CALL(keyState,isDelayWindow(cmd)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(keyState,processClick(cmd)).Times(1);
    EXPECT_CALL(keyState,removeKeyState(cmd)).Times(1);
    keyState.testProcessReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------
// Given : Repeat signal has been received and determined to be slow
// When  : Key release event is recieved and reference time is outside the delay window
// Then  : Process hold command and remove key state from map.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseSlowRepeatSignalAndOutsideDelayWindow)
{
    KeyCode cmd(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(cmd)).Times(1).WillOnce(Return(3));
    EXPECT_CALL(keyState,isDelayWindow(cmd)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(keyState,processHold(cmd)).Times(1);
    EXPECT_CALL(keyState,removeKeyState(cmd)).Times(1);
    keyState.testProcessReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------
// Given : Repeat signal has been received in the top part of delay window
// When  : Key release event is recieved and reference time is inside the delay window
// Then  : Process click command and remove key state from map.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseInTopDelayWindowAndInsideDelayWindow)
{
    KeyCode cmd(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(cmd)).Times(1).WillOnce(Return(4));
    EXPECT_CALL(keyState,isDelayWindow(cmd)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(keyState,processClick(cmd)).Times(1);
    EXPECT_CALL(keyState,removeKeyState(cmd)).Times(1);
    keyState.testProcessReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------
// Given : Repeat signal has been received in the top part of delay window
// When  : Key release event is recieved and reference time is outside the delay window
// Then  : Process hold command and remove key state from map.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseInTopDelayWindowAndOutsideDelayWindow)
{
    KeyCode cmd(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(cmd)).Times(1).WillOnce(Return(4));
    EXPECT_CALL(keyState,isDelayWindow(cmd)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(keyState,processHold(cmd)).Times(1);
    EXPECT_CALL(keyState,removeKeyState(cmd)).Times(1);
    keyState.testProcessReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------
// Given : Hold action has been determined but hold signal not emitted
// When  : Key release event is recieved
// Then  : Process hold command and remove key state from map.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseHoldSignalNotEmitted)
{
    KeyCode cmd(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(cmd)).Times(1).WillOnce(Return(5));
    EXPECT_CALL(keyState,processHold(cmd)).Times(1);
    EXPECT_CALL(keyState,removeKeyState(cmd)).Times(1);
    keyState.testProcessReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------
// Given : Hold action has been determined and hold signal has been emitted
// When  : Key release event is recieved
// Then  : Remove key state from map.
//-------------------------------------------------------------------------------------------

TEST(KeyCodeStateEngine,processReleaseHoldSignalEmitted)
{
    KeyCode cmd(1234);
    KeyCodeStateEngineProcessReleaseButton keyState;
    EXPECT_CALL(keyState,getKeyState(cmd)).Times(1).WillOnce(Return(6));
    EXPECT_CALL(keyState,removeKeyState(cmd)).Times(1);
    keyState.testProcessReleaseCommand(cmd);
}

//-------------------------------------------------------------------------------------------
