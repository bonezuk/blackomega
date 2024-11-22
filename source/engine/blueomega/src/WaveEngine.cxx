#include "engine/blueomega/inc/WaveEngine.h"
#include "engine/inc/RData.h"
#include "common/inc/BIOMemory.h"
#include "common/inc/BIOBufferedStream.h"
#include "engine/inc/FormatTypeFromFloat.h"

#include <QFile>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blueomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CodecFactory,Codec, \
                            WaveEngineFactory,WaveEngine, \
                            "wav",false)

//-------------------------------------------------------------------------------------------

WaveEngine::WaveEngine(QObject *parent) : Codec(e_codecWav,parent),
	m_file(0),
	m_info(),
	m_offset(0),
	m_dataSize(0),
	m_dataOffset(0),
	m_dataPosition(0),
	m_length(0),
	m_buffer(0),
	m_bufferLength(0),
	m_currentTime(0),
	m_completeFlag(false),
	m_outputFormatType(e_SampleFloat)
{}

//-------------------------------------------------------------------------------------------

WaveEngine::~WaveEngine()
{
	try
	{
		if(m_buffer!=0)
		{
			delete [] m_buffer;
			m_buffer = 0;
		}
	}
	catch(...) {}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "WaveEngine::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool WaveEngine::open(const QString& name)
{
	tint size = 0;
	tuint32 id;
	common::BString str;
	bool loop = true,res = false;
	
	close();
	
	if(name.left(2)==":/")
	{
		QFile resourceFile(name);
		if(resourceFile.open(QIODevice::ReadOnly))
		{
			QByteArray resourceArray = resourceFile.readAll();
			if(!resourceArray.isEmpty())
			{
				m_file = new common::BIOMemory(resourceArray);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		m_file = new common::BIOBufferedStream(common::e_BIOStream_FileRead);
	}

	if(m_file->open(name))
	{
		if(m_info.read(m_file))
		{
			m_offset = m_info.offset();
			m_completeFlag = false;
			res = true;
			
			do
			{
				id = readChunkHeader(size);
				if(id==DATA_ID)
				{
					tfloat64 l;
					
					m_dataSize = size;
					m_dataOffset = m_file->offset();
					m_dataPosition = 0;
					l = static_cast<tfloat64>(size) / static_cast<tfloat64>(m_info.audioChannels() * m_info.sampleRate() * (m_info.bitsPerSample()>>3));
					m_length = l;
					m_currentTime = 0;
					loop = false;
				}
				else
				{
					if(!skipChunk(size))
					{
						res = false;
					}
				}
			} while(loop && res);
		}
		else
		{
			str << "File '" << name.toUtf8().constData() << "' is not a PCM encoded wav file";
			printError("open",static_cast<const tchar *>(str));
		}
	}
	else
	{
		str << "Failed to open '" << name.toUtf8().constData() << "' to read";
		printError("open",static_cast<const tchar *>(str));
	}
	
	if(!res)
	{
		delete m_file;
		m_file = 0;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void WaveEngine::close()
{
	if(m_file!=0)
	{
		m_file->close();
		delete m_file;
		m_file = 0;
	}
}

//-------------------------------------------------------------------------------------------

bool WaveEngine::init()
{
	bool res;

	if(m_file!=0)
	{
		m_info.setupChannelMap(m_noOutputChannels);
		m_initFlag = true;
		res = true;
	}
	else
	{
		res = false;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

tuint32 WaveEngine::readChunkHeader(tint& size)
{
	tuint32 id = 0;
	tchar x[4];
	
	if(m_file->read(x,4)==4)
	{
		id = intFromMemory(x);
		if(m_file->read(x,4)==4)
		{
			size = intFromMemory(x);
			m_offset += 8;
		}
		else
		{
			id = 0;
		}
	}
	return id;
}

//-------------------------------------------------------------------------------------------

bool WaveEngine::skipChunk(tint size)
{
	m_offset += size;	
	return (m_file->seek(size,common::e_Seek_Current)) ? true : false;
}

//-------------------------------------------------------------------------------------------

void WaveEngine::setupBuffer(const AData& data)
{
	tint len;
	const RData& rData = dynamic_cast<const RData&>(data);
	
	len = (m_info.bitsPerSample() >> 3) * m_info.audioChannels() * rData.rLength();
	if(len > m_bufferLength)
	{
		if(m_buffer!=0)
		{
			delete [] m_buffer;
			m_buffer = 0;
		}
		
		m_buffer = new tubyte [len];
		m_bufferLength = len;
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::setPartDataType(RData::Part& part)
{
	CodecDataType type;
	
	if((m_outputFormatType & e_SampleInt16) && (dataTypesSupported() & e_SampleInt16))
	{
		type = e_SampleInt16;
	}
	else if((m_outputFormatType & e_SampleInt24) && (dataTypesSupported() & e_SampleInt24))
	{
		type = e_SampleInt24;
	}
	else if((m_outputFormatType & e_SampleInt32) && (dataTypesSupported() & e_SampleInt32))
	{
		type = e_SampleInt32;
	}
	else
	{
		type = e_SampleFloat;
	}
	part.setDataType(type);
}

//-------------------------------------------------------------------------------------------

bool WaveEngine::next(AData& data)
{
	tint amount,len,noSamples;
	bool res = true;
	RData& rData = dynamic_cast<RData&>(data);
	
	setupBuffer(data);

	{
		RData::Part& part = rData.nextPart();
		sample_t *x = rData.partData(rData.noParts() - 1);
		
		part.start() = m_currentTime;
		if(rData.noParts()==1)
		{
			data.start() = m_currentTime;
		}
		len = (m_info.bitsPerSample() >> 3) * m_info.audioChannels() * rData.rLength();
		
		if((m_dataPosition + len) > m_dataSize)
		{
			len = m_dataSize - m_dataPosition;
		}

		amount = m_file->read(m_buffer,len);
		noSamples = amount / (m_info.audioChannels() * (m_info.bitsPerSample() >> 3));
		
		switch(m_info.type())
		{
			case WaveInformation::e_8Bit:
				copyFrom8Bit(m_buffer,x,noSamples);
				break;
				
			case WaveInformation::e_16Bit_LittleEndian:
				copyFromLE16Bit(m_buffer,x,noSamples);
				break;
				
			case WaveInformation::e_24Bit_LittleEndian:
				copyFromLE24Bit(m_buffer,x,noSamples);
				break;
				
			case WaveInformation::e_32Bit_LittleEndian:
				copyFromLE32Bit(m_buffer,x,noSamples);
				break;
				
			case WaveInformation::e_Float_LittleEndian:
				copyFromLE32Float(m_buffer,x,noSamples);
				break;
				
			case WaveInformation::e_16Bit_BigEndian:
				copyFromBE16Bit(m_buffer,x,noSamples);
				break;
				
			case WaveInformation::e_24Bit_BigEndian:
				copyFromBE24Bit(m_buffer,x,noSamples);
				break;
				
			case WaveInformation::e_32Bit_BigEndian:
				copyFromBE32Bit(m_buffer,x,noSamples);
				break;
				
			case WaveInformation::e_Float_BigEndian:
				copyFromBE32Float(m_buffer,x,noSamples);
				break;
				
			case WaveInformation::e_WaveUnknown:
				res = false;
				break;
		}
		
		m_currentTime += static_cast<tfloat64>(noSamples) / static_cast<tfloat64>(m_info.sampleRate());
		
		part.length() = noSamples;
		part.end() = m_currentTime;
		part.done() = true;
		setPartDataType(part);
		
		m_dataPosition += amount;
		if(amount<len || m_dataPosition>=m_dataSize)
		{
			m_completeFlag = true;
			res = false;
		}
	}
	
	data.end() = m_currentTime;

	return res;
}

//-------------------------------------------------------------------------------------------

bool WaveEngine::isSeek() const
{
	return true;
}

//-------------------------------------------------------------------------------------------

bool WaveEngine::seek(const common::TimeStamp& v)
{
	tint offset;
	tfloat64 a;
	bool res;
	
	if(v>=0 && v<=m_length)
	{
		a  = static_cast<tfloat64>(m_info.sampleRate()) * static_cast<tfloat64>(v);
		offset = static_cast<tint>(a);
		m_currentTime = static_cast<tfloat64>(offset) / static_cast<tfloat64>(m_info.sampleRate());
		offset *= m_info.audioChannels() * (m_info.bitsPerSample() >> 3);
		if(offset < m_dataSize)
		{
			m_dataPosition = offset;
			offset += m_dataOffset;
			res = m_file->seek(offset,common::e_Seek_Start);
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
	return res;
}

//-------------------------------------------------------------------------------------------

CodecDataType WaveEngine::dataTypesSupported() const
{
	CodecDataType types = e_SampleFloat;
	switch(m_info.type())
	{
		case WaveInformation::e_16Bit_LittleEndian:
		case WaveInformation::e_16Bit_BigEndian:
			types |= e_SampleInt16;
			break;
		case WaveInformation::e_24Bit_LittleEndian:
		case WaveInformation::e_24Bit_BigEndian:
			types |= e_SampleInt24;
			break;
		case WaveInformation::e_32Bit_LittleEndian:
		case WaveInformation::e_32Bit_BigEndian:
			types |= e_SampleInt32;
			break;
        default:
            break;
	}
	return types;
}

//-------------------------------------------------------------------------------------------

bool WaveEngine::setDataTypeFormat(CodecDataType type)
{
	bool res;
	CodecDataType caps;
	
	caps = dataTypesSupported();
	if((type == e_SampleInt16 && (caps & e_SampleInt16)) || (type == e_SampleInt24 && (caps & e_SampleInt24)) || (type == e_SampleInt32 && (caps & e_SampleInt32)))
	{
		m_outputFormatType = type;
		res = true;
	}
	else
	{
		res = Codec::setDataTypeFormat(type);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool WaveEngine::isRemote() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

bool WaveEngine::isBuffered(tfloat32& percent)
{
	return false;
}

//-------------------------------------------------------------------------------------------

tint WaveEngine::bitrate() const
{
	return (m_info.bytesPerSecond() * m_info.sampleRate()) << 3;
}

//-------------------------------------------------------------------------------------------

tint WaveEngine::frequency() const
{
	return m_info.sampleRate();
}

//-------------------------------------------------------------------------------------------

tint WaveEngine::noChannels() const
{
	return m_info.outChannelNo();
}

//-------------------------------------------------------------------------------------------

common::TimeStamp WaveEngine::length() const
{
	return m_length;
}

//-------------------------------------------------------------------------------------------

bool WaveEngine::isComplete() const
{
	return m_completeFlag;
}

//-------------------------------------------------------------------------------------------

tint WaveEngine::intFromMemory(tchar *mem) const
{
	tuint x;
	
	if(mem!=0)
	{
		x  = (static_cast<tuint>(static_cast<tint>(mem[0]))) & 0x000000ff;
		x |= (static_cast<tuint>(static_cast<tint>(mem[1])) <<  8) & 0x0000ff00;
		x |= (static_cast<tuint>(static_cast<tint>(mem[2])) << 16) & 0x00ff0000;
		x |= (static_cast<tuint>(static_cast<tint>(mem[3])) << 24) & 0xff000000;
	}
	else
	{
		x = 0;
	}
	return static_cast<tint>(x);
}

//-------------------------------------------------------------------------------------------

tint WaveEngine::shortFromMemory(tchar *mem) const
{
	tuint x;
	
	if(mem!=0)
	{
		x  = (static_cast<tuint>(static_cast<tint>(mem[0]))) & 0x000000ff;
		x |= (static_cast<tuint>(static_cast<tint>(mem[1])) << 8) & 0x0000ff00;
	}
	else
	{
		x = 0;
	}
	return static_cast<tint>(x);
}

//-------------------------------------------------------------------------------------------

void WaveEngine::blankChannelsFloat(sample_t *dst,tint noSamples)
{
	tint i,j,wChs,oChs,cIdx;
	const tint *channelMap = m_info.channelMap();
	
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	for(i=0;i<oChs;i++)
	{
        sample_t *d = dst;
	
		cIdx = -1;
		for(j=0;j<wChs && cIdx<0;j++)
		{
			if(i==channelMap[j])
			{
				cIdx = 1;
			}
		}
		if(cIdx<0)
		{
			for(j=0;j<noSamples;j++,d+=oChs)
			{
				d[i] = c_zeroSample;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::blankChannelsInt16Bit(sample_t *dst,tint noSamples)
{
	tint i,j,wChs,oChs,cIdx;
	const tint *channelMap = m_info.channelMap();
	
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	for(i=0;i<oChs;i++)
	{
        tint16 *d = reinterpret_cast<tint16 *>(dst);
	
		cIdx = -1;
		for(j=0;j<wChs && cIdx<0;j++)
		{
			if(i==channelMap[j])
			{
				cIdx = 1;
			}
		}
		if(cIdx<0)
		{
			for(j=0;j<noSamples;j++,d+=oChs)
			{
				d[i] = 0x0000;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::blankChannelsInt24Bit(sample_t *dst,tint noSamples)
{
	tint i,j,wChs,oChs,cIdx,inc;
	const tint *channelMap = m_info.channelMap();
	
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	for(i=0;i<oChs;i++)
	{
        tbyte *d = reinterpret_cast<tbyte *>(dst);
	
		cIdx = -1;
		for(j=0;j<wChs && cIdx<0;j++)
		{
			if(i==channelMap[j])
			{
				cIdx = 1;
			}
		}
		if(cIdx<0)
		{
			d += i * 3;
			inc = oChs * 3;
			for(j=0;j<noSamples;j++,d+=inc)
			{
				d[0] = 0x00;
				d[1] = 0x00;
				d[2] = 0x00;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::blankChannelsInt32Bit(sample_t *dst,tint noSamples)
{
	tint i,j,wChs,oChs,cIdx;
	const tint *channelMap = m_info.channelMap();
	
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	for(i=0;i<oChs;i++)
	{
        tint32 *d = reinterpret_cast<tint32 *>(dst);
	
		cIdx = -1;
		for(j=0;j<wChs && cIdx<0;j++)
		{
			if(i==channelMap[j])
			{
				cIdx = 1;
			}
		}
		if(cIdx<0)
		{
			for(j=0;j<noSamples;j++,d+=oChs)
			{
				d[i] = 0x00000000;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::blankChannels(sample_t *dst,tint noSamples)
{
	switch(m_outputFormatType)
	{
		case e_SampleInt16:
			blankChannelsInt16Bit(dst, noSamples);
			break;
		case e_SampleInt24:
			blankChannelsInt24Bit(dst, noSamples);
			break;
		case e_SampleInt32:
			blankChannelsInt32Bit(dst, noSamples);
			break;
		case e_SampleFloat:
		default:
			blankChannelsFloat(dst, noSamples);
			break;
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::copyFrom8Bit(tubyte *src,sample_t *dst,tint noSamples)
{
	tint i,j,oChs,wChs,cIdx;
	const tint *channelMap = m_info.channelMap();
	
	blankChannels(dst,noSamples);
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	for(i=0;i<noSamples;i++)
	{
		for(j=0;j<wChs;j++)
		{
			cIdx = channelMap[j];
			if(cIdx>=0)
			{
				dst[cIdx] = sampleFrom8Bit(src[0]);
			}
			src += 1;
		}
		dst += oChs;
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::copyFromLE16Bit(tubyte *src,sample_t *dst,tint noSamples)
{
	tint i,j,oChs,wChs,cIdx;
	tint16 v;
	const tint *channelMap = m_info.channelMap();
	
	blankChannels(dst,noSamples);
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	if(m_outputFormatType & e_SampleInt16)
	{
		tint16 *iOut = reinterpret_cast<tint16 *>(dst);
		
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = static_cast<tint16>((static_cast<tuint16>(src[0])     ) & 0x00ff);
					v |= static_cast<tint16>((static_cast<tuint16>(src[1]) << 8) & 0xff00);
					iOut[cIdx] = v;
				}
				src += 2;
			}
			iOut += oChs;
		}
	}
	else
	{
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = static_cast<tint16>((static_cast<tuint16>(src[0])     ) & 0x00ff);
					v |= static_cast<tint16>((static_cast<tuint16>(src[1]) << 8) & 0xff00);
					dst[cIdx] = sampleFrom16Bit(v);
				}
				src += 2;
			}
			dst += oChs;
		}	
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::copyFromLE24Bit(tubyte *src,sample_t *dst,tint noSamples)
{
	tint i,j,oChs,wChs,cIdx,vS;
	tuint v;
	const tint *channelMap = m_info.channelMap();
	
	blankChannels(dst,noSamples);
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	if(m_outputFormatType & e_SampleInt24)
	{
		tint32 *iOut = reinterpret_cast<tint32 *>(dst);
		
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = (static_cast<tuint>(src[2]) << 16) & 0x00ff0000;
					v |= (static_cast<tuint>(src[1]) <<  8) & 0x0000ff00;
					v |= (static_cast<tuint>(src[0])      ) & 0x000000ff;
					iOut[cIdx] = (static_cast<tint>(v << 8) >> 8);
				}
				src += 3;
			}
			iOut += oChs;
		}
	}
	else
	{
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = (static_cast<tuint>(src[2]) << 16) & 0x00ff0000;
					v |= (static_cast<tuint>(src[1]) <<  8) & 0x0000ff00;
					v |= (static_cast<tuint>(src[0])      ) & 0x000000ff;
					vS = (static_cast<tint>(v << 8) >> 8);
					dst[cIdx] = sampleFrom24Bit(vS);
				}
				src += 3;
			}
			dst += oChs;
		}
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::copyFromLE32Bit(tubyte *src,sample_t *dst,tint noSamples)
{
	tint i,j,oChs,wChs,cIdx;
	tuint v;
	const tint *channelMap = m_info.channelMap();
	
	blankChannels(dst,noSamples);
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	if(m_outputFormatType & e_SampleInt32)
	{
		tint32 *iOut = reinterpret_cast<tint32 *>(dst);
	
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = (static_cast<tuint>(src[3]) << 24) & 0xff000000;
					v |= (static_cast<tuint>(src[2]) << 16) & 0x00ff0000;
					v |= (static_cast<tuint>(src[1]) <<  8) & 0x0000ff00;
					v |= (static_cast<tuint>(src[0])      ) & 0x000000ff;
					iOut[cIdx] = static_cast<tint>(v);
				}
				src += 4;
			}
			iOut += oChs;
		}
	}
	else
	{
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = (static_cast<tuint>(src[3]) << 24) & 0xff000000;
					v |= (static_cast<tuint>(src[2]) << 16) & 0x00ff0000;
					v |= (static_cast<tuint>(src[1]) <<  8) & 0x0000ff00;
					v |= (static_cast<tuint>(src[0])      ) & 0x000000ff;
					dst[cIdx] = sampleFrom32Bit(v);
				}
				src += 4;
			}
			dst += oChs;
		}
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::copyFromLE32Float(tubyte *src,sample_t *dst,tint noSamples)
{
	tint i,j,oChs,wChs,cIdx;
	union
	{
		tuint a;
		tfloat32 b;
	} v;
	const tint *channelMap = m_info.channelMap();
	
	blankChannels(dst,noSamples);
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	for(i=0;i<noSamples;i++)
	{
		for(j=0;j<wChs;j++)
		{
			cIdx = channelMap[j];
			if(cIdx>=0)
			{
				v.a  = (static_cast<tuint>(src[3]) << 24) & 0xff000000;
				v.a |= (static_cast<tuint>(src[2]) << 16) & 0x00ff0000;
				v.a |= (static_cast<tuint>(src[1]) <<  8) & 0x0000ff00;
				v.a |= (static_cast<tuint>(src[0])      ) & 0x000000ff;
				if(v.b<-1.0f)
				{
					v.b = -1.0f;
				}
				else if(v.b>1.0f)
				{
					v.b = 1.0f;
				}
				dst[cIdx] = static_cast<sample_t>(v.b);
			}
			src += 4;
		}
		dst += oChs;
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::copyFromBE16Bit(tubyte *src,sample_t *dst,tint noSamples)
{
	tint i,j,oChs,wChs,cIdx;
	tint16 v;
	const tint *channelMap = m_info.channelMap();
	
	blankChannels(dst,noSamples);
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	if(m_outputFormatType & e_SampleInt16)
	{
		tint16 *iOut = reinterpret_cast<tint16 *>(dst);
		
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = static_cast<tint16>((static_cast<tuint16>(src[1]) << 8) & 0xff00);
					v |= static_cast<tint16>((static_cast<tuint16>(src[0])     ) & 0x00ff);
					iOut[cIdx] = v;
				}
				src += 2;
			}
			iOut += oChs;
		}
	}
	else
	{
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = static_cast<tint16>((static_cast<tuint16>(src[1]) << 8) & 0xff00);
					v |= static_cast<tint16>((static_cast<tuint16>(src[0])     ) & 0x00ff);
					dst[cIdx] = sampleFrom16Bit(v);
				}
				src += 2;
			}
			dst += oChs;
		}
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::copyFromBE24Bit(tubyte *src,sample_t *dst,tint noSamples)
{
	tint i,j,oChs,wChs,cIdx,vS;
	tuint v;
	const tint *channelMap = m_info.channelMap();
	
	blankChannels(dst,noSamples);
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	if(m_outputFormatType & e_SampleInt24)
	{
		tint32 *iOut = reinterpret_cast<tint32 *>(dst);
		
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = (static_cast<tuint>(src[0]) << 16) & 0x00ff0000;
					v |= (static_cast<tuint>(src[1]) <<  8) & 0x0000ff00;
					v |= (static_cast<tuint>(src[2])      ) & 0x000000ff;
					iOut[cIdx] = (static_cast<tint>(v << 8) >> 8);
				}
				src += 3;
			}
			iOut += oChs;
		}
	}
	else
	{
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = (static_cast<tuint>(src[0]) << 16) & 0x00ff0000;
					v |= (static_cast<tuint>(src[1]) <<  8) & 0x0000ff00;
					v |= (static_cast<tuint>(src[2])      ) & 0x000000ff;
					vS = (static_cast<tint>(v << 8) >> 8);
					dst[cIdx] = sampleFrom24Bit(vS);
				}
				src += 3;
			}
			dst += oChs;
		}
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::copyFromBE32Bit(tubyte *src,sample_t *dst,tint noSamples)
{
	tint i,j,oChs,wChs,cIdx;
	tuint v;
	const tint *channelMap = m_info.channelMap();
	
	blankChannels(dst,noSamples);
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	if(m_outputFormatType & e_SampleInt32)
	{
		tint32 *iOut = reinterpret_cast<tint32 *>(dst);
		
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = (static_cast<tuint>(src[0]) << 24) & 0xff000000;
					v |= (static_cast<tuint>(src[1]) << 16) & 0x00ff0000;
					v |= (static_cast<tuint>(src[2]) <<  8) & 0x0000ff00;
					v |= (static_cast<tuint>(src[3])      ) & 0x000000ff;
					iOut[cIdx] = static_cast<tint>(v);
				}
				src += 4;
			}
			iOut += oChs;
		}
	}
	else
	{
		for(i=0;i<noSamples;i++)
		{
			for(j=0;j<wChs;j++)
			{
				cIdx = channelMap[j];
				if(cIdx>=0)
				{
					v  = (static_cast<tuint>(src[0]) << 24) & 0xff000000;
					v |= (static_cast<tuint>(src[1]) << 16) & 0x00ff0000;
					v |= (static_cast<tuint>(src[2]) <<  8) & 0x0000ff00;
					v |= (static_cast<tuint>(src[3])      ) & 0x000000ff;
					dst[cIdx] = sampleFrom32Bit(v);
				}
				src += 4;
			}
			dst += oChs;
		}
	}
}

//-------------------------------------------------------------------------------------------

void WaveEngine::copyFromBE32Float(tubyte *src,sample_t *dst,tint noSamples)
{
	tint i,j,oChs,wChs,cIdx;
	union
	{
		tuint a;
		tfloat32 b;
	} v;
	const tint *channelMap = m_info.channelMap();
	
	blankChannels(dst,noSamples);
	oChs = m_info.outChannelNo();
	wChs = m_info.audioChannels();
	for(i=0;i<noSamples;i++)
	{
		for(j=0;j<wChs;j++)
		{
			cIdx = channelMap[j];
			if(cIdx>=0)
			{
				v.a  = (static_cast<tuint>(src[0]) << 24) & 0xff000000;
				v.a |= (static_cast<tuint>(src[1]) << 16) & 0x00ff0000;
				v.a |= (static_cast<tuint>(src[2]) <<  8) & 0x0000ff00;
				v.a |= (static_cast<tuint>(src[3])      ) & 0x000000ff;
				if(v.b<-1.0f)
				{
					v.b = -1.0f;
				}
				else if(v.b>1.0f)
				{
					v.b = 1.0f;
				}
				dst[cIdx] = static_cast<tfloat64>(v.b);
			}
			src += 4;
		}
		dst += oChs;
	}
}

//-------------------------------------------------------------------------------------------
} // namespace blueomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
