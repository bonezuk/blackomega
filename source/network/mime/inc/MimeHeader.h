//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_MIME_MIMEHEADER_H
#define __OMEGA_NETWORK_MIME_MIMEHEADER_H
//-------------------------------------------------------------------------------------------

#include "network/mime/inc/MimeHeaderItem.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace mime
{
//-------------------------------------------------------------------------------------------

class MimeSection;

//-------------------------------------------------------------------------------------------

class MIME_EXPORT MimeHeader
{
    public:
        friend class MimeSection;

        MimeHeader();
        MimeHeader(const tchar *str);
        MimeHeader(const common::BString& str,tint i = 0);
        MimeHeader(const QString& str);
        MimeHeader(const MimeHeader& hdr);
        virtual ~MimeHeader();

        void equal(const common::BString& str,tint i);

        const MimeHeader& operator = (const tchar *str);
        const MimeHeader& operator = (const common::BString& str);
        const MimeHeader& operator = (const QString& str);
        const MimeHeader& operator = (const MimeHeader& hdr);

        bool add(const tchar *str);
        bool add(const common::BString& str);
        bool add(const QString& str);
        bool add(const MimeHeaderItem& hdr);

        const MimeHeaderItem& item(tint idx) const;
        const MimeHeaderItem& item(const tchar *str) const;
        const MimeHeaderItem& item(const common::BString& str) const;
        const MimeHeaderItem& item(const QString& str) const;

        tint size() const;

        operator common::BString() const;
        operator QString() const;

        void compile(common::BString& out) const;
        void compile(QString& out) const;

        bool isValid() const;
        tint parseEndPosition() const;

    protected:

        static MimeHeaderItem m_null;

        QMap<common::BString,tint> m_index;
        QList<MimeHeaderItem> m_items;

        tint m_parsePosition;
        bool m_parseResult;

        void printError(const tchar *strR,const tchar *strE) const;

        bool parse(const tchar *str);
        bool parse(const common::BString& str,tint i=0);
        bool parse(const QString& str);

        void blank();

        void copy(const MimeHeader& hdr);
};

//-------------------------------------------------------------------------------------------
} // namepsace mime
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

