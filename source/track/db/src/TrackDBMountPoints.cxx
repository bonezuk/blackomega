#include "track/db/inc/TrackDBMountPoints.h"
#include "track/db/inc/TrackDB.h"

#include <QDir>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

TrackDBMountPoints::TrackDBMountPoints(SQLiteDatabase *db) : m_db(db)
{}

//-------------------------------------------------------------------------------------------

TrackDBMountPoints::~TrackDBMountPoints()
{}

//-------------------------------------------------------------------------------------------

void TrackDBMountPoints::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "TrackDBMountPoints::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool TrackDBMountPoints::insertPointToDB(const QString& name)
{
    QString cmd, mName;
    SQLiteInsert mountI(m_db);
    
    mName = TrackDB::instance()->formatDirectoryPath(name);
    cmd = "INSERT INTO mountpoints (mountName) VALUES (?)";
    mountI.prepare(cmd);
    mountI.bind(mName);
    return mountI.next();
}

//-------------------------------------------------------------------------------------------

bool TrackDBMountPoints::addMountPoint(const QString& mountPointDir)
{
    bool res = false;
    QDir mountDir(mountPointDir);
    
    if(mountDir.exists())
    {
        QString mountName = mountDir.absolutePath();
        QStringList existMounts = mountPoints();
        
        if(!existMounts.contains(mountName))
        {
            mountName = TrackDB::instance()->formatDirectoryPath(mountName);
        
            try
            {
                m_db->exec("SAVEPOINT addmountpoint");
                if(insertPointToDB(mountName))
                {
                    res = true;
                }
                else
                {
                    printError("addMountPoint", "Failed to insert mount point into database");
                }
            
                if(res)
                {
                    m_db->exec("RELEASE addmountpoint");
                }
                else
                {
                    m_db->exec("ROLLBACK TO SAVEPOINT addmountpoint");
                }
            }
            catch(const SQLiteException& e)
            {
                QString errStr;
                m_db->exec("ROLLBACK TO SAVEPOINT addmountpoint");
                errStr = "Database exception thrown. " + e.error();
                printError("addMountPoint",errStr.toUtf8().constData());
                res = false;
            }        
        }
        else
        {
            res = true;
        }
    }
    else
    {
        common::Log::g_Log << "Cannot create mount point '" << mountPointDir << "' as directory does not exist." << common::c_endl;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QStringList TrackDBMountPoints::mountPoints()
{
    QStringList points;
    QString cmdQ, mountPointName;
    SQLiteQuery mountQ(m_db);
    
    cmdQ = "SELECT m.mountName FROM mountpoints AS m";
    mountQ.prepare(cmdQ);
    mountQ.bind(mountPointName);
    while(mountQ.next())
    {
        points.append(mountPointName);
    }
    return points;
}

//-------------------------------------------------------------------------------------------

bool TrackDBMountPoints::isSeparator(QChar c) const
{
    return (c == QChar('/') || c == QChar('\\'));
}

//-------------------------------------------------------------------------------------------

QString TrackDBMountPoints::pathPart(const QString& path, int idx) const
{
    int startPos, endPos;
    QString part;
    
    startPos = (path.at(1) == QChar(':') && isSeparator(path.at(2))) ? 3 : 0;
    while(startPos < path.length() && isSeparator(path.at(startPos)))
    {
        startPos++;
    }
    if(startPos < path.length())
    {
        do
        {
            endPos = path.indexOf(QChar('/'), startPos);
            if(endPos < 0)
            {
                endPos = path.length();
            }
            idx--;
            if(idx >= 0)
            {
                startPos = endPos + 1;
            }
        } while(idx >= 0 && startPos < path.length());
        
        if(startPos < path.length() && startPos < endPos)
        {
            part = path.mid(startPos, endPos - startPos);
        }
    }
    return part;
}

//-------------------------------------------------------------------------------------------
// With each build of iOS app the UUID to the document path changes.
// The mount point functionality is used to find the currently listed
// document directory and update it if the UUID changes.
//-------------------------------------------------------------------------------------------

bool TrackDBMountPoints::updateAppMountPath(const QString& path)
{
    int mountID;
    QString p, cmd;
    SQLiteQuery mountQ(m_db);
    bool isAdd = true, res = false;
    
    cmd = "SELECT m.mountID, m.mountName FROM mountpoints AS m";
    mountQ.prepare(cmd);
    mountQ.bind(mountID);
    mountQ.bind(p);

    while(mountQ.next() && isAdd)
    {
        tint i;
        bool isSame = true;
        
        for(i = 0; i < 4 && isSame; i++)
        {
            QString nPart = pathPart(path, i);
            QString cPart = pathPart(p, i);
            if(nPart != cPart)
            {
                isSame = false;
            }
        }
        
        if(isSame)
        {
            QString dName = TrackDB::instance()->formatDirectoryPath(path);
            if(dName != p)
            {
                try
                {
                    m_db->exec("SAVEPOINT updatepoint");
                    cmd = "UPDATE mountpoints SET mountName='" + dName + "' WHERE mountID=" + QString::number(mountID);
                    m_db->exec(cmd);
                    m_db->exec("RELEASE updatepoint");
                    res = true;
                }
                catch(const SQLiteException& e)
                {
                    QString errStr;
                    m_db->exec("ROLLBACK TO SAVEPOINT updatepoint");
                    errStr = "Database exception thrown on update. " + e.error();
                    printError("updateAppMountPath",errStr.toUtf8().constData());
                }
            }
            else
            {
                res = true;
            }
            isAdd = false;
            break;
        }
    }
    
    if(isAdd)
    {
        try
        {
            m_db->exec("SAVEPOINT insertpoint");
            if(insertPointToDB(path))
            {
                m_db->exec("RELEASE insertpoint");
                res = true;
            }
            else
            {
                m_db->exec("ROLLBACK TO SAVEPOINT insertpoint");
            }
        }
        catch(const SQLiteException& e)
        {
            QString errStr;
            m_db->exec("ROLLBACK TO SAVEPOINT insertpoint");
            errStr = "Database exception thrown on insert. " + e.error();
            printError("updateAppMountPath",errStr.toUtf8().constData());
        }
    }
    
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
