#include "track/db/inc/SQLiteInsert.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

SQLiteInsert::SQLiteInsert(SQLiteDatabase *db) : SQLiteOperation(db)
{}

//-------------------------------------------------------------------------------------------

SQLiteInsert::~SQLiteInsert()
{}

//-------------------------------------------------------------------------------------------

bool SQLiteInsert::next()
{
    if(m_statement!=0)
    {
        tint i,res = 0;
        bool loop;
        
        for(i=0;i<m_columnData.size();i++)
        {
            QPair<ColumnType,void *>& cData = m_columnData[i];
            
            switch(cData.first)
            {
                case e_columnBool:
                    {
                        bool *d = reinterpret_cast<bool *>(cData.second);
                        tint v = (*d) ? 1 : 0;
                        res = sqlite3_bind_int(m_statement,i+1,v);
                    }
                    break;
                    
                case e_columnInt:
                case e_columnUInt:
                    {
                        tint *d = reinterpret_cast<tint *>(cData.second);
                        res = sqlite3_bind_int(m_statement,i+1,*d);
                    }
                    break;
                    
                case e_columnInt64:
                case e_columnUInt64:
                    {
                        tint64 *d = reinterpret_cast<tint64 *>(cData.second);
                        res = sqlite3_bind_int64(m_statement,i+1,*d);
                    }
                    break;
                
                case e_columnFloat:
                    {
                        tfloat32 *d = reinterpret_cast<tfloat32 *>(cData.second);
                        res = sqlite3_bind_double(m_statement,i+1,static_cast<double>(*d));
                    }
                    break;
                
                case e_columnDouble:
                    {
                        tfloat64 *d = reinterpret_cast<tfloat64 *>(cData.second);
                        res = sqlite3_bind_double(m_statement,i+1,static_cast<double>(*d));
                    }
                    break;
                
                case e_columnText:
                    {
                        QString *d = reinterpret_cast<QString *>(cData.second);
                        QByteArray a(d->toUtf8());
                        res = sqlite3_bind_text(m_statement,i+1,a.constData(),a.length(),SQLITE_TRANSIENT);
                    }
                    break;
                    
                case e_columnBlob:
                    {
                        QByteArray *d = reinterpret_cast<QByteArray *>(cData.second);
                        res = sqlite3_bind_blob(m_statement,i+1,d->constData(),d->length(),SQLITE_TRANSIENT);
                    }
                    break;
                    
                default:
                    throw SQLiteException(m_db,this,"Unknown column type given for binding during insertion.",false);
                    break;
            }
            
            if(res!=SQLITE_OK)
            {
                throw SQLiteException(m_db,this,"Failed to bind column during insertion.");
            }
        }
        
        i = 0;
        do
        {
            loop = false;
            res = sqlite3_step(m_statement);
            switch(res)
            {
                case SQLITE_DONE:
                    break;

                case SQLITE_BUSY:
                    i++;
                    if(i<100)
                    {
                        sqlite3_sleep(10);
                        loop = true;
                    }
                    else
                    {
                        throw SQLiteException(m_db,this,"Timeout waiting for record to unlock during insertion.",false);
                    }
                    break;
                
                case SQLITE_ERROR:
                    throw SQLiteException(m_db,this,"Error in inserting row.");
                    
                case SQLITE_MISUSE:
                    throw SQLiteException(m_db,this,"Misuse of SQL in inserting row.");
                    
                default:
                    throw SQLiteException(m_db,this,"Unexpected result on row insertion.");
            }
        } while(loop);
        
        if(sqlite3_reset(m_statement)!=SQLITE_OK)
        {
            throw SQLiteException(m_db,this,"Error in reseting prepared insert SQL statement.");
        }
        m_columnData.clear();
    }
    else
    {
        throw SQLiteException(m_db,this,"No SQL statement has been prepared.",false);
    }
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
