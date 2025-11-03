#include "network/inc/Resource.h"
#if defined(OMEGA_WIN32)
#include "audioio/inc/WasAPIIF.h"
#endif
#include "playerapp/playerios/inc/PlayerAudioIOInterface.h"
#if defined(OMEGA_IOS)
#include "audioio/inc/AOCoreAudioIOS.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlayerAudioIOInterface::PlayerAudioIOInterface(QSharedPointer<OmegaPlaylistInterface>& pPLInterface, QObject *parent) : OmegaAudioInterface(parent),
    m_audio(),
    m_pPLInterface(pPLInterface),
    m_isUpdateRunning(false)
{}

//-------------------------------------------------------------------------------------------

PlayerAudioIOInterface::~PlayerAudioIOInterface()
{
    m_audio.clear();
    m_pPLInterface.clear();
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "PlayerAudioIOInterface::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool PlayerAudioIOInterface::init()
{
    bool res = false;

#if defined(OMEGA_WIN32)
    if(audioio::WasAPIIF::instance("wasapi").isNull())
    {
        printError("init", "Failed to initialise WasAPI interface");
        return false;
    }
    m_audio = audioio::AOBase::get("win32");
#elif defined(OMEGA_MACOSX)
    m_audio = audioio::AOBase::get("coreaudio");
#elif defined(OMEGA_LINUX)
    m_audio = audioio::AOBase::get("alsa");
#elif defined(OMEGA_IOS)
    m_audio = audioio::AOBase::get("ios");
#elif defined(OMEGA_ANDROID)

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
        res = true;
    }
    else
    {
        printError("init", "Failed to start audio engine");
        quitDaemon();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::quitDaemon()
{
    if(!m_audio.isNull())
    {
        m_audio->stop();
        m_audio.clear();
    }
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::playFile(const QString& fileName, bool isNext)
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

void PlayerAudioIOInterface::playFileWithTime(const QString& fileName, const common::TimeStamp& start,const common::TimeStamp& length, bool isNext)
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

void PlayerAudioIOInterface::play()
{
    m_audio->play();
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::pause()
{
    m_audio->pause();
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::stop()
{
    m_audio->stop();
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::seek(const common::TimeStamp& t)
{
    m_audio->seek(t);
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::onAudioStart(const QString& name)
{
    m_pPLInterface->onAudioStart(name);
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::onAudioPlay()
{
    m_pPLInterface->onAudioPlay();
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::onAudioStop()
{
    m_pPLInterface->onAudioStop();
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::onAudioPause()
{
    m_pPLInterface->onAudioPause();
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::onAudioTime(quint64 t)
{
    m_pPLInterface->playbackTime(t);
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::onAudioBuffer(tfloat32 percent)
{
    m_pPLInterface->onAudioBuffer(percent);
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::onAudioReadyForNext()
{
    m_pPLInterface->onAudioReadyForNext();
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::onAudioNoNext()
{
    m_pPLInterface->onAudioNoNext();
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::onAudioCrossfade()
{
    m_pPLInterface->onAudioCrossfade();
}

//-------------------------------------------------------------------------------------------

bool PlayerAudioIOInterface::getIsUpdateRunning() const
{
    return m_isUpdateRunning;
}

//-------------------------------------------------------------------------------------------

void PlayerAudioIOInterface::update()
{
#if defined(OMEGA_IOS)
    QSharedPointer<audioio::AOCoreAudioIOS> pAudioIOS = m_audio.dynamicCast<audioio::AOCoreAudioIOS>();
    if(!pAudioIOS.isNull())
    {
        if(pAudioIOS->isUpdateRequired())
        {
            m_isUpdateRunning = true;
            emit onUpdateRunning();

            pAudioIOS->updateCurrentDevice();

            m_isUpdateRunning = false;
            emit onUpdateRunning();
        }
    }
#endif
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
