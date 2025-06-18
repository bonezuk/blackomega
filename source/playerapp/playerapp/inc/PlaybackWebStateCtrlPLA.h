//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERAPP_PLAYBACKWEBSTATECTRLPLA_H
#define __PLAYERAPP_PLAYERAPP_PLAYBACKWEBSTATECTRLPLA_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlaybackWebStateController.h"
#include "playerapp/playercommon/inc/WebEventRegisterInterface.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PlaybackWebStateCtrlPLA : public PlaybackWebStateController
{
    Q_OBJECT
    public:
        PlaybackWebStateCtrlPLA(QObject *parent = 0);
        PlaybackWebStateCtrlPLA(QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent = 0);
        virtual ~PlaybackWebStateCtrlPLA();

        QSharedPointer<WebEventRegisterInterface>& webEventRegisterInterface();

    protected:
        QSharedPointer<WebEventRegisterInterface> m_webEventRegisterInterface;

        virtual void initWebController();
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
