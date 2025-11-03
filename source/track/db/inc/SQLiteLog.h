//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_SQLITELOG_H
#define __OMEGA_TRACK_DB_SQLITELOG_H
//-------------------------------------------------------------------------------------------

#include <QTime>
#include <QDate>
#include <QDateTime>
#include <QVector>
#include <QStringList>

#include "common/inc/TimeStamp.h"
#include "track/db/inc/DbDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class SQLiteDatabase;
class SQLiteOperation;

//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT SQLiteLog
{
    public:
        typedef enum
        {
            e_formatDebugging = 0,
            e_formatErrorReport,
            e_formatComment
        } FormatType;

        typedef enum
        {
            e_logEntry = 0,
            e_logOperation,
            e_logError,
            e_logComment
        } EntryType;

        class Entry
        {
            public:
                Entry();
                Entry(EntryType tp,const common::TimeStamp& t,const QString& txt);
                Entry(const Entry& rhs);

                const Entry& operator = (const Entry& rhs);

                EntryType type() const;
                const common::TimeStamp& time() const;
                const QString& text() const;

                QString print(FormatType fmt) const;

            protected:

                EntryType m_type;
                common::TimeStamp m_time;
                QString m_text;

                void copy(const Entry& rhs);
        };

    public:
        SQLiteLog(SQLiteDatabase *db);
        virtual ~SQLiteLog();

        // clear down log
        void clear();
        // number of entries
        int size() const;
        // access entry
        const Entry& entry(tint idx) const;
        // print out log with given format
        void print(FormatType fmt,QStringList& logOutput) const;

        // add entry for text string
        void logEntry(const QString& txt);

        // add entry for an SQL operation
        void logOperation(const QString& txt);
        void logOperation(const SQLiteOperation& op);

        // add comment
        void logComment(const QString& txt);

        // add entry for an error
        void logError(const QString& txt,bool dbError = true);
        void logError(const QString& txt,const SQLiteOperation& op,bool dbError = true);

        const QVector<Entry>& log() const;

    protected:

        SQLiteDatabase *m_db;
        common::TimeStamp m_startTime;
        QVector<Entry> m_log;
};

//-------------------------------------------------------------------------------------------

inline SQLiteLog::EntryType SQLiteLog::Entry::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& SQLiteLog::Entry::time() const
{
    return m_time;
}

//-------------------------------------------------------------------------------------------

inline const QString& SQLiteLog::Entry::text() const
{
    return m_text;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

