#include "engine/inc/RData.h"
#include "engine/inc/FormatTypeFromFloat.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------
// RData::Part
//-------------------------------------------------------------------------------------------

RData::Part::Part() : m_offset(0),
    m_length(0),
    m_start(),
    m_end(),
    m_done(false),
    m_nextFlag(false),
    m_startNext(),
    m_endNext(),
    m_refStartTime(),
    m_dataType(e_SampleFloat)
{}

//-------------------------------------------------------------------------------------------

RData::Part::Part(const Part& rhs) : m_offset(0),
    m_length(0),
    m_start(),
    m_end(),
    m_done(false),
    m_nextFlag(false),
    m_startNext(),
    m_endNext(),
    m_refStartTime(),
    m_dataType(e_SampleFloat)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

RData::Part::~Part()
{}

//-------------------------------------------------------------------------------------------

void RData::Part::copy(const Part& rhs)
{
    m_offset = rhs.m_offset;
    m_length = rhs.m_length;
    m_start = rhs.m_start;
    m_end = rhs.m_end;
    m_done = rhs.m_done;
    m_nextFlag = rhs.m_nextFlag;
    m_startNext = rhs.m_startNext;
    m_endNext = rhs.m_endNext;
    m_refStartTime = rhs.m_refStartTime;
    m_dataType = rhs.m_dataType;
}

//-------------------------------------------------------------------------------------------

