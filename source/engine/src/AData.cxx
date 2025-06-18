#include "engine/inc/AData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

AData::AData() : m_data(0),
    m_outData(0),
    m_length(20 * 576),
    m_noChannels(2),
    m_noOutChannels(2),
    m_start(),
    m_end(),
    m_completeFlag(false),
    m_filterDataMap(),
    m_centreData(NULL),
    m_isCenterValid(false)
{
    AData::init();
}

//-------------------------------------------------------------------------------------------

AData::AData(tint len,tint inChannel,tint outChannel) : m_data(0),
    m_outData(0),
    m_length(len),
    m_noChannels(inChannel),
    m_noOutChannels(outChannel),
    m_start(),
    m_end(),
    m_completeFlag(false),
    m_filterDataMap(),
    m_centreData(NULL),
    m_isCenterValid(false)
{
    AData::init();
}

//-------------------------------------------------------------------------------------------

AData::AData(const AData& rhs) : m_data(0),
    m_outData(0),
    m_length(0),
    m_noChannels(0),
    m_noOutChannels(0),
    m_start(),
    m_end(),
    m_completeFlag(false),
    m_filterDataMap(),
    m_centreData(NULL),
    m_isCenterValid(false)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

AData::~AData()
{
    try
    {
        if(m_data!=0)
        {
            delete [] m_data;
            m_data = 0;
        }
        if(m_outData!=0)
        {
            delete [] m_outData;
            m_outData = 0;
        }
        if(m_centreData != 0)
        {
            delete [] m_centreData;
            m_centreData = 0;
        }
        AData::freeFilterData();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void AData::freeFilterData()
{
    for(QMap<tint, sample_t *>::iterator ppI = m_filterDataMap.begin(); ppI != m_filterDataMap.end(); ppI++)
    {
        sample_t *filterData = ppI.value();
        delete [] filterData;
    }
    m_filterDataMap.clear();
}

//-------------------------------------------------------------------------------------------

sample_t *AData::data()
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

const sample_t *AData::dataConst() const
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

tint AData::length() const
{
    return m_length;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& AData::start()
{
    return m_start;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AData::startConst() const
{
    return m_start;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& AData::end()
{
    return m_end;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& AData::endConst() const
{
    return m_end;
}

//-------------------------------------------------------------------------------------------

void AData::reset()
{
    int len = m_length;
    for(QMap<tint, sample_t *>::iterator ppI = m_filterDataMap.begin(); ppI != m_filterDataMap.end(); ppI++)
    {
        sample_t *filterData = ppI.value();
        for(int i = 0; i < len; i++)
        {
            filterData[i] = 0.0;
        }
    }
    if(m_centreData != 0)
    {
        for(tint i = 0 ; i < len; i++)
        {
            m_centreData[i] = 0.0;
        }
    }
    m_isCenterValid = false;
}

//-------------------------------------------------------------------------------------------

bool AData::isComplete() const
{
    return m_completeFlag;
}

//-------------------------------------------------------------------------------------------

void AData::setComplete(bool flag)
{
    m_completeFlag = flag;
}

//-------------------------------------------------------------------------------------------

tint AData::noInChannels() const
{
    return m_noChannels;
}

//-------------------------------------------------------------------------------------------

tint AData::noOutChannels() const
{
    return m_noOutChannels;
}

//-------------------------------------------------------------------------------------------

const AData& AData::operator = (const AData& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void AData::init()
{
    if(m_data!=0)
    {
        delete [] m_data;
    }
    m_data = new sample_t [m_length * m_noChannels];

    freeFilterData();

    if(m_outData!=0)
    {
        delete [] m_outData;
        m_outData = 0;
    }    
    if(m_noChannels!=m_noOutChannels)
    {
        m_outData = new sample_t [m_length * m_noOutChannels];
    }

    if(m_centreData != 0)
    {
        delete [] m_centreData;
        m_centreData = 0;
    }
    m_isCenterValid = false;
}

//-------------------------------------------------------------------------------------------

void AData::copy(const AData& rhs)
{
    int len = rhs.m_length * rhs.m_noChannels;
    int outLen = rhs.m_length * rhs.m_noOutChannels;
    
    if(m_data!=0)
    {
        delete [] m_data;
    }
    m_data = new sample_t [ len ];
    ::memcpy(m_data,rhs.m_data,sizeof(sample_t) * len);
    
    if(m_outData!=0)
    {
        delete [] m_outData;
        m_outData = 0;
    }
    if(rhs.m_outData!=0)
    {
        m_outData = new sample_t [ outLen ];
        ::memcpy(m_outData,rhs.m_outData,sizeof(sample_t) * outLen);
    }
    
    if(m_centreData != 0)
    {
        delete [] m_centreData;
        m_centreData = 0;
    }
    if(rhs.m_centreData != 0)
    {
        m_centreData = new sample_t [ outLen ];
        ::memcpy(m_centreData, rhs.m_centreData, sizeof(sample_t) * rhs.m_length);
    }
    m_isCenterValid = rhs.m_isCenterValid;
    
    freeFilterData();
    for(QMap<tint, sample_t *>::const_iterator ppI = rhs.m_filterDataMap.begin(); ppI != rhs.m_filterDataMap.end(); ppI++)
    {
        sample_t *pA = ppI.value();
        sample_t *pB = new sample_t [len];
        ::memcpy(pB, pA, sizeof(sample_t) * rhs.m_length);
        m_filterDataMap.insert(ppI.key(), pB);
    }

    m_length = rhs.m_length;
    m_noChannels = rhs.m_noChannels;
    m_noOutChannels = rhs.m_noOutChannels;
    m_start = rhs.m_start;
    m_end = rhs.m_end;
    m_completeFlag = rhs.m_completeFlag;
}

//-------------------------------------------------------------------------------------------

sample_t *AData::dataOut()
{
    if(m_outData!=0)
    {
        return m_outData;
    }
    else
    {
        return m_data;
    }
}

//-------------------------------------------------------------------------------------------

const sample_t *AData::dataOutConst() const
{
    if(m_outData!=0)
    {
        return m_outData;
    }
    else
    {
        return m_data;
    }
}

//-------------------------------------------------------------------------------------------

bool AData::isMixing() const
{
    return (m_outData != NULL) ? 1 : 0;
}

//-------------------------------------------------------------------------------------------

sample_t *AData::filterData(tint filterIdx)
{
    sample_t *f;
    QMap<tint, sample_t *>::iterator ppI = m_filterDataMap.find(filterIdx);
    if(ppI != m_filterDataMap.end())
    {
        f = ppI.value();
    }
    else
    {
        tint len = m_length * ((filterIdx >= 0) ? m_noChannels : 1);
        f = new sample_t [len];
        for(tint i = 0; i < len; i++)
        {
            f[i] = 0.0;
        }
        m_filterDataMap.insert(filterIdx, f);
    }
    return f;
}

//-------------------------------------------------------------------------------------------

const sample_t *AData::filterDataConst(tint filterIdx) const
{
    const sample_t *f;
    QMap<tint, sample_t *>::const_iterator ppI = m_filterDataMap.find(filterIdx);
    if(ppI != m_filterDataMap.end())
    {
        f = ppI.value();
    }
    else
    {
        f = 0;
    }
    return f;
}

//-------------------------------------------------------------------------------------------

sample_t *AData::center()
{
    if(m_centreData == 0)
    {
        m_centreData = new sample_t [m_length];
    }
    if(!m_isCenterValid)
    {
        const sample_t *d = dataConst();
        
        for(tint idx = 0; idx < m_length; idx++)
        {
            sample_t x = 0.0f;
            
            for(tint ch = 0; ch < m_noChannels; ch++)
            {
                x += *d++;
            }
            m_centreData[idx] = x / static_cast<tfloat64>(m_noChannels);
        }
        m_isCenterValid = true;
    }
    return m_centreData;
}

//-------------------------------------------------------------------------------------------

bool AData::isCenter() const
{
    return (m_centreData != NULL) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool AData::isLFE() const
{
    QMap<tint, sample_t *>::const_iterator ppI = m_filterDataMap.find(e_lfeChannelIndex);
    return (ppI != m_filterDataMap.end()) ? true : false;
}

//-------------------------------------------------------------------------------------------
// Type A = 0.Center
// Type B = 0.F-Left, 1.F-Right
// Type C = 0.F-Left, 1.F-Right, 2.Center
// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
//-------------------------------------------------------------------------------------------

void AData::mixChannels()
{
    if(!isMixing())
        return;

    if(m_noChannels==1)
    {
        switch(m_noOutChannels-1)
        {
            case 0:
                mixAToA();
                break;
            
            case 1:
            default:
                mixAToB();
                break;
                
            case 2:
                mixAToC();
                break;
                
            case 3:
                mixAToD();
                break;
                
            case 4:
                mixAToE();
                break;
                
            case 5:
                mixAToF();
                break;
                
            case 6:
                mixAToG();
                break;
                
            case 7:
                mixAToH();
                break;
        }
    }
    else if(m_noChannels==2)
    {
        switch(m_noOutChannels-1)
        {
            case 0:
                mixBToA();
                break;
            
            case 1:
            default:
                mixBToB();
                break;
                
            case 2:
                mixBToC();
                break;
                
            case 3:
                mixBToD();
                break;
                
            case 4:
                mixBToE();
                break;
                
            case 5:
                mixBToF();
                break;
                
            case 6:
                mixBToG();
                break;
                
            case 7:
                mixBToH();
                break;
        }
    }
    else if(m_noChannels==3)
    {
        switch(m_noOutChannels-1)
        {
            case 0:
                mixCToA();
                break;
            
            case 1:
            default:
                mixCToB();
                break;
                
            case 2:
                mixCToC();
                break;
                
            case 3:
                mixCToD();
                break;
                
            case 4:
                mixCToE();
                break;
                
            case 5:
                mixCToF();
                break;
                
            case 6:
                mixCToG();
                break;
                
            case 7:
                mixCToH();
                break;
        }
    }
    else if(m_noChannels==4)
    {
        switch(m_noOutChannels-1)
        {
            case 0:
                mixDToA();
                break;
            
            case 1:
            default:
                mixDToB();
                break;
                
            case 2:
                mixDToC();
                break;
                
            case 3:
                mixDToD();
                break;
                
            case 4:
                mixDToE();
                break;
                
            case 5:
                mixDToF();
                break;
                
            case 6:
                mixDToG();
                break;
                
            case 7:
                mixDToH();
                break;
        }
    }
    else if(m_noChannels==5)
    {
        switch(m_noOutChannels-1)
        {
            case 0:
                mixEToA();
                break;
            
            case 1:
            default:
                mixEToB();
                break;
                
            case 2:
                mixEToC();
                break;
                
            case 3:
                mixEToD();
                break;
                
            case 4:
                mixEToE();
                break;
                
            case 5:
                mixEToF();
                break;
                
            case 6:
                mixEToG();
                break;
                
            case 7:
                mixEToH();
                break;
        }
    }
    else if(m_noChannels==6)
    {
        switch(m_noOutChannels-1)
        {
            case 0:
                mixFToA();
                break;
            
            case 1:
            default:
                mixFToB();
                break;
                
            case 2:
                mixFToC();
                break;
                
            case 3:
                mixFToD();
                break;
                
            case 4:
                mixFToE();
                break;
                
            case 5:
                mixFToF();
                break;
                
            case 6:
                mixFToG();
                break;
                
            case 7:
                mixFToH();
                break;
        }
    }
    else if(m_noChannels==7)
    {
        switch(m_noOutChannels-1)
        {
            case 0:
                mixGToA();
                break;
            
            case 1:
            default:
                mixGToB();
                break;
                
            case 2:
                mixGToC();
                break;
                
            case 3:
                mixGToD();
                break;
                
            case 4:
                mixGToE();
                break;
                
            case 5:
                mixGToF();
                break;
                
            case 6:
                mixGToG();
                break;
                
            case 7:
                mixGToH();
                break;
        }
    }
    else if(m_noChannels==8)
    {
        switch(m_noOutChannels-1)
        {
            case 0:
                mixHToA();
                break;
            
            case 1:
            default:
                mixHToB();
                break;
                
            case 2:
                mixHToC();
                break;
                
            case 3:
                mixHToD();
                break;
                
            case 4:
                mixHToE();
                break;
                
            case 5:
                mixHToF();
                break;
                
            case 6:
                mixHToG();
                break;
                
            case 7:
                mixHToH();
                break;
        }
    }
}

//-------------------------------------------------------------------------------------------
// Input Mix - A
//-------------------------------------------------------------------------------------------
// Type A = Center
//-------------------------------------------------------------------------------------------

void AData::mixAToA()
{}

//-------------------------------------------------------------------------------------------
// Type A = Center
// Type B = F-Left, F-Right
//-------------------------------------------------------------------------------------------

void AData::mixAToB()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=1,out+=2)
    {
        out[0] = in[0];
        out[1] = in[0];
    }
}

//-------------------------------------------------------------------------------------------
// Type A = Center
// Type C = F-Left, F-Right, Center
//-------------------------------------------------------------------------------------------

void AData::mixAToC()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=1,out+=3)
    {
        out[0] = c_zeroSample;
        out[1] = c_zeroSample;
        out[2] = in[0];
    }
}

//-------------------------------------------------------------------------------------------
// Type A = Center
// Type D = F-Left, F-Right, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixAToD()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=1,out+=4)
    {
        out[0] = in[0];
        out[1] = in[0];
        out[2] = c_zeroSample;
        out[3] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type A = Center
// Type E = F-Left, F-Right, Center, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixAToE()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=1,out+=5)
    {
        out[0] = c_zeroSample;
        out[1] = c_zeroSample;
        out[2] = in[0];
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type A = Center
// Type F = F-Left, F-Right, Center, LFE, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixAToF()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=1,out+=6)
    {
        out[0] = c_zeroSample;
        out[1] = c_zeroSample;
        out[2] = in[0];
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
        out[5] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type A = Center
// Type G = F-Left, F-Right, Center, S-Left, S-Right, R-Left, R-Right
//-------------------------------------------------------------------------------------------

void AData::mixAToG()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=1,out+=7)
    {
        out[0] = c_zeroSample;
        out[1] = c_zeroSample;
        out[2] = in[0];
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
        out[5] = c_zeroSample;
        out[6] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type A = Center
// Type H = F-Left, F-Right, Center, LFE, S-Left, S-Right, R-Left, R-Right
//-------------------------------------------------------------------------------------------

void AData::mixAToH()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=1,out+=8)
    {
        out[0] = c_zeroSample;
        out[1] = c_zeroSample;
        out[2] = in[0];
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
        out[5] = c_zeroSample;
        out[6] = c_zeroSample;
        out[7] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Input Mix - B
//-------------------------------------------------------------------------------------------
// Type B = F-Left, F-Right
// Type A = Center
//-------------------------------------------------------------------------------------------

void AData::mixBToA()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=2,out+=1)
    {
        out[0] = (c_halfSample * in[0]) + (c_halfSample * in[1]);
    }
}

//-------------------------------------------------------------------------------------------
// Type B = F-Left, F-Right
// Type B = F-Left, F-Right
//-------------------------------------------------------------------------------------------

void AData::mixBToB()
{}

//-------------------------------------------------------------------------------------------
// Type B = F-Left, F-Right
// Type C = F-Left, F-Right, Center
//-------------------------------------------------------------------------------------------

void AData::mixBToC()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=2,out+=3)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type B = F-Left, F-Right
// Type D = F-Left, F-Right, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixBToD()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=2,out+=4)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = c_zeroSample;
        out[3] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type B = F-Left, F-Right
// Type E = F-Left, F-Right, Center, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixBToE()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=2,out+=5)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = c_zeroSample;
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type B = F-Left, F-Right
// Type F = F-Left, F-Right, Center, LFE, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixBToF()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=2,out+=6)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = c_zeroSample;
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
        out[5] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type B = F-Left, F-Right
// Type G = F-Left, F-Right, Center, S-Left, S-Right, R-Left, R-Right
//-------------------------------------------------------------------------------------------

void AData::mixBToG()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=2,out+=7)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = c_zeroSample;
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
        out[5] = c_zeroSample;
        out[6] = c_zeroSample;
        out[7] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type B = F-Left, F-Right
// Type H = F-Left, F-Right, Center, LFE, S-Left, S-Right, R-Left, R-Right
//-------------------------------------------------------------------------------------------

void AData::mixBToH()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=2,out+=8)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = c_zeroSample;
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
        out[5] = c_zeroSample;
        out[6] = c_zeroSample;
        out[7] = c_zeroSample;
        out[8] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Input Mix - C
//-------------------------------------------------------------------------------------------
// Type C = F-Left, F-Right, Center
// Type A = Center
//-------------------------------------------------------------------------------------------

void AData::mixCToA()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=3,out+=1)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = (0.333333333f * in[0]) + (0.333333333f * in[1]) + (0.333333333f * in[2]);
#else
        out[0] = (0.333333333 * in[0]) + (0.333333333 * in[1]) + (0.333333333 * in[2]);
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type C = F-Left, F-Right, Center
// Type B = F-Left, F-Right
//-------------------------------------------------------------------------------------------

void AData::mixCToB()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=3,out+=2)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.58823529411764705882352941176471f * (in[0] + (0.7f * in[2]));
        out[1] = 0.58823529411764705882352941176471f * (in[1] + (0.7f * in[2]));
