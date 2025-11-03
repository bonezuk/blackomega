//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_MIME_MIMEHEADERITEM_H
#define __OMEGA_NETWORK_MIME_MIMEHEADERITEM_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include <QString>
#include <QList>
#include <QMap>

#include "common/inc/BString.h"
#include "common/inc/BStringCollection.h"
#include "common/inc/Log.h"
#include "network/mime/inc/MimeNetworkDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace mime
{
//-------------------------------------------------------------------------------------------

class MimeHeader;

//-------------------------------------------------------------------------------------------

class MIME_EXPORT MimeHeaderItem
{
    public:
        friend class MimeHeader;

        MimeHeaderItem();
        MimeHeaderItem(const tchar *str);
        MimeHeaderItem(const common::BString& str);
        MimeHeaderItem(const QString& str);
        MimeHeaderItem(const MimeHeaderItem& hdr);
        virtual ~MimeHeaderItem();

        const MimeHeaderItem& operator = (const tchar *str);
        const MimeHeaderItem& operator = (const common::BString& str);
        const MimeHeaderItem& operator = (const QString& str);
        const MimeHeaderItem& operator = (const MimeHeaderItem& hdr);

        const common::BString& name() const;
        void setName(const tchar *str);
        void setName(const common::BString& str);
        void setName(const QString& str);

        const common::BString& data() const;
        void setData(const tchar *str);
        void setData(const common::BString& str);
        void setData(const QString& str);

        const common::BString& parameter(const tchar *str) const;
        const common::BString& parameter(const common::BString& str) const;
        const common::BString& parameter(const QString& str) const;

        void add(const tchar *index,const tchar *data);
        void add(const tchar *index,const common::BString& data);
        void add(const tchar *index,const QString& data);
        void add(const common::BString& index,const tchar *data);
        void add(const common::BString& index,const common::BString& data);
        void add(const common::BString& index,const QString& data);
        void add(const QString& index,const tchar *data);
        void add(const QString& index,const common::BString& data);
        void add(const QString& index,const QString& data);

        void enumParameters() const;
        bool next(common::BString& index,common::BString& data) const;

        operator common::BString() const;
        operator QString() const;

        void compile(common::BString& out) const;
        void compile(QString& out) const;

        bool isValid() const;
        bool isHeaderEnd() const;
        tint parseEndPosition() const;

        friend MIME_EXPORT bool operator == (const MimeHeaderItem& a,const tchar *b);
        friend MIME_EXPORT bool operator == (const MimeHeaderItem& a,const common::BString& b);
        friend MIME_EXPORT bool operator == (const MimeHeaderItem& a,const QString& b);
        friend MIME_EXPORT bool operator == (const tchar *a,const MimeHeaderItem& b);
        friend MIME_EXPORT bool operator == (const common::BString& a,const MimeHeaderItem& b);
        friend MIME_EXPORT bool operator == (const QString& a,const MimeHeaderItem& b);
        friend MIME_EXPORT bool operator == (const MimeHeaderItem& a,const MimeHeaderItem& b);

        friend MIME_EXPORT bool operator != (const MimeHeaderItem& a,const tchar *b);
        friend MIME_EXPORT bool operator != (const MimeHeaderItem& a,const common::BString& b);
        friend MIME_EXPORT bool operator != (const MimeHeaderItem& a,const QString& b);
        friend MIME_EXPORT bool operator != (const tchar *a,const MimeHeaderItem& b);
        friend MIME_EXPORT bool operator != (const common::BString& a,const MimeHeaderItem& b);
        friend MIME_EXPORT bool operator != (const QString& a,const MimeHeaderItem& b);
        friend MIME_EXPORT bool operator != (const MimeHeaderItem& a,const MimeHeaderItem& b);

    protected:

        common::BString m_name;
        common::BString m_data;
        QMap<common::BString,common::BString> m_parameters;
        QList<common::BString> m_pOrder;

        mutable tint m_enumPosition;

        bool m_parseResult;
        bool m_parseEnd;
        tint m_parsePosition;

        void printError(const tchar *strR,const tchar *strE) const;

        bool parse(const tchar *str);
        bool parse(const common::BString& str,tint i = 0);
        bool parse(const QString& str);

        void blank();

        void copy(const MimeHeaderItem& hdr);

        bool compare(const MimeHeaderItem& hdr) const;
};

//-------------------------------------------------------------------------------------------
} // namespace mime
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

