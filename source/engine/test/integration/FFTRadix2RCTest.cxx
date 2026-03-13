#include "gtest/gtest.h"
#include "fftw3.h"
#include <QThread>
#include <math.h>

#include "common/inc/Random.h"

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

TEST(FFTRadix2RC, FFTWCheckR2CAndC2REquations)
{
	const tfloat64 c_TOLERANCE = 0.00000001;
	const int size = 4096;
	tint i;
	common::Random* rand = common::Random::instance();
	rand->seed(0);

	fftw_init_threads();

	tfloat64* inA = reinterpret_cast<tfloat64*>(fftw_malloc(size * sizeof(tfloat64*)));
	ASSERT_TRUE(inA != NULL);
	fftw_complex* freqA = reinterpret_cast<fftw_complex*>(fftw_malloc(size * sizeof(fftw_complex)));
	ASSERT_TRUE(freqA != NULL);
	tfloat64* outA = reinterpret_cast<tfloat64*>(fftw_malloc(size * sizeof(tfloat64*)));
	ASSERT_TRUE(outA != NULL);

	tfloat64 *inB = new tfloat64[size];
	tfloat64 *freqB = new tfloat64[size * 2];
	tfloat64 *outB = new tfloat64[size];

	for(i = 0; i < size; i++)
	{
		inA[i] = inB[i] = rand->randomReal1();
	}

	fftw_plan_with_nthreads(QThread::idealThreadCount());
	fftw_plan planF = fftw_plan_dft_r2c_1d(size, inA, freqA, FFTW_ESTIMATE);
	ASSERT_TRUE(planF != NULL);
	fftw_plan_with_nthreads(QThread::idealThreadCount());
	fftw_plan planI = fftw_plan_dft_c2r_1d(size, freqA, outA, FFTW_ESTIMATE);
	ASSERT_TRUE(planI != NULL);
	fftw_execute(planF);
	fftw_execute(planI);
	for(i = 0; i < size; i++)
	{
		outA[i] /= static_cast<tfloat64>(size);
	}

	DFT_R2C_Full(size, inB, freqB);
	for(i = 0; i < size; i++)
	{
		EXPECT_NEAR(freqA[i][0], freqB[(i << 1) + 0], c_TOLERANCE);
		EXPECT_NEAR(freqA[i][1], freqB[(i << 1) + 1], c_TOLERANCE);
	}

	InverseDFT_C2R_Full(size, freqB, outB);
	for(i = 0; i < size; i++)
	{
		EXPECT_NEAR(outA[i], outB[i], c_TOLERANCE);
	}

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
