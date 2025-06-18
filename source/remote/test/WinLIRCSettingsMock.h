//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_TEST_WINLIRCSETTINGSMOCK_H
#define __OMEGA_REMOTE_TEST_WINLIRCSETTINGSMOCK_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/WinLIRCSettings.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class WinLIRCSettingsMock : public WinLIRCSettings
{
    public:
        WinLIRCSettingsMock();
        virtual ~WinLIRCSettingsMock();
        
        MOCK_METHOD0(host,QString());
        MOCK_METHOD1(setHost,void(const QString& name));
        MOCK_METHOD0(port,int());
        MOCK_METHOD1(setPort,void(int portNum));
        MOCK_METHOD0(programCommands,QString());
        MOCK_METHOD1(setProgramCommands,void(const QString& cmdXml));
        MOCK_METHOD0(repeatTime,double());
        MOCK_METHOD1(setRepeatTime,void(double rTime));
        MOCK_METHOD0(delayTime,double());
        MOCK_METHOD1(setDelayTime,void(double dTime));
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
