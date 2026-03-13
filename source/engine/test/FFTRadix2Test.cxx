#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/Random.h"
#include "engine/inc/Complex.h"
#include "engine/inc/FFTRadix2.h"

using namespace omega;
using namespace omega::engine;

//-------------------------------------------------------------------------------------------

Complex *DFT8(Complex *x)
{
	Complex *X = new Complex [8];
	
	for(int k=0;k<8;k++)
	{
		Complex c;
		
		for(int n=0;n<8;n++)
		{
			c += x[n] * Complex::W(k,n,8);
		}
		X[k] = c;
	}
	return X;
}

//-------------------------------------------------------------------------------------------

Complex *DFT16(Complex *x)
{
	Complex *X = new Complex [16];
	
	for(int k=0;k<16;k++)
	{
		Complex c;
		
		for(int n=0;n<16;n++)
		{
			c += x[n] * Complex::W(k,n,16);
		}
		X[k] = c;
	}
	return X;
}

//-------------------------------------------------------------------------------------------

Complex *DFT_N(Complex *x,tint N)
{
	Complex *X = new Complex [N];
	
	for(int k=0;k<N;k++)
	{
		Complex c;
		
		for(int n=0;n<N;n++)
		{
			c += x[n] * Complex::W(k,n,N);
		}
		X[k] = c;
	}
	return X;
}

//-------------------------------------------------------------------------------------------

void FFTRadix2_Test(tint c_N)
{
	const tfloat64 c_TOLERANCE = 0.00000001;

	common::Random *rand = common::Random::instance();
	rand->seed(0);
	
	Complex *x = new Complex [c_N];
	tfloat64 *xD = new tfloat64 [c_N * 2];
	for(tint i=0;i<c_N;i++)
	{
		xD[(i<<1)+0] = x[i].R() = rand->randomReal1();
		xD[(i<<1)+1] = x[i].I() = rand->randomReal1();
	}
	
	Complex *eX = DFT_N(x,c_N);
	
	FFTRadix2<tfloat64> FFT(c_N);
	
	tfloat64 *tX = FFT.DFT(xD);
	
	for(tint i=0;i<c_N;i++)
	{
		EXPECT_NEAR(eX[i].R(),tX[(i<<1)+0],c_TOLERANCE);
		EXPECT_NEAR(eX[i].I(),tX[(i<<1)+1],c_TOLERANCE);
	}
	
	delete [] eX;
	delete [] tX;
	delete [] x;
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT16_Radix2)
{
	FFTRadix2_Test(16);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT32_Radix2)
{
	FFTRadix2_Test(32);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT64_Radix2)
{
	FFTRadix2_Test(64);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT128_Radix2)
{
	FFTRadix2_Test(128);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT256_Radix2)
{
	FFTRadix2_Test(256);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT512_Radix2)
{
	FFTRadix2_Test(512);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT1024_Radix2)
{
	FFTRadix2_Test(1024);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT2048_Radix2)
{
	FFTRadix2_Test(2048);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT4096_Radix2)
{
	FFTRadix2_Test(4096);
}

//-------------------------------------------------------------------------------------------
