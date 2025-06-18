#include "playerapp/playerios/inc/PlayerIOSUtils.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlayerIOSUtils::PlayerIOSUtils()
{}

//-------------------------------------------------------------------------------------------

void PlayerIOSUtils::printError(const tchar *strR, const tchar *strE)
{
    common::Log::g_Log << "PlayerIOSUtils::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

QString PlayerIOSUtils::appDataDirectory()
{
    QString appDir;
    
#ifdef OMEGA_IOS
    appDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#else
    appDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif
    return appDir;
}

//-------------------------------------------------------------------------------------------

QString PlayerIOSUtils::musicDirectory()
{
    QString mDir = common::DiskOps::mergeName(appDataDirectory(), "music");
    if(!common::DiskOps::path(mDir, true))
    {
        QString err = QString("Failed to create root music directory '%1'").arg(mDir);
        printError("musicDirectory", err.toUtf8().constData());
        mDir = "";
    }
    return mDir;
}

//-------------------------------------------------------------------------------------------

QString PlayerIOSUtils::logDirectory()
{
    QString mDir = common::DiskOps::mergeName(appDataDirectory(), "log");
    if(!common::DiskOps::path(mDir, true))
    {
        QString err = QString("Failed to create root log directory '%1'").arg(mDir);
        printError("logDirectory", err.toUtf8().constData());
        mDir = "";
    }
    return mDir;
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
