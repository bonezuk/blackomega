//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_MPCCHAPTERTAG_H
#define __OMEGA_TRACK_INFO_MPCCHAPTERTAG_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/Info.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT MPCChapterTagItem
{
    public:
        MPCChapterTagItem();
        MPCChapterTagItem(const MPCChapterTagItem& rhs);
        virtual ~MPCChapterTagItem();

        virtual const MPCChapterTagItem& operator = (const MPCChapterTagItem& rhs);

        virtual bool read(common::BIOStream *file);
        virtual void setFrequency(tint frequency);

        virtual const tint& sampleIndex() const;
        virtual const common::TimeStamp& time() const;

        virtual const QString& title() const;
        virtual const QString& performer() const;
        virtual const QString& songWriter() const;
        virtual const QString& composer() const;
        virtual const QString& arranger() const;
        virtual const QString& message() const;
        virtual const QString& genre() const;

        virtual const tint& trackIndex() const;
        virtual const tint& numberOfTracks() const;

    protected:

        tint m_sampleIndex;
        common::TimeStamp m_time;
        QString m_title;
        QString m_performer;
        QString m_songWriter;
        QString m_composer;
        QString m_arranger;
        QString m_message;
        QString m_genre;
        tint m_trackIndex;
        tint m_numberOfTracks;

        virtual void copy(const MPCChapterTagItem& rhs);

        virtual tint decodeSize(common::BIOStream *file,tint& pos);
        virtual bool trackNumber(const QString& data,int& trackIndex,int& noOfTracks);
};

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT MPCChapterTag
{
    public:
        MPCChapterTag();
        virtual ~MPCChapterTag();

        virtual bool read(common::BIOStream *file);

        virtual const QVector<MPCChapterTagItem>& items() const;

        virtual const common::TimeStamp& length() const;

    protected:

        QVector<MPCChapterTagItem> m_items;
        common::TimeStamp m_length;
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
