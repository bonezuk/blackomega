#include "gtest/gtest.h"
#include "fftw3.h"
#include <QThread>

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