#else
        out[0] = 0.58823529411764705882352941176471 * (in[0] + (0.7 * in[2]));
        out[1] = 0.58823529411764705882352941176471 * (in[1] + (0.7 * in[2]));
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type C = F-Left, F-Right, Center
// Type C = F-Left, F-Right, Center
//-------------------------------------------------------------------------------------------

void AData::mixCToC()
{}

//-------------------------------------------------------------------------------------------
// Type C = F-Left, F-Right, Center
// Type D = F-Left, F-Right, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixCToD()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=3,out+=4)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.58823529411764705882352941176471f * (in[0] + (0.7f * in[2]));
        out[1] = 0.58823529411764705882352941176471f * (in[1] + (0.7f * in[2]));
        out[2] = 0.41176470588235294117647058823529f * in[0];
        out[3] = 0.41176470588235294117647058823529f * in[1];
#else
        out[0] = 0.58823529411764705882352941176471 * (in[0] + (0.7 * in[2]));
        out[1] = 0.58823529411764705882352941176471 * (in[1] + (0.7 * in[2]));
        out[2] = 0.41176470588235294117647058823529 * in[0];
        out[3] = 0.41176470588235294117647058823529 * in[1];
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type C = F-Left, F-Right, Center
// Type E = F-Left, F-Right, Center, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixCToE()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=3,out+=5)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type C = F-Left, F-Right, Center
// Type F = F-Left, F-Right, Center, LFE, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixCToF()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=3,out+=6)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
        out[5] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type C = F-Left, F-Right, Center
