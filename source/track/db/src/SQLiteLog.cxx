#include "track/db/inc/SQLiteLog.h"
#include "track/db/inc/SQLiteDatabase.h"
#include "track/db/inc/SQLiteOperation.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------
// SQLiteLog::Entry
//-------------------------------------------------------------------------------------------

SQLiteLog::Entry::Entry() : m_type(e_logEntry),
    m_time(),
    m_text()
{}

//-------------------------------------------------------------------------------------------

SQLiteLog::Entry::Entry(EntryType tp,const common::TimeStamp& t,const QString& txt) : m_type(tp),
    m_time(t),
    m_text(txt)
{}

//-------------------------------------------------------------------------------------------

SQLiteLog::Entry::Entry(const Entry& rhs)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const SQLiteLog::Entry& SQLiteLog::Entry::operator = (const Entry& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void SQLiteLog::Entry::copy(const Entry& rhs)
{
    m_type = rhs.m_type;
    m_time = rhs.m_time;
    m_text = rhs.m_text;
}

//-------------------------------------------------------------------------------------------

QString SQLiteLog::Entry::print(FormatType fmt) const
{
    QString pTxt;
    QDate lD(m_time.year(),m_time.month(),m_time.day());
    QTime lT(m_time.hour(),m_time.minute(),m_time.second(),m_time.millisecond());
    QDateTime lDT(lD,lT);
    
    if(fmt==e_formatDebugging)
    {
        pTxt = lDT.toString("hh::mm:ss.zzz") + " : " + m_text;
    }
    else if(fmt==e_formatErrorReport)
    {
        if(m_type==e_logError)
        {
            pTxt = m_text;
        }
    }
    else if(fmt==e_formatComment)
    {
        if(m_type==e_logComment)
        {
            pTxt = m_text;
        }
    }
    return pTxt;
}

//-------------------------------------------------------------------------------------------
// SQLiteLog
//-------------------------------------------------------------------------------------------

SQLiteLog::SQLiteLog(SQLiteDatabase *db) : m_db(db),
    m_startTime(common::TimeStamp::now()),
    m_log()
{}

//-------------------------------------------------------------------------------------------

SQLiteLog::~SQLiteLog()
{}

//-------------------------------------------------------------------------------------------
// clear down log
//-------------------------------------------------------------------------------------------

void SQLiteLog::clear()
{
    m_log.clear();
}

//-------------------------------------------------------------------------------------------
// number of entries
//-------------------------------------------------------------------------------------------

int SQLiteLog::size() const
{
    return m_log.size();
}

//-------------------------------------------------------------------------------------------
// access entry
//-------------------------------------------------------------------------------------------

const SQLiteLog::Entry& SQLiteLog::entry(tint idx) const
{
    return m_log.at(idx);
}

//-------------------------------------------------------------------------------------------
// print out log with given format
//-------------------------------------------------------------------------------------------

void SQLiteLog::print(FormatType fmt,QStringList& logOutput) const
{
    QVector<Entry>::const_iterator ppI;
    
    logOutput.clear();
    for(ppI=m_log.constBegin();ppI!=m_log.constEnd();ppI++)
    {
        const Entry& e = *ppI;
        logOutput.append(e.print(fmt));
    }
}

//-------------------------------------------------------------------------------------------
// add entry for text string
//-------------------------------------------------------------------------------------------

void SQLiteLog::logEntry(const QString& txt)
{
    Entry e(e_logEntry,common::TimeStamp::now(),txt);
    m_log.append(e);
}

//-------------------------------------------------------------------------------------------

void SQLiteLog::logComment(const QString& txt)
{
    Entry e(e_logComment,common::TimeStamp::now(),txt);
    m_log.append(e);
}

//-------------------------------------------------------------------------------------------
// add entry for an SQL operation
//-------------------------------------------------------------------------------------------

void SQLiteLog::logOperation(const QString& txt)
{
    Entry e(e_logOperation,common::TimeStamp::now(),txt);
    m_log.append(e);
}

//-------------------------------------------------------------------------------------------

void SQLiteLog::logOperation(const SQLiteOperation& op)
{
    logOperation(op.sqlCommand());
}

//-------------------------------------------------------------------------------------------
// add entry for an error
//-------------------------------------------------------------------------------------------

void SQLiteLog::logError(const QString& txt,bool dbError)
{
    if(dbError && m_db->db()!=0)
    {
        QString eTxt(txt);
        eTxt += " " + QString::fromUtf8(sqlite3_errmsg(m_db->db()));
        Entry e(e_logError,common::TimeStamp::now(),eTxt);
        m_log.append(e);        
    }
    else
    {
        Entry e(e_logError,common::TimeStamp::now(),txt);
        m_log.append(e);
    }
}

//-------------------------------------------------------------------------------------------

void SQLiteLog::logError(const QString& txt,const SQLiteOperation& op,bool dbError)
{
    QString eTxt(txt);
    
    if(dbError && m_db->db()!=0)
    {
        eTxt += " " + QString::fromUtf8(sqlite3_errmsg(m_db->db())) + ".";
    }
    eTxt += " For SQL command \"" + op.sqlCommand() + "\"";
    Entry e(e_logError,common::TimeStamp::now(),eTxt);
    m_log.append(e);
}

//-------------------------------------------------------------------------------------------

const QVector<SQLiteLog::Entry>& SQLiteLog::log() const
{
    return m_log;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
