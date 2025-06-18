//-------------------------------------------------------------------------------------------
#if defined(OMEGA_IOS)
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AudioHardwareBufferCoreAudioIOS.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AudioHardwareBufferCoreAudioIOS::AudioHardwareBufferCoreAudioIOS(AudioBufferList *ioData,UInt32 inNumberFrames,tint sSize) : AbstractAudioHardwareBuffer(),
    m_ioData(ioData),
    m_inNumberFrames(inNumberFrames),
    m_sampleSize(sSize)
{}

//-------------------------------------------------------------------------------------------

AudioHardwareBufferCoreAudioIOS::~AudioHardwareBufferCoreAudioIOS()
{}

//-------------------------------------------------------------------------------------------

tbyte *AudioHardwareBufferCoreAudioIOS::buffer(tint bufferIdx)
{
    return reinterpret_cast<tbyte *>(m_ioData->mBuffers[bufferIdx].mData);
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferCoreAudioIOS::sampleSize(tint bufferIdx)
{
    return m_sampleSize;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferCoreAudioIOS::numberOfChannelsInBuffer(tint bufferIdx)
{
    return static_cast<tint>(m_ioData->mBuffers[bufferIdx].mNumberChannels);
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferCoreAudioIOS::bufferLength()
{
    return static_cast<tint>(m_inNumberFrames);
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferCoreAudioIOS::numberOfBuffers()
{
    return m_ioData->mNumberBuffers;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