// Type G = F-Left, F-Right, Center, S-Left, S-Right, R-Left, R-Right
//-------------------------------------------------------------------------------------------

void AData::mixCToG()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=3,out+=7)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
        out[5] = c_zeroSample;
        out[6] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type C = F-Left, F-Right, Center
// Type H = F-Left, F-Right, Center, LFE, S-Left, S-Right, R-Left, R-Right
//-------------------------------------------------------------------------------------------

void AData::mixCToH()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=3,out+=8)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = c_zeroSample;
        out[4] = c_zeroSample;
        out[5] = c_zeroSample;
        out[6] = c_zeroSample;
        out[7] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Input Mix - D
//-------------------------------------------------------------------------------------------
// Type D = F-Left, F-Right, S-Left, S-Right
// Type A = Center
//-------------------------------------------------------------------------------------------

void AData::mixDToA()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=4,out+=1)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.66666666666666666666666666666667f * ((0.5f * in[0]) + (0.5f * in[1]) + (0.25f * in[3]) + (0.25f * in[4]));
#else
        out[0] = 0.66666666666666666666666666666667 * ((0.5 * in[0]) + (0.5 * in[1]) + (0.25 * in[3]) + (0.25 * in[4]));
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type D = F-Left, F-Right, S-Left, S-Right
// Type B = F-Left, F-Right
//-------------------------------------------------------------------------------------------

