#include <QRegExp>
#include <QStringList>

#include "track/info/inc/XMCDInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------
// XMCDParser
//-------------------------------------------------------------------------------------------

XMCDParser::XMCDParser()
{}

//-------------------------------------------------------------------------------------------

XMCDParser::~XMCDParser()
{}

//-------------------------------------------------------------------------------------------
// ISO-8859-1 (20h-7eh) + (a0h-ffh)
// UTF-8 0xxx xxxx
//       110x xxxx 10xx xxxx
//       1110 xxxx 10xx xxxx 10xx xxxx
// true  -> ISO-8859-1 Latin1 encoding
// false -> UTF-8 Unicode (default)
//-------------------------------------------------------------------------------------------

bool XMCDParser::isUtf8(const tubyte *mem,tint len)
{
    bool res = true;

    if(mem!=0 && len>0)
    {
        tint state = 0;

        for(tint i=0;res && i<len;i++)
        {
            // previous (mem[i] & 0x40)==0
            if(state==0)
            {
                if(mem[i] & 0x80)
                {
                    if((mem[i] & 0xe0)==0xc0) // 110x xxxx
                    {
                        // next char is 10xx xxxx for UTF-8
                        state = 1;
                    }
                    else if((mem[i] & 0xf0)==0xe0) // 1110 xxxx
                    {
                        // next two char is 10xx xxxx for UTF-8
                        state = 2;
                    }
                    else
                    {
                        res = false;
                    }
                }
            }
            else
            {
                if((mem[i] & 0xc0)!=0x80)
                {
                    res = false;
                }
                state--;
            }
        }
        if(res && state>0)
        {
            res = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

XMCDParser::CommentState XMCDParser::parseLineComment(const QString& line,int& number)
{
    tint i,state = 0;
    CommentState lineType = e_NoComment;
    bool loop = true;

    for(i=0;loop && i<line.size();i++)
    {
        switch(state)
        {
            case 0:
                if(!line.at(i).isSpace())
                {
                    if(line.at(i)==QChar('#'))
                    {
                        lineType = e_CommentPlain;
                        state = 1;
                    }
                    else
                    {
                        loop = false;
                    }
                }
                break;

            case 1:
                if(!line.at(i).isSpace())
                {
                    QString rLine = line.mid(i).toLower();
                    if(rLine.at(0)==QChar('x'))
                    {
                        QRegExp reg("^xmcd((?:\\s+)|(?:\\s*$))");
                        if(reg.indexIn(rLine)==0)
                        {
                            lineType = e_CommentXMCD;
                        }
                    }
                    else if(rLine.at(0)==QChar('t'))
                    {
                        QRegExp reg("^(track\\s+frame\\s+offsets\\s*:)(?:\\s*)$");
                        if(reg.indexIn(rLine)==0)
                        {
                            lineType = e_CommentTrackHeader;
                        }
                    }
                    else if(rLine.at(0)==QChar('d'))
                    {
                        QRegExp reg("^(disc\\s+length\\s*:)(?:\\s*)(\\d+)((?:\\s*)|(?:\\s+\\S+\\s*))$");
                        if(reg.indexIn(rLine)==0)
                        {
                            bool ok = false;
                            int n = reg.cap(2).toInt(&ok);
                            if(ok)
                            {
                                number = n;
                                lineType = e_CommentDiscLength;
                            }
                        }
                    }
                    else if(rLine.at(0).isDigit())
                    {
                        QRegExp reg("^\\d+(?:\\s*)$");
                        int pos = reg.indexIn(rLine);
                        if(pos==0)
                        {
                            bool ok = false;
                            int n = reg.cap(0).toInt(&ok);
                            if(ok)
                            {
                                number = n;
                                lineType = e_CommentNumericOffset;
                            }
                        }
                    }
                    loop = false;
                }
                break;

            default:
                loop = false;
                break;
        }
    }
    return lineType;
}

//-------------------------------------------------------------------------------------------

bool XMCDParser::parseLineData(const QString& line,QString& keyword,QString& data) const
{
    tint state = 0;
    bool res = false;
    QString temp;
    QString::const_iterator ppI;

    for(ppI=line.begin();ppI!=line.end();ppI++)
    {
        const QChar& c = *ppI;

        switch(state)
        {
            case 0:
                if(c==QChar('\\'))
                {
                    state = 1;
                }
                else if(c==QChar('='))
                {
                    keyword = temp.trimmed().toLower();
                    temp = "";
                    res = true;
                }
                else
                {
                    temp += c;
                }
                break;

            case 1:
                if(c==QChar('n'))
                {
                    temp += QChar(0x000a);
                }
                else if(c==QChar('t'))
                {
                    temp += QChar(0x0009);
                }
                else if(c==QChar('\\'))
                {
                    temp += QChar('\\');
                }
                else
                {
                    temp += c;
                }
                state = 0;
                break;
        }
    }
    if(res)
    {
        data = temp;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tuint32 XMCDParser::processDiscID8ByteSequence(const QString& s) const
{
    tuint32 num;
    int state = 0;
    QString numS;
    QString::const_iterator ppI;
    bool ok = false;

    for(ppI=s.begin();ppI!=s.end() && state>=0;ppI++)
    {
        const QChar& c = *ppI;

        if(c.isDigit() || (c>=QChar('a') && c<=QChar('f')) || (c>=QChar('A') && c<=QChar('F')))
        {
            numS += c;
            state++;
        }
        else
        {
            state = -1;
        }
    }
    num = numS.toUInt(&ok,16);
    if(!ok)
    {
        state = -1;
    }
    return (state>0 && state<=8) ? num : 0;
}

//-------------------------------------------------------------------------------------------

tuint32 XMCDParser::processDiscID(const QString& line) const
{
    tuint32 discID = 0;
    int start = 0,pos;
    QRegExp reg(",+");
    QString s;

    while(pos=reg.indexIn(line,start),pos>=0 && discID==0)
    {
        s = line.mid(start,pos - start).trimmed();
        discID = processDiscID8ByteSequence(s);
        start = pos + 1;
    }
    if(discID==0)
    {
        s = line.mid(start).trimmed();
        discID = processDiscID8ByteSequence(s);
    }
    return discID;
}

//-------------------------------------------------------------------------------------------

QPair<QString,QString> XMCDParser::processTitle(const QString& line) const
{
    int pos;
    QString artist,title;
    QRegExp regSplit(" / "),regVarious("\"Various\"");
    pos = regSplit.indexIn(line);
    if(pos>=0)
    {
        if(pos > 0)
        {
            artist = line.mid(0,pos).trimmed();
        }
        title = line.mid(pos+3).trimmed();
    }
    else
    {
        artist = title = line.trimmed();
    }
    pos = regVarious.indexIn(artist);
    if(pos>=0)
    {
        artist = "Various";
    }
    return QPair<QString,QString>(artist,title);
}

//-------------------------------------------------------------------------------------------

tint XMCDParser::processYear(const QString& line) const
{
    int pos,year;
    QRegExp reg("^(?:\\s*)(\\d{4})(?:\\s*)$");

    pos = reg.indexIn(line);
    if(pos>=0)
    {
        year = reg.cap(1).toInt();
    }
    else
    {
        year = 0;
    }
    return year;
}

//-------------------------------------------------------------------------------------------

QString XMCDParser::processGenre(const QString& line) const
{
    int state = 0;
    QString genre;
    QString::const_iterator ppI;

    for(ppI=line.begin();ppI!=line.end();ppI++)
    {
        const QChar& c = *ppI;
        if(!c.isSpace())
        {
            if(state==2)
            {
                genre += QChar(' ');
                state = 0;
            }

            if(state==0)
            {
                if(c.isLetter())
                {
                    genre += c.toUpper();
                }
                else
                {
                    genre += c;
                }
                state = 1;
            }
            else if(state==1)
            {
                if(c.isLetter())
                {
                    genre += c.toLower();
                }
                else
                {
                    genre += c;
                }
            }
        }
        else
        {
            state = 2;
        }
    }
    return genre.trimmed();
}

//-------------------------------------------------------------------------------------------

int XMCDParser::getTitleNumber(const QString& keyword) const
{
    int num;
    QString k = keyword.trimmed().toLower();

    if(k.size() > 6 && k.mid(0,6)=="ttitle")
    {
        bool ok = false;
        num = k.mid(6).toInt(&ok);
        if(!ok)
        {
            num = -1;
        }
    }
    else
    {
        num = -1;
    }
    return num;
}

//-------------------------------------------------------------------------------------------

QStringList XMCDParser::findLines(const QString& text) const
{
    int pos,current = 0;
    QRegExp reg("([\\r\\n])|([\\n])");
    QStringList lines;
    QString line;

    while(pos = reg.indexIn(text,current),pos>=0)
    {
        if(current < pos)
        {
            lines.append(text.mid(current,pos - current));
        }
        QString t = reg.cap(1);
        current = t.size();
        current += pos;
    }
    if(current < text.size())
    {
        lines.append(text.mid(current));
    }
    return lines;
}

//-------------------------------------------------------------------------------------------

bool XMCDParser::read(const QString& fileName,const QByteArray& array,QVector<InfoSPtr>& list)
{
    tuint32 discID = 0;
    int total = 0,year = 0;
    QString text;
    QList<int> offsetList;
    QMap<int,QPair<QString,QString> > trackMap;
    QPair<QString,QString> defaultArtistTrack;
    QString genre;
    QStringList lines;
    QStringList::const_iterator ppI;
    bool validFlag = false,res = false;

    list.clear();

    if(isUtf8(reinterpret_cast<const tubyte *>(array.data()),array.size()))
    {
        text = QString::fromUtf8(array);
    }
    else
    {
        text = QString::fromLatin1(array);
    }
    lines = findLines(text);

    for(ppI=lines.begin();ppI!=lines.end();++ppI)
    {
        int offset = 0;
        const QString& line = *ppI;

        CommentState commentState = parseLineComment(line,offset);
        if(commentState!=e_NoComment)
        {
            if(commentState==e_CommentXMCD)
            {
                validFlag = true;
            }
            else if(commentState==e_CommentTrackHeader)
            {
                offsetList.clear();
            }
            else if(commentState==e_CommentNumericOffset)
            {
                offsetList.append(offset);
            }
            else if(commentState==e_CommentDiscLength)
            {
                total = offset;
            }
        }
        else
        {
            QString keyword,data;

            if(parseLineData(line,keyword,data))
            {
                if(keyword=="discid")
                {
                    discID = processDiscID(data);
                }
                else if(keyword=="dtitle")
                {
                    defaultArtistTrack = processTitle(data);
                }
                else if(keyword=="dyear")
                {
                    year = processYear(data);
                }
                else if(keyword=="dgenre")
                {
                    genre = processGenre(data);
                }
                else
                {
                    int trackNo = getTitleNumber(keyword);
                    if(trackNo>=0)
                    {
                        QPair<QString,QString> artistTrack = processTitle(data);
                        trackMap.insert(trackNo,artistTrack);
                    }
                }
            }
        }
    }

    if(validFlag && trackMap.size()>0 && discID>0 && !defaultArtistTrack.first.isEmpty() && !defaultArtistTrack.second.isEmpty())
    {
        QMap<int,QPair<QString,QString> >::const_iterator ppJ;

        for(ppJ=trackMap.begin();ppJ!=trackMap.end();++ppJ)
        {
            int trackNo = ppJ.key();
            InfoSPtr info(new XMCDInfo);

            if(ppJ.value().first.isEmpty() || ppJ.value().first==ppJ.value().second)
            {
                info->artist() = defaultArtistTrack.first;
            }
            else
            {
                info->artist() = ppJ.value().first;
            }
            info->originalArtist() = info->artist();
            info->composer() = defaultArtistTrack.first;
            info->title() = ppJ.value().second;
            info->setFilename(fileName + "." + QString::number(trackNo + 1) + ".xmcd");
            info->album() = defaultArtistTrack.second;
            info->genre() = genre;
            if(year>0)
            {
                info->year() = QString::number(year);
            }
            info->track() = QString::number(trackNo + 1);
            info->disc() = "1";

            double len;
            if(trackNo < offsetList.size() - 1)
            {
                len = static_cast<double>(offsetList.at(trackNo+1) - offsetList.at(trackNo)) / 75.0;
            }
            else
            {
                len = static_cast<double>((total * 75) - offsetList.at(offsetList.size()-1)) / 75.0;
            }
            info->length() = len;

            list.append(info);
        }
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// XMCDInfo
//-------------------------------------------------------------------------------------------

XMCDInfo::XMCDInfo()
{}

//-------------------------------------------------------------------------------------------

XMCDInfo::~XMCDInfo()
{}

//-------------------------------------------------------------------------------------------

QVector<InfoSPtr> XMCDInfo::readXMCD(common::BIOStream *input)
{
    QVector<InfoSPtr> trackList;

    if(input!=0)
    {
        int len = input->size();
        if(len > 0)
        {
            tbyte *mem = new tbyte [len];
            if(input->read(mem,len)==len)
            {
                QByteArray array(mem,len);
                XMCDParser parser;
                if(!parser.read(input->name(),array,trackList))
                {
                    trackList.clear();
                }
            }
            delete [] mem;
        }
    }
    return trackList;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
