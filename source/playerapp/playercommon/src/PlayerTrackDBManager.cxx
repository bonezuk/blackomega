#include "playerapp/playercommon/inc/PlayerTrackDBManager.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlayerTrackDBManager::PlayerTrackDBManager(QObject *parent) : QObject(parent)
{}

//-------------------------------------------------------------------------------------------

PlayerTrackDBManager::~PlayerTrackDBManager()
{}

//-------------------------------------------------------------------------------------------

void PlayerTrackDBManager::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "PlayerTrackDBManager::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool PlayerTrackDBManager::isProgressCancelled(common::AbstractProgressInterface *progress)
{
    bool cancel;
    
    if(progress != 0)
    {
        cancel = progress->isCancelled();
    }
    else
    {
        cancel = false;
    }
    return cancel;
}

//-------------------------------------------------------------------------------------------

void PlayerTrackDBManager::scanDirectoryRecursive(const QString& path, QStringList& fileList, int level, common::AbstractProgressInterface *progress, int mountNo, int mountTotal)
{
    QString name;
    QStringList dirList;
    QSharedPointer<common::DiskIF> pDisk = common::DiskIF::instance();
    
    if(pDisk->isDirectory(path))
    {
        common::DiskIF::DirHandle h = pDisk->openDirectory(path);
        if(h != common::DiskIF::invalidDirectory())
        {
            while(name = pDisk->nextDirectoryEntry(h), !name.isEmpty())
            {
                QString fullName = common::DiskOps::mergeName(path, name);
                if(pDisk->isFile(fullName))
                {
                    if(track::info::Info::isSupported(name))
                    {
                        fileList.append(fullName);
                    }
                }
                else if(pDisk->isDirectory(fullName))
                {
                    dirList.append(fullName);
                }
            }
            pDisk->closeDirectory(h);
            
            for(int i = 0; i < dirList.size() && !isProgressCancelled(progress); i++)
            {
                const QString& dirName = dirList.at(i);
                if(level == 0 && progress != 0)
                {
                    tfloat32 scanProgress = static_cast<tfloat32>(i) / static_cast<tfloat32>(dirList.size());
                    scanProgress = (static_cast<tfloat32>(mountNo) + scanProgress) / static_cast<tfloat32>(mountTotal);
                    scanProgress *= 5.0f;
                    progress->setProgress(scanProgress);
                }
                scanDirectoryRecursive(dirName, fileList, level + 1, progress, mountNo, mountTotal);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool PlayerTrackDBManager::buildDatabaseFromMountpoints(const QString& trackDBPath, const QStringList& mountPoints, common::AbstractProgressInterface *progress)
{
    int i;
    QStringList fileList;
    track::db::TrackDB *trackDB = track::db::TrackDB::instance();
    QSharedPointer<common::DiskIF> pDisk = common::DiskIF::instance();
    
    for(QStringList::const_iterator ppI = mountPoints.begin(); ppI != mountPoints.end(); ppI++)
    {
        const QString& mountPoint = *ppI;
        if(!pDisk->isDirectory(mountPoint))
        {
            QString err = QString("Expected mount point directory '%1' does not exist").arg(mountPoint);
            printError("buildDatabaseFromMountpoints", err.toUtf8().constData());
            return false;
        }
    }
    
    if(trackDB != 0)
    {
        trackDB->close();
        common::DiskOps::deleteFile(trackDBPath);
        if(!trackDB->open(trackDBPath))
        {
            QString err = QString("Failed to open track database '%1'").arg(trackDBPath);
            printError("buildDatabaseFromMountpoints", err.toUtf8().constData());
            return false;
        }
    }
    else
    {
        common::DiskOps::deleteFile(trackDBPath);
        trackDB = track::db::TrackDB::instance(trackDBPath);
        if(trackDB == 0)
        {
            QString err = QString("Failed to open track database '%1'").arg(trackDBPath);
            printError("buildDatabaseFromMountpoints", err.toUtf8().constData());
            return false;        
        }
    }
    
    for(i = 0; i < mountPoints.size(); i++)
    {
        const QString& mountPoint = mountPoints.at(i);
        scanDirectoryRecursive(mountPoint, fileList, 0, progress, i, mountPoints.size());
        trackDB->mountPoints()->addMountPoint(mountPoint);
    }
    
    for(i = 0; i < fileList.size() && !isProgressCancelled(progress); i++)
    {
        const QString& name = fileList.at(i);
                    
        QSharedPointer<track::info::Info> dbInfo = track::db::DBInfo::readInfo(name);
        if(dbInfo.isNull())
        {
            QString err = QString("Failed to read expected info from '%1'").arg(name);
            printError("buildDatabaseFromMountpoints", err.toUtf8().constData());
        }
                        
        if(progress != 0)
        {
            tfloat32 fileProgress = static_cast<tfloat32>(i) / static_cast<tfloat32>(fileList.size());
            fileProgress *= 95.0f;
            fileProgress += 5.0f;
            progress->setProgress(fileProgress);
        }
    }
    
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
