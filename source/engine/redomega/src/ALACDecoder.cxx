#include "engine/redomega/inc/ALACDecoder.h"
#include "engine/redomega/inc/ALACAdaptiveDecode.h"
#include "engine/redomega/inc/ALACMatrix.h"

#if defined(OMEGA_ALAC_COMPARE)
#include "engine/inc/Compare.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

ALACDecoder::ALACDecoder(ALACContainer *container) : m_container(container),
	m_channelIndex(0),
	m_activeElements(0),
	m_mixBufferU(0),
	m_mixBufferV(0),
	m_predictor(0),
	m_shiftBuffer(0)
{
#if defined(OMEGA_ALAC_COMPARE)
	m_frameCounter = 0;
	m_dynamicCounter = 0;
#endif
}

//-------------------------------------------------------------------------------------------

ALACDecoder::~ALACDecoder()
{
	if(m_mixBufferU!=0)
	{
		::free(m_mixBufferU);
		m_mixBufferU = 0;
	}
	if(m_mixBufferV!=0)
	{
		::free(m_mixBufferV);
		m_mixBufferV = 0;
	}
	if(m_predictor!=0)
	{
		::free(m_predictor);
		m_predictor = 0;
	}
}

//-------------------------------------------------------------------------------------------

bool ALACDecoder::init()
{
	tint frameLength;
	
	if(m_container==0)
	{
		return false;
	}
	frameLength = m_container->config().frameLength();
	m_mixBufferU = reinterpret_cast<tint32 *>(::calloc(frameLength * sizeof(tint32),1));
	if(m_mixBufferU==0)
	{
		return false;
	}
	m_mixBufferV = reinterpret_cast<tint32 *>(::calloc(frameLength * sizeof(tint32),1));
	if(m_mixBufferV==0)
	{
		return false;
	}
	m_predictor = reinterpret_cast<tint32 *>(::calloc(frameLength * sizeof(tint32),1));
	if(m_predictor==0)
	{
		return false;
	}
	m_shiftBuffer = reinterpret_cast<tuint16 *>(m_predictor);
	return true;
}

//-------------------------------------------------------------------------------------------

tint ALACDecoder::decode(ALACSequence *seq, sample_t *mem, tint len, CodecDataType type)
{
	tint amount,offset;
	bool loop = true;
	
	m_channelIndex = 0;
	m_activeElements = 0;
	
	offset = 0;
	while(offset>=0 && offset<len && loop)
	{
		tint tag = seq->readBitsI(3);
		
		switch(tag)
		{
			case e_idSCE:
				amount = decodeSCE(seq, mem, offset, len-offset, type);
				break;
				
			case e_idCPE:
				amount = decodeCPE(seq, mem, offset, len-offset, type);
				break;
				
			case e_idCCE:
				amount = decodeCCE(seq, mem, offset, len-offset, type);
				break;
				
			case e_idLFE:
				amount = decodeLFE(seq, mem, offset, len-offset, type);
				break;
				
			case e_idDSE:
				amount = decodeDSE(seq, mem, offset, len-offset, type);
				break;
				
			case e_idPCE:
				amount = decodePCE(seq, mem, offset, len-offset, type);
				break;
				
			case e_idFIL:
				amount = decodeFIL(seq, mem, offset, len-offset, type);
				break;
				
			case e_idEND:
			default:
				amount = decodeEND(seq, mem, offset, len-offset, type);
				seq->byteAlignment();
				loop = false;
				break;
		}
		if(amount>=0)
		{
			if(m_channelIndex>=m_container->config().numChannels())
			{
				offset += amount * m_container->config().numChannels();
				m_channelIndex = 0;
			}
		}
		else
		{
			offset = -1;
		}
	}
#if defined(OMEGA_ALAC_COMPARE)
	m_frameCounter++;
#endif
	return offset;
}

//-------------------------------------------------------------------------------------------

