#include "network/mime/inc/MimeHeader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace mime
{
//-------------------------------------------------------------------------------------------

MimeHeaderItem MimeHeader::m_null;

//-------------------------------------------------------------------------------------------

MimeHeader::MimeHeader() : m_index(),
    m_items(),
    m_parsePosition(-1),
    m_parseResult(true)
{}

//-------------------------------------------------------------------------------------------

MimeHeader::MimeHeader(const tchar *str) : m_index(),
    m_items(),
    m_parsePosition(-1),
    m_parseResult(true)
{
    m_parseResult = parse(str);
}

//-------------------------------------------------------------------------------------------

MimeHeader::MimeHeader(const common::BString& str,tint i) : m_index(),
    m_items(),
    m_parsePosition(-1),
    m_parseResult(true)
{
    m_parseResult = parse(str,i);
}

//-------------------------------------------------------------------------------------------

MimeHeader::MimeHeader(const QString& str) : m_index(),
    m_items(),
    m_parsePosition(-1),
    m_parseResult(true)
{
    m_parseResult = parse(str);
}

//-------------------------------------------------------------------------------------------

MimeHeader::MimeHeader(const MimeHeader& hdr) : m_index(),
    m_items(),
    m_parsePosition(-1),
    m_parseResult(true)
{
    copy(hdr);
}

//-------------------------------------------------------------------------------------------

MimeHeader::~MimeHeader()
{}

//-------------------------------------------------------------------------------------------

void MimeHeader::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "MimeHeader::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool MimeHeader::parse(const tchar *str)
{
    common::BString s(str);
    return parse(s);
}

//-------------------------------------------------------------------------------------------

bool MimeHeader::parse(const QString& str)
{
    common::BString s(str.toUtf8().constData());
    return parse(s);
}

//-------------------------------------------------------------------------------------------

bool MimeHeader::parse(const common::BString& str,tint i)
{
    MimeHeaderItem itemA;
    bool loop = true,res = true;

    blank();

    while(loop)
    {
        if(i<str.length())
        {
            itemA.m_parseResult = itemA.parse(str,i);
            if(itemA.m_parseResult)
            {
                if(!itemA.isHeaderEnd())
                {
                    m_index.insert(itemA.name().LCase(),m_items.size());
                    m_items.append(itemA);
                }
                else
                {
                    res = true;
                    loop = false;
                }
            }
            else
            {
                printError("parse","Error parsing mime header item");
                loop = false;
            }
            i = m_parsePosition = itemA.m_parsePosition;
        }
        else
        {
            loop = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool MimeHeader::isValid() const
{
    if(m_parseResult && m_items.size()>0)
    {
        tint i;

        for(i=0;i<m_items.size();++i)
        {
            if(!m_items.at(i).isValid())
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------------------------

tint MimeHeader::parseEndPosition() const
{
    return m_parsePosition;
}

//-------------------------------------------------------------------------------------------

void MimeHeader::blank()
{
    m_parsePosition = -1;
    m_parseResult = true;
    m_items.clear();
    m_index.clear();
}

//-------------------------------------------------------------------------------------------

void MimeHeader::copy(const MimeHeader& hdr)
{
    m_parsePosition = hdr.m_parsePosition;
    m_parseResult = hdr.m_parseResult;
    m_items = hdr.m_items;
    m_index = hdr.m_index;
}

//-------------------------------------------------------------------------------------------

void MimeHeader::compile(common::BString& out) const
{
    common::BString str;
    common::BStringCollection x;
    QList<MimeHeaderItem>::const_iterator ppI;

    for(ppI=m_items.begin();ppI!=m_items.end();++ppI)
    {
        const MimeHeaderItem& item = *ppI;
        str = item;
        x << str;
    }
    x << common::c_endl;
    x.Group(out);
}

//-------------------------------------------------------------------------------------------

void MimeHeader::compile(QString& out) const
{
    common::BString o;
    compile(o);
    out = QString::fromUtf8(static_cast<const tchar *>(o));
}

//-------------------------------------------------------------------------------------------

void MimeHeader::equal(const common::BString& str,tint i)
{
    m_parseResult = parse(str,i);
}

//-------------------------------------------------------------------------------------------

const MimeHeader& MimeHeader::operator = (const tchar *str)
{
    m_parseResult = parse(str);
    return *this;
}

//-------------------------------------------------------------------------------------------

const MimeHeader& MimeHeader::operator = (const common::BString& str)
{
    m_parseResult = parse(str);
    return *this;
}

//-------------------------------------------------------------------------------------------

const MimeHeader& MimeHeader::operator = (const QString& str)
{
    m_parseResult = parse(str);
    return *this;
}

//-------------------------------------------------------------------------------------------

const MimeHeader& MimeHeader::operator = (const MimeHeader& hdr)
{
    if(this!=&hdr)
    {
        copy(hdr);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

bool MimeHeader::add(const tchar *str)
{
    MimeHeaderItem hdr(str);
    return add(hdr);
}

//-------------------------------------------------------------------------------------------

bool MimeHeader::add(const common::BString& str)
{
    MimeHeaderItem hdr(str);
    return add(hdr);
}

//-------------------------------------------------------------------------------------------

bool MimeHeader::add(const QString& str)
{
    MimeHeaderItem hdr(str);
    return add(hdr);
}

//-------------------------------------------------------------------------------------------

bool MimeHeader::add(const MimeHeaderItem& hdr)
{
    if(hdr.isValid())
    {
        m_index.insert(hdr.name().LCase(),m_items.size());
        m_items.append(hdr);
    }
    return false;
}

//-------------------------------------------------------------------------------------------

const MimeHeaderItem& MimeHeader::item(tint idx) const
{
    if(idx>=0 && idx<m_items.size())
    {
        return m_items.at(idx);
    }
    else
    {
        return m_null;
    }
}

//-------------------------------------------------------------------------------------------

const MimeHeaderItem& MimeHeader::item(const tchar *str) const
{
    common::BString s(str);
    return item(s);
}

//-------------------------------------------------------------------------------------------

const MimeHeaderItem& MimeHeader::item(const common::BString& str) const
{
    common::BString idx;
    QMap<common::BString,tint>::const_iterator ppI;

    idx = str.LCase().Trim();
    ppI = m_index.find(idx);
    if(ppI!=m_index.end())
    {
        return m_items.at(ppI.value());
    }
    else
    {
        return m_null;
    }
}

//-------------------------------------------------------------------------------------------

const MimeHeaderItem& MimeHeader::item(const QString& str) const
{
    common::BString s(str.toUtf8().constData());
    return item(s);
}

//-------------------------------------------------------------------------------------------

tint MimeHeader::size() const
{
    return m_items.size();
}

//-------------------------------------------------------------------------------------------

MimeHeader::operator common::BString () const
{
    common::BString str;
    compile(str);
    return str;
}

//-------------------------------------------------------------------------------------------

MimeHeader::operator QString () const
{
    QString str;
    compile(str);
    return str;
}

//-------------------------------------------------------------------------------------------
} // namespace mime
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
