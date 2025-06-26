#include "audioio/inc/AudioHardwareBufferWasAPI.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AudioHardwareBufferWasAPI::AudioHardwareBufferWasAPI(WAVEFORMATEX *pFormat,tbyte *pBuffer,tint numberOfFrames) : AbstractAudioHardwareBuffer(),
    m_pFormat(pFormat),
    m_pBuffer(pBuffer),
    m_numberOfFrames(numberOfFrames)
{}

//-------------------------------------------------------------------------------------------

AudioHardwareBufferWasAPI::~AudioHardwareBufferWasAPI()
{}

//-------------------------------------------------------------------------------------------

tbyte *AudioHardwareBufferWasAPI::buffer(tint bufferIdx)
{
    return m_pBuffer;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferWasAPI::sampleSize(tint bufferIdx)
{
    return m_pFormat->wBitsPerSample >> 3;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferWasAPI::numberOfChannelsInBuffer(tint bufferIdx)
{
    return m_pFormat->nChannels;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferWasAPI::bufferLength()
{
    return m_numberOfFrames;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferWasAPI::numberOfBuffers()
{
    return 1;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