tint ALACDecoder::decodeSCE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type)
{
	tint i,val;
	tint elementInstanceTag;
	tint unusedHeader;
	tint headerByte;
	tint partialFrame;
	tint bytesShifted;
	tint shift;
	tint escapeFlag;
	tint chanBits;
	tint numSamples;
	tint mixBits;
	tint mixRes;
	tint modeU;
	tint denShiftU;
	tint pbFactorU;
	tint numU;
	tint extraBits;
	tint shiftBitsBk,currentBitsBk;
	tint16 coefsU[32];
	const ALACSpecificConfig& config = m_container->config();
	ALACAdaptiveDecode adaptiveDecode;
	ALACMatrix mixer;

	numSamples = len;
	
	elementInstanceTag = seq->readBitsI(4);
	m_activeElements |= 1 << elementInstanceTag;
	
	unusedHeader = seq->readBitsI(12);
	if(unusedHeader!=0)
	{
		return -1;
	}
	
	headerByte = seq->readBitsI(4);
	partialFrame = headerByte >> 3;
	bytesShifted = (headerByte >> 1) & 0x00000003;
	if(bytesShifted==3)
	{
		return -1;
	}
	
	shift = bytesShifted << 3;
	
	escapeFlag = headerByte & 0x00000001;
	
	chanBits = config.bitDepth() - shift;
	
	if(partialFrame!=0)
	{
		numSamples = seq->readBitsI(16) << 16;
		numSamples |= seq->readBitsI(16);
		if(numSamples>len)
		{
			return -1;
		}
	}
	
	if(escapeFlag==0)
	{
		mixBits = seq->readBitsI(8);
		mixRes = seq->readBitsI(8);
		
		headerByte = seq->readBitsI(8);
		modeU = headerByte >> 4;
		denShiftU = headerByte & 0x0000000f;
		
		headerByte = seq->readBitsI(8);
		pbFactorU = headerByte >> 5;
		numU = headerByte & 0x0000001f;
		
		for(i=0;i<numU;i++)
		{
			coefsU[i] = static_cast<tint16>(seq->readBitsI(16));
		}
		
		if(bytesShifted!=0)
		{
			shiftBitsBk = seq->bookmark();
			if(!seq->seek((bytesShifted << 3) * numSamples))
			{
				return -1;
			}
		}
		
		adaptiveDecode.set(*m_container,(config.pb() * pbFactorU)>>2,numSamples);
		adaptiveDecode.decode(seq,chanBits,m_predictor,numSamples);
#if defined(OMEGA_ALAC_COMPARE)
		{
			engine::Compare *comp = &g_Compare;

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega predictor0 - %d\n",frame);
				comp->compareA(m_predictor,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega predictor0 - %d\n",frame);
				comp->compareB(m_predictor,numSamples);
				frame = comp->frameB();		
			}
		}
#endif

		if(modeU==0)
		{
			dynamicPredictor(m_predictor,m_mixBufferU,numSamples,coefsU,numU,chanBits,denShiftU);
		}
		else
		{
			dynamicPredictor(m_predictor,m_predictor,numSamples,0,31,chanBits,0);
			dynamicPredictor(m_predictor,m_mixBufferU,numSamples,coefsU,numU,chanBits,denShiftU);
		}
#if defined(OMEGA_ALAC_COMPARE)
		{
			engine::Compare *comp = &g_Compare;

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega mixBufferU - %d\n",frame);
				comp->compareA(m_mixBufferU,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega mixBufferU - %d\n",frame);
				comp->compareB(m_mixBufferU,numSamples);
				frame = comp->frameB();		
			}
		}
#endif

	}
	else
	{
		shift = 32 - chanBits;
		if(chanBits<=16)
		{
			for(i=0;i<numSamples;i++)
			{
				val = seq->readBitsI(chanBits);
				val = (val << shift) >> shift;
				m_mixBufferU[i] = val;
			}
		}
		else
		{
			extraBits = chanBits - 16;
			for(i=0;i<numSamples;i++)
			{
				val = seq->readBitsI(16);
				val = (val << 16) >> shift;
				m_mixBufferU[i] = val | seq->readBitsI(extraBits);
			}
		}
		mixBits = mixRes = 0;
		bytesShifted = 0;
		
#if defined(OMEGA_ALAC_COMPARE)
		{
			engine::Compare *comp = &g_Compare;

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega mixBufferU2 - %d\n",frame);
				comp->compareA(m_mixBufferU,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega mixBufferU2 - %d\n",frame);
				comp->compareB(m_mixBufferU,numSamples);
				frame = comp->frameB();		
			}
		}
#endif
	}
	
	if(bytesShifted!=0)
	{
		shift = bytesShifted << 3;
		currentBitsBk = seq->bookmark();
		seq->move(shiftBitsBk);
		for(i=0;i<numSamples;i++)
		{
			m_shiftBuffer[i] = static_cast<tuint16>(seq->readBitsI(shift));
		}
		seq->move(currentBitsBk);
		
#if defined(OMEGA_ALAC_COMPARE)
		{
			engine::Compare *comp = &g_Compare;

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega shiftBuffer - %d\n",frame);
				comp->compareA((int *)m_shiftBuffer,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega shiftBuffer - %d\n",frame);
				comp->compareB((int *)m_shiftBuffer,numSamples);
				frame = comp->frameB();		
			}
		}
#endif
	}
	
	switch(config.bitDepth())
	{
		case 16:
			mixer.copyPredictorTo16(m_mixBufferU, mem, offset + m_channelIndex, config.numChannels(), numSamples, type);
			break;
			
		case 20:
			mixer.copyPredictorTo20(m_mixBufferU, mem, offset + m_channelIndex, config.numChannels(), numSamples, type);
			break;
			
		case 24:
			if(bytesShifted!=0)
			{
				mixer.copyPredictorTo24Shift(m_mixBufferU, m_shiftBuffer, mem, offset + m_channelIndex, config.numChannels(), numSamples, bytesShifted, type);
			}
			else
			{
				mixer.copyPredictorTo24(m_mixBufferU, mem, offset + m_channelIndex, config.numChannels(), numSamples, type);
			}
			break;
			
		case 32:
			if(bytesShifted!=0)
			{
				mixer.copyPredictorTo32Shift(m_mixBufferU, m_shiftBuffer, mem, offset + m_channelIndex, config.numChannels(), numSamples, bytesShifted, type);
			}
			else
			{
				mixer.copyPredictorTo32(m_mixBufferU, mem, offset + m_channelIndex, config.numChannels(), numSamples, type);
			}
			break;
	}
    mixer.clip(mem, offset + m_channelIndex, numSamples, config.numChannels(), type);
	
	m_channelIndex++;
	return numSamples;
}

