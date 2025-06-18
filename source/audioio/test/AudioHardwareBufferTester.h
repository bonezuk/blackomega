//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIOTEST_AUDIOHARDWAREBUFFERTESTER_H
#define __OMEGA_AUDIOIOTEST_AUDIOHARDWAREBUFFERTESTER_H
//-------------------------------------------------------------------------------------------

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/AbstractAudioHardwareBuffer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AudioHardwareBufferTester : public AbstractAudioHardwareBuffer
{
    public:
        static const tint c_bufferLength = 5;
        
    public:
        AudioHardwareBufferTester(const tint *channels,tint noBuffers);
        AudioHardwareBufferTester(const tint *channels,tint noBuffers,tint sampleSize,tint bLength);
        virtual ~AudioHardwareBufferTester();
        
        virtual tbyte *buffer(tint bufferIdx);
        
        virtual tint sampleSize(tint bufferIdx);
        
        virtual tint numberOfChannelsInBuffer(tint bufferIdx);
        
        virtual tint bufferLength();
        
        virtual tint numberOfBuffers();
        
    protected:
    
        tbyte **m_buffers;
        tint *m_channelCount;
        tint m_noBuffers;
        
        tint m_sampleSize;
        tint m_bufferLength;
        
        void allocate(const tint *channels,tint noBuffers,tint sampleSize,tint bLength);
        void deallocate();
};

//-------------------------------------------------------------------------------------------

class AudioHardwareBufferMock : public AbstractAudioHardwareBuffer
{
    public:
        AudioHardwareBufferMock();
        virtual ~AudioHardwareBufferMock();
        
        MOCK_METHOD1(buffer,tbyte *(tint bufferIdx));
        MOCK_METHOD1(sampleSize,tint (tint bufferIdx));
        MOCK_METHOD1(numberOfChannelsInBuffer,tint(tint bufferIdx));
        MOCK_METHOD0(bufferLength,tint());
        MOCK_METHOD0(numberOfBuffers,tint());
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
