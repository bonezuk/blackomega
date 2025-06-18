#include "track/db/inc/SQLiteQuery.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

SQLiteQuery::SQLiteQuery(SQLiteDatabase *db) : SQLiteOperation(db)
{}

//-------------------------------------------------------------------------------------------

SQLiteQuery::~SQLiteQuery()
{}

//-------------------------------------------------------------------------------------------

bool SQLiteQuery::next()
{
    tint i,res;
    bool loop;

    if(m_statement==0)
    {
        throw SQLiteException(m_db,this,"No SQL statement has been prepared.",false);
    }

    i = 0;
    do
    {
        loop = false;
        res = sqlite3_step(m_statement);
        switch(res)
        {
            case SQLITE_BUSY:
                i++;
                if(i<100)
                {
                    sqlite3_sleep(10);
                    loop = true;
                }
                else
                {
                    throw SQLiteException(m_db,this,"Timeout waiting for record to unlock during selection query.",false);
                }
                break;

            case SQLITE_DONE:
                return false;

            case SQLITE_ROW:
                break;

            case SQLITE_ERROR:
                throw SQLiteException(m_db,this,"Error in executing selection query.");

            case SQLITE_MISUSE:
                throw SQLiteException(m_db,this,"Misuse of selection query.");

            default:
                throw SQLiteException(m_db,this,"Unexpected result in selection query.");
        }
    } while(loop);

    for(i=0;i<m_columnData.size();++i)
    {
        QPair<ColumnType,void *>& cData = m_columnData[i];

        switch(cData.first)
        {
            case e_columnBool:
                {
                    bool *a = reinterpret_cast<bool *>(cData.second);
                    *a = (sqlite3_column_int(m_statement,i)!=0) ? true : false;
                }
                break;

            case e_columnInt:
                {
                    tint *a = reinterpret_cast<tint *>(cData.second);
                    *a = sqlite3_column_int(m_statement,i);
                }
                break;

            case e_columnUInt:
                {
                    tuint *a = reinterpret_cast<tuint *>(cData.second);
                    *a = static_cast<tuint>(sqlite3_column_int(m_statement,i));
                }
                break;

            case e_columnInt64:
                {
                    tint64 *a = reinterpret_cast<tint64 *>(cData.second);
                    *a = static_cast<tint64>(sqlite3_column_int64(m_statement,i));
                }
                break;

            case e_columnUInt64:
                {
                    tuint64 *a = reinterpret_cast<tuint64 *>(cData.second);
                    *a = static_cast<tuint64>(sqlite3_column_int64(m_statement,i));
                }
                break;

            case e_columnFloat:
                {
                    tfloat32 *a = reinterpret_cast<tfloat32 *>(cData.second);
                    *a = static_cast<tfloat32>(sqlite3_column_double(m_statement,i));
                }
                break;

            case e_columnDouble:
                {
                    tfloat64 *a = reinterpret_cast<tfloat64 *>(cData.second);
                    *a = sqlite3_column_double(m_statement,i);
                }
                break;

            case e_columnText:
                {
                    QString& a = *(reinterpret_cast<QString *>(cData.second));
                    a = QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(m_statement,i)),sqlite3_column_bytes(m_statement,i));
                }
                break;

            case e_columnBlob:
                {
                    QByteArray& aMem = *(reinterpret_cast<QByteArray *>(cData.second));
                    QByteArray bMem(reinterpret_cast<const char *>(sqlite3_column_blob(m_statement,i)),sqlite3_column_bytes(m_statement,i));
                    aMem = bMem;
                }
                break;

            default:
                throw SQLiteException(m_db,this,"Unknown column type given for binding during query.",false);
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool SQLiteQuery::columnBool(tint cNo)
{
    return (sqlite3_column_int(m_statement,cNo)!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

tint SQLiteQuery::columnInt(tint cNo)
{
    return sqlite3_column_int(m_statement,cNo);
}

//-------------------------------------------------------------------------------------------

tuint SQLiteQuery::columneUInt(tint cNo)
{
    return static_cast<tuint>(sqlite3_column_int(m_statement,cNo));
}

//-------------------------------------------------------------------------------------------

tint64 SQLiteQuery::columnInt64(tint cNo)
{
    return static_cast<tint64>(sqlite3_column_int64(m_statement,cNo));
}

//-------------------------------------------------------------------------------------------

tuint64 SQLiteQuery::columnUInt64(tint cNo)
{
    return static_cast<tuint64>(sqlite3_column_int64(m_statement,cNo));
}

//-------------------------------------------------------------------------------------------

tfloat32 SQLiteQuery::columnFloat(tint cNo)
{
    return static_cast<tfloat32>(sqlite3_column_double(m_statement,cNo));
}

//-------------------------------------------------------------------------------------------

tfloat64 SQLiteQuery::columnDouble(tint cNo)
{
    return sqlite3_column_double(m_statement,cNo);
}

//-------------------------------------------------------------------------------------------

QString SQLiteQuery::columnText(tint cNo)
{
    QString a(QString::fromUtf8(reinterpret_cast<const char *>(sqlite3_column_text(m_statement,cNo)),sqlite3_column_bytes(m_statement,cNo)));
    return a;
}

//-------------------------------------------------------------------------------------------

QByteArray SQLiteQuery::columnBlob(int cNo)
{
    QByteArray a(reinterpret_cast<const char *>(sqlite3_column_blob(m_statement,cNo)),sqlite3_column_bytes(m_statement,cNo));
    return a;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
