#include "network/http/inc/Unit.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------
// Unit::Language
//-------------------------------------------------------------------------------------------

Unit::Language::Language() : m_unitLang(),
    m_dateLang(),
    m_urlLang(),
    m_mutex()
{
    m_unitState[ 0] = m_unitLang.String("http/");
    m_unitState[ 1] = m_unitLang.String("OPTIONS");
    m_unitState[ 2] = m_unitLang.String("GET");
    m_unitState[ 3] = m_unitLang.String("HEAD");
    m_unitState[ 4] = m_unitLang.String("POST");
    m_unitState[ 5] = m_unitLang.String("PUT");
    m_unitState[ 6] = m_unitLang.String("DELETE");
    m_unitState[ 7] = m_unitLang.String("TRACE");
    m_unitState[ 8] = m_unitLang.String("CONNECT");
    m_unitState[ 9] = m_unitLang.String(":");
    m_unitState[10] = m_unitLang.String(".");
    m_unitState[11] = m_unitLang.String("?");

    //Initialize date symbolic language parser.
    m_dateState[0]  = m_dateLang.String("Sun");
    m_dateState[1]  = m_dateLang.String("Mon");
    m_dateState[2]  = m_dateLang.String("Tue");
    m_dateState[3]  = m_dateLang.String("Wed");
    m_dateState[4]  = m_dateLang.String("Thu");
    m_dateState[5]  = m_dateLang.String("Fri");
    m_dateState[6]  = m_dateLang.String("Sat");

    m_dateState[7]  = m_dateLang.String("Sunday");
    m_dateState[8]  = m_dateLang.String("Monday");
    m_dateState[9]  = m_dateLang.String("Tuesday");
    m_dateState[10] = m_dateLang.String("Wednesday");
    m_dateState[11] = m_dateLang.String("Thursday");
    m_dateState[12] = m_dateLang.String("Friday");
    m_dateState[13] = m_dateLang.String("Saturday");

    m_dateState[14] = m_dateLang.String("Jan");
    m_dateState[15] = m_dateLang.String("Feb");
    m_dateState[16] = m_dateLang.String("Mar");
    m_dateState[17] = m_dateLang.String("Apr");
    m_dateState[18] = m_dateLang.String("May");
    m_dateState[19] = m_dateLang.String("Jun");
    m_dateState[20] = m_dateLang.String("Jul");
    m_dateState[21] = m_dateLang.String("Aug");
    m_dateState[22] = m_dateLang.String("Sep");
    m_dateState[23] = m_dateLang.String("Oct");
    m_dateState[24] = m_dateLang.String("Nov");
    m_dateState[25] = m_dateLang.String("Dec");

    m_dateState[26] = m_dateLang.String(",");
    m_dateState[27] = m_dateLang.String(":");
    m_dateState[28] = m_dateLang.String("-");
    m_dateState[29] = m_dateLang.String("GMT");

    m_urlState[0]   = m_urlLang.String("http://");
    m_urlState[1]   = m_urlLang.String("/");
    m_urlState[2]   = m_urlLang.String(":");
    m_urlState[3]   = m_urlLang.String("?");
    m_urlState[4]   = m_urlLang.String("https://");    
}

//-------------------------------------------------------------------------------------------

void Unit::Language::lock()
{
    m_mutex.lock();
}

//-------------------------------------------------------------------------------------------

