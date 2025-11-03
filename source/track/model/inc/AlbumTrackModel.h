//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_ALBUMTRACKMODEL_H
#define __OMEGA_TRACK_MODEL_ALBUMTRACKMODEL_H
//-------------------------------------------------------------------------------------------

#include "track/model/inc/AbstractTrackModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

class TRACK_MODEL_EXPORT AlbumTrackModel : public AbstractTrackModel
{
    public:
        class Record;

        typedef enum
        {
            e_isGroup = 0,
            e_groupID = 1,
            e_albumID = 2,
            e_trackID = 3,
            e_subtrackID = 4,
            e_discNo = 5,
            e_trackNo = 6,
            e_albumName = 7,
            e_trackName = 8,
            e_fileName = 9,
            e_length = 10,
            e_artist = 11,
            e_originalArtist = 12,
            e_composer = 13
        } ColumnType;

    public:
        AlbumTrackModel();
        AlbumTrackModel(const TrackModelKey& filterKey);
        virtual ~AlbumTrackModel();
        
        virtual TrackModelType type() const;
        
        virtual QVariant data(int rowIndex, int columnIndex) const;
        virtual QVariant data(int sectionIndex,int rowIndex,int columnIndex) const;
        
        virtual int size() const;
        virtual int numberSections() const;
        virtual int numberRowsInSection(int secIdx) const;
        
        virtual bool build();

        virtual QVector<QPair<tint, tint> > indexForDBInfo(QSharedPointer<db::DBInfo>& dbItem, bool isAdd);
        virtual void addDBInfo(tint idx, tint subtrackID, QSharedPointer<db::DBInfo>& dbItem);
        virtual void removeRow(tint idx);
        
    protected:
    
        QList<Record> m_tracks;
    
        virtual void printError(const tchar *strR, const tchar *strE) const;
        virtual bool populate();
        virtual const AlbumModelKey& getAlbumID() const;
        virtual void applyAnd(QString& cmd, bool isAnd) const;
        virtual QString getQuery(bool isIDOnly) const;
};

//-------------------------------------------------------------------------------------------

class TRACK_MODEL_EXPORT AlbumTrackModel::Record
{
    public:
        Record();
        Record(const Record& rhs);
        
        const Record& operator = (const Record& rhs);
    
        bool isGroup() const;
        const tint& groupID() const;
        const tint& albumID() const;
        const tint& trackID() const;
        const tint& subtrackID() const;
        const tint& discNo() const;
        const tint& trackNo() const;
        const QString& albumName() const;
        const QString& trackName() const;
        const QString& fileName() const;
        const common::TimeStamp& length() const;
        const QString& artist() const;
        const QString& originalArtist() const;
        const QString& composer() const;
        
        void set(tint vGroupID, tint vAlbumID, tint vTrackID, tint vSubtrackID, 
                 tint vDiscNo, tint vTrackNo, const QString& albumName, const QString& trackName, const QString& fileName,
                 const QString& vArtist, const QString& vOrgArtist, const QString& vComposer, common::TimeStamp& vLength);
    
    private:
        tint m_groupID;
        tint m_albumID;
        tint m_trackID;
        tint m_subtrackID;
        tint m_discNo;
        tint m_trackNo;
        QString m_albumName;
        QString m_trackName;
        QString m_fileName;
        common::TimeStamp m_length;
        QString m_artist;
        QString m_originalArtist;
        QString m_composer;
        
        void copy(const Record& rhs);
};

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