//-------------------------------------------------------------------------------------------

tint ALACDecoder::decodeCPE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type)
{
	tint i,val;
	tint elementInstanceTag;
	tint unusedHeader;
	tint headerByte;
	tint partialFrame;
	tint bytesShifted;
	tint shift;
	tint escapeFlag;
	tint chanBits;
	tint numSamples;
	tint mixBits;
	tint mixRes;
	tint extraBits;
	tint shiftBitsBk,currentBitsBk;
	tint denShiftU,denShiftV;
	tint pbFactorU,pbFactorV;
	tint numU,numV;
	tint modeU,modeV;
	tint16 coefsU[32],coefsV[32];
	const ALACSpecificConfig& config = m_container->config();
	ALACAdaptiveDecode adaptiveDecode;
	ALACMatrix mixer;

	numSamples = len;

	if((m_channelIndex+2)>config.numChannels())
	{
		return 0;
	}
	
	elementInstanceTag = seq->readBitsI(4);
	m_activeElements |= 1 << elementInstanceTag;
	
	unusedHeader = seq->readBitsI(12);
	if(unusedHeader!=0)
	{
		return -1;
	}
	
	headerByte = seq->readBitsI(4);
	partialFrame = headerByte >> 3;
	bytesShifted = (headerByte >> 1) & 0x00000003;
	if(bytesShifted==3)
	{
		return -1;
	}
	
	shift = bytesShifted << 3;
	
	escapeFlag = headerByte & 0x00000001;
	
	chanBits = config.bitDepth() - shift + 1;
	
	if(partialFrame!=0)
	{
		numSamples = seq->readBitsI(16) << 16;
		numSamples |= seq->readBitsI(16);
		if(numSamples>len)
		{
			return -1;
		}
	}
	
	if(escapeFlag==0)
	{
		mixBits = seq->readBitsI(8);
		mixRes = seq->readBitsI(8);
		
		headerByte = seq->readBitsI(8);
		modeU = headerByte >> 4;
		denShiftU = headerByte & 0x0000000f;
		
		headerByte = seq->readBitsI(8);
		pbFactorU = headerByte >> 5;
		numU = headerByte & 0x0000001f;
		for(i=0;i<numU;i++)
		{
			coefsU[i] = static_cast<tint16>(seq->readBitsI(16));
		}
		
		headerByte = seq->readBitsI(8);
		modeV = headerByte >> 4;
		denShiftV = headerByte & 0x0000000f;
		
		headerByte = seq->readBitsI(8);
		pbFactorV = headerByte >> 5;
		numV = headerByte & 0x0000001f;
		for(i=0;i<numV;i++)
		{
			coefsV[i] = static_cast<tint16>(seq->readBitsI(16));
		}
		
		if(bytesShifted!=0)
		{
			shiftBitsBk = seq->bookmark();
			if(!seq->seek((bytesShifted << 3) * 2 * numSamples))
			{
				return -1;
			}
		}
		
		adaptiveDecode.set(*m_container,(config.pb() * pbFactorU)>>2,numSamples);
		adaptiveDecode.decode(seq,chanBits,m_predictor,numSamples);
#if defined(OMEGA_ALAC_COMPARE)
		{
			engine::Compare *comp = &g_Compare;

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega predictor0 - %d\n",frame);
				comp->compareA(m_predictor,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega predictor0 - %d\n",frame);
				comp->compareB(m_predictor,numSamples);
				frame = comp->frameB();		
			}
		}
#endif

		if(modeU==0)
		{
			dynamicPredictor(m_predictor,m_mixBufferU,numSamples,coefsU,numU,chanBits,denShiftU);
		}
		else
		{
			dynamicPredictor(m_predictor,m_predictor,numSamples,0,31,chanBits,0);
			dynamicPredictor(m_predictor,m_mixBufferU,numSamples,coefsU,numU,chanBits,denShiftU);
		}

#if defined(OMEGA_ALAC_COMPARE)
		{
			engine::Compare *comp = &g_Compare;

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega mixBufferU - %d\n",frame);
				comp->compareA(m_mixBufferU,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega mixBufferU - %d\n",frame);
				comp->compareB(m_mixBufferU,numSamples);
				frame = comp->frameB();		
			}
		}
#endif
		
		adaptiveDecode.set(*m_container,(config.pb() * pbFactorV)>>2,numSamples);
		adaptiveDecode.decode(seq,chanBits,m_predictor,numSamples);
#if defined(OMEGA_ALAC_COMPARE)
		{
			engine::Compare *comp = &g_Compare;

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega predictor1 - %d\n",frame);
				comp->compareA(m_predictor,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega predictor1 - %d\n",frame);
				comp->compareB(m_predictor,numSamples);
				frame = comp->frameB();		
			}
		}
#endif
		if(modeV==0)
		{
			dynamicPredictor(m_predictor,m_mixBufferV,numSamples,coefsV,numV,chanBits,denShiftV);
		}
		else
		{
			dynamicPredictor(m_predictor,m_predictor,numSamples,0,31,chanBits,0);
			dynamicPredictor(m_predictor,m_mixBufferV,numSamples,coefsV,numV,chanBits,denShiftV);
		}
		
#if defined(OMEGA_ALAC_COMPARE)
		{
			engine::Compare *comp = &g_Compare;

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega mixBufferV - %d\n",frame);
				comp->compareA(m_mixBufferV,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega mixBufferV - %d\n",frame);
				comp->compareB(m_mixBufferV,numSamples);
				frame = comp->frameB();		
			}
		}
#endif

	}
	else
	{
		chanBits = config.bitDepth();
		shift = 32 - chanBits;
		if(chanBits<=16)
		{
			for(i=0;i<numSamples;i++)
			{
				val = seq->readBitsI(chanBits);
				val = (val << shift) >> shift;
				m_mixBufferU[i] = val;
				
				val = seq->readBitsI(chanBits);
				val = (val << shift) >> shift;
				m_mixBufferV[i] = val;
			}
		}
		else
		{
			extraBits = chanBits - 16;
			for(i=0;i<numSamples;i++)
			{
				val = seq->readBitsI(16);
				val = (val << 16) >> shift;
				m_mixBufferU[i] = val | seq->readBitsI(extraBits);
				
				val = seq->readBitsI(16);
				val = (val << 16) >> shift;
				m_mixBufferV[i] = val | seq->readBitsI(extraBits);
			}
		}
		mixBits = mixRes = bytesShifted = 0;
		
#if defined(OMEGA_ALAC_COMPARE)
		{
			engine::Compare *comp = &g_Compare;

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega mixBufferU2 - %d\n",frame);
				comp->compareA(m_mixBufferU,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega mixBufferU2 - %d\n",frame);
				comp->compareB(m_mixBufferU,numSamples);
				frame = comp->frameB();		
			}

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega mixBufferV2 - %d\n",frame);
				comp->compareA(m_mixBufferV,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega mixBufferV2 - %d\n",frame);
				comp->compareB(m_mixBufferV,numSamples);
				frame = comp->frameB();		
			}
		}
#endif
	}
	
	if(bytesShifted!=0)
	{
		shift = bytesShifted << 3;
		currentBitsBk = seq->bookmark();
		seq->move(shiftBitsBk);
		for(i=0;i<(numSamples<<1);i+=2)
		{
			m_shiftBuffer[i+0] = static_cast<tuint16>(seq->readBitsI(shift));
			m_shiftBuffer[i+1] = static_cast<tuint16>(seq->readBitsI(shift));
		}
		seq->move(currentBitsBk);
		
#if defined(OMEGA_ALAC_COMPARE)
		{
			engine::Compare *comp = &g_Compare;

			if(comp->isThreadA())
			{
				tint frame = comp->frameA();
				common::Log::g_Log.print("redomega shiftBuffer - %d\n",frame);
				comp->compareA((int *)m_shiftBuffer,numSamples);
				frame = comp->frameA();
			}
			else
			{
				tint frame = comp->frameB();
				common::Log::g_Log.print("redomega shiftBuffer - %d\n",frame);
				comp->compareB((int *)m_shiftBuffer,numSamples);
				frame = comp->frameB();		
			}
		}
#endif
	}
	
	switch(config.bitDepth())
	{
		case 16:
			mixer.unMix16(m_mixBufferU, m_mixBufferV, mem, offset + m_channelIndex, config.numChannels(), numSamples, mixBits, mixRes, type);
			break;
			
		case 20:
			mixer.unMix20(m_mixBufferU, m_mixBufferV, mem, offset + m_channelIndex, config.numChannels(), numSamples, mixBits, mixRes, type);
			break;
			
		case 24:
			mixer.unMix24(m_mixBufferU, m_mixBufferV, mem, offset + m_channelIndex, config.numChannels(), numSamples, mixBits, mixRes, m_shiftBuffer, bytesShifted, type);
			break;
			
		case 32:
			mixer.unMix32(m_mixBufferU, m_mixBufferV, mem, offset + m_channelIndex, config.numChannels(), numSamples, mixBits, mixRes, m_shiftBuffer, bytesShifted, type);
			break;
	}
    mixer.clipLR(mem, offset + m_channelIndex, numSamples, config.numChannels(), type);
	m_channelIndex += 2;
	return numSamples;
}

