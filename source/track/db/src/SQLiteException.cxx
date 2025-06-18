#include "track/db/inc/SQLiteException.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

SQLiteException::SQLiteException(SQLiteDatabase *db,const QString& e,bool dbError) : m_db(db),
    m_error()
{
    m_db->log().logError(e,dbError);
    m_error = m_db->log().entry(m_db->log().size() - 1).print(SQLiteLog::e_formatErrorReport);
}

//-------------------------------------------------------------------------------------------

SQLiteException::SQLiteException(SQLiteDatabase *db,const SQLiteOperation *op,const QString& e,bool dbError) : m_db(db),
    m_error()
{
    m_db->log().logError(e,*op,dbError);
    m_error = m_db->log().entry(m_db->log().size() - 1).print(SQLiteLog::e_formatErrorReport);
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
