#include "gtest/gtest.h"

#include "engine/inc/FFTRadix2_R2C.h"
#include "engine/inc/FFTRadix2_C2R.h"
#include "engine/inc/DFT.h"

using namespace omega;
using namespace omega::engine;

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT8)
{
	FFTRadix2_R2C fftA;
	ASSERT_FALSE(fftA.init(8));
	FFTRadix2_C2R fftB;
	ASSERT_FALSE(fftB.init(8));
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT48)
{
	FFTRadix2_R2C fftA;
	ASSERT_FALSE(fftA.init(48));
	FFTRadix2_C2R fftB;
	ASSERT_FALSE(fftB.init(48));
}

//-------------------------------------------------------------------------------------------

void testFFTRadix2_R2C_C2R_OfSize(int size)
{
	tfloat64 *inA = new tfloat64 [size];
	engine::Complex* inB = new engine::Complex[size];

	const tfloat64 c_TOLERANCE = 0.00000001;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	for(int i = 0; i < size; i++)
	{
		inA[i] = rand->randomReal1();
		inB[i].R() = inA[i];
		inB[i].I() = 0.0;
	}

	tfloat64 *Xa = new tfloat64 [size + 2];
	FFTRadix2_R2C fftA;
	ASSERT_TRUE(fftA.init(size));
	fftA.DFT(inA, Xa);

	engine::Complex* Xb = DFT_N_Full(inB, size);

	int i, j;
	for(i = 0; i < (size / 2) + 1; i++)
	{
		EXPECT_NEAR(Xa[(i << 1) + 0], Xb[i].R(), c_TOLERANCE);
		EXPECT_NEAR(Xa[(i << 1) + 1], Xb[i].I(), c_TOLERANCE);
	}
	j = (size / 2) - 1;
	while(i < size)
	{
		EXPECT_NEAR(Xa[(j << 1) + 0], Xb[i].R(), c_TOLERANCE);
		EXPECT_NEAR(0.0 - Xa[(j << 1) + 1], Xb[i].I(), c_TOLERANCE);
		j--;
		i++;
	}

	tfloat64 *outA = new tfloat64 [size];
	FFTRadix2_C2R fftB;
	ASSERT_TRUE(fftB.init(size));
	fftB.iDFT(Xa, outA);

	for(int i = 0; i < size; i++)
	{
		EXPECT_NEAR(outA[i], inA[i], c_TOLERANCE);
	}

	delete [] outA;
	delete [] inA;
	delete [] inB;
	delete [] Xa;
	delete [] Xb;

}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT16)
{
	testFFTRadix2_R2C_C2R_OfSize(16);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT32)
{
	testFFTRadix2_R2C_C2R_OfSize(32);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT64)
{
	testFFTRadix2_R2C_C2R_OfSize(64);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT128)
{
	testFFTRadix2_R2C_C2R_OfSize(128);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT256)
{
	testFFTRadix2_R2C_C2R_OfSize(256);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT512)
{
	testFFTRadix2_R2C_C2R_OfSize(512);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT1024)
{
	testFFTRadix2_R2C_C2R_OfSize(1024);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT2048)
{
	testFFTRadix2_R2C_C2R_OfSize(2048);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_C2R, DFT4096)
{
	testFFTRadix2_R2C_C2R_OfSize(4096);
}

//-------------------------------------------------------------------------------------------

void testFFTRadix2_R2C_OctaveUpScale(int size)
{
	const tfloat64 c_TOLERANCE = 0.00000001;

	common::Random *rand = common::Random::instance();
	rand->seed(0);

	tfloat64 *inA = new tfloat64 [size];
	tfloat64 *inB = new tfloat64 [size >> 1];
	for(int i = 0; i < size; i += 2)
	{
		inA[i+0] = rand->randomReal1();
		inA[i+1] = 0.0;
		inB[i >> 1] = inA[i];
	}

	int Nout = (size / 2) + 1;

	tfloat64 *Xa = new tfloat64 [Nout << 1];
	engine::FFTRadix2_R2C fftA;
	ASSERT_TRUE(fftA.init(size));
	fftA.DFT(inA, Xa);

	tfloat64 *Xb = new tfloat64[Nout << 1];
	engine::FFTRadix2_R2C_OctaveUpscale fftB;
	ASSERT_TRUE(fftB.init(size));
	fftB.DFT(inB, Xb);

	for(int i = 0; i < Nout << 1; i++)
	{
		EXPECT_NEAR(Xa[i], Xb[i], c_TOLERANCE);
	}

	delete [] Xb;
	delete [] Xa;
	delete [] inB;
	delete [] inA;
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT16)
{
	testFFTRadix2_R2C_OctaveUpScale(16);
}
//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT32)
{
	testFFTRadix2_R2C_OctaveUpScale(32);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT64)
{
	testFFTRadix2_R2C_OctaveUpScale(64);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT128)
{
	testFFTRadix2_R2C_OctaveUpScale(128);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT256)
{
	testFFTRadix2_R2C_OctaveUpScale(256);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT512)
{
	testFFTRadix2_R2C_OctaveUpScale(512);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT1024)
{
	testFFTRadix2_R2C_OctaveUpScale(1024);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT2048)
{
	testFFTRadix2_R2C_OctaveUpScale(2048);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT4096)
{
	testFFTRadix2_R2C_OctaveUpScale(4096);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT8192)
{
	testFFTRadix2_R2C_OctaveUpScale(8192);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT16384)
{
	testFFTRadix2_R2C_OctaveUpScale(16384);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT32768)
{
	testFFTRadix2_R2C_OctaveUpScale(32768);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT65536)
{
	testFFTRadix2_R2C_OctaveUpScale(65536);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT131072)
{
	testFFTRadix2_R2C_OctaveUpScale(131072);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT262144)
{
	testFFTRadix2_R2C_OctaveUpScale(262144);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT524288)
{
	testFFTRadix2_R2C_OctaveUpScale(524288);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2_R2C_OctaveUpscale, DFT1048576)
{
	testFFTRadix2_R2C_OctaveUpScale(1048576);
}

//-------------------------------------------------------------------------------------------
