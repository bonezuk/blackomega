//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_SQLITEOPERATION_H
#define __OMEGA_TRACK_DB_SQLITEOPERATION_H
//-------------------------------------------------------------------------------------------

#include "track/db/inc/SQLiteDatabase.h"
#include "track/db/inc/SQLiteException.h"

#include <QSharedPointer>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT SQLiteOperation
{
    public:

        typedef enum
        {
            e_columnBool = 0,
            e_columnInt,
            e_columnUInt,
            e_columnInt64,
            e_columnUInt64,
            e_columnFloat,
            e_columnDouble,
            e_columnText,
            e_columnBlob
        } ColumnType;

    public:
        SQLiteOperation(SQLiteDatabase *db);
        virtual ~SQLiteOperation();

        virtual void prepare(const QString& cmd);

        virtual bool next() = 0;

        // boolean
        virtual void bind(bool& v);
        // integer
        virtual void bind(tint& v);
        // unsigned integer
        virtual void bind(tuint& v);
        // qint64
        virtual void bind(tint64& v);
        // quint64
        virtual void bind(tuint64& v);
        // float
        virtual void bind(tfloat32& v);
        // double
        virtual void bind(tfloat64& v);
        // text
        virtual void bind(QString& v);
        // blob
        virtual void bind(QByteArray& v);

        virtual const QString& sqlCommand() const;

    protected:

        SQLiteDatabase *m_db;

        QString m_command;
        sqlite3_stmt *m_statement;

        QVector<QPair<ColumnType,void *> > m_columnData;

        void freeStatement();
        void bindColumn(ColumnType type,void *data);
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

