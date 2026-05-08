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

bool FFTRadix2Base::init(int N)
{
	int nBits;
		
	nBits = noBits(N);

	if(N < 16)
		return false;
	m_N = N;
	if(m_N != (1 << nBits))
		return false;

	m_reverseIndex = static_cast<int *>(malloc(m_N * sizeof(int)));
	if(m_reverseIndex == NULL)
		return false;
	for(int i = 0; i < m_N; i++)
	{
		m_reverseIndex[i] = getReverseIndex(i, nBits);
	}

	m_xIO = static_cast<tfloat64 *>(malloc(m_N * sizeof(tfloat64)));
	if(m_xIO == NULL)
		return false;

	m_coeff = static_cast<tfloat64 **>(calloc(nBits - 3, sizeof(tfloat64 *)));
	if(m_coeff == NULL)
		return false;
	m_stack = static_cast<tfloat64 **>(calloc(nBits - 3, sizeof(tfloat64 *)));
	if(m_stack == NULL)
		return false;

	for(int i = 4; i <= nBits; i++)
	{
		int M = 1 << i;
		int len = M >> 1;
		tfloat64 *c = static_cast<tfloat64 *>(malloc(2 * len * sizeof(tfloat64)));
		if(c == NULL)
			return false;
		m_coeff[i - 4] = c;

		for(int j = 0; j < len; j++)
		{
			tfloat64 angle = (2.0 * c_PI_D * static_cast<tfloat64>(j)) / static_cast<tfloat64>(M);
			c[(j << 1) + 0] = cos(angle);
			c[(j << 1) + 1] = 0.0 - sin(angle);
		}
		m_stack[i - 4] = static_cast<tfloat64 *>(malloc(m_N * 2 * sizeof(tfloat64)));
		if(m_stack[i - 4] == NULL)
			return false;
	}

	return true;
}

int FFTRadix2Base::getReverseIndex(int index, int noBits) const
{
	tuint32 y = static_cast<tuint32>(index), x = 0;

	while(noBits > 0)
	{
		x = (x << 1) | (y & 0x00000001);
		y >>= 1;
		noBits--;
	}
	return static_cast<tint>(x);
}

void FFTRadix2Base::reverse(const tfloat64 *x, tfloat64 *y) const
{
	for(int i = 0; i < m_N; i++)
	{
		y[i] = x[m_reverseIndex[i]];
	}
}
//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

typedef struct s_FFTRadix2Cuda_R2C_Data
{
	int N;
	int noBits;
	int *reverseIndex;
	double *xA;
	double *xB;
	double **coeff;
	double **stack;
} FFTRadix2Cuda_R2C_Data;

__global__ void kernelFFTRadix2_ReverseIndex(int noBits, int *reverseIndex)
{
	int idx = threadIdx.x + (blockIdx.x * blockDim.x);
	uint32_t y = static_cast<uint32_t>(idx), x = 0;
	
	while(noBits > 0)
	{
		x = (x << 1) | (y & 0x00000001)
		y >>= 1;
		noBits--;
	}
	reverseIndex[idx] = y;
}

__global__ void kernelFFTRadix2_Reverse(const double *x, double *y, int *reverseIndex)
{
	int idx = threadIdx.x + (blockIdx.x * blockDim.x);
	y[idx] = x[reverseIndex[idx]];
}

__global__ void kernelFFTRadix2_CalcCoefficients(int bitIndex, double *coeff)
{
	int M = 1 << bitIndex;
	int idx = threadIdx.x + (blockIdx.x * blockDim.x);
	double angle = (2.0 * c_PI_D * static_cast<double>(idx)) / static_cast<double>(M);
	idx <<= 1;
	coeff[idx] = cos(angle);
	idx++;
	coeff[idx] = 0.0 - sin(angle);
}

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
*/

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

int FFTRadix2Cuda_noBits(int N) const
{
	int count = 0;

	while(N > 1)
	{
		N >>= 1;
		count++;
	}
	return count;
}

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

void FFTRadix2Cuda_R2C_Free(FFTRadix2Cuda_R2C_Data *data)
{
	if(data != NULL)
	{
		if(data->xA != NULL)
			cudaFree(data->xA);
		if(data->xB != NULL)
			cudaFree(data->xB);
		if(data->reverseIndex != NULL)
			cudaFree(data->reverseIndex);
		free(data);
	}
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
	data->stack = static_cast<double **>(calloc(nBits - 3, sizeof(double *)));
	if(data->coeff == NULL || data->stack == NULL)
	{
		FFTRadix2Cuda_R2C_Free(data);
		return NULL;	
	}
	
	res = cudaSuccess
	for(int i = 4; i <= nBits && res == cudaSuccess; i++)
	{
		int len, M;
		double *c, *s;
		
		M = 1 << i;
		len = M >> 1;
		res = cudaMalloc(&c, 2 * len * sizeof(double));
		if(res == cudaSuccess)
		{
			FFTRadix2Cuda_ThreadDivision(len, noBlocks, threadsPerBlock);
			kernelFFTRadix2_CalcCoefficients<<<noBlocks, threadsPerBlock>>>(i, c);
			data->coeff[i - 4] = c;
			
			res = cudaMalloc(&s, 2 * N * sizeof(double));
			if(res == cudaSuccess)
			{
				data->stack[i - 4] = s;
			}
		}
	}
	
	if(res != cudaSuccess)
	{
		FFTRadix2Cuda_R2C_Free(data);
		return NULL;	
	}
	return data;
}

bool FFTRadix2Cuda_R2C_DFT(const double *x, double *X, FFTRadix2Cuda_R2C_Data *data)
{
	int noBlocks, threadsPerBlock;

	if(x == NULL || X == NULL || data == NULL)
		return false;
	
	if(cudaMemcpy(data->xA, x, data->N * sizeof(double), cudaMemcpyHostToDevice) != cudaSuccess)
		return false;
	
	FFTRadix2Cuda_ThreadDivision(data->N, noBlocks, threadsPerBlock);	
	kernelFFTRadix2_Reverse<<<noBlocks, threadsPerBlock>>>(data->xA, data->xB, data->reverseIndex);
	
	FFTRadix2Cuda_ThreadDivision(data->N >> 3, noBlocks, threadsPerBlock);
	kernelFFTRadix2_R2C_FFT8<<<noBlocks, threadsPerBlock>>>(data->xB, data->stack[0]);
	
	for(int bits = 4; bits <= data->noBits; bits++)
	{
		int bIndex = bits - 4;
		FFTRadix2Cuda_ThreadDivision(data->N >> 1, noBlocks, threadsPerBlock);
		kernelFFTRadix2_R2C_MultiCoefficient<<<noBlocks, threadsPerBlock>>>(bits, data->stack[bits - 4], data->coeff[bits - 4]);
	}
}
