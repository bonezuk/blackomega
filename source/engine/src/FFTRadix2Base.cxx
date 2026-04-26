#include "engine/inc/FFTRadix2Base.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

FFTRadix2Base::FFTRadix2Base() : m_N(0),
    m_reverseIndex(NULL),
    m_coeff(NULL),
    m_stack(NULL),
    m_xIO(NULL)
{}

//-------------------------------------------------------------------------------------------

FFTRadix2Base::~FFTRadix2Base()
{
	FFTRadix2Base::done();
}

//-------------------------------------------------------------------------------------------

bool FFTRadix2Base::init(int N)
{
	int nBits;
		
	nBits = noBits(N);

	if(N < 16)
		return false;
	m_N = N;
	if(m_N != (1 << nBits))
		return false;

	m_reverseIndex = static_cast<int *>(malloc(m_N * sizeof(int)));
	if(m_reverseIndex == NULL)
		return false;
	for(int i = 0; i < m_N; i++)
	{
		m_reverseIndex[i] = getReverseIndex(i, nBits);
	}

	m_xIO = static_cast<tfloat64 *>(malloc(m_N * sizeof(tfloat64)));
	if(m_xIO == NULL)
		return false;

	m_coeff = static_cast<tfloat64 **>(calloc(nBits - 3, sizeof(tfloat64 *)));
	if(m_coeff == NULL)
		return false;
	m_stack = static_cast<tfloat64 **>(calloc(nBits - 3, sizeof(tfloat64 *)));
	if(m_stack == NULL)
		return false;

	for(int i = 4; i <= nBits; i++)
	{
		int M = 1 << i;
		int len = M >> 1;
		tfloat64 *c = static_cast<tfloat64 *>(malloc(2 * len * sizeof(tfloat64)));
		if(c == NULL)
			return false;
		m_coeff[i - 4] = c;

		for(int j = 0; j < len; j++)
		{
			tfloat64 angle = (2.0 * c_PI_D * static_cast<tfloat64>(j)) / static_cast<tfloat64>(M);
			c[(j << 1) + 0] = cos(angle);
			c[(j << 1) + 1] = 0.0 - sin(angle);
		}
		m_stack[i - 4] = static_cast<tfloat64 *>(malloc(m_N * 2 * sizeof(tfloat64)));
		if(m_stack[i - 4] == NULL)
			return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------

void FFTRadix2Base::done()
{
	int nBits = noBits(m_N);

	for(int i = 0; i <= nBits - 4; i++)
	{
		if(m_coeff != NULL && m_coeff[i] != NULL)
		{
			free(m_coeff[i]);
			m_coeff[i] = NULL;
		}
		if(m_stack != NULL && m_stack[i] != NULL)
		{
			free(m_stack[i]);
			m_stack[i] = NULL;
		}
	}
	if(m_coeff != NULL)
	{
		free(m_coeff);
		m_coeff = NULL;
	}
	if(m_stack != NULL)
	{
		free(m_stack);
		m_stack = NULL;
	}
	if(m_xIO != NULL)
	{
		free(m_xIO);
		m_xIO = NULL;
	}
	if(m_reverseIndex != NULL)
	{
		free(m_reverseIndex);
		m_reverseIndex = NULL;
	}
}

//-------------------------------------------------------------------------------------------

int FFTRadix2Base::noBits(int N) const
{
	int count = 0;

	while(N > 1)
	{
		N >>= 1;
		count++;
	}
	return count;
}

//-------------------------------------------------------------------------------------------

int FFTRadix2Base::getReverseIndex(int index, int noBits) const
{
	tuint32 y = static_cast<tuint32>(index), x = 0;

	while(noBits > 0)
	{
		x = (x << 1) | (y & 0x00000001);
		y >>= 1;
		noBits--;
	}
	return static_cast<tint>(x);
}

//-------------------------------------------------------------------------------------------

void FFTRadix2Base::reverse(const tfloat64 *x, tfloat64 *y) const
{
	for(int i = 0; i < m_N; i++)
	{
		y[i] = x[m_reverseIndex[i]];
	}
}

//-------------------------------------------------------------------------------------------

tfloat64 FFTRadix2Base::complexMultiplyReal(const tfloat64 *X, const tfloat64 *Y) const
{
	return (X[0] * Y[0]) - (X[1] * Y[1]);
}

//-------------------------------------------------------------------------------------------

tfloat64 FFTRadix2Base::complexMultiplyImaginary(const tfloat64 *X, const tfloat64 *Y) const
{
	return (X[0] * Y[1]) + (X[1] * Y[0]);
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
