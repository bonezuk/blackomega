//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_SQLITEINSERT_H
#define __OMEGA_TRACK_DB_SQLITEINSERT_H
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

class TRACK_DB_EXPORT SQLiteInsert : public SQLiteOperation
{
    public:
        SQLiteInsert(SQLiteDatabase *db);
        virtual ~SQLiteInsert();

        virtual bool next();
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