void AData::mixDToB()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=4,out+=2)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.57142857142857142857142857142857f * (in[0] + (0.75f * in[2]));
        out[1] = 0.57142857142857142857142857142857f * (in[1] + (0.75f * in[3]));
#else
        out[0] = 0.57142857142857142857142857142857 * (in[0] + (0.75 * in[2]));
        out[1] = 0.57142857142857142857142857142857 * (in[1] + (0.75 * in[3]));
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type D = F-Left, F-Right, S-Left, S-Right
// Type C = F-Left, F-Right, Center
//-------------------------------------------------------------------------------------------

void AData::mixDToC()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=4,out+=3)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.57142857142857142857142857142857f * (in[0] + (0.75f * in[2]));
        out[1] = 0.57142857142857142857142857142857f * (in[1] + (0.75f * in[3]));
#else
        out[0] = 0.57142857142857142857142857142857 * (in[0] + (0.75 * in[2]));
        out[1] = 0.57142857142857142857142857142857 * (in[1] + (0.75 * in[3]));
#endif
        out[2] = c_zeroSample;
    }
}

//-------------------------------------------------------------------------------------------
// Type D = F-Left, F-Right, S-Left, S-Right
// Type D = F-Left, F-Right, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixDToD()
{}

//-------------------------------------------------------------------------------------------
// Type D = F-Left, F-Right, S-Left, S-Right
// Type E = F-Left, F-Right, Center, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixDToE()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=4,out+=5)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = c_zeroSample;
        out[3] = in[2];
        out[4] = in[3];
    }
}

