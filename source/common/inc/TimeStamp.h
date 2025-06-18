//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_TIMESTAMP_H
#define __OMEGA_COMMON_TIMESTAMP_H
//-------------------------------------------------------------------------------------------

#include <math.h>
#include <time.h>

#include <QDateTime>

#include "common/inc/CommonTypes.h"
#include "common/inc/BString.h"
#include "common/inc/UString.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
// Input conversion for timestamp format.
// All numerical values is the time in secons relative to 0h on 1 Jan 1900.
// - tint32 / tuint32 : (fixed decimal point) high 16-bits integer part / low 16-bits decimal part
// - tint64 / tuint64 : (fixed decimal point) high 32-bits integer part / high 32-bits decimal part
// - tfloat32 / tfloat64 : (floating point)
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT TimeStamp
{
    public:
    
        static const tuint32 c_SecondsPerMinute = 60;
        static const tuint32 c_SecondsPerHour = c_SecondsPerMinute * 60;
        static const tuint32 c_SecondsPerDay = c_SecondsPerHour * 24;
        static const tuint32 c_SecondsPerYear = c_SecondsPerDay * 365;
        static const tuint32 c_SecondsPerLeapYear = c_SecondsPerDay * 366;
    
    public:
        TimeStamp();
        TimeStamp(tint32 x);
        TimeStamp(tuint32 x);
        TimeStamp(tint64 x);
        TimeStamp(tuint64 x);
        TimeStamp(tfloat32 x);
        TimeStamp(tfloat64 x);
        TimeStamp(const TimeStamp& x);
        
        const TimeStamp& operator = (tint32 x);
        const TimeStamp& operator = (tuint32 x);
        const TimeStamp& operator = (tint64 x);
        const TimeStamp& operator = (tuint64 x);
        const TimeStamp& operator = (tfloat32 x);
        const TimeStamp& operator = (tfloat64 x);
        const TimeStamp& operator = (const TimeStamp& x);
        
        bool isEmpty() const;
        bool isLeapYear() const;
        bool isLeapYear(tint x) const;
        
        static TimeStamp now();
        static TimeStamp reference();
        
        tint year() const;
        void year(tint x);
        tint month() const;
        void month(tint x);
        tint day() const;
        void day(tint x);
        tint hour() const;
        void hour(tint x);
        tint minute() const;
        void minute(tint x);
        tint second() const;
        void second(tint x);
        tint millisecond() const;
        void millisecond(tint x);
        tint microsecond() const;
        void microsecond(tint x);
        tint nanosecond() const;
        void nanosecond(tint x);
        tuint32 secondsTotal() const;
        void secondsTotal(tint x);
        void secondsTotal(tuint32 x);
        tuint64 nano64() const;
        void nano64(tuint32 hi,tuint32 lo);
        void nano64(tuint64 x);
        
        void set(const struct tm *t);
#if defined(OMEGA_WIN32)
        void set(__time64_t *t);
#elif defined(OMEGA_MACOSX) || defined(OMEGA_LINUX) || defined(OMEGA_IOS)
        void set(const struct timespec *t);
#endif
#if defined(OMEGA_LINUX) || defined(OMEGA_ANDROID)
        void set(time_t *t);
#endif

        void set(tint32 x) volatile;
        void set(tuint32 x) volatile;
        void set(tint64 x) volatile;
        void set(tuint64 x) volatile;
        void set(tfloat32 x) volatile;
        void set(tfloat64 x) volatile;
        void set(const TimeStamp& x) volatile;

        void get(struct tm *t) const;
        
        tint dayOfWeek() const;
        tint dayOfYear() const;
                
        operator tint32 () const;
        operator tuint32 () const;
        operator tint64 () const;
        operator tuint64 () const;
        operator tfloat32 () const;
        operator tfloat64 () const;
        
        const TimeStamp& operator += (tint32 x);
        const TimeStamp& operator += (tuint32 x);
        const TimeStamp& operator += (tint64 x);
        const TimeStamp& operator += (tuint64 x);
        const TimeStamp& operator += (tfloat32 x);
        const TimeStamp& operator += (tfloat64 x);
        const TimeStamp& operator += (const TimeStamp& x);

        const TimeStamp& operator -= (tint32 x);
        const TimeStamp& operator -= (tuint32 x);
        const TimeStamp& operator -= (tint64 x);
        const TimeStamp& operator -= (tuint64 x);
        const TimeStamp& operator -= (tfloat32 x);
        const TimeStamp& operator -= (tfloat64 x);
        const TimeStamp& operator -= (const TimeStamp& x);

        const TimeStamp& operator *= (tint32 x);
        const TimeStamp& operator *= (tuint32 x);
        const TimeStamp& operator *= (tint64 x);
        const TimeStamp& operator *= (tuint64 x);
        const TimeStamp& operator *= (tfloat32 x);
        const TimeStamp& operator *= (tfloat64 x);
        const TimeStamp& operator *= (const TimeStamp& x);

        const TimeStamp& operator /= (tint32 x);
        const TimeStamp& operator /= (tuint32 x);
        const TimeStamp& operator /= (tint64 x);
        const TimeStamp& operator /= (tuint64 x);
        const TimeStamp& operator /= (tfloat32 x);
        const TimeStamp& operator /= (tfloat64 x);
        const TimeStamp& operator /= (const TimeStamp& x);
        
        const TimeStamp& operator %= (tint32 x);
        const TimeStamp& operator %= (tuint32 x);
        const TimeStamp& operator %= (tint64 x);
        const TimeStamp& operator %= (tuint64 x);
        const TimeStamp& operator %= (tfloat32 x);
        const TimeStamp& operator %= (tfloat64 x);
        const TimeStamp& operator %= (const TimeStamp& x);

        friend COMMON_EXPORT bool operator <  (const TimeStamp& a,tint32 b);
        friend COMMON_EXPORT bool operator <  (const TimeStamp& a,tuint32 b);
        friend COMMON_EXPORT bool operator <  (const TimeStamp& a,tint64 b);
        friend COMMON_EXPORT bool operator <  (const TimeStamp& a,tuint64 b);
        friend COMMON_EXPORT bool operator <  (const TimeStamp& a,tfloat32 b);
        friend COMMON_EXPORT bool operator <  (const TimeStamp& a,tfloat64 b);
        friend COMMON_EXPORT bool operator <  (tint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <  (tuint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <  (tint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <  (tuint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <  (tfloat32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <  (tfloat64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <  (const TimeStamp& a,const TimeStamp& b);

        friend COMMON_EXPORT bool operator <= (const TimeStamp& a,tint32 b);
        friend COMMON_EXPORT bool operator <= (const TimeStamp& a,tuint32 b);
        friend COMMON_EXPORT bool operator <= (const TimeStamp& a,tint64 b);
        friend COMMON_EXPORT bool operator <= (const TimeStamp& a,tuint64 b);
        friend COMMON_EXPORT bool operator <= (const TimeStamp& a,tfloat32 b);
        friend COMMON_EXPORT bool operator <= (const TimeStamp& a,tfloat64 b);
        friend COMMON_EXPORT bool operator <= (tint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <= (tuint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <= (tint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <= (tuint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <= (tfloat32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <= (tfloat64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator <= (const TimeStamp& a,const TimeStamp& b);

        friend COMMON_EXPORT bool operator >  (const TimeStamp& a,tint32 b);
        friend COMMON_EXPORT bool operator >  (const TimeStamp& a,tuint32 b);
        friend COMMON_EXPORT bool operator >  (const TimeStamp& a,tint64 b);
        friend COMMON_EXPORT bool operator >  (const TimeStamp& a,tuint64 b);
        friend COMMON_EXPORT bool operator >  (const TimeStamp& a,tfloat32 b);
        friend COMMON_EXPORT bool operator >  (const TimeStamp& a,tfloat64 b);
        friend COMMON_EXPORT bool operator >  (tint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >  (tuint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >  (tint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >  (tuint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >  (tfloat32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >  (tfloat64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >  (const TimeStamp& a,const TimeStamp& b);

        friend COMMON_EXPORT bool operator >= (const TimeStamp& a,tint32 b);
        friend COMMON_EXPORT bool operator >= (const TimeStamp& a,tuint32 b);
        friend COMMON_EXPORT bool operator >= (const TimeStamp& a,tint64 b);
        friend COMMON_EXPORT bool operator >= (const TimeStamp& a,tuint64 b);
        friend COMMON_EXPORT bool operator >= (const TimeStamp& a,tfloat32 b);
        friend COMMON_EXPORT bool operator >= (const TimeStamp& a,tfloat64 b);
        friend COMMON_EXPORT bool operator >= (tint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >= (tuint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >= (tint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >= (tuint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >= (tfloat32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >= (tfloat64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator >= (const TimeStamp& a,const TimeStamp& b);

        friend COMMON_EXPORT bool operator != (const TimeStamp& a,tint32 b);
        friend COMMON_EXPORT bool operator != (const TimeStamp& a,tuint32 b);
        friend COMMON_EXPORT bool operator != (const TimeStamp& a,tint64 b);
        friend COMMON_EXPORT bool operator != (const TimeStamp& a,tuint64 b);
        friend COMMON_EXPORT bool operator != (const TimeStamp& a,tfloat32 b);
        friend COMMON_EXPORT bool operator != (const TimeStamp& a,tfloat64 b);
        friend COMMON_EXPORT bool operator != (tint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator != (tuint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator != (tint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator != (tuint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator != (tfloat32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator != (tfloat64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator != (const TimeStamp& a,const TimeStamp& b);

        friend COMMON_EXPORT bool operator == (const TimeStamp& a,tint32 b);
        friend COMMON_EXPORT bool operator == (const TimeStamp& a,tuint32 b);
        friend COMMON_EXPORT bool operator == (const TimeStamp& a,tint64 b);
        friend COMMON_EXPORT bool operator == (const TimeStamp& a,tuint64 b);
        friend COMMON_EXPORT bool operator == (const TimeStamp& a,tfloat32 b);
        friend COMMON_EXPORT bool operator == (const TimeStamp& a,tfloat64 b);
        friend COMMON_EXPORT bool operator == (tint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator == (tuint32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator == (tint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator == (tuint64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator == (tfloat32 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator == (tfloat64 a,const TimeStamp& b);
        friend COMMON_EXPORT bool operator == (const TimeStamp& a,const TimeStamp& b);

        
    protected:
    
        tuint64 m_time;
    
        void decode(tint32 x);
        void decode(tuint32 x);
        void decode(tint64 x);
        void decode(tuint64 x);
        void decode(tfloat32 x);
        void decode(tfloat64 x);
        
        void encode(tint32& x) const;
        void encode(tuint32& x) const;
        void encode(tint64& x) const;
        void encode(tuint64& x) const;
        void encode(tfloat32& x) const;
        void encode(tfloat64& x) const;
        
        tint compare(tint32 x) const;
        tint compare(tuint32 x) const;
        tint compare(tint64 x) const;
        tint compare(tuint64 x) const;
        tint compare(tfloat32 x) const;
        tint compare(tfloat64 x) const;
        tint compare(const TimeStamp& x) const;
        
        tuint32 secondsAndYear(tuint32& t,tint& year) const;
        tuint32 secondsAndMonth(tuint32& t,tint& month,bool leap) const;
        tuint32 secondsAndDay(tuint32& t,tint& day) const;
        tuint32 secondsAndHour(tuint32& t,tint& hour) const;
        tuint32 secondsAndMinute(tuint32& t,tint& min) const;
};

COMMON_EXPORT TimeStamp operator + (const TimeStamp& a,tint32 b);
COMMON_EXPORT TimeStamp operator + (const TimeStamp& a,tuint32 b);
COMMON_EXPORT TimeStamp operator + (const TimeStamp& a,tint64 b);
COMMON_EXPORT TimeStamp operator + (const TimeStamp& a,tuint64 b);
COMMON_EXPORT TimeStamp operator + (const TimeStamp& a,tfloat32 b);
COMMON_EXPORT TimeStamp operator + (const TimeStamp& a,tfloat64 b);
COMMON_EXPORT TimeStamp operator + (tint32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator + (tuint32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator + (tint64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator + (tuint64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator + (tfloat32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator + (tfloat64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator + (const TimeStamp& a,const TimeStamp& b);

COMMON_EXPORT TimeStamp operator - (const TimeStamp& a,tint32 b);
COMMON_EXPORT TimeStamp operator - (const TimeStamp& a,tuint32 b);
COMMON_EXPORT TimeStamp operator - (const TimeStamp& a,tint64 b);
COMMON_EXPORT TimeStamp operator - (const TimeStamp& a,tuint64 b);
COMMON_EXPORT TimeStamp operator - (const TimeStamp& a,tfloat32 b);
COMMON_EXPORT TimeStamp operator - (const TimeStamp& a,tfloat64 b);
COMMON_EXPORT TimeStamp operator - (tint32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator - (tuint32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator - (tint64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator - (tuint64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator - (tfloat32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator - (tfloat64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator - (const TimeStamp& a,const TimeStamp& b);

COMMON_EXPORT TimeStamp operator * (const TimeStamp& a,tint32 b);
COMMON_EXPORT TimeStamp operator * (const TimeStamp& a,tuint32 b);
COMMON_EXPORT TimeStamp operator * (const TimeStamp& a,tint64 b);
COMMON_EXPORT TimeStamp operator * (const TimeStamp& a,tuint64 b);
COMMON_EXPORT TimeStamp operator * (const TimeStamp& a,tfloat32 b);
COMMON_EXPORT TimeStamp operator * (const TimeStamp& a,tfloat64 b);
COMMON_EXPORT TimeStamp operator * (tint32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator * (tuint32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator * (tint64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator * (tuint64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator * (tfloat32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator * (tfloat64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator * (const TimeStamp& a,const TimeStamp& b);

COMMON_EXPORT TimeStamp operator / (const TimeStamp& a,tint32 b);
COMMON_EXPORT TimeStamp operator / (const TimeStamp& a,tuint32 b);
COMMON_EXPORT TimeStamp operator / (const TimeStamp& a,tint64 b);
COMMON_EXPORT TimeStamp operator / (const TimeStamp& a,tuint64 b);
COMMON_EXPORT TimeStamp operator / (const TimeStamp& a,tfloat32 b);
COMMON_EXPORT TimeStamp operator / (const TimeStamp& a,tfloat64 b);
COMMON_EXPORT TimeStamp operator / (tint32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator / (tuint32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator / (tint64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator / (tuint64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator / (tfloat32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator / (tfloat64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator / (const TimeStamp& a,const TimeStamp& b);

COMMON_EXPORT TimeStamp operator % (const TimeStamp& a,tint32 b);
COMMON_EXPORT TimeStamp operator % (const TimeStamp& a,tuint32 b);
COMMON_EXPORT TimeStamp operator % (const TimeStamp& a,tint64 b);
COMMON_EXPORT TimeStamp operator % (const TimeStamp& a,tuint64 b);
COMMON_EXPORT TimeStamp operator % (const TimeStamp& a,tfloat32 b);
COMMON_EXPORT TimeStamp operator % (const TimeStamp& a,tfloat64 b);
COMMON_EXPORT TimeStamp operator % (tint32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator % (tuint32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator % (tint64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator % (tuint64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator % (tfloat32 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator % (tfloat64 a,const TimeStamp& b);
COMMON_EXPORT TimeStamp operator % (const TimeStamp& a,const TimeStamp& b);

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

