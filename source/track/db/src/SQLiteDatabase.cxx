#include "track/db/inc/SQLiteDatabase.h"
#include "track/db/inc/SQLiteException.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

SQLiteDatabase::SQLiteDatabase() : m_db(0),
    m_dbName(),
    m_log(this),
    m_journalFlag(true)
{}

//-------------------------------------------------------------------------------------------

SQLiteDatabase::~SQLiteDatabase()
{
    close();
}

//-------------------------------------------------------------------------------------------

bool SQLiteDatabase::isOpen() const
{
    return (m_db!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

SQLiteLog& SQLiteDatabase::log()
{
    return m_log;
}

//-------------------------------------------------------------------------------------------

const SQLiteLog& SQLiteDatabase::log() const
{
    return m_log;
}

//-------------------------------------------------------------------------------------------

sqlite3 *SQLiteDatabase::db()
{
    return m_db;
}

//-------------------------------------------------------------------------------------------

const sqlite3 *SQLiteDatabase::db() const
{
    return m_db;
}

//-------------------------------------------------------------------------------------------

const QString& SQLiteDatabase::dbName() const
{
    return m_dbName;
}

//-------------------------------------------------------------------------------------------

bool SQLiteDatabase::isJournal() const
{
    return m_journalFlag;
}

//-------------------------------------------------------------------------------------------

bool SQLiteDatabase::open(const QString& name)
{
    tint res;

    close();

    m_dbName = name;
    res = sqlite3_open_v2(name.toUtf8().constData(),&m_db,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,0);
    if(res==SQLITE_OK)
    {
        return true;
    }
    else
    {
        QString err;
        err = "Failed to open database file '" + name + "'";
        m_log.logError(err);
        close();
        return false;
    }
}

//-------------------------------------------------------------------------------------------

bool SQLiteDatabase::reopen()
{
    tint res;

    close();

    res = sqlite3_open_v2(m_dbName.toUtf8().constData(),&m_db,SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,0);
    if(res==SQLITE_OK)
    {
        return true;
    }
    else
    {
        QString err;
        err = "Failed to open database file '" + m_dbName + "'";
        m_log.logError(err);
        close();
        return false;
    }
}

//-------------------------------------------------------------------------------------------

void SQLiteDatabase::close()
{
    if(m_db!=0)
    {
        if(sqlite3_close(m_db)!=SQLITE_OK)
        {
            QString err;
            err = "Failed to close database file '" + m_dbName + "'";
            m_log.logError(err);
        }
        m_db = 0;
    }
}

//-------------------------------------------------------------------------------------------

void SQLiteDatabase::exec(const QString& cmd)
{
    tchar *errMsg;

    if(sqlite3_exec(m_db,cmd.toUtf8().constData(),0,0,&errMsg)!=SQLITE_OK)
    {
        QString err;
        err = "Failed to execute SQL statement \"" + cmd + "\". " + QString::fromUtf8(errMsg);
        throw SQLiteException(this,err,false);
    }
}

//-------------------------------------------------------------------------------------------

bool SQLiteDatabase::setJournal(bool flag)
{
    bool res = true;

    try
    {
        if(flag)
        {
            exec("PRAGMA journal_mode=ON");
            m_journalFlag = true;
        }
        else
        {
            exec("PRAGMA journal_mode=OFF");
            m_journalFlag = false;
        }
    }
    catch(...)
    {
        m_log.logError("Failed to set journal mode",false);
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
