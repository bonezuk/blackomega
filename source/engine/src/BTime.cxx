#include "engine/inc/BTime.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

BTime::BTime() : m_Seconds(0),
    m_MicroSeconds(0)
{}

//-------------------------------------------------------------------------------------------

BTime::BTime(const BTime& x) : m_Seconds(x.m_Seconds),
    m_MicroSeconds(x.m_MicroSeconds)
{}

//-------------------------------------------------------------------------------------------

BTime::BTime(tint seconds,tint micro) : m_Seconds(seconds),
    m_MicroSeconds(micro%1000000)
{}

//-------------------------------------------------------------------------------------------

BTime::BTime(tfloat32 x) : m_Seconds(0),
    m_MicroSeconds(0)
{
    tfloat64 t1;
    
    t1=static_cast<tfloat64>(x);
    t1*=1000000.0;
    t1=fmod(t1,1000000.0);
    m_Seconds=static_cast<tint>(x);
    m_MicroSeconds=static_cast<tint>(t1);
}

//-------------------------------------------------------------------------------------------

BTime::BTime(tfloat64 x) : m_Seconds(0),
    m_MicroSeconds(0)
{
    tfloat64 t1;
    
    t1=x;
    t1*=1000000.0;
    t1=fmod(t1,1000000.0);
    m_Seconds=static_cast<tint>(x);
    m_MicroSeconds=static_cast<tint>(t1);
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator = (const BTime &x) 
{
    if(&x!=this)
    {
        m_Seconds=x.m_Seconds;
        m_MicroSeconds=x.m_MicroSeconds;
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator = (tint x) 
{
    m_Seconds=x;
    m_MicroSeconds=0;
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator = (tfloat32 x) 
{
    tfloat64 t1;
    
    t1=static_cast<tfloat64>(x);
    t1*=1000000.0;
    t1=fmod(t1,1000000.0);
    m_Seconds=static_cast<tint>(x);
    m_MicroSeconds=static_cast<tint>(t1);
    return *this;    
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator = (tfloat64 x) {
    tfloat64 t1;
    
    t1=x;
    t1*=1000000.0;
    t1=fmod(t1,1000000.0);
    m_Seconds=static_cast<tint>(x);
    m_MicroSeconds=static_cast<tint>(t1);
    return *this;    
}

//-------------------------------------------------------------------------------------------

void BTime::Add(const BTime& x) 
{
    m_MicroSeconds+=x.m_MicroSeconds;
    if(m_MicroSeconds>=1000000) 
    {
        m_MicroSeconds%=1000000;
        m_Seconds++;
    }
    m_Seconds+=x.m_Seconds;
}

//-------------------------------------------------------------------------------------------

void BTime::Sub(const BTime& x) 
{
    m_MicroSeconds-=x.m_MicroSeconds;
    if(m_MicroSeconds<0) 
    {
        m_MicroSeconds=1000000+m_MicroSeconds;
        m_Seconds--;
    }
    m_Seconds-=x.m_Seconds;    
}

//-------------------------------------------------------------------------------------------

void BTime::Multiple(tfloat64 x) 
{
    double a,b;

    a = static_cast<tfloat64>(m_MicroSeconds);
    b = static_cast<tfloat64>(m_Seconds);
    a *= x;
    b *= x;
    b += (a/1000000.0);
    a = fmod(a,1000000.0);
    m_MicroSeconds = static_cast<tint>(a);
    m_Seconds = static_cast<tint>(b);
    if(m_MicroSeconds<0) 
    {
        m_MicroSeconds=1000000+m_MicroSeconds;
        m_Seconds--;
    }
    if(m_MicroSeconds>=1000000) 
    {
        m_MicroSeconds%=1000000;
        m_Seconds++;
    }
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator += (const BTime& x) 
{
    Add(x);
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator += (tint x) 
{
    Add(BTime(x));
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator += (tfloat32 x) 
{
    Add(BTime(x));
    return *this;    
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator += (tfloat64 x) 
{
    Add(BTime(x));
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator -= (const BTime& x) 
{
    Sub(x);
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator -= (tint x) 
{
    Sub(BTime(x));
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator -= (tfloat32 x) 
{
    Sub(BTime(x));
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator -= (tfloat64 x) 
{
    Sub(BTime(x));
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator *= (tint x) 
{
    Multiple(static_cast<tfloat64>(x));
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator *= (tfloat32 x) 
{
    Multiple(static_cast<tfloat64>(x));
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime& BTime::operator *= (tfloat64 x) 
{
    Multiple(x);
    return *this;
}

//-------------------------------------------------------------------------------------------

BTime operator + (const BTime& x,const BTime& y) 
{
    BTime a(x);
    a+=y;
    return a;    
}

//-------------------------------------------------------------------------------------------

BTime operator + (const BTime& x,tint y) 
{
    BTime a(x);
    a+=y;
    return a;        
}

//-------------------------------------------------------------------------------------------

BTime operator + (const BTime& x,tfloat32 y) 
{
    BTime a(x);
    a+=y;
    return a;        
}

//-------------------------------------------------------------------------------------------

BTime operator + (const BTime& x,tfloat64 y) 
{
    BTime a(x);
    a+=y;
    return a;        
}

//-------------------------------------------------------------------------------------------

BTime operator + (tint x,const BTime& y) 
{
    BTime a(x);
    a+=y;
    return a;    
}

//-------------------------------------------------------------------------------------------

BTime operator + (tfloat32 x,const BTime& y) 
{
    BTime a(x);
    a+=y;
    return a;    
}

//-------------------------------------------------------------------------------------------

BTime operator + (tfloat64 x,const BTime& y) 
{
    BTime a(x);
    a+=y;
    return a;    
}

//-------------------------------------------------------------------------------------------

BTime operator - (const BTime& x,const BTime& y) 
{
    BTime a(x);
    a-=y;
    return a;    
}

//-------------------------------------------------------------------------------------------

BTime operator - (const BTime& x,tint y) 
{
    BTime a(x);
    a-=y;
    return a;
}

//-------------------------------------------------------------------------------------------

BTime operator - (const BTime& x,tfloat32 y) 
{
    BTime a(x);
    a-=y;
    return a;
}

//-------------------------------------------------------------------------------------------

BTime operator - (const BTime& x,tfloat64 y) 
{
    BTime a(x);
    a-=y;
    return a;
}

//-------------------------------------------------------------------------------------------

BTime operator - (tint x,const BTime& y) 
{
    BTime a(x);
    a-=y;
    return a;
}

//-------------------------------------------------------------------------------------------

BTime operator - (tfloat32 x,const BTime& y) 
{
    BTime a(x);
    a-=y;
    return a;    
}

//-------------------------------------------------------------------------------------------

BTime operator - (tfloat64 x,const BTime& y) 
{
    BTime a(x);
    a-=y;
    return a;    
}

//-------------------------------------------------------------------------------------------

BTime operator * (const BTime& x,tint y) 
{
    BTime a(x);
    a*=y;
    return a;
}

//-------------------------------------------------------------------------------------------

BTime operator * (const BTime& x,tfloat32 y) 
{
    BTime a(x);
    a*=y;
    return a;
}

//-------------------------------------------------------------------------------------------

BTime operator * (const BTime& x,tfloat64 y) 
{
    BTime a(x);
    a*=y;
    return a;
}

//-------------------------------------------------------------------------------------------

BTime operator * (tint x,const BTime& y) 
{
    BTime a(y);
    a*=x;
    return a;
}

//-------------------------------------------------------------------------------------------

BTime operator * (tfloat32 x,const BTime& y) 
{
    BTime a(y);
    a*=x;
    return a;
}

//-------------------------------------------------------------------------------------------

BTime operator * (tfloat64 x,const BTime& y) 
{
    BTime a(y);
    a*=x;
    return a;
}

//-------------------------------------------------------------------------------------------
// BTime Comparison method
//    x < y -> -1
//    x = y -> 0
//    x > y -> 1
//-------------------------------------------------------------------------------------------

tint BTime::Cmp(const BTime& x,const BTime& y) 
{
    if(x.m_Seconds < y.m_Seconds) 
    {
        return -1;
    }
    else if(x.m_Seconds == y.m_Seconds) 
    {
        if(x.m_MicroSeconds < y.m_MicroSeconds) 
        {
            return -1;
        }
        else if(x.m_MicroSeconds == y.m_MicroSeconds) 
        {
            return 0;
        }
    }
    return 1;
}

//-------------------------------------------------------------------------------------------

bool operator == (const BTime& x,const BTime& y) 
{
    return (BTime::Cmp(x,y)==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const BTime& x,tint y) 
{
    return (BTime::Cmp(x,BTime(y))==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const BTime& x,tfloat32 y) 
{
    return (BTime::Cmp(x,BTime(y))==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (const BTime& x,tfloat64 y) 
{
    return (BTime::Cmp(x,BTime(y))==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (tint x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (tfloat32 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator == (tfloat64 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const BTime& x,const BTime& y) 
{
    return (BTime::Cmp(x,y)>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const BTime& x,tint y) 
{
    return (BTime::Cmp(x,BTime(y))>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const BTime& x,tfloat32 y) 
{
    return (BTime::Cmp(x,BTime(y))>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const BTime& x,tfloat64 y) 
{
    return (BTime::Cmp(x,BTime(y))>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (tint x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (tfloat32 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (tfloat64 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (const BTime& x,const BTime& y) 
{
    return (BTime::Cmp(x,y)>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (const BTime& x,tint y) 
{
    return (BTime::Cmp(x,BTime(y))>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (const BTime& x,tfloat32 y) 
{
    return (BTime::Cmp(x,BTime(y))>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (const BTime& x,tfloat64 y) 
{
    return (BTime::Cmp(x,BTime(y))>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (tint x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (tfloat32 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (tfloat64 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)>0) ? true : false;
}

//-------------------------------------------------------------------------------------------
        
bool operator <= (const BTime& x,const BTime& y) 
{
    return (BTime::Cmp(x,y)<=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const BTime& x,tint y) 
{
    return (BTime::Cmp(x,BTime(y))<=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const BTime& x,tfloat32 y) 
{
    return (BTime::Cmp(x,BTime(y))<=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const BTime& x,tfloat64 y) 
{
    return (BTime::Cmp(x,BTime(y))<=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (tint x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)<=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (tfloat32 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)<=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (tfloat64 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)<=0) ? true : false;
}

//-------------------------------------------------------------------------------------------
        
bool operator < (const BTime& x,const BTime& y) 
{
    return (BTime::Cmp(x,y)<0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const BTime& x,tint y) 
{
    return (BTime::Cmp(x,BTime(y))<0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const BTime& x,tfloat32 y) 
{
    return (BTime::Cmp(x,BTime(y))<0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const BTime& x,tfloat64 y) 
{
    return (BTime::Cmp(x,BTime(y))<0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (tint x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)<0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (tfloat32 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)<0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (tfloat64 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)<0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const BTime& x,const BTime& y) 
{
    return (BTime::Cmp(x,y)!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const BTime& x,tint y) 
{
    return (BTime::Cmp(x,BTime(y))!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const BTime& x,tfloat32 y) 
{
    return (BTime::Cmp(x,BTime(y))!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const BTime& x,tfloat64 y) 
{
    return (BTime::Cmp(x,BTime(y))!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (tint x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (tfloat32 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (tfloat64 x,const BTime& y) 
{
    return (BTime::Cmp(BTime(x),y)!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

common::BString BTime::ToString(tint x) const
{
    common::BString a;
    tchar b[2]={'\0','\0'};
    
    if(x<0) 
    {
        x=-x;
        a << "-";
    }
    do 
    {
        b[0]=static_cast<tchar>((x%10)+'0');
        a=b+a;
        x/=10;
    } while(x);
    return a;
}

//-------------------------------------------------------------------------------------------

common::BString BTime::SecondString() const
{
    return ToString(m_Seconds);
}

//-------------------------------------------------------------------------------------------

common::BString BTime::MicroString() const
{
    return ToString(m_MicroSeconds);
}

//-------------------------------------------------------------------------------------------

BTime::operator tfloat64() const 
{
    tfloat64 x;

    x = static_cast<tfloat64>(m_MicroSeconds) / 1000000.0;
    x += static_cast<tfloat64>(m_Seconds);
    return x;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------