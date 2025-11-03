#include "engine/greenomega/inc/FLACMetaSeekTable.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------
// FLACMetaSeekTable::SeekPoint
//-------------------------------------------------------------------------------------------

FLACMetaSeekTable::SeekPoint::SeekPoint() : m_sampleNumber(0),
    m_offset(0),
    m_samplesInFrame(0)
{}

//-------------------------------------------------------------------------------------------

FLACMetaSeekTable::SeekPoint::SeekPoint(const SeekPoint& rhs) : m_sampleNumber(rhs.m_sampleNumber),
    m_offset(rhs.m_offset),
    m_samplesInFrame(rhs.m_samplesInFrame)
{}

//-------------------------------------------------------------------------------------------

const FLACMetaSeekTable::SeekPoint& FLACMetaSeekTable::SeekPoint::operator = (const SeekPoint& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void FLACMetaSeekTable::SeekPoint::copy(const SeekPoint& rhs)
{
    m_sampleNumber = rhs.m_sampleNumber;
    m_offset = rhs.m_offset;
    m_samplesInFrame = rhs.m_samplesInFrame;
}

//-------------------------------------------------------------------------------------------

void FLACMetaSeekTable::SeekPoint::read(engine::Sequence *seq)
{
    m_sampleNumber = seq->read64BitsI();
    m_offset = seq->read64BitsI();
    m_samplesInFrame = seq->readBitsI(16);
}

//-------------------------------------------------------------------------------------------
// FLACMetaSeekTable
//-------------------------------------------------------------------------------------------

FLACMetaSeekTable::FLACMetaSeekTable() : FLACMetaBase(e_SeekTable),
    m_frequency(44100),
    m_seekTable()
{}

//-------------------------------------------------------------------------------------------

FLACMetaSeekTable::~FLACMetaSeekTable()
{}

//-------------------------------------------------------------------------------------------

static bool SeekAscendingSort(const FLACMetaSeekTable::SeekPoint& a,const FLACMetaSeekTable::SeekPoint& b)
{
    return a.sampleNumber() < b.sampleNumber();
}

//-------------------------------------------------------------------------------------------

bool FLACMetaSeekTable::read(engine::Sequence *seq)
{
    tint i,count,len = seq->readBitsI(24);

    count = len / 18;
    for(i=0;i<count;i++)
    {
        SeekPoint p;
        p.read(seq);
        if(!p.isPlaceholder())
        {
            m_seekTable.push_back(p);
        }
    }
    std::sort(m_seekTable.begin(),m_seekTable.end(),SeekAscendingSort);
    return true;
}

//-------------------------------------------------------------------------------------------

void FLACMetaSeekTable::setFrequency(int f)
{
    m_frequency = f;
}

//-------------------------------------------------------------------------------------------

FLACMetaSeekTable::SeekPoint FLACMetaSeekTable::find(common::TimeStamp& t)
{
    tint low,high,mid;
    tuint64 sampleNo = static_cast<tuint64>(static_cast<double>(t) * static_cast<double>(m_frequency));
    SeekPoint p;

    low = 0;
    high = m_seekTable.size();
    while(low < high)
    {
        mid = low + ((high - low) / 2);
        if(m_seekTable.at(mid).sampleNumber() < sampleNo)
        {
            low = mid + 1;
        }
        else
        {
            high = mid;
        }
    }
    if(m_seekTable.at(low).sampleNumber() > sampleNo)
    {
        low--;
    }
    if(low>=0 && low<static_cast<tint>(m_seekTable.size()))
    {
        p = m_seekTable.at(low);
    }
    return p;
}

//-------------------------------------------------------------------------------------------

tint FLACMetaSeekTable::findPoints(tuint64 sampleNo,SeekPoint& lPoint,SeekPoint& hPoint)
{
    tint low,high,mid;

    low = 0;
    high = m_seekTable.size();
    while(low < high)
    {
        mid = low + ((high - low) / 2);
        if(m_seekTable.at(mid).sampleNumber() < sampleNo)
        {
            low = mid + 1;
        }
        else
        {
            high = mid;
        }
    }
    if(low>=static_cast<tint>(m_seekTable.size()))
    {
        low--;
    }
    else if(m_seekTable.at(low).sampleNumber() > sampleNo)
    {
        low--;
    }
    if(low>=0 && low<static_cast<tint>(m_seekTable.size()))
    {
        lPoint = m_seekTable.at(low);
        if(sampleNo>=lPoint.sampleNumber())
        {
            low++;
            if(low<static_cast<tint>(m_seekTable.size()))
            {
                hPoint = m_seekTable.at(low);
                if(sampleNo<hPoint.sampleNumber())
                {
                    return 2;
                }
                else
                {
                    return 1;
                }
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
