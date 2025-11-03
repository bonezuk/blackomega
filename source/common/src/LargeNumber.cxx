#include "common/inc/LargeNumber.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
// LargeNumberService
//-------------------------------------------------------------------------------------------

LargeNumberService *LargeNumberService::m_instance = 0;

//-------------------------------------------------------------------------------------------

LargeNumberService::LargeNumberService() : m_mutex(),
    m_largeArr(0),
    m_cc(0),
    m_ansA(0),
    m_ansB(0),
    m_ansC(0)
{
    m_largeArr = new tuint32[2 * (MAX_NN_DIGITS + 1)];
    m_cc = new tuint32[2 * (MAX_NN_DIGITS + 1)];
    m_ansA = new tuint32[2 * (MAX_NN_DIGITS + 1)];
    m_ansB = new tuint32[2 * (MAX_NN_DIGITS + 1)];
    m_ansC = new tuint32[2 * (MAX_NN_DIGITS + 1)];
    clear();
}

//-------------------------------------------------------------------------------------------

LargeNumberService::~LargeNumberService()
{
    delete [] m_largeArr;
    delete [] m_cc;
    delete [] m_ansA;
    delete [] m_ansB;
    delete [] m_ansC;
    m_instance = 0;
}

//-------------------------------------------------------------------------------------------

LargeNumberService& LargeNumberService::instance()
{
    if(m_instance==0)
    {
        m_instance = new LargeNumberService;
    }
    return *m_instance;
}

//-------------------------------------------------------------------------------------------

void LargeNumberService::clear()
{
    ::memset(m_largeArr,0,2 * (MAX_NN_DIGITS + 1) * sizeof(tuint32));
    ::memset(m_cc,0,2 * (MAX_NN_DIGITS + 1) * sizeof(tuint32));
    ::memset(m_ansA,0,2 * (MAX_NN_DIGITS + 1) * sizeof(tuint32));
    ::memset(m_ansB,0,2 * (MAX_NN_DIGITS + 1) * sizeof(tuint32));
    ::memset(m_ansC,0,2 * (MAX_NN_DIGITS + 1) * sizeof(tuint32));
}

//-------------------------------------------------------------------------------------------
// LargeNumber
//-------------------------------------------------------------------------------------------

LargeNumber::LargeNumber() : m_sign(true),
    m_n(0),
    m_size(0)
{}

//-------------------------------------------------------------------------------------------

LargeNumber::LargeNumber(const LargeNumber& rhs) : m_sign(true),
    m_n(0),
    m_size(0)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

LargeNumber::LargeNumber(tint a) : m_sign(true),
    m_n(0),
    m_size(0)
{
    if(a<0)
    {
        assignDigit(static_cast<tuint32>(-a),1);
        m_sign = false;
    }
    else
    {
        assignDigit(static_cast<tuint32>(a),1);
    }
}

//-------------------------------------------------------------------------------------------

LargeNumber::LargeNumber(tuint64 a) : m_sign(true),
    m_n(0),
    m_size(2)
{
    m_n = new tuint32 [2];
    m_n[0] = static_cast<tuint32>(a & 0x00000000ffffffffULL);
    m_n[1] = static_cast<tuint32>((a >> 32) & 0x00000000ffffffffULL);
}

//-------------------------------------------------------------------------------------------

LargeNumber::~LargeNumber()
{
    if(m_n!=0)
    {
        ::memset(m_n,0,m_size * sizeof(tuint32));
        delete [] m_n;
        m_n = 0;
    }
}

//-------------------------------------------------------------------------------------------

void LargeNumber::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "LargeNumber::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

