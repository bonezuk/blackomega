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

__global__ void kernelFFTRadix2_Reverse(const double *x, double *y, const int *reverseIndex)
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
// i = 0, 1, 2, 3, 4, 5, 6, 7
// idx0 = 8 , 7, 8-(0+1)
// idx1 = 9 , 6
// idx2 = 10, 5
// idx3 = 11, 4
// idx4 = 12, 3
// idx5 = 13, 2
// idx6 = 14, 1
// idx7 = 15, 0, 8-(7+1)
// N = 16
//-------------------------------------------------------------------------------------------

__global__ void kernelFFTRadix2_C2R_UnpackInput(const double *in, double *x, int N)
{
	
	int j;
	int i = threadIdx.x + (blockIdx.x * blockDim.x);
	double xI, xR;

	j = (N >> 1) + 1;
	if(i < j)
	{
		j = i << 1;
		xR = in[j + 0];
		xI = in[j + 1];

		x[j + 0] = xR;
		x[j + 1] = xI;

		if(i > 0)
		{
			j = N - i;
			if(j > (N >> 1))
			{
				j <<= 1;
				x[j + 0] = xR;
				x[j + 1] = 0.0 - xI;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------

__global__ void kernelFFTRadix2_C2R_FFTN(int bitIndex, double *x, double *X, const double *coeff)
{
	int j = threadIdx.x + (blockIdx.x * blockDim.x);
	int N = 1 << bitIndex;
	int halfN = N >> 1;
	int b = j / halfN;
	int c = (j % halfN) << 1;
	int i = ((b * N) + halfN + (j % halfN)) << 1;
	
	double X0, X1, Y0, Y1;

	j = ((b * N) + (c >> 1)) << 1;

	X0 = x[j + 0];
	X1 = x[j + 1];

	Y0 = x[i + 0];
	Y1 = x[i + 1];

	X[j + 0] = X0 + Y0;
	X[j + 1] = X1 + Y1;
	
	X0 -= Y0;
	X1 -= Y1;
	
	Y0 = coeff[c + 0];
	Y1 = coeff[c + 1];

	X[i + 0] = (X0 * Y0) - (X1 * Y1);
	X[i + 1] = (X0 * Y1) + (X1 * Y0);
}

//-------------------------------------------------------------------------------------------

__global__ void kernelFFTRadix2_C2R_FFT8(const double *xIn, double *X)
{
	const double c_halfSqr = 0.70710678118654752440084436210485;
	int idx = (threadIdx.x + (blockIdx.x * blockDim.x)) << 3;
	const double(*x)[2] = reinterpret_cast<const double(*)[2]>(&xIn[idx << 1]);
	double t[4][2], s[4], aR, aI;
	
	X = &X[idx];

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
// N = 16
// 0 = 16 - 0 = 15
// 1 = 16 - 1 = 14
// 2 = 16 - 2 = 13
// ...
// 14 = 16 - 14 = 2
// 15 = 16 - 15 = 1
//-------------------------------------------------------------------------------------------

__global__ void kernelFFTRadix2_C2R_ReorderAndNormalise(const tfloat64 *x, tfloat64 *y, const int *reverseIndex, int N, double invN)
{
	int i = threadIdx.x + (blockIdx.x * blockDim.x);
	int j = (i) ? N - i : 0;
	y[i] = x[reverseIndex[j]] * invN;
}

//-------------------------------------------------------------------------------------------

__global__ void kernelFFTRadix2_R2C_OctaveUpscaleReverse(const tfloat64 *x, tfloat64 *y, const int *reverseIndex)
{
	int i = threadIdx.x + (blockIdx.x * blockDim.x);
	y[i] = x[reverseIndex[i << 1]];
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

void FFTRadix2Cuda_Free(FFTRadix2Cuda_Data *data)
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

FFTRadix2Cuda_Data *FFTRadix2Cuda_Init(int N)
{
	int noBlocks, threadsPerBlock, nBits;
	FFTRadix2Cuda_Data *data;
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
	
	data = static_cast<FFTRadix2Cuda_Data *>(calloc(1, sizeof(FFTRadix2Cuda_Data)));
	if(data == NULL)
	{
		return NULL;
	}
	
	data->N = N;
	data->noBits = nBits;
	
	if(cudaMalloc(&(data->reverseIndex), N * sizeof(int)) != cudaSuccess)
	{
		FFTRadix2Cuda_Free(data);
		return NULL;
	}
	Omega1DCuda_ThreadDivision(N, noBlocks, threadsPerBlock);
	kernelFFTRadix2_ReverseIndex<<<noBlocks, threadsPerBlock>>>(nBits, data->reverseIndex);
	omegaDebugCUDAMemoryOmega<int>(data->reverseIndex, N);
	
	if(cudaMalloc(&(data->xA), 2 * N * sizeof(double)) != cudaSuccess)
	{
		FFTRadix2Cuda_Free(data);
		return NULL;
	}
	if(cudaMalloc(&(data->xB), 2 * N * sizeof(double)) != cudaSuccess)
	{
		FFTRadix2Cuda_Free(data);
		return NULL;
	}
	
	data->coeff = static_cast<double **>(calloc(nBits - 3, sizeof(double *)));
	data->stack = static_cast<double **>(calloc(2, sizeof(double *)));
	if(data->coeff == NULL || data->stack == NULL)
	{
		FFTRadix2Cuda_Free(data);
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
			Omega1DCuda_ThreadDivision(len, noBlocks, threadsPerBlock);
			kernelFFTRadix2_CalcCoefficients<<<noBlocks, threadsPerBlock>>>(i, c);
			omegaDebugCUDAMemoryOmega<double>(c, len * 2);
			data->coeff[i - 4] = c;
		}
	}
	for(int i = 0; i < 2 && res == cudaSuccess; i++)
	{
		double *s;
		// The additional entry N+1 allows for expansion in octave upscaling.
		res = cudaMalloc(&s, 2 * (N + 1) * sizeof(double));
		if(res == cudaSuccess)
		{
			data->stack[i] = s;
		}
	}
	
	if(res != cudaSuccess)
	{
		FFTRadix2Cuda_Free(data);
		return NULL;	
	}
	return data;
}

//-------------------------------------------------------------------------------------------

int FFTRadix2Cuda_R2C_DFT_OnDevice(double *in, FFTRadix2Cuda_Data *data)
{
	int noBlocks, threadsPerBlock, inIdx, outIdx;

	if(in == NULL || data == NULL)
		return -1;
	
	Omega1DCuda_ThreadDivision(data->N, noBlocks, threadsPerBlock);	
	kernelFFTRadix2_Reverse<<<noBlocks, threadsPerBlock>>>(in, data->xB, data->reverseIndex);
	omegaDebugCUDAMemoryOmega<double>(in, data->N);
	omegaDebugCUDAMemoryOmega<double>(data->xB, data->N);
	
	Omega1DCuda_ThreadDivision(data->N >> 3, noBlocks, threadsPerBlock);
	kernelFFTRadix2_R2C_FFT8<<<noBlocks, threadsPerBlock>>>(data->xB, data->stack[0]);
	omegaDebugCUDAMemoryOmega<double>(data->xB, data->N);
	omegaDebugCUDAMemoryOmega<double>(data->stack[0], data->N << 1);
	
	inIdx = 0;
	outIdx = 0;
	for(int bits = 4; bits <= data->noBits; bits++)
	{
		outIdx++;
		outIdx &= 0x1;
		Omega1DCuda_ThreadDivision(data->N >> 1, noBlocks, threadsPerBlock);
		kernelFFTRadix2_R2C_FFTN<<<noBlocks, threadsPerBlock>>>(bits, data->stack[inIdx], data->stack[outIdx], data->coeff[bits - 4]);
		omegaDebugCUDAMemoryOmega<double>(data->stack[inIdx], data->N << 1);
		omegaDebugCUDAMemoryOmega<double>(data->stack[outIdx], data->N << 1);
		inIdx++;
		inIdx &= 0x1;
	}
	return outIdx;
}

//-------------------------------------------------------------------------------------------

bool FFTRadix2Cuda_R2C_DFT(const double *x, double *X, FFTRadix2Cuda_Data *data)
{
	int outIdx;

	if(x == NULL || X == NULL || data == NULL)
		return false;

	if(cudaMemcpy(data->xA, x, data->N * sizeof(double), cudaMemcpyHostToDevice) != cudaSuccess)
		return false;
	
	outIdx = FFTRadix2Cuda_R2C_DFT_OnDevice(data->xA, data);
	if(outIdx < 0)
		return false;

	if(cudaMemcpy(X, data->stack[outIdx], ((data->N / 2) + 1) * 2 * sizeof(double), cudaMemcpyDeviceToHost) != cudaSuccess)
		return false;

	return true;
}

//-------------------------------------------------------------------------------------------

bool FFTRadix2Cuda_C2R_iDFT_OnDevice(const double *in, FFTRadix2Cuda_Data *data)
{
	int noBlocks, threadsPerBlock, inIdx, outIdx;

	Omega1DCuda_ThreadDivision((data->N >> 1) + 1, noBlocks, threadsPerBlock);
	kernelFFTRadix2_C2R_UnpackInput<<<noBlocks, threadsPerBlock>>>(in, data->stack[0], data->N);
	omegaDebugCUDAMemoryOmega<double>(data->stack[0], data->N << 1);
	
	inIdx = 0;
	outIdx = 0;
	for(int bits = data->noBits; bits >= 4; bits--)
	{
		outIdx++;
		outIdx &= 0x1;
		Omega1DCuda_ThreadDivision(data->N >> 1, noBlocks, threadsPerBlock);
		kernelFFTRadix2_C2R_FFTN<<<noBlocks, threadsPerBlock>>>(bits, data->stack[inIdx], data->stack[outIdx], data->coeff[bits - 4]);
		omegaDebugCUDAMemoryOmega<double>(data->stack[inIdx], data->N << 1);
		omegaDebugCUDAMemoryOmega<double>(data->stack[outIdx], data->N << 1);
		inIdx++;
		inIdx &= 0x1;
	}
	
	Omega1DCuda_ThreadDivision(data->N >> 3, noBlocks, threadsPerBlock);
	kernelFFTRadix2_C2R_FFT8<<<noBlocks, threadsPerBlock>>>(data->stack[outIdx], data->xA);
	omegaDebugCUDAMemoryOmega<double>(data->stack[outIdx], data->N << 1);
	omegaDebugCUDAMemoryOmega<double>(data->xA, data->N);
	
	Omega1DCuda_ThreadDivision(data->N, noBlocks, threadsPerBlock);
	kernelFFTRadix2_C2R_ReorderAndNormalise<<<noBlocks, threadsPerBlock>>>(data->xA, data->xB, data->reverseIndex, data->N, 1.0 / static_cast<double>(data->N));
	omegaDebugCUDAMemoryOmega<double>(data->xA, data->N);
	omegaDebugCUDAMemoryOmega<double>(data->xB, data->N);
	
	return true;
}

//-------------------------------------------------------------------------------------------

bool FFTRadix2Cuda_C2R_iDFT(const double *x, double *X, FFTRadix2Cuda_Data *data)
{
	int inSize;
	
	if(x == NULL || X == NULL || data == NULL)
		return false;
	
	inSize = (data->N / 2) + 1;
	if(cudaMemcpy(data->xA, x, 2 * inSize * sizeof(double), cudaMemcpyHostToDevice) != cudaSuccess)
		return false;
	
	if(!FFTRadix2Cuda_C2R_iDFT_OnDevice(data->xA, data))
		return false;
	
	if(cudaMemcpy(X, data->xB, data->N * sizeof(double), cudaMemcpyDeviceToHost) != cudaSuccess)
		return false;
	
	return true;
}

//-------------------------------------------------------------------------------------------

int FFTRadix2Cuda_R2C_OctaveUpscale_DFT_OnDevice(const double *x, FFTRadix2Cuda_Data *data)
{
	int noBlocks, threadsPerBlock, inIdx, outIdx;

	if(x == NULL || data == NULL)
		return -1;
	
	Omega1DCuda_ThreadDivision(data->N >> 1, noBlocks, threadsPerBlock);	
	kernelFFTRadix2_R2C_OctaveUpscaleReverse<<<noBlocks, threadsPerBlock>>>(x, data->xB, data->reverseIndex);
	omegaDebugCUDAMemoryOmega<double>(x, data->N);
	omegaDebugCUDAMemoryOmega<double>(data->xB, data->N);
	
	Omega1DCuda_ThreadDivision(data->N >> 4, noBlocks, threadsPerBlock);
	kernelFFTRadix2_R2C_FFT8<<<noBlocks, threadsPerBlock>>>(data->xB, data->stack[0]);
	omegaDebugCUDAMemoryOmega<double>(data->xB, data->N);
	omegaDebugCUDAMemoryOmega<double>(data->stack[0], data->N << 1);
	
	inIdx = 0;
	outIdx = 0;
	for(int bits = 4; bits <= data->noBits - 1; bits++)
	{
		outIdx++;
		outIdx &= 0x1;
		Omega1DCuda_ThreadDivision(data->N >> 2, noBlocks, threadsPerBlock);
		kernelFFTRadix2_R2C_FFTN<<<noBlocks, threadsPerBlock>>>(bits, data->stack[inIdx], data->stack[outIdx], data->coeff[bits - 4]);
		omegaDebugCUDAMemoryOmega<double>(data->stack[inIdx], data->N << 1);
		omegaDebugCUDAMemoryOmega<double>(data->stack[outIdx], data->N << 1);
		inIdx++;
		inIdx &= 0x1;
	}
	return outIdx;
}

//-------------------------------------------------------------------------------------------

bool FFTRadix2Cuda_R2C_OctaveUpscale_DFT(const double *x, double *X, FFTRadix2Cuda_Data *data)
{
	int outIdx;

	if(x == NULL || X == NULL || data == NULL)
		return false;
	
	if(cudaMemcpy(data->xA, x, (data->N >> 1) * sizeof(double), cudaMemcpyHostToDevice) != cudaSuccess)
		return false;
	
	outIdx = FFTRadix2Cuda_R2C_OctaveUpscale_DFT_OnDevice(data->xA, data);

	if(cudaMemcpy(X, data->stack[outIdx], (data->N / 2) * 2 * sizeof(double), cudaMemcpyDeviceToHost) != cudaSuccess)
		return false;
	X[data->N] = X[0];
	X[data->N + 1] = X[1];

	return true;
}

//-------------------------------------------------------------------------------------------