//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WHITEOMEGA_CPEDECODE_H
#define __OMEGA_WHITEOMEGA_CPEDECODE_H
//-------------------------------------------------------------------------------------------

#include "engine/whiteomega/inc/ICS.h"
#include "engine/whiteomega/inc/GAConfig.h"
#include "engine/whiteomega/inc/WSequence.h"
#include "engine/whiteomega/inc/AACDecode.h"
#include "engine/whiteomega/inc/PNS.h"
#include "engine/whiteomega/inc/Prediction.h"
#include "engine/whiteomega/inc/PredictionLTP.h"
#include "engine/whiteomega/inc/TNS.h"
#include "engine/whiteomega/inc/Window.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class CPEDecode
{
    public:
        CPEDecode();
        
        void reset();
        void setGAConfig(GAConfig *cfg);
        
        bool decode(Sequence *s,sample_t *out,tint& len);
        
        AACDecode m_channelA;
        AACDecode m_channelB;

        tint m_elementInstanceTag;
        bool m_commonWindow;
        
        tint m_msMaskPresent;
        tint m_msUsed[8][15 * 8];

    protected:


        tint m_CPECount;

        GAConfig *m_gaConfig;
        
        PNS m_pns;
        
        Prediction m_prediction;
        
        PredictionLTP m_LTPPredL;
        PredictionLTP m_LTPPredR;
        
        TNS m_tnsL;
        TNS m_tnsR;
        
        Window m_windowL;
        Window m_windowR;

        bool readCPE(Sequence *s);
        
        bool isNoise(AACDecode *ch,tint g,tint sfb) const;
        bool isNoise(ICSInfo *info,tint g,tint sfb) const;
        bool isNoise(tint cb) const;
        
        tint isIntensity(AACDecode *ch,tint g,tint sfb) const;
        tint isIntensity(ICSInfo *info,tint g,tint sfb) const;
        tint isIntensity(tint cb) const;
        
        tint invertIntensity(tint g,tint sfb) const;
        
        void msStereo();
        void isStereo();
        
        tint processOutput(sample_t *out,sample_t *L,sample_t *R,tint len);
};

//-------------------------------------------------------------------------------------------

inline bool CPEDecode::isNoise(AACDecode *ch,tint g,tint sfb) const
{
    return isNoise(&(ch->m_info),g,sfb);
}
//-------------------------------------------------------------------------------------------

inline bool CPEDecode::isNoise(ICSInfo *info,tint g,tint sfb) const
{
    return isNoise(info->sfbCb[g][sfb]);
}

//-------------------------------------------------------------------------------------------

inline bool CPEDecode::isNoise(tint cb) const
{
    return (cb==NOISE_HCB) ? true : false;
}

//-------------------------------------------------------------------------------------------

inline tint CPEDecode::isIntensity(AACDecode *ch,tint g,tint sfb) const
{
    return isIntensity(&(ch->m_info),g,sfb);
}

//-------------------------------------------------------------------------------------------

inline tint CPEDecode::isIntensity(ICSInfo *info,tint g,tint sfb) const
{
    return isIntensity(info->sfbCb[g][sfb]);
}
    
//-------------------------------------------------------------------------------------------

inline tint CPEDecode::isIntensity(tint cb) const
{
    switch(cb)
    {
        case INTENSITY_HCB:
            return 1;
            
        case INTENSITY_HCB2:
            return -1;
        
        default:
            return 0;
    }
}

//-------------------------------------------------------------------------------------------

inline tint CPEDecode::invertIntensity(tint g,tint sfb) const
{
    if(m_msMaskPresent==1)
    {
        return (1 - (m_msUsed[g][sfb] << 1));
    }
    else
    {
        return true;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

