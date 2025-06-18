#include "audioio/inc/AudioHardwareBufferCoreAudio.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AudioHardwareBufferCoreAudio::AudioHardwareBufferCoreAudio(AudioBufferList *ioData,UInt32 inNumberFrames,tint sSize) : AbstractAudioHardwareBuffer(),
    m_ioData(ioData),
    m_inNumberFrames(inNumberFrames),
    m_sampleSize(sSize)
{}

//-------------------------------------------------------------------------------------------

AudioHardwareBufferCoreAudio::~AudioHardwareBufferCoreAudio()
{}

//-------------------------------------------------------------------------------------------

tbyte *AudioHardwareBufferCoreAudio::buffer(tint bufferIdx)
{
    return reinterpret_cast<tbyte *>(m_ioData->mBuffers[bufferIdx].mData);
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferCoreAudio::sampleSize(tint bufferIdx)
{
    return m_sampleSize;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferCoreAudio::numberOfChannelsInBuffer(tint bufferIdx)
{
    return static_cast<tint>(m_ioData->mBuffers[bufferIdx].mNumberChannels);
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferCoreAudio::bufferLength()
{
    return static_cast<tint>(m_inNumberFrames);
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferCoreAudio::numberOfBuffers()
{
    return m_ioData->mNumberBuffers;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
