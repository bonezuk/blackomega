//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_TRACKMODELKEY_H
#define __OMEGA_TRACK_MODEL_TRACKMODELKEY_H
//-------------------------------------------------------------------------------------------

#include "track/model/inc/AlbumModelKey.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

class TRACK_MODEL_EXPORT TrackModelKey
{
    public:
        TrackModelKey();
        TrackModelKey(const TrackModelKey& rhs);
        virtual ~TrackModelKey();
        
        const TrackModelKey& operator = (const TrackModelKey& rhs);
        
        virtual bool isEmpty() const;
        
        virtual bool isAlbum() const;
        virtual AlbumModelKey& album();
        virtual const AlbumModelKey& album() const;
                
        virtual bool isTrack() const;
        virtual int& track();
        virtual const int& track() const;
        
        virtual bool isSubtrack() const;
        virtual int& subtrack();
        virtual const int& subtrack() const;
        
        virtual bool isGenre() const;
        virtual int& genre();
        virtual const int& genre() const;
        
        virtual bool isArtist() const;
        virtual QString& artist();
        virtual const QString& artist() const;
        
    protected:
        
        AlbumModelKey m_albumID;
        int m_trackID;
        int m_subtrackID;
        int m_genreID;
        QString m_artist;
        
        void copy(const TrackModelKey& rhs);
};

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

