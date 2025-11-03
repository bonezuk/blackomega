#include "network/inc/Resource.h"
#if defined(OMEGA_WIN32)
#include "audioio/inc/WasAPIIF.h"
#endif
#include "playerapp/playerapp/inc/OmegaAudioIOInterface.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

OmegaAudioIOInterface::OmegaAudioIOInterface(QSharedPointer<OmegaPlaylistInterface>& pPLInterface, QObject *parent) : OmegaAudioInterface(parent),
    m_audio(),
    m_pPLInterface(pPLInterface)
{}

//-------------------------------------------------------------------------------------------

OmegaAudioIOInterface::~OmegaAudioIOInterface()
{
    m_audio.clear();
    m_pPLInterface.clear();
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "OmegaAudioIOInterface::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool OmegaAudioIOInterface::init()
{
    bool res = false;

#if defined(OMEGA_WIN32)
    m_audio = audioio::AOBase::get("win32");
#elif defined(OMEGA_MACOSX)
    m_audio = audioio::AOBase::get("coreaudio");
#elif defined(OMEGA_LINUX)
    m_audio = audioio::AOBase::get("alsa");
#endif

    if(!m_audio.isNull())
    {
        connect(m_audio.data(),SIGNAL(onStart(const QString&)),this,SLOT(onAudioStart(const QString&)));
        connect(m_audio.data(),SIGNAL(onStop()),this,SLOT(onAudioStop()));
        connect(m_audio.data(),SIGNAL(onPlay()),this,SLOT(onAudioPlay()));
        connect(m_audio.data(),SIGNAL(onPause()),this,SLOT(onAudioPause()));
        connect(m_audio.data(),SIGNAL(onTime(quint64)),this,SLOT(onAudioTime(quint64)));
        connect(m_audio.data(),SIGNAL(onBuffer(tfloat32)),this,SLOT(onAudioBuffer(tfloat32)));
        connect(m_audio.data(),SIGNAL(onReadyForNext()),this,SLOT(onAudioReadyForNext()));
        connect(m_audio.data(),SIGNAL(onNoNext()),this,SLOT(onAudioNoNext()));
        connect(m_audio.data(),SIGNAL(onCrossfade()),this,SLOT(onAudioCrossfade()));
        common::Log::g_Log << "Audio Daemon running..."<< common::c_endl;

        res = true;
    }
    else
    {
        printError("OmegaAudioDaemon", "Failed to start audio engine");
        quitDaemon();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::quitDaemon()
{
    common::Log::g_Log << "quitDaemon" << common::c_endl;
    if(!m_audio.isNull())
    {
        m_audio->stop();
        m_audio.clear();
    }
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::playFile(const QString& fileName, bool isNext)
{
    if(common::DiskOps::exist(fileName))
    {
        if(isNext)
        {
            m_audio->next(fileName);
        }
        else
        {
            m_audio->open(fileName);
        }
    }
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::playFileWithTime(const QString& fileName, const common::TimeStamp& start,const common::TimeStamp& length, bool isNext)
{
    if(common::DiskOps::exist(fileName))
    {
        if(isNext)
        {
            m_audio->next(fileName, start, length);
        }
        else
        {
            m_audio->open(fileName, start, length);
        }
    }
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::play()
{
    common::Log::g_Log << "play" << common::c_endl;
    m_audio->play();
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::pause()
{
    common::Log::g_Log << "pause" << common::c_endl;
    m_audio->pause();
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::seek(const common::TimeStamp& t)
{
    m_audio->seek(t);
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::stop()
{
    m_audio->stop();
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::onAudioStart(const QString& name)
{
    common::Log::g_Log << "onAudioStart - " << name << common::c_endl;
    m_pPLInterface->onAudioStart(name);
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::onAudioPlay()
{
    common::Log::g_Log << "onAudioPlay" << common::c_endl;
    m_pPLInterface->onAudioPlay();
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::onAudioStop()
{
    common::Log::g_Log << "onAudioStop" << common::c_endl;
    m_pPLInterface->onAudioStop();
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::onAudioPause()
{
    common::Log::g_Log << "onAudioPause" << common::c_endl;
    m_pPLInterface->onAudioPause();
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::onAudioTime(quint64 t)
{
    common::TimeStamp tS(t);
    m_pPLInterface->playbackTime(t);
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::onAudioBuffer(tfloat32 percent)
{
    common::Log::g_Log << "onAudioBuffer" << common::c_endl;
    m_pPLInterface->onAudioBuffer(percent);
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::onAudioReadyForNext()
{
    common::Log::g_Log << "onAudioReadyForNext" << common::c_endl;
    m_pPLInterface->onAudioReadyForNext();
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::onAudioNoNext()
{
    common::Log::g_Log << "onAudioNoNext" << common::c_endl;
    m_pPLInterface->onAudioNoNext();
}

//-------------------------------------------------------------------------------------------

void OmegaAudioIOInterface::onAudioCrossfade()
{
    common::Log::g_Log << "onAudioCrossfade" << common::c_endl;
    m_pPLInterface->onAudioCrossfade();
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
