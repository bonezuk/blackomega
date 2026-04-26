#include "engine/inc/FFTRadix2_C2R.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

FFTRadix2_C2R::FFTRadix2_C2R() : FFTRadix2Base(),
    m_xIn(NULL)
{}

//-------------------------------------------------------------------------------------------

FFTRadix2_C2R::~FFTRadix2_C2R()
{}

//-------------------------------------------------------------------------------------------

bool FFTRadix2_C2R::init(int N)
{
	if(!FFTRadix2Base::init(N))
		return false;

	m_xIn = static_cast<tfloat64 *>(malloc(m_N * 2 * sizeof(tfloat64)));
	if(m_xIn == NULL)
		return false;
	return true;
}

//-------------------------------------------------------------------------------------------

void FFTRadix2_C2R::done()
{
	if(m_xIn != NULL)
	{
		free(m_xIn);
		m_xIn = NULL;
	}
	FFTRadix2Base::done();
}

//-------------------------------------------------------------------------------------------

void FFTRadix2_C2R::reorder(const tfloat64 *x, tfloat64 *y)
{
	int i, j;

	y[0] = x[0];
	i = 1;
	j = m_N - 1;
	while(i < m_N)
	{
		y[i] = x[m_reverseIndex[j]];
		i++;
		j--;
	}
}

//-------------------------------------------------------------------------------------------

void FFTRadix2_C2R::iDFT(const tfloat64* x, tfloat64* X)
{
	const tfloat64(*s)[2] = reinterpret_cast<const tfloat64(*)[2]>(x);
	tfloat64(*t)[2] = reinterpret_cast<tfloat64(*)[2]>(m_xIn);
	int i, j, halfN = m_N >> 1;

	for(i = 0; i <= halfN; i++)
	{
		t[i][0] = s[i][0];
		t[i][1] = s[i][1];
	}
	j = halfN - 1;
	while(i < m_N)
	{
		t[i][0] = s[j][0];
		t[i][1] = 0.0 - s[j][1];
		j--;
		i++;
	}
	FFTRecursive(m_xIn, m_xIO, 0, m_N);
	for(i = 0; i < m_N; i++)
	{
		m_xIO[i] /= static_cast<tfloat64>(m_N);
	}
	reorder(m_xIO, X);
}

//-------------------------------------------------------------------------------------------

void FFTRadix2_C2R::FFTRecursive(const tfloat64 *xIn, tfloat64 *Xout, int index, int N)
{
	if(N > 8)
	{
		int halfN = N >> 1;
		int bitIndex = noBits(N) - 4;
		tfloat64 *s = m_stack[bitIndex];
		const tfloat64(*x)[2] = reinterpret_cast<const tfloat64(*)[2]>(&xIn[index << 1]);
		tfloat64(*t)[2] = reinterpret_cast<tfloat64(*)[2]>(&s[index << 1]);
		tfloat64 *c = reinterpret_cast<tfloat64 *>(m_coeff[bitIndex]);
		tfloat64 a[2];

		for(int i = 0; i < halfN; i++)
		{
			t[i][0] = x[i][0] + x[i + halfN][0];
			t[i][1] = x[i][1] + x[i + halfN][1];
			
			a[0] = x[i][0] - x[i + halfN][0];
			a[1] = x[i][1] - x[i + halfN][1];

			t[i + halfN][0] = complexMultiplyReal(a, &c[i << 1]);
			t[i + halfN][1] = complexMultiplyImaginary(a, &c[i << 1]);
		}

		FFTRecursive(s, Xout, index, halfN);
		FFTRecursive(s, Xout, index + halfN, halfN);
	}
	else
	{
		FFT8(&xIn[index << 1], &Xout[index]);
	}
}

//-------------------------------------------------------------------------------------------

void FFTRadix2_C2R::FFT8(const tfloat64* xIn, tfloat64* X) const
{
	const tfloat64 c_halfSqr = 0.70710678118654752440084436210485;
	const tfloat64(*x)[2] = reinterpret_cast<const tfloat64(*)[2]>(xIn);
	tfloat64 t[4][2], s[4], aR, aI;

	t[0][0] = x[0][0] + x[4][0];
	t[1][0] = x[1][0] + x[5][0];
	t[1][1] = x[1][1] + x[5][1];
	t[2][0] = x[2][0] + x[6][0];
	t[3][0] = x[3][0] + x[7][0];
	t[3][1] = x[3][1] + x[7][1];

	s[0] = t[0][0] + t[2][0];
	s[1] = t[1][0] + t[3][0];
	s[2] = t[0][0] - t[2][0];
	s[3] = t[1][1] - t[3][1];

	X[0] = s[0] + s[1];
	X[1] = s[0] - s[1];
	X[2] = s[2] + s[3];
	X[3] = s[2] - s[3];

	t[0][0] = x[0][0] - x[4][0];
	aR = x[1][0] - x[5][0];
	aI = x[1][1] - x[5][1];
	t[1][0] = c_halfSqr * (aR + aI);
	t[1][1] = c_halfSqr * (aI - aR);
	t[2][0] = x[2][1] - x[6][1];
	aR = x[3][0] - x[7][0];
	aI = x[3][1] - x[7][1];
	t[3][0] = c_halfSqr * (aI - aR);
	t[3][1] = -c_halfSqr * (aR + aI);

	s[0] = t[0][0] + t[2][0];
	s[1] = t[1][0] + t[3][0];
	s[2] = t[0][0] - t[2][0];
	s[3] = t[1][1] - t[3][1];

	X[4] = s[0] + s[1];
	X[5] = s[0] - s[1];
	X[6] = s[2] + s[3];
	X[7] = s[2] - s[3];
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
