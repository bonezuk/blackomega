//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERIOS_PLAYLISTIOSMODEL_H
#define __PLAYERAPP_PLAYERIOS_PLAYLISTIOSMODEL_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "common/inc/DiskOps.h"
#include "track/db/inc/TrackDB.h"
#include "track/db/inc/DBInfo.h"

#include "playerapp/playercommon/inc/PlayListModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PlayListIOSModel : public PlayListModel
{
    public:
        Q_OBJECT
    public:
        PlayListIOSModel(QObject *parent = 0);
        PlayListIOSModel(QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent = 0);
        virtual ~PlayListIOSModel();

        virtual bool initialise();

    public slots:
        virtual void appendTrack(const QString& fileName);
        virtual void deleteTrack(const QString& fileName);

    protected:
        virtual void printError(const tchar *strE, const tchar *strR) const;
        virtual void removeAtIndex(int index);
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
