//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_SQLITEMOCK_H
#define __OMEGA_TRACK_DB_SQLITEMOCK_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/db/inc/SQLiteDatabase.h"
#include "track/db/inc/SQLiteInsert.h"
#include "track/db/inc/SQLiteQuery.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class SQLiteDatabaseMock : public SQLiteDatabase
{
    public:
        SQLiteDatabaseMock();
        
        MOCK_CONST_METHOD0(isOpen,bool());
        MOCK_METHOD1(open,bool(const QString& name));
        MOCK_METHOD0(close,void());
        MOCK_METHOD0(reopen,bool());
        MOCK_METHOD1(exec,void(const QString& cmd));
};

//-------------------------------------------------------------------------------------------

class SQLiteQueryMock : public SQLiteQuery
{
    public:
        SQLiteQueryMock(SQLiteDatabase *db);
        
        MOCK_METHOD1(prepare,void(const QString& cmd));
        MOCK_METHOD0(next,bool());
        
        MOCK_METHOD1(bind,void(bool& v));
        MOCK_METHOD1(bind,void(tint& v));
        MOCK_METHOD1(bind,void(tuint& v));
        MOCK_METHOD1(bind,void(tint64& v));
        MOCK_METHOD1(bind,void(tuint64& v));
        MOCK_METHOD1(bind,void(tfloat32& v));
        MOCK_METHOD1(bind,void(tfloat64& v));
        MOCK_METHOD1(bind,void(QString& v));
        MOCK_METHOD1(bind,void(std::string& v));
        MOCK_METHOD1(bind,void(QByteArray& v));
};

//-------------------------------------------------------------------------------------------

class SQLiteInsertMock : public SQLiteInsert
{
    public:
        SQLiteInsertMock(SQLiteDatabase *db);
        
        MOCK_METHOD1(prepare,void(const QString& cmd));
        MOCK_METHOD0(next,bool());
        
        MOCK_METHOD1(bind,void(bool& v));
        MOCK_METHOD1(bind,void(tint& v));
        MOCK_METHOD1(bind,void(tuint& v));
        MOCK_METHOD1(bind,void(tint64& v));
        MOCK_METHOD1(bind,void(tuint64& v));
        MOCK_METHOD1(bind,void(tfloat32& v));
        MOCK_METHOD1(bind,void(tfloat64& v));
        MOCK_METHOD1(bind,void(QString& v));
        MOCK_METHOD1(bind,void(std::string& v));
        MOCK_METHOD1(bind,void(QByteArray& v));
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
