//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AudioHardwareBufferALSA.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AudioHardwareBufferALSA::AudioHardwareBufferALSA(tint formatType, tint noFrames, tint noChannels, tbyte *pBuffer, tint bufferSize) : AbstractAudioHardwareBuffer(),
	m_formatType(formatType),
	m_noFrames(noFrames),
	m_noChannels(noChannels),
	m_buffer(0)
{
	if(pBuffer != 0 && AudioHardwareBufferALSA::numberOfBytesInBuffer() <= bufferSize)
	{
		m_buffer = pBuffer;
	}
}

//-------------------------------------------------------------------------------------------

AudioHardwareBufferALSA::~AudioHardwareBufferALSA()
{}

//-------------------------------------------------------------------------------------------

tbyte *AudioHardwareBufferALSA::buffer(tint bufferIdx)
{
	return (!bufferIdx) ? m_buffer : 0;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferALSA::sampleSize(tint bufferIdx)
{
	tint sSize;
	
	if(bufferIdx==0)
	{
		switch(m_formatType)
		{
			case SND_PCM_FORMAT_S8:
			case SND_PCM_FORMAT_DSD_U8:
				sSize = 1;
				break;
				
			case SND_PCM_FORMAT_S16_LE: 
			case SND_PCM_FORMAT_S16_BE:
            case SND_PCM_FORMAT_DSD_U16_LE:
            case SND_PCM_FORMAT_DSD_U16_BE:
				sSize = 2;
				break;
				
			case SND_PCM_FORMAT_S24_LE: 
			case SND_PCM_FORMAT_S24_BE: 
			case SND_PCM_FORMAT_S32_LE: 
			case SND_PCM_FORMAT_S32_BE:
			case SND_PCM_FORMAT_FLOAT_LE:
			case SND_PCM_FORMAT_FLOAT_BE:
            case SND_PCM_FORMAT_DSD_U32_BE:
            case SND_PCM_FORMAT_DSD_U32_LE:
				sSize = 4;
				break;
				
			case SND_PCM_FORMAT_FLOAT64_LE:
			case SND_PCM_FORMAT_FLOAT64_BE:
				sSize = 8;
				break;
				
			case SND_PCM_FORMAT_S24_3LE:
			case SND_PCM_FORMAT_S24_3BE:
			case SND_PCM_FORMAT_S20_3LE:
			case SND_PCM_FORMAT_S20_3BE:
			case SND_PCM_FORMAT_S18_3LE:
			case SND_PCM_FORMAT_S18_3BE:
				sSize = 3;
				break;
				
			default:
				sSize = 0;
				break;
		}
	}
	else
	{
		sSize = 0;
	}
	return sSize;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferALSA::numberOfChannelsInBuffer(tint bufferIdx)
{
	return (!bufferIdx) ? m_noChannels : 0;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferALSA::bufferLength()
{
	return m_noFrames / numberOfOutputForEveryOneInputSamples();
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferALSA::numberOfBuffers()
{
	return 1;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferALSA::numberOfBytesInBuffer()
{
	return AudioHardwareBufferALSA::sampleSize(0) * m_noFrames * m_noChannels;
}

//-------------------------------------------------------------------------------------------

tint AudioHardwareBufferALSA::numberOfOutputForEveryOneInputSamples()
{
	int num;

	switch(m_formatType)
	{
		case SND_PCM_FORMAT_DSD_U8:
			num = 8;
			break;
			
		case SND_PCM_FORMAT_DSD_U16_LE:
		case SND_PCM_FORMAT_DSD_U16_BE:
			num = 4;
			break;
			
		case SND_PCM_FORMAT_DSD_U32_BE:
		case SND_PCM_FORMAT_DSD_U32_LE:
			num = 2;
			break;
		default:
			num = 1;
			break;
	}
	return num;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