//-------------------------------------------------------------------------------------------
// Type D = F-Left, F-Right, S-Left, S-Right
// Type F = F-Left, F-Right, Center, LFE, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixDToF()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;

    for(i=0;i<len;i++,in+=4,out+=6)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = c_zeroSample;
        out[3] = c_zeroSample;
        out[4] = in[2];
        out[5] = in[3];
    }
}

//-------------------------------------------------------------------------------------------
// Type D = F-Left, F-Right, S-Left, S-Right
// Type G = F-Left, F-Right, Center, S-Left, S-Right, R-Left, R-Right
//-------------------------------------------------------------------------------------------

void AData::mixDToG()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;

    for(i=0;i<len;i++,in+=4,out+=7)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = c_zeroSample;
        out[3] = c_halfSample * in[2];
        out[4] = c_halfSample * in[3];
        out[5] = c_halfSample * in[2];
        out[6] = c_halfSample * in[3];
    }
}

//-------------------------------------------------------------------------------------------
// Type D = F-Left, F-Right, S-Left, S-Right
// Type H = F-Left, F-Right, Center, LFE, S-Left, S-Right, R-Left, R-Right
//-------------------------------------------------------------------------------------------

void AData::mixDToH()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;

    for(i=0;i<len;i++,in+=4,out+=8)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = c_zeroSample;
        out[3] = c_zeroSample;
        out[4] = c_halfSample * in[2];
        out[5] = c_halfSample * in[3];
        out[6] = c_halfSample * in[2];
        out[7] = c_halfSample * in[3];
    }
}

//-------------------------------------------------------------------------------------------
// Input Mix - E
//-------------------------------------------------------------------------------------------
// Type E = F-Left, F-Right, Center, S-Left, S-Right
// Type A = Center
//-------------------------------------------------------------------------------------------