const LargeNumber& LargeNumber::operator = (const LargeNumber& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void LargeNumber::copy(const LargeNumber& rhs)
{
    if(m_size!=rhs.m_size)
    {
        if(m_n!=0)
        {
            delete [] m_n;
        }
        if(rhs.m_n!=0)
        {
            m_n = new tuint32 [static_cast<unsigned int>(rhs.m_size)];
            m_size = rhs.m_size;
        }
        else
        {
            m_n = 0;
            m_size = 0;
            m_sign = true;
            return;
        }
    }
    if(rhs.m_n!=0 && m_n!=0)
    {
        ::memcpy(m_n,rhs.m_n,static_cast<unsigned int>(m_size)<<2);
    }
    m_sign = rhs.m_sign;
}

//-------------------------------------------------------------------------------------------

void LargeNumber::decode(const tubyte *mem,tint len)
{
    tint i,j,u;
    tuint32 t;

    if(mem!=0 && len>0)
    {
        m_size = static_cast<tint>((static_cast<tuint>(len-1) >> 2) + 1);
        if(m_n!=0)
        {
            delete [] m_n;
        }
        m_n = new tuint32 [m_size];

        for(i=0,j=len-1;i<m_size && j>=0;i++)
        {
            t = 0;
            for(u=0;j>=0 && u<NN_DIGIT_BITS;j--,u+=8)
            {
                t |= static_cast<tuint32>(mem[j]) << u;
            }
            m_n[i] = t;
        }
        while(i<m_size)
        {
            m_n[i++] = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------

void LargeNumber::decode(const QByteArray& dMem)
{
    decode(reinterpret_cast<const tubyte *>(dMem.constData()),dMem.length());
}

//-------------------------------------------------------------------------------------------

void LargeNumber::encode(QByteArray& dMem) const
{
    tint len = m_size << 2;
    encode(dMem,len);
}

//-------------------------------------------------------------------------------------------

void LargeNumber::encode(QByteArray& dMem,tint len) const
{
    tint i,j,u;
    tuint32 t;
    tubyte *mem;

    dMem.resize(len);
    mem = reinterpret_cast<tubyte *>(dMem.data());

    for(i=0,j=len-1;i<m_size && j>=0;i++)
    {
        t = m_n[i];
        for(u=0;j<len && u<NN_DIGIT_BITS;j--,u+=8)
        {
            mem[j] = static_cast<tubyte>(t >> u);
        }
    }
    for(;j>=0;j--)
    {
        mem[j] = 0;
    }
}

//-------------------------------------------------------------------------------------------

void LargeNumber::createZero()
{
    int i;

    m_size = (m_size) ? m_size : 1;
    m_n = new tuint32 [static_cast<unsigned int>(m_size)];
    for(i=0;i<m_size;i++)
    {
        m_n[i] = 0;
    }
    m_sign = true;
}

//-------------------------------------------------------------------------------------------

void LargeNumber::deSign(tuint32 *a,tint s) const
{
    tint i,j;

    for(i=0;i<s;i++)
    {
        a[i] = 0 - a[i];
        if(a[i])
        {
            for(j=i+1;a[j]==MAX_NN_DIGIT && j<s;j++)
            {
                a[j]=0;
            }
            if(j<s)
            {
                a[j]++;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool LargeNumber::sub(tuint32 *e,const tuint32 *a,int c,tuint32 *b,int d) const
{
    tuint32 t = 0;
    tint i,j;

    for(i=0;i<c && i<d;i++)
    {
        e[i] = a[i] - b[i];
        if(a[i]<b[i])
        {
            for(j=i+1;b[j]==MAX_NN_DIGIT && j<d;j++)
            {
                b[j] = 0;
            }
            if(j<d)
            {
                b[j]++;
            }
            else
            {
                t = 1;
            }
        }
    }

    if(i<c)
    {
        for(;t && i<c;i++)
        {
            if(!a[i])
            {
                e[i] = static_cast<unsigned int>(-1);
            }
            else
            {
                e[i] = a[i] - 1;
                t = 0;
            }
        }
        while(i<c)
        {
            e[i] = a[i];
            i++;
        }
    }
    else if(i<d)
    {
        do
        {
            if(b[i])
            {
                e[i] = 0 - b[i];
                i++;
                if(i!=d)
                {
                    b[i]++;
                }
                t = 1;
            }
            else
            {
                e[i] = 0;
                i++;
            }
        } while(i!=d);
        c = d;
    }

    if(t)
    {
        deSign(e,c);
        return false;
    }
    else
    {
        return true;
    }
}

//-------------------------------------------------------------------------------------------

tuint32 LargeNumber::add(tuint32 *e,const tuint32 *a,int c,const tuint32 *b,int d) const
{
    int i;
    tuint32 temp,carry = 0;

    for(i=0;i<c && i<d;i++)
    {
        if((temp=a[i]+carry) < carry)
        {
            temp = b[i];
        }
        else
        {
            if((temp+=b[i]) < b[i])
            {
                carry = 1;
            }
            else
            {
                carry = 0;
            }
        }
        e[i]=temp;
    }

    if(i<c)
    {
        while((a[i]+carry) < carry)
        {
            e[i++] = 0;
            if(i==c)
            {
                return 1;
            }
        }
        for(;i<c;i++,carry=0)
        {
            e[i] = a[i] + carry;
        }
        return 0;
    }
    else if(i<d)
    {
        while((b[i]+carry) < carry)
        {
            e[i++] = 0;
            if(i==c)
            {
                return 1;
            }
        }
        for(;i<d;i++,carry=0)
        {
            e[i] = b[i] + carry;
        }
        return 0;
    }
    else
    {
        return carry;
    }
}

//-------------------------------------------------------------------------------------------

const LargeNumber& LargeNumber::operator -= (const LargeNumber& a)
{
    tuint32 *e,*f,c;
    int s = m_size;
    bool sn = m_sign;
    LargeNumberService& svr = LargeNumberService::instance();

    svr.lock();

    m_size = (s>=a.m_size) ? s : a.m_size;
    e = new tuint32 [static_cast<unsigned int>(m_size)];

    if((!m_sign && a.m_sign) || (m_sign && !a.m_sign))
    {
        c = add(e,m_n,s,a.m_n,a.m_size);
        m_sign = (sn) ? true : false;
        if(c)
        {
            f = new tuint32 [static_cast<unsigned int>(m_size)+1];
            ::memcpy(f,e,static_cast<unsigned int>(m_size)<<2);
            f[m_size++] = c;
            delete [] e;
            delete [] m_n;
            m_n = f;
        }
        else
        {
            delete [] m_n;
            m_n = e;
        }
    }
    else
    {
        ::memcpy(svr.largeArr(),a.m_n,static_cast<unsigned int>(a.m_size)<<2);
        m_sign = sub(e,m_n,s,svr.largeArr(),a.m_size);
        if(!sn)
        {
            m_sign = (m_sign) ? false : true;
        }
        delete [] m_n;
        m_n = e;
    }
    svr.unlock();

    return *this;
}

//-------------------------------------------------------------------------------------------

const LargeNumber& LargeNumber::operator += (const LargeNumber& a)
{
    tuint32 *e,*f,c;
    int s = m_size;
    LargeNumberService& svr = LargeNumberService::instance();

    svr.lock();

    m_size = (s>=a.m_size) ? s : a.m_size;
    e = new tuint32 [static_cast<unsigned int>(m_size)];

    if((m_sign && a.m_sign) || (!m_sign && !a.m_sign))
    {
        c = add(e,m_n,s,a.m_n,a.m_size);
        if(c)
        {
            f = new tuint32 [static_cast<unsigned int>(m_size)+1];
            ::memcpy(f,e,static_cast<unsigned int>(m_size)<<2);
            f[m_size++] = c;
            delete [] m_n;
            delete [] e;
            m_n = f;
        }
        else
        {
            delete [] m_n;
            m_n = e;
        }

    }
    else
    {
        ::memcpy(svr.largeArr(),a.m_n,static_cast<unsigned int>(a.m_size)<<2);
        bool sn = sub(e,m_n,s,svr.largeArr(),a.m_size);
        if(m_sign)
        {
            m_sign = (sn) ? true : false;
        }
        else
        {
            m_sign = (sn) ? false : true;
        }
        delete [] m_n;
        m_n = e;
    }
    svr.unlock();

    return *this;
}

//-------------------------------------------------------------------------------------------

const tuint32 LargeNumberShiftMask[32][4] = {
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

tuint32 LargeNumber::leftShift(tuint32 *a,const tuint32 *b,int c,int d) const
{
    int i;
    tuint32 r = 0,t;
    const tuint32 *e = &LargeNumberShiftMask[c][0];

    if(c)
    {
        for(i=0;i<d;i++)
        {
            t = b[i];
            a[i] = ((t << e[2]) & e[0]) | r;
            r = (t >> e[3]) & e[1];
        }
    }
    else
    {
        for(i=0;i<d;i++)
        {
            a[i] = b[i];
        }
    }
    return r;
}

//-------------------------------------------------------------------------------------------

tuint32 LargeNumber::rightShift(tuint32 *a,const tuint32 *b,int c,int d) const
{
    int i;
    tuint32 r = 0,t;
    const tuint32 *e = &LargeNumberShiftMask[(32 - c) & 0x1f][0];

    if(c)
    {
        for(i=d-1;i>=0;i--)
        {
            t = b[i];
            a[i] = ((t >> e[3]) & e[1]) | r;
            r = (t << e[2]) & e[0];
        }
    }
    else
    {
        for(i=0;i<d;i++)
        {
            a[i] = b[i];
        }
    }
    return r;
}

//-------------------------------------------------------------------------------------------

int LargeNumber::digits(const tuint32 *a,int digits) const
{
    if(digits)
    {
        digits--;
        do
        {
            if(a[digits])
            {
                break;
            }
        } while(digits--);
        return (++digits);
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

int LargeNumber::digitBits(tuint32 a) const
{
    int i;
    for(i=0;i<NN_DIGIT_BITS;i++,a>>=1)
    {
        if(!a)
        {
            break;
        }
    }
    return i;
}

//-------------------------------------------------------------------------------------------

void LargeNumber::assignZero(tuint32 *a,int digits) const
{
    int i;
    for(i=0;i<digits;i++)
    {
        *a++=0;
    }
}

//-------------------------------------------------------------------------------------------

void LargeNumber::assign(tuint32 *a,const tuint32 *b,int digits) const
{
    if(digits)
    {
        do
        {
            *a++=*b++;
        } while(--digits);
    }
}

//-------------------------------------------------------------------------------------------

tuint32 LargeNumber::divSub(tuint32 *a,const tuint32 *b,const tuint32 *c,int digits) const
{
    tuint32 temp,borrow=0;

    if(digits)
    {
        do
        {
            temp = *b - borrow;
            b++;
            if(temp==MAX_NN_DIGIT)
            {
                temp = MAX_NN_DIGIT - (*c);
                c++;
            }
            else
            {
                if((temp-=(*c)) > (MAX_NN_DIGIT-(*c)))
                {
                    borrow=1;
                }
                else
                {
                    borrow=0;
                }
                c++;
            }
            *a++=temp;
        } while(--digits);
    }
    return borrow;
}

//-------------------------------------------------------------------------------------------

int LargeNumber::divCmp(const tuint32 *a,const tuint32 *b,int digits) const
{
    if(digits)
    {
        do
        {
            digits--;
            if(a[digits] > b[digits])
            {
                return 1;
            }
            if(a[digits] < b[digits])
            {
                return -1;
            }
        } while(digits);
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

tuint32 LargeNumber::subDigitMult(tuint32 *a,const tuint32 *b,tuint32 c,const tuint32 *d,int digits) const
{
    tuint32 borrow,thigh,tlow;
    int i;

    borrow=0;
    if(c!=0)
    {
        for(i=0;i<digits;i++)
        {
            dMult(c,d[i],thigh,tlow);
            if((a[i]=b[i]-borrow) > (MAX_NN_DIGIT-borrow))
            {
                borrow=1;
            }
            else
            {
                borrow=0;
            }
            if((a[i]-=tlow) > (MAX_NN_DIGIT-tlow))
            {
                borrow++;
            }
            borrow+=thigh;
        }
    }
    return borrow;
}

//-------------------------------------------------------------------------------------------

void LargeNumber::dMult(tuint32 a,tuint32 b,tuint32& high,tuint32& low) const
{
    tuint16 al,ah,bl,bh;
    tuint32 m1,m2,m,ml,mh,carry=0;

    al = static_cast<tuint16>(LOW_HALF(a));
    ah = static_cast<tuint16>(HIGH_HALF(a));
    bl = static_cast<tuint16>(LOW_HALF(b));
    bh = static_cast<tuint16>(HIGH_HALF(b));

    low  = static_cast<tuint32>(al * bl);
    high = static_cast<tuint32>(ah * bh);

    m1 =static_cast<tuint32>(al * bh);
    m2 =static_cast<tuint32>(ah * bl);

    tuint64 mO = static_cast<tuint64>(m1) + static_cast<tuint64>(m2);
    m = static_cast<tuint32>(mO & 0x00000000ffffffffULL);

    if(mO & 0xffffffff00000000ULL)
    {
        carry = 1 << (NN_DIGIT_BITS/2);
    }

    ml = (m & MAX_NN_HALF_DIGIT) << (NN_DIGIT_BITS/2);
    mh = m >> (NN_DIGIT_BITS/2);

    low += ml;
    if(low<ml)
    {
        carry++;
    }
    high += carry + mh;
}

//-------------------------------------------------------------------------------------------

void LargeNumber::divide(tuint32 *a,tuint32 *b,const tuint32 *c,int cDigits,const tuint32 *d,int dDigits) const
{
    tuint32 ai,*dd,*cc,s;
    tuint32 t0,t1,u,v,*ccptr;
    tuint16 aHigh=0,aLow=0,cHigh,cLow;
    int i,ddDigits,shift;
    LargeNumberService& svr = LargeNumberService::instance();

    dd = svr.largeArr();
    cc = svr.cc();

    ddDigits = digits(d,dDigits);
    if(ddDigits==0)
    {
        return;
    }

    shift = NN_DIGIT_BITS - digitBits(d[ddDigits-1]);
    assignZero(cc,ddDigits);
    cc[cDigits] = leftShift(cc,c,shift,cDigits);
    leftShift(dd,d,shift,ddDigits);
    s = dd[ddDigits-1];
    assignZero(a,cDigits);

    for(i=cDigits-ddDigits;i>=0;i--)
    {
        if(s==MAX_NN_DIGIT)
        {
            ai = cc[i + ddDigits];
        }
        else
        {
            ccptr = &cc[i+ddDigits-1];

            s++;
            cHigh = static_cast<tuint16>(HIGH_HALF(s));
            cLow  = static_cast<tuint16>(LOW_HALF(s));

            t0 = ccptr[0];
            t1 = ccptr[1];

            if(cHigh==MAX_NN_HALF_DIGIT)
            {
                aHigh=static_cast<tuint16>(HIGH_HALF(t1));
            }
            else
            {
                aHigh=static_cast<tuint16>(t1/(cHigh+1));
            }
            u = static_cast<tuint32>(aHigh) * static_cast<tuint32>(cLow);
            v = static_cast<tuint32>(aHigh) * static_cast<tuint32>(cHigh);
            if((t0-=TO_HIGH_HALF(u)) > (MAX_NN_DIGIT - TO_HIGH_HALF(u)))
            {
                t1--;
            }
            t1 -= HIGH_HALF(u);
            t1 -= v;

            while((t1>cHigh) || ((t1==cHigh) && (t0>=TO_HIGH_HALF(cLow))))
            {
                if((t0-=TO_HIGH_HALF(cLow)) > (MAX_NN_DIGIT-TO_HIGH_HALF(cLow)))
                {
                    t1--;
                }
                t1 -= cHigh;
                aHigh++;
            }

            if(cHigh==MAX_NN_HALF_DIGIT)
            {
                aLow = static_cast<tuint16>(LOW_HALF(t1));
            }
            else
            {
                aLow = static_cast<tuint16>((TO_HIGH_HALF(t1) + HIGH_HALF(t0)) / (cHigh + 1));
            }
            u = static_cast<tuint32>(aLow) * static_cast<tuint32>(cLow);
            v = static_cast<tuint32>(aLow) * static_cast<tuint32>(cHigh);
            if((t0-=u) > (MAX_NN_DIGIT-u))
            {
                t1--;
            }
            if((t0-=TO_HIGH_HALF(v)) > (MAX_NN_DIGIT-TO_HIGH_HALF(v)))
            {
                t1--;
            }
            t1 -= HIGH_HALF(v);

            while((t1>0) || ((t1==0) && (t0>=s)))
            {
                if((t0-=s) > (MAX_NN_DIGIT-s))
                {
                    t1--;
                }
                aLow++;
            }

            ai = TO_HIGH_HALF(aHigh) + aLow;
            s--;
        }

        cc[i+ddDigits] -= subDigitMult(&cc[i],&cc[i],ai,dd,ddDigits);

        while(cc[i+ddDigits] || (divCmp(&cc[i],dd,ddDigits)>=0))
        {
            ai++;
            cc[i+ddDigits] -= divSub(&cc[i],&cc[i],dd,ddDigits);
        }

        a[i] = ai;
    }

    assignZero(b,dDigits);
    rightShift(b,cc,shift,ddDigits);
}

//-------------------------------------------------------------------------------------------

void LargeNumber::multiple(tuint32 *a,const tuint32 *b,const tuint32 *c,int dLen) const
{
    tuint32 *t = LargeNumberService::instance().cc();
    tuint32 dhigh,dlow,carry;
    int bDigits,cDigits,i,j;

    assignZero(t,2*dLen);

    bDigits = digits(b,dLen);
    cDigits = digits(c,dLen);

    for(i=0;i<bDigits;i++)
    {
        carry = 0;
        if(b[i]!=0)
        {
            for(j=0;j<cDigits;j++)
            {
                dMult(b[i],c[j],dhigh,dlow);
                if((t[i+j]+=carry) < carry)
                {
                    carry=1;
                }
                else
                {
                    carry=0;
                }
                if((t[i+j]+=dlow) < dlow)
                {
                    carry++;
                }
                carry+=dhigh;
            }
        }
        t[i+cDigits]+=carry;
    }
    assign(a,t,2*dLen);
}

//-------------------------------------------------------------------------------------------

tuint32 *LargeNumber::scaleArray(tuint32 *a,int& dLen,bool removeF) const
{
    tuint32 *b = 0;
    int bDigits = dLen;

    if(a==NULL || !dLen)
    {
        b = new tuint32 [1];
        b[0] = 0;
        dLen=1;
        return b;
    }

    for(bDigits--;a[bDigits]==0 && bDigits>0;bDigits--) ;
    bDigits++;

    b = new tuint32 [static_cast<unsigned int>(bDigits)];

    for(dLen=0;dLen<bDigits;dLen++)
    {
        b[dLen]=a[dLen];
    }

    if(removeF)
    {
        delete [] a;
    }
    return b;
}

//-------------------------------------------------------------------------------------------

const LargeNumber& LargeNumber::operator /= (const LargeNumber& a)
{
    LargeNumberService& svr = LargeNumberService::instance();

    svr.lock();
    if(m_n==0 || a.m_n==0)
    {
        createZero();
    }
    else
    {
        divide(svr.ansA(),svr.ansB(),m_n,m_size,a.m_n,a.m_size);
        delete [] m_n;
        m_n = scaleArray(svr.ansA(),m_size,false);
        m_sign = (m_sign==a.m_sign) ? true : false;
    }
    svr.unlock();
    return *this;
}

//-------------------------------------------------------------------------------------------

const LargeNumber& LargeNumber::operator %= (const LargeNumber& a)
{
    LargeNumberService& svr = LargeNumberService::instance();

    svr.lock();
    if(m_n==0 || a.m_n==0)
    {
        createZero();
    }
    else
    {
        divide(svr.ansA(),svr.ansB(),m_n,m_size,a.m_n,a.m_size);
        delete [] m_n;
        m_size = a.m_size;
        m_n = scaleArray(svr.ansB(),m_size,false);
        m_sign = true;
    }
    svr.unlock();
    return *this;
}

//-------------------------------------------------------------------------------------------

const LargeNumber& LargeNumber::operator *= (const LargeNumber& a)
{
    tuint32 *ansB;
    LargeNumberService& svr = LargeNumberService::instance();

    svr.lock();
    if(m_n==0 || a.m_n==0)
    {
        createZero();
    }
    else
    {
        ansB = svr.ansB();
        if(a.m_size>m_size)
        {
            ::memcpy(ansB,m_n,m_size * sizeof(tuint32));
            ::memset(&ansB[m_size],0,(a.m_size - m_size) * sizeof(tuint32));
            multiple(svr.ansA(),ansB,a.m_n,a.m_size);
            m_size = static_cast<int>(static_cast<unsigned int>(a.m_size) << 1);
        }
        else if(m_size>a.m_size)
        {
            ::memcpy(ansB,a.m_n,a.m_size * sizeof(tuint32));
            ::memset(&ansB[a.m_size],0,(m_size - a.m_size) * sizeof(tuint32));
            multiple(svr.ansA(),m_n,ansB,m_size);
            m_size = static_cast<int>(static_cast<unsigned int>(m_size) << 1);
        }
        else
        {
            multiple(svr.ansA(),m_n,a.m_n,m_size);
            m_size = static_cast<int>(static_cast<unsigned int>(m_size) << 1);
        }
        delete [] m_n;
        m_n = scaleArray(svr.ansA(),m_size,false);
        m_sign = (m_sign==a.m_sign) ? true : false;
    }
    svr.unlock();
    return *this;
}

//-------------------------------------------------------------------------------------------

void LargeNumber::assignDigit(tuint32 a,int dLen)
{
    if(!dLen)
    {
        return;
    }
    if(m_n!=0 && m_size!=dLen)
    {
        delete [] m_n;
        m_n = 0;
        m_size = 0;
    }
    if(m_n==NULL)
    {
        m_n = new tuint32 [static_cast<unsigned int>(dLen)];
        m_size = dLen;
    }

    assignZero(m_n,m_size);
    m_n[0] = a;
    m_sign = true;
}

//-------------------------------------------------------------------------------------------

void LargeNumber::assign2Exp(int b,int dLen)
{
    if(!dLen)
    {
        return;
    }
    if(m_n!=0 && m_size!=dLen)
    {
        delete [] m_n;
        m_n = 0;
        m_size = 0;
    }
    if(m_n==0)
    {
        m_n = new tuint32 [static_cast<unsigned int>(dLen)];
        m_size = dLen;
    }

    assignZero(m_n,m_size);
    m_n[b/NN_DIGIT_BITS] = static_cast<tuint32>(1<<(b%NN_DIGIT_BITS));
    m_sign=true;
}

//-------------------------------------------------------------------------------------------

bool operator == (const LargeNumber& a,const LargeNumber& b)
{
    return (a.cmp(b)==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator != (const LargeNumber& a,const LargeNumber& b)
{
    return (a.cmp(b)!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator < (const LargeNumber& a,const LargeNumber& b)
{
    return (a.cmp(b)<0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator <= (const LargeNumber& a,const LargeNumber& b)
{
    return (a.cmp(b)<=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator > (const LargeNumber& a,const LargeNumber& b)
{
    return (a.cmp(b)>0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool operator >= (const LargeNumber& a,const LargeNumber& b)
{
    return (a.cmp(b)>=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

int LargeNumber::cmp(const LargeNumber& a) const
{
    int i;

    if(m_n==0)
    {
        if(a.m_n==0)
        {
            return 0;    //this == a
        }
        else
        {
            return -1;    //this < a
        }
    }
    else if(a.m_n==0)
    {
        return 1;    //this > a
    }

    if(m_size>a.m_size) //this > a = 1
    {
        for(i=m_size-1;i>=a.m_size;i--)
        {
            if(m_n[i]!=0)
            {
                return 1;
            }
        }
    }
    else if(m_size<a.m_size) //this < a = -1
    {
        for(i=a.m_size-1;i>=m_size;i--)
        {
            if(a.m_n[i]!=0)
            {
                return -1;
            }
        }
    }
    else
    {
        i=m_size-1;
    }
    for(;i>=0;i--)
    {
        if(m_n[i]>a.m_n[i])
        {
            return 1;
        }
        if(m_n[i]<a.m_n[i])
        {
            return -1;
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------

bool LargeNumber::zero()
{
    return zero(m_size);
}

//-------------------------------------------------------------------------------------------

bool LargeNumber::zero(int dLen) const
{
    if(m_n==0)
    {
        return true;
    }
    dLen = (dLen>m_size) ? m_size-1 : dLen-1;
    for(;dLen>=0;dLen--)
    {
        if(m_n[dLen]!=0)
        {
            return false;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void LargeNumber::resize(int newSize)
{
    int i=0;
    tuint32 *m;

    if(m_size>=newSize)
    {
        return;
    }
    m = new tuint32 [static_cast<unsigned int>(newSize)];
    if(m==0)
    {
        return;
    }
    if(m_n!=0)
    {
        for(i=0;i<m_size;i++)
        {
            m[i] = m_n[i];
        }
    }
    for(;i<newSize;i++)
    {
        m[i] = 0;
    }
    delete [] m_n;
    m_n = m;
    m_size = newSize;
}

//-------------------------------------------------------------------------------------------

LargeNumber operator + (const LargeNumber& a,const LargeNumber& b)
{
    LargeNumber c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

LargeNumber operator - (const LargeNumber& a,const LargeNumber& b)
{
    LargeNumber c(a);
    c -= b;
    return c;
}
//-------------------------------------------------------------------------------------------

LargeNumber operator / (const LargeNumber& a,const LargeNumber& b)
{
    LargeNumber c(a);
    c /= b;
    return c;
}

//-------------------------------------------------------------------------------------------

LargeNumber operator % (const LargeNumber& a,const LargeNumber& b)
{
    LargeNumber c(a);
    c %= b;
    return c;
}

//-------------------------------------------------------------------------------------------

LargeNumber operator * (const LargeNumber& a,const LargeNumber& b)
{
    LargeNumber c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

LargeNumber modMult(LargeNumber& b,const LargeNumber& c,const LargeNumber& d)
{
    LargeNumber a,t;

    t = b * c;
    a = t % d;
    return a;
}

//-------------------------------------------------------------------------------------------

LargeNumber modExp(const LargeNumber& b,const LargeNumber& c,LargeNumber& d)
{
    LargeNumber bPower[3],t;
    tuint32 ci;
    int i,cDigits;
    unsigned int ciBits,j,s;

    bPower[0] = b;
    bPower[1] = modMult(bPower[0],b,d);
    bPower[2] = modMult(bPower[1],b,d);
    t.assignDigit(1,d.m_size);

    cDigits = c.digits(c.m_n,c.m_size);
    for(i=cDigits-1;i>=0;i--)
    {
        ci = c.m_n[i];
        ciBits = NN_DIGIT_BITS;
        if(i==(cDigits-1))
        {
            while(!DIGIT_2MSB(ci))
            {
                ci <<= 2;
                ciBits -= 2;
            }
        }
        for(j=0;j<ciBits;j+=2,ci<<=2)
        {
            t = modMult(t,t,d);
            t = modMult(t,t,d);
            if((s=DIGIT_2MSB(ci))!=0)
            {
                t = modMult(t,bPower[s-1],d);
            }
        }
    }
    return t;
}

//-------------------------------------------------------------------------------------------
#define iplus1 (i==2 ? 0 : i+1)
#define iminus1 (i==0 ? 2 : i-1)
//-------------------------------------------------------------------------------------------

LargeNumber gcd(LargeNumber& b,const LargeNumber& c)
{
    int i;
    LargeNumber t[3];

    t[0] = c;
    t[1] = b;

    i=1;

    while(!t[i].zero())
    {
        t[(iplus1)] = t[(iminus1)] % t[i];
        i = (iplus1);
    }
    b = t[(iminus1)];
    return b;
}

//-------------------------------------------------------------------------------------------

LargeNumber modInv(const LargeNumber& b,LargeNumber& c)
{
    int u1Sign;
    LargeNumber a,q,t1,t3;
    LargeNumber u1,u3,v1,v3,w;

    u1.assignDigit(1,1);
    v1.assignDigit(0,1);
    u3 = b;
    v3 = c;
    u1Sign = 1;

    while(!v3.zero())
    {
        q = u3 / v3;
        t3 = u3 % v3;
        w = q * v1;
        t1 = u1 + w;
        u1 = v1;
        v1 = t1;
        u3 = v3;
        v3 = t3;
        u1Sign = -u1Sign;
    }

    if(u1Sign<0)
    {
        a = c - u1;
    }
    else
    {
        a = u1;
    }
    return a;
}

//-------------------------------------------------------------------------------------------

typedef struct s_ImportBits {
    unsigned int mask_a;
    int shift_a;
    int shift_b;
    bool boundary;
} ImportBits;

static const ImportBits c_writeLookup[32]={
    {0xfffffff8,0,0,false},
    {0xffffffc7,3,0,false},
    {0xfffffe3f,6,0,false},
    {0xfffff1ff,9,0,false},
    {0xffff8fff,12,0,false},
    {0xfffc7fff,15,0,false},
    {0xffe3ffff,18,0,false},
    {0xff1fffff,21,0,false},
    {0xf8ffffff,24,0,false},
    {0xc7ffffff,27,0,false},
    {0x3fffffff,30,2,true},
    {0xfffffff1,1,0,false},
    {0xffffff8f,4,0,false},
    {0xfffffc7f,7,0,false},
    {0xffffe3ff,10,0,false},
    {0xffff1fff,13,0,false},
    {0xfff8ffff,16,0,false},
    {0xffc7ffff,19,0,false},
    {0xfe3fffff,22,0,false},
    {0xf1ffffff,25,0,false},
    {0x8fffffff,28,0,false},
    {0x7fffffff,31,1,true},
    {0xffffffe3,2,0,false},
    {0xffffff1f,5,0,false},
    {0xfffff8ff,8,0,false},
    {0xffffc7ff,11,0,false},
    {0xfffe3fff,14,0,false},
    {0xfff1ffff,17,0,false},
    {0xff8fffff,20,0,false},
    {0xfc7fffff,23,0,false},
    {0xe3ffffff,26,0,false},
    {0x1fffffff,29,0,false}
};

//-------------------------------------------------------------------------------------------

void LargeNumber::fromString(const QString& str)
{
    if(!str.isEmpty())
    {
        tint i,j,k,*a,*b,len,p;

        if(str.at(0)==QChar('-'))
        {
            i = 1;
            m_sign = false;
        }
        else
        {
            i = (str.at(0)==QChar('+')) ? 1 : 0;
            m_sign = true;
        }

        a = new tint [str.length() - i];
        b = new tint [str.length() - i];

        for(j=0;i<str.length();i++)
        {
            if(str.at(i)>=QChar('0') && str.at(i)<=QChar('9'))
            {
                a[j] = static_cast<tint>(str.at(i).toLatin1() - '0');
                j++;
            }
        }
        len = j - 1;

        b[0] = 0;
        for(p=0,k=0,i=0;i<=len;)
        {
            if(a[i])
            {
                for(j=i;j<len;j++)
                {
                    a[j+1] += (a[j] & 0x00000007) * 10;
                    a[j] = a[j] >> 3;
                }
                {
                    tint t = a[j] & 0x00000007;
                    const ImportBits *m = &c_writeLookup[p++];

                    if(m->boundary)
                    {
                        b[k] = static_cast<tint>((static_cast<tuint>(b[k]) & m->mask_a) | (static_cast<tuint>(t) << m->shift_a));
                        k++;
                        b[k] = static_cast<tint>(static_cast<tuint>(t) >> m->shift_b);
                    }
                    else
                    {
                        b[k] = static_cast<tint>((static_cast<tuint>(b[k]) & m->mask_a) | (static_cast<tuint>(t) << m->shift_a));
                    }
                    p &= 0x0000001f;
                    if(!p)
                    {
                        b[++k] = 0;
                    }
                    a[j] = static_cast<tuint>(a[j]) >> 3;
                }
            }
            else
            {
                i++;
            }
        }

        if(m_n!=0)
        {
            delete [] m_n;
        }
        k++;
        m_n = new tuint32 [k];
        m_size = k;
        for(i=0;i<k;i++)
        {
            m_n[i] = static_cast<tuint>(b[i]);
        }

        delete [] b;
        delete [] a;
    }
    else
    {
        createZero();
    }
}

//-------------------------------------------------------------------------------------------

QString LargeNumber::toString() const
{
    QString oStr;

    if(m_n!=0)
    {
        tint i,j,k,t;
        tint *a;
        bool flag = false;

        i = 0;
        a = new tint [m_size << 3];
        for(j=m_size-1;j>=0;j--)
        {
            for(i=28;i>=0;i-=4)
            {
                k = (m_n[j] >> i) & 0x0000000f;
                if(k)
                {
                    a[0] = k;
                    flag = true;
                    break;
                }
            }
            if(flag)
            {
                break;
            }
        }
        if(flag)
        {
            tchar *arr=0,*str=0;

            for(k=1,i-=4;i>=0;i-=4,k++)
            {
                a[k] = (m_n[j] >> i) & 0x0000000f;
            }
            for(j--;j>=0;j--)
            {
                for(i=28;i>=0;i-=4,k++)
                {
                    a[k] = (m_n[j] >> i) & 0x0000000f;
                }
            }

            for(t=0,i=0,k--;i<=k;t++)
            {
                for(j=i;j<k;j++)
                {
                    a[j+1] += (a[j] % 10) << 4;
                    a[j] /= 10;
                }

                if(!(t & 0x000000ff))
                {
                    str = new tchar [t + 256];
                    if(arr!=0)
                    {
                        ::memcpy(str,arr,static_cast<size_t>(t));
                        delete [] arr;
                    }
                    arr = str;
                }
                if(arr!=0)
                {
                    arr[t] = static_cast<tchar>(a[k] % 10) + '0';
                    a[k] /= 10;
                    if(!a[i])
                    {
                        i++;
                    }
                }
            }

            if(arr!=0)
            {
                if(!m_sign)
                {
                    str = new tchar [t+2];
                    str[0] = '-';
                    i = 1;
                }
                else
                {
                    str = new tchar [t+1];
                    i = 0;
                }
                for(j=t-1;j>=0;i++,j--)
                {
                    str[i] = arr[j];
                }
                str[i] = '\0';
                oStr = QString::fromLatin1(str);
                delete [] arr;
                delete [] str;
            }
        }
        else
        {
            oStr = "0";
        }
        delete [] a;
    }
    else
    {
        oStr = "0";
    }
    return oStr;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
