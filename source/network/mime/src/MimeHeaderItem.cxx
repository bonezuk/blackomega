#include "network/mime/inc/MimeHeaderItem.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace mime
{
//-------------------------------------------------------------------------------------------

MimeHeaderItem::MimeHeaderItem() : m_name(),
    m_data(),
    m_parameters(),
    m_pOrder(),
    m_enumPosition(0),
    m_parseResult(true),
    m_parseEnd(false),
    m_parsePosition(-1)
{}

//-------------------------------------------------------------------------------------------

MimeHeaderItem::MimeHeaderItem(const tchar *str) : m_name(),
    m_data(),
    m_parameters(),
    m_pOrder(),
    m_enumPosition(0),
    m_parseResult(true),
    m_parseEnd(false),
    m_parsePosition(-1)
{
    m_parseResult = parse(str);
}

//-------------------------------------------------------------------------------------------

MimeHeaderItem::MimeHeaderItem(const common::BString& str) : m_name(),
    m_data(),
    m_parameters(),
    m_pOrder(),
    m_enumPosition(0),
    m_parseResult(true),
    m_parseEnd(false),
    m_parsePosition(-1)
{
    m_parseResult = parse(str);
}

//-------------------------------------------------------------------------------------------

MimeHeaderItem::MimeHeaderItem(const QString& str) : m_name(),
    m_data(),
    m_parameters(),
    m_pOrder(),
    m_enumPosition(0),
    m_parseResult(true),
    m_parseEnd(false),
    m_parsePosition(-1)
{
    m_parseResult = parse(str);
}

//-------------------------------------------------------------------------------------------

MimeHeaderItem::MimeHeaderItem(const MimeHeaderItem& hdr) : m_name(),
    m_data(),
    m_parameters(),
    m_pOrder(),
    m_enumPosition(0),
    m_parseResult(true),
    m_parseEnd(false),
    m_parsePosition(-1)
{
    copy(hdr);
}

//-------------------------------------------------------------------------------------------

MimeHeaderItem::~MimeHeaderItem()
{}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "MimeHeaderItem::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool MimeHeaderItem::parse(const tchar *str)
{
    common::BString line(str);
    return parse(line);
}

//-------------------------------------------------------------------------------------------

bool MimeHeaderItem::parse(const QString& str)
{
    common::BString line(str.toUtf8().constData());
    return parse(line);
}

//-------------------------------------------------------------------------------------------

bool MimeHeaderItem::parse(const common::BString& str,tint i)
{
    tint state = 0,eolstate = 0,start = i;
    const tchar *s = static_cast<const tchar *>(str);
    bool res = false;
    common::BString paramName,paramData;
    
    blank();
    
    if(str.isEmpty() && i<str.length())
    {
        printError("parse","No mime header data given");
        return false;
    }
    
    while(s[i]!='\0' && eolstate<2)
    {
        if(eolstate)
        {
            eolstate = (s[i]=='\n') ? 2 : 0;
            if(s[i+1]==' ' || s[i+1]=='\t')
            {
                eolstate = 0;
            }
            else
            {
                m_parsePosition = i + 1;
                i--;
            }
        }
        else
        {
            if(s[i]!='\r')
            {
                if(s[i]!='\n')
                {
                    switch(state)
                    {
                        case 0:
                            if(s[i]==':')
                            {
                                if(i>start)
                                {
                                    m_name = str.Sub(static_cast<tuint>(start),static_cast<tuint>(i - start)).Trim();
                                    start = i + 1;
                                    state = 1;
                                }
                                else
                                {
                                    printError("parse","No header name given");
                                    eolstate = 3;
                                }
                            }
                            break;
                            
                        case 1:
                            if(s[i]==';')
                            {
                                if(i>start)
                                {
                                    m_data = str.Sub(static_cast<tuint>(start),static_cast<tuint>(i - start)).Trim();
                                    start = i + 1;
                                    state = 2;
                                }
                                else
                                {
                                    printError("parse","No header data found");
                                    eolstate = 3;
                                }
                            }
                            break;
                            
                        case 2:
                            if(s[i]=='=')
                            {
                                if(i>start)
                                {
                                    paramName = str.Sub(static_cast<tuint>(start),static_cast<tuint>(i - start)).Trim();
                                    start = i + 1;
                                    state = 3;
                                }
                                else
                                {
                                    state = 4;
                                }
                            }
                            break;
                            
                        case 3:
                            if(s[i]=='\"')
                            {
                                state = 5;
                            }
                            else if(s[i]==';')
                            {
                                if(i>start)
                                {
                                    paramData = str.Sub(static_cast<tuint>(start),static_cast<tuint>(i - start)).Trim();
                                    m_parameters.insert(paramName,paramData);
                                    m_pOrder.append(paramName);
                                    start = i + 1;
                                }
                                state = 2;
                            }
                            break;
                        
                        case 4:
                            if(s[i]==';')
                            {
                                state = 2;
                            }
                            break;
                            
                        case 5:
                            if(s[i]=='\"')
                            {
                                state = 3;
                            }
                            break;
                            
                        default:
                            eolstate = 3;
                            break;
                    }
                }
                else
                {
                    m_parsePosition = i + 1;
                    eolstate = 2;
                }
            }
            else
            {
                eolstate = 1;
            }
        }
            
        if(eolstate==1 || eolstate==2)
        {
            if(s[i+1]==' ' || s[i+1]=='\t')
            {
                eolstate = 0;
            }
        }
            
        if(eolstate==2)
        {
            switch(state)
            {
                case 0:
                    if(i>start)
                    {
                        paramName = str.Sub(static_cast<tuint>(start),static_cast<tuint>(i - start)).Trim();
                        if(paramName.isEmpty())
                        {
                            res = true;
                            m_parseEnd = true;
                        }
                        else
                        {
                            res = false;
                        }
                    }
                    else
                    {
                        res = true;
                        m_parseEnd = true;
                    }
                    break;
                    
                case 1:
                    if(i>start)
                    {
                        m_data = str.Sub(static_cast<tuint>(start),static_cast<tuint>(i - start)).Trim();
                        if(!m_data.isEmpty())
                        {
                            res = true;
                        }
                        else
                        {
                            res = false;
                        }
                    }
                    else
                    {
                        printError("parse","No header data found");
                    }
                    break;
                    
                case 3:
                    paramData = str.Sub(static_cast<tuint>(start),static_cast<tuint>(i - start)).Trim();
                    m_parameters.insert(paramName,paramData);
                    m_pOrder.append(paramName);
                    res = true;
                    break;
                    
                default:
                    res = true;
                    break;
            }
        }
        i++;
    }
    
    return res;
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::blank()
{
    m_name = "";
    m_data = "";
    m_enumPosition = 0;
    m_parseResult = true;
    m_parseEnd = false;
    m_parsePosition = -1;
    m_parameters.clear();
    m_pOrder.clear();
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::copy(const MimeHeaderItem& hdr)
{
    m_name = hdr.m_name;
    m_data = hdr.m_data;
    m_enumPosition = hdr.m_enumPosition;
    m_parseResult = hdr.m_parseResult;
    m_parseEnd = hdr.m_parseEnd;
    m_parsePosition = hdr.m_parsePosition;
    m_parameters = hdr.m_parameters;
    m_pOrder = hdr.m_pOrder;
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::compile(common::BString& out) const
{
    common::BString paramName,paramData;
    QList<common::BString>::const_iterator ppI;
    QMap<common::BString,common::BString>::const_iterator ppJ;
    
    if(m_parseResult)
    {
        out << m_name << ": " << m_data;
        
        for(ppI=m_pOrder.begin();ppI!=m_pOrder.end();++ppI)
        {
            ppJ = m_parameters.find(*ppI);
            if(ppJ!=m_parameters.end())
            {
                out << "; " << ppJ.key() << "=" << ppJ.value();
            }
        }
    }
    out << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::compile(QString& out) const
{
    common::BString o;
    compile(o);
    out = QString::fromUtf8(static_cast<const tchar *>(o));
}

//-------------------------------------------------------------------------------------------

bool MimeHeaderItem::compare(const MimeHeaderItem& hdr) const
{
    QMap<common::BString,common::BString>::const_iterator ppI,ppJ;
    bool res = false;
    
    if(m_parseResult==hdr.m_parseResult && m_parseResult)
    {
        if(m_name==hdr.m_name)
        {
            if(m_data==hdr.m_data)
            {
                {
                    res = true;
                    
                    for(ppI=m_parameters.begin();ppI!=m_parameters.end() && res;++ppI)
                    {
                        ppJ = hdr.m_parameters.find(ppI.key());
                        if(ppJ!=hdr.m_parameters.end())
                        {
                            if(ppI.value()!=ppJ.value())
                            {
                                res = false;
                            }
                        }
                        else
                        {
                            res = false;
                        }
                    }
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = (m_parseResult) ? false : true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

const MimeHeaderItem& MimeHeaderItem::operator = (const tchar *str)
{
    m_parseResult = parse(str);
    return *this;
}

//-------------------------------------------------------------------------------------------

const MimeHeaderItem& MimeHeaderItem::operator = (const common::BString& str)
{
    m_parseResult = parse(str);
    return *this;
}

//-------------------------------------------------------------------------------------------

const MimeHeaderItem& MimeHeaderItem::operator = (const QString& str)
{
    m_parseResult = parse(str);
    return *this;
}

//-------------------------------------------------------------------------------------------

const MimeHeaderItem& MimeHeaderItem::operator = (const MimeHeaderItem& hdr)
{
    if(&hdr!=this)
    {
        copy(hdr);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

const common::BString& MimeHeaderItem::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::setName(const tchar *str)
{
    m_name = str;
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::setName(const common::BString& str)
{
    m_name = str;
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::setName(const QString& str)
{
    m_name = str.toUtf8().constData();
}

//-------------------------------------------------------------------------------------------

const common::BString& MimeHeaderItem::data() const
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::setData(const tchar *str)
{
    m_data = str;
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::setData(const common::BString& str)
{
    m_data = str;
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::setData(const QString& str)
{
    m_data = str.toUtf8().constData();
}

//-------------------------------------------------------------------------------------------

const common::BString& MimeHeaderItem::parameter(const tchar *str) const
{
    common::BString s(str);
    return parameter(s);
}

//-------------------------------------------------------------------------------------------

const common::BString& MimeHeaderItem::parameter(const common::BString& str) const
{
    QMap<common::BString,common::BString>::const_iterator ppI = m_parameters.find(str);
    
    if(ppI!=m_parameters.end())
    {
        return ppI.value();
    }
    else
    {
        return common::BString::null();
    }
}

//-------------------------------------------------------------------------------------------

const common::BString& MimeHeaderItem::parameter(const QString& str) const
{
    common::BString s(str.toUtf8().constData());
    return parameter(s);
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::add(const tchar *index,const tchar *data)
{
    common::BString i(index),d(data);
    add(i,d);
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::add(const tchar *index,const common::BString& data)
{
    common::BString i(index);
    add(i,data);
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::add(const tchar *index,const QString& data)
{
    common::BString i(index),d(data.toUtf8().constData());
    add(i,d);
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::add(const common::BString& index,const tchar *data)
{
    common::BString d(data);
    add(index,d);
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::add(const common::BString& index,const common::BString& data)
{
    m_parameters.insert(index,data);
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::add(const common::BString& index,const QString& data)
{
    common::BString d(data.toUtf8().constData());
    add(index,d);
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::add(const QString& index,const tchar *data)
{
    common::BString i(index.toUtf8().constData()),d(data);
    add(i,d);
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::add(const QString& index,const common::BString& data)
{
    common::BString i(index.toUtf8().constData());
    add(i,data);
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::add(const QString& index,const QString& data)
{
    common::BString i(index.toUtf8().constData()),d(data.toUtf8().constData());
    add(i,d);
}

//-------------------------------------------------------------------------------------------

void MimeHeaderItem::enumParameters() const
{
    m_enumPosition = 0;
}

//-------------------------------------------------------------------------------------------

bool MimeHeaderItem::next(common::BString& index,common::BString& data) const
{
    if(m_enumPosition>=m_pOrder.size())
    {
        return false;
    }
    index = m_pOrder.at(m_enumPosition++);
    data = parameter(index);
    return true;
}

//-------------------------------------------------------------------------------------------

MimeHeaderItem::operator common::BString() const
{
    common::BString str;
    compile(str);
    return str;
}

//-------------------------------------------------------------------------------------------

MimeHeaderItem::operator QString() const
{
    QString str;
    compile(str);
    return str;
}

//-------------------------------------------------------------------------------------------

bool MimeHeaderItem::isValid() const
{
    bool res = true;
    
    if(m_parseResult)
    {
        if(m_name.isEmpty())
        {
            res = false;
        }
        if(m_data.isEmpty())
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool MimeHeaderItem::isHeaderEnd() const
{
    return m_parseEnd;
}

//-------------------------------------------------------------------------------------------

tint MimeHeaderItem::parseEndPosition() const
{
    return m_parsePosition;
}

//-------------------------------------------------------------------------------------------
// operator ==
//-------------------------------------------------------------------------------------------

bool operator == (const MimeHeaderItem& a,const tchar *b)
{
    MimeHeaderItem iB(b);
    return a.compare(iB);
}

//-------------------------------------------------------------------------------------------

bool operator == (const MimeHeaderItem& a,const common::BString& b)
{
    MimeHeaderItem iB(b);
    return a.compare(iB);
}

//-------------------------------------------------------------------------------------------

bool operator == (const MimeHeaderItem& a,const QString& b)
{
    MimeHeaderItem iB(b);
    return a.compare(iB);
}

//-------------------------------------------------------------------------------------------

bool operator == (const tchar *a,const MimeHeaderItem& b)
{
    MimeHeaderItem iA(a);
    return b.compare(iA);
}

//-------------------------------------------------------------------------------------------

bool operator == (const common::BString& a,const MimeHeaderItem& b)
{
    MimeHeaderItem iA(a);
    return b.compare(iA);
}

//-------------------------------------------------------------------------------------------

bool operator == (const QString& a,const MimeHeaderItem& b)
{
    MimeHeaderItem iA(a);
    return b.compare(iA);
}

//-------------------------------------------------------------------------------------------

bool operator == (const MimeHeaderItem& a,const MimeHeaderItem& b)
{
    return a.compare(b);
}

//-------------------------------------------------------------------------------------------
// operator !=
//-------------------------------------------------------------------------------------------

bool operator != (const MimeHeaderItem& a,const tchar *b)
{
    MimeHeaderItem iB(b);
    return (a.compare(iB)) ? false : true;
}

//-------------------------------------------------------------------------------------------

bool operator != (const MimeHeaderItem& a,const common::BString& b)
{
    MimeHeaderItem iB(b);
    return (a.compare(iB)) ? false : true;
}

//-------------------------------------------------------------------------------------------

bool operator != (const MimeHeaderItem& a,const QString& b)
{
    MimeHeaderItem iB(b);
    return (a.compare(iB)) ? false : true;
}

//-------------------------------------------------------------------------------------------

bool operator != (const tchar *a,const MimeHeaderItem& b)
{
    MimeHeaderItem iA(a);
    return (b.compare(iA)) ? false : true;
}

//-------------------------------------------------------------------------------------------

bool operator != (const common::BString& a,const MimeHeaderItem& b)
{
    MimeHeaderItem iA(a);
    return (b.compare(iA)) ? false : true;
}

//-------------------------------------------------------------------------------------------

bool operator != (const QString& a,const MimeHeaderItem& b)
{
    MimeHeaderItem iA(a);
    return (b.compare(iA)) ? false : true;
}

//-------------------------------------------------------------------------------------------

bool operator != (const MimeHeaderItem& a,const MimeHeaderItem& b)
{
    return (a.compare(b)) ? false : true;
}

//-------------------------------------------------------------------------------------------
} // namespace mime
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
