//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_AUDIOHARDWAREBUFFERASIO_H
#define __OMEGA_AUDIOIO_AUDIOHARDWAREBUFFERASIO_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOQueryASIO.h"
#include "audioio/inc/AbstractAudioHardwareBuffer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT AudioHardwareBufferASIO : public AbstractAudioHardwareBuffer
{
    public:
        AudioHardwareBufferASIO(ASIOBufferInfo *pBufferInfos,ASIOChannelInfo *pChannelInfos,tint bufferSize,tint noBuffers,tint index);
        virtual ~AudioHardwareBufferASIO();
        
        virtual tbyte *buffer(tint bufferIdx);
        
        virtual tint sampleSize(tint bufferIdx);
        
        virtual tint numberOfChannelsInBuffer(tint bufferIdx);
        
        virtual tint bufferLength();
        
        virtual tint numberOfBuffers();
        
    protected:
    
        ASIOBufferInfo *m_bufferInfos;
        ASIOChannelInfo *m_channelInfos;
        tint m_bufferSize;
        tint m_noBuffers;
        tint m_index;
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
