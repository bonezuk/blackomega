#include "engine/whiteomega/inc/CPEDecode.h"
#if defined(WHITEOMEGA_COMPARE)
#include "engine/whiteomega/inc/WCompare.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

CPEDecode::CPEDecode() : m_channelA(),
    m_channelB(),
    m_elementInstanceTag(0),
    m_commonWindow(false),
    m_msMaskPresent(0),
    m_CPECount(0),
    m_gaConfig(0),
    m_pns(),
    m_prediction(),
    m_LTPPredL(),
    m_LTPPredR(),
    m_tnsL(),
    m_tnsR(),
    m_windowL(),
    m_windowR()
{}

//-------------------------------------------------------------------------------------------

void CPEDecode::reset()
{
    m_channelA.reset();
    m_channelB.reset();
}

//-------------------------------------------------------------------------------------------

void CPEDecode::setGAConfig(GAConfig *cfg)
{
    m_gaConfig = cfg;
    m_channelA.setGAConfig(cfg);
    m_channelB.setGAConfig(cfg);
    m_pns.set(cfg,this);
    m_prediction.set(cfg,&m_channelA,&m_channelB);
    m_LTPPredL.set(cfg,&m_channelA,&m_windowL,&m_tnsL);
    m_LTPPredR.set(cfg,&m_channelB,&m_windowR,&m_tnsR);
    m_tnsL.set(cfg,&m_channelA);
    m_tnsR.set(cfg,&m_channelB);
    m_windowL.set(cfg,&m_channelA);
    m_windowR.set(cfg,&m_channelB);
}

//-------------------------------------------------------------------------------------------

