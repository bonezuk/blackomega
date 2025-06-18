#ifndef __OMEGA_COMMON_TTIME_H
#define __OMEGA_COMMON_TTIME_H

#include <time.h>
#include "common/inc/BString.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT TTime
{
    public:
        TTime();
        TTime(const time_t *t);
        TTime(const struct tm *t);
        TTime(const TTime& t);
        TTime(tint t);
        TTime(const BString& str);
        TTime(const tchar *str);
        
        static TTime Now();
        
        bool IsEmpty() const;

        tint Year() const;
        tint Day() const;
        tint Seconds() const;
        
        void GMTime(struct tm *t) const;
        
        void Set(tint y,tint d,tint s);
        
        TTime& operator = (const time_t *t);
        TTime& operator = (const struct tm *t);
        TTime& operator = (const TTime& t);
        TTime& operator = (tint t);
        TTime& operator = (const BString& str);
        TTime& operator = (const tchar *str);
        
        TTime& operator += (const TTime& t);
        TTime& operator += (tint t);

        TTime& operator -= (const TTime& t);
        TTime& operator -= (tint t);
        
        friend COMMON_EXPORT bool operator < (const TTime& a,const time_t *b);
        friend COMMON_EXPORT bool operator < (const TTime& a,const struct tm *b);
        friend COMMON_EXPORT bool operator < (const time_t *a,const TTime& b);
        friend COMMON_EXPORT bool operator < (const struct tm *a,const TTime& b);
        friend COMMON_EXPORT bool operator < (const TTime& a,const TTime& b);
        
        friend COMMON_EXPORT bool operator <= (const TTime& a,const time_t *b);
        friend COMMON_EXPORT bool operator <= (const TTime& a,const struct tm *b);
        friend COMMON_EXPORT bool operator <= (const time_t *a,const TTime& b);
        friend COMMON_EXPORT bool operator <= (const struct tm *a,const TTime& b);
        friend COMMON_EXPORT bool operator <= (const TTime& a,const TTime& b);

        friend COMMON_EXPORT bool operator >= (const TTime& a,const time_t *b);
        friend COMMON_EXPORT bool operator >= (const TTime& a,const struct tm *b);
        friend COMMON_EXPORT bool operator >= (const time_t *a,const TTime& b);
        friend COMMON_EXPORT bool operator >= (const struct tm *a,const TTime& b);
        friend COMMON_EXPORT bool operator >= (const TTime& a,const TTime& b);

        friend COMMON_EXPORT bool operator > (const TTime& a,const time_t *b);
        friend COMMON_EXPORT bool operator > (const TTime& a,const struct tm *b);
        friend COMMON_EXPORT bool operator > (const time_t *a,const TTime& b);
        friend COMMON_EXPORT bool operator > (const struct tm *a,const TTime& b);
        friend COMMON_EXPORT bool operator > (const TTime& a,const TTime& b);

        friend COMMON_EXPORT bool operator == (const TTime& a,const time_t *b);
        friend COMMON_EXPORT bool operator == (const TTime& a,const struct tm *b);
        friend COMMON_EXPORT bool operator == (const time_t *a,const TTime& b);
        friend COMMON_EXPORT bool operator == (const struct tm *a,const TTime& b);
        friend COMMON_EXPORT bool operator == (const TTime& a,const TTime& b);

        friend COMMON_EXPORT bool operator != (const TTime& a,const time_t *b);
        friend COMMON_EXPORT bool operator != (const TTime& a,const struct tm *b);
        friend COMMON_EXPORT bool operator != (const time_t *a,const TTime& b);
        friend COMMON_EXPORT bool operator != (const struct tm *a,const TTime& b);
        friend COMMON_EXPORT bool operator != (const TTime& a,const TTime& b);

        bool IsLeapYear() const;
        bool IsLeapYear(tint year) const;

        static bool LeapYear(tint year);

        BString String();

        static int DayOfYear(const struct tm *gmt);

    protected:
    
        tint m_Year,m_yDay,m_Sec;
                
        tint DayOfWeek() const;
        tint MonthOfYear() const;
        tint DayOfMonth() const;

        tint Hour() const;
        tint Minute() const;
        
        void Increment(tint x);
        
        void Equal(const time_t *t);
        void Equal(const struct tm *t);
        void Equal(const TTime& t);
        void Equal(tint t);
        
        int Compare(const time_t *t) const;
        int Compare(const struct tm *t) const;
        int Compare(const TTime& b) const;

        void ToStr(const BString& str);
};

COMMON_EXPORT TTime operator + (const TTime& a,int b);
COMMON_EXPORT TTime operator + (tint a,const TTime& b);
COMMON_EXPORT TTime operator + (const TTime& a,const TTime& b);

COMMON_EXPORT TTime operator - (const TTime& a,tint b);
COMMON_EXPORT TTime operator - (tint a,const TTime& b);
COMMON_EXPORT TTime operator - (const TTime& a,const TTime& b);

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif

