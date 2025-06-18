#include "audioio/inc/IOTimeStamp.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

IOTimeStamp::IOTimeStamp() : m_valid(false),
    m_time()
{}

//-------------------------------------------------------------------------------------------

IOTimeStamp::IOTimeStamp(bool bValid,const common::TimeStamp& t) : m_valid(bValid),
    m_time(t)
{}

//-------------------------------------------------------------------------------------------

IOTimeStamp::IOTimeStamp(const IOTimeStamp& rhs) : m_valid(false),
    m_time()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const IOTimeStamp& IOTimeStamp::operator = (const IOTimeStamp& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void IOTimeStamp::copy(const IOTimeStamp& rhs)
{
    m_valid = rhs.m_valid;
    m_time = rhs.m_time;
}

//-------------------------------------------------------------------------------------------

bool IOTimeStamp::isValid() const
{
    return m_valid;
}

//-------------------------------------------------------------------------------------------

const common::TimeStamp& IOTimeStamp::time() const
{
    return m_time;
}

//-------------------------------------------------------------------------------------------

bool operator == (const IOTimeStamp& a,const IOTimeStamp& b)
{
    return a.isEqual(b);
}

//-------------------------------------------------------------------------------------------

bool operator != (const IOTimeStamp& a,const IOTimeStamp& b)
{
    return !(a.isEqual(b));
}

//-------------------------------------------------------------------------------------------

bool IOTimeStamp::isEqual(const IOTimeStamp& rhs) const
{
    bool res;
    
    if(!isValid() && !rhs.isValid())
    {
        res = true;
    }
    else if(isValid() && rhs.isValid())
    {
        res = (time() == rhs.time());
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------

