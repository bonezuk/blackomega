//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FFTRADIX2_H
#define __OMEGA_ENGINE_FFTRADIX2_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Complex.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

template <typename V> class FFTRadix2
{
	public:
		FFTRadix2(int N);
		virtual ~FFTRadix2();
		
		V *DFT(V *x);
		
	protected:
	
		tint m_N;
		tint *m_reverseIndex;
		
		V **m_coefficient;
		V **m_stackA;
		V **m_stackB;

		void initialize();
		void done();
		
		tint noBits(tint N) const;
		tint getReverseIndex(tint index,tint noBits) const;

		V complexMultiplyReal(const V *X,const V *Y) const;
		V complexMultiplyImaginary(const V *X,const V *Y) const;

		V *DFT4(V *x,int index,V *X);
		V *DFT8(V *x,int index,bool sFlag);
		V *DFTRecursive(V *x,int index,int N,bool sFlag);
};

//-------------------------------------------------------------------------------------------

template <typename V> FFTRadix2<V>::FFTRadix2(int N) : m_N(N),
	m_reverseIndex(0),
	m_coefficient(0),
	m_stackA(0),
	m_stackB(0)
{
	initialize();
}

//-------------------------------------------------------------------------------------------

template <typename V> FFTRadix2<V>::~FFTRadix2()
{
	done();
}

//-------------------------------------------------------------------------------------------

template <typename V> void FFTRadix2<V>::initialize()
{
	tint nBits = noBits(m_N);
	m_reverseIndex = new tint [m_N];
	for(tint i=0;i<m_N;i++)
	{
		m_reverseIndex[i] = getReverseIndex(i,nBits);
	}
	
	m_coefficient = reinterpret_cast<V **>(malloc((nBits - 2) * sizeof(V *)));
	m_stackA = reinterpret_cast<V **>(malloc((nBits - 2) * sizeof(V *)));
	m_stackB = reinterpret_cast<V **>(malloc((nBits - 3) * sizeof(V *)));

	tfloat64 halfSquared = sqrt(0.5);
	V *c2 = new V [4];
	c2[0] =  halfSquared;
	c2[1] = -halfSquared;
	c2[2] = -halfSquared;
	c2[3] = -halfSquared;
	m_coefficient[0] = c2;
	m_stackA[0] = new V [32];
	m_stackB[0] = new V [32];
	
	for(tint i=4;i<=nBits;i++)
	{
		tint M = 1 << i;
		tint len = (M >> 1) - 1;
		V *coefficients = new V [len << 1];
		for(tint j=1; j<(M >> 1); j++)
		{
			tfloat64 angle = (2.0 * c_PI_D * static_cast<tfloat64>(j)) / static_cast<tfloat64>(M);
			coefficients[((j-1)<<1)+0] = static_cast<V>(cos(angle));
			coefficients[((j-1)<<1)+1] = static_cast<V>(0.0 - sin(angle));
		}
		m_coefficient[i-3] = coefficients;
		if(i<nBits)
		{
			m_stackA[i-3] = new V [M<<1];
			m_stackB[i-3] = new V [M<<1];
		}
	}
}

//-------------------------------------------------------------------------------------------

template <typename V> void FFTRadix2<V>::done()
{
	tint nBits = noBits(m_N);

	if(m_reverseIndex!=0)
	{
		delete [] m_reverseIndex;
		m_reverseIndex = 0;
	}
	if(m_coefficient!=0)
	{
		for(tint i=3;i<=nBits;i++)
		{
			V *c = m_coefficient[i-3];
			delete [] c;
			if(i<nBits)
			{
				V *sA = m_stackA[i-3];
				delete [] sA;
				V *sB = m_stackB[i-3];
				delete [] sB;
			}
		}
		free(m_coefficient);
		m_coefficient = 0;
		free(m_stackA);
		m_stackA = 0;
		free(m_stackB);
		m_stackB = 0;
	}
}

//-------------------------------------------------------------------------------------------

template <typename V> tint FFTRadix2<V>::noBits(tint N) const
{
	tint count = 0;
	
	while(N > 1)
	{
		N >>= 1;
		count++;
	}
	return count;
}

//-------------------------------------------------------------------------------------------

template <typename V> tint FFTRadix2<V>::getReverseIndex(tint index,tint noBits) const
{
	tuint32 y = static_cast<tuint32>(index), x = 0;
	
	while(noBits>0)
	{
		x = (x << 1) | (y & 0x00000001);
		y >>= 1;
		noBits--;
	}
	return static_cast<tint>(x << 1);
}

