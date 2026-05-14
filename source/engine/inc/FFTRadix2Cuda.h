//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FFTRADIX2CUDA_H
#define __OMEGA_ENGINE_FFTRADIX2CUDA_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/EngineDLL.h"

//-------------------------------------------------------------------------------------------

typedef struct s_FFTRadix2Cuda_Data
{
	int N;
	int noBits;
	int *reverseIndex;
	double *xA;
	double *xB;
	double **coeff;
	double **stack;
} FFTRadix2Cuda_Data;

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT int initCUDAOmega();

ENGINE_EXPORT void FFTRadix2Cuda_Free(FFTRadix2Cuda_Data *data);

ENGINE_EXPORT FFTRadix2Cuda_Data *FFTRadix2Cuda_Init(int N);

ENGINE_EXPORT bool FFTRadix2Cuda_R2C_DFT(const double *x, double *X, FFTRadix2Cuda_Data *data);

ENGINE_EXPORT bool FFTRadix2Cuda_C2R_iDFT(const double *x, double *X, FFTRadix2Cuda_Data *data);

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
