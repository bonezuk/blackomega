#include "engine/whiteomega/inc/PredictionLTP.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

PredictionLTP::PredictionLTP() : m_alloc(),
    m_gaConfig(0),
    m_channel(0),
    m_window(0),
    m_tns(0),
    m_state(0),
    m_stateLen(0)
{
    m_xIn = reinterpret_cast<sample_t *>(m_alloc.MemAllocAlign(2048,sizeof(sample_t),16));
}

//-------------------------------------------------------------------------------------------

PredictionLTP::~PredictionLTP()
{
    if(m_state!=0)
    {
        delete [] m_state;
        m_state = 0;
    }
    m_alloc.FreeAll();
}

//-------------------------------------------------------------------------------------------

void PredictionLTP::set(GAConfig *cfg,AACDecode *ch,Window *win,TNS *tns)
{
    m_gaConfig = cfg;
    m_channel = ch;
    m_window = win;
    m_tns = tns;
    
    if(m_state!=0)
    {
        delete [] m_state;
        m_state = 0;
    }
    m_stateLen = m_gaConfig->m_frameLength << 2;
    m_state = new tint16 [m_stateLen];
    reset();
}

//-------------------------------------------------------------------------------------------

void PredictionLTP::reset()
{
    ::memset(m_state,0,m_stateLen * sizeof(tint16));
}

//-------------------------------------------------------------------------------------------

bool PredictionLTP::isLTP() const
{
    switch(m_gaConfig->m_audioObjectType)
    {
        case GAConfig::e_audioAACLTP:
        case GAConfig::e_audioERAACLTP:
        case GAConfig::e_audioERAACLD:
            return true;
            
        default:
            return false;
    }
}

//-------------------------------------------------------------------------------------------

void PredictionLTP::update()
{
    tint i,len = m_gaConfig->m_frameLength;
    tint lenB = len * 2,lenC = len * 3;
    sample_t *time = &(m_window->m_out[0]);
    sample_t *over = &(m_window->m_out[m_window->m_prevFrameLen]);
    
    if(m_gaConfig->m_audioObjectType==GAConfig::e_audioERAACLD)
    {
        for(i=0;i<len;++i)
        {
            m_state[i] = m_state[i + len];
            m_state[len + i] = m_state[i + lenB];
            m_state[lenB + i] = realToShort(time[i]);
            m_state[lenC + i] = realToShort(over[i]);
        }
    }
    else
    {
        for(i=0;i<len;++i)
        {
            m_state[i] = m_state[i + len];
            m_state[len + i] = realToShort(time[i]);
            m_state[lenB + i] = realToShort(over[i]);
        }
    }
}

//-------------------------------------------------------------------------------------------

void PredictionLTP::predict(LTPInfo *ltp)
{
#if defined(SINGLE_FLOAT_SAMPLE)
    static const sample_t codebook[8] = {
        0.570829f, 0.696616f, 0.813004f, 0.911304f,
        0.984900f, 1.067894f, 1.194601f, 1.369533f
    };
#else
    static const sample_t codebook[8] = {
        0.570829, 0.696616, 0.813004, 0.911304,
        0.984900, 1.067894, 1.194601, 1.369533
    };
#endif

    ICSInfo *info = &(m_channel->m_info);
    sample_t *spec = m_channel->m_spectralCoef;
    
    if(ltp->dataPresent && info->windowSequence!=EIGHT_SHORT_SEQUENCE)
    {
        tint i,j,sfb,len = 0,numSamples = m_gaConfig->m_frameLength << 1;
        sample_t coef = codebook[ltp->coefficient];
        sample_t *xOut;
        
        i = 0;
        j = numSamples + i - ltp->lag;
        while(i<numSamples)
        {
            m_xIn[i++] = coef * m_state[j++];
        }
        
        xOut = m_window->processLTP(m_xIn,len);
        
        m_tns->encodeLTP(xOut);
        
        for(sfb=0;sfb<ltp->lastBand;++sfb)
        {
            if(ltp->longUsed[sfb])
            {
                tint low = info->swbOffset[sfb];
                tint high = minV(info->swbOffset[sfb+1],info->swbOffsetMax);
                
                for(i=low;i<high;++i)
                {
                    spec[i] += xOut[i];
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
