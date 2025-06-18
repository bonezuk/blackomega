//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_LARGENUMBER_H
#define __OMEGA_COMMON_LARGENUMBER_H
//-------------------------------------------------------------------------------------------

#include <QMutex>
#include <QString>
#include <QByteArray>

#include "common/inc/CommonTypes.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

#define MAX_NN_DIGIT      0xffffffff
#define MAX_NN_HALF_DIGIT 0xffff

#define MAX_NN_DIGITS      256
#define NN_DIGIT_BITS       32
#define NN_HALF_DIGIT_BITS  16
#define NN_DIGIT_LEN         4

#define NN_LT -1
#define NN_EQ  0
#define NN_GT  1

#define LOW_HALF(x) ((x) & MAX_NN_HALF_DIGIT)
#define HIGH_HALF(x) ((static_cast<tuint32>(x) >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT)
#define TO_HIGH_HALF(x) ((static_cast<tuint32>(x)) << NN_HALF_DIGIT_BITS)
#define DIGIT_MSB(x) static_cast<unsigned int>(((x) >> (NN_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x) static_cast<unsigned int>(((x) >> (NN_DIGIT_BITS - 2)) & 3)

//-------------------------------------------------------------------------------------------

class LargeNumberService
{
    public:
        virtual ~LargeNumberService();
        
        static LargeNumberService& instance();

        void clear();
        
        void lock();
        void unlock();
        
        tuint32 *largeArr();
        tuint32 *cc();
        tuint32 *ansA();
        tuint32 *ansB();
        tuint32 *ansC();
        
    protected:
        
        static LargeNumberService *m_instance;
        
        QMutex m_mutex;
        tuint32 *m_largeArr;
        tuint32 *m_cc;
        tuint32 *m_ansA;
        tuint32 *m_ansB;
        tuint32 *m_ansC;
        
        LargeNumberService();
};

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT LargeNumber
{
    public:
        LargeNumber();
        LargeNumber(tint a);
        LargeNumber(tuint64 a);
        LargeNumber(const LargeNumber& rhs);
        virtual ~LargeNumber();
        
        const LargeNumber& operator = (const LargeNumber& rhs);
        const LargeNumber& operator -= (const LargeNumber& a);
        const LargeNumber& operator += (const LargeNumber& a);
        const LargeNumber& operator /= (const LargeNumber& a);
        const LargeNumber& operator %= (const LargeNumber& a);
        const LargeNumber& operator *= (const LargeNumber& a);
        
        void decode(const tubyte *mem,tint len);
        void decode(const QByteArray& dMem);
        void encode(QByteArray& dMem) const; 
        void encode(QByteArray& dMem,tint len) const;
        
        void fromString(const QString& str);
        QString toString() const;
        
        tint size() const;
        void resize(int newSize);
        
        void assignDigit(tuint32 a,int digits);
        void assign2Exp(int b,int digits);
        
        bool zero();
        bool zero(int digits) const;
        
        friend COMMON_EXPORT bool operator == (const LargeNumber& a,const LargeNumber& b);
        friend COMMON_EXPORT bool operator != (const LargeNumber& a,const LargeNumber& b);
        friend COMMON_EXPORT bool operator < (const LargeNumber& a,const LargeNumber& b);
        friend COMMON_EXPORT bool operator <= (const LargeNumber& a,const LargeNumber& b);
        friend COMMON_EXPORT bool operator > (const LargeNumber& a,const LargeNumber& b);
        friend COMMON_EXPORT bool operator >= (const LargeNumber& a,const LargeNumber& b);
        
        friend COMMON_EXPORT LargeNumber operator + (const LargeNumber& a,const LargeNumber& b);
        friend COMMON_EXPORT LargeNumber operator - (const LargeNumber& a,const LargeNumber& b);
        friend COMMON_EXPORT LargeNumber operator / (const LargeNumber& a,const LargeNumber& b);
        friend COMMON_EXPORT LargeNumber operator % (const LargeNumber& a,const LargeNumber& b);
        friend COMMON_EXPORT LargeNumber operator * (const LargeNumber& a,const LargeNumber& b);
        
        friend COMMON_EXPORT LargeNumber modMult(LargeNumber& b,const LargeNumber& c,const LargeNumber& d);
        friend COMMON_EXPORT LargeNumber modExp(const LargeNumber& b,const LargeNumber& c,LargeNumber& d);
        friend COMMON_EXPORT LargeNumber gcd(LargeNumber& b,const LargeNumber& c);
        friend COMMON_EXPORT LargeNumber modInv(const LargeNumber& b,LargeNumber& c);
        
    protected:
        bool m_sign; //true->'+' / false->'-'
        tuint32 *m_n; //location of number array.
        tint m_size; //size of array.
        
        void printError(const tchar *strR,const tchar *strE) const;
        void copy(const LargeNumber& rhs);
        void createZero();
        void deSign(tuint32 *a,tint s) const;
        bool sub(tuint32 *e,const tuint32 *a,int c,tuint32 *b,int d) const;
        tuint32 add(tuint32 *e,const tuint32 *a,int c,const tuint32 *b,int d) const;
        tuint32 leftShift(tuint32 *a,const tuint32 *b,int c,int d) const;
        tuint32 rightShift(tuint32 *a,const tuint32 *b,int c,int d) const;
        int digits(const tuint32 *a,int digits) const;
        int digitBits(tuint32 a) const;
        void assignZero(tuint32 *a,int digits) const;
        void assign(tuint32 *a,const tuint32 *b,int digits) const;
        tuint32 divSub(tuint32 *a,const tuint32 *b,const tuint32 *c,int digits) const;
        int divCmp(const tuint32 *a,const tuint32 *b,int digits) const;
        tuint32 subDigitMult(tuint32 *a,const tuint32 *b,tuint32 c,const tuint32 *d,int digits) const;
        void dMult(tuint32 a,tuint32 b,tuint32& high,tuint32& low) const;
        void divide(tuint32 *a,tuint32 *b,const tuint32 *c,int cDigits,const tuint32 *d,int dDigits) const;
        void multiple(tuint32 *a,const tuint32 *b,const tuint32 *c,int digits) const;
        tuint32 *scaleArray(tuint32 *a,int& digits,bool removeF) const;
        int cmp(const LargeNumber& a) const;
};

//-------------------------------------------------------------------------------------------

inline void LargeNumberService::lock()
{
    m_mutex.lock();
}

//-------------------------------------------------------------------------------------------

inline void LargeNumberService::unlock()
{
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

inline tuint32 *LargeNumberService::largeArr()
{
    return m_largeArr;
}

//-------------------------------------------------------------------------------------------

inline tuint32 *LargeNumberService::cc()
{
    return m_cc;
}

//-------------------------------------------------------------------------------------------

inline tuint32 *LargeNumberService::ansA()
{
    return m_ansA;
}

//-------------------------------------------------------------------------------------------

inline tuint32 *LargeNumberService::ansB()
{
    return m_ansB;
}

//-------------------------------------------------------------------------------------------

inline tuint32 *LargeNumberService::ansC()
{
    return m_ansC;
}

//-------------------------------------------------------------------------------------------

inline tint LargeNumber::size() const
{
    return m_size;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
