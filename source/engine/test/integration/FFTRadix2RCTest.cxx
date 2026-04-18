#include "gtest/gtest.h"
#include "fftw3.h"
#include <QThread>
#include <math.h>

#include "common/inc/Random.h"
#include "engine/inc/FFTRadix2.h"
#include "engine/inc/DFT.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2RC, FFTWFromPCM2DSDConvertor)
{
	const tfloat64 c_TOLERANCE = 0.00000001;
	const int size = 4096;
	tint i;
	common::Random *rand = common::Random::instance();
	rand->seed(0);
	
	fftw_init_threads();
	
	tfloat64 *in = reinterpret_cast<tfloat64 *>(fftw_malloc(size * sizeof(tfloat64 *)));
	ASSERT_TRUE(in != NULL);
	fftw_complex *freq = reinterpret_cast<fftw_complex *>(fftw_malloc(size * sizeof(fftw_complex)));
	ASSERT_TRUE(freq != NULL);
	tfloat64 *out = reinterpret_cast<tfloat64 *>(fftw_malloc(size * sizeof(tfloat64 *)));
	ASSERT_TRUE(out != NULL);
	
	for(i = 0; i < size; i++)
	{
		in[i] = rand->randomReal1();
	}
	
	fftw_plan_with_nthreads(QThread::idealThreadCount());	
	fftw_plan planF = fftw_plan_dft_r2c_1d(size, in, freq, FFTW_ESTIMATE);
	ASSERT_TRUE(planF != NULL);
	
	fftw_plan_with_nthreads(QThread::idealThreadCount());
	fftw_plan planI = fftw_plan_dft_c2r_1d(size, freq, out, FFTW_ESTIMATE);
	ASSERT_TRUE(planI != NULL);
	
	fftw_execute(planF);
	fftw_execute(planI);
	
	for(i = 0; i < size; i++)
	{
		out[i] /= static_cast<tfloat64>(size);
		EXPECT_NEAR(in[i], out[i], c_TOLERANCE);
	}
	
	fftw_destroy_plan(planF);
	fftw_destroy_plan(planI);
	fftw_free(out);
	fftw_free(freq);
	fftw_free(in);
}

//-------------------------------------------------------------------------------------------

void DFT_R2C_Full(int N, tfloat64 *inR, tfloat64 *outC)
{
	for(int i = 0; i < N; i++)
	{
		tfloat64 sumR = 0.0, sumI = 0.0;
		for(int j = 0; j < N; j++)
		{
			sumR += inR[j] * cos((2.0 * c_PI_D * static_cast<double>(i * j)) / static_cast<double>(N));
			sumI -= inR[j] * sin((2.0 * c_PI_D * static_cast<double>(i * j)) / static_cast<double>(N));
		}
		outC[(i << 1) + 0] = sumR;
		outC[(i << 1) + 1] = sumI;
	}
}

