#include "engine/dsdomega/inc/DSFFileReader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace dsd
{
//-------------------------------------------------------------------------------------------

DSFFileReader::DSFFileReader(common::BIOStream *ioFile) : m_ioFile(ioFile),
	m_numberOfChannels(0),
	m_frequency(0),
	m_bitsPerSample(0),
	m_channelBlockSize(0),
	m_totalSamples(0),
	m_dataStartOffset(0),
	m_dataChunkSize(0),
	m_metaDataOffset(0)
{}

//-------------------------------------------------------------------------------------------

DSFFileReader::~DSFFileReader()
{}

//-------------------------------------------------------------------------------------------

bool DSFFileReader::parse()
{
	bool res = false;

	if(m_ioFile == NULL)
	{
		return false;
	}
	if(parseDSDChunk() && parseFMTChunk() && parseDataChunk())
	{
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSFFileReader::parseDSDChunk()
{
	tubyte mem[28];
	bool res = false;
	
	if(m_ioFile->seek64(0, common::e_Seek_Start) && m_ioFile->read(mem, 28) == 28)
	{
		tuint32 id = to32BitUnsignedFromLittleEndian(mem);
		tint64 chunkSize = to64BitSignedFromLittleEndian(&mem[4]);
		
		if(id == DSD_CHUNK_ID && chunkSize >= 28)
		{
			m_metaDataOffset = to64BitSignedFromLittleEndian(&mem[20]);
			res = true;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------
// fmt header : 0 (4)
// chunk size : 4 (8)
// version : 12 (4)
// formatID : 16 (4)
// channelType : 20 (4)
// channelNum : 24 (4)
// frequency : 28 (4)
// bitsPerSample : 32 (4)
// sampleCount : 36 (8)
// blockSize : 44 (4)
// reserved : 48 (4) = 52
//-------------------------------------------------------------------------------------------

bool DSFFileReader::parseFMTChunk()
{
	tint64 offset;
	tubyte mem[52];
	bool res = false;
	
	if(seekFileToChunkID(FMT_CHUNK_ID, offset) && m_ioFile->read(mem, 52) == 52)
	{
		tint64 chunkSize = to64BitSignedFromLittleEndian(&mem[4]);
		if(chunkSize >= 52)
		{
			tuint32 version = to32BitUnsignedFromLittleEndian(&mem[12]);
			tuint32 formatID = to32BitUnsignedFromLittleEndian(&mem[16]);
			if(version == 1 && formatID == 0)
			{
				m_channelInterleaveType = to32BitSignedFromLittleEndian(&mem[20]);
				m_numberOfChannels = to32BitSignedFromLittleEndian(&mem[24]);
				m_frequency = to32BitSignedFromLittleEndian(&mem[28]);
				m_bitsPerSample = to32BitSignedFromLittleEndian(&mem[32]);
				m_totalSamples = to64BitSignedFromLittleEndian(&mem[36]);
				m_channelBlockSize = to32BitSignedFromLittleEndian(&mem[44]);
				
				res = true;
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

tint DSFFileReader::channelInterleaveType() const
{
	return m_channelInterleaveType;
}

//-------------------------------------------------------------------------------------------

bool DSFFileReader::parseDataChunk()
{
	tubyte mem[12];
	bool res = false;
	
	if(seekFileToChunkID(DATA_CHUNK_ID, m_dataStartOffset) && m_ioFile->read(mem, 12) == 12)
	{
		m_dataStartOffset += 12;
		m_dataChunkSize = to64BitSignedFromLittleEndian(&mem[4]) - 12;
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSFFileReader::seekFileToChunkID(tuint32 chunkID, tint64& offset)
{
	tubyte mem[12];
	bool res = false;
	
	offset = 0;
	do
	{
		if(m_ioFile->seek64(offset, common::e_Seek_Start) && m_ioFile->read(mem, 12) == 12)
		{
			tuint32 id = to32BitUnsignedFromLittleEndian(mem);
			tint64 chunkSize = to64BitSignedFromLittleEndian(&mem[4]);
			if(id == chunkID)
			{
				res = m_ioFile->seek64(offset, common::e_Seek_Start);
			}
			else
			{
				offset += chunkSize;
			}
		}
		else
		{
			break;
		}
	} while(!res && offset < m_ioFile->size64());
	
	return res;
}

//-------------------------------------------------------------------------------------------

tint DSFFileReader::numberOfChannels() const
{
	return m_numberOfChannels;
}

//-------------------------------------------------------------------------------------------

tint DSFFileReader::frequency() const
{
	return m_frequency;
}

//-------------------------------------------------------------------------------------------

bool DSFFileReader::isLSB() const
{
	return (m_bitsPerSample == 1);
}

//-------------------------------------------------------------------------------------------

bool DSFFileReader::isMSB() const
{
	return (m_bitsPerSample != 1);
}

//-------------------------------------------------------------------------------------------

tint DSFFileReader::bitsPerSample() const
{
	return m_bitsPerSample;
}

//-------------------------------------------------------------------------------------------

tint DSFFileReader::channelBlockSize() const
{
	return m_channelBlockSize;
}

//-------------------------------------------------------------------------------------------

tint64 DSFFileReader::totalSamples() const
{
	return m_totalSamples;
}

//-------------------------------------------------------------------------------------------

bool DSFFileReader::data(int blockIdx, QByteArray& arr, bool isBlockSize)
{
	bool res = false;
	
	if(m_ioFile != NULL)
	{
		tint64 blockOffset = blockIdx * m_numberOfChannels * m_channelBlockSize;
		tint64 offset = m_dataStartOffset + blockOffset;
		if(blockOffset < m_dataChunkSize && m_ioFile->seek64(offset, common::e_Seek_Start))
		{
			int amount;
			tint64 bitFinal;
			tint64 bitOffset = (static_cast<tint64>(blockIdx) * m_channelBlockSize) * 8;
			QByteArray array;
			
			bitFinal = bitOffset + (m_channelBlockSize << 3);
			if(bitFinal > m_totalSamples)
			{
				amount = static_cast<int>((m_totalSamples - bitFinal) >> 3);
			}
			else
			{
				amount = m_channelBlockSize;
			}

			if(isBlockSize)
			{
				array.resize(m_channelBlockSize * m_numberOfChannels);
			}
			else
			{
				array.resize(amount * m_numberOfChannels);
			}
			
			res = true;
			for(int chIdx = 0; chIdx < m_numberOfChannels && res; chIdx++, offset += m_channelBlockSize)
			{
				if(m_ioFile->seek64(offset, common::e_Seek_Start))
				{
					tubyte *d = reinterpret_cast<tubyte *>(array.data());
					d = &d[chIdx * m_channelBlockSize];
					if(m_ioFile->read(d, amount) == amount)
					{
						if(amount < m_channelBlockSize)
						{
							memset(&d[amount], c_dsdIdleSample, m_channelBlockSize - amount);
						}
					}
					else
					{
						res = false;
					}
					
				}
				else
				{
					res = false;
				}
			}

			if(res)
			{
				arr = array;
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSFFileReader::data(int blockIdx, int channelIdx, QByteArray& arr, bool isBlockSize)
{
	bool res = false;
	
	if(m_ioFile != NULL)
	{
		tint64 blockOffset = ((blockIdx * m_numberOfChannels) + channelIdx) * m_channelBlockSize;
		tint64 offset = m_dataStartOffset + blockOffset;
		if(blockOffset < m_dataChunkSize && m_ioFile->seek64(offset, common::e_Seek_Start))
		{
			int amount;
			tint64 bitFinal;
			tint64 bitOffset = (static_cast<tint64>(blockIdx) * m_channelBlockSize) * 8;
			QByteArray array;
			
			bitFinal = bitOffset + (m_channelBlockSize << 3);
			if(bitFinal > m_totalSamples)
			{
				amount = static_cast<int>((m_totalSamples - bitFinal) >> 3);
			}
			else
			{
				amount = m_channelBlockSize;
			}
			
			if(isBlockSize)
			{
				array.resize(m_channelBlockSize);
			}
			else
			{
				array.resize(amount);
			}
			if(m_ioFile->read(array.data(), amount) == amount)
			{
				if(amount < array.size())
				{
					tubyte *d = reinterpret_cast<tubyte *>(array.data());
					memset(&d[amount], c_dsdIdleSample, array.size() - amount);
				}
				arr = array;
				res = true;
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

tint64 DSFFileReader::numberOfBlocks() const
{
	return m_dataChunkSize / (m_channelBlockSize * m_numberOfChannels);
}

//-------------------------------------------------------------------------------------------

bool DSFFileReader::isMetadata() const
{
	return (m_metaDataOffset != 0);
}

//-------------------------------------------------------------------------------------------

tint64 DSFFileReader::metaOffset() const
{
	return m_metaDataOffset;
}

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
