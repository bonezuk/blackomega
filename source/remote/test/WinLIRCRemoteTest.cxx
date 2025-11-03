#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/WinLIRCRemote.h"

using namespace omega::remote;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

class WinLIRCRemoteTest : public WinLIRCRemote
{
    public:
        WinLIRCRemoteTest();
        virtual ~WinLIRCRemoteTest();
        virtual void testRemoteCommand(const QString& cmd,int repeat);
};

//-------------------------------------------------------------------------------------------

WinLIRCRemoteTest::WinLIRCRemoteTest() : WinLIRCRemote(0)
{}

//-------------------------------------------------------------------------------------------

WinLIRCRemoteTest::~WinLIRCRemoteTest()
{}

//-------------------------------------------------------------------------------------------

void WinLIRCRemoteTest::testRemoteCommand(const QString& cmd,int repeat)
{
    remoteCommand(cmd,repeat);
}

//-------------------------------------------------------------------------------------------

class WinLIRCRemoteRemoteCommandTest : public WinLIRCRemoteTest
{
    public:
        MOCK_CONST_METHOD0(isRemoteBeingAssignedTo,bool());
        MOCK_METHOD2(processRemoteCommand,void(const KeyCode& cmd,int repeat));
        MOCK_METHOD2(emitOnProgrammed,void(const QString& cmd,int repeat));
};

//-------------------------------------------------------------------------------------------

TEST(WinLIRCRemote,RemoteCommandGivenRemoteIsNotBeingProgrammed)
{
    QString cmd("play");
    KeyCode keyCode(cmd);
    WinLIRCRemoteRemoteCommandTest remote;
    EXPECT_CALL(remote,isRemoteBeingAssignedTo()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(remote,processRemoteCommand(Eq(cmd),2)).Times(1);
    remote.testRemoteCommand(cmd,2);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCRemote,RemoteCommandGivenRemoteIsBeingProgrammed)
{
    QString cmd("play");
    WinLIRCRemoteRemoteCommandTest remote;
    EXPECT_CALL(remote,isRemoteBeingAssignedTo()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,emitOnProgrammed(cmd,2)).Times(1);
    remote.testRemoteCommand(cmd,2);
}

//-------------------------------------------------------------------------------------------
