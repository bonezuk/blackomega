//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYERAPP_PLAYERAPP_PLAYERAPPMAIN_H
#define __OMEGA_PLAYERAPP_PLAYERAPP_PLAYERAPPMAIN_H
//-------------------------------------------------------------------------------------------

#include <QGuiApplication>

#include "track/db/inc/TrackDB.h"
#include "track/db/inc/DBInfo.h"

#include "playerapp/playercommon/inc/PlaybackStateController.h"
#include "playerapp/playercommon/inc/PlayListModel.h"
#include "playerapp/playercommon/inc/PlaylistLoadFunctions.h"
#include "playerapp/playercommon/inc/OmegaPlaylistInterface.h"

#include "playerapp/playerapp/inc/OmegaAudioIOInterface.h"
#include "playerapp/playerapp/inc/OmegaWebServicePLA.h"
#include "playerapp/playerapp/inc/PlayListWebModelPLA.h"
#include "playerapp/playerapp/inc/OmegaPLWebHandler.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PlayerAppMain : public QGuiApplication
{
    Q_OBJECT
    
    public:
        PlayerAppMain(const QString& rootDir, int& argc, char **argv);
        virtual ~PlayerAppMain();
        
        virtual bool initPlaylistManager(QVector<QPair<track::db::DBInfoSPtr,tint> >& playListDB);
        virtual QSharedPointer<PlaybackStateController>& getPlaybackState();
        virtual QSharedPointer<PlayListModel> getPlayListModel();
        
    private:
        QString m_rootDir;
        QSharedPointer<PlayListWebModelPLA> m_pModel;
        QSharedPointer<OmegaAudioIOInterface> m_pAudioInterface;
        QSharedPointer<OmegaPlaylistInterface> m_pPLInterface;
        QSharedPointer<OmegaWebServicePLA> m_pWebService;
        QSharedPointer<OmegaPLWebHandler> m_pWebHandler;
        
        virtual void printError(const char *strR, const char *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
