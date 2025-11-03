#include <math.h>
#include "common/inc/DLong.h"

#if defined(OMEGA_MSVC)
#pragma warning(disable:4244)
#pragma warning(disable:4514)
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

DLong::DLong() 
{
    n[0]=0;
    n[1]=0;
}

//-------------------------------------------------------------------------------------------

DLong::DLong(tint x) 
{
    n[0]=static_cast<tuint>(x);
    n[1]=0;
}

//-------------------------------------------------------------------------------------------

DLong::DLong(tuint x) 
{
    n[0]=x;
    n[1]=0;
}

//-------------------------------------------------------------------------------------------

DLong::DLong(const DLong& x) 
{
    n[0]=x.n[0];
    n[1]=x.n[1];
}

//-------------------------------------------------------------------------------------------

DLong::DLong(tuint x1,tuint x2) 
{
    n[0]=x1;
    n[1]=x2;
}

//-------------------------------------------------------------------------------------------

DLong::DLong(tfloat32 x)
{
    n[0] = static_cast<tuint>(::fmod(static_cast<tfloat64>(x),4294967296.0));
    n[1] = static_cast<tuint>(static_cast<tfloat64>(x)/4294967296.0);
}

//-------------------------------------------------------------------------------------------

DLong::DLong(tfloat64 x) 
{
    n[0] = static_cast<tuint>(::fmod(x,4294967296.0));
    n[1] = static_cast<tuint>(x/4294967296.0);
}

//-------------------------------------------------------------------------------------------

void DLong::Add(tuint x1,tuint x2) 
{
    tuint c=n[0]+x1;

    if(c<n[0])
    {
        n[1]++;
    }
    n[0]=c;
    n[1]+=x2;
}

//-------------------------------------------------------------------------------------------

void DLong::Sub(tuint x1,tuint x2) 
{
    if(n[0]<x1)
    {
        x2++;
    }
    n[0]-=x1;
    n[1]-=x2;
}

//-------------------------------------------------------------------------------------------

const DLong& DLong::operator = (tint x) 
{
    n[0]=static_cast<tuint>(x);
    n[1]=0;
    return *this;
}

//-------------------------------------------------------------------------------------------

const DLong& DLong::operator = (tuint x) 
{
    n[0]=x;
    n[1]=0;
    return *this;
}

//-------------------------------------------------------------------------------------------

