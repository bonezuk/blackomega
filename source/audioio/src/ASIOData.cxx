#include "audioio/inc/ASIOData.h"
#include "engine/inc/FormatTypeFromFloat.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

ASIOData::ASIOData() : engine::RData(),
	m_sampleType(ASIOSTFloat32LSB),
	m_asioDataArray(0),
	m_asioAuxDataArray(0),
	m_convertFlag(false),
	m_volume(1.0),
	m_vIntBuffer(NULL),
	m_vIntBufSize(0)
{}

//-------------------------------------------------------------------------------------------

ASIOData::ASIOData(tint len,tint inChannel,tint outChannel) : engine::RData(len,inChannel,outChannel),
	m_sampleType(ASIOSTFloat32LSB),
	m_asioDataArray(0),
	m_asioAuxDataArray(0),
	m_convertFlag(false),
	m_volume(1.0),
	m_vIntBuffer(NULL),
	m_vIntBufSize(0)
{}

//-------------------------------------------------------------------------------------------

ASIOData::ASIOData(const engine::AData& rhs) : engine::RData(),
	m_sampleType(ASIOSTFloat32LSB),
	m_asioDataArray(0),
	m_asioAuxDataArray(0),
	m_convertFlag(false),
	m_volume(1.0),
	m_vIntBuffer(NULL),
	m_vIntBufSize(0)
{
	ASIOData::copy(rhs);
}

//-------------------------------------------------------------------------------------------

ASIOData::~ASIOData()
{
	tint i;

	if(m_asioDataArray!=0)
	{
		for(i=0;i<m_noOutChannels;++i)
		{
			::free(m_asioDataArray[i]);
		}
		::free(m_asioDataArray);
		m_asioDataArray = 0;
	}
	if(m_asioAuxDataArray != 0)
	{
		for(i = 0; i < m_asioAuxArraySize; i++)
		{
			::free(m_asioAuxDataArray[i]);
		}
		::free(m_asioAuxDataArray);
		m_asioAuxDataArray = 0;
	}
	if(m_vIntBuffer != NULL)
	{
		delete [] m_vIntBuffer;
		m_vIntBuffer = NULL;
	}
}

//-------------------------------------------------------------------------------------------

void ASIOData::copy(const engine::AData& rhs)
{
	engine::RData::copy(rhs);
	try
	{
		tint i;
		const ASIOData& d = dynamic_cast<const ASIOData&>(rhs);
		
		asioAllocate();
		for(i=0;i<m_noOutChannels;++i)
		{
			::memcpy(m_asioDataArray[i],d.m_asioDataArray[i],length() * getSampleSize());
		}
		for(i = 0; i < m_asioAuxArraySize; i++)
		{
			::memcpy(m_asioAuxDataArray[i],d.m_asioAuxDataArray[i],length() * getSampleSize());
		}
		m_convertFlag = d.m_convertFlag;
		m_volume = d.m_volume;
	}
	catch(...) {}
}

//-------------------------------------------------------------------------------------------

void ASIOData::asioAllocate()
{
	tint i;
	
	if(m_asioDataArray!=0)
	{
		for(i=0;i<m_noOutChannels;++i)
		{
			::free(m_asioDataArray[i]);
		}
		::free(m_asioDataArray);
	}
	
	m_asioDataArray = reinterpret_cast<void **>(::malloc(sizeof(void *) * m_noOutChannels));
	if(m_asioDataArray != 0)
	{
		for(i = 0; i < m_noOutChannels; ++i)
		{
			m_asioDataArray[i] = ::malloc(length() * getSampleSize());
		}
	}
	m_asioAuxDataArray = reinterpret_cast<void **>(::malloc(sizeof(void *) * m_asioAuxArraySize));
	if(m_asioAuxDataArray != 0)
	{
		for(i = 0; i < m_asioAuxArraySize; ++i)
		{
			m_asioAuxDataArray[i] = ::malloc(length() * getSampleSize());
		}	
	}
}

