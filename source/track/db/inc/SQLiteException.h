//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_SQLITEEXCEPTION_H
#define __OMEGA_TRACK_DB_SQLITEEXCEPTION_H
//-------------------------------------------------------------------------------------------

#include "track/db/inc/SQLiteDatabase.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT SQLiteException
{
    public:
        SQLiteException(SQLiteDatabase *db,const QString& e,bool dbError = true);
        SQLiteException(SQLiteDatabase *db,const SQLiteOperation *op,const QString& e,bool dbError = true);

        SQLiteDatabase *database() const;
        const QString& error() const;

    protected:

        SQLiteDatabase *m_db;
        QString m_error;
};

//-------------------------------------------------------------------------------------------

inline SQLiteDatabase *SQLiteException::database() const
{
    return m_db;
}

//-------------------------------------------------------------------------------------------

inline const QString& SQLiteException::error() const
{
    return m_error;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

