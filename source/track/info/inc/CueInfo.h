//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_CUEINFO_H
#define __OMEGA_TRACK_INFO_CUEINFO_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/Info.h"
#include "track/info/inc/CueParser.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT CueInfo : public Info
{
    public:
        CueInfo(QObject *parent = 0);
        virtual ~CueInfo();
        
        virtual bool isChildren() const;
        virtual tint noChildren() const;
        virtual const ChildInfo& child(tint idx) const;

    protected:

        QList<ChildInfo> m_chapters;
        
        virtual bool isSeparateCuesheet(const QString& trackFileName);
        virtual QString separateCueFilename(const QString& trackFileName);
        
        virtual bool readCueSheet(const QString& fileName);
        virtual bool readCueSheet(const QByteArray& cueArray);
        
        virtual void processCueSheet(const CueParser& cParser);
        
        virtual void setLength(const common::TimeStamp& l);
        
        virtual bool canGetTrackLength() const;
        virtual common::TimeStamp getTrackLength();
        
        virtual bool isPreGapPlayed() const;
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
