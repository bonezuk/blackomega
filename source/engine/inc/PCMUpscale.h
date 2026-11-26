//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_PCMUPSCALE_H
#define __OMEGA_ENGINE_PCMUPSCALE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/DiskOps.h"
#include "engine/inc/Codec.h"
#include "engine/inc/RData.h"
#include "engine/inc/PCMToDSD.h"
#include "engine/inc/FormatType.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------
// Upscales PCM to the sample rate of a given DSD rate through a bank of octave upscaling
// FFT low pass filters. The filter bank is run either on the CPU
// (FIRConvolutionAddOverlapOctaveUpscale) or on the GPU (FIRConvAddOverlapCUDA_*).
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT PCMUpscale
{
    public:
        // e_computeMethodCPU or e_computeMethodCUDA, as used by PCMToDSD.
        typedef PCMToDSD::ComputeMethod ComputeMethod;

    public:
        // Uses CUDA when it is available, otherwise the CPU.
        PCMUpscale();
        // Uses the given compute method, falling back to the CPU if it is not available.
        PCMUpscale(ComputeMethod computeMethod);
        virtual ~PCMUpscale();

        static bool isComputeMethodAvailable(ComputeMethod method);
        ComputeMethod computeMethod() const;

        bool init(int inputFrequency, int dsdTimes);
        int outputFrequency() const;

        int noInputSamples() const;
        int noOutputSamples() const;

        QSharedPointer<double> upscale(const double *in, int noInput, int &noOutput);

        // Upscales a single block of noInputSamples() into noOutputSamples() single precision
        // samples held by this instance. The memory returned is valid until the next call.
        //  - e_computeMethodCUDA : pointer to GPU device memory, for use by CUDA kernels.
        //  - e_computeMethodCPU  : pointer to host memory, accessible to the CPU.
        const float *upscaleSingleOnDevice(const double *in, int noInput, int& noOutput);

    private:
        ComputeMethod m_computeMethod;
        // The filters of the bank in the order that they are applied.
        QVector<FIRFilterType> m_filterTypes;
        // CPU filter bank, with the output buffer for each filter bar the last.
        QVector<QSharedPointer<FIRConvolutionAddOverlapOctaveUpscale> > m_filters;
        QVector<QSharedPointer<double> > m_buffers;
        // Output of the last filter in host memory.
        QSharedPointer<double> m_fbOutput;
        // Single precision output in host memory (CPU compute).
        QSharedPointer<float> m_outSHost;
        int m_inputFrequency;
        int m_dsdTimes;
#if defined(OMEGA_CUDA)
        QVector<FIRConvAddOverlapCuda_Data *> m_filtersCUDA;
        // Single precision output in device memory (CUDA compute).
        float *m_outSDevice;
#endif

        void done();

        int noSteps(int N) const;
        FIRFilterType filterForFrequency(int freq) const;
        int filterIndexOfType(FIRFilterType type) const;
        FIRFilterType filterTypeAtIndex(int idx) const;
        bool processFilterBank(const double *in, double *out);
        void processFilterBankCPU(const double *in, double *out);
#if defined(OMEGA_CUDA)
        const double *processFilterBankOnDeviceCUDA(const double *in);
        bool processFilterBankCUDA(const double *in, double *out);
#endif
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