//-------------------------------------------------------------------------------------------

template <typename V> V FFTRadix2<V>::complexMultiplyReal(const V *X,const V *Y) const
{
	return (X[0] * Y[0]) - (X[1] * Y[1]);
}

//-------------------------------------------------------------------------------------------

template <typename V> V FFTRadix2<V>::complexMultiplyImaginary(const V *X,const V *Y) const
{
	return (X[0] * Y[1]) + (X[1] * Y[0]);
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2<V>::DFT4(V *x,int index,V *X)
{
	tint idxA,idxB,idxC,idxD;
	V F1[4],F2[4];

	idxA = m_reverseIndex[index+0];
	idxB = m_reverseIndex[index+1];
	idxC = m_reverseIndex[index+2];
	idxD = m_reverseIndex[index+3];

	F1[0] = x[idxA] + x[idxB];
	F1[1] = x[idxA+1] + x[idxB+1];
	F1[2] = x[idxA] - x[idxB];
	F1[3] = x[idxA+1] - x[idxB+1];

	F2[0] = x[idxC] + x[idxD];
	F2[1] = x[idxC+1] + x[idxD+1];
	F2[2] = x[idxC] - x[idxD];
	F2[3] = x[idxC+1] - x[idxD+1];

	X[0] = F1[0] + F2[0];
	X[1] = F1[1] + F2[1];
	X[2] = F1[2] + F2[3];
	X[3] = F1[3] - F2[2];
	X[4] = F1[0] - F2[0];
	X[5] = F1[1] - F2[1];
	X[6] = F1[2] - F2[3];
	X[7] = F1[3] + F2[2];
		
	return X;
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2<V>::DFT8(V *x,int index,bool sFlag)
{
	V *coefficients = m_coefficient[0];
	V *X = (sFlag) ? m_stackA[0] : m_stackB[0];

	V t;
	V *F1 = DFT4(x,index,&X[16]);
	V *F2 = DFT4(x,index+4,&X[24]);
	
	t = complexMultiplyReal(&coefficients[0],&F2[2]);
	F2[3] = complexMultiplyImaginary(&coefficients[0],&F2[2]);
	F2[2] = t;

	t = complexMultiplyReal(&coefficients[2],&F2[6]);
	F2[7] = complexMultiplyImaginary(&coefficients[2],&F2[6]);
	F2[6] = t;
	
	X[ 0] = F1[0] + F2[0];
	X[ 1] = F1[1] + F2[1];
	X[ 2] = F1[2] + F2[2];
	X[ 3] = F1[3] + F2[3];
	X[ 4] = F1[4] + F2[5];
	X[ 5] = F1[5] - F2[4];
	X[ 6] = F1[6] + F2[6];
	X[ 7] = F1[7] + F2[7];
	
	X[ 8] = F1[0] - F2[0];
	X[ 9] = F1[1] - F2[1];
	X[10] = F1[2] - F2[2];
	X[11] = F1[3] - F2[3];
	X[12] = F1[4] - F2[5];
	X[13] = F1[5] + F2[4];
	X[14] = F1[6] - F2[6];
	X[15] = F1[7] - F2[7];
	
	return X;
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2<V>::DFTRecursive(V *x,int index,int N,bool sFlag)
{
	tint i;
	tint halfN = N >> 1;
	tint bitIndex = noBits(N) - 3;
	V *coefficients = m_coefficient[bitIndex];
	V *X;
	
	if(N==m_N)
	{
		X = new V [N << 1];
	}
	else
	{
		X = (sFlag) ? m_stackA[bitIndex] : m_stackB[bitIndex];
	}
	
	V *Y = &X[N];
	V *F1 = (halfN != 8) ? DFTRecursive(x,index,halfN,true) : DFT8(x,index,true);
	V *F2 = (halfN != 8) ? DFTRecursive(x,index+halfN,halfN,false) : DFT8(x,index+halfN,false);
	
	for(i=1;i<halfN;i++)
	{
		tint idxA = (i-1) << 1;
		tint idxB = i << 1;
		tfloat64 t = complexMultiplyReal(&coefficients[idxA],&F2[idxB]);
		F2[idxB+1] = complexMultiplyImaginary(&coefficients[idxA],&F2[idxB]);
		F2[idxB] = t;
	}
	
	for(i=0;i<N;i++)
	{
		X[i] = F1[i] + F2[i];
		Y[i] = F1[i] - F2[i];
	}
	
	return X;
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2<V>::DFT(V *x)
{
	return DFTRecursive(x,0,m_N,false);
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
