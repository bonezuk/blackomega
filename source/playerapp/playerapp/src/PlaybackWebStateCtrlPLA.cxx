#include "playerapp/playerapp/inc/PlaybackWebStateCtrlPLA.h"
#include "playerapp/playerapp/inc/OmegaWebEventHandler.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlaybackWebStateCtrlPLA::PlaybackWebStateCtrlPLA(QObject *parent) : PlaybackWebStateController(parent),
    m_webEventRegisterInterface()
{}

//-------------------------------------------------------------------------------------------

PlaybackWebStateCtrlPLA::PlaybackWebStateCtrlPLA(QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent) : PlaybackWebStateController(pAudioInterface, parent)
{}

//-------------------------------------------------------------------------------------------

PlaybackWebStateCtrlPLA::~PlaybackWebStateCtrlPLA()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<WebEventRegisterInterface>& PlaybackWebStateCtrlPLA::webEventRegisterInterface()
{
    return m_webEventRegisterInterface;
}

//-------------------------------------------------------------------------------------------

void PlaybackWebStateCtrlPLA::initWebController()
{
    QSharedPointer<OmegaWebEventHandler> pEventInterface(new OmegaWebEventHandler());
    m_pEventInterface = pEventInterface.dynamicCast<WebEventInterface>();
    m_webEventRegisterInterface = pEventInterface.dynamicCast<WebEventRegisterInterface>();
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