//-------------------------------------------------------------------------------------------

void *ASIOData::asioData(tint chIdx,tint pIdx)
{
	return const_cast<void *>(asioDataI(chIdx,pIdx));
}

//-------------------------------------------------------------------------------------------

const void *ASIOData::asioDataConst(tint chIdx,tint pIdx) const
{
	return asioDataI(chIdx,pIdx);
}

//-------------------------------------------------------------------------------------------

const void *ASIOData::asioDataI(tint chIdx,tint pIdx) const
{
	const tubyte *mem = 0;

	if(chIdx<m_noOutChannels && pIdx<noParts())
	{
		if(chIdx >= 0 && chIdx < m_noOutChannels)
		{
			mem = reinterpret_cast<const tubyte *>(m_asioDataArray[chIdx]);
			mem = &mem[ m_parts.at(pIdx).offsetConst() * getSampleSize() ];
		}
		else if(chIdx>=engine::e_lfeChannelIndex && chIdx<= engine::e_centerChannelIndex)
		{
			mem = reinterpret_cast<const tubyte *>(m_asioAuxDataArray[(0 - chIdx) - m_asioAuxArraySize]);
			mem = &mem[ m_parts.at(pIdx).offsetConst() * getSampleSize() ];
		}
	}
	return reinterpret_cast<const void *>(mem);
}

//-------------------------------------------------------------------------------------------

void *ASIOData::asioDataChannelIndex(tint chIdx)
{
	tubyte *mem = 0;
	
	if(chIdx >= 0 && chIdx < m_noOutChannels)
	{
		mem = reinterpret_cast<tubyte *>(m_asioDataArray[chIdx]);
	}
	else if(chIdx>=engine::e_lfeChannelIndex && chIdx<= engine::e_centerChannelIndex)
	{
		mem = reinterpret_cast<tubyte *>(m_asioAuxDataArray[(0 - chIdx) - m_asioAuxArraySize]);
	}
	return reinterpret_cast<void *>(mem);
}

//-------------------------------------------------------------------------------------------

void ASIOData::setSampleType(ASIOSampleType t)
{
	m_sampleType = t;
	asioAllocate();
}

//-------------------------------------------------------------------------------------------

tint ASIOData::getSampleSize() const
{
	return ASIODriverService::getSampleSize(m_sampleType);
}

//-------------------------------------------------------------------------------------------

void ASIOData::setVolume(sample_t v)
{
	m_volume = v;
}

//-------------------------------------------------------------------------------------------

void ASIOData::reset()
{
	engine::RData::reset();
	m_convertFlag = false;
}

//-------------------------------------------------------------------------------------------

