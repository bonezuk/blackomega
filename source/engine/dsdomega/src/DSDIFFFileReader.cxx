#include "engine/dsdomega/inc/DSDIFFFileReader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace dsd
{
//-------------------------------------------------------------------------------------------

DSDIFFFileReader::DSDIFFFileReader(common::BIOStream *ioFile) : DSDFileReader(ioFile),
	m_frequency(0),
	m_noChannels(0),
	m_dataStartOffset(0),
	m_dataLength(0),
	m_inputData()
{}

//-------------------------------------------------------------------------------------------

DSDIFFFileReader::~DSDIFFFileReader()
{}

//-------------------------------------------------------------------------------------------

void DSDIFFFileReader::printError(const tchar *strR, const tchar *strE) const
{
	common::Log::g_Log << "DSDIFFFileReader::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

tuint32 DSDIFFFileReader::readHeader(tint64& len)
{
	tuint32 id = 0;
	tubyte mem[12];
	
	if(m_ioFile->read(mem, 12) == 12)
	{
		id = engine::to32BitUnsignedFromBigEndian(mem);
		len = engine::to64BitSignedFromBigEndian(&mem[4]);
	}
	return id;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::parse()
{
	tuint32 formID;
	tint64 len;
	bool res = false;
	
	if(m_ioFile == NULL)
	{
		return false;
	}
	reset();
	if(m_ioFile->seek64(0, common::e_Seek_Start))
	{
		formID = readHeader(len);
		if(formID == FORMDSD_CHUNK_ID)
		{
			tubyte mem[4];
			
			if(m_ioFile->read(mem, 4) == 4)
			{
				tuint32 dsdID = engine::to32BitUnsignedFromBigEndian(mem);
				if(dsdID == DSDDATA_CHUNK_ID)
				{
					res = parseFormDSD(len);
				}
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void DSDIFFFileReader::reset()
{
	m_frequency = 0;
	m_noChannels = 0;
	m_dataStartOffset = 0;
	m_dataLength = 0;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::parseFormDSD(tint64 formLen)
{
	tuint32 chunkID;
	tint64 chunkLen = 0, nextPosition, formPos;
	bool loop = true, res = false;
	bool isVersion = false, isProp = false;
	
	for(formPos = 4; formPos < formLen && loop; formPos += 12 + chunkLen)
	{
		chunkID = readHeader(chunkLen);
		if(chunkID != 0)
		{
			nextPosition = m_ioFile->offset64() + chunkLen;
			
			if(chunkID == FVER_CHUNK_ID)
			{
				isVersion = isVersionValid(chunkLen);
			}
			else if(chunkID == PROP_CHUNK_ID)
			{
				isProp = parseProperty(chunkLen);
			}
			else if(chunkID == DSDDATA_CHUNK_ID)
			{
				parseDSDData(chunkLen);
			}
			
			if(!m_ioFile->seek64(nextPosition, common::e_Seek_Start))
			{
				loop = false;
			}
		}
		else
		{
			loop = false;
		}
	}
	
	if(isVersion && isProp && m_dataStartOffset > 0 && m_dataLength > 0)
	{
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::isVersionValid(tint64 formLen)
{
	tuint32 version;
	tubyte mem[4];
	bool res = false;
	
	if(formLen >= 4 && m_ioFile->read(mem, 4) == 4)
	{
		version = engine::to32BitUnsignedFromBigEndian(mem);
		res = (version >= 0x01000000) ? true : false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::parseProperty(tint64 formLen)
{
	tint64 chunkLen = 0, nextPosition;
	tuint32 chunkID;
	tubyte mem[4];
	bool res = false, loop = true;
	
	if(formLen >= 4 && m_ioFile->read(mem, 4) == 4)
	{
		chunkID = engine::to32BitUnsignedFromBigEndian(mem);
		if(chunkID == SND_CHUNK_ID)
		{
			for(tint64 formPos = 4; formPos < formLen && loop; formPos += 12 + chunkLen)
			{
				chunkID = readHeader(chunkLen);
				if(chunkID != 0)
				{
					nextPosition = m_ioFile->offset64() + chunkLen;
					
					if(chunkID == FRAMESAMPLE_CHUNK_ID)
					{
						if(!readSampleRate(chunkLen))
						{
							return false;
						}
					}
					else if(chunkID == CHANNELS_CHUNK_ID)
					{
						if(!readChannelMap(chunkLen))
						{
							return false;
						}
					}
					else if(chunkID == COMPRESS_CHUNK_ID)
					{
						if(!isCompressionValid(chunkLen))
						{
							return false;
						}
					}
					
					if(!m_ioFile->seek64(nextPosition, common::e_Seek_Start))
					{
						loop = false;
					}
				}
				else
				{
					loop = false;
				}
			}
		}
	}
	if(m_frequency > 0 && m_noChannels > 0)
	{
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::readSampleRate(tint64 formLen)
{
	tubyte mem[4];
	bool res = false;
	
	if(formLen >= 4 && m_ioFile->read(mem, 4) == 4)
	{
		m_frequency = engine::to32BitSignedFromBigEndian(mem);
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------
// Type A = 0.Center
// Type B = 0.F-Left, 1.F-Right
// Type C = 0.F-Left, 1.F-Right, 2.Center
// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
/* Notes working out channel mapping and write out to respective blocks using a 3 channel example
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14
C0 R0 L0 C1 R1 L1 C2 R2 L2 C3 R3 L3 C4 R4 L4
Bl=0 F-Left
L0 L1 L2 L3 L4
Bl=1 F-Right
R0 R1 R2 R3 R4
Bl=2 Center
C0 C1 C2 C3 C4

int m_channelMap[3] = {2, 1, 0};
tubyte *out[3]
out[0] = array;
out[1] = &array[4096];
out[2] = &array[4096 * 2];
for(tint idx = 0; idx < len; idx++)
{
	tubyte *o = out[m_channelMap[idx % 3]];
	o[idx / 3] = in[idx];
}
*/
//-------------------------------------------------------------------------------------------

void DSDIFFFileReader::defaultChannelMap()
{
	for(int idx = 0; idx < 8; idx++)
	{
		m_channelMap[idx] = idx;
	}
}

//-------------------------------------------------------------------------------------------

int DSDIFFFileReader::channelIndexFromID(tuint32 chID)
{
	int idx;
	
	if(chID == CHMAP_SLEFT_ID || chID == CHMAP_MLEFT_ID)
	{
		idx = 0;
	}
	else if(chID == CHMAP_SRIGHT_ID || chID == CHMAP_MRIGHT_ID)
	{
		idx = 1;
	}
	else if(chID == CHMAP_CENTER_ID)
	{
		idx = (m_noChannels > 1) ? 2 : 0;
	}
	else if(chID == CHMAP_LFE_ID)
	{
		idx = 3;
	}
	else if(chID == CHMAP_RLEFT_ID)
	{
		idx = (m_noChannels > 5) ? 4 : 3;
	}
	else if(chID == CHMAP_RRIGHT_ID)
	{
		idx = (m_noChannels > 5) ? 5 : 4;
	}
	return idx;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::readChannelMap(tint64 formLen)
{
	tint64 formPos;
	tbyte mem[4];
	bool res = false;
	
	if(formLen >= 2 && m_ioFile->read(mem, 2) == 2)
	{
		m_noChannels = static_cast<tint>(engine::to16BitUnsignedFromBigEndian(mem));
		defaultChannelMap();
		
		formPos = 2;
		for(int idx = 0; idx < m_noChannels && formPos < formLen; idx++, formPos += 4)
		{
			if(m_ioFile->read(mem, 4) == 4)
			{
				tuint32 chID = engine::to32BitSignedFromBigEndian(mem);
				m_channelMap[idx] = channelIndexFromID(chID);
			}
			else
			{
				return false;
			}
		}
	}
	if(m_noChannels > 0 && m_noChannels <= 8)
	{
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::isCompressionValid(tint64 formLen)
{
	tubyte mem[4];
	bool res = false;
	
	if(formLen >= 4 && m_ioFile->read(mem, 4) == 4)
	{
		tuint32 cmpID = engine::to32BitSignedFromBigEndian(mem);
		if(cmpID == DSDDATA_CHUNK_ID)
		{
			res = true;
		}
		else
		{
			printError("isCompressionValid", "Unsupported DSD compression");
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void DSDIFFFileReader::parseDSDData(tint64 formLen)
{
	m_dataStartOffset = m_ioFile->offset64();
	m_dataLength = formLen;
}

//-------------------------------------------------------------------------------------------

tint DSDIFFFileReader::numberOfChannels() const
{
	return m_noChannels;
}

//-------------------------------------------------------------------------------------------

tint DSDIFFFileReader::frequency() const
{
	return m_frequency;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::isLSB() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::isMSB() const
{
	return true;
}

//-------------------------------------------------------------------------------------------

tint DSDIFFFileReader::bitsPerSample() const
{
	return 1;
}

//-------------------------------------------------------------------------------------------

tint DSDIFFFileReader::channelBlockSize() const
{
	return c_channelBlockSize;
}

//-------------------------------------------------------------------------------------------

tint64 DSDIFFFileReader::totalSamples() const
{
	tint64 totalSamp = (m_dataLength / m_noChannels) * 8;
	return totalSamp;
}

//-------------------------------------------------------------------------------------------

tint64 DSDIFFFileReader::numberOfBlocks() const
{
	tint64 dataLen = m_dataLength / m_noChannels;
	tint64 noblks = dataLen / c_channelBlockSize;
	if(dataLen % c_channelBlockSize)
	{
		noblks++;
	}
	return noblks;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::data(int blockIdx, QByteArray& arr, bool isBlockSize)
{
	tint64 posOffset = blockIdx * c_channelBlockSize * m_noChannels;
	bool res = false;
	
	if(posOffset < m_dataLength)
	{
		if(m_ioFile->seek64(posOffset + m_dataStartOffset, common::e_Seek_Start))
		{
			int amount = c_channelBlockSize * m_noChannels;
			int total;
			
			if(posOffset + amount > m_dataLength)
			{
				amount = static_cast<int>(m_dataLength - posOffset);
			}
			if(amount > m_inputData.size())
			{
				m_inputData.resize(amount);
			}
			total = (isBlockSize) ? c_channelBlockSize : amount / m_noChannels;
			arr.resize(total * m_noChannels);
			
			tubyte *in = reinterpret_cast<tubyte *>(m_inputData.data());
			if(m_ioFile->read(in, amount) == amount)
			{
				int idx, chIdx, oIdx;
				
				tubyte *out[8];
				out[0] = reinterpret_cast<tubyte *>(arr.data());
				for(chIdx = 1; chIdx < m_noChannels; chIdx++)
				{
					tubyte *bOut = out[chIdx - 1];
					out[chIdx] = &bOut[total];
				}
				while(chIdx < 8)
				{
					out[chIdx++] = 0;
				}
				
				for(idx = 0, oIdx = 0; idx < amount; idx += m_noChannels, oIdx++)
				{
					for(chIdx = 0; chIdx < m_noChannels; chIdx++)
					{
						tubyte *o = out[m_channelMap[chIdx]];
						if(o != 0)
						{
							o[oIdx] = in[idx + chIdx];
						}
					}
				}
				
				while(oIdx < total)
				{
					for(chIdx = 0; chIdx < m_noChannels; chIdx++)
					{
						tubyte *o = out[m_channelMap[chIdx]];
						if(o != 0)
						{
							o[oIdx] = c_dsdIdleSample;
						}
					}
					oIdx++;				
				}
				
				res = true;
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDIFFFileReader::data(int blockIdx, int channelIdx, QByteArray& arr, bool isBlockSize)
{
	tint64 posOffset = blockIdx * c_channelBlockSize * m_noChannels;
	bool res = false;
	
	if(posOffset < m_dataLength && channelIdx < m_noChannels)
	{
		int chIdx;
		
		for(chIdx = 0; chIdx < m_noChannels; chIdx++)
		{
			if(m_channelMap[chIdx] == channelIdx)
				break;
		}
		if(chIdx >= m_noChannels)
		{
			return false;
		}
		
		if(m_ioFile->seek64(posOffset + m_dataStartOffset, common::e_Seek_Start))
		{
			int amount = c_channelBlockSize * m_noChannels;
			int total;
			
			if(posOffset + amount > m_dataLength)
			{
				amount = static_cast<int>(m_dataLength - posOffset);
			}
			if(amount > m_inputData.size())
			{
				m_inputData.resize(amount);
			}
			total = (isBlockSize) ? c_channelBlockSize : amount / m_noChannels;
			arr.resize(total);
			
			tubyte *in = reinterpret_cast<tubyte *>(m_inputData.data());
			if(m_ioFile->read(in, amount) == amount)
			{
				int idx, oIdx;
				
				tubyte *out = reinterpret_cast<tubyte *>(arr.data());
				
				for(idx = 0, oIdx = 0; idx < amount; idx += m_noChannels, oIdx++)
				{
					out[oIdx] = in[idx + chIdx];
				}
				
				while(oIdx < total)
				{
					out[oIdx++] = c_dsdIdleSample;
				}
				
				res = true;
			}
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
