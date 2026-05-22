#include <cuda_runtime.h>

#include "common/inc/CommonTypes.h"
#include "engine/inc/FFTRadix2Cuda.h"
#include "engine/inc/FIRConvolutionAddOverlapCUDA.h"

//-------------------------------------------------------------------------------------------

__global__ void kernelFIRComplexMultiplication(const double *firH, double *Xio)
{
    int i = threadIdx.x + (blockIdx.x * blockDim.x);
    double (*X)[2] = reinterpret_cast<double(*)[2]>(Xio);
    const double (*H)[2] = reinterpret_cast<const double(*)[2]>(firH);
    double Xr = X[i][0];
    double Hr = H[i][0];
    double Xi = X[i][1];
    double Hi = H[i][1];
    X[i][0] = (Xr * Hr) - (Xi * Hi);
    X[i][1] = (Xr * Hi) + (Xi * Hr);
}

//-------------------------------------------------------------------------------------------

__global__ void kernelFIRAddOverlap(const double *olap, double *y, int M)
{
    int idx = threadIdx.x + (blockIdx.x * blockDim.x);
    if(idx < (M - 1))
    {
        y[idx] += olap[idx];
    }
}

//-------------------------------------------------------------------------------------------

__global__ void kernelFIRSaveOverlap(double *olap, const double *y, int M, int N)
{
    int j = threadIdx.x + (blockIdx.x * blockDim.x);
    int i = (N + 1 - M) + j;
    if(i < N)
    {
        olap[j] = y[i];
    }
}

//-------------------------------------------------------------------------------------------

void FIRConvAddOverlapCUDA_Free(FIRConvAddOverlapCuda_Data *data)
{
    if(data != NULL)
    {
        if(data->firH != NULL)
        {
            cudaFree(data->firH);
        }
        if(data->in != NULL)
        {
            cudaFree(data->in);
        }
        if(data->olap != NULL)
        {
            cudaFree(data->olap);
        }
        if(data->FFT != NULL)
        {
            FFTRadix2Cuda_Free(data->FFT);
        }
        if(data->iFFT != NULL)
        {
            FFTRadix2Cuda_Free(data->iFFT);
        }
        free(data);
    }
}

//-------------------------------------------------------------------------------------------

FIRConvAddOverlapCuda_Data *FIRConvAddOverlapCUDA_Init(const double *firCoeff, int firSize, int outputSize)
{
    int outIdx;
    FIRConvAddOverlapCuda_Data *data;

    data = static_cast<FIRConvAddOverlapCuda_Data *>(calloc(1, sizeof(FIRConvAddOverlapCuda_Data)));
    if(data == NULL)
    {
        return NULL;
    }

    data->M = firSize;
    data->L = outputSize;
    data->N = data->L + data->M - 1;
    data->Nout = (data->N / 2) + 1;

    if(cudaMalloc(&data->firH, data->Nout * 2 * sizeof(double)) != cudaSuccess)
        goto firinit_error;
    if(cudaMalloc(&data->in, data->N * sizeof(double)) != cudaSuccess)
        goto firinit_error;

    data->FFT  = FFTRadix2Cuda_Init(data->N);
    data->iFFT = FFTRadix2Cuda_Init(data->N);
    if(data->FFT == NULL || data->iFFT == NULL)
        goto firinit_error;

    if(cudaMemcpy(data->in, firCoeff, data->M * sizeof(double), cudaMemcpyHostToDevice) != cudaSuccess)
        goto firinit_error;
    if(cudaMemset(&(data->in[data->M]), 0, (data->N - data->M) * sizeof(double)) != cudaSuccess)
        goto firinit_error;
    outIdx = FFTRadix2Cuda_R2C_DFT_OnDevice(data->in, data->FFT);
    if(outIdx < 0)
        goto firinit_error;
    omegaDebugCUDAMemoryOmega<double>(data->FFT->stack[outIdx], data->Nout * 2);
    if(cudaMemcpy(data->firH, data->FFT->stack[outIdx], data->Nout * 2 * sizeof(double), cudaMemcpyDeviceToDevice) != cudaSuccess)
        goto firinit_error;

    if(cudaMalloc(&data->olap, (data->M - 1) * sizeof(double)) != cudaSuccess)
        goto firinit_error;
    if(cudaMemset(data->olap, 0, (data->M - 1) * sizeof(double)) != cudaSuccess)
        goto firinit_error;
    
    return data;

firinit_error:
    FIRConvAddOverlapCUDA_Free(data);
    return NULL;
}

//-------------------------------------------------------------------------------------------

bool FIRConvAddOverlapCUDA_Process(const double *in, double *out, FIRConvAddOverlapCuda_Data *data)
{
    int noBlocks, threadsPerBlock, outIdx;

    if(cudaMemcpy(data->in, in, data->L * sizeof(double), cudaMemcpyHostToDevice) != cudaSuccess)
        return false;
    if(cudaMemset(&(data->in[data->L]), 0, (data->N - data->L) * sizeof(double)) != cudaSuccess)
        return false;
    
    outIdx = FFTRadix2Cuda_R2C_DFT_OnDevice(data->in, data->FFT);
    if(outIdx < 0)
        return false;
    omegaDebugCUDAMemoryOmega<double>(data->FFT->stack[outIdx], data->Nout * 2);
    
    double *X = data->FFT->stack[outIdx];
    Omega1DCuda_ThreadDivision(data->Nout, noBlocks, threadsPerBlock);
    kernelFIRComplexMultiplication<<<noBlocks, threadsPerBlock>>>(data->firH, X);
    omegaDebugCUDAMemoryOmega<double>(data->firH, data->Nout * 2);
    omegaDebugCUDAMemoryOmega<double>(X, data->Nout * 2);

    if(!FFTRadix2Cuda_C2R_iDFT_OnDevice(X, data->iFFT))
        return false;
    omegaDebugCUDAMemoryOmega<double>(data->iFFT->xB, data->N);

    Omega1DCuda_ThreadDivision(data->M - 1, noBlocks, threadsPerBlock);
    kernelFIRAddOverlap<<<noBlocks, threadsPerBlock>>>(data->olap, data->iFFT->xB, data->M);
    omegaDebugCUDAMemoryOmega<double>(data->olap, data->M - 1);
    omegaDebugCUDAMemoryOmega<double>(data->iFFT->xB, data->M - 1);

    kernelFIRSaveOverlap<<<noBlocks, threadsPerBlock>>>(data->olap, data->iFFT->xB, data->M, data->N);
    omegaDebugCUDAMemoryOmega<double>(data->olap, data->M - 1);

    if(cudaMemcpy(out, data->iFFT->xB, data->L * sizeof(double), cudaMemcpyDeviceToHost) != cudaSuccess)
        return false;

    return true;
}

//-------------------------------------------------------------------------------------------