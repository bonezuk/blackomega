#include "engine/whiteomega/inc/PNS.h"
#include "engine/whiteomega/inc/CPEDecode.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

const tuint32 PNS::m_parity[256] = {
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0
};

tuint32 PNS::m_randA = 1;
tuint32 PNS::m_randB = 1;

//-------------------------------------------------------------------------------------------

PNS::PNS() : m_gaConfig(0),
    m_CPE(0),
    m_chL(0),
    m_chR(0)
{}

//-------------------------------------------------------------------------------------------

void PNS::set(GAConfig *cfg,AACDecode *ch)
{
    m_gaConfig = cfg;
    m_chL = ch;
    m_chR = 0;
}

//-------------------------------------------------------------------------------------------

void PNS::set(GAConfig *cfg,CPEDecode *cpe)
{
    m_gaConfig = cfg;
    m_CPE = cpe;
    m_chL = &(cpe->m_channelA);
    m_chR = &(cpe->m_channelB);
}

//-------------------------------------------------------------------------------------------

void PNS::processL()
{
    processCh(m_chL);
}

//-------------------------------------------------------------------------------------------

void PNS::processR()
{
    processCh(m_chR);
}

//-------------------------------------------------------------------------------------------

void PNS::processCh(AACDecode *ch)
{
    tint g,b,sfb,offset,size;
    tint group = 0,nshort = m_gaConfig->m_frameLength >> 3;
    ICSInfo *info = &(ch->m_info);
    sample_t *spec = ch->m_spectralCoef;
    
    for(g=0;g<info->numWindowGroups;++g)
    {
        for(b=0;b<info->windowGroupLength[g];++b)
        {
            for(sfb=0;sfb<info->maxSfb;++sfb)
            {
                if(isNoise(ch,g,sfb))
                {
                    info->ltp1.longUsed[sfb] = 0;
                    info->ltp2.longUsed[sfb] = 0;
                    info->pred.predictionUsed[sfb] = 0;
                    
                    offset = info->swbOffset[sfb];
                    size = minV(info->swbOffset[sfb+1],info->swbOffsetMax) - offset;
                    generateRandomVector(&spec[group + offset],info->scaleFactors[g][sfb],size);
                }
            }
            group += nshort;
        }
    }
}

//-------------------------------------------------------------------------------------------

void PNS::processBoth()
{
    tint g,b,sfb,offset,size;
    tint group = 0,nshort = m_gaConfig->m_frameLength >> 3;
    tint msMask = m_CPE->m_msMaskPresent;
    ICSInfo *infoL = &(m_chL->m_info);
    ICSInfo *infoR = &(m_chR->m_info);
    sample_t *specL = m_chL->m_spectralCoef;
    sample_t *specR = m_chR->m_spectralCoef;
    
    for(g=0;g<infoL->numWindowGroups;++g)
    {
        for(b=0;b<infoL->windowGroupLength[g];++b)
        {
            for(sfb=0;sfb<infoL->maxSfb;++sfb)
            {
                if(isNoiseL(g,sfb))
                {
                    infoL->ltp1.longUsed[sfb] = 0;
                    infoL->ltp2.longUsed[sfb] = 0;
                    infoL->pred.predictionUsed[sfb] = 0;
                    
                    offset = infoL->swbOffset[sfb];
                    size = minV(infoL->swbOffset[sfb+1],infoL->swbOffsetMax) - offset;
                    generateRandomVector(&specL[group + offset],infoL->scaleFactors[g][sfb],size);
                }
                
                if(isNoiseR(g,sfb))
                {
                    offset = infoR->swbOffset[sfb];
                    size = minV(infoR->swbOffset[sfb+1],infoR->swbOffsetMax) - offset;

                    if(msMask==2 || (msMask==1 && m_CPE->m_msUsed[g][sfb]))
                    {
                        tint n = group + offset;
                        ::memcpy(&specR[n],&specL[n],sizeof(sample_t) * size);
                    }
                    else
                    {
                        infoR->ltp1.longUsed[sfb] = 0;
                        infoR->ltp2.longUsed[sfb] = 0;
                        infoR->pred.predictionUsed[sfb] = 0;
                    
                        generateRandomVector(&specR[group + offset],infoR->scaleFactors[g][sfb],size);                        
                    }
                }
            }
            group += nshort;
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
