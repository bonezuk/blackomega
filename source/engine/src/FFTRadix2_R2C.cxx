#include "engine/inc/FFTRadix2_R2C.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------


FFTRadix2_R2C::FFTRadix2_R2C() : FFTRadix2Base()
{}

//-------------------------------------------------------------------------------------------

FFTRadix2_R2C::~FFTRadix2_R2C()
{}

//-------------------------------------------------------------------------------------------

void FFTRadix2_R2C::DFT(const tfloat64* x, tfloat64* X)
{
	reverse(x, m_xIO);
	FFTRecursive(m_xIO, X, 0, m_N);
}

//-------------------------------------------------------------------------------------------

void FFTRadix2_R2C::FFTRecursive(const tfloat64 *xIn, tfloat64 *Xout, int index, int N)
{
	const tfloat64 *x = &xIn[index];
	tfloat64 *X = &Xout[index << 1];

	if(N < 16)
	{
		FFT8(x, X);
	}
	else
	{
		int halfN = N >> 1;
		int bitIndex = noBits(N) - 4;
		tfloat64 *F1, *F2, *F, *W, *Y;

		F = m_stack[bitIndex];
		FFTRecursive(xIn, F, index, halfN);
		F1 = &F[index << 1];
		FFTRecursive(xIn, F, index + halfN, halfN);
		F2 = &F[(index + halfN) << 1];

		W = m_coeff[bitIndex];
		for(int i = 0; i < halfN; i++)
		{
			int idx = i << 1;
			tfloat64 tR = complexMultiplyReal(&W[idx], &F2[idx]);
			tfloat64 tI = complexMultiplyImaginary(&W[idx], &F2[idx]);
			F2[idx + 0] = tR;
			F2[idx + 1] = tI;
		}

		if(N < m_N)
		{
			Y = &X[N];
			for(int i = 0; i < N; i++)
			{
				X[i] = F1[i] + F2[i];
				Y[i] = F1[i] - F2[i];
			}
		}
		else
		{
			for(int i = 0; i < N; i++)
			{
				X[i] = F1[i] + F2[i];
			}
			X[N] = F1[0] - F2[0];
			X[N+1] = F1[1] - F2[1];
		}
	}
}

//-------------------------------------------------------------------------------------------

void FFTRadix2_R2C::FFT8(const tfloat64* x, tfloat64* Xout) const
{
	const tfloat64 c_halfSqr = 0.70710678118654752440084436210485;
	tfloat64(*X)[2] = reinterpret_cast<tfloat64(*)[2]>(Xout);
	tfloat64 A[4][2], F1[2], F2[2], F3[2], sA, sB;

	A[0][0] = x[0] + x[1];
	A[0][1] = x[0] - x[1];
	A[1][0] = x[2] + x[3];
	A[1][1] = x[2] - x[3];
	A[2][0] = x[4] + x[5];
	A[2][1] = x[4] - x[5];
	A[3][0] = x[6] + x[7];
	A[3][1] = x[6] - x[7];

	F1[0] = A[0][0] + A[1][0];
	F1[1] = A[0][0] - A[1][0];
	F2[0] = A[2][0] + A[3][0];
	F2[1] = A[2][0] - A[3][0];

	sA = -c_halfSqr * A[2][1];
	sB = -c_halfSqr * A[3][1];
	F3[0] = sB - sA;
	F3[1] = sB + sA;

	X[0][0] = F1[0] + F2[0];
	X[0][1] = 0.0;
	X[1][0] = A[0][1] + F3[0];
	X[1][1] = F3[1] - A[1][1];
	X[2][0] = F1[1];
	X[2][1] = -F2[1];
	X[3][0] = A[0][1] - F3[0];
	X[3][1] = A[1][1] + F3[1];
	X[4][0] = F1[0] - F2[0];
	X[4][1] = 0.0;
	X[5][0] = A[0][1] - F3[0];
	X[5][1] = -A[1][1] - F3[1];
	X[6][0] = F1[1];
	X[6][1] = F2[1];
	X[7][0] = A[0][1] + F3[0];
	X[7][1] = A[1][1] - F3[1];
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
