#include "common/inc/TTime.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

TTime::TTime() : m_Year(0), m_yDay(0), m_Sec(0)
{}

//-------------------------------------------------------------------------------------------

TTime::TTime(const time_t *t) : m_Year(0), m_yDay(0), m_Sec(0)
{
    Equal(t);
}

//-------------------------------------------------------------------------------------------

TTime::TTime(const struct tm *t) : m_Year(0), m_yDay(0), m_Sec(0)
{
    Equal(t);
}

//-------------------------------------------------------------------------------------------

TTime::TTime(const TTime& t) : m_Year(0), m_yDay(0), m_Sec(0)
{
    Equal(t);
}

//-------------------------------------------------------------------------------------------

TTime::TTime(tint t) : m_Year(0), m_yDay(0), m_Sec(0)
{
    Equal(t);
}

//-------------------------------------------------------------------------------------------

TTime::TTime(const BString& str) : m_Year(0), m_yDay(0), m_Sec(0)
{
    ToStr(str);
}

//-------------------------------------------------------------------------------------------

TTime::TTime(const tchar *str) : m_Year(0), m_yDay(0), m_Sec(0)
{
    BString s(str);
    ToStr(s);
}

//-------------------------------------------------------------------------------------------

TTime TTime::Now()
{
    TTime n;
    time_t t;

    ::time(&t); //lint !e534 Value adjusted by pointer parameter.
    n = &t;
    return n;
}

//-------------------------------------------------------------------------------------------

bool TTime::IsEmpty() const
{
    if(m_Year==0 && m_yDay==0 && m_Sec==0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

bool TTime::IsLeapYear() const
{
    return IsLeapYear(m_Year);
}

//-------------------------------------------------------------------------------------------

bool TTime::IsLeapYear(tint year) const
{
    return TTime::LeapYear(year);
}

//-------------------------------------------------------------------------------------------

bool TTime::LeapYear(tint year)
{
    if((year%4)==0)
    {
        if((year%100)==0)
        {
            if((year%400)==0)
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

tint TTime::DayOfWeek() const
{
    static tint pshift[7] = {6, 0, 1, 2, 3, 4, 5};
    static tint nshift[7] = {6, 5, 4, 3, 2, 1, 0};
    tint y,o = 0;

    if(m_Year>=2000)
    {
        y=2000;

        while(y<m_Year)
        {
            o += (IsLeapYear(y)) ? 366 : 365;
            y++;
        }
        o += m_yDay;
        return pshift[o%7];
    }
    else
    {
        y=1999;

        while(y>m_Year)
        {
            o -= (IsLeapYear(y)) ? 366 : 365;
            y--;
        }
        o -= (IsLeapYear(m_Year)) ? (366 - m_yDay) : (365 - m_yDay);
        return nshift[(-o)%7];
    }
}

//-------------------------------------------------------------------------------------------

tint TTime::MonthOfYear() const
{
    static tint moy[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    tint d = m_yDay,m = 0;

    while(d>=0 && m<12)
    {
        if(m==1 && IsLeapYear(m_Year))
        {
            if(d < 29)
            {
                break;
            }
        }
        else if(d < moy[m])
        {
            break;
        }
        d -= moy[m];
        m++;
    }
    return (m%12);
}

//-------------------------------------------------------------------------------------------

const tint c_SecondsPerHour = 60 * 60;

tint TTime::Hour() const
{
    return m_Sec / c_SecondsPerHour;
}

//-------------------------------------------------------------------------------------------

tint TTime::Minute() const
{
    return (m_Sec % c_SecondsPerHour) / 60;
}

//-------------------------------------------------------------------------------------------

tint TTime::DayOfMonth() const
{
    static tint moy[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    tint d = m_yDay,m = 0;

    while(d>=0 && m<12)
    {
        if(m==1 && IsLeapYear(m_Year))
        {
            if(d < 29)
            {
                break;
            }
            else
            {
                d--;
            }
        }
        else if(d < moy[m])
        {
            break;
        }
        d -= moy[m];
        m++;
    }
    return d+1;
}

//-------------------------------------------------------------------------------------------

const tint c_SecondsPerDay = 60 * 60 * 24;

void TTime::Increment(tint x)
{
    if(m_Year)
    {
        if(x>=0)
        {
            tint dYear = (IsLeapYear()) ? 366 : 365;

            while(x>=(c_SecondsPerDay - m_Sec))
            {
                m_yDay++;
                if(m_yDay>=dYear)
                {
                    m_yDay = 0;
                    m_Year++;
                    dYear = (IsLeapYear()) ? 366 : 365;
                }
                x -= (c_SecondsPerDay - m_Sec);
                m_Sec = 0;
            }
            m_Sec += x;
        }
        else
        {
            x = -x;

            while(x>0)
            {
                if(x > m_Sec)
                {
                    m_yDay--;
                    if(m_yDay<0)
                    {
                        m_Year--;
                        m_yDay = (IsLeapYear()) ? 365 : 364;
                    }
                    x -= m_Sec;
                    m_Sec = c_SecondsPerDay;
                }
                else
                {
                    m_Sec -= x;
                    x = 0;
                }
            }
        }
    }
    else
    {
        m_Sec += x;
    }
}

//-------------------------------------------------------------------------------------------

void TTime::GMTime(struct tm *t) const
{
    t->tm_sec = m_Sec % 60;
    t->tm_min = Minute();
    t->tm_hour = Hour();
    t->tm_mday = DayOfMonth();
    t->tm_mon = MonthOfYear();
    t->tm_year = m_Year - 1900;
    t->tm_wday = DayOfWeek();
    t->tm_yday = m_yDay;
    t->tm_isdst = 1;
}

//-------------------------------------------------------------------------------------------

void TTime::Equal(const time_t *t)
{
    struct tm *gmt;

    gmt = ::gmtime(t);
    Equal(gmt);
}

//-------------------------------------------------------------------------------------------

void TTime::Equal(const struct tm *t)
{
    m_Year = 1900 + t->tm_year;
    m_yDay = t->tm_yday;
    m_Sec = (t->tm_hour * c_SecondsPerHour) + (t->tm_min * 60) + t->tm_sec;
}

//-------------------------------------------------------------------------------------------

void TTime::Equal(const TTime& t)
{
    m_Year = t.m_Year;
    m_yDay = t.m_yDay;
    m_Sec = t.m_Sec;
}

//-------------------------------------------------------------------------------------------

void TTime::Equal(tint t)
{
    m_Year = 0;
    m_yDay = 0;
    m_Sec = t;
}

//-------------------------------------------------------------------------------------------

tint TTime::Year() const
{
    return m_Year;
}

//-------------------------------------------------------------------------------------------

tint TTime::Day() const
{
    return m_yDay;
}

//-------------------------------------------------------------------------------------------

tint TTime::Seconds() const
{
    return m_Sec;
}

//-------------------------------------------------------------------------------------------

void TTime::Set(tint y,tint d,tint s)
{
    tint dYear;

    if(y>0)
    {
        m_Year = y;
        dYear = (IsLeapYear(y)) ? 366 : 365;
        if(d >= dYear)
        {
            d = dYear - 1;
        }
        if(s >= c_SecondsPerDay)
        {
            s = c_SecondsPerDay - 1;
        }
        m_yDay = d;
        m_Sec = s;
    }
}

//-------------------------------------------------------------------------------------------

TTime& TTime::operator = (const time_t *t)
{
    Equal(t);
    return *this;
}

//-------------------------------------------------------------------------------------------

TTime& TTime::operator = (const struct tm *t)
{
    Equal(t);
    return *this;
}

//-------------------------------------------------------------------------------------------

TTime& TTime::operator = (const TTime& t)
{
    if(this!=&t)
    {
        Equal(t);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

TTime& TTime::operator = (tint t)
{
    Equal(t);
    return *this;
}

//-------------------------------------------------------------------------------------------

TTime& TTime::operator = (const BString& str)
{
    ToStr(str);
    return *this;
}

//-------------------------------------------------------------------------------------------

TTime& TTime::operator = (const char *str)
{
    BString s(str);

    ToStr(s);
    return *this;
}

//-------------------------------------------------------------------------------------------

TTime& TTime::operator += (const TTime& t)
{
    if(t.m_Year==0)
    {
        Increment(t.m_Sec);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

TTime& TTime::operator += (tint t)
{
    Increment(t);
    return *this;
}

//-------------------------------------------------------------------------------------------

TTime& TTime::operator -= (const TTime& t)
{
    if(t.m_Year==0)
    {
        Increment(-t.m_Sec);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

TTime& TTime::operator -= (tint t)
{
    Increment(-t);
    return *this;
}

//-------------------------------------------------------------------------------------------

TTime operator + (const TTime& a,tint b)
{
    TTime c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TTime operator + (tint a,const TTime& b)
{
    TTime c(b);
    c += a;
    return c;
}

//-------------------------------------------------------------------------------------------

TTime operator + (const TTime& a,const TTime& b)
{
    TTime c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

TTime operator - (const TTime& a,tint b)
{
    TTime c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

TTime operator - (tint a,const TTime& b)
{
    TTime c(b);
    c -= a;
    return c;
}

//-------------------------------------------------------------------------------------------

TTime operator - (const TTime& a,const TTime& b)
{
    TTime c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------
// a < b : -1
// a = b :  0
// a > b :  1
//-------------------------------------------------------------------------------------------

int TTime::Compare(const time_t *t) const
{
    TTime b(t);
    return Compare(b);
}

//-------------------------------------------------------------------------------------------

int TTime::Compare(const struct tm *t) const
{
    TTime b(t);
    return Compare(b);
}

//-------------------------------------------------------------------------------------------

int TTime::Compare(const TTime& b) const
{
    if(m_Year < b.m_Year)
    {
        return -1;
    }
    else if(m_Year > b.m_Year)
    {
        return 1;
    }

    if(m_yDay < b.m_yDay)
    {
        return -1;
    }
    else if(m_yDay > b.m_yDay)
    {
        return 1;
    }

    if(m_Sec < b.m_Sec)
    {
        return -1;
    }
    else if(m_Sec > b.m_Sec)
    {
        return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

bool operator < (const TTime& a,const time_t *b)
{
    return (a.Compare(b) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const TTime& a,const struct tm *b)
{
    return (a.Compare(b) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const time_t *a,const TTime& b)
{
    return (b.Compare(a) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const struct tm *a,const TTime& b)
{
    return (b.Compare(a) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const TTime& a,const TTime& b)
{
    return (a.Compare(b) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const TTime& a,const time_t *b)
{
    return (a.Compare(b) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const TTime& a,const struct tm *b)
{
    return (a.Compare(b) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const time_t *a,const TTime& b)
{
    return (b.Compare(a) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const struct tm *a,const TTime& b)
{
    return (b.Compare(a) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const TTime& a,const TTime& b)
{
    return (a.Compare(b) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const TTime& a,const time_t *b)
{
    return (a.Compare(b) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const TTime& a,const struct tm *b)
{
    return (a.Compare(b) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const time_t *a,const TTime& b)
{
    return (b.Compare(a) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const struct tm *a,const TTime& b)
{
    return (b.Compare(a) <= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const TTime& a,const TTime& b)
{
    return (a.Compare(b) >= 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (const TTime& a,const time_t *b)
{
    return (a.Compare(b) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (const TTime& a,const struct tm *b)
{
    return (a.Compare(b) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (const time_t *a,const TTime& b)
{
    return (b.Compare(a) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (const struct tm *a,const TTime& b)
{
    return (b.Compare(a) < 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (const TTime& a,const TTime& b)
{
    return (a.Compare(b) > 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const TTime& a,const time_t *b)
{
    return (a.Compare(b) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const TTime& a,const struct tm *b)
{
    return (a.Compare(b) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const time_t *a,const TTime& b)
{
    return (b.Compare(a) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const struct tm *a,const TTime& b)
{
    return (b.Compare(a) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const TTime& a,const TTime& b)
{
    return (a.Compare(b) == 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const TTime& a,const time_t *b)
{
    return (a.Compare(b) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const TTime& a,const struct tm *b)
{
    return (a.Compare(b) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const time_t *a,const TTime& b)
{
    return (b.Compare(a) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const struct tm *a,const TTime& b)
{
    return (b.Compare(a) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const TTime& a,const TTime& b)
{
    return (a.Compare(b) != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------
// Time string format - YYYY-DDD-SSSSS
//-------------------------------------------------------------------------------------------

#define T_MINMAX(a,b,c) { if((a) < (b)) { (a) = (b); } else if((a) > (c)) { (a) = (c); } }

void TTime::ToStr(const BString& str)
{
    tint i,j,y=0,d=0,e=0,len,type=0,state=0;
    const tchar *s = static_cast<const tchar *>(str);

    if(s!=NULL)
    {
        for(i=0,j=0,len=str.GetLength()+1;i<len;i++)
        {
            switch(state)
            {
                case 0:
                    if(s[i]>='0' && s[i]<='9')
                    {
                        j = i;
                        state = 1;
                    }
                    break;

                case 1:
                    if(!(s[i]>='0' && s[i]<='9'))
                    {
                        state = 2;
                    }
                    break;

                default:
                    state = 0;
                    break;
            }
            if(state==2)
            {

                switch(type)
                {
                    case 0:
                        y = str.Sub(static_cast<tuint>(j),static_cast<tuint>(i-j)).Atoi();
                        T_MINMAX(y,0,9999)
                        break;

                    case 1:
                        d = str.Sub(static_cast<tuint>(j),static_cast<tuint>(i-j)).Atoi();
                        if(IsLeapYear(y))
                        {
                            T_MINMAX(d,0,365)
                        }
                        else
                        {
                            T_MINMAX(d,0,364)
                        }
                        break;

                    case 2:
                        e = str.Sub(static_cast<tuint>(j),static_cast<tuint>(i-j)).Atoi();
                        T_MINMAX(e,0,86359)
                        break;

                    default:
                        break;
                }
                state = 0;
                type++;
            }
        }

        if(type>=2)
        {
            m_Year = y;
            m_yDay = d;
            m_Sec = e;
        }
    }
}

//-------------------------------------------------------------------------------------------

BString TTime::String()
{
    BString s;

    s << BString::Int(m_Year) << "-" << BString::Int(m_yDay) << "-" << BString::Int(m_Sec);
    return s;
}

//-------------------------------------------------------------------------------------------

tint TTime::DayOfYear(const struct tm *gmt)
{
    static tint moy[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    tint i,total=0;

    if(gmt!=NULL)
    {
        for(i=0;i<gmt->tm_mon;i++)
        {
            total += moy[i];
            if(i==1 && LeapYear(gmt->tm_year + 1900))
            {
                total++;
            }
        }
        total += gmt->tm_mday - 1;
    }
    return total;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
