#include "remote/inc/AppleRemoteService.h"
#include "remote/test/integration/AppleRemoteServiceIntegration.h"

#import <Foundation/NSAutoreleasePool.h>
#import <AppKit/NSApplication.h>

using namespace omega;
using namespace omega::remote;

using namespace testing;

//-------------------------------------------------------------------------------------------

AppleRemoteServiceApplication::AppleRemoteServiceApplication(int testType,int& argc,char **argv) : QApplication(argc,argv),
    m_testType(testType),
    m_count(0)
{
    QTimer::singleShot(100,this,SLOT(onInit()));
}

//-------------------------------------------------------------------------------------------

AppleRemoteServiceApplication::~AppleRemoteServiceApplication()
{}

//-------------------------------------------------------------------------------------------

void AppleRemoteServiceApplication::onInit()
{
    if(m_testType==0)
    {
        AppleRemoteService::instance(0);
    }
    else if(m_testType==1)
    {
        QTimer::singleShot(10,this,SLOT(onRepeatOpen()));
    }
    else if(m_testType==2)
    {
        common::Log::g_Log.print("Opening Apple IR Service\n");
        AppleRemoteService::instance(0);
        m_count = 41;
        onCountToClose();
    }
}

//-------------------------------------------------------------------------------------------

void AppleRemoteServiceApplication::onRepeatOpen()
{
    common::Log::g_Log.print("Opening Apple IR Service\n");
    AppleRemoteService::instance(0);
    common::Log::g_Log.print("Service running\n");
    QTimer::singleShot(200,this,SLOT(onRepeatClose()));
}

//-------------------------------------------------------------------------------------------

void AppleRemoteServiceApplication::onRepeatClose()
{
    common::Log::g_Log.print("Closing Apple IR Service\n");
    AppleRemoteService::release();
    common::Log::g_Log.print("Service shutdown\n");
    m_count++;
    if(m_count < 100)
    {
        QTimer::singleShot(200,this,SLOT(onRepeatOpen()));
    }
    else
    {
        quit();
    }
}

//-------------------------------------------------------------------------------------------

void AppleRemoteServiceApplication::onCountToClose()
{
    m_count--;
    if(m_count > 0)
    {
        common::Log::g_Log.print("%ds\n",m_count);
        QTimer::singleShot(1000,this,SLOT(onCountToClose()));
    }
    else
    {
        common::Log::g_Log.print("Closing Apple IR Service\n");
        AppleRemoteService::release();
        quit();
    }
}

//-------------------------------------------------------------------------------------------

TEST(AppleRemoteService,Integration)
{
    int argc = 0;
    AppleRemoteServiceApplication remoteApp(0,argc,0);
    remoteApp.exec();
}

//-------------------------------------------------------------------------------------------

TEST(AppleRemoteService,findRemoteHardware)
{
    if(AppleRemoteService::isRemoteAvailable())
    {
        common::Log::g_Log.print("Found Apple IR Remote reciever\n");
    }
    else
    {
        common::Log::g_Log.print("No Apple IR Remote reciever\n");
    }
}

//-------------------------------------------------------------------------------------------

TEST(AppleRemoteService,openAndClose)
{
    int argc = 0;
    AppleRemoteServiceApplication remoteApp(1,argc,0);
    remoteApp.exec();
}

//-------------------------------------------------------------------------------------------

TEST(AppleRemoteService,runForPeriod)
{
    int argc = 0;
    AppleRemoteServiceApplication remoteApp(2,argc,0);
    remoteApp.exec();
}

//-------------------------------------------------------------------------------------------
