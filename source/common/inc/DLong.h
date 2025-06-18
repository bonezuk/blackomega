#ifndef __OMEGA_COMMON_DLONG_H
#define __OMEGA_COMMON_DLONG_H

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT DLong 
{
    public:
        DLong();
        DLong(tint x);
        DLong(tuint x);
        DLong(const DLong& x);
        DLong(tuint x1,tuint x2);
        DLong(tfloat32 x);
        DLong(tfloat64 x);

        const DLong& operator = (tint x);
        const DLong& operator = (tuint x);
        const DLong& operator = (const DLong& x);
        const DLong& operator = (tfloat32 x);
        const DLong& operator = (tfloat64 x);

        DLong& operator += (tint x);
        DLong& operator += (tuint x);
        DLong& operator += (const DLong& x);
        DLong& operator += (tfloat32 x);
        DLong& operator += (tfloat64 x);

        DLong& operator -= (tint x);
        DLong& operator -= (tuint x);
        DLong& operator -= (const DLong& x);
        DLong& operator -= (tfloat32 x);
        DLong& operator -= (tfloat64 x);

        DLong& operator <<= (tint x);
        DLong& operator <<= (tuint x);
        
        DLong& operator >>= (tint x);
        DLong& operator >>= (tuint x);
        
        friend COMMON_EXPORT bool operator > (tint x,const DLong& y);
        friend COMMON_EXPORT bool operator > (tuint x,const DLong& y);
        friend COMMON_EXPORT bool operator > (tfloat32 x,const DLong& y);
        friend COMMON_EXPORT bool operator > (tfloat64 x,const DLong& y);
        friend COMMON_EXPORT bool operator > (const DLong& x,tint y);
        friend COMMON_EXPORT bool operator > (const DLong& x,tuint y);
        friend COMMON_EXPORT bool operator > (const DLong& x,tfloat32 y);
        friend COMMON_EXPORT bool operator > (const DLong& x,tfloat64 y);
        friend COMMON_EXPORT bool operator > (const DLong& x,const DLong& y);

        friend COMMON_EXPORT bool operator >= (tint x,const DLong& y);
        friend COMMON_EXPORT bool operator >= (tuint x,const DLong& y);
        friend COMMON_EXPORT bool operator >= (tfloat32 x,const DLong& y);
        friend COMMON_EXPORT bool operator >= (tfloat64 x,const DLong& y);
        friend COMMON_EXPORT bool operator >= (const DLong& x,tint y);
        friend COMMON_EXPORT bool operator >= (const DLong& x,tuint y);
        friend COMMON_EXPORT bool operator >= (const DLong& x,tfloat32 y);
        friend COMMON_EXPORT bool operator >= (const DLong& x,tfloat64 y);
        friend COMMON_EXPORT bool operator >= (const DLong& x,const DLong& y);

        friend COMMON_EXPORT bool operator == (tint x,const DLong& y);
        friend COMMON_EXPORT bool operator == (tuint x,const DLong& y);
        friend COMMON_EXPORT bool operator == (tfloat32 x,const DLong& y);
        friend COMMON_EXPORT bool operator == (tfloat64 x,const DLong& y);
        friend COMMON_EXPORT bool operator == (const DLong& x,tint y);
        friend COMMON_EXPORT bool operator == (const DLong& x,tuint y);
        friend COMMON_EXPORT bool operator == (const DLong& x,tfloat32 y);
        friend COMMON_EXPORT bool operator == (const DLong& x,tfloat64 y);
        friend COMMON_EXPORT bool operator == (const DLong& x,const DLong& y);

        friend COMMON_EXPORT bool operator != (tint x,const DLong& y);
        friend COMMON_EXPORT bool operator != (tuint x,const DLong& y);
        friend COMMON_EXPORT bool operator != (tfloat32 x,const DLong& y);
        friend COMMON_EXPORT bool operator != (tfloat64 x,const DLong& y);
        friend COMMON_EXPORT bool operator != (const DLong& x,tint y);
        friend COMMON_EXPORT bool operator != (const DLong& x,tuint y);
        friend COMMON_EXPORT bool operator != (const DLong& x,tfloat32 y);
        friend COMMON_EXPORT bool operator != (const DLong& x,tfloat64 y);
        friend COMMON_EXPORT bool operator != (const DLong& x,const DLong& y);

        friend COMMON_EXPORT bool operator <= (tint x,const DLong& y);
        friend COMMON_EXPORT bool operator <= (tuint x,const DLong& y);
        friend COMMON_EXPORT bool operator <= (tfloat32 x,const DLong& y);
        friend COMMON_EXPORT bool operator <= (tfloat64 x,const DLong& y);
        friend COMMON_EXPORT bool operator <= (const DLong& x,tint y);
        friend COMMON_EXPORT bool operator <= (const DLong& x,tuint y);
        friend COMMON_EXPORT bool operator <= (const DLong& x,tfloat32 y);
        friend COMMON_EXPORT bool operator <= (const DLong& x,tfloat64 y);
        friend COMMON_EXPORT bool operator <= (const DLong& x,const DLong& y);

        friend COMMON_EXPORT bool operator < (tint x,const DLong& y);
        friend COMMON_EXPORT bool operator < (tuint x,const DLong& y);
        friend COMMON_EXPORT bool operator < (tfloat32 x,const DLong& y);
        friend COMMON_EXPORT bool operator < (tfloat64 x,const DLong& y);
        friend COMMON_EXPORT bool operator < (const DLong& x,tint y);
        friend COMMON_EXPORT bool operator < (const DLong& x,tuint y);
        friend COMMON_EXPORT bool operator < (const DLong& x,tfloat32 y);
        friend COMMON_EXPORT bool operator < (const DLong& x,tfloat64 y);
        friend COMMON_EXPORT bool operator < (const DLong& x,const DLong& y);

        tint Encode(tchar *mem,tint offset,tint len,tint aLen=-1) const;
        tint Decode(const tchar *mem,tint offset,tint len);

        tuint High() const;
        tuint Low() const;

        tint Length() const;

        static tint MemLength(tchar a);

        operator tfloat32 () const;
        operator tfloat64 () const;

    protected:
        tuint n[2];

        void Add(tuint x1,tuint x2);
        void Sub(tuint x1,tuint x2);

        tint Cmp(tint x) const;
        tint Cmp(tuint x) const;
        tint Cmp(tfloat32 x) const;
        tint Cmp(tfloat64 x) const;
        tint Cmp(const DLong& x) const;
};

