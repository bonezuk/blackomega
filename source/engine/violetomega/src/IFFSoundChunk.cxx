#include "engine/violetomega/inc/IFFSoundChunk.h"
#include "engine/inc/FormatType.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(IFFChunkFactory,IFFChunk, \
                            IFFSoundChunkFactory,IFFSoundChunk, \
                            "SSND",false)

//-------------------------------------------------------------------------------------------

IFFSoundChunk::IFFSoundChunk() : IFFChunk(),
    m_offset(0),
	m_blockSize(0),
	m_noChannels(0),
	m_noSampleFrames(0),
	m_sampleSize(0),
	m_sampleRate(1.0),
	m_currentIndexPosition(0),
	m_readBlockMem(0),
	m_readBlockSize(0)
{}

//-------------------------------------------------------------------------------------------

IFFSoundChunk::~IFFSoundChunk()
{
	if(m_readBlockMem != 0)
	{
		delete [] m_readBlockMem;
	}
}

//-------------------------------------------------------------------------------------------

void IFFSoundChunk::setCommon(IFFCommonChunkSPtr pCommon)
{
	if(!pCommon.isNull())
	{
		m_noChannels = pCommon->noChannels();
		m_noSampleFrames = pCommon->noSampleFrames();
		m_sampleSize = pCommon->sampleSize();
		m_sampleRate = pCommon->sampleRate();
		m_formatType = pCommon->formatType();
	}
}

//-------------------------------------------------------------------------------------------

const tint& IFFSoundChunk::offset() const
{
	return m_offset;
}

//-------------------------------------------------------------------------------------------

const tint& IFFSoundChunk::blockSize() const
{
	return m_blockSize;
}

//-------------------------------------------------------------------------------------------