bool CPEDecode::readCPE(Sequence *s)
{
    tint g,sfb;
    WSequence *seq = dynamic_cast<WSequence *>(s);
    
    if(seq==0)
    {
        return false;
    }
    
    m_elementInstanceTag = seq->readBitsI(4);
    
    m_commonWindow = (seq->readBitI()) ? true : false;
    if(m_commonWindow)
    {
        if(!m_channelA.readICSInfo(m_commonWindow,seq))
        {
            return false;
        }
        m_msMaskPresent = seq->readBitsI(2);
        if(m_msMaskPresent==1)
        {
            for(g=0;g<m_channelA.m_info.numWindowGroups;++g)
            {
                for(sfb=0;sfb<m_channelA.m_info.maxSfb;++sfb)
                {
                    m_msUsed[g][sfb] = seq->readBitI();
                }
            }
        }
        
        if(m_gaConfig->m_audioObjectType>=GAConfig::e_audioERAACLC && m_channelA.m_info.pred.predictorDataPresent)
        {
            m_channelA.m_info.ltp1.dataPresent = (seq->readBitI()) ? true : false;
            if(m_channelA.m_info.ltp1.dataPresent)
            {
                m_channelA.readLTPData(seq,&(m_channelA.m_info.ltp1));
            }
        }
        
        m_channelA.copyICSInfo(&(m_channelB.m_info));
        if(!m_channelB.windowGroupingInfo())
        {
            return false;
        }
    }
    else
    {
        m_msMaskPresent = 0;
    }

    if(!m_channelA.readICS(seq,m_commonWindow,0))
    {
        return false;
    }

    if(m_gaConfig->m_audioObjectType>=GAConfig::e_audioERAACLC && m_channelA.m_info.pred.predictorDataPresent)
    {
        m_channelA.m_info.ltp2.dataPresent = (seq->readBitI()) ? true : false;
        if(m_channelA.m_info.ltp2.dataPresent)
        {
            m_channelA.readLTPData(seq,&(m_channelA.m_info.ltp2));
        }
    }
    
    if(!m_channelB.readICS(seq,m_commonWindow,0))
    {
        return false;
    }
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool CPEDecode::decode(Sequence *s,sample_t *out,tint& len)
{
    tint lenL,lenR;
    sample_t *outL,*outR;

    m_CPECount++;
    
#if defined(WHITEOMEGA_COMPARE)
    common::Log::g_Log.print("Frame - %d\r",m_CPECount);
#endif

    if(!readCPE(s))
    {
        return false;
    }
    
    m_channelA.dequantize();
    m_channelB.dequantize();

#if defined(WHITEOMEGA_COMPARE)
    if(g_WCompare!=0)
    {
        g_WCompare->compareA(m_channelA.m_spectralCoef,m_gaConfig->m_frameLength,false);
        g_WCompare->compareA(m_channelB.m_spectralCoef,m_gaConfig->m_frameLength,false);
    }
#endif

    if(m_msMaskPresent)
    {
        m_pns.processBoth();
    }
    else
    {
        m_pns.processL();
        m_pns.processR();
    }

#if defined(WHITEOMEGA_COMPARE)
    if(g_WCompare!=0)
    {
        g_WCompare->compareA(m_channelA.m_spectralCoef,m_gaConfig->m_frameLength,false);
        g_WCompare->compareA(m_channelB.m_spectralCoef,m_gaConfig->m_frameLength,false);
    }
#endif

    msStereo();

#if defined(WHITEOMEGA_COMPARE)
    if(g_WCompare!=0)
    {
        g_WCompare->compareA(m_channelA.m_spectralCoef,m_gaConfig->m_frameLength,false);
        g_WCompare->compareA(m_channelB.m_spectralCoef,m_gaConfig->m_frameLength,false);
    }
#endif

    isStereo();

#if defined(WHITEOMEGA_COMPARE)
    if(g_WCompare!=0)
    {
        g_WCompare->compareA(m_channelA.m_spectralCoef,m_gaConfig->m_frameLength,false);
        g_WCompare->compareA(m_channelB.m_spectralCoef,m_gaConfig->m_frameLength,false);
    }
#endif

    if(m_gaConfig->m_audioObjectType==GAConfig::e_audioAACMain)
    {
        m_prediction.icPredictionL();
        m_prediction.icPredictionR();
        m_prediction.pnsResetPredStateL();
        m_prediction.pnsResetPredStateR();
    }

#if defined(WHITEOMEGA_COMPARE)
    if(g_WCompare!=0)
    {
        g_WCompare->compareA(m_channelA.m_spectralCoef,m_gaConfig->m_frameLength,false);
        g_WCompare->compareA(m_channelB.m_spectralCoef,m_gaConfig->m_frameLength,false);
    }
#endif

    if(m_LTPPredL.isLTP())
    {
        LTPInfo *ltp1 = &(m_channelA.m_info.ltp1);
        LTPInfo *ltp2 = (m_commonWindow) ? &(m_channelA.m_info.ltp2) : &(m_channelB.m_info.ltp1);
        
        m_LTPPredL.predict(ltp1);
        m_LTPPredR.predict(ltp2);
    }

#if defined(WHITEOMEGA_COMPARE)
    if(g_WCompare!=0)
    {
        g_WCompare->compareA(m_channelA.m_spectralCoef,m_gaConfig->m_frameLength,false);
        g_WCompare->compareA(m_channelB.m_spectralCoef,m_gaConfig->m_frameLength,false);
    }
#endif

    m_tnsL.decode();
    m_tnsR.decode();

#if defined(WHITEOMEGA_COMPARE)
    if(g_WCompare!=0)
    {
        g_WCompare->compareA(m_channelA.m_spectralCoef,m_gaConfig->m_frameLength,false);
        g_WCompare->compareA(m_channelB.m_spectralCoef,m_gaConfig->m_frameLength,false);
    }
#endif

    outL = m_windowL.process(lenL);
    outR = m_windowR.process(lenR);
    
#if defined(WHITEOMEGA_COMPARE)
    if(g_WCompare!=0)
    {
//        g_WCompare->compareA(outL,lenL,(m_CPECount==4178) ? true : false);
//        g_WCompare->compareA(outR,lenR,false);
    }
#endif

    if(m_LTPPredL.isLTP())
    {
        m_LTPPredL.update();
        m_LTPPredR.update();
    }
    
    len = processOutput(out,outL,outR,lenL);

    return true;
}

//-------------------------------------------------------------------------------------------

void CPEDecode::msStereo()
{
    tint g,b,sfb;
    tint group = 0,nshort = m_gaConfig->m_frameLength >> 3;
    ICSInfo *infoL = &(m_channelA.m_info);
    ICSInfo *infoR = &(m_channelB.m_info);
    sample_t *specL = m_channelA.m_spectralCoef;
    sample_t *specR = m_channelB.m_spectralCoef;
    
    if(m_msMaskPresent==2)
    {
        for(g=0;g<infoL->numWindowGroups;++g)
        {
            for(b=0;b<infoL->windowGroupLength[g];++b)
            {
                for(sfb=0;sfb<infoL->maxSfb;++sfb)
                {
                    if(!isIntensity(infoR,g,sfb) && !isNoise(infoL,g,sfb))
                    {
                        tint i,k,swbUpper;
                        
                        i = infoL->swbOffset[sfb];
                        k = group + i;
                        swbUpper = minV(infoL->swbOffset[sfb+1],infoL->swbOffsetMax);
                        
                        while(i<swbUpper)
                        {
                            sample_t t = specL[k] - specR[k];
                            specL[k] = specL[k] + specR[k];
                            specR[k] = t;
                            k++;
                            i++;
                        }
                    }
                }
                group += nshort;
            }
        }
    }
    else if(m_msMaskPresent==1)
    {
        for(g=0;g<infoL->numWindowGroups;++g)
        {
            for(b=0;b<infoL->windowGroupLength[g];++b)
            {
                for(sfb=0;sfb<infoL->maxSfb;++sfb)
                {
                    if(m_msUsed[g][sfb] && !isIntensity(infoR,g,sfb) && !isNoise(infoL,g,sfb))
                    {
                        tint i,k,swbUpper;
                        
                        i = infoL->swbOffset[sfb];
                        k = group + i;
                        swbUpper = minV(infoL->swbOffset[sfb+1],infoL->swbOffsetMax);
                        
                        while(i<swbUpper)
                        {
                            sample_t t = specL[k] - specR[k];
                            specL[k] = specL[k] + specR[k];
                            specR[k] = t;
                            k++;
                            i++;
                        }
                    }
                }
                group += nshort;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void CPEDecode::isStereo()
{
    tint g,b,sfb,sf,cb;
    tint group = 0,nshort = m_gaConfig->m_frameLength >> 3;
    ICSInfo *infoL = &(m_channelA.m_info);
    ICSInfo *infoR = &(m_channelB.m_info);
    sample_t *specL = m_channelA.m_spectralCoef;
    sample_t *specR = m_channelB.m_spectralCoef;
    
    for(g=0;g<infoR->numWindowGroups;++g)
    {
        for(b=0;b<infoR->windowGroupLength[g];++b)
        {
            for(sfb=0;sfb<infoR->maxSfb;++sfb)
            {
                cb = infoR->sfbCb[g][sfb];
                
                if(isIntensity(cb))
                {
                    tint k,swbUpper;
                    sample_t scale;
                                        
                    infoL->pred.predictionUsed[sfb] = 0;
                    infoR->pred.predictionUsed[sfb] = 0;
                    
                    k = group + infoL->swbOffset[sfb];
                    swbUpper = k + minV(infoL->swbOffset[sfb+1],infoL->swbOffsetMax);
                    
                    sf = infoR->scaleFactors[g][sfb];
                    if(sf>=-512 && sf<=512)
                    {
                        scale = AACDecode::m_intensityScalefactor[sf];
                    }
                    else
                    {
                        scale = static_cast<sample_t>(pow(0.5,0.25 * static_cast<tfloat64>(sf)));
                    }
                    
                    if(isIntensity(cb) != invertIntensity(g,sfb))
                    {
                        scale = -scale;
                    }
                    
                    while(k<swbUpper)
                    {
                        specR[k] = specL[k] * scale;
                        k++;
                    }
                }
            }
        }
        group += nshort;
    }    
}

//-------------------------------------------------------------------------------------------

tint CPEDecode::processOutput(sample_t *out,sample_t *L,sample_t *R,tint len)
{
    tint i,j;
    tfloat64 xL,xR;
    
    for(i=0,j=0;i<len;++i,j+=2)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        xL = L[i] / 32768.0f;
        if(xL > 1.0f)
        {
            xL = 1.0f;
        }
        else if(xL < -1.0f)
        {
            xL = -1.0f;
        }
        
        xR = R[i] / 32768.0f;
        if(xR > 1.0f)
        {
            xR = 1.0f;
        }
        else if(xR < -1.0f)
        {
            xR = -1.0f;
        }
#else
        xL = L[i] / 32768.0;
        if(xL > 1.0)
        {
            xL = 1.0;
        }
        else if(xL < -1.0)
        {
            xL = -1.0;
        }
        
        xR = R[i] / 32768.0;
        if(xR > 1.0)
        {
            xR = 1.0;
        }
        else if(xR < -1.0)
        {
            xR = -1.0;
        }
#endif
    
        out[j + 0] = xL;
        out[j + 1] = xR;
    }
    return i;
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