//-------------------------------------------------------------------------------------------

tint ALACDecoder::decodeCCE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type)
{
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ALACDecoder::decodeLFE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type)
{
	return decodeSCE(seq, mem, offset, len, type);
}

//-------------------------------------------------------------------------------------------

tint ALACDecoder::decodeDSE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type)
{
	tint elementInstanceTag;
	tint dataByteAlignFlag;
	tint count;
	
	elementInstanceTag = seq->readBitsI(4);
	dataByteAlignFlag = seq->readBitI();
	count = seq->readBitsI(8);
	if(count==255)
	{
		count += seq->readBitsI(8);
	}
	if(dataByteAlignFlag)
	{
		seq->byteAlignment();
	}
	if(!seq->seek(count << 3))
	{
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------

tint ALACDecoder::decodePCE(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type)
{
	return -1;
}

//-------------------------------------------------------------------------------------------

tint ALACDecoder::decodeFIL(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type)
{
	tint count;
	
	count = seq->readBitsI(4);
	if(count==15)
	{
		count = seq->readBitsI(8) - 1;
	}
	if(seq->seek(count << 3))
	{
		return -1;
	}
	return 0;
}

//-------------------------------------------------------------------------------------------

tint ALACDecoder::decodeEND(ALACSequence *seq, sample_t *mem, tint offset, tint len, CodecDataType type)
{
	return 0;
}

//-------------------------------------------------------------------------------------------

void ALACDecoder::dynamicPredictor(tint *pc,tint *out,tint num,tint16 *coefs,tint numActive,tuint chanBits,tuint denShift)
{
	tint j,k,lim;
	tint sum1,sg,sgn,top,dd;
	tint *pout;
	tint del,del0;
	tuint chanShift = 32 - chanBits;
	tint denHalf = 1 << (denShift - 1);
	
	out[0] = pc[0];
	if(numActive==0)
	{
		if(num>1 && pc!=out)
		{
			::memcpy(&out[1],&pc[1],(num-1)*sizeof(tint));
		}
	}
	else if(numActive==31)
	{
		tint prev = out[0];
		for(j=1;j<num;j++)
		{
			del = pc[j] + prev;
			prev = (del << chanShift) >> chanShift;
			out[j] = prev;
		}
	}
	else
	{
		for(j=1;j<=numActive;j++)
		{
			del = pc[j] + out[j-1];
			out[j] = (del << chanShift) >> chanShift;
		}
		lim = numActive + 1;
		
		if(numActive==4)
		{
			tint16 a0,a1,a2,a3;
			tint32 b0,b1,b2,b3;
			
			a0 = coefs[0];
			a1 = coefs[1];
			a2 = coefs[2];
			a3 = coefs[3];
			
			for(j=lim;j<num;j++)
			{
				top = out[j - lim];
				pout = out + j - 1;
				
				b0 = top - pout[0];
				b1 = top - pout[-1];
				b2 = top - pout[-2];
				b3 = top - pout[-3];
				
				sum1 = (denHalf - (a0 * b0) - (a1 * b1) - (a2 * b2) - (a3 * b3)) >> denShift;
				
				del = pc[j];
				del0 = del;
				sg = signOfInt(del);
				del += top + sum1;
				
				out[j] = (del << chanShift) >> chanShift;
				if(sg>0)
				{
					sgn = signOfInt(b3);
					a3 -= sgn;
					del0 -= (4 - 3) * ((sgn * b3) >> denShift);
					if(del0<=0)
					{
						continue;
					}
					
					sgn = signOfInt(b2);
					a2 -= sgn;
					del0 -= (4 - 2) * ((sgn * b2) >> denShift);
					if(del0<=0)
					{
						continue;
					}
					
					sgn = signOfInt(b1);
					a1 -= sgn;
					del0 -= (4 - 1) * ((sgn * b1) >> denShift);
					if(del0<=0)
					{
						continue;
					}
					
					a0 -= signOfInt(b0);
				}
				else if(sg<0)
				{
					sgn = -signOfInt(b3);
					a3 -= sgn;
					del0 -= (4 - 3) * ((sgn * b3) >> denShift);
					if(del0>=0)
					{
						continue;
					}
					
					sgn = -signOfInt(b2);
					a2 -= sgn;
					del0 -= (4 - 2) * ((sgn * b2) >> denShift);
					if(del0>=0)
					{
						continue;
					}
					
					sgn = -signOfInt(b1);
					a1 -= sgn;
					del0 -= (4 - 1) * ((sgn * b1) >> denShift);
					if(del0>=0)
					{
						continue;
					}
					
					a0 += signOfInt(b0);
				}
			}
			
			coefs[0] = a0;
			coefs[1] = a1;
			coefs[2] = a2;
			coefs[3] = a3;
		}
		else if(numActive==8)
		{
			tint16 a0,a1,a2,a3,a4,a5,a6,a7;
			tint32 b0,b1,b2,b3,b4,b5,b6,b7;
			
			a0 = coefs[0];
			a1 = coefs[1];
			a2 = coefs[2];
			a3 = coefs[3];
			a4 = coefs[4];
			a5 = coefs[5];
			a6 = coefs[6];
			a7 = coefs[7];
			
			for(j=lim;j<num;j++)
			{
				top = out[j - lim];
				pout = out + j - 1;
				
				b0 = top - pout[0];
				b1 = top - pout[-1];
				b2 = top - pout[-2];
				b3 = top - pout[-3];
				b4 = top - pout[-4];
				b5 = top - pout[-5];
				b6 = top - pout[-6];
				b7 = top - pout[-7];
				
				sum1 = (denHalf - (a0 * b0) - (a1 * b1) - (a2 * b2) - (a3 * b3) - (a4 * b4) - (a5 * b5) - (a6 * b6) - (a7 * b7)) >> denShift;
				
				del = pc[j];
				del0 = del;
				sg = signOfInt(del);
				del += top + sum1;
				
				out[j] = (del << chanShift) >> chanShift;
				
				if(sg>0)
				{
					sgn = signOfInt(b7);
					a7 -= sgn;
					del0 -= ((sgn * b7) >> denShift);
					if(del0<=0)
					{
						continue;
					}
					
					sgn = signOfInt(b6);
					a6 -= sgn;
					del0 -= ((sgn * b6) >> denShift) << 1;
					if(del0<=0)
					{
						continue;
					}
					
					sgn = signOfInt(b5);
					a5 -= sgn;
					del0 -= 3 * ((sgn * b5) >> denShift);
					if(del0<=0)
					{
						continue;
					}
					
					sgn = signOfInt(b4);
					a4 -= sgn;
					del0 -= ((sgn * b4) >> denShift) << 2;
					if(del0<=0)
					{
						continue;
					}
					
					sgn = signOfInt(b3);
					a3 -= sgn;
					del0 -= 5 * ((sgn * b3) >> denShift);
					if(del0<=0)
					{
						continue;
					}
					
					sgn = signOfInt(b2);
					a2 -= sgn;
					del0 -= 6 * ((sgn * b2) >> denShift);
					if(del0<=0)
					{
						continue;
					}
					
					sgn = signOfInt(b1);
					a1 -= sgn;
					del0 -= 7 * ((sgn * b1) >> denShift);
					if(del0<=0)
					{
						continue;
					}
					
					a0 -= signOfInt(b0);
				}
				else if(sg<0)
				{
					sgn = -signOfInt(b7);
					a7 -= sgn;
					del0 -= ((sgn * b7) >> denShift);
					if(del0>=0)
					{
						continue;
					}
					
					sgn = -signOfInt(b6);
					a6 -= sgn;
					del0 -= ((sgn * b6) >> denShift) << 1;
					if(del0>=0)
					{
						continue;
					}
					
					sgn = -signOfInt(b5);
					a5 -= sgn;
					del0 -= 3 * ((sgn * b5) >> denShift);
					if(del0>=0)
					{
						continue;
					}
					
					sgn = -signOfInt(b4);
					a4 -= sgn;
					del0 -= ((sgn * b4) >> denShift) << 2;
					if(del0>=0)
					{
						continue;
					}
					
					sgn = -signOfInt(b3);
					a3 -= sgn;
					del0 -= 5 * ((sgn * b3) >> denShift);
					if(del0>=0)
					{
						continue;
					}
					
					sgn = -signOfInt(b2);
					a2 -= sgn;
					del0 -= 6 * ((sgn * b2) >> denShift);
					if(del0>=0)
					{
						continue;
					}
					
					sgn = -signOfInt(b1);
					a1 -= sgn;
					del0 -= 7 * ((sgn * b1) >> denShift);
					if(del0>=0)
					{
						continue;
					}
					
					a0 += signOfInt(b0);				
				}
			}
			
			coefs[0] = a0;
			coefs[1] = a1;
			coefs[2] = a2;
			coefs[3] = a3;
			coefs[4] = a4;
			coefs[5] = a5;
			coefs[6] = a6;
			coefs[7] = a7;
		}
		else
		{
			for(j=lim;j<num;j++)
			{
				sum1 = 0;
				pout = out + j - 1;
				top = out[j-lim];
				
				for(k=0;k<numActive;k++)
				{
					sum1 += coefs[k] * (pout[-k] - top);
				}
				
				del = pc[j];
				del0 = del;
				sg = signOfInt(del);
				del += top + ((sum1 + denHalf) >> denShift);
				out[j] = (del << chanShift) >> chanShift;
				
				if(sg>0)
				{
					for(k=(numActive-1);k>=0;k--)
					{
						dd = top - pout[-k];
						sgn = signOfInt(dd);
						coefs[k] -= sgn;
						del0 -= (numActive - k) * ((sgn * dd) >> denShift);
						if(del0<=0)
						{
							break;
						}
					}
				}
				else if(sg<0)
				{
					for(k=(numActive-1);k>=0;k--)
					{
						dd = top - pout[-k];
						sgn = signOfInt(dd);
						coefs[k] += sgn;
						del0 -= (numActive - k) * ((sgn * dd) >> denShift);
						if(del0>=0)
						{
							break;
						}
					}
				}
			}
		}
	}
#if defined(OMEGA_ALAC_COMPARE)
	m_dynamicCounter++;
#endif
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
