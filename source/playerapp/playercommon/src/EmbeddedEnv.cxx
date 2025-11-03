#include "network/inc/Resource.h"

#include <QFileInfo>
#include <QDir>

#include "common/inc/SBService.h"
#include "common/inc/CommonFunctions.h"
#include "common/inc/DiskOps.h"
#include "engine/blackomega/inc/MPCodec.h"
#include "engine/silveromega/inc/SilverCodec.h"
#include "engine/whiteomega/inc/WhiteCodec.h"
#include "playerapp/playercommon/inc/EmbeddedEnv.h"

#include <QCoreApplication>

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

void setPluginLocation(const char *appPath)
{
#if defined(Q_OS_MAC)
    QFileInfo appFile(appPath);
    QDir d = appFile.absolutePath();
    QString pluginDir;
#if defined(BUNDLE_PLUGINS)
    d.cdUp();
#endif
    d.cd("Plugins");
    pluginDir = d.absolutePath();
    QCoreApplication::addLibraryPath(pluginDir);
#elif defined(Q_OS_WIN)
    QFileInfo appFile(appPath);
    QDir d = appFile.absolutePath();
    QString pluginDir;
    d.cdUp();
    d.cd("plugins");
    pluginDir = d.absolutePath();
    QCoreApplication::setLibraryPaths(QStringList(pluginDir));
#endif
}

//-------------------------------------------------------------------------------------------

void setupPlatform()
{
    common::loadSharedLibrary("blackomega");
    common::loadSharedLibrary("blueomega");
    common::loadSharedLibrary("cyanomega");
    common::loadSharedLibrary("greenomega");
    common::loadSharedLibrary("redomega");
    common::loadSharedLibrary("silveromega");
    common::loadSharedLibrary("toneomega");
    common::loadSharedLibrary("violetomega");
    common::loadSharedLibrary("wavpackomega");
    common::loadSharedLibrary("whiteomega");
    common::loadSharedLibrary("widget");
#if defined(OMEGA_WIN32)
    CoInitialize(NULL);
#endif
}

//-------------------------------------------------------------------------------------------

void setupSettingsPath()
{
    QCoreApplication::setOrganizationName("Tiger-Eye");
    QCoreApplication::setOrganizationDomain("www.blackomega.co.uk");
    QCoreApplication::setApplicationName("BlackOmega2");
}

//-------------------------------------------------------------------------------------------

void setupEnviroment(const char *appPath)
{
    network::Resource::instance();
    setPluginLocation(appPath);
    setupPlatform();
    setupSettingsPath();
}

//-------------------------------------------------------------------------------------------

QString userApplicationDataDirectory()
{
    return common::SBService::applicationDataDirectory();
}

//-------------------------------------------------------------------------------------------

void initCodecs()
{
    engine::CodecInitialize::start();
    engine::blackomega::MPCodecInitialize::start();
    engine::silveromega::SilverCodecInitialize::start();
    engine::whiteomega::WhiteCodecInitialize::start();
}

//-------------------------------------------------------------------------------------------

void releaseCodecs()
{
    engine::whiteomega::WhiteCodecInitialize::end();
    engine::silveromega::SilverCodecInitialize::end();
    engine::blackomega::MPCodecInitialize::end();
    engine::CodecInitialize::end();
}

//-------------------------------------------------------------------------------------------

QString pathToUNIXSocket(const QString& serviceName)
{
    QString fileName = serviceName + ".usock";
    QString userDir = userApplicationDataDirectory();
    common::Log::g_Log << "UserDataDir - " << userDir << ", socket - " << fileName << common::c_endl;
    QString path = common::DiskOps::mergeName(userApplicationDataDirectory(), fileName);
    return path;
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
