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

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT PCMToDSD
{
    public:
        PCMToDSD();
        virtual ~PCMToDSD();

        bool init(int inputFrequency, int dsdTimes, bool isLSB);

        void process(const double *in, uint8_t *out);

        bool isLSB() const;
        int noInputSamples() const;
        int noOutputSamples() const;
        int noOutputBytes() const;

        int inputFrequency() const;
        int outputFrequency() const;

    private:
        QVector<QPair<FIRFilterType, QSharedPointer<FIRConvolutionAddOverlapOctaveUpscale> > > m_filters;
        QVector<QSharedPointer<double> > m_buffers;
        DeltaSigmaModulator m_modulator;
        int m_inputFrequency;
        int m_dsdTimes;

        int noSteps(int N) const;
        int baseFrequency(int freq) const;
        FIRFilterType filterForFrequency(int freq) const;
        int filterIndexOfType(FIRFilterType type) const;
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
