//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_DELTASIGMAMODULATOR_H
#define __OMEGA_ENGINE_DELTASIGMAMODULATOR_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/EngineDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

#define DSM_IIR_ORDER 8
#define DSM_DELTA_SIZE 32768

class ENGINE_EXPORT DeltaSigmaModulator
{
    public:
        DeltaSigmaModulator();
        virtual ~DeltaSigmaModulator();

        void init(bool isLSB);
        void process(const double *pcm, uint8_t *dsd, int len);

        bool isLSB() const;

    private:

        bool m_isLSB;
        int m_deltaIdx;
        double *m_delta;
        double m_NS[2][DSM_IIR_ORDER];
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
