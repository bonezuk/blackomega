//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_ALBUMMODELUTILITIES_H
#define __OMEGA_TRACK_MODEL_ALBUMMODELUTILITIES_H
//-------------------------------------------------------------------------------------------

#include "track/model/inc/AlbumModel.h"
#include "track/model/inc/ImageRepositary.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

class TRACK_MODEL_EXPORT AlbumModelUtilities
{
    public:
        AlbumModelUtilities();
        AlbumModelUtilities(const AlbumModelKey& albumID);
        AlbumModelUtilities(const AlbumModelKey& albumID,const TrackModelKey& key);
        virtual ~AlbumModelUtilities();
        
        virtual const AlbumModelKey& id() const;
        
        virtual bool isVariousArtists() const;
        
        virtual QString artist() const;
        virtual QString composer() const;

        virtual QImage *albumArt(int iWidth,int iHeight) const;

        virtual int year() const;
        
    protected:
    
        AlbumModelKey m_albumID;
        TrackModelKey m_filterKey;
        
        mutable QString m_artist;
        mutable QString m_composer;
        
        virtual QString getArtistTypeString(int artistType) const;
        
        virtual QString getQueryForAlbumArtist() const;
        virtual QString getQueryPartForAlbumArtist(int artistType,const AlbumModelKey& key) const;
        virtual QString getQueryConditionForFilter() const;
        
        virtual QueryRecord createRecordAlbumArtist(const AlbumModelKey& key,const QString& artist,int artistCount) const;
        virtual AlbumModelKey keyAlbumArtistRecord(const QueryRecord& record) const;
        virtual QString artistAlbumArtistRecord(const QueryRecord& record) const;
        virtual int countAlbumArtistRecord(const QueryRecord& record) const;
        
        virtual bool runAlbumArtistQuery(const QString& cmdQ,QueryResult& results) const;

        virtual db::SQLiteQuerySPtr getDBQuery() const;

        virtual QString primaryArtist(const QueryResult& results) const;
        
        virtual QString getQueryForAlbumImage() const;
        virtual bool runAlbumImageQuery(const QString& cmdQ,QueryResult& results) const;
        
        virtual QueryRecord createRecordAlbumImage(const AlbumModelKey& key,int imageID,int imageCount,int imageType) const;
        virtual AlbumModelKey keyAlbumImageRecord(const QueryRecord& record) const;
        virtual int imageIDAlbumImageRecord(const QueryRecord& record) const;
        virtual int imageCountAlbumImageRecord(const QueryRecord& record) const;
        virtual info::IDTagImageType imageTypeAlbumImageRecord(const QueryRecord& record) const;

        static AlbumModelKey keyAlbumImageRecordStatic(const QueryRecord& record);
        static int imageIDAlbumImageRecordStatic(const QueryRecord& record);
        static int imageCountAlbumImageRecordStatic(const QueryRecord& record);
        static info::IDTagImageType imageTypeAlbumImageRecordStatic(const QueryRecord& record);

        virtual void sortImageListByPreferredType(QueryResult& results) const;
        
        static bool compareImageResults(const QueryRecord& a,const QueryRecord& b);
        static bool preferedImageType(const info::IDTagImageType& a,const info::IDTagImageType& b);
        
        virtual QString getQueryForYear() const;
};

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace model
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

