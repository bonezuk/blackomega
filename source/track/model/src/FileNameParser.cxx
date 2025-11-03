#include "track/model/inc/FileNameParser.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

FileNameParser::FileNameParser()
{}

//-------------------------------------------------------------------------------------------

FileNameParser::~FileNameParser()
{}

//-------------------------------------------------------------------------------------------

QStringList FileNameParser::processPart(const QString& str)
{
    int i, start;
    QString s;
    QStringList list;

    start = 0;
    for(i = 0; i < str.size(); i++)
    {
        QChar c = str.at(i);
        if(c == QChar('-') || c == QChar(','))
        {
            if(start < i)
            {
                s = str.mid(start, i - start).trimmed();
                if(!s.isEmpty())
                {
                    list.append(s);
                }
            }
            start = i + 1;
        }
    }
    if(start < str.size())
    {
        s = str.mid(start).trimmed();
        if(!s.isEmpty())
        {
            list.append(s);
        }
    }
    return list;
}

//-------------------------------------------------------------------------------------------

QStringList FileNameParser::parsePart(const QString& str)
{
    int i, start;
    QString s;
    QStringList list;

    start = 0;
    for(i = 0; i < str.size(); i++)
    {
        QChar c = str.at(i);
        if(c == QChar('(') || c == QChar(')') || c == QChar('[') || c == QChar(']') || c == QChar('{') || c == QChar('}'))
        {
            if(start < i)
            {
                s = str.mid(start, i - start).trimmed();
                if(!s.isEmpty())
                {
                    list.append(processPart(s));
                }
            }
            start = i + 1;
        }
    }
    if(start < str.size())
    {
        s = str.mid(start).trimmed();
        if(!s.isEmpty())
        {
            list.append(processPart(s));
        }
    }
    return list;
}

//-------------------------------------------------------------------------------------------

int FileNameParser::trackNumberIfAvailable(const QString& str, QString& nPart)
{
    QString n;
    bool res = false;
    int idx, trackNo = -1;

    idx = str.indexOf(QChar(' '));
    if(idx > 0)
    {
        n = str.left(idx);
        nPart = str.mid(idx + 1).trimmed();
    }
    else
    {
        n = str;
        nPart = "";
    }
    trackNo = n.toInt(&res);
    if(!res)
    {
        trackNo = -1;
    }
    return trackNo;
}

//-------------------------------------------------------------------------------------------

QStringList FileNameParser::parse(const QString& str)
{
    qsizetype idx;
    QString n;
    QStringList list;

    idx = str.lastIndexOf(QChar('.'));
    if(idx > 0)
    {
        n = str.mid(0, idx);
    }
    else
    {
        n = str;
    }
    list = parsePart(n);
    if(list.size() > 0)
    {
        int trackNo = trackNumberIfAvailable(list.at(0), n);
        if(trackNo > 0)
        {
            QStringList tList;
            tList.append(QString::number(trackNo));
            if(!n.isEmpty())
            {
                tList.append(n);
            }
            list.removeFirst();
            tList.append(list);
            list = tList;
        }
    }
    return list;
}

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
