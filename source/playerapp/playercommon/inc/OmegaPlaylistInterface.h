//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYERAPP_PLAYERCOMMON_OMEGAPLAYLISTINTERFACE_H
#define __OMEGA_PLAYERAPP_PLAYERCOMMON_OMEGAPLAYLISTINTERFACE_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"

#include <QtGlobal>
#include <QSharedPointer>

#include "common/inc/CommonTypes.h"
#include "playerapp/playercommon/inc/PlayListModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT OmegaPlaylistInterface : public QObject
{
    public:
        OmegaPlaylistInterface(QObject *parent = 0);
        virtual ~OmegaPlaylistInterface();

        virtual void init(QSharedPointer<PlayListModel>& pModel);
        
        virtual void playbackTime(quint64 tS);
        virtual void onAudioStart(const QString& name);
        virtual void onAudioPlay();
        virtual void onAudioPause();
        virtual void onAudioStop();
        virtual void onAudioBuffer(tfloat32 percent);
        virtual void onAudioReadyForNext();
        virtual void onAudioNoNext();
        virtual void onAudioCrossfade();
        
    private:
        QSharedPointer<PlayListModel> m_pModel;
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
