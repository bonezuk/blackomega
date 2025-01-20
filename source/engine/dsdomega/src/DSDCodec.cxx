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
	m_dsfHandler(0),
	m_inBufferList(),
	m_inSampleOffset(0),
	m_inBlockNumber(0)
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
			m_dsfHandler = new DSFFileReader(m_file);
			if(m_dsfHandler->parse())
			{
				setupDSFBuffers();
				m_inBlockNumber = 0;
				res = readInNextDSFBlock();
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
	if(m_dsfHandler != NULL)
	{
		delete m_dsfHandler;
		m_dsfHandler = NULL;
	}
	if(m_file != NULL)
	{
		m_file->close();
		delete m_file;
		m_file = NULL;
	}
}

//-------------------------------------------------------------------------------------------

void DSDCodec::setupDSFBuffers()
{
	freeDSFBuffers();
	for(tint i = 0; i < m_dsfHandler->numberOfChannels(); i++)
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
		if(!m_dsfHandler->data(m_inBlockNumber, channelIndex, arr))
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
	tint64 noBits = ((static_cast<tint64>(blockIdx) * m_dsfHandler->channelBlockSize()) + offset) << 3;
	return noBits;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp DSDCodec::timeAtInDSF(tint blockIdx, tint offset)
{
	tfloat64 tS = static_cast<tfloat64>(bitAtInDSF(blockIdx, offset)) / static_cast<tfloat64>(m_dsfHandler->frequency());
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

bool DSDCodec::next(AData& data)
{
	bool res = true;
	tint64 bitPosition = bitAtInDSF(m_inBlockNumber, m_inSampleOffset);
	
	if(bitPosition < m_dsfHandler->totalSamples())
	{
		tint i, pos, len, chIdx;
		RData& rData = dynamic_cast<RData&>(data);
		RData::Part& part = rData.nextPart();
		tubyte *out = reinterpret_cast<tubyte *>(rData.partData(rData.noParts() - 1));
		common::TimeStamp startTs = timeAtInDSF(m_inBlockNumber, m_inSampleOffset);
		
		part.start() = startTs;
		if(rData.noParts() == 1)
		{
			data.start() = startTs;
		}
		
		len = rData.rLength() * sizeof(sample_t);
		pos = 0;
		while(pos < len && res)
		{
			while(pos < len && m_inSampleOffset < currentBlockLength())
			{
				for(chIdx = 0; chIdx < m_dsfHandler->numberOfChannels(); chIdx++)
				{
					const tubyte *in = reinterpret_cast<const tubyte *>(m_inBufferList.at(chIdx).constData());
					for(i = 0; i < sizeof(sample_t); i++)
					{
						*out++ = in[m_inSampleOffset + i];
					}
				}
				pos += sizeof(sample_t);
				m_inSampleOffset += sizeof(sample_t);
			}
			if(pos < len && m_inSampleOffset >= currentBlockLength())
			{
				m_inBlockNumber++;
				res = readInNextDSFBlock();
			}
		}
		
		common::TimeStamp endTs = timeAtInDSF(m_inBlockNumber, m_inSampleOffset);
		part.length() = pos / sizeof(sample_t);
		part.end() = endTs;
		part.done() = true;
		part.setDataType(m_dsfHandler->isLSB() ? e_SampleDSD8LSB : e_SampleDSD8MSB);
		data.end() = endTs;
	}
	else
	{
		res = false;
	}
	return res;
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
	
	if(m_dsfHandler != NULL && v < length())
	{
		tfloat64 pos = static_cast<tfloat64>(v) * static_cast<tfloat64>(m_dsfHandler->frequency());
		tint64 bytePosition = static_cast<tint64>(pos) / 8;
		m_inBlockNumber = static_cast<tint>(bytePosition / m_dsfHandler->channelBlockSize());
		m_inSampleOffset = static_cast<tint>(bytePosition % m_dsfHandler->channelBlockSize());
		res = readInNextDSFBlock();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::isComplete() const
{
	bool res = false;
	tint64 bitPos = bitAtInDSF(m_inBlockNumber, m_inSampleOffset);
	if(m_dsfHandler != NULL)
	{
		res = (bitPos >= m_dsfHandler->totalSamples());
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
	if(m_dsfHandler != NULL)
	{
		rate = m_dsfHandler->frequency();
	}
	return rate;
}

//-------------------------------------------------------------------------------------------

tint DSDCodec::frequency() const
{
	return bitrate();
}

//-------------------------------------------------------------------------------------------

tint DSDCodec::noChannels() const
{
	tint chs = 2;
	if(m_dsfHandler != NULL)
	{
		chs = m_dsfHandler->numberOfChannels();
	}
	return chs;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp DSDCodec::length() const
{
	common::TimeStamp tS;
	if(m_dsfHandler != NULL)
	{
		tfloat64 lenT = static_cast<tfloat64>(m_dsfHandler->totalSamples()) / static_cast<tfloat64>(m_dsfHandler->frequency());
		tS = lenT;
	}
	return tS;
}

//-------------------------------------------------------------------------------------------

CodecDataType DSDCodec::dataTypesSupported() const
{
	CodecDataType t = e_SampleDSD8LSB;
	if(m_dsfHandler != NULL && m_dsfHandler->isMSB())
	{
		t = e_SampleDSD8MSB;
	}
	return t;
}

//-------------------------------------------------------------------------------------------

bool DSDCodec::setDataTypeFormat(CodecDataType type)
{
	return (type == dataTypesSupported());
}

//-------------------------------------------------------------------------------------------
} // namespace dsd
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
