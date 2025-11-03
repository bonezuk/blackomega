#include "playerapp/playercommon/inc/OmegaPlaylistInterface.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

OmegaPlaylistInterface::OmegaPlaylistInterface(QObject *parent) : QObject(parent),
    m_pModel()
{}

//-------------------------------------------------------------------------------------------

OmegaPlaylistInterface::~OmegaPlaylistInterface()
{
    if(!m_pModel.isNull())
    {
        m_pModel.clear();
    }
}

//-------------------------------------------------------------------------------------------

void OmegaPlaylistInterface::init(QSharedPointer<PlayListModel>& pModel)
{
    m_pModel = pModel;
}

//-------------------------------------------------------------------------------------------

void OmegaPlaylistInterface::playbackTime(quint64 tS)
{
    m_pModel->playbackState()->setTime(tS);
}

//-------------------------------------------------------------------------------------------

void OmegaPlaylistInterface::onAudioStart(const QString& name)
{
    m_pModel->playbackState()->onAudioStart(name);
}

//-------------------------------------------------------------------------------------------

void OmegaPlaylistInterface::onAudioPlay()
{
    m_pModel->playbackState()->onAudioPlay();
}

//-------------------------------------------------------------------------------------------

void OmegaPlaylistInterface::onAudioPause()
{
    m_pModel->playbackState()->onAudioPause();
}

//-------------------------------------------------------------------------------------------

void OmegaPlaylistInterface::onAudioStop()
{
    m_pModel->playNextItem(false);
}

//-------------------------------------------------------------------------------------------

void OmegaPlaylistInterface::onAudioBuffer(tfloat32 percent)
{}

//-------------------------------------------------------------------------------------------

void OmegaPlaylistInterface::onAudioReadyForNext()
{
    m_pModel->playNextItem(true);
}

//-------------------------------------------------------------------------------------------

void OmegaPlaylistInterface::onAudioNoNext()
{}

//-------------------------------------------------------------------------------------------

void OmegaPlaylistInterface::onAudioCrossfade()
{}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