void AData::mixEToA()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=5,out+=1)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = (0.333333333f * in[0]) + (0.333333333f * in[1]) + (0.333333333f * in[2]);
#else
        out[0] = (0.333333333 * in[0]) + (0.333333333 * in[1]) + (0.333333333 * in[2]);
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
// Type B = 0.F-Left, 1.F-Right
//-------------------------------------------------------------------------------------------

void AData::mixEToB()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=5,out+=2)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.40816326530612244897959183673469f * (in[0] + (0.75f * in[3]) + (0.7f * in[2]));
        out[1] = 0.40816326530612244897959183673469f * (in[1] + (0.75f * in[4]) + (0.7f * in[2]));
#else
        out[0] = 0.40816326530612244897959183673469 * (in[0] + (0.75 * in[3]) + (0.7 * in[2]));
        out[1] = 0.40816326530612244897959183673469 * (in[1] + (0.75 * in[4]) + (0.7 * in[2]));
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
// Type C = 0.F-Left, 1.F-Right, 2.Center
//-------------------------------------------------------------------------------------------

void AData::mixEToC()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=5,out+=3)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.57142857142857142857142857142857f * (in[0] + (0.75f * in[3]));
        out[1] = 0.57142857142857142857142857142857f * (in[1] + (0.75f * in[4]));
#else
        out[0] = 0.57142857142857142857142857142857 * (in[0] + (0.75 * in[3]));
        out[1] = 0.57142857142857142857142857142857 * (in[1] + (0.75 * in[4]));
#endif
        out[2] = in[2];
    }
}

//-------------------------------------------------------------------------------------------
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixEToD()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=5,out+=4)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.58823529411764705882352941176471f * (in[0] + (0.7f * in[2]));
        out[1] = 0.58823529411764705882352941176471f * (in[1] + (0.7f * in[2]));
#else
        out[0] = 0.58823529411764705882352941176471 * (in[0] + (0.7 * in[2]));
        out[1] = 0.58823529411764705882352941176471 * (in[1] + (0.7 * in[2]));
#endif
        out[2] = in[3];
        out[3] = in[4];
    }
}

//-------------------------------------------------------------------------------------------
// Type E = F-Left, F-Right, Center, S-Left, S-Right
// Type E = F-Left, F-Right, Center, S-Left, S-Right
//-------------------------------------------------------------------------------------------

void AData::mixEToE()
{}

//-------------------------------------------------------------------------------------------
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixEToF()
{
    int i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=5,out+=6)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = c_zeroSample;
        out[4] = in[3];
        out[5] = in[4];
    }
}

//-------------------------------------------------------------------------------------------
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
//-------------------------------------------------------------------------------------------

void AData::mixEToG()
{
    int i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=5,out+=7)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = c_halfSample * in[3];
        out[4] = c_halfSample * in[4];
        out[5] = c_halfSample * in[3];
        out[6] = c_halfSample * in[4];
    }
}

//-------------------------------------------------------------------------------------------
// Type E = F-Left, F-Right, Center, S-Left, S-Right
// Type H = F-Left, F-Right, Center, LFE, S-Left, S-Right, R-Left, R-Right
//-------------------------------------------------------------------------------------------

void AData::mixEToH()
{
    int i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=5,out+=7)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = c_zeroSample;
        out[4] = c_halfSample * in[3];
        out[5] = c_halfSample * in[4];
        out[6] = c_halfSample * in[3];
        out[7] = c_halfSample * in[4];
    }
}

//-------------------------------------------------------------------------------------------
// Input Mix - F
//-------------------------------------------------------------------------------------------
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type A = 0.Center
//-------------------------------------------------------------------------------------------

void AData::mixFToA()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=6,out+=1)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = (0.333333333f * in[0]) + (0.333333333f * in[1]) + (0.333333333f * in[2]);
#else
        out[0] = (0.333333333 * in[0]) + (0.333333333 * in[1]) + (0.333333333 * in[2]);
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type B = 0.F-Left, 1.F-Right
//-------------------------------------------------------------------------------------------

void AData::mixFToB()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=6,out+=2)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.40816326530612244897959183673469f * (in[0] + (0.75f * in[4]) + (0.7f * in[2]));
        out[1] = 0.40816326530612244897959183673469f * (in[1] + (0.75f * in[5]) + (0.7f * in[2]));
#else
        out[0] = 0.40816326530612244897959183673469 * (in[0] + (0.75 * in[4]) + (0.7 * in[2]));
        out[1] = 0.40816326530612244897959183673469 * (in[1] + (0.75 * in[5]) + (0.7 * in[2]));
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type C = 0.F-Left, 1.F-Right, 2.Center
//-------------------------------------------------------------------------------------------

void AData::mixFToC()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=6,out+=3)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.57142857142857142857142857142857f * (in[0] + (0.75f * in[4]));
        out[1] = 0.57142857142857142857142857142857f * (in[1] + (0.75f * in[5]));
