//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FIRCONVOLUTIONADDOVERLAPCUDA_H
#define __OMEGA_ENGINE_FIRCONVOLUTIONADDOVERLAPCUDA_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/EngineDLL.h"
#include "engine/inc/FFTRadix2Cuda.h"

//-------------------------------------------------------------------------------------------

typedef struct s_FIRConvAddOverlapCuda_Data
{
    // length of FIR filter = firSize
    int M;
    // length of input/output audio block = outputSize
    int L;
    // length of FFT (N = L + M - 1)
    int N;
    int Nout;
    double *firH;
    double *in;
    double *olap;
    FFTRadix2Cuda_Data *FFT;
    FFTRadix2Cuda_Data *iFFT;
} FIRConvAddOverlapCuda_Data;

//-------------------------------------------------------------------------------------------

ENGINE_EXPORT void FIRConvAddOverlapCUDA_Free(FIRConvAddOverlapCuda_Data *data);

ENGINE_EXPORT FIRConvAddOverlapCuda_Data *FIRConvAddOverlapCUDA_Init(const double *firCoeff, int firSize, int outputSize);

ENGINE_EXPORT bool FIRConvAddOverlapCUDA_Process(const double *in, double *out, FIRConvAddOverlapCuda_Data *data);

ENGINE_EXPORT bool FIRConvAddOverlapCUDA_OctaveUpscale_Process(const double *in, double *out, FIRConvAddOverlapCuda_Data *data);

ENGINE_EXPORT const double *FIRConvAddOverlapCUDA_OctaveUpscale_Process_Device(const double *in, FIRConvAddOverlapCuda_Data *data, bool isInputHost);

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
