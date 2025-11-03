#include "track/info/inc/CueInfo.h"
#include "common/inc/DiskOps.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

CueInfo::CueInfo(QObject *parent) : Info(parent),
    m_chapters()
{}

//-------------------------------------------------------------------------------------------

CueInfo::~CueInfo()
{}

//-------------------------------------------------------------------------------------------

bool CueInfo::isChildren() const
{
    return (m_chapters.size()>0);
}

//-------------------------------------------------------------------------------------------

tint CueInfo::noChildren() const
{
    return m_chapters.size();
}

//-------------------------------------------------------------------------------------------

const Info::ChildInfo& CueInfo::child(tint idx) const
{
    return m_chapters.at(idx);
}

//-------------------------------------------------------------------------------------------

bool CueInfo::isSeparateCuesheet(const QString& trackFileName)
{
    return common::DiskOps::exist(separateCueFilename(trackFileName));
}

//-------------------------------------------------------------------------------------------

QString CueInfo::separateCueFilename(const QString& trackFileName)
{
    QString cueName;
    
    for(int i=trackFileName.length()-1;i>=0 && cueName.isEmpty();i--)
    {
        if(trackFileName.at(i)==QChar('.'))
        {
            cueName = trackFileName.mid(0,i) + ".cue";
        }
    }
    return cueName;
}

//-------------------------------------------------------------------------------------------

bool CueInfo::readCueSheet(const QString& fileName)
{
    bool res;
    CueParser parser;

    if(parser.parse(fileName))
    {
        processCueSheet(parser);
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool CueInfo::readCueSheet(const QByteArray& cueArray)
{
    bool res;
    CueParser parser;
    
    if(parser.parse(cueArray))
    {
        processCueSheet(parser);
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool CueInfo::isPreGapPlayed() const
{
    return playPreGap();
}

//-------------------------------------------------------------------------------------------

void CueInfo::processCueSheet(const CueParser& cParser)
{
    if(m_Title.isEmpty())
    {
        m_Title = cParser.title().trimmed();
    }
    if(m_Album.isEmpty())
    {
        m_Album = cParser.title().trimmed();
    }
    if(m_Artist.isEmpty())
    {
        m_Artist = cParser.artist().trimmed();
    }
    if(m_Genre.isEmpty())
    {
        m_Genre = cParser.genre().trimmed();
    }
    if(m_Year.isEmpty())
    {
        m_Year = cParser.date().trimmed();
    }

    for(tint i=0;i<cParser.noTracks();i++)
    {
        ChildInfo c;
        const CueParser::Track& t = cParser.track(i);
        
        c.name() = t.name();
        if(t.index1()>0.0 && !isPreGapPlayed())
        {
            c.startTime() = t.index1();
        }
        else if(t.index0()>0.0)
        {
            c.startTime() = t.index0();
        }
        else
        {
            c.startTime() = 0.0;
        }
        
        if((i+1)==cParser.noTracks())
        {
            if(canGetTrackLength())
            {
                c.length() = getTrackLength() - c.startTime();
            }
            else
            {
                c.length() = 0;
            }
        }
        else
        {
            const CueParser::Track& nT = cParser.track(i+1);
            if(nT.index0()>0.0)
            {
                c.length() = nT.index0() - c.startTime();
            }
            else
            {
                c.length() = nT.index1() - c.startTime();
            }
        }
        
        if(i < m_chapters.size())
        {
            QString tName = "Track " + QString::number(i+1);
            if(m_chapters.at(i).name().isEmpty() || m_chapters.at(i).name()==tName)
            {
                m_chapters[i].name() = c.name();
            }
        }
        else
        {
            m_chapters.append(c);
        }
    }
}

//-------------------------------------------------------------------------------------------

void CueInfo::setLength(const common::TimeStamp& l)
{
    Info::setLength(l);
    if(!canGetTrackLength() && !m_chapters.isEmpty())
    {
        ChildInfo& cInfo = m_chapters[m_chapters.size() - 1];
        cInfo.length() = l - cInfo.startTime();
    }
}

//-------------------------------------------------------------------------------------------

bool CueInfo::canGetTrackLength() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp CueInfo::getTrackLength()
{
    return common::TimeStamp(0);
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
