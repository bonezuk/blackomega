#include "gtest/gtest.h"

#include "common/inc/Random.h"
#include "engine/inc/FFTRadix2_R2C.h"
#include "engine/inc/FFTRadix2Cuda.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

class FFTRadix2CudaTest : public ::testing::Test 
{
protected:
    static void SetUpTestSuite() {
        cuda_init_result_ = initCUDAOmega();

        if (cuda_init_result_ >= 0) {
            std::cout << "[INFO] CUDA device initialized successfully (result = "
                      << cuda_init_result_ << ")." << std::endl;
        } else {
            std::cout << "[WARN] No CUDA device available (initCUDAOmega() returned "
                      << cuda_init_result_ << "). CUDA tests will be skipped." << std::endl;
        }
    }

    void SetUp() override {
        if (cuda_init_result_ < 0) {
            GTEST_SKIP() << "CUDA device not available or initialization failed "
                         << "(initCUDAOmega() returned " << cuda_init_result_ << ")";
        }
    }

    static void TearDownTestSuite() {}

private:
    static int cuda_init_result_;
};

int FFTRadix2CudaTest::cuda_init_result_ = -1;

//-------------------------------------------------------------------------------------------

void testFFTRadix2Cuda(int N)
{
	int NOut = (N / 2) + 1;

	tfloat64 *inA = new tfloat64 [N];
	tfloat64 *inB = new tfloat64 [N];
	tfloat64 *outA = new tfloat64 [NOut * 2];
	tfloat64 *outB = new tfloat64 [NOut * 2];
	
	for(int i = 0; i < N; i++)
	{
		inA[i] = rand->randomReal1();
		inB[i] = inA[i];
	}
	
	engine::FFTRadix2_R2C cpuFFT;
	ASSERT_TRUE(cpuFFT.init(N));
	cpuFFT.DFT(inA, outA);
	
	FFTRadix2Cuda_R2C_Data *gpuFFT = FFTRadix2Cuda_R2C_Init(N);
	FFTRadix2Cuda_R2C_DFT(inB, outB);
	FFTRadix2Cuda_R2C_Free(gpuFFT);
	
	for(int i = 0; i < NOut * 2; i++)
	{
		EXPECT_NEAR(outA[i], outB[i], c_TOLERANCE);
	}
	
	delete [] inA;
	delete [] inB;
	delete [] outA;
	delete [] outB;
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT16)
{
	testFFTRadix2Cuda(16);
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT32)
{
	testFFTRadix2Cuda(32);
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT64)
{
	testFFTRadix2Cuda(64);
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT128)
{
	testFFTRadix2Cuda(128);
}

//-------------------------------------------------------------------------------------------
TEST_F(FFTRadix2CudaTest, DFT256)
{
	testFFTRadix2Cuda(256);
}

//-------------------------------------------------------------------------------------------
TEST_F(FFTRadix2CudaTest, DFT512)
{
	testFFTRadix2Cuda(512);
}

//-------------------------------------------------------------------------------------------
TEST_F(FFTRadix2CudaTest, DFT1024)
{
	testFFTRadix2Cuda(1024);
}

//-------------------------------------------------------------------------------------------
TEST_F(FFTRadix2CudaTest, DFT2048)
{
	testFFTRadix2Cuda(2048);
}

//-------------------------------------------------------------------------------------------
TEST_F(FFTRadix2CudaTest, DFT4096)
{
	testFFTRadix2Cuda(4096);
}

//-------------------------------------------------------------------------------------------
TEST_F(FFTRadix2CudaTest, DFT8192)
{
	testFFTRadix2Cuda(8192);
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT16384)
{
	testFFTRadix2Cuda(16384);
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT32768)
{
	testFFTRadix2Cuda(32768);
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT65536)
{
	testFFTRadix2Cuda(65536);
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT131072)
{
	testFFTRadix2Cuda(131072);
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT262144)
{
	testFFTRadix2Cuda(262144);
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT524288)
{
	testFFTRadix2Cuda(524288);
}

//-------------------------------------------------------------------------------------------

TEST_F(FFTRadix2CudaTest, DFT1048576)
{
	testFFTRadix2Cuda(1048576);
}

//-------------------------------------------------------------------------------------------
