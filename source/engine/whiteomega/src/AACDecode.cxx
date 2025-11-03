#include "engine/whiteomega/inc/AACDecode.h"
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
#if defined(WHITEOMEGA_COMPARE)
tint AACDecode::m_ICSFrameNo = 0;
#endif
//-------------------------------------------------------------------------------------------
// Dequantization lookup tables.
//-------------------------------------------------------------------------------------------

sample_t *AACDecode::m_dequantScalefactor = 0;
sample_t *AACDecode::m_dequantSpectral = 0;
sample_t *AACDecode::m_dequantSp = 0;
sample_t *AACDecode::m_intensityScalefactor = 0;
sample_t *AACDecode::m_intensitySf = 0;

//-------------------------------------------------------------------------------------------

AACDecode::AACDecode() : m_gaConfig(0)
{}

//-------------------------------------------------------------------------------------------

AACDecode::~AACDecode()
{}

//-------------------------------------------------------------------------------------------

void AACDecode::start()
{
    tint i,j;
    
    m_dequantScalefactor = new sample_t [256];
    for(i=0;i<256;++i)
    {
        m_dequantScalefactor[i] = static_cast<sample_t>(pow(2.0,0.25 * static_cast<tfloat64>(i - 100)));
    }
    
    m_intensitySf = new sample_t [1025];
    for(i=-512,j=0;i<=512;++i,++j)
    {
        if(!i)
        {
            m_intensityScalefactor = &m_intensitySf[j];
        }
        m_intensitySf[j] = static_cast<sample_t>(pow(0.5,0.25 * static_cast<tfloat64>(i)));
    }
    
    m_dequantSp = new sample_t [16385];
    for(i=-8192,j=0;i<=8192;++i,++j)
    {
        if(!i)
        {
            m_dequantSpectral = &m_dequantSp[j];
        }
        m_dequantSp[j] = static_cast<sample_t>(pow(static_cast<tfloat64>(abs(i)),4.0 / 3.0));
        if(i<0)
        {
            m_dequantSp[j] = -m_dequantSp[j];
        }
    }
}

//-------------------------------------------------------------------------------------------

void AACDecode::stop()
{
    if(m_dequantScalefactor!=0)
    {
        delete [] m_dequantScalefactor;
        m_dequantScalefactor = 0;
    }
    if(m_dequantSp!=0)
    {
        delete [] m_dequantSp;
        m_dequantSp = 0;
        m_dequantSpectral = 0;
    }
    if(m_intensitySf!=0)
    {
        delete [] m_intensitySf;
        m_intensityScalefactor = 0;
        m_intensitySf = 0;
    }
}

//-------------------------------------------------------------------------------------------

void AACDecode::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "AACDecode::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void AACDecode::setGAConfig(GAConfig *cfg)
{
    m_gaConfig = cfg;
}

//-------------------------------------------------------------------------------------------

