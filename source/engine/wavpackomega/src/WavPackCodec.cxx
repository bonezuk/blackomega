#include "engine/inc/FormatType.h"
#include "engine/wavpackomega/inc/WavPackBIOReader.h"
#include "engine/wavpackomega/inc/WavPackCodec.h"
#include "common/inc/BIOMemoryStream.h"
#include "common/inc/DiskOps.h"

#include <QStringList>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace wavpackomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CodecFactory,Codec, \
                            WavPackCodecFactory,WavPackCodec, \
                            "wv",false)

//-------------------------------------------------------------------------------------------

WavPackCodec::WavPackCodec(QObject *parent) : InterleavedCodec(e_codecWavPack,parent),
	m_context(0),
	m_bitsPerSample(0),
	m_channelMask(0),
	m_noWavChannels(0),
	m_sampleRate(0),
	m_channelMap(),
	m_buffer(0),
	m_readSample(0),
	m_file(0),
	m_fileCorrection(0),
	m_reader(0)
{}

//-------------------------------------------------------------------------------------------

WavPackCodec::~WavPackCodec()
{
	WavPackCodec::close();
}

//-------------------------------------------------------------------------------------------

void WavPackCodec::printError(const tchar *strR,const tchar *strE) const
{
	common::Log::g_Log << "WavPackCodec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

QString WavPackCodec::correctionFileName(const QString& name)
{
	int i;
	QString cName;

	for(i=name.length()-1;i>=0;i--)
	{
		if(name.at(i)==QChar('.'))
		{
			break;
		}
	}
	if(i>=0)
	{
		QString baseName = name.mid(0,i);
		QStringList extList;

#if defined(OMEGA_POSIX)
		extList << "wvc" << "wvC" << "wVc" << "wVC" << "Wvc" << "WvC" << "WVc" << "WVC";
#else
		extList << "wvc";
#endif

		for(QStringList::const_iterator ppI=extList.constBegin();ppI!=extList.constEnd() && cName.isEmpty(); ++ppI)
		{
			QString nName = baseName + "." + *ppI;
			if(common::DiskOps::exist(nName))
			{
				cName = nName;
			}
		}
	}
	return cName;
}

//-------------------------------------------------------------------------------------------

bool WavPackCodec::open(const QString& name)
{
	QString correctionName;
	bool res = false;
	char errorStr[128];

	close();
	
	memset(errorStr,0,128 * sizeof(char));
	
	correctionName = correctionFileName(name);

	m_file = new common::BIOBufferedStream(common::e_BIOStream_FileRead);
	if(!correctionName.isEmpty())
	{
		m_fileCorrection = new common::BIOBufferedStream(common::e_BIOStream_FileRead);
	}
	else
	{
		m_fileCorrection = 0;
	}

	if(m_file->open(name))
	{
		int flags = OPEN_NORMALIZE;

		if(m_fileCorrection!=0)
		{
			if(m_fileCorrection->open(correctionName))
			{
				flags |= OPEN_WVC;
			}
			else
			{
				delete m_fileCorrection;
				m_fileCorrection = 0;
			}
		}

		m_reader = new WavpackStreamReader;
		WavPackBIOReaderSetupReader(m_reader);

		m_context = WavpackOpenFileInputEx(m_reader,reinterpret_cast<void *>(m_file),reinterpret_cast<void *>(m_fileCorrection),errorStr,flags,0);
		if(m_context!=0)
		{
			m_noWavChannels = WavpackGetNumChannels(m_context);
			m_sampleRate = WavpackGetSampleRate(m_context);
			m_channelMask = WavpackGetChannelMask(m_context);
			
			if(WavpackGetMode(m_context) & MODE_FLOAT)
			{
				m_bitsPerSample = 0;
			}
			else
			{
				m_bitsPerSample = WavpackGetBitsPerSample(m_context);
			}
			
			m_buffer = new sample_t [m_noWavChannels * c_BufferLength];
			res = true;
		}
		else
		{
			QString err = QString("Failed to open '%1'. %2").arg(name).arg(errorStr);
			printError("open",err.toUtf8().constData());
		}
	}

	if(!res)
	{
		close();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void WavPackCodec::close()
{
	if(m_context!=0)
	{
		WavpackCloseFile(m_context);
		m_context = 0;
	}
	if(m_buffer!=0)
	{
		delete [] m_buffer;
		m_buffer = 0;
	}
	if(m_fileCorrection!=0)
	{
		delete m_fileCorrection;
		m_fileCorrection = 0;
	}
	if(m_file!=0)
	{
		delete m_file;
		m_file = 0;
	}
	if(m_reader!=0)
	{
		delete m_reader;
		m_reader = 0;
	}
}

//-------------------------------------------------------------------------------------------

bool WavPackCodec::init()
{
	bool res = false;

	if(m_context!=0)
	{
		m_channelMap.setup(m_noWavChannels,m_noOutputChannels,m_channelMask);
		setupReadSample();
		res = InterleavedCodec::init();
	}
	return res;
}

//-------------------------------------------------------------------------------------------

void WavPackCodec::readDecodedData(sample_t *output,tint sampleOffset,tint amount)
{
	tint i,j,oChs,wChs,cIdx;
	sample_t *dest;
    const tint *channelMap = m_channelMap.channelMap();
	
    oChs = m_channelMap.outChannelNo();
	wChs = m_noWavChannels;
	
	dest = &output[sampleOffset * oChs];
	
    memset(dest,0,oChs * amount * sizeof(sample_t));

	for(i=0;i<amount;i++)
	{
		for(j=0;j<wChs;j++)
		{
			cIdx = channelMap[j];
			if(cIdx>=0)
			{
				dest[j] = readSample(m_pBuffer);
			}
			m_pBuffer += bytesPerSample();
		}
		dest += oChs;
	}
	
	springClean();
}

//-------------------------------------------------------------------------------------------

void WavPackCodec::springClean()
{}

//-------------------------------------------------------------------------------------------

bool WavPackCodec::decodeNextPacket(int& outLen)
{
    outLen = static_cast<int>(WavpackUnpackSamples(m_context,reinterpret_cast<int32_t *>(m_buffer),c_BufferLength));
	return (outLen > 0);
}

//-------------------------------------------------------------------------------------------

char *WavPackCodec::getPacketBuffer()
{
    return reinterpret_cast<char *>(m_buffer);
}

//-------------------------------------------------------------------------------------------

int WavPackCodec::bytesPerSample()
{
	return 4;
}

//-------------------------------------------------------------------------------------------

bool WavPackCodec::isSeek() const
{
	return true;
}

//-------------------------------------------------------------------------------------------

bool WavPackCodec::seek(const common::TimeStamp& v)
{
	bool res = false;
	uint32_t nBlockOffset = static_cast<tint32>(floor(static_cast<tfloat64>(v) * static_cast<tfloat64>(m_sampleRate)));
	
	if(WavpackSeekSample(m_context,nBlockOffset))
	{
		m_state = 0;
		m_time = static_cast<tfloat64>(nBlockOffset) / static_cast<tfloat64>(m_sampleRate);
		res = true;
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool WavPackCodec::isComplete() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

bool WavPackCodec::isRemote() const
{
	return false;
}

//-------------------------------------------------------------------------------------------

bool WavPackCodec::isBuffered(tfloat32& percent)
{
	return false;
}

//-------------------------------------------------------------------------------------------

tint WavPackCodec::bitrate() const
{
	return static_cast<tint>(WavpackGetAverageBitrate(m_context,(m_fileCorrection!=0) ? 1 : 0));
}

//-------------------------------------------------------------------------------------------

tint WavPackCodec::frequency() const
{
	return m_sampleRate;
}

//-------------------------------------------------------------------------------------------

tint WavPackCodec::noChannels() const
{
    return m_channelMap.outChannelNo();
}

//-------------------------------------------------------------------------------------------

common::TimeStamp WavPackCodec::length() const
{
	tfloat64 v = static_cast<tfloat64>(WavpackGetNumSamples(m_context)) / static_cast<tfloat64>(m_sampleRate);
	common::TimeStamp t(v);
	return t;
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSample(char *buffer)
{
    return static_cast<tfloat64>(m_readSample(buffer));
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleFloat(char *buffer)
{
    tfloat32 *x = reinterpret_cast<tfloat32 *>(buffer);
    tfloat32 v = x[0];
	if(v < -1.0f)
	{
		v = 1.0f;
	}
	else if(v > 1.0f)
	{
		v = 1.0f;
	}
    return static_cast<sample_t>(v);
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger1BitLE(char *buffer)
{
	tbyte t = static_cast<tbyte>(static_cast<tubyte>(buffer[0]) >> 7);
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom1Bit(&t);
#else
	return toSample64From1Bit(&t);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger1BitBE(char *buffer)
{
	tbyte t = static_cast<tbyte>(static_cast<tubyte>(buffer[3]) >> 7);
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom1Bit(&t);
#else
	return toSample64From1Bit(&t);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger2BitsLE(char *buffer)
{
	tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(&buffer[0])) >> 6;
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFromBits(x,2);
#else
	return toSample64FromBits(x,2);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger2BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom2Bits(&buffer[3]);
#else
	return toSample64From2Bits(&buffer[3]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger3BitsLE(char *buffer)
{
	tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(&buffer[0])) >> 5;
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFromBits(x,3);
#else
	return toSample64FromBits(x,3);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger3BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom3Bits(&buffer[3]);
#else
	return toSample64From3Bits(&buffer[3]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger4BitsLE(char *buffer)
{
	tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(&buffer[0])) >> 4;
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFromBits(x,4);
#else
	return toSample64FromBits(x,4);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger4BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom4Bits(&buffer[3]);
#else
	return toSample64From4Bits(&buffer[3]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger5BitsLE(char *buffer)
{
	tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(&buffer[0])) >> 3;
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFromBits(x,5);
#else
	return toSample64FromBits(x,5);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger5BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom5Bits(&buffer[3]);
#else
	return toSample64From5Bits(&buffer[3]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger6BitsLE(char *buffer)
{
	tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(&buffer[0])) >> 2;
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFromBits(x,6);
#else
	return toSample64FromBits(x,6);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger6BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom6Bits(&buffer[3]);
#else
	return toSample64From6Bits(&buffer[3]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger7BitsLE(char *buffer)
{
	tint32 x = static_cast<tint32>(to8BitUnsignedFromLittleEndian(&buffer[0])) >> 1;
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFromBits(x,7);
#else
	return toSample64FromBits(x,7);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger7BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom7Bits(&buffer[3]);
#else
	return toSample64From7Bits(&buffer[3]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger8BitsLE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom8Bits(&buffer[0]);
#else
	return toSample64From8Bits(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger8BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom8Bits(&buffer[3]);
#else
	return toSample64From8Bits(&buffer[3]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger9BitsLE(char *buffer)
{
	tint32 x = static_cast<tint32>(to16BitSignedFromLittleEndian(&buffer[0])) >> 7;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,9);
#else
	return toSample64FromBits(x,9);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger9BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom9BitsFromBigEndian(&buffer[2]);
#else
	return toSample64From9BitsFromBigEndian(&buffer[2]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger10BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to16BitSignedFromLittleEndian(&buffer[0])) >> 6;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,10);
#else
	return toSample64FromBits(x,10);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger10BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom10BitsFromBigEndian(&buffer[2]);
#else
	return toSample64From10BitsFromBigEndian(&buffer[2]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger11BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to16BitSignedFromLittleEndian(&buffer[0])) >> 5;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,11);
#else
	return toSample64FromBits(x,11);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger11BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom11BitsFromBigEndian(&buffer[2]);
#else
	return toSample64From11BitsFromBigEndian(&buffer[2]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger12BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to16BitSignedFromLittleEndian(&buffer[0])) >> 4;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,12);
#else
	return toSample64FromBits(x,12);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger12BitsBE(char *buffer)
{
    tint32 x = static_cast<tint32>(to16BitSignedFromBigEndian(&buffer[2])) >> 4;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,12);
#else
	return toSample64FromBits(x,12);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger13BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to16BitSignedFromLittleEndian(&buffer[0])) >> 3;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,13);
#else
	return toSample64FromBits(x,13);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger13BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom13BitsFromBigEndian(&buffer[2]);
#else
	return toSample64From13BitsFromBigEndian(&buffer[2]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger14BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to16BitSignedFromLittleEndian(&buffer[0])) >> 2;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,14);
#else
	return toSample64FromBits(x,14);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger14BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom14BitsFromBigEndian(&buffer[2]);
#else
	return toSample64From14BitsFromBigEndian(&buffer[2]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger15BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to16BitSignedFromLittleEndian(&buffer[0])) >> 1;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,15);
#else
	return toSample64FromBits(x,15);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger15BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom15BitsFromBigEndian(&buffer[2]);
#else
	return toSample64From15BitsFromBigEndian(&buffer[2]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger16BitsLE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom16BitsFromLittleEndian(&buffer[0]);
#else
	return toSample64From16BitsFromLittleEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger16BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom16BitsFromBigEndian(&buffer[2]);
#else
	return toSample64From16BitsFromBigEndian(&buffer[2]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger17BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to24BitSignedFromLittleEndian(&buffer[0])) >> 7;
#if defined(SINGLE_FLOAT_SAMPLE) 
    return toSampleFromBits(x,17);
#else
	return toSample64FromBits(x,17);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger17BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom17BitsFromBigEndian(&buffer[1]);
#else
	return toSample64From17BitsFromBigEndian(&buffer[1]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger18BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to24BitSignedFromLittleEndian(&buffer[0])) >> 6;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,18);
#else
	return toSample64FromBits(x,18);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger18BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom18BitsFromBigEndian(&buffer[1]);
#else
	return toSample64From18BitsFromBigEndian(&buffer[1]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger19BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to24BitSignedFromLittleEndian(&buffer[0])) >> 5;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,19);
#else
	return toSample64FromBits(x,19);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger19BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom19BitsFromBigEndian(&buffer[1]);
#else
	return toSample64From19BitsFromBigEndian(&buffer[1]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger20BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to24BitSignedFromLittleEndian(&buffer[0])) >> 4;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,20);
#else
	return toSample64FromBits(x,20);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger20BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom20BitsFromBigEndian(&buffer[1]);
#else
	return toSample64From20BitsFromBigEndian(&buffer[1]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger21BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to24BitSignedFromLittleEndian(&buffer[0])) >> 3;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,21);
#else
	return toSample64FromBits(x,21);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger21BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom21BitsFromBigEndian(&buffer[1]);
#else
	return toSample64From21BitsFromBigEndian(&buffer[1]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger22BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to24BitSignedFromLittleEndian(&buffer[0])) >> 2;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,22);
#else
	return toSample64FromBits(x,22);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger22BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom22BitsFromBigEndian(&buffer[1]);
#else
	return toSample64From22BitsFromBigEndian(&buffer[1]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger23BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to24BitSignedFromLittleEndian(&buffer[0])) >> 1;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,23);
#else
	return toSample64FromBits(x,23);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger23BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom23BitsFromBigEndian(&buffer[1]);
#else
	return toSample64From23BitsFromBigEndian(&buffer[1]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger24BitsLE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom24BitsFromLittleEndian(&buffer[0]);
#else
	return toSample64From24BitsFromLittleEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger24BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom24BitsFromBigEndian(&buffer[1]);
#else
	return toSample64From24BitsFromBigEndian(&buffer[1]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger25BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(&buffer[0])) >> 7;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,25);
#else
	return toSample64FromBits(x,25);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger25BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom25BitsFromBigEndian(&buffer[0]);
#else
	return toSample64From25BitsFromBigEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger26BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(&buffer[0])) >> 6;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,26);
#else
	return toSample64FromBits(x,26);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger26BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom26BitsFromBigEndian(&buffer[0]);
#else
	return toSample64From26BitsFromBigEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger27BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(&buffer[0])) >> 5;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,27);
#else
	return toSample64FromBits(x,27);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger27BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom27BitsFromBigEndian(&buffer[0]);
#else
	return toSample64From27BitsFromBigEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger28BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(&buffer[0])) >> 4;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,28);
#else
	return toSample64FromBits(x,28);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger28BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom28BitsFromBigEndian(&buffer[0]);
#else
	return toSample64From28BitsFromBigEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger29BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(&buffer[0])) >> 3;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,29);
