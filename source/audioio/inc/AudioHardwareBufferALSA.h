//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AUDIOHARDWAREBUFFERALSA_H
#define __OMEGA_AUDIOIO_AUDIOHARDWAREBUFFERALSA_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryALSA.h"
#include "audioio/inc/AbstractAudioHardwareBuffer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AudioHardwareBufferALSA : public AbstractAudioHardwareBuffer
{
    public:
        AudioHardwareBufferALSA(tint formatType,tint noFrames,tint noChannels, tbyte *pBuffer, tint bufferSize);
        virtual ~AudioHardwareBufferALSA();

        virtual tbyte *buffer(tint bufferIdx);

        virtual tint sampleSize(tint bufferIdx);

        virtual tint numberOfChannelsInBuffer(tint bufferIdx);

        virtual tint bufferLength();

        virtual tint numberOfBuffers();

        virtual tint numberOfBytesInBuffer();

    protected:

        tint m_formatType;
        tint m_noFrames;
        tint m_noChannels;
        tbyte *m_buffer;
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
