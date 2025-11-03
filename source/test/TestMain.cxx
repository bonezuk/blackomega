#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/test/UPnPProviderTestEnviroment.h"
#include "track/model/test/TrackDBTestEnviroment.h"

#include "common/inc/CommonFunctions.h"
#include "engine/inc/Codec.h"
#include "engine/blackomega/inc/MPCodec.h"
#include "engine/silveromega/inc/SilverCodec.h"
#include "engine/whiteomega/inc/WhiteCodec.h"

#if defined(Q_OS_MAC)
#include "common/inc/Bundle.h"
#endif

#include "common/inc/SBService.h"

#include <QApplication>
#include <QSettings>

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
{
    omega::common::test::UPnPProviderTestEnviroment::instance();
    omega::track::model::TrackDBTestEnviroment *pTrackDBTest = omega::track::model::TrackDBTestEnviroment::instance();
    ASSERT_TRUE(pTrackDBTest->load());
}

//-------------------------------------------------------------------------------------------

void GlobalEnviroment::TearDown()
{
    omega::track::model::TrackDBTestEnviroment *pTrackDBTest = omega::track::model::TrackDBTestEnviroment::instance();
    pTrackDBTest->unload();
    delete pTrackDBTest;
    omega::common::test::UPnPProviderTestEnviroment *pUPnPProviderTest = omega::common::test::UPnPProviderTestEnviroment::instance();
    delete pUPnPProviderTest;
}

//-------------------------------------------------------------------------------------------

/*
TEST(TrackDBTestEnviroment,buildTestDB)
{
    omega::track::model::TrackDBTestEnviroment *pTrackDBTest = omega::track::model::TrackDBTestEnviroment::instance();
    pTrackDBTest->buildTestDB("/Volumes/Iomega_HDD/TestMusicDB/track.db","/Volumes/Iomega_HDD/TestMusicDB");
    delete pTrackDBTest;
}
*/

//-------------------------------------------------------------------------------------------

bool setSpawnICSProcess(int argc, char **argv)
{
    bool spawn = true;
    
    for(int i = 1; i < argc && spawn; i++)
    {
        QString a = QString::fromUtf8(argv[i]);
        if(a.toLower().trimmed() == "--no-spawn")
        {
            spawn = false;
        }
    }
    return spawn;
}

//-------------------------------------------------------------------------------------------

int main(int argc,char **argv)
{
#if defined(Q_OS_MAC)
    QString pluginDir;
#if defined(OMEGA_MAC_STORE)
    omega::common::Log::g_Log.print("Store Bundle Plugin\n");
    pluginDir = omega::common::Bundle::pluginDirectory();
    omega::common::Log::g_Log.print("%s\n",pluginDir.toLatin1().constData());
#else
    QFileInfo appFile(argv[0]);
    QDir d = appFile.absolutePath();
    omega::common::Log::g_Log.print("%s\n",d.absolutePath().toLatin1().constData());
#if defined(OMEGA_BUNDLE_TEST)
    omega::common::Log::g_Log.print("Bundle Plugin\n");
    d.cdUp();
    omega::common::Log::g_Log.print("%s\n",d.absolutePath().toLatin1().constData());
#endif
    d.cd("Plugins");
    omega::common::Log::g_Log.print("%s\n",d.absolutePath().toLatin1().constData());
    pluginDir = d.absolutePath();
#endif
    QApplication::setLibraryPaths(QStringList(pluginDir));
#elif defined(Q_OS_WIN)
    QFileInfo appFile(argv[0]);
    QDir d = appFile.absolutePath();
    QString pluginDir;
    d.cdUp();
    d.cd("plugins");
    pluginDir = d.absolutePath();
    QApplication::setLibraryPaths(QStringList(pluginDir));
#endif

#if !defined(Q_OS_MAC)
    omega::common::loadSharedLibrary("blueomega");
    omega::common::loadSharedLibrary("silveromega");
    omega::common::loadSharedLibrary("blackomega");
    omega::common::loadSharedLibrary("whiteomega");
    omega::common::loadSharedLibrary("redomega");
    omega::common::loadSharedLibrary("greenomega");
    omega::common::loadSharedLibrary("rtp_silveromega");
    omega::common::loadSharedLibrary("widget");
    omega::common::loadSharedLibrary("violetomega");
    omega::common::loadSharedLibrary("wavpackomega");
    omega::common::loadSharedLibrary("toneomega");
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
    
    omega::track::model::TrackDBTestEnviroment *pTrackDBTest = omega::track::model::TrackDBTestEnviroment::instance();
    pTrackDBTest->setExecPath(QString::fromUtf8(argv[0]));
    pTrackDBTest->setSpawnICSProcess(setSpawnICSProcess(argc, argv));
    
    return RUN_ALL_TESTS();
}