#else
	return toSample64FromBits(x,29);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger29BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom29BitsFromBigEndian(&buffer[0]);
#else
	return toSample64From29BitsFromBigEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger30BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(&buffer[0])) >> 2;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,30);
#else
	return toSample64FromBits(x,30);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger30BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom30BitsFromBigEndian(&buffer[0]);
#else
	return toSample64From30BitsFromBigEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger31BitsLE(char *buffer)
{
    tint32 x = static_cast<tint32>(to32BitSignedFromLittleEndian(&buffer[0])) >> 1;
#if defined(SINGLE_FLOAT_SAMPLE)
    return toSampleFromBits(x,31);
#else
	return toSample64FromBits(x,31);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger31BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom31BitsFromBigEndian(&buffer[0]);
#else
	return toSample64From31BitsFromBigEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger32BitsLE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom32BitsFromLittleEndian(&buffer[0]);
#else
	return toSample64From32BitsFromLittleEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

sample_t WavPackCodec::readSampleInteger32BitsBE(char *buffer)
{
#if defined(SINGLE_FLOAT_SAMPLE)
	return toSampleFrom32BitsFromBigEndian(&buffer[0]);
#else
	return toSample64From32BitsFromBigEndian(&buffer[0]);
#endif
}

//-------------------------------------------------------------------------------------------

void WavPackCodec::setupReadSample()
{
	if(isLittleEndian())
	{
		setupReadSampleLE();
	}
	else
	{
		setupReadSampleBE();
	}
}

//-------------------------------------------------------------------------------------------

void WavPackCodec::setupReadSampleLE()
{
	switch(m_bitsPerSample)
	{
		case 1:
			m_readSample = readSampleInteger1BitLE;
			break;
			
		case 2:
			m_readSample = readSampleInteger2BitsLE;
			break;

		case 3:
			m_readSample = readSampleInteger3BitsLE;
			break;

		case 4:
			m_readSample = readSampleInteger4BitsLE;
			break;

		case 5:
			m_readSample = readSampleInteger5BitsLE;
			break;

		case 6:
			m_readSample = readSampleInteger6BitsLE;
			break;

		case 7:
			m_readSample = readSampleInteger7BitsLE;
			break;

		case 8:
			m_readSample = readSampleInteger8BitsLE;
			break;

		case 9:
			m_readSample = readSampleInteger9BitsLE;
			break;

		case 10:
			m_readSample = readSampleInteger10BitsLE;
			break;

		case 11:
			m_readSample = readSampleInteger11BitsLE;
			break;

		case 12:
			m_readSample = readSampleInteger12BitsLE;
			break;

		case 13:
			m_readSample = readSampleInteger13BitsLE;
			break;

		case 14:
			m_readSample = readSampleInteger14BitsLE;
			break;

		case 15:
			m_readSample = readSampleInteger15BitsLE;
			break;

		case 16:
			m_readSample = readSampleInteger16BitsLE;
			break;

		case 17:
			m_readSample = readSampleInteger17BitsLE;
			break;

		case 18:
			m_readSample = readSampleInteger18BitsLE;
			break;

		case 19:
			m_readSample = readSampleInteger19BitsLE;
			break;

		case 20:
			m_readSample = readSampleInteger20BitsLE;
			break;

		case 21:
			m_readSample = readSampleInteger21BitsLE;
			break;

		case 22:
			m_readSample = readSampleInteger22BitsLE;
			break;

		case 23:
			m_readSample = readSampleInteger23BitsLE;
			break;

		case 24:
			m_readSample = readSampleInteger24BitsLE;
			break;

		case 25:
			m_readSample = readSampleInteger25BitsLE;
			break;

		case 26:
			m_readSample = readSampleInteger26BitsLE;
			break;

		case 27:
			m_readSample = readSampleInteger27BitsLE;
			break;

		case 28:
			m_readSample = readSampleInteger28BitsLE;
			break;

		case 29:
			m_readSample = readSampleInteger29BitsLE;
			break;

		case 30:
			m_readSample = readSampleInteger30BitsLE;
			break;

		case 31:
			m_readSample = readSampleInteger31BitsLE;
			break;

		case 32:
			m_readSample = readSampleInteger32BitsLE;
			break;

		case 0:
		default:
			m_readSample = readSampleFloat;
			break;
	}
}

//-------------------------------------------------------------------------------------------

void WavPackCodec::setupReadSampleBE()
{
	switch(m_bitsPerSample)
	{
		case 1:
			m_readSample = readSampleInteger1BitBE;
			break;
			
		case 2:
			m_readSample = readSampleInteger2BitsBE;
			break;

		case 3:
			m_readSample = readSampleInteger3BitsBE;
			break;

		case 4:
			m_readSample = readSampleInteger4BitsBE;
			break;

		case 5:
			m_readSample = readSampleInteger5BitsBE;
			break;

		case 6:
			m_readSample = readSampleInteger6BitsBE;
			break;

		case 7:
			m_readSample = readSampleInteger7BitsBE;
			break;

		case 8:
			m_readSample = readSampleInteger8BitsBE;
			break;

		case 9:
			m_readSample = readSampleInteger9BitsBE;
			break;

		case 10:
			m_readSample = readSampleInteger10BitsBE;
			break;

		case 11:
			m_readSample = readSampleInteger11BitsBE;
			break;

		case 12:
			m_readSample = readSampleInteger12BitsBE;
			break;

		case 13:
			m_readSample = readSampleInteger13BitsBE;
			break;

		case 14:
			m_readSample = readSampleInteger14BitsBE;
			break;

		case 15:
			m_readSample = readSampleInteger15BitsBE;
			break;

		case 16:
			m_readSample = readSampleInteger16BitsBE;
			break;

		case 17:
			m_readSample = readSampleInteger17BitsBE;
			break;

		case 18:
			m_readSample = readSampleInteger18BitsBE;
			break;

		case 19:
			m_readSample = readSampleInteger19BitsBE;
			break;

		case 20:
			m_readSample = readSampleInteger20BitsBE;
			break;

		case 21:
			m_readSample = readSampleInteger21BitsBE;
			break;

		case 22:
			m_readSample = readSampleInteger22BitsBE;
			break;

		case 23:
			m_readSample = readSampleInteger23BitsBE;
			break;

		case 24:
			m_readSample = readSampleInteger24BitsBE;
			break;

		case 25:
			m_readSample = readSampleInteger25BitsBE;
			break;

		case 26:
			m_readSample = readSampleInteger26BitsBE;
			break;

		case 27:
			m_readSample = readSampleInteger27BitsBE;
			break;

		case 28:
			m_readSample = readSampleInteger28BitsBE;
			break;

		case 29:
			m_readSample = readSampleInteger29BitsBE;
			break;

		case 30:
			m_readSample = readSampleInteger30BitsBE;
			break;

		case 31:
			m_readSample = readSampleInteger31BitsBE;
			break;

		case 32:
			m_readSample = readSampleInteger32BitsBE;
			break;

		case 0:
		default:
			m_readSample = readSampleFloat;
			break;
	}
}

//-------------------------------------------------------------------------------------------
} // namespace wavpackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
