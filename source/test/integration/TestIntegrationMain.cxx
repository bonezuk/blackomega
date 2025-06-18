#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QtCore/QDir>
#include <QApplication>
#include <QSettings>

#include "common/inc/CommonTypes.h"
#include "common/inc/SBService.h"
#include "engine/inc/Codec.h"
#include "engine/blackomega/inc/MPCodec.h"
#include "engine/silveromega/inc/SilverCodec.h"
#include "engine/whiteomega/inc/WhiteCodec.h"
#include "engine/violetomega/inc/VioletCodec.h"

//-------------------------------------------------------------------------------------------
#if defined(_WIN32)
testing::internal::Mutex testing::internal::g_gmock_mutex;
testing::internal::Mutex testing::internal::g_linked_ptr_mutex;
testing::internal::ThreadLocal<class testing::Sequence *> testing::internal::g_gmock_implicit_sequence;
#endif
//-------------------------------------------------------------------------------------------

class GlobalEnviroment : public ::testing::Environment
{
    public:
        virtual ~GlobalEnviroment();
        
        virtual void SetUp();
        virtual void TearDown();
};

//-------------------------------------------------------------------------------------------

GlobalEnviroment::~GlobalEnviroment()
{}

//-------------------------------------------------------------------------------------------

void GlobalEnviroment::SetUp()
{}

//-------------------------------------------------------------------------------------------

void GlobalEnviroment::TearDown()
{}

//-------------------------------------------------------------------------------------------

int main(int argc,char **argv)
{
    ::omega::network::Resource::instance();

#if defined(Q_OS_MAC)
    QFileInfo appFile(argv[0]);
    QDir d = appFile.absolutePath();
    QString pluginDir;
#if defined(BUNDLE_PLUGINS)
    d.cdUp();
#endif
    d.cd("Plugins");
    pluginDir = d.absolutePath();
    QApplication::setLibraryPaths(QStringList(pluginDir));
#else
    QFileInfo appFile(argv[0]);
    QDir d = appFile.absolutePath();
    QString pluginDir;
    d.cdUp();
    d.cd("plugins");
    pluginDir = d.absolutePath();
    QApplication::setLibraryPaths(QStringList(pluginDir));
#endif

#if defined(OMEGA_WIN32)
    HMODULE blueOmegaDLL = ::LoadLibraryA("blueomega.dll");
    HMODULE silverOmegaDLL = ::LoadLibraryA("silveromega.dll");
    HMODULE blackOmegaDLL = ::LoadLibraryA("blackomega.dll");
    HMODULE whiteOmegaDLL = ::LoadLibraryA("whiteomega.dll");
    HMODULE redOmegaDLL = ::LoadLibraryA("redomega.dll");
    HMODULE greenOmegaDLL = ::LoadLibraryA("greenomega.dll");
    HMODULE RTPsilverOmegaDLL = ::LoadLibraryA("rtp_silveromega.dll");
    HMODULE widgetDLL = ::LoadLibraryA("widget.dll");
    HMODULE violetOmegaDLL = ::LoadLibraryA("violetomega.dll");
    HMODULE cyanOmegaDLL = ::LoadLibraryA("cyanomega.dll");
    HMODULE monkeyOmegaDLL = ::LoadLibraryA("monkeyomega.dll");

    CoInitialize(NULL);
#endif

#if defined(OMEGA_MAC_STORE)
    QString settingPath = omega::common::SBService::applicationDataDirectory();
    QSettings::setPath(QSettings::IniFormat,QSettings::UserScope,settingPath);
    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,settingPath);
        
    QCoreApplication::setOrganizationName("Stuart MacLean");
    QCoreApplication::setOrganizationDomain("www.blackomega.co.uk");
    QCoreApplication::setApplicationName("Black Omega Test");
#else
    QCoreApplication::setOrganizationName("Tiger-Eye-Test");
    QCoreApplication::setOrganizationDomain("www.blackomega.co.uk");
    QCoreApplication::setApplicationName("BlackOmega2Test");
#endif

    omega::engine::CodecInitialize::start();
    omega::engine::blackomega::MPCodecInitialize::start();
    omega::engine::silveromega::SilverCodecInitialize::start();
    omega::engine::whiteomega::WhiteCodecInitialize::start();

    ::testing::InitGoogleTest(&argc,argv);
    ::testing::InitGoogleMock(&argc,argv);
    ::testing::AddGlobalTestEnvironment(new GlobalEnviroment);
    return RUN_ALL_TESTS();
}
