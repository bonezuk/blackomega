#include "network/http/inc/Query.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

Query::Query() : m_order(),
    m_header(),
    m_data(),
    m_blank()
{}

//-------------------------------------------------------------------------------------------

Query::Query(const tchar *s) : m_order(),
    m_header(),
    m_data(),
    m_blank()
{
    parse(s);
}

//-------------------------------------------------------------------------------------------

Query::Query(const QString& s) : m_order(),
    m_header(),
    m_data(),
    m_blank()
{
    parse(s.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

Query::Query(const Query& rhs) : m_order(),
    m_header(),
    m_data(),
    m_blank()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

void Query::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Query::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void Query::clear()
{
    m_order.clear();
    m_header.clear();
    m_data.clear();
}

//-------------------------------------------------------------------------------------------

void Query::copy(const Query& rhs)
{
    m_order = rhs.m_order;
    m_header = rhs.m_header;
    m_data = rhs.m_data;
}

//-------------------------------------------------------------------------------------------

void Query::queryGenerateMask(tint *mask) const
{
    tint i,j;
    tchar x;
    const tchar a[9] = { '-' , '_' , '.' , '!' , '~' , '*' , '\'' , '(' , ')' };
    
    if(mask!=0)
    {
        for(i=-128;i<128;++i)
        {
            x = static_cast<tchar>(i);
            if(x>='0' && x<='9')
            {
                mask[i + 128] = 1;
            }
            else if(x>='a' && x<='z')
            {
                mask[i + 128] = 1;
            }
            else if(x>='A' && x<='Z')
            {
                mask[i + 128] = 1;
            }
            else
            {
                for(j=0;j<9 && x!=a[j];++j) ;
                if(j<9)
                {
                    mask[i + 128] = 1;
                }
                else
                {
                    mask[i + 128] = 0;
                }
            }
        }
    }
    else
    {
        printError("qGenerateMask","No mask array given");
    }
}

//-------------------------------------------------------------------------------------------

void Query::queryAddEscapeCharacter(tchar x,QString& out) const
{
    tuint xUpper,xLower;
    tchar y[4] = {'%','0','0','\0'};
    
    xUpper = static_cast<tuint>(static_cast<tuchar>(x));
    xLower = xUpper & 0x0000000f;
    xUpper = (xUpper >> 4) & 0x0000000f;
    if(xLower>=10)
    {
        y[2] = static_cast<tchar>(static_cast<tuchar>(xLower - 10)) + 'a';
    }
    else
    {
        y[2] = static_cast<tchar>(static_cast<tuchar>(xLower)) + '0';
    }
    if(xUpper>=10)
    {
        y[1] = static_cast<tchar>(static_cast<tuchar>(xUpper - 10)) + 'a';
    }
    else
    {
        y[1] = static_cast<tchar>(static_cast<tuchar>(xUpper)) + '0';
    }
    out += y;
}

//-------------------------------------------------------------------------------------------

void Query::queryString(const QString& in,QString& out) const
{
    static tint cmask[256], *mask = 0;
    
    tint i,x;
    QByteArray mem(in.toUtf8());
    const tchar *s = mem.constData();
    
    out = "";
    
    if(mask==0)
    {
        queryGenerateMask(cmask);
        mask = &cmask[128];
    }
    
    for(i=0;i<mem.length();++i)
    {
        x = static_cast<tint>(s[i]);
        if(mask[x])
        {
            out += s[i];
        }
        else
        {
            if(s[i]==' ')
            {
                out += '+';
            }
            else
            {
                queryAddEscapeCharacter(s[i],out);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Query::parse(const tchar *s)
{
    tint i=0,j=0,state=0;
    tuint x=0;
    QString h,d,index;
    
    clear();
    
    if(s==0)
    {
        return;
    }
    
    while(s[i]!='\0')
    {
        switch(state)
        {
            case 0:
                if(s[i]!='&')
                {
                    if(s[i]=='%')
                    {
                        x = 0;
                        j = i;
                        state = 1;
                    }
                    else if(s[i]=='=')
                    {
                        h = h.trimmed();
                        state = 3;
                    }
                    else if(s[i]=='+')
                    {
                        h += ' ';
                    }
                    else
                    {
                        h += s[i];
                    }
                }
                break;
                
            case 3:
                if(s[i]!='=')
                {
                    if(s[i]=='%')
                    {
                        x = 0;
                        j = i;
                        state = 4;
                    }
                    else if(s[i]=='&')
                    {
                        index = h.toLower();
                        m_header.insert(index,h);
                        m_data.insert(index,d);
                        h.clear();
                        d.clear();
                        state = 0;
                    }
                    else if(s[i]=='+')
                    {
                        d += ' ';
                    }
                    else
                    {
                        d += s[i];
                    }
                }
                break;
                
            case 1:
            case 2:
            case 4:
            case 5:
                x = (x << 4) & 0x000000f0;
                if(s[i]>='0' && s[i]<='9')
                {
                    x |= static_cast<tuint>(static_cast<tuchar>(s[i] - '0'));
                }
                else if(s[i]>='a' && s[i]<='f')
                {
                    x |= 10 + static_cast<tuint>(static_cast<tuchar>(s[i] - 'a'));
                }
                else if(s[i]>='A' && s[i]<='F')
                {
                    x |= 10 + static_cast<tuint>(static_cast<tuchar>(s[i] - 'A'));
                }
                else
                {
                    i = j;
                    state = (state>=3) ? 2 : -1;
                }
                state++;
                
                if(state==3)
                {
                    h += static_cast<tchar>(static_cast<tuchar>(x));
                    state = 0;
                }
                else if(state==6)
                {
                    d += static_cast<tchar>(static_cast<tuchar>(x));
                    state = 3;
                }
                break;
                
            default:
                break;
        }
        i++;
    }
    
    if(state==3)
    {
        index = h.toLower();
        m_header.insert(index,h);
        m_data.insert(index,d);
    }
}

//-------------------------------------------------------------------------------------------

void Query::generate(QString& out) const
{
    tint i=0,j;
    QStringList x;
    QString h,d,s,index;
    QSet<QString> dIndex;
    QMap<QString,QString>::const_iterator ppI,ppJ;
    QSet<QString>::const_iterator ppK;
    
    for(j=0;j<m_order.size();++j)
    {
        index = m_order.at(j);
        ppI = m_header.find(index);
        ppJ = m_data.find(index);
        if(ppI!=m_header.end() && ppJ!=m_data.end())
        {
            dIndex.insert(index);
            queryString(ppI.value(),s);
            if(i)
            {
                x << QString::fromLatin1("&");
            }
            x << s << QString::fromLatin1("=");
            queryString(ppJ.value(),s);
            x << s;
            i++;
        }
    }
    
    for(ppI=m_header.begin();ppI!=m_header.end();++ppI)
    {
        index = ppI.key();
        ppK = dIndex.find(index);
        if(ppK==dIndex.end())
        {
            ppJ = m_data.find(index);
            if(ppJ!=m_data.end())
            {
                queryString(ppI.value(),s);
                if(i)
                {
                    x << QString::fromLatin1("&");
                }
                x << s << QString::fromLatin1("=");
                queryString(ppJ.value(),s);
                x << s;
                i++;
            }
        }
    }
    
    out = x.join(QString());
}

//-------------------------------------------------------------------------------------------

void Query::string(QString& str) const
{
    generate(str);
}

//-------------------------------------------------------------------------------------------

QString Query::value() const
{
    QString str;
    generate(str);
    return str;
}

//-------------------------------------------------------------------------------------------

const Query& Query::operator = (const tchar *s)
{
    parse(s);
    return *this;
}

//-------------------------------------------------------------------------------------------

const Query& Query::operator = (const QString& s)
{
    parse(s.toUtf8());
    return *this;
}

//-------------------------------------------------------------------------------------------

const Query& Query::operator = (const Query& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void Query::add(const tchar *h,const tchar *d)
{
    QString hS(h),dS(d);
    add(hS,dS);
}

//-------------------------------------------------------------------------------------------

void Query::add(const tchar *h,const QString& d)
{
    QString hS(h);
    add(hS,d);
}

//-------------------------------------------------------------------------------------------

void Query::add(const QString& h,const tchar *d)
{
    QString dS(d);
    add(h,dS);
}

//-------------------------------------------------------------------------------------------

void Query::add(const QString& h,const QString& d)
{
    QString index(h.toLower().trimmed());
    m_order.append(index);
    m_header.insert(index,h);
    m_data.insert(index,d);
}

//-------------------------------------------------------------------------------------------

const QString& Query::data(const tchar *idx) const
{
    return data(QString::fromUtf8(idx));
}

//-------------------------------------------------------------------------------------------

const QString& Query::data(const QString& idx) const
{
    QString index(idx.toLower().trimmed());
    QMap<QString,QString>::const_iterator ppI,ppJ;
    
    ppI = m_data.find(index);
    if(ppI!=m_data.end())
    {
        return ppI.value();
    }
    else
    {
        return m_blank;
    }
}

//-------------------------------------------------------------------------------------------

void Query::map(QMap<QString,QString>& m) const
{
    QMap<QString,QString>::const_iterator ppI,ppJ;
    
    m.clear();
    
    for(ppI=m_header.begin();ppI!=m_header.end();++ppI)
    {
        ppJ = m_data.find(ppI.key());
        if(ppJ!=m_data.end())
        {
            m.insert(ppI.value(),ppJ.value());
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
