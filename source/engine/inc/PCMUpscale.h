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

class ENGINE_EXPORT PCMUpscale
{
    public:
        PCMUpscale();
        virtual ~PCMUpscale();
        
        bool init(int inputFrequency, int dsdTimes);
        int outputFrequency() const;

        int noInputSamples() const;
        int noOutputSamples() const;

        QSharedPointer<double> upscale(const double *in, int noInput, int &noOutput);
        
        const float *upscaleSingleOnDevice(const double *in, int noInput, int& noOutput);

    private:
        QVector<QPair<engine::FIRFilterType, FIRConvAddOverlapCuda_Data *> > m_filtersCUDA;
        QSharedPointer<double> m_fbOutput;
        int m_inputFrequency;
        int m_dsdTimes;
        float *m_outSDevice;

        int noSteps(int N) const;
        engine::FIRFilterType filterForFrequency(int freq) const;
        int filterIndexOfType(engine::FIRFilterType type) const;
        engine::FIRFilterType filterTypeAtIndex(int idx) const;
        bool processFilterBankCUDA(const double *in, double *out);
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
