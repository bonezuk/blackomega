#include "engine/redomega/inc/ALACMatrix.h"
#include "engine/inc/FormatTypeFromFloat.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

ALACMatrix::ALACMatrix()
{}

//-------------------------------------------------------------------------------------------

ALACMatrix::~ALACMatrix()
{}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix16(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		unMix16Int16(u, v, out, outIdx, stride, numSamples, mixBits, mixRes);
	}
	else if(type & e_SampleInt24)
	{
		unMix16Int24(u, v, out, outIdx, stride, numSamples, mixBits, mixRes);
	}
	else if(type & e_SampleInt32)
	{
		unMix16Int32(u, v, out, outIdx, stride, numSamples, mixBits, mixRes);
	}
	else
	{
		unMix16Float(u, v, out, outIdx, stride, numSamples, mixBits, mixRes);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix16Int16(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes)
{
	tint32 j;
	tint16 *outInt16 = reinterpret_cast<tint16 *>(out);
	tint16 *op = &outInt16[outIdx];
	
	if(mixRes!=0)
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 lCh,rCh;
			lCh = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
			rCh = lCh - v[j];
			op[0] = sampleInt16From16Bit(lCh);
			op[1] = sampleInt16From16Bit(rCh);
			op += stride;
		}
	}
	else
	{
		for(j=0;j<numSamples;j++)
		{
			op[0] = sampleInt16From16Bit(u[j]);
			op[1] = sampleInt16From16Bit(v[j]);
			op += stride;
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix16Int24(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes)
{
	tint32 j;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	
	if(mixRes!=0)
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 lCh,rCh;
			lCh = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
			rCh = lCh - v[j];
			op[0] = sampleInt24From16Bit(lCh);
			op[1] = sampleInt24From16Bit(rCh);
			op += stride;
		}
	}
	else
	{
		for(j=0;j<numSamples;j++)
		{
			op[0] = sampleInt24From16Bit(u[j]);
			op[1] = sampleInt24From16Bit(v[j]);
			op += stride;
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix16Int32(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes)
{
	tint32 j;
	tint32 *outInt32 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt32[outIdx];
	
	if(mixRes!=0)
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 lCh,rCh;
			lCh = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
			rCh = lCh - v[j];
			op[0] = sampleInt32From16Bit(lCh);
			op[1] = sampleInt32From16Bit(rCh);
			op += stride;
		}
	}
	else
	{
		for(j=0;j<numSamples;j++)
		{
			op[0] = sampleInt32From16Bit(u[j]);
			op[1] = sampleInt32From16Bit(v[j]);
			op += stride;
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix16Float(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes)
{
	tint32 j;
	sample_t *op = &out[outIdx];
	
	if(mixRes!=0)
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 lCh,rCh;
			lCh = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
			rCh = lCh - v[j];
			op[0] = sampleFrom16Bit(lCh);
			op[1] = sampleFrom16Bit(rCh);
			op += stride;
		}
	}
	else
	{
		for(j=0;j<numSamples;j++)
		{
			op[0] = sampleFrom16Bit(u[j]);
			op[1] = sampleFrom16Bit(v[j]);
			op += stride;
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix20(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		unMix20Int16(u, v, out, outIdx, stride, numSamples, mixBits, mixRes);
	}
	else if(type & e_SampleInt24)
	{
		unMix20Int24(u, v, out, outIdx, stride, numSamples, mixBits, mixRes);
	}
	else if(type & e_SampleInt32)
	{
		unMix20Int32(u, v, out, outIdx, stride, numSamples, mixBits, mixRes);
	}
	else
	{
		unMix20Float(u, v, out, outIdx, stride, numSamples, mixBits, mixRes);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix20Int16(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes)
{
	tint j;
	tint16 *outInt16 = reinterpret_cast<tint16 *>(out);
	tint16 *op = &outInt16[outIdx];
	
	if(mixRes!=0)
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 l,r;
			
			l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
			r = l - v[j];
			l <<= 4;
			r <<= 4;
			op[0] = sampleInt16From24Bit(l);
			op[1] = sampleInt16From24Bit(r);
			op += stride;
		}
	}
	else
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 val;
			
			val = u[j] << 4;
			op[0] = sampleInt16From24Bit(val);
			val = v[j] << 4;
			op[1] = sampleInt16From24Bit(val);
			op += stride;
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix20Int24(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes)
{
	tint j;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	
	if(mixRes!=0)
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 l,r;
			
			l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
			r = l - v[j];
			l <<= 4;
			r <<= 4;
			op[0] = sampleInt24From24Bit(l);
			op[1] = sampleInt24From24Bit(r);
			op += stride;
		}
	}
	else
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 val;
			
			val = u[j] << 4;
			op[0] = sampleInt24From24Bit(val);
			val = v[j] << 4;
			op[1] = sampleInt24From24Bit(val);
			op += stride;
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix20Int32(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes)
{
	tint j;
	tint32 *outInt32 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt32[outIdx];
	
	if(mixRes!=0)
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 l,r;
			
			l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
			r = l - v[j];
			l <<= 4;
			r <<= 4;
			op[0] = sampleInt32From24Bit(l);
			op[1] = sampleInt32From24Bit(r);
			op += stride;
		}
	}
	else
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 val;
			
			val = u[j] << 4;
			op[0] = sampleInt32From24Bit(val);
			val = v[j] << 4;
			op[1] = sampleInt32From24Bit(val);
			op += stride;
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix20Float(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes)
{
	tint j;
	sample_t *op = &out[outIdx];
	
	if(mixRes!=0)
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 l,r;
			
			l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
			r = l - v[j];
			l <<= 4;
			r <<= 4;
			op[0] = sampleFrom24Bit(l);
			op[1] = sampleFrom24Bit(r);
			op += stride;
		}
	}
	else
	{
		for(j=0;j<numSamples;j++)
		{
			tint32 val;
			
			val = u[j] << 4;
			op[0] = sampleFrom24Bit(val);
			val = v[j] << 4;
			op[1] = sampleFrom24Bit(val);
			op += stride;
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix24(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, 
	tuint16 *shiftUV, tint32 bytesShifted, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		unMix24Int16(u, v, out, outIdx, stride, numSamples, mixBits, mixRes, shiftUV, bytesShifted);
	}
	else if(type & e_SampleInt24)
	{
		unMix24Int24(u, v, out, outIdx, stride, numSamples, mixBits, mixRes, shiftUV, bytesShifted);
	}
	else if(type & e_SampleInt32)
	{
		unMix24Int32(u, v, out, outIdx, stride, numSamples, mixBits, mixRes, shiftUV, bytesShifted);
	}
	else
	{
		unMix24Float(u, v, out, outIdx, stride, numSamples, mixBits, mixRes, shiftUV, bytesShifted);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix24Int16(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes,
	tuint16 *shiftUV, tint32 bytesShifted)
{
	tint32 l,r,j,k;
	tint32 shift = bytesShifted << 3;
	tint16 *outInt16 = reinterpret_cast<tint16 *>(out);
	tint16 *op = &outInt16[outIdx];
	
	if(mixRes!=0)
	{
		if(bytesShifted!=0)
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
				r = l - v[j];
				l = (l << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (r << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleInt16From24Bit(l);
				op[1] = sampleInt16From24Bit(r);
				op += stride;
			}
		}
		else
		{
			for(j=0;j<numSamples;j++)
			{
				l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
				r = l - v[j];
				op[0] = sampleInt16From24Bit(l);
				op[1] = sampleInt16From24Bit(r);
				op += stride;
			}
		}
	}
	else
	{
		if(bytesShifted!=0)
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = (u[j] << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (v[j] << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleInt16From24Bit(l);
				op[1] = sampleInt16From24Bit(r);
				op += stride;
			}
		}
		else
		{
			for(j=0;j<numSamples;j++)
			{
				l = u[j];
				r = v[j];
				op[0] = sampleInt16From24Bit(l);
				op[1] = sampleInt16From24Bit(r);
				op += stride;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix24Int24(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes,
	tuint16 *shiftUV, tint32 bytesShifted)
{
	tint32 l,r,j,k;
	tint32 shift = bytesShifted << 3;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	
	if(mixRes!=0)
	{
		if(bytesShifted!=0)
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
				r = l - v[j];
				l = (l << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (r << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleInt24From24Bit(l);
				op[1] = sampleInt24From24Bit(r);
				op += stride;
			}
		}
		else
		{
			for(j=0;j<numSamples;j++)
			{
				l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
				r = l - v[j];
				op[0] = sampleInt24From24Bit(l);
				op[1] = sampleInt24From24Bit(r);
				op += stride;
			}
		}
	}
	else
	{
		if(bytesShifted!=0)
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = (u[j] << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (v[j] << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleInt24From24Bit(l);
				op[1] = sampleInt24From24Bit(r);
				op += stride;
			}
		}
		else
		{
			for(j=0;j<numSamples;j++)
			{
				l = u[j];
				r = v[j];
				op[0] = sampleInt24From24Bit(l);
				op[1] = sampleInt24From24Bit(r);
				op += stride;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix24Int32(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes,
	tuint16 *shiftUV, tint32 bytesShifted)
{
	tint32 l,r,j,k;
	tint32 shift = bytesShifted << 3;
	tint32 *outInt32 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt32[outIdx];
	
	if(mixRes!=0)
	{
		if(bytesShifted!=0)
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
				r = l - v[j];
				l = (l << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (r << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleInt32From24Bit(l);
				op[1] = sampleInt32From24Bit(r);
				op += stride;
			}
		}
		else
		{
			for(j=0;j<numSamples;j++)
			{
				l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
				r = l - v[j];
				op[0] = sampleInt32From24Bit(l);
				op[1] = sampleInt32From24Bit(r);
				op += stride;
			}
		}
	}
	else
	{
		if(bytesShifted!=0)
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = (u[j] << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (v[j] << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleInt32From24Bit(l);
				op[1] = sampleInt32From24Bit(r);
				op += stride;
			}
		}
		else
		{
			for(j=0;j<numSamples;j++)
			{
				l = u[j];
				r = v[j];
				op[0] = sampleInt32From24Bit(l);
				op[1] = sampleInt32From24Bit(r);
				op += stride;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix24Float(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes,
	tuint16 *shiftUV, tint32 bytesShifted)
{
	tint32 l,r,j,k;
	tint32 shift = bytesShifted << 3;
	sample_t *op = &out[outIdx];
	
	if(mixRes!=0)
	{
		if(bytesShifted!=0)
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
				r = l - v[j];
				l = (l << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (r << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleFrom24Bit(l);
				op[1] = sampleFrom24Bit(r);
				op += stride;
			}
		}
		else
		{
			for(j=0;j<numSamples;j++)
			{
				l = u[j] + v[j] - ((mixRes * v[j]) >> mixBits);
				r = l - v[j];
				op[0] = sampleFrom24Bit(l);
				op[1] = sampleFrom24Bit(r);
				op += stride;
			}
		}
	}
	else
	{
		if(bytesShifted!=0)
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = (u[j] << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (v[j] << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleFrom24Bit(l);
				op[1] = sampleFrom24Bit(r);
				op += stride;
			}
		}
		else
		{
			for(j=0;j<numSamples;j++)
			{
				l = u[j];
				r = v[j];
				op[0] = sampleFrom24Bit(l);
				op[1] = sampleFrom24Bit(r);
				op += stride;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix32(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, tuint16 *shiftUV, tint32 bytesShifted, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		unMix32Int16(u, v, out, outIdx, stride, numSamples, mixBits, mixRes, shiftUV, bytesShifted);
	}
	else if(type & e_SampleInt24)
	{
		unMix32Int24(u, v, out, outIdx, stride, numSamples, mixBits, mixRes, shiftUV, bytesShifted);
	}
	else if(type & e_SampleInt32)
	{
		unMix32Int32(u, v, out, outIdx, stride, numSamples, mixBits, mixRes, shiftUV, bytesShifted);
	}
	else
	{
		unMix32Float(u, v, out, outIdx, stride, numSamples, mixBits, mixRes, shiftUV, bytesShifted);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix32Int16(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, tuint16 *shiftUV, tint32 bytesShifted)
{
	tint32 j,k,l,r;
	tint16 *outInt16 = reinterpret_cast<tint16 *>(out);
	tint16 *op = &outInt16[outIdx];
	tint32 shift = bytesShifted << 3;
	
	if(mixRes!=0)
	{
		for(j=0,k=0;j<numSamples;j++,k+=2)
		{
			tint32 lt = u[j];
			tint32 rt = v[j];
			l = lt + rt - ((mixRes * rt) >> mixBits);
			r = l - rt;
			lt = (l << shift) | static_cast<tuint32>(shiftUV[k]);
			rt = (r << shift) | static_cast<tuint32>(shiftUV[k+1]);
			op[0] = sampleInt16From32Bit(lt);
			op[1] = sampleInt16From32Bit(rt);
			op += stride;
		}
	}
	else
	{
		if(bytesShifted==0)
		{
			for(j=0;j<numSamples;j++)
			{
				op[0] = sampleInt16From32Bit(u[j]);
				op[1] = sampleInt16From32Bit(v[j]);
				op += stride;
			}
		}
		else
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = (u[j] << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (v[j] << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleInt16From32Bit(l);
				op[1] = sampleInt16From32Bit(r);
				op += stride;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix32Int24(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, tuint16 *shiftUV, tint32 bytesShifted)
{
	tint32 j,k,l,r;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	tint32 shift = bytesShifted << 3;
	
	if(mixRes!=0)
	{
		for(j=0,k=0;j<numSamples;j++,k+=2)
		{
			tint32 lt = u[j];
			tint32 rt = v[j];
			l = lt + rt - ((mixRes * rt) >> mixBits);
			r = l - rt;
			lt = (l << shift) | static_cast<tuint32>(shiftUV[k]);
			rt = (r << shift) | static_cast<tuint32>(shiftUV[k+1]);
			op[0] = sampleInt24From32Bit(lt);
			op[1] = sampleInt24From32Bit(rt);
			op += stride;
		}
	}
	else
	{
		if(bytesShifted==0)
		{
			for(j=0;j<numSamples;j++)
			{
				op[0] = sampleInt24From32Bit(u[j]);
				op[1] = sampleInt24From32Bit(v[j]);
				op += stride;
			}
		}
		else
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = (u[j] << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (v[j] << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleInt24From32Bit(l);
				op[1] = sampleInt24From32Bit(r);
				op += stride;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix32Int32(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, tuint16 *shiftUV, tint32 bytesShifted)
{
	tint32 j,k,l,r;
	tint32 *outInt32 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt32[outIdx];
	tint32 shift = bytesShifted << 3;
	
	if(mixRes!=0)
	{
		for(j=0,k=0;j<numSamples;j++,k+=2)
		{
			tint32 lt = u[j];
			tint32 rt = v[j];
			l = lt + rt - ((mixRes * rt) >> mixBits);
			r = l - rt;
			lt = (l << shift) | static_cast<tuint32>(shiftUV[k]);
			rt = (r << shift) | static_cast<tuint32>(shiftUV[k+1]);
			op[0] = sampleInt32From32Bit(lt);
			op[1] = sampleInt32From32Bit(rt);
			op += stride;
		}
	}
	else
	{
		if(bytesShifted==0)
		{
			for(j=0;j<numSamples;j++)
			{
				op[0] = sampleInt32From32Bit(u[j]);
				op[1] = sampleInt32From32Bit(v[j]);
				op += stride;
			}
		}
		else
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = (u[j] << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (v[j] << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleInt32From32Bit(l);
				op[1] = sampleInt32From32Bit(r);
				op += stride;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::unMix32Float(tint32 *u, tint32 *v, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 mixBits, tint32 mixRes, tuint16 *shiftUV, tint32 bytesShifted)
{
	tint32 j,k,l,r;
	sample_t *op = &out[outIdx];
	tint32 shift = bytesShifted << 3;
	
	if(mixRes!=0)
	{
		for(j=0,k=0;j<numSamples;j++,k+=2)
		{
			tint32 lt = u[j];
			tint32 rt = v[j];
			l = lt + rt - ((mixRes * rt) >> mixBits);
			r = l - rt;
			lt = (l << shift) | static_cast<tuint32>(shiftUV[k]);
			rt = (r << shift) | static_cast<tuint32>(shiftUV[k+1]);
			op[0] = sampleFrom32Bit(lt);
			op[1] = sampleFrom32Bit(rt);
			op += stride;
		}
	}
	else
	{
		if(bytesShifted==0)
		{
			for(j=0;j<numSamples;j++)
			{
				op[0] = sampleFrom32Bit(u[j]);
				op[1] = sampleFrom32Bit(v[j]);
				op += stride;
			}
		}
		else
		{
			for(j=0,k=0;j<numSamples;j++,k+=2)
			{
				l = (u[j] << shift) | static_cast<tuint32>(shiftUV[k]);
				r = (v[j] << shift) | static_cast<tuint32>(shiftUV[k+1]);
				op[0] = sampleFrom32Bit(l);
				op[1] = sampleFrom32Bit(r);
				op += stride;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo16(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		copyPredictorTo16Int16(in, out, outIdx, stride, numSamples);
	}
	else if(type & e_SampleInt24)
	{
		copyPredictorTo16Int24(in, out, outIdx, stride, numSamples);
	}
	else if(type & e_SampleInt32)
	{
		copyPredictorTo16Int32(in, out, outIdx, stride, numSamples);
	}
	else
	{
		copyPredictorTo16Float(in, out, outIdx, stride, numSamples);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo16Int16(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples)
{
	tint32 j;
	tint16 *outInt16 = reinterpret_cast<tint16 *>(out);
	tint16 *op = &outInt16[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		*op = sampleInt16From16Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo16Int24(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples)
{
	tint32 j;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		*op = sampleInt24From16Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo16Int32(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples)
{
	tint32 j;
	tint32 *outInt32 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt32[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		*op = sampleInt32From16Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo16Float(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples)
{
	tint32 j;
	sample_t *op = &out[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		*op = sampleFrom16Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo24(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		copyPredictorTo24Int16(in, out, outIdx, stride, numSamples);
	}
	else if(type & e_SampleInt24)
	{
		copyPredictorTo24Int24(in, out, outIdx, stride, numSamples);
	}
	else if(type & e_SampleInt32)
	{
		copyPredictorTo24Int32(in, out, outIdx, stride, numSamples);
	}
	else
	{
		copyPredictorTo24Float(in, out, outIdx, stride, numSamples);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo24Int16(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples)
{
	tint32 j;
	tint16 *outInt16 = reinterpret_cast<tint16 *>(out);
	tint16 *op = &outInt16[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		*op = sampleInt16From24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo24Int24(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples)
{
	tint32 j;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		*op = sampleInt24From24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo24Int32(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples)
{
	tint32 j;
	tint32 *outInt32 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt32[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		*op = sampleInt32From24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo24Float(tint32 *in, sample_t *out, tint outIdx, tint32 stride, tint32 numSamples)
{
	tint32 j;
	sample_t *op = &out[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		*op = sampleFrom24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo24Shift(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		copyPredictorTo24ShiftInt16(in, shift, out, outIdx, stride, numSamples, bytesShifted);
	}
	else if(type & e_SampleInt24)
	{
		copyPredictorTo24ShiftInt24(in, shift, out, outIdx, stride, numSamples, bytesShifted);
	}
	else if(type & e_SampleInt32)
	{
		copyPredictorTo24ShiftInt32(in, shift, out, outIdx, stride, numSamples, bytesShifted);
	}
	else
	{
		copyPredictorTo24ShiftFloat(in, shift, out, outIdx, stride, numSamples, bytesShifted);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo24ShiftInt16(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted)
{
	tint32 j;
	tint32 shiftVal = bytesShifted << 3;
	tint16 *outInt16 = reinterpret_cast<tint16 *>(out);
	tint16 *op = &outInt16[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		val = (val << shiftVal) | static_cast<tuint32>(shift[j]);
		*op = sampleInt16From24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo24ShiftInt24(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted)
{
	tint32 j;
	tint32 shiftVal = bytesShifted << 3;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		val = (val << shiftVal) | static_cast<tuint32>(shift[j]);
		*op = sampleInt24From24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo24ShiftInt32(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted)
{
	tint32 j;
	tint32 shiftVal = bytesShifted << 3;
	tint32 *outInt32 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt32[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		val = (val << shiftVal) | static_cast<tuint32>(shift[j]);
		*op = sampleInt32From24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo24ShiftFloat(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted)
{
	tint32 j;
	tint32 shiftVal = bytesShifted << 3;
	sample_t *op = &out[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j];
		val = (val << shiftVal) | static_cast<tuint32>(shift[j]);
		*op = sampleFrom24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo20(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint numSamples, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		copyPredictorTo20Int16(in, out, outIdx, stride, numSamples);
	}
	else if(type & e_SampleInt24)
	{
		copyPredictorTo20Int24(in, out, outIdx, stride, numSamples);
	}
	else if(type & e_SampleInt32)
	{
		copyPredictorTo20Int32(in, out, outIdx, stride, numSamples);
	}
	else
	{
		copyPredictorTo20Float(in, out, outIdx, stride, numSamples);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo20Int16(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint numSamples)
{
	tint32 j;
	tint16 *outInt16 = reinterpret_cast<tint16 *>(out);
	tint16 *op = &outInt16[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j] << 4;
		*op = sampleInt16From24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo20Int24(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint numSamples)
{
	tint32 j;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j] << 4;
		*op = sampleInt24From24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo20Int32(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint numSamples)
{
	tint32 j;
	tint32 *outInt32 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt32[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j] << 4;
		*op = sampleInt32From24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo20Float(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint numSamples)
{
	tint32 j;
	sample_t *op = &out[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		tint32 val = in[j] << 4;
		*op = sampleFrom24Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo32(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		copyPredictorTo32Int16(in, out, outIdx, stride, numSamples);
	}
	else if(type & e_SampleInt24)
	{
		copyPredictorTo32Int24(in, out, outIdx, stride, numSamples);
	}
	else if(type & e_SampleInt32)
	{
		copyPredictorTo32Int32(in, out, outIdx, stride, numSamples);
	}
	else
	{
		copyPredictorTo32Float(in, out, outIdx, stride, numSamples);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo32Int16(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples)
{
	tint32 i,j;
	tint16 *outInt16 = reinterpret_cast<tint16 *>(out);
	
	for(i=0,j=0;i<numSamples;i++,j+=stride)
	{
		outInt16[j + outIdx] = sampleInt16From32Bit(in[j]);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo32Int24(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples)
{
	tint32 i,j;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	
	for(i=0,j=0;i<numSamples;i++,j+=stride)
	{
		outInt24[j + outIdx] = sampleInt24From32Bit(in[j]);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo32Int32(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples)
{
	tint32 i,j;
	tint32 *outInt32 = reinterpret_cast<tint32 *>(out);
	
	for(i=0,j=0;i<numSamples;i++,j+=stride)
	{
		outInt32[j + outIdx] = sampleInt32From32Bit(in[j]);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo32Float(tint32 *in, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples)
{
	tint32 i,j;
	
	for(i=0,j=0;i<numSamples;i++,j+=stride)
	{
		out[j + outIdx] = sampleFrom32Bit(in[j]);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo32Shift(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		copyPredictorTo32ShiftInt16(in, shift, out, outIdx, stride, numSamples, bytesShifted);
	}
	else if(type & e_SampleInt24)
	{
		copyPredictorTo32ShiftInt24(in, shift, out, outIdx, stride, numSamples, bytesShifted);
	}
	else if(type & e_SampleInt32)
	{
		copyPredictorTo32ShiftInt32(in, shift, out, outIdx, stride, numSamples, bytesShifted);
	}
	else
	{
		copyPredictorTo32ShiftFloat(in, shift, out, outIdx, stride, numSamples, bytesShifted);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo32ShiftInt16(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted)
{
	tint32 j,val;
	tuint32 shiftVal = bytesShifted << 3;
	tint16 *outInt16 = reinterpret_cast<tint16 *>(out);
	tint16 *op = &outInt16[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		val = (in[j] << shiftVal) | static_cast<tuint32>(shift[j]);
		*op = sampleInt16From32Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo32ShiftInt24(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted)
{
	tint32 j,val;
	tuint32 shiftVal = bytesShifted << 3;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		val = (in[j] << shiftVal) | static_cast<tuint32>(shift[j]);
		*op = sampleInt24From32Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo32ShiftInt32(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted)
{
	tint32 j,val;
	tuint32 shiftVal = bytesShifted << 3;
	tint32 *outInt32 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt32[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		val = (in[j] << shiftVal) | static_cast<tuint32>(shift[j]);
		*op = sampleInt32From32Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::copyPredictorTo32ShiftFloat(tint32 *in, tuint16 *shift, sample_t *out, tint outIdx, tuint32 stride, tint32 numSamples, tint32 bytesShifted)
{
	tint32 j,val;
	tuint32 shiftVal = bytesShifted << 3;
	sample_t *op = &out[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		val = (in[j] << shiftVal) | static_cast<tuint32>(shift[j]);
		*op = sampleFrom32Bit(val);
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::clip(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		// Do nothing as Int16 is clipped by its data type size.
	}
	else if(type & e_SampleInt24)
	{
		clipInt24(out, outIdx, numSamples, stride);
	}
	else if(type & e_SampleInt32)
	{
		// Do nothing as Int32 is clipped by its data type size.
	}
	else
	{
		clipFloat(out, outIdx, numSamples, stride);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::clipInt24(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride)
{
	tint32 j;
	tint32 val;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		val = *op;

		if(val > 8388607)
		{
			val = 8388607;
		}
		else if(val < -8388608)
		{
			val = -8388608;
		}
		
		*op = val;
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::clipFloat(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride)
{
	tint32 j;
	sample_t val;
	sample_t *op = &out[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		val = *op;
		
#if defined(SINGLE_FLOAT_SAMPLE)
		if(val<-1.0f)
		{
			val = -1.0f;
		}
		else if(val>1.0f)
		{
			val = 1.0f;
		}
#else
		if(val<-1.0)
		{
			val = -1.0;
		}
		else if(val>1.0)
		{
			val = 1.0;
		}
#endif
		*op = val;
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::clipLR(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride, CodecDataType type)
{
	if(type & e_SampleInt16)
	{
		// Do nothing as Int16 is clipped by its data type size.
	}
	else if(type & e_SampleInt24)
	{
		clipLRInt24(out, outIdx, numSamples, stride);
	}
	else if(type & e_SampleInt32)
	{
		// Do nothing as Int32 is clipped by its data type size.
	}
	else
	{
		clipLRFloat(out, outIdx, numSamples, stride);
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::clipLRInt24(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride)
{
	tint32 j;
	tint32 valL, valR;
	tint32 *outInt24 = reinterpret_cast<tint32 *>(out);
	tint32 *op = &outInt24[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
		valL = op[0];
		valR = op[1];

		if(valL > 8388607)
		{
			valL = 8388607;
		}
		else if(valL < -8388608)
		{
			valL = -8388608;
		}
		
		if(valR > 8388607)
		{
			valR = 8388607;
		}
		else if(valR < -8388608)
		{
			valR = -8388608;
		}
		
		op[0] = valL;
		op[1] = valR;
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------

void ALACMatrix::clipLRFloat(sample_t *out, tint outIdx, tint32 numSamples, tuint32 stride)
{
	tint32 j;
	sample_t valL,valR;
	sample_t *op = &out[outIdx];
	
	for(j=0;j<numSamples;j++)
	{
#if defined(SINGLE_FLOAT_SAMPLE)		
		valL = op[0];
		if(valL<-1.0f)
		{
			valL = -1.0f;
		}
		else if(valL>1.0f)
		{
			valL = 1.0f;
		}
		
		valR = op[1];
		if(valR<-1.0f)
		{
			valR = -1.0f;
		}
		else if(valR>1.0f)
		{
			valR = 1.0f;
		}
#else
		valL = op[0];
		if(valL<-1.0)
		{
			valL = -1.0;
		}
		else if(valL>1.0)
		{
			valL = 1.0;
		}
		
		valR = op[1];
		if(valR<-1.0)
		{
			valR = -1.0;
		}
		else if(valR>1.0)
		{
			valR = 1.0;
		}
#endif
		
		op[0] = valL;
		op[1] = valR;
		op += stride;
	}
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
