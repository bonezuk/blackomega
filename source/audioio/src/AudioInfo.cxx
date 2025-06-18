#include "audioio/inc/AudioInfo.h"
#include <QSettings>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AudioInfo *AudioInfo::m_instance = 0;

//-------------------------------------------------------------------------------------------

AudioInfo::AudioInfo() : m_errorMap()
{
#if defined(OMEGA_WIN32)
    m_errorMap.insert(MMSYSERR_INVALHANDLE,QString::fromLatin1("Specified device handle is invalid"));
    m_errorMap.insert(MMSYSERR_NODRIVER,QString::fromLatin1("No device driver is present"));
    m_errorMap.insert(MMSYSERR_NOMEM,QString::fromLatin1("Unable to allocate or lock memory"));
    m_errorMap.insert(MMSYSERR_ALLOCATED,QString::fromLatin1("Specified resource is already allocated"));
    m_errorMap.insert(MMSYSERR_BADDEVICEID,QString::fromLatin1("Specified device identifier is out of range"));
    m_errorMap.insert(WAVERR_BADFORMAT,QString::fromLatin1("Attempted to open with an unsupported waveform-audio format"));
    m_errorMap.insert(WAVERR_SYNC,QString::fromLatin1("The device is synchronous but waveOutOpen was called without using the WAVE_ALLOWSYNC flag"));
    m_errorMap.insert(WAVERR_UNPREPARED,QString::fromLatin1("The data block pointed to by the pwh parameter hasn't been prepared"));
    m_errorMap.insert(MMSYSERR_NOTSUPPORTED,QString::fromLatin1("Specified device is synchronous and does not support pausing"));
    m_errorMap.insert(WAVERR_STILLPLAYING,QString::fromLatin1("The data block pointed to by the pwh parameter is still in the queue"));
#endif
}

//-------------------------------------------------------------------------------------------

AudioInfo::~AudioInfo()
{
    m_instance = 0;
}

//-------------------------------------------------------------------------------------------

void AudioInfo::start()
{
    if(m_instance==0)
    {
        m_instance = new AudioInfo;
    }
}

//-------------------------------------------------------------------------------------------

void AudioInfo::stop()
{
    if(m_instance!=0)
    {
        delete m_instance;
        m_instance = 0;
    }
}

//-------------------------------------------------------------------------------------------

AudioInfo& AudioInfo::instance()
{
    return *m_instance;
}

//-------------------------------------------------------------------------------------------

QString AudioInfo::error(tint r) const
{
    QString eS;
    QMap<tint,QString>::const_iterator ppI = m_errorMap.find(r);
    
    if(ppI!=m_errorMap.end())
    {
        eS = ppI.value();
    }
    return eS;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
