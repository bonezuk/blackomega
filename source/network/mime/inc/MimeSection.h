//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_MIME_MIMESECTION_H
#define __OMEGA_NETWORK_MIME_MIMESECTION_H
//-------------------------------------------------------------------------------------------

#include "network/mime/inc/MimeHeader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace mime
{
//-------------------------------------------------------------------------------------------

class MimeForm;

//-------------------------------------------------------------------------------------------

class MIME_EXPORT MimeSection
{
    public:
        friend class MimeForm;

        MimeSection();
        MimeSection(const tchar *mem,tint len);
        MimeSection(const NetArray& array);
        MimeSection(const MimeSection& section);
        virtual ~MimeSection();

        const MimeSection& operator = (const NetArray& array);
        const MimeSection& operator = (const MimeSection& section);

        bool set(const tchar *mem,tint len);
        bool set(const NetArray& array);
        bool set(const MimeSection& section);

        MimeHeader& header();
        const MimeHeader& header() const;
        bool setHeader(const tchar *str);
        bool setHeader(const common::BString& str);
        bool setHeader(const QString& str);
        bool setHeader(const MimeHeader& hdr);

        NetArray& body();
        const NetArray& body() const;
        bool setBody(const tchar *mem,tint len);
        bool setBody(const NetArray& array);

        const common::BString& boundary() const;
        bool setBoundary(const tchar *str);
        bool setBoundary(const common::BString& str);
        bool setBoundary(const QString& str);

        bool compile(NetArray& array,bool doStart=true) const;

        bool isEnd() const;
        bool isValid() const;
        tint parseEndPosition() const;

    protected:

        common::BString m_boundary;
        MimeHeader m_header;
        NetArray m_body;

        tint m_parsePosition;
        bool m_parseResult;
        bool m_endSection;

        void printError(const tchar *strR,const tchar *strE) const;

        bool parseBoundaryLine(const tchar *mem,tint len,tint& offset);
        bool parseBoundaryLine(const tchar *mem,tint len,tint& offset,tint& bodyEnd);

        bool parseForHeader(const tchar *mem,tint len,tint& offset,common::BString& hdrStr) const;

        bool parse(const tchar *mem,tint len);
        bool parse(const NetArray& array);

        void blank();
        void copy(const MimeSection& section);
};

//-------------------------------------------------------------------------------------------
} // namespace mime
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

