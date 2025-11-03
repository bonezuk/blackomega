//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_ABSTRACTTRACKMODEL_H
#define __OMEGA_TRACK_MODEL_ABSTRACTTRACKMODEL_H
//-------------------------------------------------------------------------------------------

#include <QVector>
#include <QVariant>

#include "track/db/inc/TrackDB.h"
#include "track/model/inc/TrackModelKey.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

typedef enum
{
    e_modelAlbums = 1,
    e_modelAlbumTracks,
    e_modelSongs,
    e_modelArtist,
    e_modelGenre,
    e_modelArtistAlbums,
    e_modelPlaylist,
    e_modelUnknown = 0
} TrackModelType;

//-------------------------------------------------------------------------------------------

class TRACK_MODEL_EXPORT AbstractTrackModel
{
    public:
        AbstractTrackModel();
        AbstractTrackModel(const TrackModelKey& filterKey);
        virtual ~AbstractTrackModel();

        const TrackModelKey& filterKey() const;

        virtual bool build() = 0;

        virtual TrackModelType type() const = 0;

        virtual QVariant data(int rowIndex, int columnIndex) const = 0;
        virtual QVariant data(int sectionIndex,int rowIndex,int columnIndex) const = 0;

        virtual int size() const = 0;
        virtual int numberSections() const = 0;
        virtual int numberRowsInSection(int secIdx) const = 0;

    protected:

        TrackModelKey m_filterKey;

        virtual db::SQLiteQuerySPtr getDBQuery() const;
};

//-------------------------------------------------------------------------------------------

typedef QSharedPointer<AbstractTrackModel> AbstractTrackModelSPtr;

//-------------------------------------------------------------------------------------------

typedef QVector<QVariant> QueryRecord;
typedef QVector<QueryRecord> QueryResult;

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
