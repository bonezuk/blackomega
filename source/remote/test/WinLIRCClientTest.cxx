#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/WinLIRCClient.h"
#include "remote/test/WinLIRCSettingsMock.h"

using namespace omega::remote;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

class WinLIRCClientTest : public WinLIRCClient
{
    public:
        WinLIRCClientTest();
        virtual ~WinLIRCClientTest();
        void testProcessLine(const QString& line);
        QStringList testGetStringParts(const QString& line) const;
        bool testParseIRLine(const QString& line,int& code,int& repeat,QString& cmd,QString& remote) const;
        bool testIsValidCode(const QString& code) const;
        void testProcessNotConnected(const LIRCCommand& cmd);
        void testProcessConnected();
};

//-------------------------------------------------------------------------------------------

WinLIRCClientTest::WinLIRCClientTest() : WinLIRCClient(0,0)
{}

//-------------------------------------------------------------------------------------------

WinLIRCClientTest::~WinLIRCClientTest()
{}

//-------------------------------------------------------------------------------------------

void WinLIRCClientTest::testProcessLine(const QString& line)
{
    processLine(line);
}

//-------------------------------------------------------------------------------------------

QStringList WinLIRCClientTest::testGetStringParts(const QString& line) const
{
    return getStringParts(line);
}

//-------------------------------------------------------------------------------------------

bool WinLIRCClientTest::testParseIRLine(const QString& line,int& code,int& repeat,QString& cmd,QString& remote) const
{
    return parseIRLine(line,code,repeat,cmd,remote);
}

//-------------------------------------------------------------------------------------------

bool WinLIRCClientTest::testIsValidCode(const QString& code) const
{
    return isValidCode(code);
}

//-------------------------------------------------------------------------------------------

void WinLIRCClientTest::testProcessNotConnected(const LIRCCommand& cmd)
{
    processNotConnected(cmd);
}

//-------------------------------------------------------------------------------------------