void ASIOData::convert()
{
	if(!m_convertFlag)
	{
		tint i,j,pLen,amount, nOChannels;
		tint offsetCenter = 0;
		tint offsetLFE = 0;
		QVector<tint> amountS(m_noOutChannels,0);
	
		for(i=0;i<noParts();++i)
		{
			sample_t *in = partDataOut(i);
		
			pLen = part(i).length();
			for(j=0;j<m_noOutChannels;++j)
			{
				if(part(i).getDataType() == engine::e_SampleInt16)
				{
					tint16 *inInt16 = reinterpret_cast<tint16 *>(in);
					amount = copyToBuffer(reinterpret_cast<sample_t *>(&inInt16[j]),pLen,amountS.at(j),j,part(i).getDataType());
				}
				else if(part(i).getDataType() == engine::e_SampleInt24 || part(i).getDataType() == engine::e_SampleInt32)
				{
					tint32 *inInt32 = reinterpret_cast<tint32 *>(in);
					amount = copyToBuffer(reinterpret_cast<sample_t *>(&inInt32[j]),pLen,amountS.at(j),j,part(i).getDataType());
				}
				else
				{
					amount = copyToBuffer(&in[j],pLen,amountS.at(j),j,part(i).getDataType());
				}

				Q_ASSERT(pLen == amount);
				amountS[j] += amount;
			}
			nOChannels = m_noOutChannels;
			m_noOutChannels = 1;
			if(isCenter())
			{
				sample_t *in = partDataCenter(i);
				offsetCenter += copyToBuffer(in, pLen, offsetCenter, engine::e_centerChannelIndex, engine::e_SampleFloat);
			}
			if(isLFE())
			{
				sample_t *in = partFilterData(i, engine::e_lfeChannelIndex);
				offsetLFE += copyToBuffer(in, pLen, offsetLFE, engine::e_lfeChannelIndex, engine::e_SampleFloat);
			}
			m_noOutChannels = nOChannels;
		}
		m_convertFlag = true;
	}
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt16LSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint16 *out = reinterpret_cast<tint16 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			engine::write16BitsLittleEndianFromSample(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::write16BitsLittleEndianFromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write16BitsLittleEndianFromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write16BitsLittleEndianFromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt16MSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint16 *out = reinterpret_cast<tint16 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			engine::write16BitsBigEndianFromSample(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::write16BitsBigEndianFromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write16BitsBigEndianFromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write16BitsBigEndianFromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt24LSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tbyte *out = reinterpret_cast<tbyte *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset*3;i<len;i++,j+=3,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			engine::write24BitsLittleEndianFromSample(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return (j - (oOffset * 3)) / 3;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset*3;i<len;i++,j+=3,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::write24BitsLittleEndianFromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return (j - (oOffset * 3)) / 3;	
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset*3;i<len;i++,j+=3,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write24BitsLittleEndianFromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return (j - (oOffset * 3)) / 3;	
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset*3;i<len;i++,j+=3,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write24BitsLittleEndianFromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return (j - (oOffset * 3)) / 3;	
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt24MSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tbyte *out = reinterpret_cast<tbyte *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset*3;i<len;i++,j+=3,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			engine::write24BitsBigEndianFromSample(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return (j - (oOffset * 3)) / 3;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset*3;i<len;i++,j+=3,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::write24BitsBigEndianFromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return (j - (oOffset * 3)) / 3;	
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset*3;i<len;i++,j+=3,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write24BitsBigEndianFromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return (j - (oOffset * 3)) / 3;	
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset*3;i<len;i++,j+=3,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write24BitsBigEndianFromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return (j - (oOffset * 3)) / 3;	
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt32LSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint32 *out = reinterpret_cast<tint32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			engine::write32BitsLittleEndianFromSample(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::write32BitsLittleEndianFromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write32BitsLittleEndianFromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write32BitsLittleEndianFromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt32MSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint32 *out = reinterpret_cast<tint32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			engine::write32BitsBigEndianFromSample(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::write32BitsBigEndianFromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write32BitsBigEndianFromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::write32BitsBigEndianFromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferFloat32LSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tfloat32 *out = reinterpret_cast<tfloat32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			tfloat32 x = static_cast<tfloat32>(volumeAndClip(*src));
			engine::writeNative32BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;	
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat32 x = engine::sample32From16Bit(*in);
			engine::writeNative32BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat32 x = engine::sample32From24Bit(*in);
			engine::writeNative32BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat32 x = engine::sample32From32Bit(*in);
			engine::writeNative32BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferFloat32MSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tfloat32 *out = reinterpret_cast<tfloat32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			tfloat32 x = static_cast<tfloat32>(volumeAndClip(*src));
			engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat32 x = engine::sample32From16Bit(*in);
			engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat32 x = engine::sample32From24Bit(*in);
			engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat32 x = engine::sample32From32Bit(*in);
			engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferFloat64LSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tfloat64 *out = reinterpret_cast<tfloat64 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			tfloat64 x = static_cast<tfloat64>(volumeAndClip(*src));
			engine::writeNative64BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat64 x = engine::sample64From16Bit(*in);
			engine::writeNative64BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat64 x = engine::sample64From24Bit(*in);
			engine::writeNative64BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat64 x = engine::sample64From32Bit(*in);
			engine::writeNative64BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferFloat64MSB(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tfloat64 *out = reinterpret_cast<tfloat64 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			tfloat64 x = static_cast<tfloat64>(volumeAndClip(*src));
			engine::writeNative64BitsAsBigEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat64 x = engine::sample64From16Bit(*in);
			engine::writeNative64BitsAsBigEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat64 x = engine::sample64From24Bit(*in);
			engine::writeNative64BitsAsBigEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tfloat64 x = engine::sample64From32Bit(*in);
			engine::writeNative64BitsAsBigEndian(reinterpret_cast<const tbyte *>(&x),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt32LSB16(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint32 *out = reinterpret_cast<tint32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			tint32 y = engine::sampleToInteger(x,16);
			engine::writeNative32BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&y),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::writeInt32LSB16FromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32LSB16FromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32LSB16FromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt32MSB16(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint32 *out = reinterpret_cast<tint32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			tint32 y = engine::sampleToInteger(x,16);
			engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(&y),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::writeInt32MSB16FromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32MSB16FromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32MSB16FromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt32LSB18(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint32 *out = reinterpret_cast<tint32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			tint32 y = engine::sampleToInteger(x,18);
			engine::writeNative32BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&y),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::writeInt32LSB18FromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32LSB18FromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32LSB18FromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt32MSB18(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint32 *out = reinterpret_cast<tint32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			tint32 y = engine::sampleToInteger(x,18);
			engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(&y),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::writeInt32MSB18FromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32MSB18FromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32MSB18FromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt32LSB20(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint32 *out = reinterpret_cast<tint32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			tint32 y = engine::sampleToInteger(x,20);
			engine::writeNative32BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&y),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::writeInt32LSB20FromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32LSB20FromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32LSB20FromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt32MSB20(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint32 *out = reinterpret_cast<tint32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			tint32 y = engine::sampleToInteger(x,20);
			engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(&y),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::writeInt32MSB20FromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32MSB20FromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32MSB20FromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt32LSB24(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint32 *out = reinterpret_cast<tint32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			tint32 y = engine::sampleToInteger(x,24);
			engine::writeNative32BitsAsLittleEndian(reinterpret_cast<const tbyte *>(&y),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
	
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::writeInt32LSB24FromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32LSB24FromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32LSB24FromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferInt32MSB24(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	tint i,j;
	tint32 *out = reinterpret_cast<tint32 *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleFloat)
	{
		for(i=0,j=oOffset;i<len;i++,j++,src+=m_noOutChannels)
		{
			sample_t x = volumeAndClip(*src);
			tint32 y = engine::sampleToInteger(x,24);
			engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(&y),reinterpret_cast<tbyte *>(&out[j]),1);
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt16)
	{
		const tint16 *in = reinterpret_cast<const tint16 *>(src);
	
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint16 x = *in;
			engine::writeInt32MSB24FromSampleInt16(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt24)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32MSB24FromSampleInt24(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleInt32)
	{
		const tint32 *in = reinterpret_cast<const tint32 *>(src);
		
		for(i=0,j=oOffset;i<len;i++,j++,in+=m_noOutChannels)
		{
			tint32 x = *in;
			engine::writeInt32MSB24FromSampleInt32(x,reinterpret_cast<tbyte *>(&out[j]));
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferDSD8LSB1(const sample_t *src, tint len, tint oOffset, tint chIndex, engine::CodecDataType type)
{
	tint i, j, k;
	tubyte *out = reinterpret_cast<tubyte *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleDSD8LSB)
	{
		for(i = 0, j = oOffset; i < len; i++, j++, src += m_noOutChannels)
		{
			tubyte *o = reinterpret_cast<tubyte *>(&out[j]);
			for(k = 0; k < sizeof(sample_t); k++)
			{
				o[k] = src[k];
			}
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleDSD8MSB)
	{
		for(i = 0, j = oOffset; i < len; i++, j++, src += m_noOutChannels)
		{
			tubyte *o = reinterpret_cast<tubyte *>(&out[j]);
			for(k = 0; k < sizeof(sample_t); k++)
			{
				o[k] = engine::lsb2msb(src[k]);
			}
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferDSD8MSB1(const sample_t *src, tint len, tint oOffset, tint chIndex, engine::CodecDataType type)
{
	tint i, j, k;
	tubyte *out = reinterpret_cast<tubyte *>(asioDataChannelIndex(chIndex));
	
	if(type == engine::e_SampleDSD8LSB)
	{
		for(i = 0, j = oOffset; i < len; i++, j++, src += m_noOutChannels)
		{
			tubyte *o = reinterpret_cast<tubyte *>(&out[j]);
			for(k = 0; k < sizeof(sample_t); k++)
			{
				o[k] = engine::lsb2msb(src[k]);
			}
		}
		return j - oOffset;
	}
	else if(type == engine::e_SampleDSD8MSB)
	{
		for(i = 0, j = oOffset; i < len; i++, j++, src += m_noOutChannels)
		{
			tubyte *o = reinterpret_cast<tubyte *>(&out[j]);
			for(k = 0; k < sizeof(sample_t); k++)
			{
				o[k] = src[k];
			}
		}
		return j - oOffset;
	}
	return -1;
}

//-------------------------------------------------------------------------------------------

tint32 *ASIOData::volumeIntBuffer()
{
	if(m_vIntBuffer == NULL || m_vIntBufSize < length())
	{
		if(m_vIntBuffer != NULL)
		{
			delete [] m_vIntBuffer;
		}
		m_vIntBuffer = new tint32 [length() * m_noOutChannels];
		m_vIntBufSize = length();
	}
	return m_vIntBuffer;
}

//-------------------------------------------------------------------------------------------

void ASIOData::volumeInt16Upscale(const tint16 *in, tint32 *out, tint noSamples) const
{
	tint64 vS = static_cast<tint64>(4294967296.0 * m_volume);

	for(tint i=0; i<noSamples; i++, in+=m_noOutChannels, out+=m_noOutChannels)
	{
		tint64 x = static_cast<tint64>(*in) * vS;
		tint32 y = static_cast<tint32>(x >> 16);
		if(x & 0x0000000000008000ULL)
		{
			y++;
		}
		*out = y;
	}
}

//-------------------------------------------------------------------------------------------

void ASIOData::volumeInt24Upscale(const tint32 *in, tint32 *out, tint noSamples) const
{
	tint64 vS = static_cast<tint64>(4294967296.0 * m_volume);

	for(tint i=0; i<noSamples; i++, in+=m_noOutChannels, out+=m_noOutChannels)
	{
		tint64 x = static_cast<tint64>(*in) * vS;
		tint32 y = static_cast<tint32>(x >> 24);
		if(x & 0x0000000000800000ULL)
		{
			y++;
		}
		*out = y;
	}
}

//-------------------------------------------------------------------------------------------

void ASIOData::volumeInt32Upscale(const tint32 *in, tint32 *out, tint noSamples) const
{
	tint64 vS = static_cast<tint64>(4294967296.0 * m_volume);

	for(tint i=0; i<noSamples; i++, in+=m_noOutChannels, out+=m_noOutChannels)
	{
		tint64 x = static_cast<tint64>(*in) * vS;
		tint32 y = static_cast<tint32>(x >> 32);
		if(x & 0x0000000080000000ULL)
		{
			y++;
		}
		*out = y;
	}
}

//-------------------------------------------------------------------------------------------

void ASIOData::volumeIntUpscale(const sample_t *in, tint32 *out, tint noSamples, engine::CodecDataType type) const
{
	if(type == engine::e_SampleInt24)
	{
        const tint32 *input = reinterpret_cast<const tint32 *>(in);
        volumeInt24Upscale(input, out, noSamples);
	}
	else if(type == engine::e_SampleInt32)
	{
        const tint32 *input = reinterpret_cast<const tint32 *>(in);
        volumeInt32Upscale(input, out, noSamples);
	}
	else
	{
        const tint16 *input = reinterpret_cast<const tint16 *>(in);
        volumeInt16Upscale(input, out, noSamples);
	}
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBuffer(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type)
{
	return copyToBufferR(src, len, oOffset, chIndex, type, false);
}

//-------------------------------------------------------------------------------------------

tint ASIOData::copyToBufferR(const sample_t *src,tint len,tint oOffset,tint chIndex,engine::CodecDataType type, bool recursive)
{
	tint amount;
	
	if(!recursive && !isEqual(m_volume, c_plusOneSample) && (type != engine::e_SampleFloat && type != engine::e_SampleDSD8LSB && type != engine::e_SampleDSD8MSB)
	{
		tint32 *vBuffer = volumeIntBuffer();
		volumeIntUpscale(src, vBuffer, len, type);
		return copyToBufferR(reinterpret_cast<const sample_t *>(vBuffer), len, oOffset, chIndex, engine::e_SampleInt32, true);
	}
	
	switch(m_sampleType)
	{
		case ASIOSTInt16LSB:
			amount = copyToBufferInt16LSB(src,len,oOffset,chIndex,type);
			break;
		
		case ASIOSTInt16MSB:
			amount = copyToBufferInt16MSB(src,len,oOffset,chIndex,type);
			break;
		
		case ASIOSTInt24LSB:
			amount = copyToBufferInt24LSB(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTInt24MSB:
			amount = copyToBufferInt24MSB(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTInt32LSB:
			amount = copyToBufferInt32LSB(src,len,oOffset,chIndex,type);
			break;

		case ASIOSTInt32MSB:
			amount = copyToBufferInt32MSB(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTFloat32LSB:
			amount = copyToBufferFloat32LSB(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTFloat32MSB:
			amount = copyToBufferFloat32MSB(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTFloat64LSB:
			amount = copyToBufferFloat64LSB(src,len,oOffset,chIndex,type);
			break;

		case ASIOSTFloat64MSB:
			amount = copyToBufferFloat64MSB(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTInt32LSB16:
			amount = copyToBufferInt32LSB16(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTInt32MSB16:
			amount = copyToBufferInt32MSB16(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTInt32LSB18:
			amount = copyToBufferInt32LSB18(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTInt32MSB18:
			amount = copyToBufferInt32MSB18(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTInt32LSB20:
			amount = copyToBufferInt32LSB20(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTInt32MSB20:
			amount = copyToBufferInt32MSB20(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTInt32LSB24:
			amount = copyToBufferInt32LSB24(src,len,oOffset,chIndex,type);
			break;
			
		case ASIOSTInt32MSB24:
			amount = copyToBufferInt32MSB24(src,len,oOffset,chIndex,type);
			break;

		case ASIOSTDSDInt8LSB1:
			amount = copyToBufferDSD8LSB1(src,len,oOffset,chIndex,type);
			break;
		
		case ASIOSTDSDInt8MSB1:
			amount = copyToBufferDSD8MSB1(src,len,oOffset,chIndex,type);
			break;

		case ASIOSTDSDInt8NER8:
			if(engine::isLittleEndian())
			{
				amount = copyToBufferDSD8LSB1(src,len,oOffset,chIndex,type);
			}
			else
			{
				amount = copyToBufferDSD8MSB1(src,len,oOffset,chIndex,type);
			}
			break;
			
		default:
			amount = -1;
			break;
	}
	return amount;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
