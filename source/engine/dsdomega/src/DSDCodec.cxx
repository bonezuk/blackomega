#include "engine/dsdomega/inc/DSDCodec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace dsd
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CodecFactory, Codec, \
                            DSDCodecFactory, DSDCodec, \
                            "dsd", false)

//-------------------------------------------------------------------------------------------

DSDCodec::DSDCodec(QObject *parent) : engine::Codec(e_codecDSD, parent),
	m_file(0),
	m_dsdFileHandler(0),
	m_inBufferList(),
	m_inSampleOffset(0),
	m_inBlockNumber(0),
	m_pcmFrequency(0),
	m_pcmConverters(),
	m_pcmBufferList(),
	m_pcmSampleOffset(0),
	m_readComplete(false),
	m_isDSDOverPCM(0),
	m_markerIncr(0),
	m_noBlocksLastReadIn(0)
{}

//-------------------------------------------------------------------------------------------

DSDCodec::~DSDCodec()
{
	try
	{
		DSDCodec::close();
	}
	catch(...) {}
}

//-------------------------------------------------------------------------------------------

void DSDCodec::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "DSDCodec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::open(const QString& name)
{
	bool res = false;
	QString str;

	close();
	m_file = new common::BIOBufferedStream(common::e_BIOStream_FileRead, 65536);
	if(m_file->open(name))
	{
		QString ext = getFileExtension(name);
		if(ext == "dsf")
		{
			m_dsdFileHandler = new DSFFileReader(m_file);
		}
		else if(ext == "dff")
		{
			m_dsdFileHandler = new DSDIFFFileReader(m_file);
		}
		
		if(m_dsdFileHandler != 0)
		{
			if(m_dsdFileHandler->parse())
			{
				setupDSFBuffers();
				m_inBlockNumber = 0;
				m_inSampleOffset = 0;
				res = true;
			}
			else
			{
				str = "Failed to parse DSF file '" + name + "'";
				printError("open",str.toUtf8().constData());
			}
		}
		else
		{
			str = "Unknown DSD file format in '" + name + "'";
			printError("open",str.toUtf8().constData());
		}
	}
	else
	{
		str = "Failed to open '" + name + "' to read";
		printError("open",str.toUtf8().constData());
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void DSDCodec::close()
{
	m_inBufferList.clear();
	if(m_dsdFileHandler != NULL)
	{
		delete m_dsdFileHandler;
		m_dsdFileHandler = NULL;
	}
	if(m_file != NULL)
	{
		m_file->close();
		delete m_file;
		m_file = NULL;
	}
	m_pcmConverters.clear();
	m_pcmBufferList.clear();
	m_pcmFrequency = 0;
	m_isDSDOverPCM = 0;
}

//-------------------------------------------------------------------------------------------

void DSDCodec::setupDSFBuffers()
{
	freeDSFBuffers();
	for(tint i = 0; i < m_dsdFileHandler->numberOfChannels(); i++)
	{
		QByteArray arr;
		m_inBufferList.append(arr);
	}
}

//-------------------------------------------------------------------------------------------

void DSDCodec::freeDSFBuffers()
{
	m_inBufferList.clear();
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::init()
{
	return true;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::readInNextDSFBlock()
{
	tint channelIndex;
	
	channelIndex = 0;
	for(QVector<QByteArray>::iterator ppI = m_inBufferList.begin(); ppI != m_inBufferList.end(); ppI++, channelIndex++)
	{
		QByteArray& arr = *ppI;
		if(!m_dsdFileHandler->data(m_inBlockNumber, channelIndex, arr))
		{
			return false;
		}
	}
	m_inSampleOffset = 0;
	return true;
}

//-------------------------------------------------------------------------------------------

tint64 DSDCodec::bitAtInDSF(tint blockIdx, tint offset) const
{
	tint64 noBits = ((static_cast<tint64>(blockIdx) * m_dsdFileHandler->channelBlockSize()) + offset) << 3;
	return noBits;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp DSDCodec::timeAtInDSF(tint blockIdx, tint offset)
{
	tfloat64 tS = static_cast<tfloat64>(bitAtInDSF(blockIdx, offset)) / static_cast<tfloat64>(m_dsdFileHandler->frequency());
	return common::TimeStamp(tS);
}

//-------------------------------------------------------------------------------------------

tint DSDCodec::currentBlockLength()
{
	tint len;
	
	if(m_inBufferList.size() > 0)
	{
		len = m_inBufferList.at(0).size();
	}
	else
	{
		len = 0;
	}
	return len;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::nextPCMOutput(RData& rData)
{
	const int c_numberofBlocksAtATime = 10;
	int pos, len;
	bool res = true;
	common::TimeStamp startTs, endTs;
	RData::Part& part = rData.nextPart();
	
	if(m_pcmBufferList.size() < 1)
	{
		printError("nextPCMOutput", "PCM output buffers are not initialised in DSD codec");
		return false;
	}
	if(m_readComplete && m_pcmSampleOffset >= (m_pcmBufferList.at(0).size() / sizeof(sample_t)))
	{
		// All DSF data has been read and the PCM buffer is empty.
		part.length() = 0;
		part.start() = part.end() = length();
		if(rData.noParts() == 1)
		{
			rData.start() = part.start();
		}
		rData.end() = part.end();
		return false;
	}

	int pcmLen = static_cast<int>(m_pcmBufferList.at(0).size()) / sizeof(sample_t);
	sample_t* out = reinterpret_cast<sample_t*>(rData.partData(rData.noParts() - 1));
	
	startTs = timeAtInDSF((m_pcmSampleOffset < pcmLen) ? m_inBlockNumber - m_noBlocksLastReadIn : m_inBlockNumber, 0);
	startTs += static_cast<tfloat64>(m_pcmSampleOffset) / static_cast<tfloat64>(m_pcmFrequency);
	part.start() = startTs;
	if(rData.noParts() == 1)
	{
		rData.start() = startTs;
	}
	
	pos = 0;
	len = rData.rLength();
	while(pos < len)
	{
		int noChs = m_pcmBufferList.size();
		pcmLen = static_cast<int>(m_pcmBufferList.at(0).size()) / sizeof(sample_t);
	
		if(m_pcmSampleOffset < pcmLen)
		{
			while(pos < len && m_pcmSampleOffset < pcmLen)
			{
				for(tint ch = 0; ch < noChs; ch++)
				{
					const sample_t *pcm = reinterpret_cast<const sample_t *>(m_pcmBufferList.at(ch).constData());
					out[(pos * noChs) + ch] = pcm[m_pcmSampleOffset];
				}
				m_pcmSampleOffset++;
				pos++;
			}
		}
		else if(!m_readComplete)
		{
			m_noBlocksLastReadIn = 0;
			for(tint blks = 0; blks < c_numberofBlocksAtATime; blks++)
			{
				m_readComplete = (readInNextDSFBlock()) ? false : true;
				m_inBlockNumber++;
				m_noBlocksLastReadIn++;
				if(!m_readComplete)
				{
					for(tint ch = 0; ch < noChs; ch++)
					{
						m_pcmConverters[ch]->push(m_inBufferList[ch]);
					}
				}
				else
				{
					break;
				}
			}
			for(tint ch = 0; ch < noChs; ch++)
			{
				m_pcmConverters[ch]->pull(m_pcmBufferList[ch], m_readComplete);
			}
			m_pcmSampleOffset = 0;
		}
		else
		{
			break;
		}
	}
	
	endTs = startTs + (static_cast<tfloat64>(pos) / static_cast<tfloat64>(m_pcmFrequency));
	part.length() = pos;
	part.end() = endTs;
	part.done() = true;
	part.setDataType(e_SampleFloat);
	rData.end() = endTs;
	
	return true;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::writeDSDOutputNative(RData& rData, tint& pos)
{
	bool res = true;
	tint len = rData.rLength() * sizeof(sample_t);
	tubyte *out = reinterpret_cast<tubyte *>(rData.partData(rData.noParts() - 1));

	while(pos < len && res)
	{
		while(pos < len && m_inSampleOffset < currentBlockLength())
		{
			for(tint chIdx = 0; chIdx < m_dsdFileHandler->numberOfChannels(); chIdx++)
			{
				const tubyte *in = reinterpret_cast<const tubyte *>(m_inBufferList.at(chIdx).constData());
				for(tint i = 0; i < sizeof(sample_t); i++)
				{
					*out++ = in[m_inSampleOffset + i];
				}
			}
			pos += sizeof(sample_t);
			m_inSampleOffset += sizeof(sample_t);
		}
		if(pos < len && m_inSampleOffset >= currentBlockLength())
		{
			res = readInNextDSFBlock();
			m_inBlockNumber++;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::writeDSDOutputOverPCM(RData& rData, tint& pos)
{
	bool res = true;
	tint len = rData.rLength();
	tuint32 *out = reinterpret_cast<tuint32 *>(rData.partData(rData.noParts() - 1));
	bool is32Bit = (dataTypesSupported() & engine::e_SampleInt32) ? true : false;

	while(pos < len && res)
	{
		while(pos < len && m_inSampleOffset < currentBlockLength())
		{
			for(tint chIdx = 0; chIdx < m_dsdFileHandler->numberOfChannels(); chIdx++)
			{
				const tubyte *in = reinterpret_cast<const tubyte *>(m_inBufferList.at(chIdx).constData());
				tuint32 s = (m_markerIncr & 0x01) ? 0xfffa0000 : 0x00050000;
				tubyte a0 = in[m_inSampleOffset + 0];
				tubyte a1 = in[m_inSampleOffset + 1];
				
				if(m_dsdFileHandler->isLSB())
				{
					a0 = lsb2msb(a0);
					a1 = lsb2msb(a1);
				}
				s |= ((static_cast<tuint32>(a0) << 8) & 0x0000ff00) | (static_cast<tuint32>(a1) & 0x000000ff);
				if(is32Bit)
				{
					s <<= 8;
				}
				*out++ = s;
			}
			m_markerIncr++;
			m_inSampleOffset += 2;
			pos++;
		}	
		if(pos < len && m_inSampleOffset >= currentBlockLength())
		{
			res = readInNextDSFBlock();
			m_inBlockNumber++;
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::nextDSDOutput(RData& rData)
{
	bool res = true;
	tint64 bitPosition = bitAtInDSF((m_inSampleOffset < currentBlockLength()) ? m_inBlockNumber - 1 : m_inBlockNumber, m_inSampleOffset);
	RData::Part& part = rData.nextPart();
	
	if(bitPosition < m_dsdFileHandler->totalSamples())
	{
		tint pos = 0;
		common::TimeStamp startTs, endTs;
		
		startTs = timeAtInDSF((m_inSampleOffset < currentBlockLength()) ? m_inBlockNumber - 1 : m_inBlockNumber, m_inSampleOffset);

		part.start() = startTs;
		if(rData.noParts() == 1)
		{
			rData.start() = startTs;
		}
		
		if(!m_isDSDOverPCM)
		{
			res = writeDSDOutputNative(rData, pos);
			part.setDataType(m_dsdFileHandler->isLSB() ? e_SampleDSD8LSB : e_SampleDSD8MSB);
			endTs = startTs + static_cast<tfloat64>(pos * 8) / static_cast<tfloat64>(m_dsdFileHandler->frequency());
			part.length() = pos / sizeof(sample_t);
		}
		else
		{
			res = writeDSDOutputOverPCM(rData, pos);
			part.setDataType((m_isDSDOverPCM == 1) ? e_SampleInt24 : e_SampleInt32);
			endTs = startTs + (static_cast<tfloat64>(pos * 16) / static_cast<tfloat64>(m_dsdFileHandler->frequency()));
			part.length() = pos;
		}
		part.end() = endTs;
	}
	else
	{
		part.start() = part.end() = length();
		if(rData.noParts() == 1)
		{
			rData.start() = part.start();
		}
		part.length() = 0;
		res = false;
	}
	part.done() = true;
	rData.end() = part.end();
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::next(AData& data)
{
	RData& rData = dynamic_cast<RData&>(data);
	return (m_pcmFrequency > 0) ? nextPCMOutput(rData) : nextDSDOutput(rData);
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::isSeek() const
{
	return true;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::seek(const common::TimeStamp& v)
{
	bool res = false;
	
	if(m_dsdFileHandler != NULL && v < length())
	{
		tfloat64 pos = static_cast<tfloat64>(v) * static_cast<tfloat64>(m_dsdFileHandler->frequency());
		tint64 bytePosition = static_cast<tint64>(pos) / 8;
		m_inBlockNumber = static_cast<tint>(bytePosition / m_dsdFileHandler->channelBlockSize());
		m_inSampleOffset = static_cast<tint>(bytePosition % m_dsdFileHandler->channelBlockSize());
		res = readInNextDSFBlock();
		if(res && m_pcmSampleOffset > 0)
		{
			m_pcmBufferList.clear();
			m_pcmSampleOffset = 0;
			m_noBlocksLastReadIn = 0;
			for(int idx = 0; idx < noChannels() && res; idx++)
			{
				m_pcmBufferList.append(QByteArray());
				m_pcmConverters[idx]->reset();
			}
			m_readComplete = false;
		}
		m_markerIncr = 0;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::isComplete() const
{
	bool res = false;
	
	if(m_pcmFrequency > 0)
	{
		res = (m_readComplete && m_pcmSampleOffset >= (m_pcmBufferList.at(0).size() / sizeof(sample_t)));
	}
	else
	{
		tint64 bitPos = bitAtInDSF(m_inBlockNumber, m_inSampleOffset);
		if(m_dsdFileHandler != NULL)
		{
			res = (bitPos >= m_dsdFileHandler->totalSamples());
		}	
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::isRemote() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::isBuffered(tfloat32& percent)
{
	return false;
}

//-------------------------------------------------------------------------------------------

tint DSDCodec::bitrate() const
{
	tint rate = 0;
	if(m_dsdFileHandler != NULL)
	{
		rate = m_dsdFileHandler->frequency();
	}
	return rate;
}

//-------------------------------------------------------------------------------------------

tint DSDCodec::frequency() const
{
	tint freq;
	
	if(m_pcmFrequency > 0)
	{
		freq = m_pcmFrequency;
	}
	else
	{
		freq = bitrate();
		if(m_isDSDOverPCM)
		{
			freq /= 16;
		}
	} 
	return freq;
}

//-------------------------------------------------------------------------------------------

tint DSDCodec::noChannels() const
{
	tint chs = 2;
	if(m_dsdFileHandler != NULL)
	{
		chs = m_dsdFileHandler->numberOfChannels();
	}
	return chs;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp DSDCodec::length() const
{
	common::TimeStamp tS;
	if(m_dsdFileHandler != NULL)
	{
		tfloat64 lenT = static_cast<tfloat64>(m_dsdFileHandler->totalSamples()) / static_cast<tfloat64>(m_dsdFileHandler->frequency());
		tS = lenT;
	}
	return tS;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::isLSB() const
{
	return (m_dsdFileHandler != NULL && m_dsdFileHandler->isLSB());
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::isMSB() const
{
	return (m_dsdFileHandler != NULL && m_dsdFileHandler->isMSB());
}

//-------------------------------------------------------------------------------------------

CodecDataType DSDCodec::dataTypesSupported() const
{
	CodecDataType t = 0;
	
	if(m_pcmFrequency == 0 && !m_isDSDOverPCM)
	{
		if(isLSB())
		{
			t = e_SampleDSD8LSB;
		}
		if(isMSB())
		{
			t = e_SampleDSD8MSB;
		}
		t |= e_SampleFloat | e_SampleInt24 | e_SampleInt32;
	}
	else if(m_isDSDOverPCM)
	{
		t = (m_isDSDOverPCM == 1) ? e_SampleInt24 : e_SampleInt32;
	}
	else
	{
		t = e_SampleFloat;
	}
	return t;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::setDataTypeFormat(CodecDataType type)
{
	bool res = false;
	
	if(type & e_SampleFloat)
	{
		if(m_pcmFrequency > 0)
		{
			res = true;
		}
		else
		{
			res = setOutputPCM(352800);
		}
		m_isDSDOverPCM = 0;
	}
	else if((type & e_SampleDSD8LSB) && isLSB())
	{
		m_pcmFrequency = 0;
		m_isDSDOverPCM = 0;
		res = true;
	}
	else if((type & e_SampleDSD8MSB) && isMSB())
	{
		m_pcmFrequency = 0;
		m_isDSDOverPCM = 0;
		res = true;
	}
	else if(type & e_SampleInt24)
	{
		m_pcmFrequency = 0;
		m_isDSDOverPCM = 1;
		res = true;	
	}
	else if(type & e_SampleInt32)
	{
		m_pcmFrequency = 0;
		m_isDSDOverPCM = 2;
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::initPCMOutput()
{
	bool res = true;
	
	m_pcmBufferList.clear();
	m_pcmSampleOffset = 0;
	
	m_pcmConverters.clear();
	if(m_pcmFrequency > 0)
	{
		for(int idx = 0; idx < noChannels() && res; idx++)
		{
			QSharedPointer<DSD2PCMConverter> converter(new DSD2PCMConverter());
			m_pcmBufferList.append(QByteArray());
			res = converter->setup(bitrate(), m_pcmFrequency, isLSB());
			m_pcmConverters.append(converter);
		}
	}
	else
	{
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::setOutputPCM(tint pcmFrequency)
{
	bool res;

	if(pcmFrequency < 0)
	{
		pcmFrequency = 0;
	}
	m_pcmFrequency = pcmFrequency;
	if(m_pcmFrequency > 0)
	{
		res = initPCMOutput();
	}
	else if(m_pcmFrequency == 0)
	{
		res = true;
	}
	
	if(m_pcmFrequency == 0)
	{
		m_pcmConverters.clear();
	}
	return res;
}

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
