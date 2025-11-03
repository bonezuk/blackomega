//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_HTTP_QUERY_H
#define __OMEGA_NETWORK_HTTP_QUERY_H
//-------------------------------------------------------------------------------------------

#include "network/http/inc/ResponseCodes.h"
#include "common/inc/Log.h"
#include <QList>
#include <QSet>
#include <QMap>
#include <QString>
#include <QStringList>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

class HTTP_EXPORT Query
{
    public:
        Query();
        Query(const tchar *s);
        Query(const QString& s);
        Query(const Query& rhs);

        void string(QString& str) const;

        QString value() const;

        const Query& operator = (const tchar *s);
        const Query& operator = (const QString& s);
        const Query& operator = (const Query& rhs);

        void add(const tchar *h,const tchar *d);
        void add(const tchar *h,const QString& d);
        void add(const QString& h,const tchar *d);
        void add(const QString& h,const QString& d);

        void clear();

        const QString& data(const tchar *idx) const;
        const QString& data(const QString& idx) const;

        void map(QMap<QString,QString>& m) const;

    protected:

        QList<QString> m_order;
        QMap<QString,QString> m_header;
        QMap<QString,QString> m_data;
        QString m_blank;

        void printError(const tchar *strR,const tchar *strE) const;;

        void copy(const Query& rhs);

        void queryGenerateMask(tint *mask) const;
        void queryAddEscapeCharacter(tchar x,QString& out) const;
        void queryString(const QString& in,QString& out) const;

        void parse(const tchar *s);

        void generate(QString& out) const;
};

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

