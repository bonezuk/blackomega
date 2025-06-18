#include "audioio/inc/AudioHardwareBufferASIO.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AudioHardwareBufferASIO::AudioHardwareBufferASIO(ASIOBufferInfo *pBufferInfos,ASIOChannelInfo *pChannelInfos,tint bufferSize,tint noBuffers,tint index) : AbstractAudioHardwareBuffer(),
    m_bufferInfos(pBufferInfos),
    m_channelInfos(pChannelInfos),
    m_bufferSize(bufferSize),
    m_noBuffers(noBuffers),
    m_index(index)
{}

//-------------------------------------------------------------------------------------------

AudioHardwareBufferASIO::~AudioHardwareBufferASIO()
{}

//-------------------------------------------------------------------------------------------

tbyte *AudioHardwareBufferASIO::buffer(tint bufferIdx)
{
    return reinterpret_cast<tbyte *>(m_bufferInfos[bufferIdx].buffers[m_index]);
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferASIO::sampleSize(tint bufferIdx)
{
    return ASIODriverService::instance().getSampleSize(m_channelInfos[bufferIdx].type);
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferASIO::numberOfChannelsInBuffer(tint bufferIdx)
{
    return 1;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferASIO::bufferLength()
{
    return m_bufferSize;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferASIO::numberOfBuffers()
{
    return m_noBuffers;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
