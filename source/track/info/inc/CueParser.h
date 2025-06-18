//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_CUEPARSER_H
#define __OMEGA_TRACK_INFO_CUEPARSER_H
//-------------------------------------------------------------------------------------------

#include <QString>
#include <QVector>
#include <QMap>

#include "common/inc/BString.h"
#include "common/inc/BOParse.h"
#include "common/inc/BIOStream.h"
#include "common/inc/TimeStamp.h"
#include "track/info/inc/InfoDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT CueParser
{
    public:
        class Track;
        
    public:
        CueParser();
        virtual ~CueParser();
        
        bool parse(const QString& fileName);
        bool parse(const QByteArray& dMem);
        
        const QString& title() const;
        const QString& artist() const;
        const QString& genre() const;
        const QString& date() const;
        
        const QString& dataFilename() const;
        
        const Track& track(int idx) const;
        int noTracks() const;
        
    protected:
        
        tint m_state[11];
        common::BOParse m_parser;
        
        QString m_title;
        QString m_artist;
        QString m_genre;
        QString m_date;
        QString m_dataFilename;
        QVector<Track> m_tracks;

        bool m_isUtf8;
        
        common::BO_Parse_Unit *nextLine(common::BO_Parse_Unit *item);
        common::BO_Parse_Unit *nextLine(common::BO_Parse_Unit *item,int& end);
        common::BString getCueString(const common::BString& cString,int start,int end);
        common::BO_Parse_Unit *parseREM(const common::BString& cString,common::BO_Parse_Unit *item);
        common::BO_Parse_Unit *parseSingleString(const common::BString& cString,common::BO_Parse_Unit *item,QString& nStr);
        common::BO_Parse_Unit *parseFILE(const common::BString& cString,common::BO_Parse_Unit *item);
        common::BO_Parse_Unit *parseTRACK(const common::BString& cString,common::BO_Parse_Unit *item,int& tIndex);
        common::BO_Parse_Unit *parseINDEX(const common::BString& cString,common::BO_Parse_Unit *item,Track& cTrack);

        virtual QString toQString(const tbyte *mem,int len) const;
};

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT CueParser::Track
{
    public:
        Track();
        Track(const Track& rhs);
        
        const Track& operator = (const Track& rhs);
        
        void clear();
        
        tint& trackNo();
        const tint& trackNo() const;
        QString& name();
        const QString& name() const;
        common::TimeStamp& index0();
        const common::TimeStamp& index0() const;
        common::TimeStamp& index1();
        const common::TimeStamp& index1() const;
        
    protected:
    
        tint m_trackNo;
        QString m_name;
        common::TimeStamp m_index0;
        common::TimeStamp m_index1;
        
        void copy(const Track& rhs);
};

//-------------------------------------------------------------------------------------------
// CueParse::Track
//-------------------------------------------------------------------------------------------

inline tint& CueParser::Track::trackNo()
{
    return m_trackNo;
}

//-------------------------------------------------------------------------------------------

inline const tint& CueParser::Track::trackNo() const
{
    return m_trackNo;
}

//-------------------------------------------------------------------------------------------

inline QString& CueParser::Track::name()
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

inline const QString& CueParser::Track::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& CueParser::Track::index0()
{
    return m_index0;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& CueParser::Track::index0() const
{
    return m_index0;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& CueParser::Track::index1()
{
    return m_index1;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& CueParser::Track::index1() const
{
    return m_index1;
}

//-------------------------------------------------------------------------------------------
// CueParser
//-------------------------------------------------------------------------------------------

inline const QString& CueParser::title() const
{
    return m_title;
}

//-------------------------------------------------------------------------------------------

inline const QString& CueParser::artist() const
{
    return m_artist;
}

//-------------------------------------------------------------------------------------------

inline const QString& CueParser::genre() const
{
    return m_genre;
}

//-------------------------------------------------------------------------------------------

inline const QString& CueParser::date() const
{
    return m_date;
}

//-------------------------------------------------------------------------------------------

inline const QString& CueParser::dataFilename() const
{
    return m_dataFilename;
}

//-------------------------------------------------------------------------------------------

inline const CueParser::Track& CueParser::track(int idx) const
{
    return m_tracks.at(idx);
}

//-------------------------------------------------------------------------------------------

inline int CueParser::noTracks() const
{
    return m_tracks.size();
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