#else
        out[0] = 0.57142857142857142857142857142857 * (in[0] + (0.75 * in[4]));
        out[1] = 0.57142857142857142857142857142857 * (in[1] + (0.75 * in[5]));
#endif
        out[2] = in[2];
    }
}

//-------------------------------------------------------------------------------------------
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixFToD()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=6,out+=4)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.58823529411764705882352941176471f * (in[0] + (0.7f * in[2]));
        out[1] = 0.58823529411764705882352941176471f * (in[1] + (0.7f * in[2]));
#else
        out[0] = 0.58823529411764705882352941176471 * (in[0] + (0.7 * in[2]));
        out[1] = 0.58823529411764705882352941176471 * (in[1] + (0.7 * in[2]));
#endif
        out[2] = in[4];
        out[3] = in[5];
    }
}

//-------------------------------------------------------------------------------------------
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixFToE()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=6,out+=5)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = in[4];
        out[4] = in[5];
    }
}

//-------------------------------------------------------------------------------------------
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixFToF()
{}

//-------------------------------------------------------------------------------------------
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
//-------------------------------------------------------------------------------------------

void AData::mixFToG()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=6,out+=7)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = c_halfSample * in[4];
        out[4] = c_halfSample * in[5];
        out[5] = c_halfSample * in[4];
        out[6] = c_halfSample * in[5];
    }
}

//-------------------------------------------------------------------------------------------
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type H = 0.F-Left, F-Right, Center, LFE, S-Left, S-Right, R-Left, R-Right
//-------------------------------------------------------------------------------------------

void AData::mixFToH()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=6,out+=8)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = in[3];
        out[4] = c_halfSample * in[4];
        out[5] = c_halfSample * in[5];
        out[6] = c_halfSample * in[4];
        out[7] = c_halfSample * in[5];
    }
}

//-------------------------------------------------------------------------------------------
// Input Mix - G
//-------------------------------------------------------------------------------------------
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type A = 0.Center
//-------------------------------------------------------------------------------------------

void AData::mixGToA()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=7,out+=1)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = (0.333333333 * in[0]) + (0.333333333f * in[1]) + (0.333333333f * in[2]);
#else
        out[0] = (0.333333333 * in[0]) + (0.333333333 * in[1]) + (0.333333333 * in[2]);
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type B = 0.F-Left, 1.F-Right
//-------------------------------------------------------------------------------------------

void AData::mixGToB()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=7,out+=2)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.40816326530612244897959183673469f * (in[0] + (0.75f * ((0.5f * in[3]) + (0.5f * in[5]))) + (0.7f * in[2]));
        out[1] = 0.40816326530612244897959183673469f * (in[1] + (0.75f * ((0.5f * in[4]) + (0.5f * in[6]))) + (0.7f * in[2]));
#else
        out[0] = 0.40816326530612244897959183673469 * (in[0] + (0.75 * ((0.5 * in[3]) + (0.5 * in[5]))) + (0.7 * in[2]));
        out[1] = 0.40816326530612244897959183673469 * (in[1] + (0.75 * ((0.5 * in[4]) + (0.5 * in[6]))) + (0.7 * in[2]));
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type C = 0.F-Left, 1.F-Right, 2.Center
//-------------------------------------------------------------------------------------------

void AData::mixGToC()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=7,out+=3)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.57142857142857142857142857142857f * (in[0] + (0.75f * ((0.5f * in[3]) + (0.5f * in[5]))));
        out[1] = 0.57142857142857142857142857142857f * (in[1] + (0.75f * ((0.5f * in[4]) + (0.5f * in[6]))));
#else
        out[0] = 0.57142857142857142857142857142857 * (in[0] + (0.75 * ((0.5 * in[3]) + (0.5 * in[5]))));
        out[1] = 0.57142857142857142857142857142857 * (in[1] + (0.75 * ((0.5 * in[4]) + (0.5 * in[6]))));
#endif
        out[2] = in[2];
    }
}

//-------------------------------------------------------------------------------------------
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixGToD()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=7,out+=4)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.58823529411764705882352941176471f * (in[0] + (0.7f * in[2]));
        out[1] = 0.58823529411764705882352941176471f * (in[1] + (0.7f * in[2]));
#else
        out[0] = 0.58823529411764705882352941176471 * (in[0] + (0.7 * in[2]));
        out[1] = 0.58823529411764705882352941176471 * (in[1] + (0.7 * in[2]));
#endif
        out[2] = (c_halfSample * in[3]) + (c_halfSample * in[5]);
        out[3] = (c_halfSample * in[4]) + (c_halfSample * in[6]);
    }
}

//-------------------------------------------------------------------------------------------
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixGToE()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=7,out+=5)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = (c_halfSample * in[3]) + (c_halfSample * in[5]);
        out[4] = (c_halfSample * in[4]) + (c_halfSample * in[6]);
    }
}

