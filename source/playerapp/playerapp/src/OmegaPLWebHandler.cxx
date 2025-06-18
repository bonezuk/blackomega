#include "playerapp/playerapp/inc/OmegaPLWebHandler.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

OmegaPLWebHandler::OmegaPLWebHandler(QSharedPointer<OmegaWebInterface>& pPLWebInterface) : OmegaPLWebInterface(),
    m_pPLWebInterface(pPLWebInterface)
{}

//-------------------------------------------------------------------------------------------

OmegaPLWebHandler::~OmegaPLWebHandler()
{}

//-------------------------------------------------------------------------------------------

void OmegaPLWebHandler::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "OmegaPLWebInterface::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

QJsonDocument OmegaPLWebHandler::getFullPlaylist()
{
    int len = m_pPLWebInterface->playlistSize();
    QJsonDocument doc = m_pPLWebInterface->playlistAsJson(0, len);
    return doc;
}

//-------------------------------------------------------------------------------------------

QJsonDocument OmegaPLWebHandler::getPlaybackState()
{
    return m_pPLWebInterface->getPlaybackState();
}

//-------------------------------------------------------------------------------------------

void OmegaPLWebHandler::onPressPlay()
{
    m_pPLWebInterface->onPressPlay();
}

//-------------------------------------------------------------------------------------------

void OmegaPLWebHandler::onStartPlaying(tuint64 id)
{
    m_pPLWebInterface->onStartPlaying(id);
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
