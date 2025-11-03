//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_SPOTIFY_SPOTIFTYAPIKEY_H
#define __OMEGA_TRACK_SPOTIFY_SPOTIFTYAPIKEY_H
//-------------------------------------------------------------------------------------------

#include "api.h"

#include <QSharedPointer>

#include "track/spotify/inc/SpotifyDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace spotify
{
//-------------------------------------------------------------------------------------------

class TRACK_SPOTIFY_EXPORT SpotifyAPIKey
{
    public:
        virtual ~SpotifyAPIKey();

        static QSharedPointer<SpotifyAPIKey> instance();

        virtual const QByteArray& key() const;

    private:

        static QSharedPointer<SpotifyAPIKey> m_instance;

        QByteArray m_key;

        SpotifyAPIKey();
};

typedef QSharedPointer<SpotifyAPIKey> SpotifyAPIKeySPtr;

//-------------------------------------------------------------------------------------------
} // namespace spotify
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
