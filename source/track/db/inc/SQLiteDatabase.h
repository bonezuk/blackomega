//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_SQLITEDATABASE_H
#define __OMEGA_TRACK_DB_SQLITEDATABASE_H
//-------------------------------------------------------------------------------------------

#include <QString>
#include <QVector>
#include <QByteArray>
#include <QPair>

#include "track/db/inc/sqlite3.h"
#include "track/db/inc/SQLiteLog.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class SQLiteLog;
class SQLiteException;

//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT SQLiteDatabase
{
    public:
        SQLiteDatabase();
        virtual ~SQLiteDatabase();

        virtual bool isOpen() const;

        virtual bool open(const QString& name);
        virtual void close();
        virtual bool reopen();

        virtual void exec(const QString& cmd);

        virtual SQLiteLog& log();
        virtual const SQLiteLog& log() const;

        virtual sqlite3 *db();
        virtual const sqlite3 *db() const;
        virtual const QString& dbName() const;

        virtual bool isJournal() const;
        virtual bool setJournal(bool flag);

    protected:

        sqlite3 *m_db;

        QString m_dbName;
        SQLiteLog m_log;
        bool m_journalFlag;
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

