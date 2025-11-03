//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_MIME_MIMEFORM_H
#define __OMEGA_NETWORK_MIME_MIMEFORM_H
//-------------------------------------------------------------------------------------------

#include "network/mime/inc/MimeSection.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace mime
{
//-------------------------------------------------------------------------------------------

class MIME_EXPORT MimeForm
{
    public:
        MimeForm();
        MimeForm(const tchar *mem,tint len);
        MimeForm(const NetArray& array);
        MimeForm(const MimeForm& form);
        virtual ~MimeForm();

        const MimeForm& operator = (const NetArray& array);
        const MimeForm& operator = (const MimeForm& form);

        bool set(const tchar *mem,tint len);
        bool set(const NetArray& array);
        bool set(const MimeForm& form);

        tint size() const;

        MimeSection& section(tint idx);
        const MimeSection& section(tint idx) const;

        bool append(const MimeSection& section);

        const common::BString& boundary() const;
        bool setBoundary(const tchar *str);
        bool setBoundary(const common::BString& str);
        bool setBoundary(const QString& str);

        bool compile(NetArray& array) const;

        bool isValid() const;

        tint parseEndPosition() const;

    protected:

        common::BString m_boundary;
        QList<MimeSection> m_sections;
        bool m_parseResult;
        tint m_parsePosition;

        void printError(const tchar *strR,const tchar *strE) const;

        bool parse(const tchar *mem,tint len);
        bool parse(const NetArray& array);

        void blank();
        void copy(const MimeForm& form);
};

//-------------------------------------------------------------------------------------------
} // namespace mime
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