void InverseDFT_C2R_Full(int N, tfloat64 *inC, tfloat64 *outR)
{
	for(int i = 0; i < N; i++)
	{
		tfloat64 sum = 0.0;
		for(int j = 0; j < N; j++)
		{
			sum += inC[(j << 1) + 0] * cos((2.0 * c_PI_D * static_cast<double>(i * j)) / static_cast<double>(N));
			sum += inC[(j << 1) + 1] * sin((2.0 * c_PI_D * static_cast<double>(i * j)) / static_cast<double>(N));
		}
		outR[i] = sum / static_cast<double>(N);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2RC, FFTRadix2CheckR2CAndC2REquations)
{
	const tfloat64 c_TOLERANCE = 0.00000001;
	const int size = 4096;
	tint i;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	fftw_init_threads();

	tfloat64* inA = reinterpret_cast<tfloat64*>(fftw_malloc(size * sizeof(tfloat64*)));
	ASSERT_TRUE(inA != NULL);
	fftw_complex* freqA = reinterpret_cast<fftw_complex*>(fftw_malloc(((size / 2) + 1) * sizeof(fftw_complex)));
	ASSERT_TRUE(freqA != NULL);
	tfloat64* outA = reinterpret_cast<tfloat64*>(fftw_malloc(size * sizeof(tfloat64*)));
	ASSERT_TRUE(outA != NULL);

	tfloat64 *inB = new tfloat64 [size];
	tfloat64 *freqB = new tfloat64 [size * 2];
	tfloat64 *outB = new tfloat64 [size];

	engine::Complex *inC = new engine::Complex [size];
	tfloat64 *inD = new tfloat64 [size * 2];

	for(i = 0; i < size; i++)
	{
		inA[i] = rand->randomReal1();
		inB[i] = inA[i];
		inC[i].R() = inA[i];
		inC[i].I() = 0.0;
		inD[(i << 1) + 0] = inA[i];
		inD[(i << 1) + 1] = 0.0;
	}

	//fftw_plan_with_nthreads(QThread::idealThreadCount());
	fftw_plan planF = fftw_plan_dft_r2c_1d(size, inA, freqA, FFTW_ESTIMATE);
	ASSERT_TRUE(planF != NULL);
	//fftw_plan_with_nthreads(QThread::idealThreadCount());
	fftw_plan planI = fftw_plan_dft_c2r_1d(size, freqA, outA, FFTW_ESTIMATE);
	ASSERT_TRUE(planI != NULL);
	fftw_execute(planF);
	fftw_execute(planI);
	for(i = 0; i < size; i++)
	{
		outA[i] /= static_cast<tfloat64>(size);
	}

	DFT_R2C_Full(size, inB, freqB);
	InverseDFT_C2R_Full(size, freqB, outB);

	engine::Complex *freqC = DFT_N_Full(inC, size);
	engine::Complex *outC = IDFT_N_Full(freqC, size);

	engine::FFTRadix2<tfloat64> dft(size);
	tfloat64 *freqD = dft.DFT(inD);
	tfloat64 *outD = dft.IDFT(freqD);

	FILE *fout = fopen("D:\\Development\\Temp\\dft_9.csv", "w");
	ASSERT_TRUE(fout != NULL);
	fprintf(fout, "idx,inA,freqA.R,freqA.I,outA,freqB.R,freqB.I,outB,freqC.R,freqC.I,outC,freqD.R,freqD.I,outD,diffB,diffC,diffD\n");
	for(i = 0; i < size / 2; i++)
	{
		fprintf(fout, "%d,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,%.8f,\n", i,
			inA[i], freqA[i][0], freqA[i][1], outA[i],
			freqB[(i << 1) + 0], freqB[(i << 1) + 1], outB[i],
			freqC[i].R(), freqC[i].I(), outC[i].R(),
			freqD[(i << 1) + 0], freqD[(i << 1) + 1], outD[(i << 1) + 0],
			freqA[i][0] - freqB[(i << 1) + 0], freqA[i][0] - freqC[i].R(), freqA[i][0] - freqD[(i << 1) + 0]);
	}
	fflush(fout);
	fclose(fout);

	delete [] inD;
	delete [] freqD;
	delete [] outD;

	delete [] inC;
	delete [] freqC;
	delete [] outC;

	delete [] inB;
	delete [] freqB;
	delete [] outB;

	fftw_destroy_plan(planF);
	fftw_destroy_plan(planI);
	fftw_free(outA);
	fftw_free(freqA);
	fftw_free(inA);
}

//-------------------------------------------------------------------------------------------

void FFT2_Radix2R2C_A(const tfloat64 x0, const tfloat64 x1, engine::Complex& X0, engine::Complex& X1)
{
	X0.R() = x0 + x1;
	X0.I() = 0.0;
	X1.R() = x0 - x1;
	X1.I() = 0.0;
}

void FFT4_Radix2R2C_A(const tfloat64 x[4], engine::Complex *X)
{
	tfloat64 F1[2], F2[2];

	F1[0] = x[0] + x[2];
	F1[1] = x[0] - x[2];
	F2[0] = x[1] + x[3];
	F2[1] = x[1] - x[3];

	X[0].R() = F1[0] + F2[0];
	X[0].I() = 0.0;
	X[1].R() = F1[1];
	X[1].I() = 0.0 - F2[1];
	X[2].R() = F1[0] - F2[0];
	X[2].I() = 0.0;
	X[3].R() = F1[1];
	X[3].I() = F2[1];
}

void FFT8_Radix2R2C_A(const tfloat64 x[8], engine::Complex *X)
{
	const tfloat64 c_halfSqr = 0.70710678118654752440084436210485;
	tfloat64 A1[2], A2[2], A3[2], A4[2];
	engine::Complex F1[4], F2[4];

	A1[0] = x[0] + x[4];
	A1[1] = x[0] - x[4];
	A2[0] = x[2] + x[6];
	A2[1] = x[2] - x[6];
	A3[0] = x[1] + x[5];
	A3[1] = x[1] - x[5];
	A4[0] = x[3] + x[7];
	A4[1] = x[3] - x[7];

	F1[0].R() = A1[0] + A2[0];
	F1[0].I() = 0.0;
	F1[1].R() = A1[1];
	F1[1].I() = 0.0 - A2[1];
	F1[2].R() = A1[0] - A2[0];
	F1[2].I() = 0.0;
	F1[3].R() = A1[1];
	F1[3].I() = A2[1];

	F2[0].R() = A3[0] + A4[0];
	F2[0].I() = 0.0;
	F2[1].R() = A3[1];
	F2[1].I() = 0.0 - A4[1];
	F2[2].R() = A3[0] - A4[0];
	F2[2].I() = 0.0;
	F2[3].R() = A3[1];
	F2[3].I() = A4[1];

	engine::Complex F2_1(c_halfSqr,-c_halfSqr);
	F2_1 *= F2[1];
	engine::Complex F2_2(0.0,-1.0);
	F2_2 *= F2[2];
	engine::Complex F2_3(-c_halfSqr,-c_halfSqr);
	F2_3 *= F2[3];
	
	X[0] = F1[0] + F2[0];
	X[1] = F1[1] + F2_1;
	X[2] = F1[2] + F2_2;
	X[3] = F1[3] + F2_3;
	
	X[4] = F1[0] - F2[0];
	X[5] = F1[1] - F2_1;
	X[6] = F1[2] - F2_2;
	X[7] = F1[3] - F2_3;
}

void FFT8_Radix2R2C_B(const tfloat64 x[8], engine::Complex* X)
{
	const tfloat64 c_halfSqr = 0.70710678118654752440084436210485;
	tfloat64 A1[2], A2[2], A3[2], A4[2], sA, sB;
	engine::Complex F1[4], F2[4];

	A1[0] = x[0] + x[4];
	A1[1] = x[0] - x[4];
	A2[0] = x[2] + x[6];
	A2[1] = x[2] - x[6];
	A3[0] = x[1] + x[5];
	A3[1] = x[1] - x[5];
	A4[0] = x[3] + x[7];
	A4[1] = x[3] - x[7];

	F1[0].R() = A1[0] + A2[0];
	F1[0].I() = 0.0;
	F1[1].R() = A1[1];
	F1[1].I() = 0.0 - A2[1];
	F1[2].R() = A1[0] - A2[0];
	F1[2].I() = 0.0;
	F1[3].R() = A1[1];
	F1[3].I() = A2[1];

	F2[0].R() = A3[0] + A4[0];
	F2[0].I() = 0.0;
	F2[1].R() = A3[1];
	F2[1].I() = 0.0 - A4[1];
	F2[2].R() = A3[0] - A4[0];
	F2[2].I() = 0.0;
	F2[3].R() = A3[1];
	F2[3].I() = A4[1];

	// a =  c_halfSqr
	// b = -c_halfSqr
	// c = F2[1].R()
	// d = F2[1].I()
	engine::Complex F2_1;
	//F2_1.R() = (c_halfSqr * F2[1].R()) - (-c_halfSqr * F2[1].I()); // (a * c) - (b * d);
	//F2_1.I() = (-c_halfSqr * F2[1].R()) + (c_halfSqr * F2[1].I()); // (b * c) + (a * d);
	sA = c_halfSqr * F2[1].R();
	sB = c_halfSqr * F2[1].I();
	F2_1.R() = sA + sB;
	F2_1.I() = sB - sA;

	// a = 0.0
	// b = -1.0
	// c = F2[2].R()
	// d = F2[2].I() = 0.0
	engine::Complex F2_2;
	//F2_2.R() = (0.0 * F2[2].R()) - (-1.0 * 0.0); // (a * c) - (b * d);
	//F2_2.I() = (-1.0 * F2[2].R()) + (0.0 * 0.0);// (b * c) + (a * d);
	F2_2.R() = 0.0;
	F2_2.I() = -F2[2].R();

	// a = -c_halfSqr
	// b = -c_halfSqr
	// c = F2[3].R()
	// d = F2[3].I()
	engine::Complex F2_3;
	//F2_1.R() = (-c_halfSqr * F2[3].R()) - (-c_halfSqr * F2[3].I()); // (a * c) - (b * d);
	//F2_1.I() = (-c_halfSqr * F2[3].R()) + (-c_halfSqr * F2[3].I()); // (b * c) + (a * d);
	sA = -c_halfSqr * F2[3].R();
	sB = -c_halfSqr * F2[3].I();
	F2_3.R() = sA - sB;
	F2_3.I() = sB + sA;

	X[0] = F1[0] + F2[0];
	X[1] = F1[1] + F2_1;
	X[2] = F1[2] + F2_2;
	X[3] = F1[3] + F2_3;

	X[4] = F1[0] - F2[0];
	X[5] = F1[1] - F2_1;
	X[6] = F1[2] - F2_2;
	X[7] = F1[3] - F2_3;
}

void FFT8_Radix2R2C_C(const tfloat64 x[8], engine::Complex* X)
{
	const tfloat64 c_halfSqr = 0.70710678118654752440084436210485;
	tfloat64 A1[2], A2[2], A3[2], A4[2], sA, sB;
	engine::Complex F1[4], F2[4];

	A1[0] = x[0] + x[4];
	A1[1] = x[0] - x[4];
	A2[0] = x[2] + x[6];
	A2[1] = x[2] - x[6];
	A3[0] = x[1] + x[5];
	A3[1] = x[1] - x[5];
	A4[0] = x[3] + x[7];
	A4[1] = x[3] - x[7];

	F1[0].R() = A1[0] + A2[0];
	F1[0].I() = 0.0;
	F1[1].R() = A1[1];
	F1[1].I() = 0.0 - A2[1];
	F1[2].R() = A1[0] - A2[0];
	F1[2].I() = 0.0;
	F1[3].R() = A1[1];
	F1[3].I() = A2[1];

	F2[0].R() = A3[0] + A4[0];
	F2[0].I() = 0.0;
	F2[1].R() = A3[1];
	F2[1].I() = 0.0 - A4[1];
	F2[2].R() = A3[0] - A4[0];
	F2[2].I() = 0.0;
	F2[3].R() = A3[1];
	F2[3].I() = A4[1];

	engine::Complex F2_1;
	sA = c_halfSqr * F2[1].R();
	sB = c_halfSqr * F2[1].I();
	F2_1.R() = sA + sB;
	F2_1.I() = sB - sA;

	engine::Complex F2_3;
	sA = -c_halfSqr * F2[3].R();
	sB = -c_halfSqr * F2[3].I();
	F2_3.R() = sA - sB;
	F2_3.I() = sB + sA;

	X[0].R() = F1[0].R() + F2[0].R();
	X[0].I() = F2[0].I();

	X[1].R() = F1[1].R() + F2_1.R();
	X[1].I() = F1[1].I() + F2_1.I();

	X[2].R() = F1[2].R();
	X[2].I() = -F2[2].R();

	X[3].R() = F1[3].R() + F2_3.R();
	X[3].I() = F1[3].I() + F2_3.I();

	X[4].R() = F1[0].R() - F2[0].R();
	X[4].I() = -F2[0].I();

	X[5].R() = F1[1].R() - F2_1.R();
	X[5].I() = F1[1].I() - F2_1.I();

	X[6].R() = F1[2].R();
	X[6].I() = F2[2].R();

	X[7].R() = F1[3].R() - F2_3.R();
	X[7].I() = F1[3].I() - F2_3.I();
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2RC, FFT2_Radix2R2C_A)
{
	const int size = 2;
	const tfloat64 c_TOLERANCE = 0.00000001;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	tfloat64 *inA = new tfloat64[size];
	engine::Complex *inB = new engine::Complex[size];

	for(int i = 0; i < size; i++)
	{
		inA[i] = rand->randomReal1();
		inB[i].R() = inA[i];
		inB[i].I() = 0.0;
	}

	engine::Complex Xa[2];
	FFT2_Radix2R2C_A(inA[0], inA[1], Xa[0], Xa[1]);

	engine::Complex *Xb = DFT_N_Full(inB, 2);

	for(int i = 0; i < size; i++)
	{
		EXPECT_NEAR(Xa[i].R(), Xb[i].R(), c_TOLERANCE);
		EXPECT_NEAR(Xa[i].I(), Xb[i].I(), c_TOLERANCE);
	}

	delete [] Xb;
	delete [] inB;
	delete [] inA;
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2RC, FFT4_Radix2R2C_A)
{
	const int size = 4;
	const tfloat64 c_TOLERANCE = 0.00000001;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	tfloat64 *inA = new tfloat64[size];
	engine::Complex *inB = new engine::Complex[size];

	for(int i = 0; i < size; i++)
	{
		inA[i] = rand->randomReal1();
		inB[i].R() = inA[i];
		inB[i].I() = 0.0;
	}

	engine::Complex Xa[4];
	FFT4_Radix2R2C_A(inA, Xa);

	engine::Complex *Xb = DFT_N_Full(inB, size);

	for(int i = 0; i < size; i++)
	{
		EXPECT_NEAR(Xa[i].R(), Xb[i].R(), c_TOLERANCE);
		EXPECT_NEAR(Xa[i].I(), Xb[i].I(), c_TOLERANCE);
	}

	delete [] Xb;
	delete [] inB;
	delete [] inA;
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2RC, FFT8_Radix2R2C_A)
{
	const int size = 8;
	const tfloat64 c_TOLERANCE = 0.00000001;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	tfloat64 *inA = new tfloat64[size];
	engine::Complex *inB = new engine::Complex[size];

	for(int i = 0; i < size; i++)
	{
		inA[i] = rand->randomReal1();
		inB[i].R() = inA[i];
		inB[i].I() = 0.0;
	}

	engine::Complex *Xa = new engine::Complex[size];
	FFT8_Radix2R2C_C(inA, Xa);

	engine::Complex *Xb = DFT_N_Full(inB, size);

	for(int i = 0; i < size; i++)
	{
		EXPECT_NEAR(Xa[i].R(), Xb[i].R(), c_TOLERANCE);
		EXPECT_NEAR(Xa[i].I(), Xb[i].I(), c_TOLERANCE);
	}

	delete [] Xa;
	delete [] Xb;

	delete [] inB;
	delete [] inA;
}

//-------------------------------------------------------------------------------------------
// FFTRadix2 decimation in frequency version (complex)
//-------------------------------------------------------------------------------------------

void FFT2_Radix2_DIF_A(engine::Complex *x, engine::Complex *X)
{
	X[0] = x[0] + x[1];
	X[1] = x[0] - x[1];
}

void FFT4_Radix2_DIF_A(engine::Complex* x, engine::Complex* X)
{
	engine::Complex t[4];

	t[0] = x[0] + x[2];
	t[1] = x[1] + x[3];
	t[2] = (x[0] - x[2]) * engine::Complex::W(0, 4);
	t[3] = (x[1] - x[3]) * engine::Complex::W(1, 4);

	X[0] = t[0] + t[1];
	X[2] = t[0] - t[1];
	X[1] = t[2] + t[3];
	X[3] = t[2] - t[3];
}

void FFT8_Radix2_DIF_A(engine::Complex* x, engine::Complex* X)
{
	engine::Complex t[8], Xt[8];

	t[0] = x[0] + x[4];
	t[1] = x[1] + x[5];
	t[2] = x[2] + x[6];
	t[3] = x[3] + x[7];
	t[4] = (x[0] - x[4]) * engine::Complex::W(0, 8);
	t[5] = (x[1] - x[5]) * engine::Complex::W(1, 8);
	t[6] = (x[2] - x[6]) * engine::Complex::W(2, 8);
	t[7] = (x[3] - x[7]) * engine::Complex::W(3, 8);

	FFT4_Radix2_DIF_A(t, Xt);
	FFT4_Radix2_DIF_A(&t[4], &Xt[4]);

	X[0] = Xt[0];
	X[1] = Xt[4];
	X[2] = Xt[1];
	X[3] = Xt[5];
	X[4] = Xt[2];
	X[5] = Xt[6];
	X[6] = Xt[3];
	X[7] = Xt[7];
}

void FFT16_Radix2_DIF_A(engine::Complex* x, engine::Complex* X)
{
	engine::Complex t[16], Xt[16];

	t[ 0] = x[ 0] + x[ 8];
	t[ 1] = x[ 1] + x[ 9];
	t[ 2] = x[ 2] + x[10];
	t[ 3] = x[ 3] + x[11];
	t[ 4] = x[ 4] + x[12];
	t[ 5] = x[ 5] + x[13];
	t[ 6] = x[ 6] + x[14];
	t[ 7] = x[ 7] + x[15];

	t[ 8] = (x[0] - x[ 8]) * engine::Complex::W(0, 16);
	t[ 9] = (x[1] - x[ 9]) * engine::Complex::W(1, 16);
	t[10] = (x[2] - x[10]) * engine::Complex::W(2, 16);
	t[11] = (x[3] - x[11]) * engine::Complex::W(3, 16);
	t[12] = (x[4] - x[12]) * engine::Complex::W(4, 16);
	t[13] = (x[5] - x[13]) * engine::Complex::W(5, 16);
	t[14] = (x[6] - x[14]) * engine::Complex::W(6, 16);
	t[15] = (x[7] - x[15]) * engine::Complex::W(7, 16);

	FFT8_Radix2_DIF_A(t, Xt);
	FFT8_Radix2_DIF_A(&t[8], &Xt[8]);

	X[ 0] = Xt[ 0];
	X[ 1] = Xt[ 8];
	X[ 2] = Xt[ 1];
	X[ 3] = Xt[ 9];
	X[ 4] = Xt[ 2];
	X[ 5] = Xt[10];
	X[ 6] = Xt[ 3];
	X[ 7] = Xt[11];
	X[ 8] = Xt[ 4];
	X[ 9] = Xt[12];
	X[10] = Xt[ 5];
	X[11] = Xt[13];
	X[12] = Xt[ 6];
	X[13] = Xt[14];
	X[14] = Xt[ 7];
	X[15] = Xt[15];
}

void FFT4_Radix2_DIF_B_Recursive(engine::Complex* x, engine::Complex* X)
{
	engine::Complex t[4];

	t[0] = x[0] + x[2];
	t[1] = x[1] + x[3];
	t[2] = (x[0] - x[2]) * engine::Complex::W(0, 4);
	t[3] = (x[1] - x[3]) * engine::Complex::W(1, 4);

	X[0] = t[0] + t[1];
	X[1] = t[0] - t[1];
	X[2] = t[2] + t[3];
	X[3] = t[2] - t[3];
}

void FFT8_Radix2_DIF_B(engine::Complex* x, engine::Complex* X)
{
	engine::Complex t[8], Xt[8];

	t[0] = x[0] + x[4];
	t[1] = x[1] + x[5];
	t[2] = x[2] + x[6];
	t[3] = x[3] + x[7];
	t[4] = (x[0] - x[4]) * engine::Complex::W(0, 8);
	t[5] = (x[1] - x[5]) * engine::Complex::W(1, 8);
	t[6] = (x[2] - x[6]) * engine::Complex::W(2, 8);
	t[7] = (x[3] - x[7]) * engine::Complex::W(3, 8);

	FFT4_Radix2_DIF_B_Recursive(t, Xt);
	FFT4_Radix2_DIF_B_Recursive(&t[4], &Xt[4]);

	X[0] = Xt[0];
	X[1] = Xt[4];
	X[2] = Xt[2];
	X[3] = Xt[6];
	X[4] = Xt[1];
	X[5] = Xt[5];
	X[6] = Xt[3];
	X[7] = Xt[7];
}

TEST(FFTRadixDIF, FFT8_Radix2_DIF_TestB)
{
	const int size = 8;
	const tfloat64 c_TOLERANCE = 0.00000001;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	engine::Complex* inA = new engine::Complex[size];
	engine::Complex* inB = new engine::Complex[size];

	for(int i = 0; i < size; i++)
	{
		inA[i].R() = rand->randomReal1();
		inA[i].I() = rand->randomReal1();
		inB[i] = inA[i];
	}

	engine::Complex* Xa = new engine::Complex[size];
	FFT8_Radix2_DIF_B(inA, Xa);

	engine::Complex* Xb = DFT_N_Full(inB, size);

	for(int i = 0; i < size; i++)
	{
		EXPECT_NEAR(Xa[i].R(), Xb[i].R(), c_TOLERANCE);
		EXPECT_NEAR(Xa[i].I(), Xb[i].I(), c_TOLERANCE);
	}

	delete[] Xa;
	delete[] Xb;
	delete[] inB;
	delete[] inA;
}

TEST(FFTRadixDIF, FFT16_Radix2_DIF_TestA)
{
	const int size = 16;
	const tfloat64 c_TOLERANCE = 0.00000001;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	engine::Complex* inA = new engine::Complex[size];
	engine::Complex* inB = new engine::Complex[size];

	for(int i = 0; i < size; i++)
	{
		inA[i].R() = rand->randomReal1();
		inA[i].I() = rand->randomReal1();
		inB[i] = inA[i];
	}

	engine::Complex* Xa = new engine::Complex[size];
	FFT16_Radix2_DIF_A(inA, Xa);

	engine::Complex* Xb = DFT_N_Full(inB, size);

	for(int i = 0; i < size; i++)
	{
		EXPECT_NEAR(Xa[i].R(), Xb[i].R(), c_TOLERANCE);
		EXPECT_NEAR(Xa[i].I(), Xb[i].I(), c_TOLERANCE);
	}

	delete[] Xa;
	delete[] Xb;
	delete[] inB;
	delete[] inA;
}

TEST(FFTRadixDIF, FFT8_Radix2_DIF_TestA)
{
	const int size = 8;
	const tfloat64 c_TOLERANCE = 0.00000001;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	engine::Complex* inA = new engine::Complex[size];
	engine::Complex* inB = new engine::Complex[size];

	for(int i = 0; i < size; i++)
	{
		inA[i].R() = rand->randomReal1();
		inA[i].I() = rand->randomReal1();
		inB[i] = inA[i];
	}

	engine::Complex* Xa = new engine::Complex[size];
	FFT8_Radix2_DIF_A(inA, Xa);

	engine::Complex* Xb = DFT_N_Full(inB, size);

	for(int i = 0; i < size; i++)
	{
		EXPECT_NEAR(Xa[i].R(), Xb[i].R(), c_TOLERANCE);
		EXPECT_NEAR(Xa[i].I(), Xb[i].I(), c_TOLERANCE);
	}

	delete [] Xa;
	delete [] Xb;
	delete [] inB;
	delete [] inA;
}


TEST(FFTRadixDIF, FFT4_Radix2_DIF_TestA)
{
	const int size = 4;
	const tfloat64 c_TOLERANCE = 0.00000001;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	engine::Complex* inA = new engine::Complex[size];
	engine::Complex* inB = new engine::Complex[size];

	for(int i = 0; i < size; i++)
	{
		inA[i].R() = rand->randomReal1();
		inA[i].I() = rand->randomReal1();
		inB[i] = inA[i];
	}

	engine::Complex *Xa = new engine::Complex[size];
	FFT4_Radix2_DIF_A(inA, Xa);

	engine::Complex* Xb = DFT_N_Full(inB, size);

	for(int i = 0; i < size; i++)
	{
		EXPECT_NEAR(Xa[i].R(), Xb[i].R(), c_TOLERANCE);
		EXPECT_NEAR(Xa[i].I(), Xb[i].I(), c_TOLERANCE);
	}

	delete [] Xb;
	delete [] Xa;
	delete [] inB;
	delete [] inA;
}

TEST(FFTRadixDIF, FFT2_Radix2_DIF_TestA)
{
	const int size = 2;
	const tfloat64 c_TOLERANCE = 0.00000001;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	engine::Complex* inA = new engine::Complex[size];
	engine::Complex* inB = new engine::Complex[size];

	for(int i = 0; i < size; i++)
	{
		inA[i].R() = rand->randomReal1();
		inA[i].I() = rand->randomReal1();
		inB[i] = inA[i];
	}

	engine::Complex Xa[2];
	FFT2_Radix2_DIF_A(inA, Xa);

	engine::Complex* Xb = DFT_N_Full(inB, 2);

	for(int i = 0; i < size; i++)
	{
		EXPECT_NEAR(Xa[i].R(), Xb[i].R(), c_TOLERANCE);
		EXPECT_NEAR(Xa[i].I(), Xb[i].I(), c_TOLERANCE);
	}

	delete[] Xb;
	delete[] inB;
	delete[] inA;
}

//-------------------------------------------------------------------------------------------

class FFTRadix2_DIF
{
	public:
		FFTRadix2_DIF(int N);
		virtual ~FFTRadix2_DIF();
		
		engine::Complex *DFT(engine::Complex *x);
		
	private:
		int m_N;
		int *m_reverseIndex;
		
		int noBits(int N) const;
		int getReverseIndex(tint index,tint noBits) const;
		
		void FFT4_Recursive(engine::Complex* x, engine::Complex* X);
		void FFT8_Recursive(engine::Complex* x, engine::Complex* X);
		void FFT_N_Recursive(engine::Complex* x, engine::Complex* X, int N);
};

FFTRadix2_DIF::FFTRadix2_DIF(int N) : m_N(N)
{
	int nBits = noBits(m_N);
	m_reverseIndex = new int [m_N];
	for(tint i=0;i<m_N;i++)
	{
		m_reverseIndex[i] = getReverseIndex(i,nBits);
	}
}

FFTRadix2_DIF::~FFTRadix2_DIF()
{
	delete [] m_reverseIndex;
}

int FFTRadix2_DIF::noBits(int N) const
{
	int count = 0;
	
	while(N > 1)
	{
		N >>= 1;
		count++;
	}
	return count;
}

int FFTRadix2_DIF::getReverseIndex(tint index,tint noBits) const
{
	tuint32 y = static_cast<tuint32>(index), x = 0;
	
	while(noBits>0)
	{
		x = (x << 1) | (y & 0x00000001);
		y >>= 1;
		noBits--;
	}
	return static_cast<int>(x);
}

void FFTRadix2_DIF::FFT4_Recursive(engine::Complex* x, engine::Complex* X)
{
	engine::Complex t[4];

	t[0] = x[0] + x[2];
	t[1] = x[1] + x[3];
	t[2] = (x[0] - x[2]) * engine::Complex::W(0, 4);
	t[3] = (x[1] - x[3]) * engine::Complex::W(1, 4);

	X[0] = t[0] + t[1];
	X[1] = t[0] - t[1];
	X[2] = t[2] + t[3];
	X[3] = t[2] - t[3];
}

void FFTRadix2_DIF::FFT8_Recursive(engine::Complex* x, engine::Complex* X)
{
	engine::Complex t[8];

	t[0] = x[0] + x[4];
	t[1] = x[1] + x[5];
	t[2] = x[2] + x[6];
	t[3] = x[3] + x[7];
	t[4] = (x[0] - x[4]) * engine::Complex::W(0, 8);
	t[5] = (x[1] - x[5]) * engine::Complex::W(1, 8);
	t[6] = (x[2] - x[6]) * engine::Complex::W(2, 8);
	t[7] = (x[3] - x[7]) * engine::Complex::W(3, 8);

	FFT4_Recursive(t, X);
	FFT4_Recursive(&t[4], &X[4]);
}

void FFTRadix2_DIF::FFT_N_Recursive(engine::Complex* x, engine::Complex* X, int N)
{
	int i, Nhalf;
	
	if(N > 8)
	{
		engine::Complex *t = new engine::Complex[N];
	
		Nhalf = N >> 1;
		for(i = 0; i < Nhalf; i++)
		{
			t[i] = x[i] + x[i + Nhalf];
			t[i + Nhalf] = (x[i] - x[i + Nhalf]) * engine::Complex::W(i, N);
		}
		
		FFT_N_Recursive(t, X, Nhalf);
		FFT_N_Recursive(&t[Nhalf], &X[Nhalf], Nhalf);
		
		delete [] t;
	}
	else
	{
		FFT8_Recursive(x, X);
	}
}

engine::Complex *FFTRadix2_DIF::DFT(engine::Complex *x)
{
	int i;
	engine::Complex *Xt, *X;
	
	X = new engine::Complex[m_N];
	Xt = new engine::Complex[m_N];
	FFT_N_Recursive(x, Xt, m_N);
	for(i = 0; i < m_N; i++)
	{
		X[i] = Xt[m_reverseIndex[i]];
	}
	delete [] Xt;
	return X;
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadixDIF, FFT_Radix2_DIF)
{
	const tfloat64 c_TOLERANCE = 0.00000001;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	for(int shift = 4; shift < 13; shift++)
	{
		int size = 1 << shift;
		engine::Complex* inA = new engine::Complex[size];
		engine::Complex* inB = new engine::Complex[size];

		for(int i = 0; i < size; i++)
		{
			inA[i].R() = rand->randomReal1();
			inA[i].I() = rand->randomReal1();
			inB[i] = inA[i];
		}
	
		FFTRadix2_DIF fft(size);
		engine::Complex* Xa = fft.DFT(inA);

		engine::Complex* Xb = DFT_N_Full(inB, size);

		for(int i = 0; i < size; i++)
		{
			EXPECT_NEAR(Xa[i].R(), Xb[i].R(), c_TOLERANCE);
			EXPECT_NEAR(Xa[i].I(), Xb[i].I(), c_TOLERANCE);
		}

		delete[] Xa;
		delete[] Xb;
		delete[] inB;
		delete[] inA;
	}
}

//-------------------------------------------------------------------------------------------
