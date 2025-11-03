//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_SQLITEQUERY_H
#define __OMEGA_TRACK_DB_SQLITEQUERY_H
//-------------------------------------------------------------------------------------------

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

class TRACK_DB_EXPORT SQLiteQuery : public SQLiteOperation
{
    public:
        SQLiteQuery(SQLiteDatabase *db);
        virtual ~SQLiteQuery();
        
        virtual bool next();

        virtual bool columnBool(tint cNo);
        virtual tint columnInt(tint cNo);
        virtual tuint columneUInt(tint cNo);
        virtual tint64 columnInt64(tint cNo);
        virtual tuint64 columnUInt64(tint cNo);
        virtual tfloat32 columnFloat(tint cNo);
        virtual tfloat64 columnDouble(tint cNo);
        virtual QString columnText(tint cNo);
        virtual QByteArray columnBlob(tint cNo);
};

typedef QSharedPointer<SQLiteQuery> SQLiteQuerySPtr;

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

