#include <cuda_runtime.h>

#include "common/inc/CommonTypes.h"
#include "engine/inc/FFTRadix2Cuda.h"

//-------------------------------------------------------------------------------------------

__global__ void kernelFFTRadix2_ReverseIndex(int noBits, int *reverseIndex)
{
	int idx = threadIdx.x + (blockIdx.x * blockDim.x);
	uint32_t y = static_cast<uint32_t>(idx), x = 0;
	
	while(noBits > 0)
	{
		x = (x << 1) | (y & 0x00000001);
		y >>= 1;
		noBits--;
	}
	reverseIndex[idx] = x;
}

//-------------------------------------------------------------------------------------------

__global__ void kernelFFTRadix2_Reverse(const double *x, double *y, int *reverseIndex)
{
	int idx = threadIdx.x + (blockIdx.x * blockDim.x);
	y[idx] = x[reverseIndex[idx]];
}

//-------------------------------------------------------------------------------------------

__global__ void kernelFFTRadix2_CalcCoefficients(int bitIndex, double *coeff)
{
	const tfloat64 c_PI_D_CUDA = 3.141592653589793238464264338832795;
	int M = 1 << bitIndex;
	int idx = threadIdx.x + (blockIdx.x * blockDim.x);
	double angle = (2.0 * c_PI_D_CUDA * static_cast<double>(idx)) / static_cast<double>(M);
	idx <<= 1;
	coeff[idx] = cos(angle);
	idx++;
	coeff[idx] = 0.0 - sin(angle);
}

//-------------------------------------------------------------------------------------------
/*
__global__ void kernelFFTRadix2_R2C_MultiCoefficient(double *F, const double *coeff)
{
	int idx = (threadIdx.x + (blockIdx.x * blockDim.x)) << 1;
	double X0, X1, Y0, Y1;
	X0 = coeff[idx + 0];
	X1 = coeff[idx + 1];
	Y0 = F[idx + 0];
	Y1 = F[idx + 1];
	F[idx + 0] = (X0 * Y0) - (X1 * Y1);
	F[idx + 1] = (X0 * Y1) + (X1 * Y0);
}

bitIndex = 4 -> FFT16 (N = 16, halfN = 8)
bitIndex = 5 -> FFT32 (N = 32, halfN = 16)
bitIndex = 6 -> FFT64 (N = 64, halfN = 32)

__global__ void kernelFFTRadix2_R2C_MultiCoefficient(int bitIndex, double *F, const double *coeff)
{
	int idx = threadIdx.x + (blockIdx.x * blockDim.x);
	int N = 1 << bitIndex;
	int halfN = N >> 1;
	int b = idx / halfN;
	int c = (idx % halfN) << 1;
	int i = ((b * N) + halfN + (idx % halfN)) << 1;

	double X0, X1, Y0, Y1;

	X0 = coeff[c + 0];
	X1 = coeff[c + 1];

	Y0 = F[i + 0];
	Y1 = F[i + 1];

	F[i + 0] = (X0 * Y0) - (X1 * Y1);
	F[i + 1] = (X0 * Y1) + (X1 * Y0);
}
*/
//-------------------------------------------------------------------------------------------

__global__ void kernelFFTRadix2_R2C_FFTN(int bitIndex, double *x, double *X, const double *coeff)
{
	int j = threadIdx.x + (blockIdx.x * blockDim.x);
	int N = 1 << bitIndex;
	int halfN = N >> 1;
	int b = j / halfN;
	int c = (j % halfN) << 1;
	int i = ((b * N) + halfN + (j % halfN)) << 1;

	double X0, X1, Y0, Y1, cR, cI;

	X0 = coeff[c + 0];
	X1 = coeff[c + 1];

	Y0 = x[i + 0];
	Y1 = x[i + 1];

	cR = (X0 * Y0) - (X1 * Y1);
	cI = (X0 * Y1) + (X1 * Y0);
	
	j = ((b * N) + (c >> 1)) << 1;

	X0 = x[j + 0];
	Y0 = x[j + 1];
	
	X[j + 0] = X0 + cR;
	X[j + 1] = Y0 + cI;
	X[i + 0] = X0 - cR;
	X[i + 1] = Y0 - cI;
}

