#include "playerapp/playerios/inc/PlayerIOSTrackDBManager.h"
#include "common/inc/DiskIF.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlayerIOSTrackDBManager *PlayerIOSTrackDBManager::m_instance = 0;

//-------------------------------------------------------------------------------------------

PlayerIOSTrackDBManager::PlayerIOSTrackDBManager(QObject *parent) : QObject(parent)
{
    m_logFlushTimer = new QTimer(this);
    m_logFlushTimer->setInterval(200);
    m_logFlushTimer->setSingleShot(false);
    QObject::connect(m_logFlushTimer, SIGNAL(timeout()), this, SLOT(onFlushLog()));
    m_logFlushTimer->start();
}

//-------------------------------------------------------------------------------------------

PlayerIOSTrackDBManager::~PlayerIOSTrackDBManager()
{
    m_instance = 0;
}

//-------------------------------------------------------------------------------------------

PlayerIOSTrackDBManager *PlayerIOSTrackDBManager::instance(QObject *parent)
{
    if(m_instance == 0)
    {
        m_instance = new PlayerIOSTrackDBManager(parent);
        if(!m_instance->open())
        {
            delete m_instance;
            m_instance = 0;
        }
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void PlayerIOSTrackDBManager::release()
{
    if(m_instance != 0)
    {
        delete m_instance;
        m_instance = 0;
    }
}

//-------------------------------------------------------------------------------------------

void PlayerIOSTrackDBManager::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "PlayerIOSTrackDBManager::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

QString PlayerIOSTrackDBManager::trackDBPath() const
{
    QString path = common::DiskOps::mergeName(PlayerIOSUtils::appDataDirectory(), "db");
    common::DiskOps::path(path, true);
    path = common::DiskOps::mergeName(path, "track.db");
    return path;
}

//-------------------------------------------------------------------------------------------

bool PlayerIOSTrackDBManager::open()
{
    track::db::TrackDB *trackDB;
    bool res = false;
    
    //common::DiskOps::remove(trackDBPath());

    trackDB = track::db::TrackDB::instance(trackDBPath());
    if(trackDB != 0)
    {
        if(defineMountpoint())
        {
            res = true;
        }
        else
        {
            printError("open", "Failed to define document mount point");
        }
    }
    else
    {
        printError("open", "Failed to open track database");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PlayerIOSTrackDBManager::close()
{
    track::db::TrackDB *trackDB = track::db::TrackDB::instance();
    if(trackDB != 0)
    {
        delete trackDB;
    }
}

//-------------------------------------------------------------------------------------------

void PlayerIOSTrackDBManager::onFlushLog()
{
    fflush(stdout);
}

//-------------------------------------------------------------------------------------------

bool PlayerIOSTrackDBManager::defineMountpoint()
{
    bool res = false;
    QString docPath = PlayerIOSUtils::appDataDirectory();
    
    if(track::db::TrackDB::instance()->mountPoints()->updateAppMountPath(docPath))
    {
        res = true;
    }
    else
    {
        QString err = QString("Failed to define mount point at '%1'").arg(docPath);
        printError("defineMountpoint", err.toUtf8().constData());
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PlayerIOSTrackDBManager::addUploadedFile(const QString& fileName)
{
    bool res = false;
    QSharedPointer<track::info::Info> dbInfo;
    
    dbInfo = track::db::DBInfo::readInfo(fileName);
    if(!dbInfo.isNull())
    {
        emit newtrack(fileName);
        res = true;
    }
    else
    {
        QString err = QString("Failed to read information for '%1'").arg(fileName);
        printError("addUploadedFile", err.toUtf8().constData());
    }
    if(!res)
    {
        common::DiskOps::deleteFile(fileName);
    }
}

//-------------------------------------------------------------------------------------------

void PlayerIOSTrackDBManager::onDeleteFile(const QString& fileName)
{
    track::db::TrackDB *trackDB = track::db::TrackDB::instance();
    
    emit removetrack(fileName);
    trackDB->erase(fileName);
}

//-------------------------------------------------------------------------------------------

void PlayerIOSTrackDBManager::renameDBFile(const QString& fileDBName)
{
    QFile dbFile(fileDBName);
    if(dbFile.exists())
    {
        tint i, noRetry;
    
        for(i = 0, noRetry = 0; noRetry < 5; i++)
        {
            common::BString nExt = common::BString::Int(i + 1, (i < 3) ? 3 : i);
            QString rFilename = fileDBName + ".";
            rFilename += static_cast<const char *>(nExt);
            if(!common::DiskOps::exist(rFilename))
            {
                if(dbFile.rename(rFilename))
                {
                    break;
                }
                else
                {
                    QString err = QString("Failed to rename '%1' to '%2'. ").arg(fileDBName).arg(rFilename);
                    noRetry++;
                    err += (noRetry < 5) ? "Retrying." : "Removing DB file.";
                    printError("renameDBFile", err.toUtf8().constData());
                    
                }
            }
        }
        if(noRetry >= 5)
        {
            // if failed to rename then remove
            common::DiskOps::deleteFile(fileDBName);
        }
    }
}

//-------------------------------------------------------------------------------------------

void PlayerIOSTrackDBManager::rebuildDatabase()
{
    QString fileDBName = trackDBPath();
    track::db::TrackDB *trackDB = track::db::TrackDB::instance();
    bool res = false;
    
    // Close existing database
    trackDB->close();
    // Rename the old database file for diagnostics.
    renameDBFile(fileDBName);
    // Open the database will cause a new blank database to be created.
    if(trackDB->open(fileDBName))
    {
        if(defineMountpoint())
        {
            buildDBForDirectory(PlayerIOSUtils::musicDirectory());
            res = true;
        }
    }
    if(!res)
    {
        QString err = QString("FATAL ERROR. Failed to created new track database at '%1'").arg(fileDBName);
        printError("rebuildDatabase", err.toUtf8().constData());
        exit(-1);
    }
}

//-------------------------------------------------------------------------------------------

void PlayerIOSTrackDBManager::buildDBForDirectory(const QString& dirName)
{
    QSharedPointer<common::DiskIF> pDisk = common::DiskIF::instance();
    
    if(pDisk->isDirectory(dirName))
    {
        common::DiskIF::DirHandle h = pDisk->openDirectory(dirName);
        if(h != common::DiskIF::invalidDirectory())
        {
            QString name;
            QStringList fileList, dirList;
            QStringList::iterator ppI;
            
            while(name = pDisk->nextDirectoryEntry(h), !name.isEmpty())
            {
                QString fullName = common::DiskOps::mergeName(dirName, name);
                if(pDisk->isFile(fullName))
                {
                    fileList.append(fullName);
                }
                else if(pDisk->isDirectory(fullName))
                {
                    dirList.append(fullName);
                }
            }
            pDisk->closeDirectory(h);
            
            for(ppI = dirList.begin(); ppI != dirList.end(); ppI++)
            {
                buildDBForDirectory(*ppI);
            }
            for(ppI = fileList.begin(); ppI != fileList.end(); ppI++)
            {
                name = *ppI;
                if(track::info::Info::isSupported(name))
                {
                    QSharedPointer<track::info::Info> dbInfo = track::db::DBInfo::readInfo(name);
                    if(dbInfo.isNull())
                    {
                        QString err = QString("Failed to read expected info from '%1'").arg(name);
                        printError("buildDBForDirectory", err.toUtf8().constData());
                    }
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
