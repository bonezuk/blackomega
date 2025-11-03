#include "track/info/inc/CueParser.h"
#include "track/info/inc/XMCDInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------
// CueParser::Track
//-------------------------------------------------------------------------------------------

CueParser::Track::Track() : m_trackNo(0),
    m_name(),
    m_index0(),
    m_index1()
{}

//-------------------------------------------------------------------------------------------

CueParser::Track::Track(const Track& rhs) : m_trackNo(0),
    m_name(),
    m_index0(),
    m_index1()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const CueParser::Track& CueParser::Track::operator = (const Track& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void CueParser::Track::copy(const Track& rhs)
{
    m_trackNo = rhs.m_trackNo;
    m_name = rhs.m_name;
    m_index0 = rhs.m_index0;
    m_index1 = rhs.m_index1;
}

//-------------------------------------------------------------------------------------------

void CueParser::Track::clear()
{
    m_trackNo = 0;
    m_name = "";
    m_index0 = 0.0;
    m_index1 = 0.0;
}

//-------------------------------------------------------------------------------------------
// CueParser
//-------------------------------------------------------------------------------------------

CueParser::CueParser() : m_title(),
    m_artist(),
    m_genre(),
    m_date(),
    m_dataFilename(),
    m_tracks(),
    m_isUtf8(false)
{
    m_state[0]  = m_parser.String("REM");
    m_state[1]  = m_parser.String("GENRE");
    m_state[2]  = m_parser.String("DATE");
    m_state[3]  = m_parser.String("PERFORMER");
    m_state[4]  = m_parser.String("TITLE");
    m_state[5]  = m_parser.String("FILE");
    m_state[6]  = m_parser.String("TRACK");
    m_state[7]  = m_parser.String("AUDIO");
    m_state[8]  = m_parser.String("INDEX");
    m_state[9]  = m_parser.String("\r\n");
    m_state[10] = m_parser.String("\n");
}

//-------------------------------------------------------------------------------------------

CueParser::~CueParser()
{}

//-------------------------------------------------------------------------------------------

bool CueParser::parse(const QString& fileName)
{
    common::BIOStream fileIO(common::e_BIOStream_FileRead);
    bool res = false;

    if(fileIO.open(fileName))
    {
        int len = fileIO.size();
        if(len>0)
        {
            QByteArray arr(len,'\0');
            if(fileIO.read(arr.data(),len)==len)
            {
                res = parse(arr);
            }
        }
        fileIO.close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool CueParser::parse(const QByteArray& dMem)
{
    int tState = 0,tIndex = -1;
    common::BO_Parse_Unit *item;
    common::BString dStr(dMem.constData(),dMem.length());
    Track cTrack;
    QMap<int,Track> trackMap;
    QMap<int,Track>::iterator ppI;

    m_isUtf8 = XMCDParser::isUtf8(reinterpret_cast<const tubyte *>(dMem.constData()),dMem.length());

    m_title = "";
    m_artist = "";
    m_genre = "";
    m_date = "";
    m_dataFilename = "";
    m_tracks.clear();

    item = m_parser.Lexical(dStr.getString());
    while(item!=0)
    {
        if(item->state==m_state[0] && tState==0) // REM
        {
            item = parseREM(dStr,item);
        }
        else if(item->state==m_state[3] && tState==0) // PERFORMER
        {
            item = parseSingleString(dStr,item,m_artist);
        }
        else if(item->state==m_state[4]) // TITLE
        {
            if(tState==0)
            {
                item = parseSingleString(dStr,item,m_title);
            }
            else
            {
                item = parseSingleString(dStr,item,cTrack.name());
            }
        }
        else if(item->state==m_state[5] && tState==0) // FILE
        {
            item = parseFILE(dStr,item);
        }
        else if(item->state==m_state[6]) // TRACK
        {
            if(tState==1)
            {
                trackMap.insert(tIndex,cTrack);
            }
            item = parseTRACK(dStr,item,tIndex);
            if(tIndex>=0)
            {
                cTrack.clear();
                cTrack.trackNo() = tIndex;
                tState = 1;
            }
            else
            {
                tState = 2;
            }
        }
        else if(item->state==m_state[8] && tState==1) // INDEX
        {
            item = parseINDEX(dStr,item,cTrack);
        }
        else
        {
            item = nextLine(item);
        }
    }
    if(tState==1)
    {
        trackMap.insert(tIndex,cTrack);
    }

    for(ppI=trackMap.begin();ppI!=trackMap.end();++ppI)
    {
        m_tracks.append(ppI.value());
    }
    return (m_tracks.size()>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

common::BO_Parse_Unit *CueParser::nextLine(common::BO_Parse_Unit *item)
{
    while(item!=0)
    {
        common::BO_Parse_Unit *pItem = item;
        item = item->next;
        if(pItem->state==m_state[9] || pItem->state==m_state[10])
        {
            break;
        }
    }
    return item;
}

//-------------------------------------------------------------------------------------------

common::BO_Parse_Unit *CueParser::nextLine(common::BO_Parse_Unit *item,int& end)
{
    end = 0;
    while(item!=0)
    {
        common::BO_Parse_Unit *pItem = item;
        end = pItem->start;
        item = item->next;
        if(pItem->state==m_state[9] || pItem->state==m_state[10])
        {
            break;
        }
    }
    return item;
}

//-------------------------------------------------------------------------------------------

common::BString CueParser::getCueString(const common::BString& cString,int start,int end)
{
    common::BString t;

    if(start<end)
    {
        t = cString.mid(start,end - start).trim();
        if(t.length()>2 && t.at(0)=='\"' && t.at(t.length()-1)=='\"')
        {
            t = t.mid(1,t.length()-2);
        }
    }
    return t;
}

//-------------------------------------------------------------------------------------------

common::BO_Parse_Unit *CueParser::parseREM(const common::BString& cString,common::BO_Parse_Unit *item)
{
    bool res = false;

    if(item->state==m_state[0])
    {
        item = item->next;
        if(item!=0)
        {
            int cState = 0;
            if(item->state==m_state[1])
            {
                cState = 1;
            }
            else if(item->state==m_state[2])
            {
                cState = 2;
            }
            if(cState>0)
            {
                int start,end;
                common::BString x;
                start = item->start + item->length;
                item = nextLine(item,end);
                x = getCueString(cString,start,end);
                if(cState==1)
                {
                    m_genre = toQString(x.getString(),x.length());
                }
                else
                {
                    m_date = toQString(x.getString(),x.length());
                }
                res = true;
            }
        }
    }
    if(!res)
    {
        item = nextLine(item);
    }
    return item;
}

//-------------------------------------------------------------------------------------------

common::BO_Parse_Unit *CueParser::parseSingleString(const common::BString& cString,common::BO_Parse_Unit *item,QString& nStr)
{
    int start,end;
    common::BString x;
    start = item->start + item->length;
    item = nextLine(item,end);
    x = getCueString(cString,start,end);
    nStr = toQString(x.getString(),x.length());
    return item;
}

//-------------------------------------------------------------------------------------------

common::BO_Parse_Unit *CueParser::parseFILE(const common::BString& cString,common::BO_Parse_Unit *item)
{
    int start,end;
    common::BString x;
    start = item->start + item->length;
    item = nextLine(item,end);
    if(start<end)
    {
        int len = end - start;
        x = cString.mid(start,len).trim();
        len--;
        while(len>0)
        {
            if(x.at(len)==' ')
            {
                break;
            }
            len--;
        }
        x = x.mid(0,len).trim();
        if(x.length()>2 && x.at(0)=='\"' && x.at(x.length()-1)=='\"')
        {
            x = x.mid(1,x.length()-2);
        }
        m_dataFilename = toQString(x.getString(),x.length());
    }
    return item;
}

//-------------------------------------------------------------------------------------------

common::BO_Parse_Unit *CueParser::parseTRACK(const common::BString& cString,common::BO_Parse_Unit *item,int& tIndex)
{
    int start,end;
    bool res = false;

    if(item->state==m_state[6])
    {
        start = item->start + item->length;
        item = item->next;
        if(item!=0 && item->state==m_state[7])
        {
            end = item->start;
            if(start<end)
            {
                common::BString x = cString.mid(start,end - start).trim();
                tIndex = x.Atoi();
                if(tIndex>=0)
                {
                    res = true;
                }
            }
        }
    }
    item = nextLine(item);
    if(!res)
    {
        tIndex = -1;
    }
    return item;
}

//-------------------------------------------------------------------------------------------

common::BO_Parse_Unit *CueParser::parseINDEX(const common::BString& cString,common::BO_Parse_Unit *item,Track& cTrack)
{
    bool res = false;

    if(item->state==m_state[8])
    {
        int start,end;
        start = item->start + item->length;
        item = nextLine(item,end);
        res = true;
        if(start<end)
        {
            int iIndex = -1,minT = 0,secT = 0,frameT = 0;
            int i = 0,state = 0;
            const tchar *x;
            common::BString xStr;

            xStr = cString.mid(start,end - start).trim();
            x = static_cast<const tchar *>(xStr);
            while(i<xStr.length() && state<6)
            {
                switch(state)
                {
                    case 0:
                    case 2:
                    case 4:
                        if(x[i]>='0' && x[i]<='9')
                        {
                            start = i;
                            state++;
                        }
                        break;

                    case 1:
                        if(x[i]==' ')
                        {
                            iIndex = xStr.mid(start,i - start).Atoi();
                            state = 2;
                        }
                        break;

                    case 3:
                        if(x[i]==':')
                        {
                            minT = xStr.mid(start,i - start).Atoi();
                            state = 4;
                        }
                        break;

                    case 5:
                        if(x[i]==':')
                        {
                            secT = xStr.mid(start,i - start).Atoi();
                            frameT = xStr.mid(i+1).trim().Atoi();
                            state = 6;
                        }
                        break;
                }
                i++;
            }
            if(iIndex==0 || iIndex==1)
            {
                tfloat64 tN = static_cast<tfloat64>(frameT) / 75.0;
                tN += static_cast<tfloat64>(secT);
                tN += static_cast<tfloat64>(minT * 60.0);
                if(iIndex==0)
                {
                    cTrack.index0() = tN;
                }
                else
                {
                    cTrack.index1() = tN;
                }
            }
        }
    }
    if(!res)
    {
        item = nextLine(item);
    }
    return item;
}

//-------------------------------------------------------------------------------------------

QString CueParser::toQString(const tbyte *mem,int len) const
{
    QString str;

    if(m_isUtf8)
    {
        str = QString::fromUtf8(mem,len);
    }
    else
    {
        str = QString::fromLatin1(mem,len);
    }
    return str;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