bool IFFSoundChunk::scan()
{
    bool res = false;

	if(m_file!=0 && filePositionToStart())
	{
		tbyte mem[8];
		
		if(m_file->read(mem,8)==8)
		{
			m_offset = static_cast<tint>(read32BitUnsigned(mem));
			m_blockSize = static_cast<tint>(read32BitUnsigned(&mem[4]));
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunk::noOutChannels() const
{
	return (m_noChannels==6) ? 7 : m_noChannels;
}

//-------------------------------------------------------------------------------------------

void IFFSoundChunk::sortOutputChannelsSample(const sample_t* in, sample_t* out)
{
	sortOutputChannels<sample_t>(in, out);
}

//-------------------------------------------------------------------------------------------

template<class X> void IFFSoundChunk::sortOutputChannels(const X *in, X *out)
{
	switch(m_noChannels)
	{
		case 1:
			out[0] = in[0];
			break;
			
		case 2:
			out[0] = in[0];
			out[1] = in[1];
			break;
			
		case 3:
			out[0] = in[0];
			out[1] = in[1];
			out[2] = in[2];
			break;
		
		case 4:
			out[0] = in[0];
			out[1] = in[1];
			out[2] = in[2];
			out[3] = in[3];
			break;
			
		case 6:
			out[0] = in[1];
			out[1] = in[4];
			out[2] = in[2];
			out[3] = in[0];
			out[4] = in[3];
			out[5] = in[5];
			out[6] = in[5];
			break;
			
		default:
			for(int i=0;i<m_noChannels;i++)
			{
				out[i] = in[i];
			}
			break;	
	}
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunk::readAsBlocks(sample_t *sampleMem, int noSamples, CodecDataType type)
{
	int amount = 0;
	tbyte mem[24];
	int cIndex,bPerFrame,bPerSample;
	bool res = true;
	
	bPerFrame = bytesPerFrame();
	bPerSample = bytesPerSample();

	cIndex = currentIndexPosition();
	if(m_file->seek(cIndex + 8,common::e_Seek_Current) && cIndex >= 0)
	{
		while(res && amount<noSamples)
		{
			int nIndex,diff;
			
			nIndex = nextIndexPosition();
			if(nIndex>=0)
			{
				diff = nIndex - cIndex;
				if(diff)
				{
					if(m_file->seek(diff,common::e_Seek_Current))
					{
						cIndex += diff;
					}
					else
					{
						res = false;
					}
				}
			}
			else
			{
				res = false;
			}

			if(res)
			{
				if(m_file->read(mem,bPerFrame)==bPerFrame)
				{
					if(type == e_SampleInt16)
					{
						tint16 out[6];
						tint16 *pSInt16 = reinterpret_cast<tint16 *>(sampleMem);
						tint16 *pSample = &pSInt16[amount * noOutChannels()];
						for(int i=0;i<m_noChannels;i++)
						{
							out[i] = readSampleInt16(&mem[i * bPerSample],m_sampleSize);
						}
						sortOutputChannels<tint16>(out, pSample);
					}
					else if(type == e_SampleInt24)
					{
						tint32 out[6];
						tint32 *pSInt24 = reinterpret_cast<tint32 *>(sampleMem);
						tint32 *pSample = &pSInt24[amount * noOutChannels()];
						for(int i=0;i<m_noChannels;i++)
						{
							out[i] = readSampleInt24(&mem[i * bPerSample],m_sampleSize);
						}
						sortOutputChannels<tint32>(out, pSample);
					}
					else if(type == e_SampleInt32)
					{
						tint32 out[6];
						tint32 *pSInt32 = reinterpret_cast<tint32 *>(sampleMem);
						tint32 *pSample = &pSInt32[amount * noOutChannels()];
						for(int i=0;i<m_noChannels;i++)
						{
							out[i] = readSampleInt32(&mem[i * bPerSample],m_sampleSize);
						}
						sortOutputChannels<tint32>(out, pSample);
					}
					else
					{
						sample_t out[6];
						sample_t *pSample = &sampleMem[amount * noOutChannels()];
						for(int i=0;i<m_noChannels;i++)
						{
							out[i] = readSample(&mem[i * bPerSample],m_sampleSize);
						}
						sortOutputChannels<sample_t>(out,pSample);
					}
					cIndex += bPerFrame;
					amount++;
				}
				else
				{
					res = false;
				}
			}
		}
	}
	return amount;
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunk::readAsWhole(sample_t *sampleMem, int noSamples, CodecDataType type)
{
	int amount = 0;
	int cIndex, bPerFrame, bPerSample, totalSize;
	
	bPerFrame = bytesPerFrame();
	bPerSample = bytesPerSample();
	
	totalSize = noSamples * bPerFrame;
	if(totalSize > m_readBlockSize || m_readBlockMem == 0)
	{
		if(m_readBlockMem != 0)
		{
			delete [] m_readBlockMem;
		}
		m_readBlockMem = new tbyte [totalSize];
		m_readBlockSize = totalSize;
	}

	cIndex = currentIndexPosition();
	if(m_file->seek(cIndex + 8,common::e_Seek_Current) && cIndex >= 0)
	{
		totalSize = m_file->read(m_readBlockMem, totalSize);
		noSamples = totalSize / bPerFrame;
		
		if(type == e_SampleInt16)
		{
			while(amount<noSamples)
			{
				tint16 out[6];
				tint16 *sampleMemInt16 = reinterpret_cast<tint16 *>(sampleMem);
				tint16 *pSample = &sampleMemInt16[amount * noOutChannels()];
				tbyte *mem = &m_readBlockMem[amount * bPerFrame];
				
				for(int i=0;i<m_noChannels;i++)
				{
					out[i] = readSampleInt16(&mem[i * bPerSample],m_sampleSize);
				}
				sortOutputChannels<tint16>(out, pSample);
				amount++;
			}
		}
		else if(type == e_SampleInt24)
		{
			while(amount<noSamples)
			{
				tint32 out[6];
				tint32 *sampleMemInt24 = reinterpret_cast<tint32 *>(sampleMem);
				tint32 *pSample = &sampleMemInt24[amount * noOutChannels()];
				tbyte *mem = &m_readBlockMem[amount * bPerFrame];
				
				for(int i=0;i<m_noChannels;i++)
				{
					out[i] = readSampleInt24(&mem[i * bPerSample],m_sampleSize);
				}
				sortOutputChannels<tint32>(out, pSample);
				amount++;
			}
		}
		else if(type == e_SampleInt32)
		{
			while(amount<noSamples)
			{
				tint32 out[6];
				tint32 *sampleMemInt32 = reinterpret_cast<tint32 *>(sampleMem);
				tint32 *pSample = &sampleMemInt32[amount * noOutChannels()];
				tbyte *mem = &m_readBlockMem[amount * bPerFrame];
				
				for(int i=0;i<m_noChannels;i++)
				{
					out[i] = readSampleInt32(&mem[i * bPerSample],m_sampleSize);
				}
				sortOutputChannels<tint32>(out, pSample);
				amount++;
			}
		}
		else
		{
			while(amount<noSamples)
			{
				sample_t out[6];
				sample_t *pSample = &sampleMem[amount * noOutChannels()];
				tbyte *mem = &m_readBlockMem[amount * bPerFrame];
				
				for(int i=0;i<m_noChannels;i++)
				{
					out[i] = readSample(&mem[i * bPerSample],m_sampleSize);
				}
				sortOutputChannels<sample_t>(out, pSample);
				amount++;
			}
		}
		m_currentIndexPosition += amount;
	}
	return amount;
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunk::read(sample_t *sampleMem, int noSamples, CodecDataType type)
{
	int amount = 0;
	
    if(m_file!=0 && filePositionToStart() && sampleMem!=0 && noSamples>0)
	{
		if(m_blockSize > 0)
		{
			amount = readAsBlocks(sampleMem, noSamples, type);
		}
		else
		{
			amount = readAsWhole(sampleMem, noSamples, type);
		}
	}
	return amount;
}


//-------------------------------------------------------------------------------------------

bool IFFSoundChunk::seek(int idx)
{
	bool res;
	int nIndex = indexPosition(idx);
    if(nIndex>=m_offset && nIndex<(size() - 8))
	{
		m_currentIndexPosition = idx;
		res = true;
	}
	else
	{
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp IFFSoundChunk::currentTime() const
{
	common::TimeStamp cT(static_cast<tfloat64>(m_currentIndexPosition) / m_sampleRate);
	return cT;
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunk::indexPosition(int idx) const
{
	int pos;
	
	if(m_offset > 0 && m_blockSize > 0)
	{
		int sampleBlockIdx = idx / m_noSampleFrames;
		int indexInBlock = idx % m_noSampleFrames;
		int noBlocks = bytesPerSampleBlock() / m_blockSize;
		if((bytesPerSampleBlock() % m_blockSize) != 0)
		{
			noBlocks++;
		}
		pos = (sampleBlockIdx * m_blockSize * noBlocks) + (indexInBlock * bytesPerFrame()) + m_offset;
	}
	else
	{
		pos = (bytesPerFrame() * idx) + m_offset;
	}
	return pos;
}


//-------------------------------------------------------------------------------------------
// Number of bytes in a continuous sample block that is block-aligned
//-------------------------------------------------------------------------------------------

int IFFSoundChunk::bytesPerSampleBlock() const
{
	return m_noSampleFrames * bytesPerFrame();
}

//-------------------------------------------------------------------------------------------
// Number of bytes in a single sample frame
//-------------------------------------------------------------------------------------------

int IFFSoundChunk::bytesPerFrame() const
{
	return m_noChannels * bytesPerSample();
}

//-------------------------------------------------------------------------------------------
// Size of sample in bytes.
//-------------------------------------------------------------------------------------------

int IFFSoundChunk::bytesPerSample() const
{
	tint size;
	
	if(m_sampleSize < 9)
	{
		size = 1;
	}
	else if(m_sampleSize < 17)
	{
		size = 2;
	}
	else if(m_sampleSize < 25)
	{
		size = 3;
	}
	else if(m_sampleSize == 64)
	{
		size = 8;
	}
	else
	{
		size = 4;
	}
	return size;
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunk::blocksPerSection() const
{
	int sampleBytesPerSection = m_noSampleFrames * bytesPerFrame();
	int blocksPerSection = sampleBytesPerSection / m_blockSize;
	if((sampleBytesPerSection % m_blockSize) != 0)
	{
		blocksPerSection++;
	}
	return blocksPerSection;
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunk::totalSampleBytes() const
{
	return size() - (8 + m_offset);
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunk::numberOfSamples() const
{
	int nSamples;
	
	if(m_offset > 0 && m_blockSize > 0)
	{
		int bytesPerSection = blocksPerSection() * m_blockSize;
		int noSections = totalSampleBytes() / bytesPerSection;
		
		nSamples = m_noSampleFrames * noSections;
		
		int remain = totalSampleBytes() - (noSections * bytesPerSection);
		while(remain >= bytesPerFrame())
		{
			nSamples++;
			remain -= bytesPerFrame();
		}
	}
	else
	{
		nSamples = totalSampleBytes() / bytesPerFrame();
		if(nSamples > m_noSampleFrames)
		{
			nSamples = m_noSampleFrames;
		}
	}
	return nSamples;
}

//-------------------------------------------------------------------------------------------

void IFFSoundChunk::setCurrentIndexPosition(int idx)
{
	m_currentIndexPosition = idx;
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunk::currentIndexPosition()
{
	int idx = indexPosition(m_currentIndexPosition);
	if(!(idx>=0 && idx<(size() - 8)))
	{
		idx = -1;
	}
	return idx;
}

//-------------------------------------------------------------------------------------------

int IFFSoundChunk::nextIndexPosition()
{
	int idx = currentIndexPosition();
	m_currentIndexPosition++;
	return idx;
}

//-------------------------------------------------------------------------------------------

sample_t IFFSoundChunk::readSample(const tbyte *mem,int noBits)
{
	sample_t x;


	if(m_littleEndian)
	{
		switch(m_formatType)
		{
			case IFFCommonChunk::e_PCM_Float32:
			{
				union
				{
					tuint32 sInt;
					tfloat32 sFloat;
				} t;
				t.sInt = to32BitUnsignedFromLittleEndian(mem);
				x = static_cast<sample_t>(t.sFloat);
			}
			break;

			case IFFCommonChunk::e_PCM_Float64:
			{
				union
				{
					tuint64 sInt;
					tfloat64 sFloat;
				} t;
				t.sInt = to64BitUnsignedFromLittleEndian(mem);
				x = static_cast<sample_t>(t.sFloat);
			}
			break;

			case IFFCommonChunk::e_PCM_Integer:
			default:
#if defined(SINGLE_FLOAT_SAMPLE)
				x = readSampleLittleEndian(reinterpret_cast<const tubyte*>(mem), noBits);
#else
				x = readSample64LittleEndian(reinterpret_cast<const tubyte*>(mem), noBits);
#endif
				break;
		}
	}
	else
	{
		switch(m_formatType)
		{
		case IFFCommonChunk::e_PCM_Float32:
		{
			union
			{
				tuint32 sInt;
				tfloat32 sFloat;
			} t;
			t.sInt = to32BitUnsignedFromBigEndian(mem);
			x = static_cast<sample_t>(t.sFloat);
		}
		break;

		case IFFCommonChunk::e_PCM_Float64:
		{
			union
			{
				tuint64 sInt;
				tfloat64 sFloat;
			} t;
			t.sInt = to64BitUnsignedFromBigEndian(mem);
			x = static_cast<sample_t>(t.sFloat);
		}
		break;

		case IFFCommonChunk::e_PCM_Integer:
		default:
#if defined(SINGLE_FLOAT_SAMPLE)
			x = readSampleBigEndian(reinterpret_cast<const tubyte*>(mem), noBits);
#else
			x = readSample64BigEndian(reinterpret_cast<const tubyte*>(mem), noBits);
#endif
			break;
		}

	}
	return x;
}

//-------------------------------------------------------------------------------------------

tint16 IFFSoundChunk::readSampleInt16(const tbyte *mem, int noBits)
{
	tint16 x;
	
	if(m_littleEndian)
	{
		x = readInt16SampleLittleEndian(mem, noBits);
	}
	else
	{
		x = readInt16SampleBigEndian(mem, noBits);
	}
	return x;
}

//-------------------------------------------------------------------------------------------

tint32 IFFSoundChunk::readSampleInt24(const tbyte *mem, int noBits)
{
	tint32 x;
	
	if(m_littleEndian)
	{
		x = readInt24SampleLittleEndian(mem, noBits);
	}
	else
	{
		x = readInt24SampleBigEndian(mem, noBits);
	}
	return x;
}

//-------------------------------------------------------------------------------------------

tint32 IFFSoundChunk::readSampleInt32(const tbyte *mem, int noBits)
{
	tint32 x;
	
	if(m_littleEndian)
	{
		x = readInt32SampleLittleEndian(mem, noBits);
	}
	else
	{
		x = readInt32SampleBigEndian(mem, noBits);
	}
	return x;
}

//-------------------------------------------------------------------------------------------

tint IFFSoundChunk::bitrate() const
{
	return static_cast<tint>(static_cast<tfloat64>(bytesPerSample() * noOutChannels()) * m_sampleRate) << 3;
}

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
