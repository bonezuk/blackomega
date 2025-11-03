#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/KeyControlService.h"

using namespace omega::remote;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

class KeyControlServiceTest : public KeyControlService
{
    public:
        KeyControlServiceTest();
        virtual ~KeyControlServiceTest();
        virtual void testRemoveKeyboardMappings();
};

//-------------------------------------------------------------------------------------------

KeyControlServiceTest::KeyControlServiceTest() : KeyControlService()
{}

//-------------------------------------------------------------------------------------------

KeyControlServiceTest::~KeyControlServiceTest()
{}

//-------------------------------------------------------------------------------------------

void KeyControlServiceTest::testRemoveKeyboardMappings()
{
    removeKeyboardMappings();
}

//-------------------------------------------------------------------------------------------

class KeyControlServiceRemoveKeyboardMappingTest : public KeyControlServiceTest
{
    public:
        MOCK_METHOD0(getKeyStateMap,QMap<KeyCode,QPair<int,common::TimeStamp> >&());
        MOCK_METHOD1(removeKeyRepeatCount,void(const KeyCode& keyCode));
};

//-------------------------------------------------------------------------------------------

TEST(KeyControlService,removeKeyboardMappingGivenEmptyMap)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyMap;
    KeyControlServiceRemoveKeyboardMappingTest kService;
    EXPECT_CALL(kService,getKeyStateMap()).Times(1).WillOnce(ReturnRef(keyMap));

    kService.testRemoveKeyboardMappings();

    EXPECT_TRUE(keyMap.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyControlService,removeKeyboardMappingGivenOneKeyboard)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyMap;
    KeyCode kA(1234);
    keyMap.insert(kA,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));

    KeyControlServiceRemoveKeyboardMappingTest kService;
    EXPECT_CALL(kService,getKeyStateMap()).Times(1).WillOnce(ReturnRef(keyMap));
    EXPECT_CALL(kService,removeKeyRepeatCount(kA)).Times(1);

    kService.testRemoveKeyboardMappings();

    EXPECT_TRUE(keyMap.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyControlService,removeKeyboardMappingGivenOneRemote)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyMap;
    KeyCode kB(QString::fromLatin1("play"));
    keyMap.insert(kB,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));

    KeyControlServiceRemoveKeyboardMappingTest kService;
    EXPECT_CALL(kService,getKeyStateMap()).Times(1).WillOnce(ReturnRef(keyMap));

    kService.testRemoveKeyboardMappings();

    EXPECT_TRUE(keyMap.size()==1);
    EXPECT_TRUE(keyMap.find(kB)!=keyMap.end());
}

//-------------------------------------------------------------------------------------------

TEST(KeyControlService,removeKeyboardMappingGivenKeyAndRemote)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyMap;
    KeyCode kA(1234);
    keyMap.insert(kA,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));
    KeyCode kB(QString::fromLatin1("play"));
    keyMap.insert(kB,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));

    KeyControlServiceRemoveKeyboardMappingTest kService;
    EXPECT_CALL(kService,getKeyStateMap()).Times(1).WillOnce(ReturnRef(keyMap));
    EXPECT_CALL(kService,removeKeyRepeatCount(kA)).Times(1);

    kService.testRemoveKeyboardMappings();

    EXPECT_TRUE(keyMap.size()==1);
    EXPECT_TRUE(keyMap.find(kB)!=keyMap.end());
}

//-------------------------------------------------------------------------------------------

TEST(KeyControlService,removeKeyboardMappingGivenRemoteAndKey)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyMap;
    KeyCode kB(QString::fromLatin1("play"));
    keyMap.insert(kB,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));
    KeyCode kC(1235);
    keyMap.insert(kC,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));

    KeyControlServiceRemoveKeyboardMappingTest kService;
    EXPECT_CALL(kService,getKeyStateMap()).Times(1).WillOnce(ReturnRef(keyMap));
    EXPECT_CALL(kService,removeKeyRepeatCount(kC)).Times(1);

    kService.testRemoveKeyboardMappings();

    EXPECT_TRUE(keyMap.size()==1);
    EXPECT_TRUE(keyMap.find(kB)!=keyMap.end());
}

//-------------------------------------------------------------------------------------------

TEST(KeyControlService,removeKeyboardMappingGivenKeyRemoteKey)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyMap;
    KeyCode kA(1234);
    keyMap.insert(kA,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));
    KeyCode kB(QString::fromLatin1("play"));
    keyMap.insert(kB,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));
    KeyCode kC(1235);
    keyMap.insert(kC,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));

    KeyControlServiceRemoveKeyboardMappingTest kService;
    EXPECT_CALL(kService,getKeyStateMap()).Times(1).WillOnce(ReturnRef(keyMap));
    EXPECT_CALL(kService,removeKeyRepeatCount(kA)).Times(1);
    EXPECT_CALL(kService,removeKeyRepeatCount(kC)).Times(1);

    kService.testRemoveKeyboardMappings();

    EXPECT_TRUE(keyMap.size()==1);
    EXPECT_TRUE(keyMap.find(kB)!=keyMap.end());
}

//-------------------------------------------------------------------------------------------

TEST(KeyControlService,removeKeyboardMappingGivenRemoteKeyRemote)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> > keyMap;
    KeyCode kA(QString::fromLatin1("play"));
    keyMap.insert(kA,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));
    KeyCode kB(1234);
    keyMap.insert(kB,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));
    KeyCode kC(QString::fromLatin1("pause"));
    keyMap.insert(kC,QPair<int,common::TimeStamp>(1,common::TimeStamp::now()));

    KeyControlServiceRemoveKeyboardMappingTest kService;
    EXPECT_CALL(kService,getKeyStateMap()).Times(1).WillOnce(ReturnRef(keyMap));
    EXPECT_CALL(kService,removeKeyRepeatCount(kB)).Times(1);

    kService.testRemoveKeyboardMappings();

    EXPECT_TRUE(keyMap.size()==2);
    EXPECT_TRUE(keyMap.find(kA)!=keyMap.end());
    EXPECT_TRUE(keyMap.find(kC)!=keyMap.end());
}

//-------------------------------------------------------------------------------------------
