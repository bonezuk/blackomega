#include "playerapp/playerapp/inc/PlayListWebModelPLA.h"
#include "playerapp/playerapp/inc/PlaybackWebStateCtrlPLA.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlayListWebModelPLA::PlayListWebModelPLA(QObject *parent) : PlayListWebModel(parent)
{}

//-------------------------------------------------------------------------------------------

PlayListWebModelPLA::PlayListWebModelPLA(QVector<QPair<track::db::DBInfoSPtr,tint> >& playList, QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent) : PlayListWebModel(playList, pAudioInterface, parent)
{}

//-------------------------------------------------------------------------------------------

PlayListWebModelPLA::~PlayListWebModelPLA()
{}

//-------------------------------------------------------------------------------------------

bool PlayListWebModelPLA::initialise()
{
    QSharedPointer<PlaybackWebStateCtrlPLA> plState(new PlaybackWebStateCtrlPLA(m_pAudioInterface, this));
    m_pPlaybackState = plState.dynamicCast<PlaybackStateController>();
    plState->init();
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