COMMON_EXPORT DLong operator + (const DLong& x,tint y);
COMMON_EXPORT DLong operator + (const DLong& x,tuint y);
COMMON_EXPORT DLong operator + (const DLong& x,tfloat32 y);
COMMON_EXPORT DLong operator + (const DLong& x,tfloat64 y);
COMMON_EXPORT DLong operator + (tint x,const DLong& y);
COMMON_EXPORT DLong operator + (tuint x,const DLong& y);
COMMON_EXPORT DLong operator + (tfloat32 x,const DLong& y);
COMMON_EXPORT DLong operator + (tfloat64 x,const DLong& y);
COMMON_EXPORT DLong operator + (const DLong& x,const DLong& y);

COMMON_EXPORT DLong operator - (const DLong& x,tint y);
COMMON_EXPORT DLong operator - (const DLong& x,tuint y);
COMMON_EXPORT DLong operator - (const DLong& x,tfloat32 y);
COMMON_EXPORT DLong operator - (const DLong& x,tfloat64 y);
COMMON_EXPORT DLong operator - (tint x,const DLong& y);
COMMON_EXPORT DLong operator - (tuint x,const DLong& y);
COMMON_EXPORT DLong operator - (tfloat32 x,const DLong& y);
COMMON_EXPORT DLong operator - (tfloat64 x,const DLong& y);
COMMON_EXPORT DLong operator - (const DLong& x,const DLong& y);

COMMON_EXPORT DLong operator << (const DLong& in,tint x);
COMMON_EXPORT DLong operator << (const DLong& in,tuint x);

COMMON_EXPORT DLong operator >> (const DLong& in,tint x);
COMMON_EXPORT DLong operator >> (const DLong& in,tuint x);

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif

