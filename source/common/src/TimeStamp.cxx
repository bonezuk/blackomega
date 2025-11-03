#include "common/inc/TimeStamp.h"
#if defined(OMEGA_WIN32)
#include <mmsystem.h>
#elif defined(OMEGA_MACOSX)
#include <CoreServices/CoreServices.h>
#include <mach/mach_time.h>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

TimeStamp::TimeStamp() : m_time(0)
{}

//-------------------------------------------------------------------------------------------

TimeStamp::TimeStamp(tint32 x) : m_time(0)
{
    decode(x);
}

//-------------------------------------------------------------------------------------------

TimeStamp::TimeStamp(tuint32 x) : m_time(0)
{
    decode(x);
}

//-------------------------------------------------------------------------------------------

TimeStamp::TimeStamp(tint64 x) : m_time(0)
{
    decode(x);
}

//-------------------------------------------------------------------------------------------

TimeStamp::TimeStamp(tuint64 x) : m_time(0)
{
    decode(x);
}

//-------------------------------------------------------------------------------------------

TimeStamp::TimeStamp(tfloat32 x) : m_time(0)
{
    decode(x);
}

//-------------------------------------------------------------------------------------------

TimeStamp::TimeStamp(tfloat64 x) : m_time(0)
{
    decode(x);
}

//-------------------------------------------------------------------------------------------

TimeStamp::TimeStamp(const TimeStamp& x) : m_time(x.m_time)
{}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator = (tint32 x)
{
    decode(x);
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator = (tuint32 x)
{
    decode(x);
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator = (tint64 x)
{
    decode(x);
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator = (tuint64 x)
{
    decode(x);
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator = (tfloat32 x)
{
    decode(x);
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator = (tfloat64 x)
{
    decode(x);
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator = (const TimeStamp& x)
{
    if(this!=&x)
    {
        m_time = x.m_time;
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::set(tint32 x) volatile
{
    set(static_cast<tuint32>(x));
}

//-------------------------------------------------------------------------------------------

void TimeStamp::set(tuint32 x) volatile
{
    tuint64 t;
    t = static_cast<tuint64>(x);
    t <<= 16;
    t &= 0x0000ffffffff0000ULL;
    m_time = t;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::set(tint64 x) volatile
{
    set(static_cast<tuint64>(x));
}

//-------------------------------------------------------------------------------------------

void TimeStamp::set(tuint64 x) volatile
{
    m_time = x;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::set(tfloat32 x) volatile
{
    set(static_cast<tfloat64>(x));
}

//-------------------------------------------------------------------------------------------

void TimeStamp::set(tfloat64 x) volatile
{
    tuint64 x1,x2;
    tfloat64 y1,y2;
    tuint32 z1,z2;
    
    y1 = ::fabs(x);
    x *= 4294967296.0;
    x = ::fmod(x,4294967296.0);
    y2 = ::fabs(x);
    z1 = static_cast<tuint32>(y1);
    z2 = static_cast<tuint32>(y2);

    x1 = static_cast<tuint64>(z1);
    x2 = static_cast<tuint64>(z2);
    x1 <<= 32;
    x1 &= 0xffffffff00000000ULL;
    x2 &= 0x00000000ffffffffULL;
    m_time = x1 | x2;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::set(const TimeStamp& x) volatile
{
    m_time = x.m_time;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::decode(tint32 x)
{
    decode(static_cast<tuint32>(x));
}

//-------------------------------------------------------------------------------------------

void TimeStamp::decode(tuint32 x)
{
    m_time = static_cast<tuint64>(x);
    m_time <<= 16;
    m_time &= 0x0000ffffffff0000ULL;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::decode(tint64 x)
{
    decode(static_cast<tuint64>(x));
}

//-------------------------------------------------------------------------------------------

void TimeStamp::decode(tuint64 x)
{
    m_time = x;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::decode(tfloat32 x)
{
    decode(static_cast<tfloat64>(x));
}

//-------------------------------------------------------------------------------------------

void TimeStamp::decode(tfloat64 x)
{
    tuint64 x1,x2;
    tfloat64 y1,y2;
    tuint32 z1,z2;
    
    y1 = ::fabs(x);
    x *= 4294967296.0;
    x = ::fmod(x,4294967296.0);
    y2 = ::fabs(x);
    z1 = static_cast<tuint32>(y1);
    z2 = static_cast<tuint32>(y2);

    x1 = static_cast<tuint64>(z1);
    x2 = static_cast<tuint64>(z2);
    x1 <<= 32;
    x1 &= 0xffffffff00000000ULL;
    x2 &= 0x00000000ffffffffULL;
    m_time = x1 | x2;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::encode(tint32& x) const
{
    tuint32 t;
    
    encode(t);
    x = static_cast<tuint32>(t);
}

//-------------------------------------------------------------------------------------------

void TimeStamp::encode(tuint32& x) const
{
    tuint64 t = m_time;
    
    t >>= 16;
    t &= 0x00000000ffffffffULL;
    x = static_cast<tuint32>(t);
}

//-------------------------------------------------------------------------------------------

void TimeStamp::encode(tint64& x) const
{
    x = static_cast<tint64>(m_time);
}

//-------------------------------------------------------------------------------------------

void TimeStamp::encode(tuint64& x) const
{
    x = m_time;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::encode(tfloat32& x) const
{
    tfloat64 t;
    encode(t);
    x = static_cast<tfloat32>(t);
}

//-------------------------------------------------------------------------------------------

void TimeStamp::encode(tfloat64& x) const
{
    tfloat64 x1;
    
    x1 = static_cast<tfloat64>(m_time & 0x00000000ffffffffULL);
    x1 /= 4294967296.0;
    x = static_cast<tfloat64>((m_time >> 32) & 0x00000000ffffffffULL);
    x += x1;
}

//-------------------------------------------------------------------------------------------

bool TimeStamp::isEmpty() const
{
    return (m_time==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

TimeStamp TimeStamp::now()
{
    TimeStamp t;
    QDateTime c(QDateTime::currentDateTime());
    
    t.year(c.date().year());
    t.month(c.date().month());
    t.day(c.date().day());
    t.hour(c.time().hour());
    t.minute(c.time().minute());
    t.second(c.time().second());
    t.millisecond(c.time().msec());
    
    return t;
}

//-------------------------------------------------------------------------------------------

TimeStamp TimeStamp::reference()
{
    TimeStamp t;
    
#if defined(OMEGA_WIN32)
    {
        DWORD tS = ::timeGetTime();
        t.secondsTotal(static_cast<tuint32>(tS) / 1000);
        t.millisecond(static_cast<tint>(tS) % 1000);
    }
#elif defined(OMEGA_MACOSX)
    static bool initFlag = false;
    static Float64 nDem = 1.0,nNum = 1.0;
    
    if(!initFlag)
    {
        struct mach_timebase_info timeBaseInfo;
        ::mach_timebase_info(&timeBaseInfo);
        nNum = static_cast<Float64>(timeBaseInfo.numer);
        nDem = static_cast<Float64>(timeBaseInfo.denom);
        initFlag = true;
    }
    UInt64 tS = mach_absolute_time();
    Float64 fT = (static_cast<Float64>(tS) / nDem) *nNum;
    tS = static_cast<UInt64>(fT);
    t.nano64(tS);
#else
    t = now();
#endif
    return t;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

void TimeStamp::set(__time64_t *t)
{
    m_time = 0;
    year(1970);
    m_time += (static_cast<tuint64>(*t) << 32) & 0xffffffff00000000ULL;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_MACOSX) || defined(OMEGA_LINUX) || defined(OMEGA_IOS)
//-------------------------------------------------------------------------------------------

void TimeStamp::set(const struct timespec *t)
{
    m_time = 0;
    year(1970);
    m_time += (static_cast<tuint64>(t->tv_sec) << 32) & 0xffffffff00000000ULL;
    m_time |= static_cast<tuint64>(t->tv_nsec) & 0x00000000ffffffffULL;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_LINUX) || defined(OMEGA_ANDROID)
//-------------------------------------------------------------------------------------------

void TimeStamp::set(time_t *t)
{
    m_time = 0;
    year(1970);
    m_time += (static_cast<tuint64>(*t) << 32) & 0xffffffff00000000ULL;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

bool TimeStamp::isLeapYear() const
{
    return isLeapYear( year() );
}

//-------------------------------------------------------------------------------------------

bool TimeStamp::isLeapYear(tint x) const
{
    if((x%4)==0)
    {
        if((x%100)==0)
        {
            if((x%400)==0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::year() const
{
    tuint32 t;
    tint yVar = 0;
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    secondsAndYear(t,yVar);
    return yVar;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::year(tint x)
{
    if(x >= 1900)
    {
        tuint32 t;
        tint yVar = 0;
    
        t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
        secondsAndYear(t,yVar);
        
        for(int i=1900;i<x;++i)
        {
            if(isLeapYear(i))
            {
                t += c_SecondsPerLeapYear;
            }
            else
            {
                t += c_SecondsPerYear;
            }
        }
        
        m_time = ((static_cast<tuint64>(t) << 32) & 0xffffffff00000000ULL) | (m_time & 0x00000000ffffffffULL);
    }    
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::month() const
{
    static tuint32 moy[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    tint yVar = 0,m = 0,d;
    tuint32 t;
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    secondsAndYear(t,yVar);
    d = static_cast<tint>(t / c_SecondsPerDay);
    
    while(d>=0 && m<12)
    {
        if(m==1 && isLeapYear(yVar))
        {
            if(d < 29)
            {
                break;
            }
        }
        else if(static_cast<tuint>(d) < moy[m])
        {
            break;
        }
        d -= moy[m];
        m++;
    }
    return (m%12)+1;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::month(tint x)
{
    static tuint32 moy[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    tint i,yVar = 0,mVar = 0;
    tuint32 t,s,ySec;
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    ySec = secondsAndYear(t,yVar);
    secondsAndMonth(t,mVar,isLeapYear(yVar));
    
    x--;
    if(x < 0)
    {
        x = 0;
    }
    else if(x >= 12)
    {
        x = 11;
    }
    
    for(i=0,s=0;i<x;++i)
    {
        if(i==1 && isLeapYear(yVar))
        {
            s += 29;
        }
        else
        {
            s += moy[i];
        }
    }
    s *= c_SecondsPerDay;
    t += ySec + s;
    m_time = ((static_cast<tuint64>(t) << 32) & 0xffffffff00000000ULL) | (m_time & 0x00000000ffffffffULL);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::day() const
{
    tint d,yVar = 0,mVar = 0;
    tuint32 t;
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    secondsAndYear(t,yVar);
    secondsAndMonth(t,mVar,isLeapYear(yVar));
    d = static_cast<tint>(t / c_SecondsPerDay);
    return d+1;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::day(tint x)
{
    tint yVar = 0,mVar = 0;
    tuint32 t,ySec,mSec;
    
    x--;
    if(x<0)
    {
        x = 0;
    }
    else if(x>31)
    {
        x = 31;
    }
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    ySec = secondsAndYear(t,yVar);
    mSec = secondsAndMonth(t,mVar,isLeapYear(yVar));
    t -= ((t / c_SecondsPerDay) * c_SecondsPerDay);
    t += (static_cast<tuint32>(x) * c_SecondsPerDay);
    t += ySec + mSec;
    m_time = ((static_cast<tuint64>(t) << 32) & 0xffffffff00000000ULL) | (m_time & 0x00000000ffffffffULL);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::hour() const
{
    tint h,yVar = 0,dVar = 0;
    tuint32 t;
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    secondsAndYear(t,yVar);
    secondsAndDay(t,dVar);
    h = static_cast<tint>(t / c_SecondsPerHour);
    return h;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::hour(tint x)
{
    tint yVar = 0,dVar = 0;
    tuint32 t,ySec,dSec;
    
    if(x<0)
    {
        x = 0;
    }
    else if(x>23)
    {
        x = 23;
    }
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    ySec = secondsAndYear(t,yVar);
    dSec = secondsAndDay(t,dVar);
    t -= ((t / c_SecondsPerHour) * c_SecondsPerHour);
    t += static_cast<tuint32>(x) * c_SecondsPerHour;
    t += dSec + ySec;
    m_time = ((static_cast<tuint64>(t) << 32) & 0xffffffff00000000ULL) | (m_time & 0x00000000ffffffffULL);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::minute() const
{
    tint m,yVar = 0,dVar = 0,hVar = 0;
    tuint32 t;
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    secondsAndYear(t,yVar);
    secondsAndDay(t,dVar);
    secondsAndHour(t,hVar);
    m = static_cast<tint>(t / c_SecondsPerMinute);
    return m;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::minute(tint x)
{
    tint yVar = 0,dVar = 0,hVar = 0;
    tuint32 t,ySec,dSec,hSec;
    
    if(x<0)
    {
        x = 0;
    }
    else if(x>59)
    {
        x = 59;
    }
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    ySec = secondsAndYear(t,yVar);
    dSec = secondsAndDay(t,dVar);
    hSec = secondsAndHour(t,hVar);
    t -= ((t / c_SecondsPerMinute) * c_SecondsPerMinute);
    t += static_cast<tuint32>(x) * c_SecondsPerMinute;
    t += ySec + dSec + hSec;
    m_time = ((static_cast<tuint64>(t) << 32) & 0xffffffff00000000ULL) | (m_time & 0x00000000ffffffffULL);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::second() const
{
    tint s,yVar = 0,dVar = 0,hVar = 0,mVar = 0;
    tuint32 t;
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    secondsAndYear(t,yVar);
    secondsAndDay(t,dVar);
    secondsAndHour(t,hVar);
    secondsAndMinute(t,mVar);
    s = static_cast<tint>(t);
    return s;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::second(tint x)
{
    tint yVar = 0,dVar = 0,hVar = 0,mVar = 0;
    tuint32 t,ySec,dSec,hSec,mSec;
    
    if(x<0)
    {
        x = 0;
    }
    else if(x>59)
    {
        x = 59;
    }
    
    t = static_cast<tuint32>((m_time >> 32) & 0x00000000ffffffffULL);
    ySec = secondsAndYear(t,yVar);
    dSec = secondsAndDay(t,dVar);
    hSec = secondsAndHour(t,hVar);
    mSec = secondsAndMinute(t,mVar);
    t = static_cast<tuint32>(x);
    t += ySec + dSec + hSec + mSec;
    m_time = ((static_cast<tuint64>(t) << 32) & 0xffffffff00000000ULL) | (m_time & 0x00000000ffffffffULL);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::millisecond() const
{
    tuint64 t;
    
    t = m_time & 0x00000000ffffffffULL;
    t *= 1000;
    t >>= 32;
    return static_cast<tint>(t);
}

//-------------------------------------------------------------------------------------------

void TimeStamp::millisecond(tint x)
{
    tuint64 t;
    
    t = static_cast<tuint64>(x) % 1000;
    t <<= 32;
    t /= 1000;
    m_time = (m_time & 0xffffffff00000000ULL) | (t & 0x00000000ffffffffULL);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::microsecond() const
{
    tuint64 t;
    
    t = m_time & 0x00000000ffffffffULL;
    if(t)
    {
        t *= 1000000;
        t >>= 32;
        t++;
    }
    else
    {
        t = 0;
    }
    return static_cast<tint>(t);
}

//-------------------------------------------------------------------------------------------

void TimeStamp::microsecond(tint x)
{
    tuint64 t;
    
    if(x)
    {
        t = static_cast<tuint64>(x) % 1000000;
        t <<= 32;
        t /= 1000000;
    }
    else
    {
        t = 0;
    }
    m_time = (m_time & 0xffffffff00000000ULL) | (t & 0x00000000ffffffffULL);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::nanosecond() const
{
    tuint64 t;
    
    t = m_time & 0x00000000ffffffffULL;
    if(t)
    {
        t *= 1000000000;
        t >>= 32;
        t++;
    }
    else
    {
        t = 0;
    }
    return static_cast<tint>(t);
}

//-------------------------------------------------------------------------------------------

void TimeStamp::nanosecond(tint x)
{
    tuint64 t;
    
    if(x)
    {
        t = static_cast<tuint64>(x) % 1000000000;
        t <<= 32;
        t /= 1000000000;
    }
    else
    {
        t = 0;
    }
    m_time = (m_time & 0xffffffff00000000ULL) | (t & 0x00000000ffffffffULL);
}

//-------------------------------------------------------------------------------------------

tuint64 TimeStamp::nano64() const
{
    tuint64 xA,xB;
    
    xA = (m_time >> 32) & 0x00000000ffffffffULL;
    xA *= 1000000000;
    xB = m_time & 0x00000000ffffffffULL;
    if(xB)
    {
        xB *= 1000000000;
        xB >>= 32;
        xA += xB + 1;
    }
    return xA;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::nano64(tuint32 hi,tuint32 lo)
{
    tuint64 x = ((static_cast<tuint64>(hi) << 32) & 0xffffffff00000000ULL) | (static_cast<tuint64>(lo) & 0x00000000ffffffffULL);
    nano64(x);
}

//-------------------------------------------------------------------------------------------

void TimeStamp::nano64(tuint64 x)
{
    tuint64 xA,xB;
    
    xA = x / 1000000000;
    xA <<= 32;
    xA &= 0xffffffff00000000ULL;
    xB = x % 1000000000;
    xB <<= 32;
    xB /= 1000000000;
    xB &= 0x00000000ffffffffULL;
    m_time = xA | xB;
}

//-------------------------------------------------------------------------------------------

tuint32 TimeStamp::secondsTotal() const
{
    tuint64 t = (m_time >> 32) & 0x00000000ffffffffULL;
    return static_cast<tuint32>(t);
}

//-------------------------------------------------------------------------------------------

void TimeStamp::secondsTotal(tint x)
{
    secondsTotal(static_cast<tuint32>(x));
}

//-------------------------------------------------------------------------------------------

void TimeStamp::secondsTotal(tuint32 x)
{
    tuint64 t = (static_cast<tuint64>(x) << 32);
    m_time = (t & 0xffffffff00000000ULL) | (m_time & 0x00000000ffffffffULL);
}

//-------------------------------------------------------------------------------------------

tuint32 TimeStamp::secondsAndYear(tuint32& t,tint& year) const
{
    tuint32 x = 0;
    bool loop = true;
    
    year = 1900;
    
    while(loop)
    {
        if(isLeapYear(year))
        {
            if(t < c_SecondsPerLeapYear)
            {
                loop = false;
            }
            else
            {
                t -= c_SecondsPerLeapYear;
                x += c_SecondsPerLeapYear;
                year++;
            }
        }
        else
        {
            if(t < c_SecondsPerYear)
            {
                loop = false;
            }
            else
            {
                t -= c_SecondsPerYear;
                x += c_SecondsPerYear;
                year++;
            }
        }
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tuint32 TimeStamp::secondsAndMonth(tuint32& t,tint& month,bool leap) const
{
    static tuint32 moy[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    tuint32 mSec,x = 0;
    bool loop = true;
    
    month = 0;
    
    while(loop && month<12)
    {
        mSec = (month==1 && leap) ? 29 : moy[month];
        mSec *= c_SecondsPerDay;
        if(mSec <= t)
        {
            t -= mSec;
            x += mSec;
            month++;
        }
        else
        {
            loop = false;
        }
    }
    return x;
}

//-------------------------------------------------------------------------------------------

tuint32 TimeStamp::secondsAndDay(tuint32& t,tint& day) const
{
    tuint32 sec;
    
    day = static_cast<tint>(t / c_SecondsPerDay);
    sec = static_cast<tuint32>(day) * c_SecondsPerDay;
    t %= c_SecondsPerDay;
    return sec;
}

//-------------------------------------------------------------------------------------------

tuint32 TimeStamp::secondsAndHour(tuint32& t,tint& hour) const
{
    tuint32 sec;
    
    hour = static_cast<tint>(t / c_SecondsPerHour);
    sec = static_cast<tuint32>(hour) * c_SecondsPerHour;
    t %= c_SecondsPerHour;
    return sec;
}

//-------------------------------------------------------------------------------------------

tuint32 TimeStamp::secondsAndMinute(tuint32& t,tint& min) const
{
    tuint32 sec;
    
    min = static_cast<tint>(t / c_SecondsPerMinute);
    sec = static_cast<tuint32>(min) * c_SecondsPerMinute;
    t %= c_SecondsPerMinute;
    return sec;
}

//-------------------------------------------------------------------------------------------

void TimeStamp::set(const struct tm *t)
{
    m_time = 0;
    if(t!=0)
    {
        year(t->tm_year + 1900);
        month(t->tm_mon + 1);
        day(t->tm_mday);
        hour(t->tm_hour);
        minute(t->tm_min);
        second(t->tm_sec);
    }
}

//-------------------------------------------------------------------------------------------

void TimeStamp::get(struct tm *t) const
{
    if(t!=0)
    {
        ::memset(t,0,sizeof(struct tm));
        t->tm_sec = second();
        t->tm_min = minute();
        t->tm_hour = hour();
        t->tm_mday = day();
        t->tm_mon = month() - 1;
        t->tm_year = year() - 1900;
        t->tm_wday = dayOfWeek();
        t->tm_yday = dayOfYear();
    }
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::dayOfWeek() const
{
    static tint pshift[7] = {6, 0, 1, 2, 3, 4, 5};
    static tint nshift[7] = {6, 5, 4, 3, 2, 1, 0};
    tint y,o=0,yearV;
    
    yearV = year();
    if(yearV>=2000)
    {
        y = 2000;
        
        while(y < yearV)
        {
            o += (isLeapYear(y)) ? 366 : 365;
            y++;
        }
        o += dayOfYear();
        return pshift[o%7];
    }
    else
    {
        y = 1999;
        
        while(y < yearV)
        {
            o -= (isLeapYear(y)) ? 366 : 365;
            y--;
        }
        o -= (isLeapYear(y)) ? (366 - dayOfYear()) : (365 - dayOfYear());
        return nshift[(-o)%7];
    }
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::dayOfYear() const
{
    static tint moy[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    tint i,x,monthV;
    bool leapFlag = isLeapYear();
    
    monthV = month() - 1;
    for(i=0,x=0;i<monthV;++i)
    {
        x += moy[i];
        if(i==1 && leapFlag)
        {
            x++;
        }
    }
    x += day() - 1;
    return x;
}

//-------------------------------------------------------------------------------------------

TimeStamp::operator tint32 () const
{
    tint32 x;
    encode(x);
    return x;
}

//-------------------------------------------------------------------------------------------

TimeStamp::operator tuint32 () const
{
    tuint32 x;
    encode(x);
    return x;
}

//-------------------------------------------------------------------------------------------

TimeStamp::operator tint64 () const
{
    tint64 x;
    encode(x);
    return x;
}

//-------------------------------------------------------------------------------------------

TimeStamp::operator tuint64 () const
{
    tuint64 x;
    encode(x);
    return x;
}

//-------------------------------------------------------------------------------------------

TimeStamp::operator tfloat32 () const
{
    tfloat32 x;
    encode(x);
    return x;
}

//-------------------------------------------------------------------------------------------

TimeStamp::operator tfloat64 () const
{
    tfloat64 x;
    encode(x);
    return x;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator += (tint32 x)
{
    TimeStamp t(x);
    m_time += t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator += (tuint32 x)
{
    TimeStamp t(x);
    m_time += t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator += (tint64 x)
{
    TimeStamp t(x);
    m_time += t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator += (tuint64 x)
{
    TimeStamp t(x);
    m_time += t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator += (tfloat32 x)
{
    TimeStamp t(x);
    m_time += t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator += (tfloat64 x)
{
    TimeStamp t(x);
    m_time += t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator += (const TimeStamp& x)
{
    m_time += x.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator -= (tint32 x)
{
    TimeStamp t(x);
    m_time -= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator -= (tuint32 x)
{
    TimeStamp t(x);
    m_time -= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator -= (tint64 x)
{
    TimeStamp t(x);
    m_time -= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator -= (tuint64 x)
{
    TimeStamp t(x);
    m_time -= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator -= (tfloat32 x)
{
    TimeStamp t(x);
    m_time -= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator -= (tfloat64 x)
{
    TimeStamp t(x);
    m_time -= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator -= (const TimeStamp& x)
{
    m_time -= x.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator *= (tint32 x)
{
    TimeStamp t(x);
    m_time *= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator *= (tuint32 x)
{
    TimeStamp t(x);
    m_time *= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator *= (tint64 x)
{
    TimeStamp t(x);
    m_time *= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator *= (tuint64 x)
{
    TimeStamp t(x);
    m_time *= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator *= (tfloat32 x)
{
    TimeStamp t(x);
    m_time *= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator *= (tfloat64 x)
{
    TimeStamp t(x);
    m_time *= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator *= (const TimeStamp& x)
{
    m_time *= x.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator /= (tint32 x)
{
    TimeStamp t(x);
    m_time /= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator /= (tuint32 x)
{
    TimeStamp t(x);
    m_time /= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator /= (tint64 x)
{
    TimeStamp t(x);
    m_time /= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator /= (tuint64 x)
{
    TimeStamp t(x);
    m_time /= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator /= (tfloat32 x)
{
    TimeStamp t(x);
    m_time /= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator /= (tfloat64 x)
{
    TimeStamp t(x);
    m_time /= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator /= (const TimeStamp& x)
{
    m_time /= x.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator %= (tint32 x)
{
    TimeStamp t(x);
    m_time %= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator %= (tuint32 x)
{
    TimeStamp t(x);
    m_time %= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator %= (tint64 x)
{
    TimeStamp t(x);
    m_time %= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator %= (tuint64 x)
{
    TimeStamp t(x);
    m_time %= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator %= (tfloat32 x)
{
    TimeStamp t(x);
    m_time %= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator %= (tfloat64 x)
{
    TimeStamp t(x);
    m_time %= t.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

const TimeStamp& TimeStamp::operator %= (const TimeStamp& x)
{
    m_time %= x.m_time;
    return *this;
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::compare(tint32 x) const
{
    TimeStamp t(x);
    return compare(t);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::compare(tuint32 x) const
{
    TimeStamp t(x);
    return compare(t);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::compare(tint64 x) const
{
    TimeStamp t(x);
    return compare(t);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::compare(tuint64 x) const
{
    TimeStamp t(x);
    return compare(t);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::compare(tfloat32 x) const
{
    TimeStamp t(x);
    return compare(t);
}

//-------------------------------------------------------------------------------------------

tint TimeStamp::compare(tfloat64 x) const
{
    TimeStamp t(x);
    return compare(t);
}

//-------------------------------------------------------------------------------------------
// this < x = -1
// this = x = 0
// this > x = 1
//-------------------------------------------------------------------------------------------

tint TimeStamp::compare(const TimeStamp& x) const
{
    if(m_time < x.m_time)
    {
        return -1;
    }
    else if(m_time > x.m_time)
    {
        return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------
// operator <
//-------------------------------------------------------------------------------------------

bool operator < (const TimeStamp& a,tint32 b)
{
    return (a.compare(b) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const TimeStamp& a,tuint32 b)
{
    return (a.compare(b) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const TimeStamp& a,tint64 b)
{
    return (a.compare(b) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const TimeStamp& a,tuint64 b)
{
    return (a.compare(b) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const TimeStamp& a,tfloat32 b)
{
    return (a.compare(b) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const TimeStamp& a,tfloat64 b)
{
    return (a.compare(b) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (tint32 a,const TimeStamp& b)
{
    return (b.compare(a) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (tuint32 a,const TimeStamp& b)
{
    return (b.compare(a) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (tint64 a,const TimeStamp& b)
{
    return (b.compare(a) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (tuint64 a,const TimeStamp& b)
{
    return (b.compare(a) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (tfloat32 a,const TimeStamp& b)
{
    return (b.compare(a) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (tfloat64 a,const TimeStamp& b)
{
    return (b.compare(a) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const TimeStamp& a,const TimeStamp& b)
{
    return (a.compare(b) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------
// operator <=
//-------------------------------------------------------------------------------------------

bool operator <= (const TimeStamp& a,tint32 b)
{
    return (a.compare(b) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const TimeStamp& a,tuint32 b)
{
    return (a.compare(b) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const TimeStamp& a,tint64 b)
{
    return (a.compare(b) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const TimeStamp& a,tuint64 b)
{
    return (a.compare(b) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const TimeStamp& a,tfloat32 b)
{
    return (a.compare(b) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const TimeStamp& a,tfloat64 b)
{
    return (a.compare(b) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (tint32 a,const TimeStamp& b)
{
    return (b.compare(a) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (tuint32 a,const TimeStamp& b)
{
    return (b.compare(a) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (tint64 a,const TimeStamp& b)
{
    return (b.compare(a) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (tuint64 a,const TimeStamp& b)
{
    return (b.compare(a) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (tfloat32 a,const TimeStamp& b)
{
    return (b.compare(a) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (tfloat64 a,const TimeStamp& b)
{
    return (b.compare(a) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const TimeStamp& a,const TimeStamp& b)
{
    return (a.compare(b) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------
// operator >
//-------------------------------------------------------------------------------------------

bool operator >  (const TimeStamp& a,tint32 b)
{
    return (a.compare(b) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (const TimeStamp& a,tuint32 b)
{
    return (a.compare(b) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (const TimeStamp& a,tint64 b)
{
    return (a.compare(b) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (const TimeStamp& a,tuint64 b)
{
    return (a.compare(b) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (const TimeStamp& a,tfloat32 b)
{
    return (a.compare(b) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (const TimeStamp& a,tfloat64 b)
{
    return (a.compare(b) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (tint32 a,const TimeStamp& b)
{
    return (b.compare(a) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (tuint32 a,const TimeStamp& b)
{
    return (b.compare(a) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (tint64 a,const TimeStamp& b)
{
    return (b.compare(a) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (tuint64 a,const TimeStamp& b)
{
    return (b.compare(a) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (tfloat32 a,const TimeStamp& b)
{
    return (b.compare(a) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (tfloat64 a,const TimeStamp& b)
{
    return (b.compare(a) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >  (const TimeStamp& a,const TimeStamp& b)
{
    return (a.compare(b) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------
// operator >=
//-------------------------------------------------------------------------------------------

bool operator >= (const TimeStamp& a,tint32 b)
{
    return (a.compare(b) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const TimeStamp& a,tuint32 b)
{
    return (a.compare(b) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const TimeStamp& a,tint64 b)
{
    return (a.compare(b) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const TimeStamp& a,tuint64 b)
{
    return (a.compare(b) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const TimeStamp& a,tfloat32 b)
{
    return (a.compare(b) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const TimeStamp& a,tfloat64 b)
{
    return (a.compare(b) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (tint32 a,const TimeStamp& b)
{
    return (b.compare(a) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (tuint32 a,const TimeStamp& b)
{
    return (b.compare(a) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (tint64 a,const TimeStamp& b)
{
    return (b.compare(a) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (tuint64 a,const TimeStamp& b)
{
    return (b.compare(a) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (tfloat32 a,const TimeStamp& b)
{
    return (b.compare(a) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (tfloat64 a,const TimeStamp& b)
{
    return (b.compare(a) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const TimeStamp& a,const TimeStamp& b)
{
    return (a.compare(b) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------
// operator !=
//-------------------------------------------------------------------------------------------

bool operator != (const TimeStamp& a,tint32 b)
{
    return (a.compare(b) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const TimeStamp& a,tuint32 b)
{
    return (a.compare(b) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const TimeStamp& a,tint64 b)
{
    return (a.compare(b) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const TimeStamp& a,tuint64 b)
{
    return (a.compare(b) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const TimeStamp& a,tfloat32 b)
{
    return (a.compare(b) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const TimeStamp& a,tfloat64 b)
{
    return (a.compare(b) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (tint32 a,const TimeStamp& b)
{
    return (b.compare(a) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (tuint32 a,const TimeStamp& b)
{
    return (b.compare(a) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (tint64 a,const TimeStamp& b)
{
    return (b.compare(a) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (tuint64 a,const TimeStamp& b)
{
    return (b.compare(a) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (tfloat32 a,const TimeStamp& b)
{
    return (b.compare(a) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (tfloat64 a,const TimeStamp& b)
{
    return (b.compare(a) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const TimeStamp& a,const TimeStamp& b)
{
    return (a.compare(b) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------
// operator ==
//-------------------------------------------------------------------------------------------

bool operator == (const TimeStamp& a,tint32 b)
{
    return (a.compare(b) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const TimeStamp& a,tuint32 b)
{
    return (a.compare(b) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const TimeStamp& a,tint64 b)
{
    return (a.compare(b) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const TimeStamp& a,tuint64 b)
{
    return (a.compare(b) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const TimeStamp& a,tfloat32 b)
{
    return (a.compare(b) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const TimeStamp& a,tfloat64 b)
{
    return (a.compare(b) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (tint32 a,const TimeStamp& b)
{
    return (b.compare(a) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (tuint32 a,const TimeStamp& b)
{
    return (b.compare(a) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (tint64 a,const TimeStamp& b)
{
    return (b.compare(a) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (tuint64 a,const TimeStamp& b)
{
    return (b.compare(a) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (tfloat32 a,const TimeStamp& b)
{
    return (b.compare(a) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (tfloat64 a,const TimeStamp& b)
{
    return (b.compare(a) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const TimeStamp& a,const TimeStamp& b)
{
    return (a.compare(b) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------
// operator +
//-------------------------------------------------------------------------------------------

TimeStamp operator + (const TimeStamp& a,tint32 b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (const TimeStamp& a,tuint32 b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (const TimeStamp& a,tint64 b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (const TimeStamp& a,tuint64 b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (const TimeStamp& a,tfloat32 b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (const TimeStamp& a,tfloat64 b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (tint32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (tuint32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (tint64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (tuint64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (tfloat32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (tfloat64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator + (const TimeStamp& a,const TimeStamp& b)
{
    TimeStamp c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------
// operator -
//-------------------------------------------------------------------------------------------

TimeStamp operator - (const TimeStamp& a,tint32 b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (const TimeStamp& a,tuint32 b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (const TimeStamp& a,tint64 b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (const TimeStamp& a,tuint64 b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (const TimeStamp& a,tfloat32 b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (const TimeStamp& a,tfloat64 b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (tint32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (tuint32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (tint64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (tuint64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (tfloat32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (tfloat64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator - (const TimeStamp& a,const TimeStamp& b)
{
    TimeStamp c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------
// operator *
//-------------------------------------------------------------------------------------------

TimeStamp operator * (const TimeStamp& a,tint32 b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (const TimeStamp& a,tuint32 b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (const TimeStamp& a,tint64 b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (const TimeStamp& a,tuint64 b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (const TimeStamp& a,tfloat32 b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (const TimeStamp& a,tfloat64 b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (tint32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (tuint32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (tint64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (tuint64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (tfloat32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (tfloat64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator * (const TimeStamp& a,const TimeStamp& b)
{
    TimeStamp c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------
// operator /
//-------------------------------------------------------------------------------------------

TimeStamp operator / (const TimeStamp& a,tint32 b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (const TimeStamp& a,tuint32 b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}
//-------------------------------------------------------------------------------------------

TimeStamp operator / (const TimeStamp& a,tint64 b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (const TimeStamp& a,tuint64 b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (const TimeStamp& a,tfloat32 b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (const TimeStamp& a,tfloat64 b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (tint32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (tuint32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (tint64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (tuint64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (tfloat32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (tfloat64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator / (const TimeStamp& a,const TimeStamp& b)
{
    TimeStamp c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------
// operator %
//-------------------------------------------------------------------------------------------

TimeStamp operator % (const TimeStamp& a,tint32 b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (const TimeStamp& a,tuint32 b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (const TimeStamp& a,tint64 b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (const TimeStamp& a,tuint64 b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (const TimeStamp& a,tfloat32 b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (const TimeStamp& a,tfloat64 b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (tint32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (tuint32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (tint64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (tuint64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (tfloat32 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (tfloat64 a,const TimeStamp& b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TimeStamp operator % (const TimeStamp& a,const TimeStamp& b)
{
    TimeStamp c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
