//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_WHITEOMEGA_TNS_H
#define __OMEGA_ENGINE_WHITEOMEGA_TNS_H
//-------------------------------------------------------------------------------------------

#include "engine/whiteomega/inc/ICS.h"
#include "engine/whiteomega/inc/Common.h"
#include "engine/whiteomega/inc/GAConfig.h"
#include "engine/whiteomega/inc/AACDecode.h"

#define TNS_MAX_ORDER 20

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT TNS
{
    public:
        TNS();
        ~TNS();

        void decode();
        void encodeLTP(sample_t *spec);

        void set(GAConfig *cfg,AACDecode *ch);

    protected:

        static const sample_t m_winCoef_0_0[16];
        static const sample_t m_winCoef_0_1[16];
        static const sample_t m_winCoef_1_0[16];
        static const sample_t m_winCoef_1_1[16];

        GAConfig *m_gaConfig;
        AACDecode *m_channel;

        tint maxSfb();

        void decodeCoef(tint order,tint coefRes,tint coefCompress,tint *coef,sample_t *a);

        void arFilter(sample_t *spec,tint size,tint inc,sample_t *lpc,tint order);
        void maFilter(sample_t *spec,tint size,tint inc,sample_t *lpc,tint order);

        void doDecode(sample_t *spec);
        void doEncode(sample_t *spec);
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