const DLong& DLong::operator = (const DLong& x) 
{
    if(&x!=this)
    {
        n[0]=x.n[0];
        n[1]=x.n[1];
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

const DLong& DLong::operator = (tfloat32 x)
{
    DLong a(x);
    n[0] = a.n[0];
    n[1] = a.n[1];
    return *this;
}

//-------------------------------------------------------------------------------------------

const DLong& DLong::operator = (tfloat64 x) 
{
    DLong a(x);
    n[0]=a.n[0];
    n[1]=a.n[1];
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator += (tint x)
{
    Add(static_cast<tuint>(x),0);
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator += (tuint x)
{
    Add(x,0);
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator += (const DLong& x)
{
    Add(x.n[0],x.n[1]);
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator += (tfloat32 x)
{
    DLong a(x);
    
    Add(a.n[0],a.n[1]);
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator += (tfloat64 x)
{
    DLong a(x);

    Add(a.n[0],a.n[1]);
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator -= (tint x)
{
    Sub(static_cast<tuint>(x),0);
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator -= (tuint x)
{
    Sub(x,0);
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator -= (const DLong& x)
{
    Sub(x.n[0],x.n[1]);
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator -= (tfloat32 x)
{
    DLong a(x);
    
    Sub(a.n[0],a.n[1]);
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator -= (tfloat64 x)
{
    DLong a(x);

    Sub(a.n[0],a.n[1]);
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong operator + (const DLong& x,tint y)
{
    DLong z(x);
    z += y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator + (const DLong& x,tuint y)
{
    DLong z(x);
    z += y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator + (const DLong& x,tfloat32 y)
{
    DLong z(x);
    z += y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator + (const DLong& x,tfloat64 y)
{
    DLong z(x);
    z += y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator + (tint x,const DLong& y)
{
    DLong z(x);
    z += y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator + (tuint x,const DLong& y)
{
    DLong z(x);
    z += y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator + (tfloat32 x,const DLong& y)
{
    DLong z(x);
    z += y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator + (tfloat64 x,const DLong& y)
{
    DLong z(x);
    z += y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator + (const DLong& x,const DLong& y)
{
    DLong z(x);
    z += y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator - (const DLong& x,tint y)
{
    DLong z(x);
    z -= y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator - (const DLong& x,tuint y)
{
    DLong z(x);
    z -= y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator - (const DLong& x,tfloat32 y)
{
    DLong z(x);
    z -= y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator - (const DLong& x,tfloat64 y)
{
    DLong z(x);
    z -= y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator - (tint x,const DLong& y)
{
    DLong z(x);
    z -= y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator - (tuint x,const DLong& y)
{
    DLong z(x);
    z -= y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator - (tfloat32 x,const DLong& y)
{
    DLong z(x);
    z -= y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator - (tfloat64 x,const DLong& y)
{
    DLong z(x);
    z -= y;
    return z;
}

//-------------------------------------------------------------------------------------------

DLong operator - (const DLong& x,const DLong& y)
{
    DLong z(x);
    z -= y;
    return z;
}

//-------------------------------------------------------------------------------------------

const tuint g_DLong_LShiftMask[32][4]={
    {0xffffffff,0x00000000,0,0},
    {0xfffffffe,0x00000001,1,31},
    {0xfffffffc,0x00000003,2,30},
    {0xfffffff8,0x00000007,3,29},
    {0xfffffff0,0x0000000f,4,28},
    {0xffffffe0,0x0000001f,5,27},
    {0xffffffc0,0x0000003f,6,26},
    {0xffffff80,0x0000007f,7,25},
    {0xffffff00,0x000000ff,8,24},
    {0xfffffe00,0x000001ff,9,23},
    {0xfffffc00,0x000003ff,10,22},
    {0xfffff800,0x000007ff,11,21},
    {0xfffff000,0x00000fff,12,20},
    {0xffffe000,0x00001fff,13,19},
    {0xffffc000,0x00003fff,14,18},
    {0xffff8000,0x00007fff,15,17},
    {0xffff0000,0x0000ffff,16,16},
    {0xfffe0000,0x0001ffff,17,15},
    {0xfffc0000,0x0003ffff,18,14},
    {0xfff80000,0x0007ffff,19,13},
    {0xfff00000,0x000fffff,20,12},
    {0xffe00000,0x001fffff,21,11},
    {0xffc00000,0x003fffff,22,10},
    {0xff800000,0x007fffff,23,9},
    {0xff000000,0x00ffffff,24,8},
    {0xfe000000,0x01ffffff,25,7},
    {0xfc000000,0x03ffffff,26,6},
    {0xf8000000,0x07ffffff,27,5},
    {0xf0000000,0x0fffffff,28,4},
    {0xe0000000,0x1fffffff,29,3},
    {0xc0000000,0x3fffffff,30,2},
    {0x80000000,0x7fffffff,31,1}
};

//-------------------------------------------------------------------------------------------

DLong& DLong::operator <<=(tint x) 
{
    if(x>0 && x<32) 
    {
        tuint r;
        const tuint *e=&g_DLong_LShiftMask[x][0];

        r = (n[0] >> e[3]) & e[1];
        n[0] = (n[0] << e[2]) & e[0];
        n[1] = ((n[1] << e[2]) & e[0]) | r;
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator <<=(tuint x) 
{
    if(x>0 && x<32) 
    {
        tuint r;
        const tuint *e=&g_DLong_LShiftMask[x][0];

        r = (n[0] >> e[3]) & e[1];
        n[0] = (n[0] << e[2]) & e[0];
        n[1] = ((n[1] << e[2]) & e[0]) | r;
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator >>=(tint x) 
{
    if(x>0 && x<32) 
    {
        tuint r;
        const tuint *e=&g_DLong_LShiftMask[(32-x)&0x1f][0];

        r = (n[1] << e[2]) & e[0];
        n[1] = (n[1] >> e[3]) & e[1];
        n[0] = ((n[0] >> e[3]) & e[1]) | r;
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

DLong& DLong::operator >>=(tuint x) 
{
    if(x>0 && x<32) 
    {
        tuint r;
        const tuint *e=&g_DLong_LShiftMask[(32-x)&0x1f][0];

        r = (n[1] << e[2]) & e[0];
        n[1] = (n[1] >> e[3]) & e[1];
        n[0] = ((n[0] >> e[3]) & e[1]) | r;
    }
    return *this;
}

//-------------------------------------------------------------------------------------------
// this <  x = -1
// this == x =  0
// this >  x =  1
//-------------------------------------------------------------------------------------------

tint DLong::Cmp(tint x) const
{
    DLong y(x);
    return Cmp(y);
}

//-------------------------------------------------------------------------------------------

tint DLong::Cmp(tuint x) const
{
    DLong y(x);
    return Cmp(y);
}

//-------------------------------------------------------------------------------------------

tint DLong::Cmp(tfloat32 x) const
{
    DLong y(x);
    return Cmp(y);
}

//-------------------------------------------------------------------------------------------

tint DLong::Cmp(tfloat64 x) const
{
    DLong y(x);
    return Cmp(y);
}

//-------------------------------------------------------------------------------------------

tint DLong::Cmp(const DLong& x) const
{
    if(n[1] < x.n[1]) 
    {
        return -1;
    }
    else if(n[1] > x.n[1]) 
    {
        return 1;
    }
    else if(n[0] < x.n[0]) 
    {
        return -1;
    }
    else if(n[0] > x.n[0]) 
    {
        return 1;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

bool operator > (tint x,const DLong& y)
{
    return (y.Cmp(x)<0);
}

//-------------------------------------------------------------------------------------------

bool operator > (tuint x,const DLong& y)
{
    return (y.Cmp(x)<0);
}

//-------------------------------------------------------------------------------------------

bool operator > (tfloat32 x,const DLong& y)
{
    return (y.Cmp(x)<0);
}

//-------------------------------------------------------------------------------------------

bool operator > (tfloat64 x,const DLong& y)
{
    return (y.Cmp(x)<0);
}

//-------------------------------------------------------------------------------------------

bool operator > (const DLong& x,tint y)
{
    return (x.Cmp(y)>0);
}

//-------------------------------------------------------------------------------------------

bool operator > (const DLong& x,tuint y)
{
    return (x.Cmp(y)>0);
}

//-------------------------------------------------------------------------------------------

bool operator > (const DLong& x,tfloat32 y)
{
    return (x.Cmp(y)>0);
}

//-------------------------------------------------------------------------------------------

bool operator > (const DLong& x,tfloat64 y)
{
    return (x.Cmp(y)>0);
}

//-------------------------------------------------------------------------------------------

bool operator > (const DLong& x,const DLong& y)
{
    return (x.Cmp(y)>0);
}

//-------------------------------------------------------------------------------------------

bool operator >= (tint x,const DLong& y)
{
    return (y.Cmp(x)<=0);
}

//-------------------------------------------------------------------------------------------

bool operator >= (tuint x,const DLong& y)
{
    return (y.Cmp(x)<=0);
}

//-------------------------------------------------------------------------------------------

bool operator >= (tfloat32 x,const DLong& y)
{
    return (y.Cmp(x)<=0);
}

//-------------------------------------------------------------------------------------------

bool operator >= (tfloat64 x,const DLong& y)
{
    return (y.Cmp(x)<=0);
}

//-------------------------------------------------------------------------------------------

bool operator >= (const DLong& x,tint y)
{
    return (x.Cmp(y)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator >= (const DLong& x,tuint y)
{
    return (x.Cmp(y)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator >= (const DLong& x,tfloat32 y)
{
    return (x.Cmp(y)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator >= (const DLong& x,tfloat64 y)
{
    return (x.Cmp(y)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator >= (const DLong& x,const DLong& y)
{
    return (x.Cmp(y)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator == (tint x,const DLong& y)
{
    return (y.Cmp(x)==0);
}

//-------------------------------------------------------------------------------------------

bool operator == (tuint x,const DLong& y)
{
    return (y.Cmp(x)==0);
}

//-------------------------------------------------------------------------------------------

bool operator == (tfloat32 x,const DLong& y)
{
    return (y.Cmp(x)==0);
}

//-------------------------------------------------------------------------------------------

bool operator == (tfloat64 x,const DLong& y)
{
    return (y.Cmp(x)==0);
}

//-------------------------------------------------------------------------------------------

bool operator == (const DLong& x,tint y)
{
    return (x.Cmp(y)==0);
}

//-------------------------------------------------------------------------------------------

bool operator == (const DLong& x,tuint y)
{
    return (x.Cmp(y)==0);
}

//-------------------------------------------------------------------------------------------

bool operator == (const DLong& x,tfloat32 y)
{
    return (x.Cmp(y)==0);
}

//-------------------------------------------------------------------------------------------

bool operator == (const DLong& x,tfloat64 y)
{
    return (x.Cmp(y)==0);
}

//-------------------------------------------------------------------------------------------

bool operator == (const DLong& x,const DLong& y)
{
    return (x.Cmp(y)==0);
}

//-------------------------------------------------------------------------------------------

bool operator != (tint x,const DLong& y)
{
    return (y.Cmp(x)!=0);
}

//-------------------------------------------------------------------------------------------

bool operator != (tuint x,const DLong& y)
{
    return (y.Cmp(x)!=0);
}

//-------------------------------------------------------------------------------------------

bool operator != (tfloat32 x,const DLong& y)
{
    return (y.Cmp(x)!=0);
}

//-------------------------------------------------------------------------------------------

bool operator != (tfloat64 x,const DLong& y)
{
    return (y.Cmp(x)!=0);
}

//-------------------------------------------------------------------------------------------

bool operator != (const DLong& x,tint y)
{
    return (x.Cmp(y)!=0);
}

//-------------------------------------------------------------------------------------------

bool operator != (const DLong& x,tuint y)
{
    return (x.Cmp(y)!=0);
}

//-------------------------------------------------------------------------------------------

bool operator != (const DLong& x,tfloat32 y)
{
    return (x.Cmp(y)!=0);
}

//-------------------------------------------------------------------------------------------

bool operator != (const DLong& x,tfloat64 y)
{
    return (x.Cmp(y)!=0);
}

//-------------------------------------------------------------------------------------------

bool operator != (const DLong& x,const DLong& y)
{
    return (x.Cmp(y)!=0);
}

//-------------------------------------------------------------------------------------------

bool operator <= (tint x,const DLong& y)
{
    return (y.Cmp(x)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator <= (tuint x,const DLong& y)
{
    return (y.Cmp(x)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator <= (tfloat32 x,const DLong& y)
{
    return (y.Cmp(x)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator <= (tfloat64 x,const DLong& y)
{
    return (y.Cmp(x)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator <= (const DLong& x,tint y)
{
    return (x.Cmp(y)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator <= (const DLong& x,tuint y)
{
    return (x.Cmp(y)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator <= (const DLong& x,tfloat32 y)
{
    return (x.Cmp(y)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator <= (const DLong& x,tfloat64 y)
{
    return (x.Cmp(y)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator <= (const DLong& x,const DLong& y)
{
    return (x.Cmp(y)>=0);
}

//-------------------------------------------------------------------------------------------

bool operator < (tint x,const DLong& y)
{
    return (y.Cmp(x)>0);
}

//-------------------------------------------------------------------------------------------

bool operator < (tuint x,const DLong& y)
{
    return (y.Cmp(x)>0);
}

//-------------------------------------------------------------------------------------------

bool operator < (tfloat32 x,const DLong& y)
{
    return (y.Cmp(x)>0);
}

//-------------------------------------------------------------------------------------------

bool operator < (tfloat64 x,const DLong& y)
{
    return (y.Cmp(x)>0);
}

//-------------------------------------------------------------------------------------------

bool operator < (const DLong& x,tint y)
{
    return (x.Cmp(y)<0);
}

//-------------------------------------------------------------------------------------------

bool operator < (const DLong& x,tuint y)
{
    return (x.Cmp(y)<0);
}

//-------------------------------------------------------------------------------------------

bool operator < (const DLong& x,tfloat32 y)
{
    return (x.Cmp(y)<0);
}

//-------------------------------------------------------------------------------------------

bool operator < (const DLong& x,tfloat64 y)
{
    return (x.Cmp(y)<0);
}

//-------------------------------------------------------------------------------------------

bool operator < (const DLong& x,const DLong& y)
{
    return (x.Cmp(y)<0);
}

//-------------------------------------------------------------------------------------------

tint DLong::Encode(tchar *mem,tint offset,tint len,tint aLen)  const
{
    tint i,j;
    tuchar v[8];

    for(j=4,i=1;i>=0;i--,j-=4) 
    {
        v[j+3]=static_cast<tuchar>((n[i]>>24) & 0x000000ff);
        v[j+2]=static_cast<tuchar>((n[i]>>16) & 0x000000ff);
        v[j+1]=static_cast<tuchar>((n[i]>>8) & 0x000000ff);
        v[j+0]=static_cast<tuchar>(n[i] & 0x000000ff);
    }

    for(i=7;i>=0 && v[i]==0;i--) ;

    if(aLen==-1) 
    {
        mem[offset++] = static_cast<tchar>(i & 0x7f);
    }
    else 
    {
        aLen-=2;
        if(i>aLen) 
        {
            i=aLen;
            mem[offset++] = static_cast<tchar>(static_cast<tchar>(0x00000080 | ((static_cast<tuint>(i) << 4) & 0x00000070) | (static_cast<tuint>(v[i+1]) & 0x0000000f)));
        }
        else 
        {
            i=aLen;
            mem[offset++] = static_cast<tchar>(i & 0x7f);
        }
    }

    if((offset+i+2)>=len)
    {
        return -1;
    }

    while(i>=0)
    {
        mem[offset++] = static_cast<tchar>(v[i--]);
    }
    return offset;
}

//-------------------------------------------------------------------------------------------

tint DLong::Decode(const tchar *mem,tint offset,tint len) 
{
    tint i;
    tuchar v[8]={0,0,0,0,0,0,0,0};

    if(offset>=len)
    {
        return -1;
    }

    if(mem[offset] & 0x80) 
    {
        i = static_cast<tint>((static_cast<tuint>(static_cast<tint>(mem[offset])) >> 4) & 0x00000007);
        i = (i==7) ? -1 : i;
        v[i+1] = static_cast<tuchar>(mem[offset++] & 0x0f);
    }
    else 
    {
        i=static_cast<tint>(mem[offset++]);
    }

    if(i>=8)
    {
        i=-1;
    }

    if((offset+i)>=len)
    {
        return -1;
    }

    while(i>=0)
    {
        v[i--]=static_cast<tuchar>(mem[offset++]);
    }

    n[0]=(static_cast<tuint>(v[3])<<24) | (static_cast<tuint>(v[2])<<16) | (static_cast<tuint>(v[1])<<8) | (static_cast<tuint>(v[0]));
    n[1]=(static_cast<tuint>(v[7])<<24) | (static_cast<tuint>(v[6])<<16) | (static_cast<tuint>(v[5])<<8) | (static_cast<tuint>(v[4]));

    return offset;
}

//-------------------------------------------------------------------------------------------

tuint DLong::High() const 
{
    return n[1];
}

//-------------------------------------------------------------------------------------------

tuint DLong::Low() const
{
    return n[0];
}

//-------------------------------------------------------------------------------------------

tint DLong::Length() const
{
    tint i,j;
    tuchar v[8];

    for(j=4,i=1;i>=0;i--,j-=4) 
    {
        v[j+3]=static_cast<tuchar>((n[i]>>24) & 0x000000ff);
        v[j+2]=static_cast<tuchar>((n[i]>>16) & 0x000000ff);
        v[j+1]=static_cast<tuchar>((n[i]>>8) & 0x000000ff);
        v[j+0]=static_cast<tuchar>(n[i] & 0x000000ff);
    }

    for(i=7;i>=0 && v[i]==0;i--) ;
    
    return i+2;
}

//-------------------------------------------------------------------------------------------

tint DLong::MemLength(tchar a) 
{
    int i;

    if(a & 0x80) 
    {
        i=static_cast<tint>(static_cast<tuint>(static_cast<tint>(a)) >> 4) & 0x00000007;
        i=(i==7) ? -1 : i;
    }
    else 
    {
        i=static_cast<tint>(a);
    }
    if(i>=8)
    {
        return -1;
    }
    return i+2;
}

//-------------------------------------------------------------------------------------------

DLong::operator tfloat32() const
{
    tfloat32 x;
    
    x  = static_cast<tfloat32>(n[1] * 4294967296.0f);
    x += static_cast<tfloat32>(n[0]);
    return x;
}

//-------------------------------------------------------------------------------------------

DLong::operator tfloat64() const
{
    tfloat64 x;

    x  = static_cast<tfloat64>(n[1]) * 4294967296.0;
    x += static_cast<tfloat64>(n[0]);
    return x;
}

//-------------------------------------------------------------------------------------------

DLong operator << (const DLong& in,tint x)
{
    DLong out(in);
    out <<= x;
    return out;
}

//-------------------------------------------------------------------------------------------

DLong operator << (const DLong& in,tuint x)
{
    DLong out(in);
    out <<= x;
    return out;
}

//-------------------------------------------------------------------------------------------

DLong operator >> (const DLong& in,tint x)
{
    DLong out(in);
    out >>= x;
    return out;
}
 
//-------------------------------------------------------------------------------------------

DLong operator >> (const DLong& in,tuint x)
{
    DLong out(in);
    out >>= x;
    return out;
}
 
//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
