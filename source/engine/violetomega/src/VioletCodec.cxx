#include "engine/violetomega/inc/VioletCodec.h"
#include "common/inc/BIOBufferedStream.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CodecFactory,Codec, \
                            VioletCodecFactory,VioletCodec, \
                            "aiff",false)

//-------------------------------------------------------------------------------------------

VioletCodec::VioletCodec(QObject *parent) : InterleavedCodec(e_codecAIFF,parent),
	m_file(0),
	m_iffFile(),
	m_pCommon(),
	m_pSound(),
	m_buffer(0)
{}

//-------------------------------------------------------------------------------------------

VioletCodec::~VioletCodec()
{}

//-------------------------------------------------------------------------------------------

void VioletCodec::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "VioletCodec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool VioletCodec::open(const QString& name)
{
	bool res = false;
	
	close();
	
	m_file = new common::BIOBufferedStream(common::e_BIOStream_FileRead);

	if(m_file->open(name))
	{
		IFFFileSPtr pIFFFile(new IFFFile(m_file));
		IFFChunkSPtr pRoot;
		
		pRoot = pIFFFile->root();
		if(!pRoot.isNull())
		{
			IFFFormChunkSPtr pFormChunk = pRoot.dynamicCast<IFFFormChunk>();
			
			if(!pFormChunk.isNull() && pFormChunk->formType()==IFF_ID('A','I','F','F'))
			{
				IFFCommonChunkSPtr pCommon;
				IFFSoundChunkSPtr pSound;
				
				for(IFFFormChunk::Iterator ppI = pFormChunk->begin();ppI!=pFormChunk->end();ppI++)
				{
					IFFChunkSPtr pChunk = *ppI;
					
					if(pChunk->id()==IFF_COMM_ID && pCommon.isNull())
					{
						pCommon = pChunk.dynamicCast<IFFCommonChunk>();
						if(!pCommon.isNull())
						{
							if(!pCommon->scan())
							{
								pCommon.clear();
							}
						}
					}
					else if(pChunk->id()==IFF_SSND_ID && pSound.isNull())
					{
						pSound = pChunk.dynamicCast<IFFSoundChunk>();
						if(!pSound.isNull())
						{
							if(!pSound->scan())
							{
								pSound.clear();
							}
						}
					}
				}
				
				if(!pCommon.isNull() && !pSound.isNull())
				{
					m_buffer = new sample_t [1024 * noChannels()];
					m_pCommon = pCommon;
					m_pSound = pSound;
					m_pSound->setCommon(m_pCommon);
					m_state = 0;
					m_time = 0;
					res = true;
				}
			}
			else
			{
				QString errStr = "Given file '" + name + "' is not an AIFF audio file.";
				printError("open",errStr.toUtf8().constData());
			}
		}
		else
		{
			QString errStr = "Given file '" + name + "' is not an AIFF audio file.";
			printError("open",errStr.toUtf8().constData());
		}
	}
	else
	{
		QString errStr = "Failed to open file '" + name + "'";
		printError("open",errStr.toUtf8().constData());
	}
	
	if(!res)
	{
		close();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void VioletCodec::close()
{
	if(m_buffer!=0)
	{
		delete [] m_buffer;
		m_buffer = 0;
	}
	if(!m_pSound.isNull())
	{
		m_pSound.clear();
	}
	if(!m_pCommon.isNull())
	{
		m_pCommon.clear();
	}
	if(!m_iffFile.isNull())
	{
		m_iffFile.clear();
	}
	if(m_file!=0)
	{
		m_file->close();
		delete m_file;
	}
	m_state = -1;
}

//-------------------------------------------------------------------------------------------

bool VioletCodec::init()
{
	bool res = false;

	if(InterleavedCodec::init())
	{
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool VioletCodec::decodeNextPacket(int& outLen)
{
	bool res;
	
    outLen = m_pSound->read(m_buffer,1024,m_dataType);
	if(outLen==0)
	{
		res = false;
	}
	else
	{
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

char *VioletCodec::getPacketBuffer()
{
	return reinterpret_cast<char *>(m_buffer);
}

//-------------------------------------------------------------------------------------------

int VioletCodec::bytesPerSample()
{
	if(m_dataType == e_SampleInt16)
	{
		return sizeof(tint16);
	}
	else if(m_dataType == e_SampleInt24 || m_dataType == e_SampleInt32)
	{
		return sizeof(tint32);
	}
	else
	{
		return sizeof(sample_t);
	}
}

//-------------------------------------------------------------------------------------------

sample_t VioletCodec::readSample(char *buffer)
{
    sample_t *x = reinterpret_cast<sample_t *>(buffer);
	return static_cast<sample_t>(*x);
}

//-------------------------------------------------------------------------------------------

bool VioletCodec::isSeek() const
{
	return true;
}

//-------------------------------------------------------------------------------------------

bool VioletCodec::seek(const common::TimeStamp& v)
{
	bool res = false;
	
	if(!m_pSound.isNull())
	{
		int idx = static_cast<int>(::floor(static_cast<tfloat64>(v) * frequency()));
		
		if(m_pSound->seek(idx))
		{
			m_time = m_pSound->currentTime();
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool VioletCodec::isComplete() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

bool VioletCodec::isRemote() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

bool VioletCodec::isBuffered(tfloat32& percent)
{
	return false;
}

//-------------------------------------------------------------------------------------------

tint VioletCodec::bitrate() const
{
	return (!m_pSound.isNull()) ? m_pSound->bitrate() : 0;
}

//-------------------------------------------------------------------------------------------

tint VioletCodec::frequency() const
{
	return (!m_pCommon.isNull()) ? (static_cast<tint>(m_pCommon->sampleRate())) : 44100;
}

//-------------------------------------------------------------------------------------------

tint VioletCodec::noChannels() const
{
	return (!m_pSound.isNull()) ? m_pSound->noOutChannels() : 2;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp VioletCodec::length() const
{
	common::TimeStamp len;
	
	if(!m_pSound.isNull() && !m_pCommon.isNull())
	{
		len = static_cast<tfloat64>(m_pSound->numberOfSamples()) / m_pCommon->sampleRate();
	}
	return len;
}

//-------------------------------------------------------------------------------------------

CodecDataType VioletCodec::dataTypesSupported() const
{
	CodecDataType types = e_SampleFloat;
	
	if(!m_pSound.isNull())
	{
		if(m_pSound->bytesPerSample() <= 2)
		{
			types |= e_SampleInt16;
		}
		else if(m_pSound->bytesPerSample() == 3)
		{
			types |= e_SampleInt24;
		}
		else 
		{
			types |= e_SampleInt32;
		}
	}
	return types;
}

//-------------------------------------------------------------------------------------------

bool VioletCodec::setDataTypeFormat(CodecDataType type)
{
	bool res;
	CodecDataType caps;
	
	caps = dataTypesSupported();
	if((type == e_SampleInt16 && (caps & e_SampleInt16)) || (type == e_SampleInt24 && (caps & e_SampleInt24)) || (type == e_SampleInt32 && (caps & e_SampleInt32)))
	{
		m_dataType = type;
		res = true;
	}
	else
	{
		res = Codec::setDataTypeFormat(type);
	}
	return res;
}

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

