#include "track/model/inc/TrackModelKey.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

TrackModelKey::TrackModelKey() : m_albumID(),
    m_trackID(-1),
    m_subtrackID(-1),
    m_genreID(-1),
    m_artist()
{}

//-------------------------------------------------------------------------------------------

TrackModelKey::TrackModelKey(const TrackModelKey& rhs) : m_albumID(),
    m_trackID(-1),
    m_subtrackID(-1),
    m_genreID(-1),
    m_artist()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

TrackModelKey::~TrackModelKey()
{}

//-------------------------------------------------------------------------------------------

const TrackModelKey& TrackModelKey::operator = (const TrackModelKey& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void TrackModelKey::copy(const TrackModelKey& rhs)
{
    m_albumID = rhs.m_albumID;
    m_trackID = rhs.m_trackID;
    m_subtrackID = rhs.m_subtrackID;
    m_genreID = rhs.m_genreID;
    m_artist = rhs.m_artist;
}

//-------------------------------------------------------------------------------------------

bool TrackModelKey::isEmpty() const
{
    return (!isAlbum() && !isGenre() && !isArtist()) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool TrackModelKey::isAlbum() const
{
    return !m_albumID.isAll();
}

//-------------------------------------------------------------------------------------------

AlbumModelKey& TrackModelKey::album()
{
    return m_albumID;
}

//-------------------------------------------------------------------------------------------

const AlbumModelKey& TrackModelKey::album() const
{
    return m_albumID;
}

//-------------------------------------------------------------------------------------------

bool TrackModelKey::isTrack() const
{
    return (isAlbum() && m_trackID>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

int& TrackModelKey::track()
{
    return m_trackID;
}

//-------------------------------------------------------------------------------------------

const int& TrackModelKey::track() const
{
    return m_trackID;
}

//-------------------------------------------------------------------------------------------

bool TrackModelKey::isSubtrack() const
{
    return (isAlbum() && m_trackID>=0 && m_subtrackID>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

int& TrackModelKey::subtrack()
{
    return m_subtrackID;
}

//-------------------------------------------------------------------------------------------

const int& TrackModelKey::subtrack() const
{
    return m_subtrackID;
}

//-------------------------------------------------------------------------------------------

bool TrackModelKey::isGenre() const
{
    return (m_genreID>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

int& TrackModelKey::genre()
{
    return m_genreID;
}

//-------------------------------------------------------------------------------------------

const int& TrackModelKey::genre() const
{
    return m_genreID;
}

//-------------------------------------------------------------------------------------------

bool TrackModelKey::isArtist() const
{
    return (!m_artist.isEmpty()) ? true : false;
}

//-------------------------------------------------------------------------------------------

QString& TrackModelKey::artist()
{
    return m_artist;
}

//-------------------------------------------------------------------------------------------

const QString& TrackModelKey::artist() const
{
    return m_artist;
}

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------

