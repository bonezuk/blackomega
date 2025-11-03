#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/test/AudioHardwareBufferTester.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------
// AudioHardwareBufferTester
//-------------------------------------------------------------------------------------------

AudioHardwareBufferTester::AudioHardwareBufferTester(const tint *channels,tint noBuffers) : AbstractAudioHardwareBuffer()
{
    allocate(channels,noBuffers,sizeof(tfloat32),c_bufferLength);
}

AudioHardwareBufferTester::AudioHardwareBufferTester(const tint *channels,tint noBuffers,tint sampleSize,tint bLength)
{
    allocate(channels,noBuffers,sampleSize,bLength);
}

//-------------------------------------------------------------------------------------------

AudioHardwareBufferTester::~AudioHardwareBufferTester()
{
    deallocate();
}

//-------------------------------------------------------------------------------------------

void AudioHardwareBufferTester::allocate(const tint *channels,tint noBuffers,tint sampleSize,tint bLength)
{
    m_buffers = reinterpret_cast<tbyte **>(malloc(noBuffers * sizeof(tbyte *)));
    m_channelCount = new tint [noBuffers];
    for(tint i=0;i<noBuffers;i++)
    {
        tint noChs = channels[i];
        m_buffers[i] = new tbyte [noChs * bLength * sampleSize];
        m_channelCount[i] = noChs;
    }
    m_noBuffers = noBuffers;
    m_sampleSize = sampleSize;
    m_bufferLength = bLength;
}

//-------------------------------------------------------------------------------------------

void AudioHardwareBufferTester::deallocate()
{
    for(tint i=0;i<m_noBuffers;i++)
    {
        tbyte *b = m_buffers[i];
        delete [] b;
    }
    delete [] m_channelCount;
    free(m_buffers);    
}

//-------------------------------------------------------------------------------------------

tbyte *AudioHardwareBufferTester::buffer(tint bufferIdx)
{
    return m_buffers[bufferIdx];
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferTester::sampleSize(tint bufferIdx)
{
    return m_sampleSize;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferTester::numberOfChannelsInBuffer(tint bufferIdx)
{
    return m_channelCount[bufferIdx];
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferTester::bufferLength()
{
    return m_bufferLength;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferTester::numberOfBuffers()
{
    return m_noBuffers;
}

//-------------------------------------------------------------------------------------------
// AudioHardwareBufferMock
//-------------------------------------------------------------------------------------------

AudioHardwareBufferMock::AudioHardwareBufferMock() : AbstractAudioHardwareBuffer()
{}

//-------------------------------------------------------------------------------------------

AudioHardwareBufferMock::~AudioHardwareBufferMock()
{}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