const RData::Part& RData::Part::operator = (const Part& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

tint& RData::Part::offset()
{
    return m_offset;
}

//-------------------------------------------------------------------------------------------

const tint& RData::Part::offsetConst() const
{
    return m_offset;
}

//-------------------------------------------------------------------------------------------

tint& RData::Part::length()
{
    return m_length;
}

//-------------------------------------------------------------------------------------------

const tint& RData::Part::lengthConst() const
{
    return m_length;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& RData::Part::start()
{
    return m_start;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& RData::Part::startConst() const
{
    return m_start;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& RData::Part::end()
{
    return m_end;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& RData::Part::endConst() const
{
    return m_end;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& RData::Part::startNext()
{
    return m_startNext;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& RData::Part::startNextConst() const
{
    return m_startNext;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& RData::Part::endNext()
{
    return m_endNext;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& RData::Part::endNextConst() const
{
    return m_endNext;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp& RData::Part::refStartTime()
{
    return m_refStartTime;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& RData::Part::refStartTimeConst() const
{
    return m_refStartTime;
}

//-------------------------------------------------------------------------------------------

bool& RData::Part::done()
{
    return m_done;
}

//-------------------------------------------------------------------------------------------

const bool& RData::Part::doneConst() const
{
    return m_done;
}

//-------------------------------------------------------------------------------------------

bool RData::Part::isNext() const
{
    return m_nextFlag;
}

//-------------------------------------------------------------------------------------------

void RData::Part::setNext(bool flag)
{
    m_nextFlag = flag;
}

//-------------------------------------------------------------------------------------------

bool operator == (const RData::Part& a,const RData::Part& b)
{
    return a.isEqual(b);
}

//-------------------------------------------------------------------------------------------

bool operator != (const RData::Part& a,const RData::Part& b)
{
    return !(a.isEqual(b));
}

//-------------------------------------------------------------------------------------------

bool RData::Part::isEqual(const Part& rhs) const
{
    bool res = false;

    if(m_offset==rhs.m_offset && m_length==rhs.m_length && m_start==rhs.m_start && m_end==rhs.m_end && m_done==rhs.m_done)
    {
        if(m_nextFlag==rhs.m_nextFlag && m_startNext==rhs.m_startNext && m_endNext==rhs.m_endNext && m_refStartTime==rhs.m_refStartTime)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

CodecDataType RData::Part::getDataType() const
{
    return m_dataType;
}

//-------------------------------------------------------------------------------------------

void RData::Part::setDataType(CodecDataType dType)
{
    m_dataType = dType;
}


//-------------------------------------------------------------------------------------------
// RData
//-------------------------------------------------------------------------------------------

tint RData::noParts() const
{
    return m_parts.size();
}

//-------------------------------------------------------------------------------------------

RData::Part& RData::part(tint i)
{
    if(i==m_parts.size())
    {
        return nextPart();
    }
    else
    {
        return m_parts[i];
    }
}

//-------------------------------------------------------------------------------------------

const RData::Part& RData::partConst(tint i) const
{
    return m_parts[i];
}

//-------------------------------------------------------------------------------------------
// RData
//-------------------------------------------------------------------------------------------

RData::RData() : AData(),
    m_parts()
{}

//-------------------------------------------------------------------------------------------

RData::RData(tint len,tint inChannel,tint outChannel) : AData(len,inChannel,outChannel),
    m_parts()
{}

//-------------------------------------------------------------------------------------------

RData::RData(const AData& rhs) : AData(),
    m_parts()
{
    RData::copy(rhs);
}

//-------------------------------------------------------------------------------------------

RData::~RData()
{}

//-------------------------------------------------------------------------------------------

void RData::copy(const AData& rhs)
{
    AData::copy(rhs);
    try
    {
        const RData& r = dynamic_cast<const RData&>(rhs);
        m_parts = r.m_parts;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void RData::reset()
{
    m_parts.clear();
    AData::reset();
}

//-------------------------------------------------------------------------------------------

RData::Part& RData::nextPart()
{
    Part p;
    
    if(m_parts.size() > 0)
    {
        const Part& lP = m_parts.last();
        p.offset() = lP.offsetConst() + lP.lengthConst();
        p.length() = length() - p.offsetConst();
        p.start() = lP.endConst();
        p.end() = lP.endConst();
    }
    else
    {
        p.offset() = 0;
        p.length() = rLength();
    }
    m_parts.append(p);
    return m_parts.last();
}

//-------------------------------------------------------------------------------------------

tint RData::rOffset() const
{
    tint offset;
    
    if(m_parts.size() > 0)
    {
        const Part& lP = m_parts.last();

        if(lP.doneConst())
        {
            offset = lP.offsetConst() + lP.lengthConst();
        }
        else
        {
            offset = lP.offsetConst();
        }
    }
    else
    {
        offset = 0;
    }
    return offset;
}

//-------------------------------------------------------------------------------------------

tint RData::rLength() const
{
    return length() - rOffset();
}

//-------------------------------------------------------------------------------------------

CodecDataType RData::partGetDataType(tint i) const
{
    return m_parts.at(i).getDataType();
}

//-------------------------------------------------------------------------------------------

sample_t *RData::partData(tint i)
{
    return &m_data[ m_parts.at(i).offsetConst() * m_noChannels ];
}

//-------------------------------------------------------------------------------------------

const sample_t *RData::partDataConst(tint i) const
{
    return &m_data[ m_parts.at(i).offsetConst() * m_noChannels ];
}

//-------------------------------------------------------------------------------------------

sample_t *RData::partDataCenter(tint i)
{
    sample_t *c = center();
    return &c[m_parts.at(i).offsetConst()];
}

//-------------------------------------------------------------------------------------------

const sample_t *RData::partDataCenterConst(tint i) const
{
    if(m_centreData != 0)
    {
        return &m_centreData[m_parts.at(i).offsetConst()];
    }
    else
    {
        return NULL;
    }
}

//-------------------------------------------------------------------------------------------

sample_t *RData::partDataOut(tint i)
{
    if(m_outData!=0)
    {
        return &m_outData[ m_parts.at(i).offsetConst() * m_noOutChannels ];
    }
    else
    {
        return &m_data[ m_parts.at(i).offsetConst() * m_noChannels ];
    }
}

//-------------------------------------------------------------------------------------------

const sample_t *RData::partDataOutConst(tint i) const
{
    if(m_outData!=0)
    {
        return &m_outData[ m_parts.at(i).offsetConst() * m_noOutChannels ];
    }
    else
    {
        return &m_data[ m_parts.at(i).offsetConst() * m_noOutChannels ];
    }    
}

//-------------------------------------------------------------------------------------------

sample_t *RData::partFilterData(tint i, tint filterIndex)
{
    sample_t *pFilterData = filterData(filterIndex);
    tint noChs = (filterIndex >= 0) ? m_noChannels : 1;
    return &pFilterData[ m_parts.at(i).offsetConst() * noChs ];
}

//-------------------------------------------------------------------------------------------

const sample_t *RData::partFilterDataConst(tint i, tint filterIndex) const
{
    const sample_t *pFilterData = filterDataConst(filterIndex);
    tint noChs = (filterIndex >= 0) ? m_noChannels : 1;
    return &pFilterData[ m_parts.at(i).offsetConst() * noChs ];
}

//-------------------------------------------------------------------------------------------

sample_t *RData::currentPartData()
{
    return &m_data[ m_parts.last().offsetConst() * m_noChannels ];
}

//-------------------------------------------------------------------------------------------

const sample_t *RData::currentPartDataConst() const
{
    return &m_data[ m_parts.last().offsetConst() * m_noChannels ];
}

//-------------------------------------------------------------------------------------------

void RData::clipToTime(const common::TimeStamp& clipT)
{
    tint i;
    bool clip = false;
    QList<tint> pCStateList; // 0 - keep, 1 - remove, 2 - clip
    
    for(i=0;i<m_parts.size();i++)
    {
        tint state;
        Part& p = m_parts[i];
        
        if(clipT > p.start())
        {
            if(p.end() <= clipT)
            {
                state = 0;
            }
            else
            {
                state = 2;
                clip = true;
            }
        }
        else
        {
            state = 1;
            clip = true;
        }
        pCStateList.append(state);
    }
    
    if(clip)
    {
        int cOffset = 0;
        int len = length() * m_noChannels;
        sample_t *cData = new sample_t [len];
        QVector<Part> cParts;
        
        for(i=0;i<m_parts.size();i++)
        {
            int state = pCStateList.at(i);
            Part& p = m_parts[i];
            
            if(state==0 || state==2)
            {
                Part nPart(p);
                nPart.offset() = cOffset;
                
                if(state==2)
                {
                    tfloat64 timePerSample = static_cast<tfloat64>(nPart.end() - nPart.start()) / static_cast<tfloat64>(nPart.length());
                    int nLength = static_cast<int>(ceil(static_cast<tfloat64>(clipT - nPart.start()) / timePerSample));
                    nPart.length() = nLength;
                    nPart.end() = nPart.start() + (static_cast<tfloat64>(nLength) * timePerSample);
                }
                memcpy(&cData[cOffset * m_noChannels],partData(i),nPart.length() * m_noChannels * sizeof(sample_t));
                cOffset += nPart.length();
                cParts.append(nPart);
            }
        }
        memcpy(m_data,cData,cOffset * m_noChannels * sizeof(sample_t));
        
        if(!cParts.isEmpty())
        {
            m_parts = cParts;
            start() = m_parts.at(0).startConst();
            end() = m_parts.at(m_parts.size() - 1).endConst();
        }
        else
        {
            end() = start();
            m_parts.clear();
        }
        
        delete [] cData;
    }
}

//-------------------------------------------------------------------------------------------

sample_t *RData::center()
{
    if(m_centreData == 0)
    {
        m_centreData = new sample_t [m_length];
    }
    
    if(!m_isCenterValid)
    {
        for(tint partIdx = 0; partIdx < noParts(); partIdx++)
        {
            int idx;
            const sample_t *d = partDataConst(partIdx);
            Part& p = part(partIdx);
            sample_t* o = &m_centreData[p.offsetConst()];
                
            if(p.getDataType() == e_SampleInt16)
            {
                const tint16 *in = reinterpret_cast<const tint16 *>(d);
                
                for(idx = 0; idx < p.length(); idx++)
                {
                    sample_t x = 0.0f;
                    
                    for(tint ch = 0; ch < m_noChannels; ch++)
                    {
                        x += sample64From16Bit(*in++);
                    }
                    o[idx] = x / static_cast<tfloat64>(m_noChannels);
                }
            }
            else if(p.getDataType() == e_SampleInt24)
            {
                const tint32 *in = reinterpret_cast<const tint32 *>(d);
                
                for(idx = 0; idx < p.length(); idx++)
                {
                    sample_t x = 0.0f;
                    
                    for(tint ch = 0; ch < m_noChannels; ch++)
                    {
                        x += sample64From24Bit(*in++);
                    }
                    o[idx] = x / static_cast<tfloat64>(m_noChannels);
                }
            }
            else if(p.getDataType() == e_SampleInt32)
            {
                const tint32 *in = reinterpret_cast<const tint32 *>(d);
                
                for(idx = 0; idx < p.length(); idx++)
                {
                    sample_t x = 0.0f;
                    
                    for(tint ch = 0; ch < m_noChannels; ch++)
                    {
                        x += sample64From32Bit(*in++);
                    }
                    o[idx] = x / static_cast<tfloat64>(m_noChannels);
                }
            }
            else
            {
                for(idx = 0; idx < p.length(); idx++)
                {
                    sample_t x = 0.0f;
                    
                    for(tint ch = 0; ch < m_noChannels; ch++)
                    {
                        x += *d++;
                    }
                    o[idx] = x / static_cast<tfloat64>(m_noChannels);
                }
            }
        }
        m_isCenterValid = true;
    }
    return m_centreData;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
