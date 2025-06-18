#ifndef __BLACKOMEGA_BTIME_H
#define __BLACKOMEGA_BTIME_H

#include "engine/inc/EngineDLL.h"
#include <math.h>
#include "common/inc/BString.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT BTime 
{
    protected:
        tint m_Seconds;
        tint m_MicroSeconds;

        void Add(const BTime& x);
        void Sub(const BTime& x);

        void Multiple(tfloat64 x);

        static int Cmp(const BTime& x,const BTime& y);

        common::BString ToString(tint x) const;

    public:
        BTime();
        BTime(const BTime& x);
        BTime(tint seconds,tint micro=0);
        BTime(tfloat32 x);
        BTime(tfloat64 x);
        
        tint Seconds() const;
        tint Micro() const;
        
        BTime& operator = (const BTime &x);
        BTime& operator = (tint x);
        BTime& operator = (tfloat32 x);
        BTime& operator = (tfloat64 x);
        
        BTime& operator += (const BTime &x);
        BTime& operator += (tint x);
        BTime& operator += (tfloat32 x);
        BTime& operator += (tfloat64 x);
        
        BTime& operator -= (const BTime &x);
        BTime& operator -= (tint x);
        BTime& operator -= (tfloat32 x);
        BTime& operator -= (tfloat64 x);
        
        BTime& operator *= (tint x);
        BTime& operator *= (tfloat32 x);
        BTime& operator *= (tfloat64 x);

        friend ENGINE_EXPORT BTime operator + (const BTime& x,const BTime& y);
        friend ENGINE_EXPORT BTime operator + (const BTime& x,tint y);
        friend ENGINE_EXPORT BTime operator + (const BTime& x,tfloat32 y);
        friend ENGINE_EXPORT BTime operator + (const BTime& x,tfloat64 y);
        friend ENGINE_EXPORT BTime operator + (tint x,const BTime& y);
        friend ENGINE_EXPORT BTime operator + (tfloat32 x,const BTime& y);
        friend ENGINE_EXPORT BTime operator + (tfloat64 x,const BTime& y);

        friend ENGINE_EXPORT BTime operator - (const BTime& x,const BTime& y);
        friend ENGINE_EXPORT BTime operator - (const BTime& x,tint y);
        friend ENGINE_EXPORT BTime operator - (const BTime& x,tfloat32 y);
        friend ENGINE_EXPORT BTime operator - (const BTime& x,tfloat64 y);
        friend ENGINE_EXPORT BTime operator - (tint x,const BTime& y);
        friend ENGINE_EXPORT BTime operator - (tfloat32 x,const BTime& y);
        friend ENGINE_EXPORT BTime operator - (tfloat64 x,const BTime& y);
        
        friend ENGINE_EXPORT BTime operator * (const BTime& x,tint y);
        friend ENGINE_EXPORT BTime operator * (const BTime& x,tfloat32 y);
        friend ENGINE_EXPORT BTime operator * (const BTime& x,tfloat64 y);
        friend ENGINE_EXPORT BTime operator * (tint x,const BTime& y);
        friend ENGINE_EXPORT BTime operator * (tfloat32 x,const BTime& y);
        friend ENGINE_EXPORT BTime operator * (tfloat64 x,const BTime& y);

        friend ENGINE_EXPORT bool operator == (const BTime& x,const BTime& y);
        friend ENGINE_EXPORT bool operator == (const BTime& x,tint y);
        friend ENGINE_EXPORT bool operator == (const BTime& x,tfloat32 y);
        friend ENGINE_EXPORT bool operator == (const BTime& x,tfloat64 y);
        friend ENGINE_EXPORT bool operator == (tint x,const BTime& y);
        friend ENGINE_EXPORT bool operator == (tfloat32 x,const BTime& y);
        friend ENGINE_EXPORT bool operator == (tfloat64 x,const BTime& y);
        
        friend ENGINE_EXPORT bool operator >= (const BTime& x,const BTime& y);
        friend ENGINE_EXPORT bool operator >= (const BTime& x,tint y);
        friend ENGINE_EXPORT bool operator >= (const BTime& x,tfloat32 y);
        friend ENGINE_EXPORT bool operator >= (const BTime& x,tfloat64 y);
        friend ENGINE_EXPORT bool operator >= (tint x,const BTime& y);
        friend ENGINE_EXPORT bool operator >= (tfloat32 x,const BTime& y);
        friend ENGINE_EXPORT bool operator >= (tfloat64 x,const BTime& y);

        friend ENGINE_EXPORT bool operator > (const BTime& x,const BTime& y);
        friend ENGINE_EXPORT bool operator > (const BTime& x,tint y);
        friend ENGINE_EXPORT bool operator > (const BTime& x,tfloat32 y);
        friend ENGINE_EXPORT bool operator > (const BTime& x,tfloat64 y);
        friend ENGINE_EXPORT bool operator > (tint x,const BTime& y);
        friend ENGINE_EXPORT bool operator > (tfloat32 x,const BTime& y);
        friend ENGINE_EXPORT bool operator > (tfloat64 x,const BTime& y);
        
        friend ENGINE_EXPORT bool operator <= (const BTime& x,const BTime& y);
        friend ENGINE_EXPORT bool operator <= (const BTime& x,tint y);
        friend ENGINE_EXPORT bool operator <= (const BTime& x,tfloat32 y);
        friend ENGINE_EXPORT bool operator <= (const BTime& x,tfloat64 y);
        friend ENGINE_EXPORT bool operator <= (tint x,const BTime& y);
        friend ENGINE_EXPORT bool operator <= (tfloat32 x,const BTime& y);
        friend ENGINE_EXPORT bool operator <= (tfloat64 x,const BTime& y);

        friend ENGINE_EXPORT bool operator < (const BTime& x,const BTime& y);
        friend ENGINE_EXPORT bool operator < (const BTime& x,tint y);
        friend ENGINE_EXPORT bool operator < (const BTime& x,tfloat32 y);
        friend ENGINE_EXPORT bool operator < (const BTime& x,tfloat64 y);
        friend ENGINE_EXPORT bool operator < (tint x,const BTime& y);
        friend ENGINE_EXPORT bool operator < (tfloat32 x,const BTime& y);
        friend ENGINE_EXPORT bool operator < (tfloat64 x,const BTime& y);

        friend ENGINE_EXPORT bool operator != (const BTime& x,const BTime& y);
        friend ENGINE_EXPORT bool operator != (const BTime& x,tint y);
        friend ENGINE_EXPORT bool operator != (const BTime& x,tfloat32 y);
        friend ENGINE_EXPORT bool operator != (const BTime& x,tfloat64 y);
        friend ENGINE_EXPORT bool operator != (tint x,const BTime& y);
        friend ENGINE_EXPORT bool operator != (tfloat32 x,const BTime& y);
        friend ENGINE_EXPORT bool operator != (tfloat64 x,const BTime& y);

        common::BString SecondString() const;
        common::BString MicroString() const;

        operator tfloat64() const;
};

//-------------------------------------------------------------------------------------------

inline tint BTime::Seconds() const
{
    return m_Seconds;
}

//-------------------------------------------------------------------------------------------

inline tint BTime::Micro() const
{
    return m_MicroSeconds;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif

