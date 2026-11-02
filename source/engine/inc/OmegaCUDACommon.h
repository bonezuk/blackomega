//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_OMEGACUDACOMMON_H
#define __OMEGA_ENGINE_OMEGACUDACOMMON_H
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_CUDA)
//-------------------------------------------------------------------------------------------

#include <cuda_runtime.h>

#include "engine/inc/EngineDLL.h"

//#define __KERNEL_DEBUG_CUDA_MEMORY 1

//-------------------------------------------------------------------------------------------

template <typename T> void omegaDebugCUDAMemoryOmega(const T *gMem, int len)
{
#if defined(__KERNEL_DEBUG_CUDA_MEMORY)
	T *cMem = new T [len];
	cudaMemcpy(cMem, gMem, len * sizeof(T), cudaMemcpyDeviceToHost);
	delete [] cMem;
#endif
}

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT void Omega1DCuda_ThreadDivision(int N, int& noBlocks, int& threadsPerBlock);

ENGINE_EXPORT int initCUDAOmega();

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT int isPointerForCUDADevice(void *ptr);

ENGINE_EXPORT cudaMemcpyKind memoryCopyKind(void *pSrc, void *pDest);

ENGINE_EXPORT void convertCUDADoubleToFloat(const double *in, float *out, int noSamples);

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