//-------------------------------------------------------------------------------------------
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixGToF()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=7,out+=6)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = c_zeroSample;
        out[4] = (c_halfSample * in[3]) + (c_halfSample * in[5]);
        out[5] = (c_halfSample * in[4]) + (c_halfSample * in[6]);
    }
}

//-------------------------------------------------------------------------------------------
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
//-------------------------------------------------------------------------------------------

void AData::mixGToG()
{}

//-------------------------------------------------------------------------------------------
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
//-------------------------------------------------------------------------------------------

void AData::mixGToH()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=7,out+=8)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = c_zeroSample;
        out[4] = in[3];
        out[5] = in[4];
        out[6] = in[5];
        out[7] = in[6];
    }
}

//-------------------------------------------------------------------------------------------
// Input Mix - H
//-------------------------------------------------------------------------------------------
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
// Type A = 0.Center
//-------------------------------------------------------------------------------------------

void AData::mixHToA()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=8,out+=1)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = (0.333333333f * in[0]) + (0.333333333f * in[1]) + (0.333333333f * in[2]);
#else
        out[0] = (0.333333333 * in[0]) + (0.333333333 * in[1]) + (0.333333333 * in[2]);
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
// Type B = 0.F-Left, 1.F-Right
//-------------------------------------------------------------------------------------------

void AData::mixHToB()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=8,out+=2)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.40816326530612244897959183673469f * (in[0] + (0.75f * ((0.5f * in[4]) + (0.5f * in[6]))) + (0.7f * in[2]));
        out[1] = 0.40816326530612244897959183673469f * (in[1] + (0.75f * ((0.5f * in[5]) + (0.5f * in[7]))) + (0.7f * in[2]));
#else
        out[0] = 0.40816326530612244897959183673469 * (in[0] + (0.75 * ((0.5 * in[4]) + (0.5 * in[6]))) + (0.7 * in[2]));
        out[1] = 0.40816326530612244897959183673469 * (in[1] + (0.75 * ((0.5 * in[5]) + (0.5 * in[7]))) + (0.7 * in[2]));
#endif
    }
}

//-------------------------------------------------------------------------------------------
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
// Type C = 0.F-Left, 1.F-Right, 2.Center
//-------------------------------------------------------------------------------------------

void AData::mixHToC()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=8,out+=3)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.57142857142857142857142857142857f * (in[0] + (0.75f * ((0.5f * in[4]) + (0.5f * in[6]))));
        out[1] = 0.57142857142857142857142857142857f * (in[1] + (0.75f * ((0.5f * in[5]) + (0.5f * in[7]))));
#else
        out[0] = 0.57142857142857142857142857142857 * (in[0] + (0.75 * ((0.5 * in[4]) + (0.5 * in[6]))));
        out[1] = 0.57142857142857142857142857142857 * (in[1] + (0.75 * ((0.5 * in[5]) + (0.5 * in[7]))));
#endif
        out[2] = in[2];
    }
}

//-------------------------------------------------------------------------------------------
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixHToD()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=8,out+=4)
    {
#if defined(SINGLE_FLOAT_SAMPLE)
        out[0] = 0.58823529411764705882352941176471f * (in[0] + (0.7f * in[2]));
        out[1] = 0.58823529411764705882352941176471f * (in[1] + (0.7f * in[2]));
#else
        out[0] = 0.58823529411764705882352941176471 * (in[0] + (0.7 * in[2]));
        out[1] = 0.58823529411764705882352941176471 * (in[1] + (0.7 * in[2]));
#endif
        out[2] = (c_halfSample * in[4]) + (c_halfSample * in[6]);
        out[3] = (c_halfSample * in[5]) + (c_halfSample * in[7]);
    }
}

//-------------------------------------------------------------------------------------------
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixHToE()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=8,out+=5)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = (c_halfSample * in[4]) + (c_halfSample * in[6]);
        out[4] = (c_halfSample * in[5]) + (c_halfSample * in[7]);
    }
}

//-------------------------------------------------------------------------------------------
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
//-------------------------------------------------------------------------------------------

void AData::mixHToF()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=8,out+=6)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = in[3];
        out[4] = (c_halfSample * in[4]) + (c_halfSample * in[6]);
        out[5] = (c_halfSample * in[5]) + (c_halfSample * in[7]);
    }
}

//-------------------------------------------------------------------------------------------
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
//-------------------------------------------------------------------------------------------

void AData::mixHToG()
{
    tint i,len = length();
    sample_t *in = m_data;
    sample_t *out = m_outData;
    
    for(i=0;i<len;i++,in+=8,out+=7)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2];
        out[3] = in[4];
        out[4] = in[5];
        out[5] = in[6];
        out[6] = in[7];
    }
}

//-------------------------------------------------------------------------------------------
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
//-------------------------------------------------------------------------------------------

void AData::mixHToH()
{}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
