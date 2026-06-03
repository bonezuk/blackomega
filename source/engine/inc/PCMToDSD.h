//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_PCMTODSD_H
#define __OMEGA_ENGINE_PCMTODSD_H
//-------------------------------------------------------------------------------------------

#include <QVector>
#include <QPair>
#include <QSharedPointer>

#include "engine/inc/FFTRadix2_R2C.h"
#include "engine/inc/FFTRadix2_C2R.h"
#include "engine/inc/FIRFilterDB.h"
#include "engine/inc/FIRConvolutionAddOverlap.h"
#include "engine/inc/DeltaSigmaModulator.h"

#if defined(OMEGA_CUDA)
#include "engine/inc/FFTRadix2Cuda.h"
#include "engine/inc/FIRConvolutionAddOverlapCUDA.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT PCMToDSD
{
    public:
        typedef enum
        {
            e_computeMethodCPU = 0,
            e_computeMethodCUDA = 1
        } ComputeMethod;

    public:
        PCMToDSD(ComputeMethod computeMethod);
        virtual ~PCMToDSD();

        static bool isComputeMethodAvailable(ComputeMethod method);

        bool init(int inputFrequency, int dsdTimes, bool isLSB);

        bool process(const double *in, uint8_t *out);

        bool isLSB() const;
        int noInputSamples() const;
        int noOutputSamples() const;
        int noOutputBytes() const;

        int inputFrequency() const;
        int outputFrequency() const;

    private:
        ComputeMethod m_computeMethod;
        QVector<QPair<FIRFilterType, QSharedPointer<FIRConvolutionAddOverlapOctaveUpscale> > > m_filters;
        QVector<QSharedPointer<double> > m_buffers;
        DeltaSigmaModulator m_modulator;
        int m_inputFrequency;
        int m_dsdTimes;

#if defined(OMEGA_CUDA)
        QVector<QPair<FIRFilterType, FIRConvAddOverlapCuda_Data *> > m_filtersCUDA;
        QSharedPointer<double> m_fbOutput;
#endif

        void done();

        int noSteps(int N) const;
        int baseFrequency(int freq) const;
        FIRFilterType filterForFrequency(int freq) const;
        int filterIndexOfType(FIRFilterType type) const;

        int noFilters() const;
        FIRFilterType filterTypeAtIndex(int idx) const;

        double *processFilterBankCPU(const double *in);
#if defined(OMEGA_CUDA)
        double *processFilterBankCUDA(const double *in);
#endif
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
