//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERCOMMON_PLAYBACKWEBSTATECONTROLLER_H
#define __PLAYERAPP_PLAYERCOMMON_PLAYBACKWEBSTATECONTROLLER_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"
#include "playerapp/playercommon/inc/PlaybackStateController.h"
#include "playerapp/playercommon/inc/WebEventInterface.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT PlaybackWebStateController : public PlaybackStateController
{
    Q_OBJECT
    public:
        PlaybackWebStateController(QObject *parent = 0);
        PlaybackWebStateController(QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent = 0);
        virtual ~PlaybackWebStateController();

        virtual void init();

        virtual void setTime(quint64 tS);

        virtual void onAudioStart(const QString& fileName);
        virtual void onAudioPlay();
        virtual void onAudioPause();
        virtual void onAudioStop();

    protected:

        QSharedPointer<WebEventInterface> m_pEventInterface;

        virtual void initWebController() = 0;
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