void AACDecode::reset()
{
    ::memset(&m_info,0,sizeof(ICSInfo));
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readICS(Sequence *s,bool commonWindow,bool scaleFlag)
{
    WSequence *seq = dynamic_cast<WSequence *>(s);

#if defined(WHITEOMEGA_COMPARE)
    m_ICSFrameNo++;
#endif

    if(seq==0)
    {
        printError("readICS","Bitstream sequence not a white omega sequence");
        return false;        
    }
    
    m_info.globalGain = seq->readBitsI(8);
    
    if(!commonWindow && !scaleFlag)
    {
        if(!readICSInfo(commonWindow,seq))
        {
            return false;
        }
    }
    
    if(!readSectionData(seq))
    {
        return false;
    }
    
    if(!readScalefactor(seq))
    {
        return false;
    }
    
    if(!scaleFlag)
    {
        if(seq->readBitI())
        {
            m_info.pulseDataPresent = true;
            if(!readPulse(seq))
            {
                return false;
            }
        }
        else
        {
            m_info.pulseDataPresent = false;
        }
        
        if(seq->readBitI())
        {
            m_info.tnsDataPresent = true;
            if(!readTNS(seq))
            {
                return false;
            }
        }
        else
        {
            m_info.tnsDataPresent = false;
        }
        
        if(seq->readBitI())
        {
            m_info.gainControlDataPresent = true;
            if(!readGainControlData(seq))
            {
                return false;
            }
        }
        else
        {
            m_info.gainControlDataPresent = false;
        }
    }
    
    ::memset(m_spectralData,0,sizeof(tint) * m_gaConfig->m_frameLength);
    
    if(m_gaConfig->m_aacSpectralDataResilienceFlag)
    {
        if(!m_hcr.read(seq,&m_info,m_gaConfig,m_spectralData))
        {
            return false;
        }
    }
    else
    {
        if(!readSpectralData(seq))
        {
            return false;
        }
    }
    
    if(m_info.pulseDataPresent)
    {
        if(m_info.windowSequence!=EIGHT_SHORT_SEQUENCE)
        {
            decodePulse();
        }
        else
        {
            return false;
        }
    }
    
#if defined(WHITEOMEGA_COMPARE)
    if(g_WCompare!=0)
    {
        g_WCompare->compareICSA(&m_info,m_spectralData,m_gaConfig->m_frameLength);
    }
#endif

    return true;
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readICSInfo(bool commonWindow,WSequence *seq)
{
    if(seq->readBitI())
    {
        return false;
    }
    m_info.windowSequence = seq->readBitsI(2);
    m_info.windowShape = seq->readBitI();
    if(m_info.windowSequence==EIGHT_SHORT_SEQUENCE)
    {
        m_info.maxSfb = seq->readBitsI(4);
        m_info.scaleFactorGrouping = seq->readBitsI(7);
    }
    else
    {
        m_info.maxSfb = seq->readBitsI(6);
        
        m_info.pred.predictorDataPresent = (seq->readBitI()) ? true : false;
        if(m_info.pred.predictorDataPresent)
        {
            if(m_gaConfig->m_audioObjectType==GAConfig::e_audioAACMain)
            {
                tint sfb;
                
                m_info.pred.limit = minV(m_info.maxSfb,maxPredSfb(m_gaConfig->m_samplingFrequencyIndex));
                m_info.pred.predictorReset = (seq->readBitI()) ? true : false;
                if(m_info.pred.predictorReset)
                {
                    m_info.pred.predictorResetGroupNumber = seq->readBitsI(5);
                }
                for(sfb=0;sfb<m_info.pred.limit;++sfb)
                {
                    m_info.pred.predictionUsed[sfb] = seq->readBitI();
                }
            }
            else
            {
                if(m_gaConfig->m_audioObjectType < GAConfig::e_audioERAACLC)
                {
                    m_info.ltp1.dataPresent = (seq->readBitI()) ? true : false;
                    if(m_info.ltp1.dataPresent)
                    {
                        readLTPData(seq,&(m_info.ltp1));
                    }
                    if(commonWindow)
                    {
                        m_info.ltp2.dataPresent = (seq->readBitI()) ? true : false;
                        if(m_info.ltp2.dataPresent)
                        {
                            readLTPData(seq,&(m_info.ltp2));
                        }
                    }
                }
                else if(!commonWindow && m_gaConfig->m_audioObjectType >= GAConfig::e_audioERAACLC)
                {
                    m_info.ltp1.dataPresent = (seq->readBitI()) ? true : false;
                    if(m_info.ltp1.dataPresent)
                    {
                        readLTPData(seq,&(m_info.ltp1));
                    }
                }
            }
        }
    }
    if(!windowGroupingInfo())
    {
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void AACDecode::copyICSInfo(ICSInfo *dInfo)
{
    dInfo->windowSequence = m_info.windowSequence;
    dInfo->windowShape = m_info.windowShape;
    dInfo->maxSfb = m_info.maxSfb;
    if(dInfo->windowSequence==EIGHT_SHORT_SEQUENCE)
    {
        dInfo->scaleFactorGrouping = m_info.scaleFactorGrouping;
    }
    else
    {
        dInfo->pred.predictorDataPresent = m_info.pred.predictorDataPresent;
        if(dInfo->pred.predictorDataPresent)
        {
            if(m_gaConfig->m_audioObjectType==GAConfig::e_audioAACMain)
            {
                ::memcpy(&(dInfo->pred),&(m_info.pred),sizeof(PredictionInfo));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readLTPData(WSequence *seq,LTPInfo *ltp)
{
    tint sfb,limit;
    
    if(m_gaConfig->m_audioObjectType==GAConfig::e_audioERAACLD)
    {
        ltp->lagUpdate = (seq->readBitI()) ? true : false;
        if(ltp->lagUpdate)
        {
            ltp->lag = seq->readBitsI(10);
        }
        ltp->coefficient = seq->readBitI();
        
        ltp->lastBand = limit = minV(m_info.maxSfb,MAX_LTP_SFB);
        for(sfb=0;sfb<limit;++sfb)
        {
            ltp->longUsed[sfb] = seq->readBitI();
        }
    }
    else
    {
        ltp->lag = seq->readBitsI(11);
        ltp->coefficient = seq->readBitsI(3);
        if(m_info.windowSequence!=EIGHT_SHORT_SEQUENCE)
        {
            ltp->lastBand = limit = minV(m_info.maxSfb,MAX_LTP_SFB);
            for(sfb=0;sfb<limit;++sfb)
            {
                ltp->longUsed[sfb] = seq->readBitI();
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACDecode::windowGroupingInfo()
{
    tint g,i,sfIndex = m_gaConfig->m_samplingFrequencyIndex;
    tint *swbOffset;
    
    switch(m_info.windowSequence)
    {
        case ONLY_LONG_SEQUENCE:
        case LONG_START_SEQUENCE:
        case LONG_STOP_SEQUENCE:
            {
                m_info.numWindows = 1;
                m_info.numWindowGroups = 1;
                m_info.windowGroupLength[0] = 1;
                if(m_gaConfig->m_audioObjectType==GAConfig::e_audioERAACLD)
                {
                    if(m_gaConfig->m_frameLength==512)
                    {
                        m_info.numSwb = GAConfig::m_swbWindowSize_512[sfIndex];
                    }
                    else
                    {
                        m_info.numSwb = GAConfig::m_swbWindowSize_480[sfIndex];
                    }
                }
                else
                {
                    if(m_gaConfig->m_frameLength==1024)
                    {
                        m_info.numSwb = GAConfig::m_swbWindowSize_1024[sfIndex];
                    }
                    else
                    {
                        m_info.numSwb = GAConfig::m_swbWindowSize_960[sfIndex];
                    }
                }
            
                if(m_info.maxSfb > m_info.numSwb)
                {
                    return false;
                }
                
                if(m_gaConfig->m_audioObjectType==GAConfig::e_audioERAACLD)
                {
                    if(m_gaConfig->m_frameLength==512)
                    {
                        swbOffset = GAConfig::m_swbOffset_512[sfIndex];
                    }
                    else
                    {
                        swbOffset = GAConfig::m_swbOffset_480[sfIndex];
                    }
                }
                else
                {
                    swbOffset = GAConfig::m_swbOffset_1024[sfIndex];
                }
                for(i=0;i<m_info.numSwb;++i)
                {
                    m_info.sectSfbOffset[0][i] = swbOffset[i];
                    m_info.swbOffset[i] = swbOffset[i];
                }
                m_info.sectSfbOffset[0][i] = m_gaConfig->m_frameLength;
                m_info.swbOffset[i] = m_gaConfig->m_frameLength;
                m_info.swbOffsetMax = m_gaConfig->m_frameLength;
            }
            break;
            
        case EIGHT_SHORT_SEQUENCE:
            {
                m_info.numWindows = 8;
                m_info.numWindowGroups = 1;
                m_info.windowGroupLength[0] = 1;
                m_info.numSwb = GAConfig::m_swbWindowSize_128[sfIndex];
                
                if(m_info.maxSfb > m_info.numSwb)
                {
                    return false;
                }
                
                swbOffset = GAConfig::m_swbOffset_128[sfIndex];
                for(i=0;i<m_info.numSwb;++i)
                {
                    m_info.swbOffset[i] = swbOffset[i];
                }
                m_info.swbOffset[i] = m_gaConfig->m_frameLength >> 3;
                m_info.swbOffsetMax = m_gaConfig->m_frameLength >> 3;
                
                for(i=0;i<(m_info.numWindows - 1);++i)
                {
                    if(bitSet(m_info.scaleFactorGrouping,6 - i)==0)
                    {
                        m_info.numWindowGroups += 1;
                        m_info.windowGroupLength[m_info.numWindowGroups - 1] = 1;
                    }
                    else
                    {
                        m_info.windowGroupLength[m_info.numWindowGroups - 1] += 1;
                    }
                }
                
                for(g=0;g<m_info.numWindowGroups;++g)
                {
                    tint width, sectSfb = 0, offset = 0;
                    
                    for(i=0;i<m_info.numSwb;++i)
                    {
                        if(m_info.numSwb==(i+1))
                        {
                            width = (m_gaConfig->m_frameLength >> 3) - swbOffset[i];
                        }
                        else
                        {
                            width = swbOffset[i + 1] - swbOffset[i];
                        }
                        width *= m_info.windowGroupLength[g];
                        m_info.sectSfbOffset[g][sectSfb++] = offset;
                        offset += width;
                    }
                    m_info.sectSfbOffset[g][sectSfb] = offset;
                }
            }
            break;
            
        default:
            return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readSectionData(WSequence *seq)
{
    tint i,j,k,l,sfb,cb;
    tint sectBits,sectEscVal,sectLen,sectLenInc;
    tint *sfbCb,*sectCb,*sectStart,*sectEnd;
    
    if(m_info.windowSequence==EIGHT_SHORT_SEQUENCE)
    {
        sectEscVal = (1 << 3) - 1;
        sectBits = 3;
    }
    else
    {
        sectEscVal = (1 << 5) - 1;
        sectBits = 5;
    }
    m_info.intensityFlag = false;
    m_info.noiseFlag = false;
    
    if(m_gaConfig->m_aacSectionDataResilienceFlag)
    {
        for(j=0;j<m_info.numWindowGroups;++j)
        {
            sfbCb = &(m_info.sfbCb[j][0]);
            sectCb = &(m_info.sectCb[j][0]);
            sectStart = &(m_info.sectStart[j][0]);
            sectEnd = &(m_info.sectEnd[j][0]);
            
            for(k=0,i=0;k<m_info.maxSfb;++i)
            {
                cb = seq->readBitsI(5);
                switch(cb)
                {
                    case INTENSITY_HCB:
                    case INTENSITY_HCB2:
                        m_info.intensityFlag = true;
                        break;
                        
                    case NOISE_HCB:
                        m_info.noiseFlag = true;
                        break;
                }
                
                if(cb<11 || (cb>11 && cb<16))
                {
                    sectLen = 0;
                    while(sectLenInc = seq->readBitsI(sectBits),sectLenInc==sectEscVal)
                    {
                        sectLen += sectEscVal;
                    }
                    sectLen += sectLenInc;
                }
                else
                {
                    sectLen = 1;
                }
                l = k + sectLen;
                
                sectCb[i] = cb;
                sectStart[i] = k;
                sectEnd[i] = l;
                
                for(sfb=k;sfb<l;++sfb)
                {
                    sfbCb[sfb] = cb;
                }
                
                k = l;
            }
            m_info.numSec[j] = i;
        }
    }
    else
    {
        for(j=0;j<m_info.numWindowGroups;++j)
        {
            sfbCb = &(m_info.sfbCb[j][0]);
            sectCb = &(m_info.sectCb[j][0]);
            sectStart = &(m_info.sectStart[j][0]);
            sectEnd = &(m_info.sectEnd[j][0]);
        
            for(k=0,i=0;k<m_info.maxSfb;++i)
            {
                cb = seq->readBitsI(4);
                switch(cb)
                {
                    case INTENSITY_HCB:
                    case INTENSITY_HCB2:
                        m_info.intensityFlag = true;
                        break;
                        
                    case NOISE_HCB:
                        m_info.noiseFlag = true;
                        break;
                }
                
                sectLen = 0;
                while(sectLenInc = seq->readBitsI(sectBits),sectLenInc==sectEscVal)
                {
                    sectLen += sectEscVal;
                }
                sectLen += sectLenInc;

                l = k + sectLen;
                
                sectCb[i] = cb;
                sectStart[i] = k;
                sectEnd[i] = l;
                
                for(sfb=k;sfb<l;++sfb)
                {
                    sfbCb[sfb] = cb;
                }
                
                k = l;
            }
            m_info.numSec[j] = i;
        }        
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readScalefactor(WSequence *seq)
{    
    if(!m_gaConfig->m_aacScalefactorDataResilienceFlag)
    {
        return readScalefactorNormal(seq);
    }
    else
    {
        return readScalefactorRLVC(seq);
    }
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readScalefactorNormal(WSequence *seq)
{
    tint t,g,sfb;
    tint *cb,*sf;
    tint isPosition,scaleFactor,noiseEnergy;
    bool noiseFlag = true;
    
    isPosition = 0;
    scaleFactor = m_info.globalGain;
    noiseEnergy = m_info.globalGain - 90;
    
    for(g=0;g<m_info.numWindowGroups;++g)
    {
        cb = &(m_info.sfbCb[g][0]);
        sf = &(m_info.scaleFactors[g][0]);
        
        for(sfb=0;sfb<m_info.maxSfb;++sfb)
        {
            switch(cb[sfb])
            {
                case ZERO_HCB:
                    sf[sfb] = 0;
                    break;
                    
                case INTENSITY_HCB:
                case INTENSITY_HCB2:
                    isPosition += seq->decodeSf();
                    sf[sfb] = isPosition;
                    break;
                    
                case NOISE_HCB:
                    if(noiseFlag)
                    {
                        t = seq->readBitsI(9);
                        noiseFlag = false;
                    }
                    else
                    {
                        t = seq->decodeSf();
                    }
                    noiseEnergy += t;
                    sf[sfb] = noiseEnergy;
                    break;
                    
                default:
                    scaleFactor += seq->decodeSf();
                    sf[sfb] = scaleFactor;
                    break;
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readScalefactorRLVC(WSequence *seq)
{
    tint t,g,e,sfb,escNo;
    tint *cb,*sf,*esc;
    tint startOffset,rvlcOffset,escapeOffset;
    tint isPosition,noiseEnergy,scaleFactor;
    bool noiseFlag = false,escapePresent,errorFlag = false;
        
    m_info.sfConcealment = seq->readBitI();
    m_info.revGlobalGain = seq->readBitsI(8);
    if(m_info.windowSequence==EIGHT_SHORT_SEQUENCE)
    {
        m_info.lengthOfRvlcSf = seq->readBitsI(11);
    }
    else
    {
        m_info.lengthOfRvlcSf = seq->readBitsI(9);
    }
    
    startOffset = seq->offset();
    rvlcOffset = startOffset + m_info.lengthOfRvlcSf;
    seq->setOffset(rvlcOffset);
    escapePresent = (seq->readBitI()) ? true : false;
    escapeOffset = seq->offset();
    
    if(escapePresent)
    {
        m_info.lengthOfRvlcEscapes = seq->readBitsI(8);
        escapeOffset += m_info.lengthOfRvlcEscapes;
        
        esc = &(m_info.escapeSF[0]);
        for(g=0;seq->offset() < escapeOffset;g++)
        {
            esc[g] = seq->decodeRVLCEsc();
        }
        escNo = g;
    }
    else
    {
        escNo = 0;
    }
    
    isPosition = 0;
    scaleFactor = m_info.globalGain;
    noiseEnergy = m_info.globalGain - 90;
    seq->setOffset(startOffset);
    
    try
    {
        for(e=0,g=0;g<m_info.numWindowGroups;++g)
        {
            cb = &(m_info.sfbCb[g][0]);
            sf = &(m_info.scaleFactors[g][0]);
            
            for(sfb=0;sfb<m_info.maxSfb;++sfb)
            {
                switch(cb[sfb])
                {
                    case ZERO_HCB:
                        sf[sfb] = 0;
                        break;
                        
                    case INTENSITY_HCB:
                    case INTENSITY_HCB2:
                        t = seq->decodeRVLC();
                        if(t>=7)
                        {
                            t += esc[e++];
                        }
                        else if(t<=-7)
                        {
                            t -= esc[e++];
                        }
                        isPosition += t;
                        sf[sfb] = isPosition;
                        break;
                    
                    case NOISE_HCB:
                        if(!noiseFlag)
                        {
                            t = seq->readBitsI(9);
                            noiseFlag = true;
                        }
                        else
                        {
                            t = seq->decodeRVLC();
                            if(t>=7)
                            { 
                                t += esc[e++];
                            }
                            else if(t<=-7)
                            {
                                t -= esc[e++];
                            }
                        }
                        noiseEnergy += t;
                        sf[sfb] = noiseEnergy;
                        break;
                    
                    default:
                        t = seq->decodeRVLC();
                        if(t>=7)
                        {
                            t += esc[e++];
                        }
                        else if(t<=-7)
                        {
                            t -= esc[e++];
                        }
                        scaleFactor += t;
                        sf[sfb] = scaleFactor;
                        break;
                }
            }
        }
    }
    catch(WSequenceException&)
    {
        errorFlag = true;
        
        if(g<m_info.numWindowGroups)
        {
            sf = &(m_info.scaleFactors[g][0]);
            while(sfb<m_info.maxSfb)
            {
                sf[sfb++] = 0;
            }
            g++;
            while(g<m_info.numWindowGroups)
            {
                sf = &(m_info.scaleFactors[g][0]);
                for(sfb=0;sfb<m_info.maxSfb;++sfb)
                {
                    sf[sfb++] = 0;
                }
                g++;
            }
        }
    }
    
    if(errorFlag)
    {
        try
        {
            scaleFactor = m_info.revGlobalGain;
            if(m_info.noiseFlag)
            {
                seq->setOffset(escapeOffset);
                noiseEnergy = seq->readBitsI(9);
            }
            seq->setOffset(rvlcOffset);
            e = escNo - 1;
            if(m_info.intensityFlag)
            {
                isPosition = seq->decodeRVLCRev();
                if(isPosition>=7)
                {
                    isPosition += esc[e++];
                }
                else if(isPosition<=-7)
                {
                    isPosition -= esc[e--];
                }
            }
        
            for(g=(m_info.numWindowGroups - 1);g>=0;--g)
            {
                cb = &(m_info.sfbCb[g][0]);
                sf = &(m_info.scaleFactors[g][0]);
            
                for(sfb=(m_info.maxSfb - 1);sfb>=0;--sfb)
                {
                    switch(cb[sfb])
                    {
                        case INTENSITY_HCB:
                        case INTENSITY_HCB2:
                            sf[sfb] = isPosition;
                            t = seq->decodeRVLCRev();
                            if(t>=7)
                            {
                                t += esc[e--];
                            }
                            else if(t<=-7)
                            {
                                t -= esc[e--];
                            }
                            isPosition -= t;
                            break;
                        
                        case NOISE_HCB:
                            sf[sfb] = noiseEnergy;
                            t = seq->decodeRVLCRev();
                            if(t>=7)
                            {
                                t += esc[e--];
                            }
                            else if(t<=-7)
                            {
                                t -= esc[e--];
                            }
                            noiseEnergy -= t;
                            break;
                        
                        default:
                            sf[sfb] = scaleFactor;
                            t = seq->decodeRVLCRev();
                            if(t>=7)
                            {
                                t += esc[e--];
                            }
                            else if(t<=-7)
                            {
                                t -= esc[e--];
                            }
                            scaleFactor -=t;
                            break;
                    }
                }
            }
        }
        catch(WSequenceException&) {}
    }
    
    if(m_info.noiseFlag)
    {
        seq->setOffset(escapeOffset + 9);
    }
    else
    {
        seq->setOffset(escapeOffset);
    }
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readPulse(WSequence *seq)
{
    tint i;
    PulseData *p = &(m_info.pulse);
    
    p->numberPulse = seq->readBitsI(2);
    p->pulseStartSfb = seq->readBitsI(6);
    for(i=0;i<=p->numberPulse;++i)
    {
        p->pulseOffset[i] = seq->readBitsI(5);
        p->pulseAmp[i] = seq->readBitsI(4);
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void AACDecode::decodePulse()
{
    tint i,k;
    PulseData *p = &(m_info.pulse);
    
    k = minV(m_info.swbOffset[p->pulseStartSfb],m_info.swbOffsetMax);
    
    for(i=0;i<=p->numberPulse;++i)
    {
        k += p->pulseOffset[i];
        
        if(m_spectralData[k] > 0)
        {
            m_spectralData[k] += p->pulseAmp[i];
        }
        else
        {
            m_spectralData[k] -= p->pulseAmp[i];
        }
    }
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readTNS(WSequence *seq)
{
    tint i,w,filt,nFilt,order;
    tint nBits,oBits,lBits,cBits,scBits;
    tint *coef;
    TNSData *tns = &(m_info.tns);
    
    if(m_info.windowSequence==EIGHT_SHORT_SEQUENCE)
    {
        nBits = 1;
        lBits = 4;
        oBits = 3;
    }
    else
    {
        nBits = 2;
        lBits = 6;
        oBits = 5;
    }
    
    for(w=0;w<m_info.numWindows;++w)
    {
        tns->nFilt[w] = nFilt = seq->readBitsI(nBits);
        if(nFilt)
        {
            tns->coefRes[w] = seq->readBitI();
            scBits = (tns->coefRes[w]) ? 4 : 3;
            
            for(filt=0;filt<nFilt;++filt)
            {
                tns->length[w][filt] = seq->readBitsI(lBits);
                tns->order[w][filt] = order = seq->readBitsI(oBits);
                if(order)
                {
                    tns->direction[w][filt] = seq->readBitI();
                    tns->coefCompress[w][filt] = seq->readBitI();
                    coef = &(tns->coef[w][filt][0]);
                    cBits = scBits - tns->coefCompress[w][filt];
                    for(i=0;i<order;++i)
                    {
                        coef[i] = seq->readBitsI(cBits);
                    }
                }
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readGainControlData(WSequence *seq)
{
    tint bd,ad,wd,maxBand,adSize;
    SSRInfo *ssr = &(m_info.ssr);
    
    ssr->maxBand = maxBand = seq->readBitsI(2);
    if(m_info.windowSequence==ONLY_LONG_SEQUENCE)
    {
        for(bd=1;bd<=maxBand;++bd)
        {
            ssr->adjustNum[bd][0] = adSize = seq->readBitsI(3);
            for(ad=0;ad<adSize;++ad)
            {
                ssr->alevcode[bd][0][ad] = seq->readBitsI(4);
                ssr->aloccode[bd][0][ad] = seq->readBitsI(5);
            }
        }
    }
    else if(m_info.windowSequence==LONG_START_SEQUENCE)
    {
        for(bd=1;bd<=maxBand;++bd)
        {
            ssr->adjustNum[bd][0] = adSize = seq->readBitsI(3);
            for(ad=0;ad<adSize;++ad)
            {
                ssr->alevcode[bd][0][ad] = seq->readBitsI(4);
                ssr->aloccode[bd][0][ad] = seq->readBitsI(4);
            }
            ssr->adjustNum[bd][1] = adSize = seq->readBitsI(3);
            for(ad=0;ad<adSize;++ad)
            {
                ssr->alevcode[bd][1][ad] = seq->readBitsI(4);
                ssr->aloccode[bd][1][ad] = seq->readBitsI(2);
            }
        }
    }
    else if(m_info.windowSequence==EIGHT_SHORT_SEQUENCE)
    {
        for(bd=1;bd<=maxBand;++bd)
        {
            for(wd=0;wd<8;++wd)
            {
                ssr->adjustNum[bd][wd] = adSize =  seq->readBitsI(3);
                for(ad=0;ad<adSize;++ad)
                {
                    ssr->alevcode[bd][wd][ad] = seq->readBitsI(4);
                    ssr->aloccode[bd][wd][ad] = seq->readBitsI(2);
                }
            }
        }
    }
    else if(m_info.windowSequence==LONG_STOP_SEQUENCE)
    {
        for(bd=1;bd<=maxBand;++bd)
        {
            ssr->adjustNum[bd][0] = adSize = seq->readBitsI(3);
            for(ad=0;ad<adSize;++ad)
            {
                ssr->alevcode[bd][0][ad] = seq->readBitsI(4);
                ssr->aloccode[bd][0][ad] = seq->readBitsI(4);
            }
            ssr->adjustNum[bd][1] = adSize = seq->readBitsI(3);
            for(ad=0;ad<adSize;++ad)
            {
                ssr->alevcode[bd][1][ad] = seq->readBitsI(4);
                ssr->aloccode[bd][1][ad] = seq->readBitsI(5);
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool AACDecode::readSpectralData(WSequence *seq)
{
    tint g,i,p,groups = 0,cb,pEnd,nShorts = (m_gaConfig->m_frameLength >> 3);
    tint *secCb,*secStart,*secEnd,*offset;
    
    for(g=0;g<m_info.numWindowGroups;++g)
    {
        p = groups * nShorts;
        
        secStart = &(m_info.sectStart[g][0]);
        secEnd = &(m_info.sectEnd[g][0]);
        secCb = &(m_info.sectCb[g][0]);
        offset = &(m_info.sectSfbOffset[g][0]);
        
        for(i=0;i<m_info.numSec[g];++i)
        {
            pEnd = p + (offset[secEnd[i]] - offset[secStart[i]]);
            cb = secCb[i];
            
            switch(cb)
            {
                case 1:
                case 2:
                    while(p < pEnd)
                    {
                        seq->decodeSigned4(cb,&m_spectralData[p]);
                        p += 4;
                    }
                    break;
            
                case 3:
                case 4:
                    while(p < pEnd)
                    {
                        seq->decodeUnsigned4(cb,&m_spectralData[p]);
                        p += 4;
                    }
                    break;
            
                case 5:
                case 6:
                    while(p < pEnd)
                    {
                        seq->decodeSigned2(cb,&m_spectralData[p]);
                        p += 2;
                    }
                    break;
            
                case 7:
                case 8:
                case 9:
                case 10:
                    while(p < pEnd)
                    {
                        seq->decodeUnsigned2(cb,&m_spectralData[p]);
                        p += 2;
                    }
                    break;
            
                case 11:
                case 16:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:
                case 31:
                    while(p < pEnd)
                    {
                        seq->decodeEsc(cb,&m_spectralData[p]);
                        p += 2;
                    }
                    break;
                
                case ZERO_HCB:
                case NOISE_HCB:
                case INTENSITY_HCB:
                case INTENSITY_HCB2:
                default:
                    p = pEnd;
                    break;
            }
        }
        groups += m_info.windowGroupLength[g];
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void AACDecode::dequantize()
{
    tint g,j,sfb,win,bin,width;
    tint scfIndex;
    tint gIndex,gIncrease,wa,winInc;
    sample_t scf;
    sample_t *sC;
    tint32 *sF,*sD = m_spectralData;
    
    gIndex = 0;
    winInc = m_info.swbOffset[ m_info.numSwb ];
    
    for(g=0;g<m_info.numWindowGroups;++g)
    {
        j = 0;
        gIncrease = 0;
        sF = &(m_info.scaleFactors[g][0]);
        
        for(sfb=0;sfb<m_info.numSwb;++sfb)
        {
            width = m_info.swbOffset[sfb + 1] - m_info.swbOffset[sfb];
            
            scfIndex = sF[sfb];
            if(scfIndex>=0 && scfIndex<256)
            {
                scf = m_dequantScalefactor[ scfIndex ];
            }
            else
            {
                scf = m_dequantScalefactor[ 0 ];
            }
            
            wa = gIndex + j;
            sC = &m_spectralCoef[gIndex + j];
            
            for(win=0;win<m_info.windowGroupLength[g];++win)
            {
                for(bin=0;bin<width;bin+=4,sC+=4,sD+=4)
                {
                    sC[0] = m_dequantSpectral[ sD[0] ] * scf;
                    sC[1] = m_dequantSpectral[ sD[1] ] * scf;
                    sC[2] = m_dequantSpectral[ sD[2] ] * scf;
                    sC[3] = m_dequantSpectral[ sD[3] ] * scf;
                }
                sC += winInc - bin;
                gIncrease += bin;
            }
            j += width;
        }
        gIndex += gIncrease;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
