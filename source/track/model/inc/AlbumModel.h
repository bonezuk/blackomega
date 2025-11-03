//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_ALBUMMODEL_H
#define __OMEGA_TRACK_MODEL_ALBUMMODEL_H
//-------------------------------------------------------------------------------------------

#include "track/model/inc/AbstractTrackModel.h"
#include "track/model/inc/AlbumModelKey.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------
// AlbumModel 
// Each item in album model is an album. An album is defined as pertaining to an
// entry in the Album table in the database. As each album relates to a given
// collection of tracks then each data item is in itself a model item that is an
// instance of AlbumTrackModel.
// The model is divided up into alphabetical sections. Each row is a letter of the
// defined alphabet (default 'A'-'Z' plus all other characters). Other alphabets can
// can be defined to support internationalization.
// An album belongs to a paticular row if the case incensitive comparison of the first
// character corresponds with that rows alphabetical character.
//-------------------------------------------------------------------------------------------

class TRACK_MODEL_EXPORT AlbumModel : public AbstractTrackModel
{
    public:
        AlbumModel();
        AlbumModel(const TrackModelKey& filterKey);
        virtual ~AlbumModel();

        virtual TrackModelType type() const;
        
        virtual QVariant data(int rowIndex, int columnIndex) const;
        virtual QVariant data(int sectionIndex,int rowIndex,int columnIndex) const;
        
        virtual int size() const;
        virtual int numberSections() const;
        virtual int numberRowsInSection(int secIdx) const;

        virtual bool build();

        virtual QVector<tint> indexForDBInfo(QSharedPointer<db::DBInfo>& dbItem, bool isAdd);
        virtual void addDBInfo(tint idx, QSharedPointer<db::DBInfo>& dbItem);
        virtual void removeRow(tint idx);

    protected:
    
        QList<QPair<AlbumModelKey,QString> > m_albums;
        QVector<int> m_index;
    
        virtual bool populate();
        virtual QString getQuery(const AlbumModelKey& albumID) const;

        virtual QueryRecord createRecordAlbum(const AlbumModelKey& key,const QString& name) const;
        static AlbumModelKey keyRecordAlbumStatic(const QueryRecord& record);
        virtual AlbumModelKey keyRecordAlbum(const QueryRecord& record) const;
        static QString nameRecordAlbumStatic(const QueryRecord& record);
        virtual QString nameRecordAlbum(const QueryRecord& record) const;
        
        virtual bool runAlbumQuery(const QString& cmdQ,QueryResult& results) const;
        
        virtual void enumerateSections(const QueryResult& results);

        virtual QVector<QChar> getIndexAlphabet() const;
        virtual void buildIndexMap(const QVector<QChar>& alphabet,QMap<QChar,int>& indexMap) const;
        virtual QChar getFirstCharacter(const QString& name,const QMap<QChar,int>& indexMap,const QVector<QChar>& alphabet) const;
        virtual void mapResultsToAlphabetIndex(const QueryResult& results, const QMap<QChar, int>& indexMap, const QVector<QChar>& alphabet, QMap<QChar, QMultiMap<QString, int> >& sectionMap) const;
        virtual void insertIntoAlbum(QVector<QueryRecord>& recordList);
        virtual void addToModelForGivenMap(const QueryResult& results,const QMultiMap<QString,int>& map);
        virtual void buildModelFromSortedIndex(const QueryResult& results,const QVector<QChar>& alphabet,const QMap<QChar,QMultiMap<QString,int> >& sectionMap);

        static bool compareIdenticalAlbumNameLessThan(const QueryRecord& a,const QueryRecord& b);
        
        virtual QVariant dataAtIndex(int idx, int columnIndex) const;
        virtual tint findSectionIndex(const QString& albumName);
        virtual tint numberOfTracks(const AlbumModelKey& key) const;
        
        virtual int queryImageID(const AlbumModelKey& key) const;
        virtual int queryAlbumYear(const AlbumModelKey& key) const;
        virtual QString queryAlbumArtist(const AlbumModelKey& key) const;
        
        virtual bool isAlbumListed(const AlbumModelKey& key) const;

};

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