//-------------------------------------------------------------------------------------------

__global__ void kernelFFTRadix2_R2C_FFT8(const double *xIn, double *Xout)
{
	const double c_halfSqr = 0.70710678118654752440084436210485;
	int idx = (threadIdx.x + (blockIdx.x * blockDim.x)) << 3;
	const double *x = &xIn[idx];
	double (*X)[2] = reinterpret_cast<double(*)[2]>(&Xout[idx << 1]);
	double A[4][2], F1[2], F2[2], F3[2], sA, sB;

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

int FFTRadix2Cuda_noBits(int N)
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

void FFTRadix2Cuda_ThreadDivision(int N, int& noBlocks, int& threadsPerBlock)
{
	if(N < 256)
	{
		noBlocks = 1;
		threadsPerBlock = N;
	}
	else
	{
		noBlocks = N >> 8;
		threadsPerBlock = 256;	
	}
}

//-------------------------------------------------------------------------------------------

void FFTRadix2Cuda_R2C_Free(FFTRadix2Cuda_R2C_Data *data)
{
	if(data != NULL)
	{
		if(data->coeff != NULL)
		{
			for(int i = 4; i <= data->noBits; i++)
			{
				if(data->coeff[i - 4] != NULL)
					cudaFree(data->coeff[i - 4]);
			}
			free(data->coeff);
		}
		if(data->stack != NULL)
		{
			for(int i = 0; i < 2; i++)
			{
				if(data->stack[i] != NULL)
					cudaFree(data->stack[i]);
			}
			free(data->stack);
		}
		if(data->xA != NULL)
			cudaFree(data->xA);
		if(data->xB != NULL)
			cudaFree(data->xB);
		if(data->reverseIndex != NULL)
			cudaFree(data->reverseIndex);
		free(data);
	}
}

//-------------------------------------------------------------------------------------------

#define __KERNEL_DEBUG_CUDA_MEMORY 1

template <typename T> void kernelDebugCUDAMemoryOmega(T *gMem, int len)
{
#if defined(__KERNEL_DEBUG_CUDA_MEMORY)
	T *cMem = new T [len];
	cudaMemcpy(cMem, gMem, len * sizeof(T), cudaMemcpyDeviceToHost);
	delete [] cMem;
#endif
}

FFTRadix2Cuda_R2C_Data *FFTRadix2Cuda_R2C_Init(int N)
{
	int noBlocks, threadsPerBlock, nBits;
	FFTRadix2Cuda_R2C_Data *data;
	cudaError_t res;
	
	if(N < 16)
	{
		return NULL;
	}
	nBits = FFTRadix2Cuda_noBits(N);
	if(N != (1 << nBits))
	{
		return NULL;
	}
	
	data = static_cast<FFTRadix2Cuda_R2C_Data *>(calloc(1, sizeof(FFTRadix2Cuda_R2C_Data)));
	if(data == NULL)
	{
		return NULL;
	}
	
	data->N = N;
	data->noBits = nBits;
	
	if(cudaMalloc(&(data->reverseIndex), N * sizeof(int)) != cudaSuccess)
	{
		FFTRadix2Cuda_R2C_Free(data);
		return NULL;
	}
	FFTRadix2Cuda_ThreadDivision(N, noBlocks, threadsPerBlock);
	kernelFFTRadix2_ReverseIndex<<<noBlocks, threadsPerBlock>>>(nBits, data->reverseIndex);
	kernelDebugCUDAMemoryOmega<int>(data->reverseIndex, N);
	
	if(cudaMalloc(&(data->xA), N * sizeof(double)) != cudaSuccess)
	{
		FFTRadix2Cuda_R2C_Free(data);
		return NULL;
	}
	if(cudaMalloc(&(data->xB), N * sizeof(double)) != cudaSuccess)
	{
		FFTRadix2Cuda_R2C_Free(data);
		return NULL;
	}
	
	data->coeff = static_cast<double **>(calloc(nBits - 3, sizeof(double *)));
	data->stack = static_cast<double **>(calloc(2, sizeof(double *)));
	if(data->coeff == NULL || data->stack == NULL)
	{
		FFTRadix2Cuda_R2C_Free(data);
		return NULL;	
	}
	
	res = cudaSuccess;
	for(int i = 4; i <= nBits && res == cudaSuccess; i++)
	{
		int len, M;
		double *c;
		
		M = 1 << i;
		len = M >> 1;
		res = cudaMalloc(&c, 2 * len * sizeof(double));
		if(res == cudaSuccess)
		{
			FFTRadix2Cuda_ThreadDivision(len, noBlocks, threadsPerBlock);
			kernelFFTRadix2_CalcCoefficients<<<noBlocks, threadsPerBlock>>>(i, c);
			kernelDebugCUDAMemoryOmega<double>(c, len * 2);
			data->coeff[i - 4] = c;
		}
	}
	for(int i = 0; i < 2 && res == cudaSuccess; i++)
	{
		double *s;
		res = cudaMalloc(&s, 2 * N * sizeof(double));
		if(res == cudaSuccess)
		{
			data->stack[i] = s;
		}
	}
	
	if(res != cudaSuccess)
	{
		FFTRadix2Cuda_R2C_Free(data);
		return NULL;	
	}
	return data;
}

//-------------------------------------------------------------------------------------------

bool FFTRadix2Cuda_R2C_DFT(const double *x, double *X, FFTRadix2Cuda_R2C_Data *data)
{
	int noBlocks, threadsPerBlock, inIdx, outIdx;

	if(x == NULL || X == NULL || data == NULL)
		return false;
	
	if(cudaMemcpy(data->xA, x, data->N * sizeof(double), cudaMemcpyHostToDevice) != cudaSuccess)
		return false;
	
	FFTRadix2Cuda_ThreadDivision(data->N, noBlocks, threadsPerBlock);	
	kernelFFTRadix2_Reverse<<<noBlocks, threadsPerBlock>>>(data->xA, data->xB, data->reverseIndex);
	kernelDebugCUDAMemoryOmega<double>(data->xA, data->N);
	kernelDebugCUDAMemoryOmega<double>(data->xB, data->N);
	
	FFTRadix2Cuda_ThreadDivision(data->N >> 3, noBlocks, threadsPerBlock);
	kernelFFTRadix2_R2C_FFT8<<<noBlocks, threadsPerBlock>>>(data->xB, data->stack[0]);
	kernelDebugCUDAMemoryOmega<double>(data->xB, data->N);
	kernelDebugCUDAMemoryOmega<double>(data->stack[0], data->N << 1);
	
	inIdx = 0;
	outIdx = 0;
	for(int bits = 4; bits <= data->noBits; bits++)
	{
		outIdx++;
		outIdx &= 0x1;
		FFTRadix2Cuda_ThreadDivision(data->N >> 1, noBlocks, threadsPerBlock);
		kernelFFTRadix2_R2C_FFTN<<<noBlocks, threadsPerBlock>>>(bits, data->stack[inIdx], data->stack[outIdx], data->coeff[bits - 4]);
		kernelDebugCUDAMemoryOmega<double>(data->stack[inIdx], data->N << 1);
		kernelDebugCUDAMemoryOmega<double>(data->stack[outIdx], data->N << 1);
		inIdx++;
		inIdx &= 0x1;
	}
	
	if(cudaMemcpy(X, data->stack[outIdx], ((data->N / 2) + 1) * 2 * sizeof(double), cudaMemcpyDeviceToHost) != cudaSuccess)
		return false;
	
	return true;
}

//-------------------------------------------------------------------------------------------
// Beginning of GPU Architecture definitions
//-------------------------------------------------------------------------------------------

int _ConvertSMVer2Cores(int major, int minor) 
{
	// Defines for GPU Architecture types (using the SM version to determine
	// the # of cores per SM
	typedef struct {
		int SM;  // 0xMm (hexidecimal notation), M = SM Major version,
		// and m = SM minor version
		int Cores;
	} sSMtoCores;

	sSMtoCores nGpuArchCoresPerSM[] = {
		{0x30, 192},
		{0x32, 192},
		{0x35, 192},
		{0x37, 192},
		{0x50, 128},
		{0x52, 128},
		{0x53, 128},
		{0x60,  64},
		{0x61, 128},
		{0x62, 128},
		{0x70,  64},
		{0x72,  64},
		{0x75,  64},
		{0x80,  64},
		{0x86, 128},
		{0x87, 128},
		{0x89, 128},
		{0x90, 128},
		{0xa0, 128},
		{0xa1, 128},
		{0xa3, 128},
		{0xb0, 128},
		{0xc0, 128},
		{0xc1, 128},
		{-1, -1}
	};

	int index = 0;
	
	while (nGpuArchCoresPerSM[index].SM != -1) 
	{
		if (nGpuArchCoresPerSM[index].SM == ((major << 4) + minor)) 
		{
			return nGpuArchCoresPerSM[index].Cores;
		}
		index++;
	}

	return nGpuArchCoresPerSM[index - 1].Cores;
}

//-------------------------------------------------------------------------------------------

int initCUDAOmega()
{
	cudaError_t res;
	int currentDevice, deviceCount;
	tuint64 maxComputePerf = 0;
	int devID = -1;
	
	deviceCount = 0;
	if(cudaGetDeviceCount(&deviceCount) != cudaSuccess)
		return -1;
	
	for(currentDevice = 0; currentDevice < deviceCount; currentDevice++)
	{
		int computeMode = -1, major = 0, minor = 0;
		int smPerMultiproc;
		
		res = cudaDeviceGetAttribute(&computeMode, cudaDevAttrComputeMode, currentDevice);
		if(res != cudaSuccess || computeMode == cudaComputeModeProhibited)
			continue;
		res = cudaDeviceGetAttribute(&major, cudaDevAttrComputeCapabilityMajor, currentDevice);
		if(res != cudaSuccess)
			continue;
		res = cudaDeviceGetAttribute(&minor, cudaDevAttrComputeCapabilityMinor, currentDevice);
		if(res != cudaSuccess)
			continue;
		
		smPerMultiproc = (major == 9999 && minor == 9999) ? 1 : _ConvertSMVer2Cores(major,  minor);
		
		int multiProcessorCount = 0, clockRate = 0;
		res = cudaDeviceGetAttribute(&multiProcessorCount, cudaDevAttrMultiProcessorCount, currentDevice);
		if(res != cudaSuccess)
			continue;
		
		res = cudaDeviceGetAttribute(&clockRate, cudaDevAttrClockRate, currentDevice);
		if(res != cudaSuccess)
		{
			if(res == cudaErrorInvalidValue)
			{
				clockRate = 1;
			}
			else
			{
				continue;
			}
		}
		
		tuint64 computePerf = static_cast<tuint64>(multiProcessorCount) * static_cast<tuint64>(smPerMultiproc) * static_cast<tuint64>(clockRate);
		if(computePerf > maxComputePerf)
		{
			maxComputePerf = computePerf;
			devID = currentDevice;
		}
	}
	if(devID >= 0)
	{
		res = cudaSetDevice(devID);
		if(res != cudaSuccess)
			devID = -1;
	}
	return devID;
}

//-------------------------------------------------------------------------------------------