void WinLIRCClientTest::testProcessConnected()
{
    processConnected();
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClientLIRCCommand,defaultContructor)
{
    WinLIRCClient::LIRCCommand cmd;
    EXPECT_TRUE(cmd.command()==WinLIRCClient::LIRCCommand::e_UnknownCommand);
    EXPECT_TRUE(cmd.host().isEmpty());
    EXPECT_TRUE(cmd.port()==8765);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClientLIRCCommand,copyConstructor)
{
    WinLIRCClient::LIRCCommand cmdA;
    cmdA.command() = WinLIRCClient::LIRCCommand::e_OpenConnection;
    cmdA.host() = "localhost";
    cmdA.port() = 5000;

    WinLIRCClient::LIRCCommand cmdB(cmdA);
    EXPECT_TRUE(cmdB.command()==WinLIRCClient::LIRCCommand::e_OpenConnection);
    EXPECT_TRUE(cmdB.host()=="localhost");
    EXPECT_TRUE(cmdB.port()==5000);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClientLIRCCommand,equalsOperator)
{
    WinLIRCClient::LIRCCommand cmdA;
    cmdA.command() = WinLIRCClient::LIRCCommand::e_OpenConnection;
    cmdA.host() = "localhost";
    cmdA.port() = 5000;

    WinLIRCClient::LIRCCommand cmdB;
    cmdB = cmdA;
    EXPECT_TRUE(cmdB.command()==WinLIRCClient::LIRCCommand::e_OpenConnection);
    EXPECT_TRUE(cmdB.host()=="localhost");
    EXPECT_TRUE(cmdB.port()==5000);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClientLIRCCommand,command)
{
    WinLIRCClient::LIRCCommand cmd;
    cmd.command() =  WinLIRCClient::LIRCCommand::e_OpenConnection;
    EXPECT_TRUE(cmd.command()== WinLIRCClient::LIRCCommand::e_OpenConnection);
    cmd.command() =  WinLIRCClient::LIRCCommand::e_ShutdownClient;
    EXPECT_TRUE(cmd.command()== WinLIRCClient::LIRCCommand::e_ShutdownClient);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClientLIRCCommand,host)
{
    WinLIRCClient::LIRCCommand cmd;
    cmd.host() = "localhost";
    EXPECT_TRUE(cmd.host()=="localhost");
    cmd.host() = "192.168.5.9";
    EXPECT_TRUE(cmd.host()=="192.168.5.9");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClientLIRCCommand,port)
{
    WinLIRCClient::LIRCCommand cmd;
    cmd.port() = 1234;
    EXPECT_TRUE(cmd.port()==1234);
    cmd.port() = 5678;
    EXPECT_TRUE(cmd.port()==5678);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsEmpty)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("");
    EXPECT_TRUE(parts.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsWhiteSpaceOnly)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("\t \t \t");
    EXPECT_TRUE(parts.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsOnePartCharacterNoWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("a");
    EXPECT_TRUE(parts.size()==1);
    EXPECT_TRUE(parts.at(0)=="a");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsOnePartCharacterBeginsWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("   a");
    EXPECT_TRUE(parts.size()==1);
    EXPECT_TRUE(parts.at(0)=="a");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsOnePartCharacterEndsWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("a   ");
    EXPECT_TRUE(parts.size()==1);
    EXPECT_TRUE(parts.at(0)=="a");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsOnePartCharacterBeginsAndEndsWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts(" a ");
    EXPECT_TRUE(parts.size()==1);
    EXPECT_TRUE(parts.at(0)=="a");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsOnePartNoWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("abc");
    EXPECT_TRUE(parts.size()==1);
    EXPECT_TRUE(parts.at(0)=="abc");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsOnePartBeginsWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("  abc");
    EXPECT_TRUE(parts.size()==1);
    EXPECT_TRUE(parts.at(0)=="abc");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsOnePartEndsWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("abc  ");
    EXPECT_TRUE(parts.size()==1);
    EXPECT_TRUE(parts.at(0)=="abc");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsOnePartBeginsAndEndsWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("\t\tabc\t\t");
    EXPECT_TRUE(parts.size()==1);
    EXPECT_TRUE(parts.at(0)=="abc");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsThreePartsNoWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("abc d e");
    EXPECT_TRUE(parts.size()==3);
    EXPECT_TRUE(parts.at(0)=="abc");
    EXPECT_TRUE(parts.at(1)=="d");
    EXPECT_TRUE(parts.at(2)=="e");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsThreePartsBeginsWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("  abc def ghi");
    EXPECT_TRUE(parts.size()==3);
    EXPECT_TRUE(parts.at(0)=="abc");
    EXPECT_TRUE(parts.at(1)=="def");
    EXPECT_TRUE(parts.at(2)=="ghi");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsThreePartsEndsWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts("abc def ghi  ");
    EXPECT_TRUE(parts.size()==3);
    EXPECT_TRUE(parts.at(0)=="abc");
    EXPECT_TRUE(parts.at(1)=="def");
    EXPECT_TRUE(parts.at(2)=="ghi");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,getStringPartsThreePartsBeginsAndEndsWhiteSpace)
{
    WinLIRCClientTest irClient;
    QStringList parts = irClient.testGetStringParts(" abc    def   ghi ");
    EXPECT_TRUE(parts.size()==3);
    EXPECT_TRUE(parts.at(0)=="abc");
    EXPECT_TRUE(parts.at(1)=="def");
    EXPECT_TRUE(parts.at(2)=="ghi");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeEmpty)
{
    WinLIRCClientTest irClient;
    EXPECT_FALSE(irClient.testIsValidCode(""));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeSingleNumericHex)
{
    WinLIRCClientTest irClient;
    EXPECT_TRUE(irClient.testIsValidCode("0"));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeSingleAlphaLowerHex)
{
    WinLIRCClientTest irClient;
    EXPECT_TRUE(irClient.testIsValidCode("a"));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeSingleAlphaUpperHex)
{
    WinLIRCClientTest irClient;
    EXPECT_TRUE(irClient.testIsValidCode("F"));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeSingleNonHex)
{
    WinLIRCClientTest irClient;
    EXPECT_FALSE(irClient.testIsValidCode("G"));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeUnmixedNumeric)
{
    WinLIRCClientTest irClient;
    EXPECT_TRUE(irClient.testIsValidCode("0123456789"));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeUnmixedAlphaLower)
{
    WinLIRCClientTest irClient;
    EXPECT_TRUE(irClient.testIsValidCode("abcdef"));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeUnmixedAlphaUpper)
{
    WinLIRCClientTest irClient;
    EXPECT_TRUE(irClient.testIsValidCode("ABCDEF"));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeMixed)
{
    WinLIRCClientTest irClient;
    EXPECT_TRUE(irClient.testIsValidCode("0000000000eab154"));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeNoAlphaNumeric)
{
    WinLIRCClientTest irClient;
    EXPECT_FALSE(irClient.testIsValidCode("STu!?"));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,isValidCodeMixedWithNonHex)
{
    WinLIRCClientTest irClient;
    EXPECT_FALSE(irClient.testIsValidCode("0000000000eab15G"));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,parseIRLineWhenOnly3Parts)
{
    int code,repeat;
    QString cmd,remote;
    WinLIRCClientTest irremote;
    EXPECT_FALSE(irremote.testParseIRLine("0000000000eab154 00 play",code,repeat,cmd,remote));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,parseIRLineWhen5Parts)
{
    int code,repeat;
    QString cmd,remote;
    WinLIRCClientTest irremote;
    EXPECT_FALSE(irremote.testParseIRLine("0000000000eab154 00 play myremote 111",code,repeat,cmd,remote));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,parseIRLine4PartsInvalidRemoteCode)
{
    int code,repeat;
    QString cmd,remote;
    WinLIRCClientTest irremote;
    EXPECT_FALSE(irremote.testParseIRLine("00000000G0eab154 00 play myremote",code,repeat,cmd,remote));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,parseIRLine4PartsInvalidCountCode)
{
    int code,repeat;
    QString cmd,remote;
    WinLIRCClientTest irremote;
    EXPECT_FALSE(irremote.testParseIRLine("0000000000eab154 HH play myremote",code,repeat,cmd,remote));
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,parseIRLine4PartsValidCodeLowerCase)
{
    int code,repeat;
    QString cmd,remote;
    WinLIRCClientTest irremote;
    EXPECT_TRUE(irremote.testParseIRLine("0000000000eab154 23 play myremote",code,repeat,cmd,remote));
    EXPECT_TRUE(code==0x00eab154);
    EXPECT_TRUE(repeat==0x23);
    EXPECT_TRUE(cmd=="play");
    EXPECT_TRUE(remote=="myremote");
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,parseIRLine4PartsValidCodeUpperCase)
{
    int code,repeat;
    QString cmd,remote;
    WinLIRCClientTest irremote;
    EXPECT_TRUE(irremote.testParseIRLine("0000000000EAB154 23 PLAY MYREMOTE",code,repeat,cmd,remote));
    EXPECT_TRUE(code==0x00eab154);
    EXPECT_TRUE(repeat==0x23);
    EXPECT_TRUE(cmd=="play");
    EXPECT_TRUE(remote=="myremote");
}

