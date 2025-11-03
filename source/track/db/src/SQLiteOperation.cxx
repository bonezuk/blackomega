#include "track/db/inc/SQLiteOperation.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

SQLiteOperation::SQLiteOperation(SQLiteDatabase *db) : m_db(db),
    m_command(),
    m_statement(0),
    m_columnData()
{}

//-------------------------------------------------------------------------------------------

SQLiteOperation::~SQLiteOperation()
{
    freeStatement();
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::freeStatement()
{
    if(m_statement!=0)
    {
        if(sqlite3_finalize(m_statement)!=SQLITE_OK)
        {
            m_db->log().logError("Failed to finalize prepared SQL statement.");
        }
        m_statement = 0;
    }
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::prepare(const QString& cmd)
{
    tint res;
    
    freeStatement();
    
    m_command = cmd;
    res = sqlite3_prepare_v2(m_db->db(),cmd.toUtf8().constData(),-1,&m_statement,0);
    if(res!=SQLITE_OK)
    {
        throw SQLiteException(m_db,this,"Failed to prepare SQL statement.");
    }
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::bind(bool& v)
{
    bindColumn(e_columnBool,reinterpret_cast<void *>(&v));
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::bind(tint& v)
{
    bindColumn(e_columnInt,reinterpret_cast<void *>(&v));
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::bind(tuint& v)
{
    bindColumn(e_columnUInt,reinterpret_cast<void *>(&v));
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::bind(tint64& v)
{
    bindColumn(e_columnInt64,reinterpret_cast<void *>(&v));
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::bind(tuint64& v)
{
    bindColumn(e_columnUInt64,reinterpret_cast<void *>(&v));
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::bind(tfloat32& v)
{
    bindColumn(e_columnFloat,reinterpret_cast<void *>(&v));
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::bind(tfloat64& v)
{
    bindColumn(e_columnDouble,reinterpret_cast<void *>(&v));
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::bind(QString& v)
{
    bindColumn(e_columnText,reinterpret_cast<void *>(&v));
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::bind(QByteArray& v)
{
    bindColumn(e_columnBlob,reinterpret_cast<void *>(&v));
}

//-------------------------------------------------------------------------------------------

void SQLiteOperation::bindColumn(ColumnType type,void *data)
{
    QPair<ColumnType,void *> dPair(type,data);
    m_columnData.append(dPair);
}

//-------------------------------------------------------------------------------------------

const QString& SQLiteOperation::sqlCommand() const
{
    return m_command;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