void Unit::Language::unlock()
{
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------
// Unit
//-------------------------------------------------------------------------------------------

Unit::Language Unit::m_language;

//-------------------------------------------------------------------------------------------

Unit::Unit() : m_type(e_Unknown),
    m_versionMajor(1),
    m_versionMinor(1),
    m_request(e_ReqUnknown),
    m_resource(),
    m_query(),
    m_code(0),
    m_reason(),
    m_header(),
    m_data(),
    m_time(),
    m_proxyFlag(false),
    m_host(),
    m_port(80),
    m_username(),
    m_password(),
    m_lastIndex(),
    m_blank()
{
    m_time = common::TimeStamp::now();
}

//-------------------------------------------------------------------------------------------

Unit::Unit(const Unit& rhs) : m_type(e_Unknown),
    m_versionMajor(1),
    m_versionMinor(1),
    m_request(e_ReqUnknown),
    m_resource(),
    m_query(),
    m_code(0),
    m_reason(),
    m_header(),
    m_data(),
    m_time(),
    m_proxyFlag(false),
    m_host(),
    m_port(80),
    m_username(),
    m_password(),
    m_lastIndex(),
    m_blank()
{
    m_time = common::TimeStamp::now();
    Unit::copy(rhs);
}

//-------------------------------------------------------------------------------------------

Unit::~Unit()
{}

//-------------------------------------------------------------------------------------------

void Unit::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Unit::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void Unit::copy(const Unit& rhs)
{
    m_type = rhs.m_type;
    m_versionMajor = rhs.m_versionMinor;
    m_versionMinor = rhs.m_versionMinor;
    m_request = rhs.m_request;
    m_resource = rhs.m_resource;
    m_query = rhs.m_query;
    m_code = rhs.m_code;
    m_reason = rhs.m_reason;
    m_header = rhs.m_header;
    m_data = rhs.m_data;
    m_time = rhs.m_time;
    m_proxyFlag = rhs.m_proxyFlag;
    m_host = rhs.m_host;
    m_port = rhs.m_port;
    m_username = rhs.m_username;
    m_password = rhs.m_password;
    m_lastIndex = rhs.m_lastIndex;
}

//-------------------------------------------------------------------------------------------

const Unit& Unit::operator = (const Unit& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

bool Unit::parseHTTPVersion(const common::BString& line,common::BO_Parse_Unit **pItem,tint& end,tint& major,tint& minor)
{
    tint i,state=0,start=0;
    bool res = false;
    common::BO_Parse_Unit *item;
    const tchar *s = static_cast<const tchar *>(line);
    
    if(pItem==0 || line.isEmpty())
    {
        return false;
    }
    item = *pItem;
    
    while(state>=0 && item!=0)
    {
        switch(state)
        {
            case 0:
                if(item->state==m_language.m_unitState[0])
                {
                    state = 1;
                    start = item->start + item->length;
                }
                else
                {
                    state = -1;
                }
                break;
                
            case 1:
                if(item->state==m_language.m_unitState[10] && item->start>start)
                {
                    major = line.Sub(start,item->start - start).Atoi();
                    start = item->start + item->length;
                    
                    for(i=start;i<line.length() && s[i]>='0' && s[i]<='9';++i) ;
                    if(i > start)
                    {
                        minor = line.Sub(start,i - start).Atoi();
                    }
                    else
                    {
                        minor = 0;
                    }
                    end = i + 1;
                    res = true;
                }
                state = -1;
                break;
                
            default:
                state = -1;
                break;
        }
        if(state>=0)
        {
            item = item->next;
        }
    }
    
    if(res)
    {
        *pItem = item;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Unit::parseInLine(const common::BString& line)
{
    tint i=0,state=0,start=0,qposition = -1;
    common::BO_Parse_Unit *item;
    const tchar *s = static_cast<const tchar *>(line);
    QString index,cmd,data;
    RequestType type = e_ReqUnknown;
    bool res = false;
    
    m_language.lock();
    
    item = m_language.m_unitLang.Lexical(s);
    
    while(item!=0 && !res && state>=0)
    {
        if(item->state==m_language.m_unitState[9])
        {
            cmd = QString::fromUtf8(line.Sub(0,item->start).Trim().GetString());
            data = QString::fromUtf8(line.Sub(item->start + item->length).Trim().GetString());
            index = cmd.toLower();
        }
        
        switch(state)
        {
            case 0:
                {
                    if(item->state==m_language.m_unitState[0])
                    {
                        if(parseHTTPVersion(line,&item,i,m_versionMajor,m_versionMinor))
                        {
                            state = 2;
                            break;
                        }
                    }
                    else
                    {
                        for(i=1;i<9 && item->state!=m_language.m_unitState[i];++i) ;
                        if(i<9)
                        {
                            start = item->start + item->length;
                            switch(i)
                            {
                                case 1:
                                    type = e_Options;
                                    break;
                                    
                                case 2:
                                    type = e_Get;
                                    break;
                                    
                                case 3:
                                    type = e_Head;
                                    break;
                                    
                                case 4:
                                    type = e_Post;
                                    break;
                                    
                                case 5:
                                    type = e_Put;
                                    break;
                                    
                                case 6:
                                    type = e_Delete;
                                    break;
                                    
                                case 7:
                                    type = e_Trace;
                                    break;
                                    
                                case 8:
                                    type = e_Connect;
                                    break;
                                    
                                default:
                                    break;
                            }
                            state = 3;
                            break;
                        }
                        else
                        {
                            state = 1;
                        }
                    }
                }
                
            case 1:
                if(item->state==m_language.m_unitState[9])
                {
                    state = -1;
                }
                break;
                
            case 3:
                if(item->state==m_language.m_unitState[11])
                {
                    qposition = item->start;
                    state = 4;
                }
                
            case 4:
                if(item->state==m_language.m_unitState[0])
                {
                    m_type = e_Request;
                    m_request = type;
                    if(qposition>=0)
                    {
                        m_resource = QString::fromUtf8(line.Sub(start,qposition - start).GetString()).trimmed();
                        if(item->start > (qposition + 1))
                        {
                            m_query = QString::fromUtf8(line.Sub(qposition + 1,item->start - (qposition + 2)).GetString());
                        }
                        else
                        {
                            m_query.clear();
                        }
                    }
                    else
                    {
                        m_resource = QString::fromUtf8(line.Sub(start,item->start - start).GetString()).trimmed();
                        m_query.clear();
                    }
                    if(parseHTTPVersion(line,&item,i,m_versionMajor,m_versionMinor))
                    {
                        res = true;
                    }
                    else
                    {
                        state = -1;
                    }
                }
                break;
                
            default:
                break;
        }
        
        if(state==2)
        {
            m_type = e_Response;
            while(i<line.GetLength() && s[i]==' ')
            {
                i++;
            }
            if(i<line.GetLength())
            {
                for(start=i;i<line.GetLength() && s[i]>='0' && s[i]<='9';++i) ;
                m_code = line.Sub(start,i - start).Atoi();
                m_reason = QString::fromUtf8(line.Sub(i).Trim().GetString());
                res = true;
            }
            else
            {
                state = -1;
            }
        }
        
        if(item!=0)
        {
            item = item->next;
        }
    }
    
    if(!res && !index.isEmpty())
    {
        if(index=="date")
        {
            common::BString dt(data.toLatin1().constData());
            if(parseDate(dt,m_time))
            {
                res = true;
            }
        }
        else
        {
            m_lastIndex = index;
            m_header.insert(index,cmd);
            m_data.insert(index,data);
            res = true;
        }
    }
    
    m_language.unlock();
    return res;
}

//-------------------------------------------------------------------------------------------

tint Unit::asciiToInteger(const tchar *s,tint start,tint end) const
{
    tint i,x=0,state=0;
    
    if(s!=0)
    {
        for(i=start;i<end && state<2;++i)
        {
            if(s[i]>='0' && s[i]<='9')
            {
                x *= 10;
                x += static_cast<tint>(s[i] - '0');
                state = 1;
            }
            else if(s[i]==' ' && state==1)
            {
                state = 2;
            }
        }
    }
    return x;
}

//-------------------------------------------------------------------------------------------

bool Unit::parseDate_RFC1123(const common::BO_Parse_Unit *item,const tchar *s,common::TimeStamp& t) const
{
    tint i,state=0,start=0;
    bool flag,res=true;
    
    while(item!=0 && res && state<5)
    {
        switch(state)
        {
            case 0:
                if(item->state==m_language.m_dateState[26])
                {
                    start = item->start + item->length + 1;
                    state = 1;
                }
                else
                {
                    res = false;
                }
                break;
                
            case 1:
                flag = false;
                for(i=14;i<26 && !flag;++i)
                {
                    if(item->state==m_language.m_dateState[i])
                    {
                        t.month(i - 13);
                        flag = true;
                    }
                }
                if(!flag)
                {
                    res = false;
                }
                t.day(asciiToInteger(s,start,item->start));
                start = item->start + item->length;
                state = 2;
                break;
                
            case 2:
                if(item->state==m_language.m_dateState[27])
                {
                    t.year(asciiToInteger(s,start,item->start));
                    t.hour(asciiToInteger(s,item->start - 2,item->start));
                    start = item->start + item->length;
                    state = 3;
                }
                else
                {
                    res = false;
                }
                break;
                
            case 3:
                if(item->state==m_language.m_dateState[27])
                {
                    t.minute(asciiToInteger(s,start,item->start));
                    start = item->start + item->length;
                    state = 4;
                }
                else
                {
                    res = false;
                }
                break;
                
            case 4:
                if(item->state==m_language.m_dateState[29])
                {
                    t.second(asciiToInteger(s,start,item->start));
                    state = 5;
                }
                else
                {
                    res = false;
                }
                break;
            
            default:
                break;
        }
        item = item->next;
    }
    
    if(state!=5)
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Unit::parseDate_RFC1036(const common::BO_Parse_Unit *item,const tchar *s,common::TimeStamp& t) const
{
    tint i,state=0,start=0;
    bool flag,res=true;
    
    while(item!=0 && res && state<6)
    {
        switch(state)
        {
            case 0:
                if(item->state==m_language.m_dateState[26])
                {
                    start = item->start + item->length;
                    state = 1;
                }
                else
                {
                    res = false;
                }
                break;
                
            case 1:
                if(item->state==m_language.m_dateState[28])
                {
                    t.day(asciiToInteger(s,start,item->start));
                    state = 2;
                }
                else
                {
                    res = false;
                }
                break;
                
            case 2:
                flag = false;
                for(i=14;i<26 && !flag;++i)
                {
                    if(item->state==m_language.m_dateState[i])
                    {
                        t.month(i - 13);
                        flag = true;
                    }
                }
                if(flag)
                {
                    state = 3;
                }
                else
                {
                    res = false;
                }
                break;
                
            case 3:
                if(item->state==m_language.m_dateState[27])
                {
                    tint y = asciiToInteger(s,start,item->start+item->length);
                    if(y>=0 && y<50)
                    {
                        y += 100;
                    }
                    y += 1900;
                    t.year(y);
                    t.hour(asciiToInteger(s,item->start-2,item->start));
                    start = item->start + item->length;
                    state = 4;
                }
                else
                {
                    start = item->start + item->length;
                }
                break;
                
            case 4:
                if(item->state==m_language.m_dateState[27])
                {
                    t.minute(asciiToInteger(s,start,item->start));
                    start = item->start + item->length;
                    state = 5;
                }
                else
                {
                    res = false;
                }
                break;
                
            case 5:
                t.second(asciiToInteger(s,start,item->start));
                state = 6;
                break;
                
            default:
                break;
        }
        item = item->next;
    }
    
    if(state!=6)
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Unit::parseDate_ANSI(const common::BO_Parse_Unit *item,const tchar *s,common::TimeStamp& t) const
{
    tint i,state=0,start=0;
    bool flag,res=true;
    
    while(item!=0 && res && state<3)
    {
        switch(state)
        {
            case 0:
                flag = false;
                for(i=14;i<26 && !flag;++i)
                {
                    if(item->state==m_language.m_dateState[i])
                    {
                        t.month(i - 13);
                        flag = true;
                    }
                }
                if(flag)
                {
                    start = item->start + item->length;
                    state = 1;
                }
                else
                {
                    res = false;
                }
                break;
                
            case 1:
                if(item->state==m_language.m_dateState[27])
                {
                    t.day(asciiToInteger(s,start,item->start));
                    t.hour(asciiToInteger(s,item->start-2,item->start));
                    start = item->start + item->length;
                    state = 2;
                }
                else
                {
                    res = false;
                }
                break;
                
            case 2:
                if(item->state==m_language.m_dateState[27])
                {
                    t.minute(asciiToInteger(s,start,item->start));
                    start = item->start + item->length;
                    t.second(asciiToInteger(s,start,start+2));
                    t.year(asciiToInteger(s,start+2,start+7));
                    state = 3;
                }
                else
                {
                    res = false;
                }
                break;
                
            default:
                break;
        }
        item = item->next;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool Unit::parseDate(const common::BString& str,common::TimeStamp& t)
{
    tint i,state = 0;
    bool flag,res = true;
    common::BO_Parse_Unit *item;
    const tchar *s = static_cast<const tchar *>(str);
    
    item = m_language.m_dateLang.Lexical(s);
    
    while(item!=0 && res)
    {
        switch(state)
        {
            case 0:
                {
                    flag = false;
                    for(i=0;i<7 && !flag;++i)
                    {
                        if(item->state==m_language.m_dateState[i])
                        {
                            flag = true;
                            state = 1;
                        }
                    }
                    if(!flag)
                    {
                        while(i<14 && !flag)
                        {
                            if(item->state==m_language.m_dateState[i])
                            {
                                flag = true;
                                state = 2;
                            }
                            i++;
                        }
                    }
                    if(!flag)
                    {
                        res = false;
                    }
                }
                break;
                
            case 1:
                {
                    if(item->state==m_language.m_dateState[26])
                    {
                        res = parseDate_RFC1123(item,s,t);
                    }
                    else
                    {
                        res = parseDate_ANSI(item,s,t);
                    }
                    state = 3;
                }
                break;
                
            case 2:
                {
                    res = parseDate_RFC1036(item,s,t);
                    state = 3;
                }
                break;
                
            case 3:
            default:
                break;
        }
        item = item->next;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

Unit::Type Unit::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

void Unit::request(RequestType t)
{
    if(m_resource.isEmpty())
    {
        m_resource = "/";
    }
    m_type = e_Request;
    m_request = t;
}

//-------------------------------------------------------------------------------------------

void Unit::request(RequestType t,const tchar *r)
{
    request(t,QString::fromUtf8(r));
}

//-------------------------------------------------------------------------------------------

void Unit::request(RequestType t,const QString& r)
{
    m_type = e_Request;
    m_request = t;
    m_resource = r;
}

//-------------------------------------------------------------------------------------------

Unit::RequestType Unit::request() const
{
    if(m_type==e_Request)
    {
        return m_request;
    }
    else
    {
        return e_ReqUnknown;
    }
}

//-------------------------------------------------------------------------------------------

const QString& Unit::resource() const
{
    return m_resource;
}

//-------------------------------------------------------------------------------------------

void Unit::response(tint c)
{
    m_type = e_Response;
    m_code = c;
    m_reason = ResponseCodes::reason(c);
}

//-------------------------------------------------------------------------------------------

tint Unit::response() const
{
    return m_code;
}

//-------------------------------------------------------------------------------------------

const QString& Unit::reason() const
{
    return m_reason;
}

//-------------------------------------------------------------------------------------------

Query& Unit::query()
{
    return m_query;
}

//-------------------------------------------------------------------------------------------

const Query& Unit::query() const
{
    return m_query;
}

//-------------------------------------------------------------------------------------------

void Unit::query(const tchar *x)
{
    query(QString::fromUtf8(x));
}

//-------------------------------------------------------------------------------------------

void Unit::query(const QString& x)
{
    m_query = x;
}

//-------------------------------------------------------------------------------------------

void Unit::map(QMap<QString,QString>& m) const
{
    QMap<QString,QString>::const_iterator ppI,ppJ;
    
    m.clear();
    
    for(ppI=m_header.begin();ppI!=m_header.end();++ppI)
    {
        ppJ = m_data.find(ppI.value());
        if(ppJ!=m_data.end())
        {
            m.insert(ppI.value(),ppJ.value());
        }
    }
}

//-------------------------------------------------------------------------------------------

bool Unit::exist(const tchar *idx) const
{
    return exist(QString::fromUtf8(idx));
}

//-------------------------------------------------------------------------------------------

bool Unit::exist(const QString& idx) const
{
    QString index(idx.toLower().trimmed());
    QMap<QString,QString>::const_iterator ppI = m_data.find(index);
    return (ppI!=m_data.end()) ? true : false;
}

//-------------------------------------------------------------------------------------------

const QString& Unit::data(const tchar *idx) const
{
    return data(QString::fromUtf8(idx));
}

//-------------------------------------------------------------------------------------------

const QString& Unit::data(const QString& idx) const
{
    QString index(idx.toLower().trimmed());
    QMap<QString,QString>::const_iterator ppI = m_data.find(index);
    
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

void Unit::date(common::TimeStamp& t) const
{
    t = m_time;
}

//-------------------------------------------------------------------------------------------

void Unit::add(const common::TimeStamp& t)
{
    m_time = t;
}

//-------------------------------------------------------------------------------------------

void Unit::add(const tchar *h,const tchar *d)
{
    add(QString::fromUtf8(h),QString::fromUtf8(d));
}

//-------------------------------------------------------------------------------------------

void Unit::add(const QString& h,const tchar *d)
{
    add(h,QString::fromUtf8(d));
}

//-------------------------------------------------------------------------------------------

void Unit::add(const tchar *h,const QString& d)
{
    add(QString::fromUtf8(h),d);
}

//-------------------------------------------------------------------------------------------

void Unit::add(const QString& h,const QString& d)
{
    QString index(h.toLower().trimmed());
    m_header.insert(index,h);
    m_data.insert(index,d);
}

//-------------------------------------------------------------------------------------------

void Unit::remove(const tchar *x)
{
    remove(QString::fromUtf8(x));
}

//-------------------------------------------------------------------------------------------

void Unit::remove(const QString& x)
{
    QString index(x.toLower().trimmed());
    QMap<QString,QString>::iterator ppI;
    
    ppI = m_header.find(index);
    if(ppI!=m_header.end())
    {
        m_header.erase(ppI);
    }
    ppI = m_data.find(index);
    if(ppI!=m_data.end())
    {
        m_data.erase(ppI);
    }
}

//-------------------------------------------------------------------------------------------

void Unit::makeResourceLocal()
{
    tint start=0,state=0;
    common::BO_Parse_Unit *item;
    
    m_language.lock();
    item = m_language.m_urlLang.Lexical(m_resource.toUtf8().constData());
    while(item!=0 && state<2)
    {
        switch(state)
        {
            case 0:
                if(item->state==m_language.m_urlState[0] || item->state==m_language.m_urlState[4])
                {
                    state = 1;
                }
                else
                {
                    state = 3;
                }
                break;
                
            case 1:
                if(item->state==m_language.m_urlState[1])
                {
                    start = item->start;
                    state = 2;
                }
                break;
                
            default:
                break;
        }
        item = item->next;
    }
    m_language.unlock();
    
    if(state==1)
    {
        m_resource = "/";
    }
    else if(state==2)
    {
        m_resource = m_resource.mid(start);
    }
}

//-------------------------------------------------------------------------------------------

bool Unit::insertLine(const tchar *l,bool& end)
{
    common::BString line(l),str;
    const tchar *x = static_cast<const tchar *>(line);
    
    if(line.GetLength() > 0)
    {
        if(x[0]=='\t' || x[0]==' ')
        {
            tint i;
            
            for(i=0;i<line.GetLength() && (x[i]=='\t' || x[i]==' ');++i) ;
            str = line.Sub(static_cast<tuint>(i)).Trim();
            
            if(!str.IsEmpty())
            {
                if(!m_lastIndex.isEmpty())
                {
                    QMap<QString,QString>::iterator ppI = m_data.find(m_lastIndex);
                    
                    if(ppI!=m_data.end())
                    {
                        QString& vStr = ppI.value();
                        vStr += static_cast<const tchar *>(str);
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                makeResourceLocal();
                end = true;
                return true;
            }
        }
        else
        {
            str = line.Trim();
        }
    }
    else
    {
        makeResourceLocal();
        end = true;
        return true;
    }
    end = false;
    return parseInLine(str);
}

//-------------------------------------------------------------------------------------------

bool Unit::insertLine(const QString& l,bool& end)
{
    return insertLine(l.toUtf8().constData(),end);
}

//-------------------------------------------------------------------------------------------

void Unit::setVersion(tint major,tint minor)
{
    if(major==1)
    {
        m_versionMajor = 1;
        if(minor)
        {
            m_versionMinor = 1;
        }
        else
        {
            m_versionMinor = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------

void Unit::getVersion(tint& major,tint& minor) const
{
    major = m_versionMajor;
    minor = m_versionMinor;
}

//-------------------------------------------------------------------------------------------

void Unit::print(QString& str) const
{
    QString index;
    QMap<QString,QString>::const_iterator ppI,ppJ;
    
    str = "";
    
    if(m_type==e_Request)
    {
        switch(m_request)
        {
            case e_Options:
                str = "OPTIONS";
                break;
            
            case e_Get:
                str = "GET";
                break;
                
            case e_Head:
                str = "HEAD";
                break;

            case e_Post:
                str = "POST";
                break;

            case e_Put:
                str = "PUT";
                break;
                
            case e_Delete:
                str = "DELETE";
                break;
                
            case e_Trace:
                str = "TRACE";
                break;
                
            case e_Connect:
                str = "CONNECT";
                break;
                
            case e_ReqUnknown:
            default:
                return;
        }
        
        str += " ";
        if(m_proxyFlag)
        {
            str += "http://" + m_host;
            if(m_port!=80)
            {
                str += ":" + QString::number(m_port);
            }
        }
        else
        {
            str += m_resource;
        }
        m_query.string(index);
        if(!index.isEmpty())
        {
            str += "?" + index;
        }
        str += " HTTP/" + QString::number(m_versionMajor) + "." + QString::number(m_versionMinor);
    }
    else if(m_type==e_Response)
    {
        str = "HTTP/" + QString::number(m_versionMajor) + "." + QString::number(m_versionMinor) + " ";
        str += QString::number(m_code) + " " + m_reason;
    }
    else
    {
        return;
    }
    str += "\r\n";
    
    str += "Date: " + buildTimeStamp(m_time) + "\r\n";
    
    for(ppI=m_header.begin();ppI!=m_header.end();++ppI)
    {
        ppJ = m_data.find(ppI.key());
        if(ppJ!=m_data.end())
        {
            str += ppI.value() + ": " + ppJ.value() + "\r\n";
        }
    }
    
    if(!m_username.isEmpty() && !m_password.isEmpty())
    {
        QString userpass,baseUP;
        
        userpass = m_username + ":" + m_password;
        baseUP = QString::fromUtf8(userpass.toUtf8().toBase64());
        str += "Authorization: Basic " + baseUP + "\r\n";
    }
    
    str += "\r\n";
}

//-------------------------------------------------------------------------------------------

QString Unit::buildTimeStamp(const common::TimeStamp& t) const
{
    static const tchar *weekDays[7] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
    static const tchar *months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

    QString str;
    
    str += weekDays[ t.dayOfWeek() ];
    str += ", ";
    str += common::BString::Int(t.day(),2).GetString();
    str += " ";
    str += months[ t.month() - 1 ];
    str += " ";
    str += common::BString::Int(t.year(),4).GetString();
    str += " ";
    str += common::BString::Int(t.hour(),2).GetString();
    str += ":";
    str += common::BString::Int(t.minute(),2).GetString();
    str += ":";
    str += common::BString::Int(t.second(),2).GetString();
    str += " GMT";
    
    return str;
}

//-------------------------------------------------------------------------------------------

void Unit::clear()
{
    m_type = e_Unknown;
    m_versionMajor = 1;
    m_versionMinor = 1;
    m_request = e_ReqUnknown;
    m_resource = "";
    m_query.clear();
    m_code = 0;
    m_reason = "";
    m_header.clear();
    m_data.clear();
}

//-------------------------------------------------------------------------------------------

void Unit::setProxyConnection(const tchar *host,tint port)
{
    setProxyConnection(QString::fromUtf8(host),port);
}

//-------------------------------------------------------------------------------------------

void Unit::setProxyConnection(const QString& host,tint port)
{
    m_proxyFlag = true;
    m_host = host;
    m_port = port;
}

//-------------------------------------------------------------------------------------------

tint Unit::processURL(const QString& url,QString& dns,QString& path,QString& query)
{
    bool secure = false;
    return processURL(url,dns,path,query,secure);
}

//-------------------------------------------------------------------------------------------

tint Unit::processURL(const QString& url,QString& dns,QString& path,QString& query,bool& secure)
{
    common::BString u(url.toUtf8().constData());
    tint len = u.GetLength();
    tint sDNS = 0,eDNS = len;
    tint sPort = len,ePort = len;
    tint sPath = len,ePath = len;
    tint sQuery = len,eQuery = len;
    tint port = 80,state = 0;
    common::BO_Parse_Unit *item;
    
    path = "/";
    query = "";
    
    m_language.lock();
    item = m_language.m_urlLang.Lexical(static_cast<const tchar *>(u));
    if(item!=0)
    {
        while(item!=0)
        {
            switch(state)
            {
                case 0:
                    {
                        if(item->state==m_language.m_urlState[0] || item->state==m_language.m_urlState[4])
                        {
                            sDNS = item->start + item->length;
                            state = 1;
                            secure = (item->state==m_language.m_urlState[0]) ? false : true;
                            break;
                        }
                    }
                    
                case 1:
                    {
                        eDNS = item->start;
                        if(item->state==m_language.m_urlState[1])
                        {
                            sPath = item->start + item->length;
                            state = 3;
                        }
                        else if(item->state==m_language.m_urlState[2])
                        {
                            sPort = item->start + item->length;
                            state = 2;
                        }
                        else if(item->state==m_language.m_urlState[3])
                        {
                            sQuery = item->start + item->length;
                            state = 4;
                        }
                    }
                    break;
                    
                case 2:
                    {
                        ePort = item->start;
                        if(item->state==m_language.m_urlState[1])
                        {
                            sPath = item->start + item->length;
                            state = 3;
                        }
                        else if(item->state==m_language.m_urlState[3])
                        {
                            sQuery = item->start + item->length;
                            state = 4;
                        }
                    }
                    break;
                    
                case 3:
                    {
                        if(item->state==m_language.m_urlState[3])
                        {
                            ePath = item->start;
                            sQuery = item->start + item->length;
                            state = 4;
                        }
                    }
                    break;
                    
                default:
                    break;
            }
            item = item->next;
        }
        
        if(sDNS < eDNS)
        {
            dns = u.Sub(sDNS,eDNS - sDNS).GetString();
        }
        if(sPath < ePath)
        {
            path += u.Sub(sPath,ePath - sPath).GetString();
        }
        if(sQuery < eQuery)
        {
            query = u.Sub(sQuery,eQuery - sQuery).GetString();
        }
        if(sPort < ePort)
        {
            port = u.Sub(sPort,ePort - sPort).Atoi();
        }
        else
        {
            port = (secure) ? 443 : 80;
        }
    }
    else
    {
        dns = static_cast<const tchar *>(u);
    }
    m_language.unlock();
    
    return port;
}

//-------------------------------------------------------------------------------------------

void Unit::setUsername(const tchar *user)
{
    setUsername(QString::fromUtf8(user));
}

//-------------------------------------------------------------------------------------------

void Unit::setUsername(const QString& user)
{
    m_username = user;
}

//-------------------------------------------------------------------------------------------

void Unit::setPassword(const tchar *pass)
{
    setPassword(QString::fromUtf8(pass));
}

//-------------------------------------------------------------------------------------------

void Unit::setPassword(const QString& pass)
{
    m_password = pass;
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
