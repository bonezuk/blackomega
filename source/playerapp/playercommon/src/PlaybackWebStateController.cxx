#include "playerapp/playercommon/inc/PlaybackWebStateController.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlaybackWebStateController::PlaybackWebStateController(QObject *parent) : PlaybackStateController(parent),
    m_pEventInterface()
{}

//-------------------------------------------------------------------------------------------

PlaybackWebStateController::PlaybackWebStateController(QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent) : PlaybackStateController(pAudioInterface, parent),
    m_pEventInterface()
{}

//-------------------------------------------------------------------------------------------

PlaybackWebStateController::~PlaybackWebStateController()
{}

//-------------------------------------------------------------------------------------------

void PlaybackWebStateController::init()
{
    initWebController();
}

//-------------------------------------------------------------------------------------------

void PlaybackWebStateController::setTime(quint64 tS)
{
    PlaybackStateController::setTime(tS);
    m_pEventInterface->onPlaybackTime(m_playbackTime);
}

//-------------------------------------------------------------------------------------------

void PlaybackWebStateController::onAudioStart(const QString& fileName)
{
    PlaybackStateController::onAudioStart(fileName);
    m_pEventInterface->onAudioStart(m_currentId);
}

//-------------------------------------------------------------------------------------------

void PlaybackWebStateController::onAudioPlay()
{
    PlaybackStateController::onAudioPlay();
    m_pEventInterface->onAudioPlaybackEvent(true, m_currentId);
}

//-------------------------------------------------------------------------------------------

void PlaybackWebStateController::onAudioPause()
{
    PlaybackStateController::onAudioPause();
    m_pEventInterface->onAudioPlaybackEvent(false, m_currentId);
}

//-------------------------------------------------------------------------------------------

void PlaybackWebStateController::onAudioStop()
{
    PlaybackStateController::onAudioStop();
    m_pEventInterface->onAudioPlaybackEvent(false, 0);
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
