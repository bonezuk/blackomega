//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WHITEOMEGA_PNS_H
#define __OMEGA_WHITEOMEGA_PNS_H
//-------------------------------------------------------------------------------------------

#include "engine/whiteomega/inc/GAConfig.h"
#include "engine/whiteomega/inc/AACDecode.h"
#include "engine/whiteomega/inc/Common.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class CPEDecode;

//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT PNS
{
    public:
        PNS();
        
        void set(GAConfig *cfg,AACDecode *ch);
        void set(GAConfig *cfg,CPEDecode *cpe);
        
        void processL();
        void processR();
        void processBoth();
        
    protected:
        
        static const tuint32 m_parity[256];

        static tuint32 m_randA;
        static tuint32 m_randB;
        
        GAConfig *m_gaConfig;
        CPEDecode *m_CPE;
        AACDecode *m_chL;
        AACDecode *m_chR;
        
        bool isNoiseL(tint g,tint sfb);
        bool isNoiseR(tint g,tint sfb);
        bool isNoise(AACDecode *ch,tint g,tint sfb);
        tint rand();
        void generateRandomVector(sample_t *spec,tint scaleFactor,tint size);
        
        void processCh(AACDecode *ch);
};

//-------------------------------------------------------------------------------------------

inline bool PNS::isNoiseL(tint g,tint sfb)
{
    return isNoise(m_chL,g,sfb);
}

//-------------------------------------------------------------------------------------------

inline bool PNS::isNoiseR(tint g,tint sfb)
{
    return isNoise(m_chR,g,sfb);
}

//-------------------------------------------------------------------------------------------

inline bool PNS::isNoise(AACDecode *ch,tint g,tint sfb)
{
    return (ch->m_info.sfbCb[g][sfb] == NOISE_HCB) ? true : false;
}

//-------------------------------------------------------------------------------------------

inline tint PNS::rand()
{
    tuint32 t1 = m_parity[m_randA & 0x0000000f5];
    tuint32 t2 = m_parity[(m_randB >> 25) & 0x00000063];
    
    m_randA = (m_randA >> 1) | t1;
    m_randB = (m_randB << 1) | t2;
    return static_cast<tint>(m_randA ^ m_randB);
}

//-------------------------------------------------------------------------------------------

inline void PNS::generateRandomVector(sample_t *spec,tint scaleFactor,tint size)
{
    tint i;
    sample_t t,scale,energy = c_zeroSample;
    
    scale = c_plusOneSample / static_cast<sample_t>(size);
    
    for(i=0;i<size;++i)
    {
        t = scale * static_cast<sample_t>(rand());
        spec[i] = t;
        energy += t * t;
    }
    
    scale  = c_plusOneSample / static_cast<sample_t>(sqrt(energy));
    if(scaleFactor>=-100 && scaleFactor<156)
    {
        scale *= AACDecode::m_dequantScalefactor[scaleFactor + 100];
    }
    else
    {
        scale *= static_cast<sample_t>(pow(2.0,0.25 * static_cast<tfloat64>(scaleFactor)));
    }
    
    for(i=0;i<size;++i)
    {
        spec[i] *= scale;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