//-------------------------------------------------------------------------------------------

class WinLIRCClientProcessTest : public WinLIRCClientTest
{
    public:
        MOCK_METHOD0(getNextCommand,LIRCCommand());
        MOCK_CONST_METHOD0(getProcessState,int());
        MOCK_METHOD1(processNotConnected,void(const WinLIRCClient::LIRCCommand& cmd));
        MOCK_METHOD0(processConnected,void());
        MOCK_METHOD2(emitRemoteStatus,void(QString text,int statusCode));
        MOCK_METHOD0(close,void());
        MOCK_METHOD0(getSocket,network::socket_type());
};

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processNormalConnectedState)
{
    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_UnknownCommand;

    WinLIRCClientProcessTest remote;
    EXPECT_CALL(remote,getNextCommand()).Times(1).WillOnce(Return(cmd));
    EXPECT_CALL(remote,getProcessState()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(remote,processConnected()).Times(1);

    EXPECT_TRUE(remote.process());
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processNormalNotConnectedState)
{
    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_UnknownCommand;

    WinLIRCClientProcessTest remote;
    EXPECT_CALL(remote,getNextCommand()).Times(1).WillOnce(Return(cmd));
    EXPECT_CALL(remote,getProcessState()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(remote,processNotConnected(A<const WinLIRCClient::LIRCCommand&>())).Times(1);

    EXPECT_TRUE(remote.process());
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processShutdownCommandSocketStillOpen)
{
    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_ShutdownClient;

    network::socket_type socket = (network::socket_type)(10);
    QString statusStr = "LIRC Service is shutting down";

    WinLIRCClientProcessTest remote;
    EXPECT_CALL(remote,getNextCommand()).Times(1).WillOnce(Return(cmd));
    EXPECT_CALL(remote,emitRemoteStatus(statusStr,0)).Times(1);
    EXPECT_CALL(remote,getSocket()).Times(1).WillOnce(Return(socket));
    EXPECT_CALL(remote,close()).Times(1);

    EXPECT_FALSE(remote.process());
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processShutdownCommandSocketNowClosed)
{
    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_ShutdownClient;

    network::socket_type socket = network::c_invalidSocket;
    QString statusStr = "LIRC Service is shutting down";

    WinLIRCClientProcessTest remote;
    EXPECT_CALL(remote,getNextCommand()).Times(1).WillOnce(Return(cmd));
    EXPECT_CALL(remote,emitRemoteStatus(statusStr,0)).Times(1);
    EXPECT_CALL(remote,getSocket()).Times(1).WillOnce(Return(socket));

    EXPECT_FALSE(remote.process());
}

//-------------------------------------------------------------------------------------------

class WinLIRCClientProcessNotConnectedTest : public WinLIRCClientTest
{
    public:
        MOCK_METHOD2(open,bool(const QString& name,tint port));
        MOCK_METHOD0(close,void());
        MOCK_METHOD2(emitRemoteStatus,void(QString text,int statusCode));
        MOCK_METHOD0(getSocket,network::socket_type());
        MOCK_METHOD1(setProcessState,void(int nState));
        MOCK_METHOD0(getCurrentReferenceTime,common::TimeStamp());
        MOCK_METHOD0(getConnectReferenceTime,common::TimeStamp());
        MOCK_METHOD0(setConnectReferenceTime,void());
};

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processNotConnectedOpenSuccessGivenConnected)
{
    QString host = "www.blackomega.co.uk";

    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_OpenConnection;
    cmd.host() = host;
    cmd.port() = 1234;

    QString statusA = "Closing connection to LIRC server";
    QString statusB = "Trying to connect to www.blackomega.co.uk on port 1234";
    QString statusC = "Connected to LIRC server";
    network::socket_type socket = (network::socket_type)(10);

    WinLIRCClientProcessNotConnectedTest remote;
    EXPECT_CALL(remote,getSocket()).Times(1).WillOnce(Return(socket));
    EXPECT_CALL(remote,emitRemoteStatus(statusA,1)).Times(1);
    EXPECT_CALL(remote,close()).Times(1);
    EXPECT_CALL(remote,emitRemoteStatus(statusB,1)).Times(1);
    EXPECT_CALL(remote,open(host,1234)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,emitRemoteStatus(statusC,2)).Times(1);
    EXPECT_CALL(remote,setProcessState(1)).Times(1);

    remote.testProcessNotConnected(cmd);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processNotConnectedOpenSuccessGivenNotConnected)
{
    QString host = "www.blackomega.co.uk";

    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_OpenConnection;
    cmd.host() = host;
    cmd.port() = 1234;

    QString statusA = "Closing connection to LIRC server";
    QString statusB = "Trying to connect to www.blackomega.co.uk on port 1234";
    QString statusC = "Connected to LIRC server";
    network::socket_type socket = network::c_invalidSocket;

    WinLIRCClientProcessNotConnectedTest remote;
    EXPECT_CALL(remote,getSocket()).Times(1).WillOnce(Return(socket));
    EXPECT_CALL(remote,emitRemoteStatus(statusB,1)).Times(1);
    EXPECT_CALL(remote,open(host,1234)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,emitRemoteStatus(statusC,2)).Times(1);
    EXPECT_CALL(remote,setProcessState(1)).Times(1);

    remote.testProcessNotConnected(cmd);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processNotConnectedOpenFailureGivenConnected)
{
    QString host = "www.blackomega.co.uk";

    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_OpenConnection;
    cmd.host() = host;
    cmd.port() = 1234;

    QString statusA = "Closing connection to LIRC server";
    QString statusB = "Trying to connect to www.blackomega.co.uk on port 1234";
    QString statusC = "Failed to connect to www.blackomega.co.uk on port 1234";
    network::socket_type socket = (network::socket_type)(10);

    WinLIRCClientProcessNotConnectedTest remote;
    EXPECT_CALL(remote,getSocket()).Times(1).WillOnce(Return(socket));
    EXPECT_CALL(remote,emitRemoteStatus(statusA,1)).Times(1);
    EXPECT_CALL(remote,close()).Times(2);
    EXPECT_CALL(remote,emitRemoteStatus(statusB,1)).Times(1);
    EXPECT_CALL(remote,open(host,1234)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(remote,emitRemoteStatus(statusC,0)).Times(1);
    EXPECT_CALL(remote,setConnectReferenceTime()).Times(1);

    remote.testProcessNotConnected(cmd);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processNotConnectedOpenFailureGivenNotConnected)
{
    QString host = "www.blackomega.co.uk";

    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_OpenConnection;
    cmd.host() = host;
    cmd.port() = 1234;

    QString statusA = "Closing connection to LIRC server";
    QString statusB = "Trying to connect to www.blackomega.co.uk on port 1234";
    QString statusC = "Failed to connect to www.blackomega.co.uk on port 1234";
    network::socket_type socket = network::c_invalidSocket;

    WinLIRCClientProcessNotConnectedTest remote;
    EXPECT_CALL(remote,getSocket()).Times(1).WillOnce(Return(socket));
    EXPECT_CALL(remote,emitRemoteStatus(statusB,1)).Times(1);
    EXPECT_CALL(remote,open(host,1234)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(remote,emitRemoteStatus(statusC,0)).Times(1);
    EXPECT_CALL(remote,setConnectReferenceTime()).Times(1);
    EXPECT_CALL(remote,close()).Times(1);

    remote.testProcessNotConnected(cmd);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processNotConnectedReconnectOpenSuccess)
{
    QString host = "www.blackomega.co.uk";

    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_UnknownCommand;

    WinLIRCSettingsMock settings;
    EXPECT_CALL(settings,host()).Times(1).WillOnce(Return(host));
    EXPECT_CALL(settings,port()).Times(1).WillOnce(Return(1234));

    QString statusA = "Connected to LIRC server";

    network::socket_type socket = network::c_invalidSocket;
    common::TimeStamp connectT(10.0);
    common::TimeStamp referenceT(75.0);

    WinLIRCClientProcessNotConnectedTest remote;
    EXPECT_CALL(remote,getSocket()).Times(1).WillOnce(Return(socket));
    EXPECT_CALL(remote,getConnectReferenceTime()).Times(1).WillOnce(Return(connectT));
    EXPECT_CALL(remote,getCurrentReferenceTime()).Times(1).WillOnce(Return(referenceT));
    EXPECT_CALL(remote,open(host,1234)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,emitRemoteStatus(statusA,2)).Times(1);
    EXPECT_CALL(remote,setProcessState(1)).Times(1);

    remote.testProcessNotConnected(cmd);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processNotConnectedReconnectOpenSuccessFailure)
{
    QString host = "www.blackomega.co.uk";

    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_UnknownCommand;

    WinLIRCSettingsMock settings;
    EXPECT_CALL(settings,host()).Times(1).WillOnce(Return(host));
    EXPECT_CALL(settings,port()).Times(1).WillOnce(Return(1234));

    QString statusA = "No connection to LIRC server";

    network::socket_type socket = network::c_invalidSocket;
    common::TimeStamp connectT(10.0);
    common::TimeStamp referenceT(75.0);

    WinLIRCClientProcessNotConnectedTest remote;
    EXPECT_CALL(remote,getSocket()).Times(1).WillOnce(Return(socket));
    EXPECT_CALL(remote,getConnectReferenceTime()).Times(1).WillOnce(Return(connectT));
    EXPECT_CALL(remote,getCurrentReferenceTime()).Times(1).WillOnce(Return(referenceT));
    EXPECT_CALL(remote,open(host,1234)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(remote,emitRemoteStatus(statusA,0)).Times(1);
    EXPECT_CALL(remote,setConnectReferenceTime()).Times(1);

    remote.testProcessNotConnected(cmd);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processNotConnectedNoConnectGivenConnected)
{
    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_UnknownCommand;

    QString statusA = "Closing connection to LIRC server";
    QString statusB = "No connection to LIRC server";

    network::socket_type socket = (network::socket_type)(10);
    common::TimeStamp connectT(10.0);
    common::TimeStamp referenceT(65.0);

    WinLIRCClientProcessNotConnectedTest remote;
    EXPECT_CALL(remote,getSocket()).Times(1).WillOnce(Return(socket));
    EXPECT_CALL(remote,emitRemoteStatus(statusA,1)).Times(1);
    EXPECT_CALL(remote,close()).Times(1);
    EXPECT_CALL(remote,getConnectReferenceTime()).Times(1).WillOnce(Return(connectT));
    EXPECT_CALL(remote,getCurrentReferenceTime()).Times(1).WillOnce(Return(referenceT));
    EXPECT_CALL(remote,emitRemoteStatus(statusB,0)).Times(1);

    remote.testProcessNotConnected(cmd);
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processNotConnectedNoConnectGivenNotConnected)
{
    WinLIRCClient::LIRCCommand cmd;
    cmd.command() = WinLIRCClient::LIRCCommand::e_UnknownCommand;

    QString statusA = "No connection to LIRC server";

    network::socket_type socket = network::c_invalidSocket;
    common::TimeStamp connectT(10.0);
    common::TimeStamp referenceT(65.0);

    WinLIRCClientProcessNotConnectedTest remote;
    EXPECT_CALL(remote,getSocket()).Times(1).WillOnce(Return(socket));
    EXPECT_CALL(remote,getConnectReferenceTime()).Times(1).WillOnce(Return(connectT));
    EXPECT_CALL(remote,getCurrentReferenceTime()).Times(1).WillOnce(Return(referenceT));
    EXPECT_CALL(remote,emitRemoteStatus(statusA,0)).Times(1);

    remote.testProcessNotConnected(cmd);
}

//-------------------------------------------------------------------------------------------

class WinLIRCClientProcessConnectedTest : public WinLIRCClientTest
{
    public:
        MOCK_METHOD0(connectSocketProcess,bool());
        MOCK_METHOD0(canGetNextLine,bool());
        MOCK_METHOD1(getNextLine,bool(QString& line));
        MOCK_METHOD1(processLine,void(const QString& line));
        MOCK_METHOD0(close,void());
        MOCK_METHOD1(setProcessState,void(int nState));
        MOCK_METHOD2(emitRemoteStatus,void(QString text,int statusCode));
        MOCK_METHOD0(setConnectReferenceTime,void());
};

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processConnectedGivenErrorInConnection)
{
    QString status = "Connection to LIRC server has been closed";

    WinLIRCClientProcessConnectedTest remote;
    EXPECT_CALL(remote,connectSocketProcess()).Times(1).WillOnce(Return(false));
    EXPECT_CALL(remote,close()).Times(1);
    EXPECT_CALL(remote,setProcessState(0)).Times(1);
    EXPECT_CALL(remote,emitRemoteStatus(status,0)).Times(1);
    EXPECT_CALL(remote,setConnectReferenceTime()).Times(1);

    remote.testProcessConnected();
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processConnectedGivenNoLines)
{
    WinLIRCClientProcessConnectedTest remote;
    EXPECT_CALL(remote,connectSocketProcess()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,canGetNextLine()).Times(1).WillOnce(Return(false));

    remote.testProcessConnected();
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processConnectedGivenOneLine)
{
    QString lineA = "0000001234567 00 play remote";

    WinLIRCClientProcessConnectedTest remote;
    EXPECT_CALL(remote,connectSocketProcess()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,canGetNextLine()).Times(2).WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_CALL(remote,getNextLine(A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<0>(lineA),Return(true)));
    EXPECT_CALL(remote,processLine(lineA)).Times(1);

    remote.testProcessConnected();
}

//-------------------------------------------------------------------------------------------

TEST(WinLIRCClient,processConnectedGivenThreeLines)
{
    QString lineA = "0000001234567 00 play remote";
    QString lineB = "0000001234567 00 next remote";
    QString lineC = "0000001234567 00 prev remote";

    WinLIRCClientProcessConnectedTest remote;
    EXPECT_CALL(remote,connectSocketProcess()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(remote,canGetNextLine()).Times(4).WillOnce(Return(true)).WillOnce(Return(true))
        .WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_CALL(remote,getNextLine(A<QString&>())).Times(3)
        .WillOnce(DoAll(SetArgReferee<0>(lineA),Return(true)))
        .WillOnce(DoAll(SetArgReferee<0>(lineB),Return(true)))
        .WillOnce(DoAll(SetArgReferee<0>(lineC),Return(true)));
    EXPECT_CALL(remote,processLine(lineA)).Times(1);
    EXPECT_CALL(remote,processLine(lineB)).Times(1);
    EXPECT_CALL(remote,processLine(lineC)).Times(1);

    remote.testProcessConnected();
}

//-------------------------------------------------------------------------------------------
