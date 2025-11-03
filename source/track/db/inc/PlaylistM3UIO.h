//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_PLAYLISTM3UIO_H
#define __OMEGA_TRACK_DB_PLAYLISTM3UIO_H
//-------------------------------------------------------------------------------------------

#include "track/db/inc/PlaylistAbstractIO.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT PlaylistM3UIO : public PlaylistAbstractIO
{
    public:
        PlaylistM3UIO();
        virtual ~PlaylistM3UIO();

        virtual bool load(const QString& fileName,QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress);
        virtual bool save(const QString& fileName,const QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress);
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
