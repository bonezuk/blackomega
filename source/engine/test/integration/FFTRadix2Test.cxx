#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/CommonTypes.h"
#include "common/inc/Allocation.h"
#include "common/inc/TimeStamp.h"
#include "common/inc/Random.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

class Complex
{
    public:
        Complex();
        Complex(const tfloat64& rNum,const tfloat64& iNum);
        Complex(const Complex & rhs);

        const Complex& operator = (const Complex& rhs);

        const Complex& operator += (const Complex& b);
        const Complex& operator -= (const Complex& b);
        const Complex& operator *= (const Complex& b);

        tfloat64& R();
        const tfloat64& R() const;
        tfloat64& I();
        const tfloat64& I() const;

        static Complex W(int n,int m,int N);
        static Complex W(int n,int N);

    protected:

        tfloat64 m_R;
        tfloat64 m_I;

        void copy(const Complex& rhs);
};

Complex operator + (const Complex& a,const Complex& b);
Complex operator - (const Complex& a,const Complex& b);
Complex operator * (const Complex& a,const Complex& b);

//-------------------------------------------------------------------------------------------

Complex::Complex() : m_R(0.0),
    m_I(0.0)
{}

//-------------------------------------------------------------------------------------------

Complex::Complex(const tfloat64& rNum,const tfloat64& iNum) : m_R(rNum),
    m_I(iNum)
{}

//-------------------------------------------------------------------------------------------

Complex::Complex(const Complex & rhs) : m_R(0.0),
    m_I(0.0)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

const Complex& Complex::operator = (const Complex& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void Complex::copy(const Complex& rhs)
{
    m_R = rhs.m_R;
    m_I = rhs.m_I;
}

//-------------------------------------------------------------------------------------------

const Complex& Complex::operator += (const Complex& b)
{
    m_R += b.m_R;
    m_I += b.m_I;
    return *this;
}

//-------------------------------------------------------------------------------------------

const Complex& Complex::operator -= (const Complex& b)
{
    m_R -= b.m_R;
    m_I -= b.m_I;
    return *this;
}

//-------------------------------------------------------------------------------------------

const Complex& Complex::operator *= (const Complex& b)
{
    tfloat64 a = (m_R * b.m_R) - (m_I * b.m_I);
    tfloat64 c = (m_R * b.m_I) + (m_I * b.m_R);
    m_R = a;
    m_I = c;
    return *this;
}

//-------------------------------------------------------------------------------------------

tfloat64& Complex::R()
{
    return m_R;
}

//-------------------------------------------------------------------------------------------

const tfloat64& Complex::R() const
{
    return m_R;
}

//-------------------------------------------------------------------------------------------

tfloat64& Complex::I()
{
    return m_I;
}

//-------------------------------------------------------------------------------------------

const tfloat64& Complex::I() const
{
    return m_I;
}

//-------------------------------------------------------------------------------------------

Complex operator + (const Complex& a,const Complex& b)
{
    Complex c(a);
    c += b;
    return c;
}

//-------------------------------------------------------------------------------------------

Complex operator - (const Complex& a,const Complex& b)
{
    Complex c(a);
    c -= b;
    return c;
}

//-------------------------------------------------------------------------------------------

Complex operator * (const Complex& a,const Complex& b)
{
    Complex c(a);
    c *= b;
    return c;
}

//-------------------------------------------------------------------------------------------

Complex Complex::W(int n,int m,int N)
{
    return W(n * m,N);
}

//-------------------------------------------------------------------------------------------

Complex Complex::W(int n,int N)
{
    tfloat64 angle = (2.0 * c_PI_D * static_cast<tfloat64>(n)) / static_cast<tfloat64>(N);
    tfloat64 R = cos(angle);
    tfloat64 I = 0.0 - sin(angle);
    return Complex(R,I);
}

//-------------------------------------------------------------------------------------------

Complex *DFT8(Complex *x)
{
    Complex *X = new Complex [8];

    for(int k=0;k<8;k++)
    {
        Complex c;

        for(int n=0;n<8;n++)
        {
            c += x[n] * Complex::W(k,n,8);
        }
        X[k] = c;
    }
    return X;
}

//-------------------------------------------------------------------------------------------

Complex *DFT16(Complex *x)
{
    Complex *X = new Complex [16];

    for(int k=0;k<16;k++)
    {
        Complex c;

        for(int n=0;n<16;n++)
        {
            c += x[n] * Complex::W(k,n,16);
        }
        X[k] = c;
    }
    return X;
}

//-------------------------------------------------------------------------------------------

Complex *DFT_N(Complex *x,tint N)
{
    Complex *X = new Complex [N];

    for(int k=0;k<N;k++)
    {
        Complex c;

        for(int n=0;n<N;n++)
        {
            c += x[n] * Complex::W(k,n,N);
        }
        X[k] = c;
    }
    return X;
}

//-------------------------------------------------------------------------------------------

Complex *FFT8_Radix2_A(Complex *x)
{
    Complex *X = new Complex [8];

    Complex G1_0 = (x[0] * Complex::W(0,2)) + (x[4] * Complex::W(0,2));
    Complex G1_1 = (x[0] * Complex::W(0,2)) + (x[4] * Complex::W(1,2));

    Complex G2_0 = (x[2] * Complex::W(0,2)) + (x[6] * Complex::W(0,2));
    Complex G2_1 = (x[2] * Complex::W(0,2)) + (x[6] * Complex::W(1,2));

    Complex J1_0 = (x[1] * Complex::W(0,2)) + (x[5] * Complex::W(0,2));
    Complex J1_1 = (x[1] * Complex::W(0,2)) + (x[5] * Complex::W(1,2));

    Complex J2_0 = (x[3] * Complex::W(0,2)) + (x[7] * Complex::W(0,2));
    Complex J2_1 = (x[3] * Complex::W(0,2)) + (x[7] * Complex::W(1,2));

    Complex F1_0 = G1_0 + (Complex::W(0,4) * G2_0);
    Complex F1_1 = G1_1 + (Complex::W(1,4) * G2_1);
    Complex F1_2 = G1_0 - (Complex::W(0,4) * G2_0);
    Complex F1_3 = G1_1 - (Complex::W(1,4) * G2_1);

    Complex F2_0 = J1_0 + (Complex::W(0,4) * J2_0);
    Complex F2_1 = J1_1 + (Complex::W(1,4) * J2_1);
    Complex F2_2 = J1_0 - (Complex::W(0,4) * J2_0);
    Complex F2_3 = J1_1 - (Complex::W(1,4) * J2_1);

    X[0] = F1_0 + (Complex::W(0,8) * F2_0);
    X[1] = F1_1 + (Complex::W(1,8) * F2_1);
    X[2] = F1_2 + (Complex::W(2,8) * F2_2);
    X[3] = F1_3 + (Complex::W(3,8) * F2_3);
    X[4] = F1_0 - (Complex::W(0,8) * F2_0);
    X[5] = F1_1 - (Complex::W(1,8) * F2_1);
    X[6] = F1_2 - (Complex::W(2,8) * F2_2);
    X[7] = F1_3 - (Complex::W(3,8) * F2_3);

    return X;
}

//-------------------------------------------------------------------------------------------

tint getFFTBitReverseIndex_B(tint index,tint noBits)
{
    tuint32 y = static_cast<tuint32>(index), x = 0;

    while(noBits>0)
    {
        x = (x << 1) | (y & 0x00000001);
        y >>= 1;
        noBits--;
    }
    return static_cast<tint>(x);
}

Complex *FFT2_Radix2_B(Complex *x,int index,int noBits)
{
    tint idxA = getFFTBitReverseIndex_B(index,noBits);
    tint idxB = getFFTBitReverseIndex_B(index+1,noBits);
    Complex *X = new Complex [2];
    X[0] = (x[idxA] * Complex::W(0,2)) + (x[idxB] * Complex::W(0,2));
    X[1] = (x[idxA] * Complex::W(0,2)) + (x[idxB] * Complex::W(1,2));
    return X;
}

Complex *FFT4_Radix2_B(Complex *x,int index,int noBits)
{
    Complex *X = new Complex[4];
    Complex *F1 = FFT2_Radix2_B(x,index,noBits);
    Complex *F2 = FFT2_Radix2_B(x,index+2,noBits);
    X[0] = F1[0] + (Complex::W(0,4) * F2[0]);
    X[1] = F1[1] + (Complex::W(1,4) * F2[1]);
    X[2] = F1[0] - (Complex::W(0,4) * F2[0]);
    X[3] = F1[1] - (Complex::W(1,4) * F2[1]);
    delete [] F1;
    delete [] F2;
    return X;
}

Complex *FFT8_Radix2_B(Complex *x,int index,int noBits)
{
    Complex *X = new Complex [8];
    Complex *F1 = FFT4_Radix2_B(x,index,noBits);
    Complex *F2 = FFT4_Radix2_B(x,index+4,noBits);
    X[0] = F1[0] + (Complex::W(0,8) * F2[0]);
    X[1] = F1[1] + (Complex::W(1,8) * F2[1]);
    X[2] = F1[2] + (Complex::W(2,8) * F2[2]);
    X[3] = F1[3] + (Complex::W(3,8) * F2[3]);
    X[4] = F1[0] - (Complex::W(0,8) * F2[0]);
    X[5] = F1[1] - (Complex::W(1,8) * F2[1]);
    X[6] = F1[2] - (Complex::W(2,8) * F2[2]);
    X[7] = F1[3] - (Complex::W(3,8) * F2[3]);
    delete [] F1;
    delete [] F2;
    return X;
}

Complex *FFT8_Radix2_B(Complex *x)
{
    return FFT8_Radix2_B(x,0,3);
}

//-------------------------------------------------------------------------------------------

static const tfloat64 c_TOLERANCE = 0.00000001;

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT8_Radix2)
{
    tfloat64 t[16] = {
        2.3, 1.4, 5.6, 4.3, 4.5, 6.7, 8.1, 2.5,
        3.4, 6.3, 9.2, 4.8, 7.0, 0.3, 3.2, 1.3
    };

    Complex x[8];
    for(int i=0;i<8;i++)
    {
        x[i].R() = t[(i*2)+0];
        x[i].I() = t[(i*2)+1];
    }

    Complex *eX = DFT8(x);
    Complex *tX = FFT8_Radix2_B(x);

    EXPECT_NEAR(eX[0].R(),tX[0].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[0].I(),tX[0].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[1].R(),tX[1].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[1].I(),tX[1].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[2].R(),tX[2].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[2].I(),tX[2].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[3].R(),tX[3].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[3].I(),tX[3].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[4].R(),tX[4].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[4].I(),tX[4].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[5].R(),tX[5].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[5].I(),tX[5].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[6].R(),tX[6].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[6].I(),tX[6].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[7].R(),tX[7].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[7].I(),tX[7].I(),c_TOLERANCE);

    delete [] eX;
    delete [] tX;
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

template <typename C> class FFTRadix2_C
{
    public:
        FFTRadix2_C(int N);
        virtual ~FFTRadix2_C();

        //C *DFT(C *x);
        C *DFT2(C *x,int index);
        C *DFT4(C *x,int index);
        C *DFT8(C *x,int index);

    protected:

        tint m_N;
        tint *m_reverseIndex;

        void initialize();
        void done();

        tint noBits() const;
        tint getReverseIndex(tint index,tint noBits) const;
};

//-------------------------------------------------------------------------------------------

template <typename C> FFTRadix2_C<C>::FFTRadix2_C(int N) : m_N(N),
    m_reverseIndex(0)
{
    initialize();
}

//-------------------------------------------------------------------------------------------

template <typename C> FFTRadix2_C<C>::~FFTRadix2_C()
{
    done();
}

//-------------------------------------------------------------------------------------------

template <typename C> void FFTRadix2_C<C>::initialize()
{
    tint nBits = noBits();
    m_reverseIndex = new tint [m_N];
    for(tint i=0;i<m_N;i++)
    {
        m_reverseIndex[i] = getReverseIndex(i,nBits);
    }
}

//-------------------------------------------------------------------------------------------

template <typename C> void FFTRadix2_C<C>::done()
{
    if(m_reverseIndex!=0)
    {
        delete [] m_reverseIndex;
        m_reverseIndex = 0;
    }
}

//-------------------------------------------------------------------------------------------

template <typename C> tint FFTRadix2_C<C>::noBits() const
{
    int N,count = 0;

    N = m_N;
    while(N > 1)
    {
        N >>= 1;
        count++;
    }
    return count;
}

//-------------------------------------------------------------------------------------------

template <typename C> tint FFTRadix2_C<C>::getReverseIndex(tint index,tint noBits) const
{
    tuint32 y = static_cast<tuint32>(index), x = 0;

    while(noBits>0)
    {
        x = (x << 1) | (y & 0x00000001);
        y >>= 1;
        noBits--;
    }
    return static_cast<tint>(x);
}

//-------------------------------------------------------------------------------------------

template <typename C> C *FFTRadix2_C<C>::DFT2(C *x,int index)
{
    C *X = new C [2];
    tint idxA = m_reverseIndex[index];
    tint idxB = m_reverseIndex[index+1];
    X[0] = x[idxA] + x[idxB];
    X[1] = x[idxA] - x[idxB];
    return X;
}

//-------------------------------------------------------------------------------------------

template <typename C> C *FFTRadix2_C<C>::DFT4(C *x,int index)
{
    C *X = new Complex[4];
    Complex *F1 = DFT2(x,index);
    Complex *F2 = DFT2(x,index+2);
    Complex F2_1(F2[1].I(),-F2[1].R());
    X[0] = F1[0] + F2[0];
    X[1] = F1[1] + F2_1;
    X[2] = F1[0] - F2[0];
    X[3] = F1[1] - F2_1;
    delete [] F1;
    delete [] F2;
    return X;
}

//-------------------------------------------------------------------------------------------

template <typename C> C *FFTRadix2_C<C>::DFT8(C *x,int index)
{
    tfloat64 c_HalfSquared = sqrt(0.5);

    C *X = new Complex[8];
    Complex *F1 = DFT4(x,index);
    Complex *F2 = DFT4(x,index+4);

    Complex F2_1(c_HalfSquared,-c_HalfSquared);
    F2_1 *= F2[1];
    Complex F2_2(0.0,-1.0);
    F2_2 *= F2[2];
    Complex F2_3(-c_HalfSquared,-c_HalfSquared);
    F2_3 *= F2[3];

    X[0] = F1[0] + F2[0];
    X[1] = F1[1] + F2_1;
    X[2] = F1[2] + F2_2;
    X[3] = F1[3] + F2_3;

    X[4] = F1[0] - F2[0];
    X[5] = F1[1] - F2_1;
    X[6] = F1[2] - F2_2;
    X[7] = F1[3] - F2_3;

    return X;
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT8_Radix2_C)
{
    tfloat64 t[16] = {
        2.3, 1.4, 5.6, 4.3, 4.5, 6.7, 8.1, 2.5,
        3.4, 6.3, 9.2, 4.8, 7.0, 0.3, 3.2, 1.3
    };

    Complex x[8];
    for(int i=0;i<8;i++)
    {
        x[i].R() = t[(i*2)+0];
        x[i].I() = t[(i*2)+1];
    }

    Complex *eX = DFT8(x);

    FFTRadix2_C<Complex> FFT(8);

    Complex *tX = FFT.DFT8(x,0);

    EXPECT_NEAR(eX[0].R(),tX[0].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[0].I(),tX[0].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[1].R(),tX[1].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[1].I(),tX[1].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[2].R(),tX[2].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[2].I(),tX[2].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[3].R(),tX[3].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[3].I(),tX[3].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[4].R(),tX[4].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[4].I(),tX[4].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[5].R(),tX[5].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[5].I(),tX[5].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[6].R(),tX[6].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[6].I(),tX[6].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[7].R(),tX[7].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[7].I(),tX[7].I(),c_TOLERANCE);

    delete [] eX;
    delete [] tX;
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

template <typename C> class FFTRadix2_D
{
    public:
        FFTRadix2_D(int N);
        virtual ~FFTRadix2_D();

        C *DFT(C *x);

    protected:

        tint m_N;
        tint *m_reverseIndex;

        C **m_coefficient;

        void initialize();
        void done();

        tint noBits(tint N) const;
        tint getReverseIndex(tint index,tint noBits) const;

        C *DFT2(C *x,int index);
        C *DFT4(C *x,int index);
        C *DFT8(C *x,int index);
        C *DFTRecursive(C *x,int index,int N);
};

//-------------------------------------------------------------------------------------------

template <typename C> FFTRadix2_D<C>::FFTRadix2_D(int N) : m_N(N),
    m_reverseIndex(0),
    m_coefficient(0)
{
    initialize();
}

//-------------------------------------------------------------------------------------------

template <typename C> FFTRadix2_D<C>::~FFTRadix2_D()
{
    done();
}

//-------------------------------------------------------------------------------------------

template <typename C> void FFTRadix2_D<C>::initialize()
{
    tint nBits = noBits(m_N);
    m_reverseIndex = new tint [m_N];
    for(tint i=0;i<m_N;i++)
    {
        m_reverseIndex[i] = getReverseIndex(i,nBits);
    }

    m_coefficient = reinterpret_cast<C **>(malloc((nBits - 2) * sizeof(C *)));

    tfloat64 halfSquared = sqrt(0.5);
    C *c2 = m_coefficient[0] = new C [3];
    C F2_1(halfSquared,-halfSquared);
    C F2_2(0.0,-1.0);
    C F2_3(-halfSquared,-halfSquared);
    c2[0] = F2_1;
    c2[1] = F2_2;
    c2[2] = F2_3;

    for(tint i=4;i<=nBits;i++)
    {
        tint M = 1 << i;
        tint len = (M >> 1) - 1;
        C *coefficients = new C [len];
        for(tint j=1; j<(M >> 1); j++)
        {
            coefficients[j-1] = C::W(j,M);
        }
        m_coefficient[i-3] = coefficients;
    }
}

//-------------------------------------------------------------------------------------------

template <typename C> void FFTRadix2_D<C>::done()
{
    tint nBits = noBits(m_N);

    if(m_reverseIndex!=0)
    {
        delete [] m_reverseIndex;
        m_reverseIndex = 0;
    }
    if(m_coefficient!=0)
    {
        for(tint i=3;i<=nBits;i++)
        {
            C *c = m_coefficient[i-3];
            delete [] c;
        }
        free(m_coefficient);
        m_coefficient = 0;
    }
}

//-------------------------------------------------------------------------------------------

template <typename C> tint FFTRadix2_D<C>::noBits(tint N) const
{
    tint count = 0;

    while(N > 1)
    {
        N >>= 1;
        count++;
    }
    return count;
}

//-------------------------------------------------------------------------------------------

template <typename C> tint FFTRadix2_D<C>::getReverseIndex(tint index,tint noBits) const
{
    tuint32 y = static_cast<tuint32>(index), x = 0;

    while(noBits>0)
    {
        x = (x << 1) | (y & 0x00000001);
        y >>= 1;
        noBits--;
    }
    return static_cast<tint>(x);
}

//-------------------------------------------------------------------------------------------

template <typename C> C *FFTRadix2_D<C>::DFT2(C *x,int index)
{
    C *X = new C [2];
    tint idxA = m_reverseIndex[index];
    tint idxB = m_reverseIndex[index+1];
    X[0] = x[idxA] + x[idxB];
    X[1] = x[idxA] - x[idxB];
    return X;
}

//-------------------------------------------------------------------------------------------

template <typename C> C *FFTRadix2_D<C>::DFT4(C *x,int index)
{
    C *X = new C [4];

    C *F1 = DFT2(x,index);
    C *F2 = DFT2(x,index+2);
    C F2_1(F2[1].I(),-F2[1].R());

    X[0] = F1[0] + F2[0];
    X[1] = F1[1] + F2_1;
    X[2] = F1[0] - F2[0];
    X[3] = F1[1] - F2_1;

    delete [] F1;
    delete [] F2;

    return X;
}

//-------------------------------------------------------------------------------------------

template <typename C> C *FFTRadix2_D<C>::DFT8(C *x,int index)
{
    C *coefficients = m_coefficient[0];

    C *X = new C [8];
    C *F1 = DFT4(x,index);
    C *F2 = DFT4(x,index+4);

    F2[1] = coefficients[0] * F2[1];
    F2[2] = coefficients[1] * F2[2];
    F2[3] = coefficients[2] * F2[3];

    X[0] = F1[0] + F2[0];
    X[1] = F1[1] + F2[1];
    X[2] = F1[2] + F2[2];
    X[3] = F1[3] + F2[3];

    X[4] = F1[0] - F2[0];
    X[5] = F1[1] - F2[1];
    X[6] = F1[2] - F2[2];
    X[7] = F1[3] - F2[3];

    delete [] F1;
    delete [] F2;

    return X;
}

//-------------------------------------------------------------------------------------------

template <typename C> C *FFTRadix2_D<C>::DFTRecursive(C *x,int index,int N)
{
    C *coefficients = m_coefficient[noBits(N) - 3];

    tint i;
    tint halfN = N >> 1;
    C *X = new C [N];
    C *Y = &X[halfN];
    C *F1 = (halfN != 8) ? DFTRecursive(x,index,halfN) : DFT8(x,index);
    C *F2 = (halfN != 8) ? DFTRecursive(x,index+halfN,halfN) : DFT8(x,index+halfN);

    for(i=1;i<halfN;i++)
    {
        F2[i] = coefficients[i-1] * F2[i];
    }

    for(i=0;i<halfN;i++)
    {
        X[i] = F1[i] + F2[i];
        Y[i] = F1[i] - F2[i];
    }

    return X;
}

//-------------------------------------------------------------------------------------------

template <typename C> C *FFTRadix2_D<C>::DFT(C *x)
{
    return DFTRecursive(x,0,m_N);
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT16_Radix2_D)
{
    tfloat64 t[32] = {
        2.3, 1.4, 5.6, 4.3, 4.5, 6.7, 8.1, 2.5,
        3.4, 6.3, 9.2, 4.8, 7.0, 0.3, 3.2, 1.3,
        4.3, 5.6, 1.2, 0.9, 8.3, 4.5, 2.4, 2.5,
        9.8, 6.5, 3.4, 3.9, 1.3, 2.0, 5.4, 3.4
    };

    Complex x[16];
    for(int i=0;i<16;i++)
    {
        x[i].R() = t[(i*2)+0];
        x[i].I() = t[(i*2)+1];
    }

    Complex *eX = DFT16(x);

    FFTRadix2_D<Complex> FFT(16);

    Complex *tX = FFT.DFT(x);

    EXPECT_NEAR(eX[0].R(),tX[0].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[0].I(),tX[0].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[1].R(),tX[1].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[1].I(),tX[1].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[2].R(),tX[2].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[2].I(),tX[2].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[3].R(),tX[3].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[3].I(),tX[3].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[4].R(),tX[4].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[4].I(),tX[4].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[5].R(),tX[5].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[5].I(),tX[5].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[6].R(),tX[6].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[6].I(),tX[6].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[7].R(),tX[7].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[7].I(),tX[7].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[8].R(),tX[8].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[8].I(),tX[8].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[9].R(),tX[9].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[9].I(),tX[9].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[10].R(),tX[10].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[10].I(),tX[10].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[11].R(),tX[11].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[11].I(),tX[11].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[12].R(),tX[12].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[12].I(),tX[12].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[13].R(),tX[13].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[13].I(),tX[13].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[14].R(),tX[14].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[14].I(),tX[14].I(),c_TOLERANCE);

    EXPECT_NEAR(eX[15].R(),tX[15].R(),c_TOLERANCE);
    EXPECT_NEAR(eX[15].I(),tX[15].I(),c_TOLERANCE);

    delete [] eX;
    delete [] tX;
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT32_Radix2_D)
{
    const tint c_N = 32;

    common::Random *rand = common::Random::instance();
    rand->seed(0);

    Complex *x = new Complex [c_N];
    for(tint i=0;i<c_N;i++)
    {
        x[i].R() = rand->randomReal1();
        x[i].I() = rand->randomReal1();
    }

    Complex *eX = DFT_N(x,c_N);

    FFTRadix2_D<Complex> FFT(c_N);

    Complex *tX = FFT.DFT(x);

    for(tint i=0;i<c_N;i++)
    {
        EXPECT_NEAR(eX[i].R(),tX[i].R(),c_TOLERANCE);
        EXPECT_NEAR(eX[i].I(),tX[i].I(),c_TOLERANCE);
    }

    delete [] eX;
    delete [] tX;
    delete [] x;
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT2048_Radix2_D)
{
    const tint c_N = 2048;

    common::Random *rand = common::Random::instance();
    rand->seed(0);

    Complex *x = new Complex [c_N];
    for(tint i=0;i<c_N;i++)
    {
        x[i].R() = rand->randomReal1();
        x[i].I() = rand->randomReal1();
    }

    Complex *eX = DFT_N(x,c_N);

    FFTRadix2_D<Complex> FFT(c_N);

    Complex *tX = FFT.DFT(x);

    for(tint i=0;i<c_N;i++)
    {
        EXPECT_NEAR(eX[i].R(),tX[i].R(),c_TOLERANCE);
        EXPECT_NEAR(eX[i].I(),tX[i].I(),c_TOLERANCE);
    }

    delete [] eX;
    delete [] tX;
    delete [] x;
}

//-------------------------------------------------------------------------------------------

TEST(FFTRadix2,FFT4096_Radix2_D)
{
    const tint c_N = 4096;

    common::Random *rand = common::Random::instance();
    rand->seed(0);

    Complex *x = new Complex [c_N];
    for(tint i=0;i<c_N;i++)
    {
        x[i].R() = rand->randomReal1();
        x[i].I() = rand->randomReal1();
    }

    Complex *eX = DFT_N(x,c_N);

    FFTRadix2_D<Complex> FFT(c_N);
    Complex *tX = FFT.DFT(x);

    for(tint i=0;i<c_N;i++)
    {
        EXPECT_NEAR(eX[i].R(),tX[i].R(),c_TOLERANCE);
        EXPECT_NEAR(eX[i].I(),tX[i].I(),c_TOLERANCE);
    }

    delete [] eX;
    delete [] tX;
    delete [] x;
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

template <typename V> class FFTRadix2_E
{
    public:
        FFTRadix2_E(int N);
        virtual ~FFTRadix2_E();

        V *DFT(V *x);

    protected:

        tint m_N;
        tint *m_reverseIndex;

        V **m_coefficient;

        void initialize();
        void done();

        tint noBits(tint N) const;
        tint getReverseIndex(tint index,tint noBits) const;

        V complexMultiplyReal(const V *X,const V *Y) const;
        V complexMultiplyImaginary(const V *X,const V *Y) const;


        V *DFT2(V *x,int index);
        V *DFT4(V *x,int index);
        V *DFT8(V *x,int index);
        V *DFTRecursive(V *x,int index,int N);
};

//-------------------------------------------------------------------------------------------

template <typename V> FFTRadix2_E<V>::FFTRadix2_E(int N) : m_N(N),
    m_reverseIndex(0),
    m_coefficient(0)
{
    initialize();
}

//-------------------------------------------------------------------------------------------

template <typename V> FFTRadix2_E<V>::~FFTRadix2_E()
{
    done();
}

//-------------------------------------------------------------------------------------------

template <typename V> void FFTRadix2_E<V>::initialize()
{
    tint nBits = noBits(m_N);
    m_reverseIndex = new tint [m_N];
    for(tint i=0;i<m_N;i++)
    {
        m_reverseIndex[i] = getReverseIndex(i,nBits);
    }

    m_coefficient = reinterpret_cast<V **>(malloc((nBits - 2) * sizeof(V *)));

    tfloat64 halfSquared = sqrt(0.5);
    V *c2 = new V [6];
    c2[0] = halfSquared;
    c2[1] = -halfSquared;
    c2[2] = 0.0;
    c2[3] = -1.0;
    c2[4] = -halfSquared;
    c2[5] = -halfSquared;
    m_coefficient[0] = c2;

    for(tint i=4;i<=nBits;i++)
    {
        tint M = 1 << i;
        tint len = (M >> 1) - 1;
        V *coefficients = new V [len << 1];
        for(tint j=1; j<(M >> 1); j++)
        {
            tfloat64 angle = (2.0 * c_PI_D * static_cast<tfloat64>(j)) / static_cast<tfloat64>(M);
            coefficients[((j-1)<<1)+0] = cos(angle);
            coefficients[((j-1)<<1)+1] = 0.0 - sin(angle);
        }
        m_coefficient[i-3] = coefficients;
    }
}

//-------------------------------------------------------------------------------------------

template <typename V> void FFTRadix2_E<V>::done()
{
    tint nBits = noBits(m_N);

    if(m_reverseIndex!=0)
    {
        delete [] m_reverseIndex;
        m_reverseIndex = 0;
    }
    if(m_coefficient!=0)
    {
        for(tint i=3;i<=nBits;i++)
        {
            V *c = m_coefficient[i-3];
            delete [] c;
        }
        free(m_coefficient);
        m_coefficient = 0;
    }
}

//-------------------------------------------------------------------------------------------

template <typename V> tint FFTRadix2_E<V>::noBits(tint N) const
{
    tint count = 0;

    while(N > 1)
    {
        N >>= 1;
        count++;
    }
    return count;
}

//-------------------------------------------------------------------------------------------

template <typename V> tint FFTRadix2_E<V>::getReverseIndex(tint index,tint noBits) const
{
    tuint32 y = static_cast<tuint32>(index), x = 0;

    while(noBits>0)
    {
        x = (x << 1) | (y & 0x00000001);
        y >>= 1;
        noBits--;
    }
    return static_cast<tint>(x << 1);
}

//-------------------------------------------------------------------------------------------

template <typename V> V FFTRadix2_E<V>::complexMultiplyReal(const V *X,const V *Y) const
{
    return (X[0] * Y[0]) - (X[1] * Y[1]);
}

//-------------------------------------------------------------------------------------------

template <typename V> V FFTRadix2_E<V>::complexMultiplyImaginary(const V *X,const V *Y) const
{
    return (X[0] * Y[1]) + (X[1] * Y[0]);
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2_E<V>::DFT2(V *x,int index)
{
    V *X = new V [4];
    tint idxA = m_reverseIndex[index];
    tint idxB = m_reverseIndex[index+1];
    X[0] = x[idxA] + x[idxB];
    X[1] = x[idxA+1] + x[idxB+1];
    X[2] = x[idxA] - x[idxB];
    X[3] = x[idxA+1] - x[idxB+1];
    return X;
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2_E<V>::DFT4(V *x,int index)
{
    V *X = new V [8];

    V *F1 = DFT2(x,index);
    V *F2 = DFT2(x,index+2);

    X[0] = F1[0] + F2[0];
    X[1] = F1[1] + F2[1];
    X[2] = F1[2] + F2[3];
    X[3] = F1[3] - F2[2];
    X[4] = F1[0] - F2[0];
    X[5] = F1[1] - F2[1];
    X[6] = F1[2] - F2[3];
    X[7] = F1[3] + F2[2];

    delete [] F1;
    delete [] F2;

    return X;
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2_E<V>::DFT8(V *x,int index)
{
    V *coefficients = m_coefficient[0];

    V t;
    V *X = new V [16];
    V *F1 = DFT4(x,index);
    V *F2 = DFT4(x,index+4);

    t = complexMultiplyReal(&coefficients[0],&F2[2]);
    F2[3] = complexMultiplyImaginary(&coefficients[0],&F2[2]);
    F2[2] = t;

    t = complexMultiplyReal(&coefficients[2],&F2[4]);
    F2[5] = complexMultiplyImaginary(&coefficients[2],&F2[4]);
    F2[4] = t;

    t = complexMultiplyReal(&coefficients[4],&F2[6]);
    F2[7] = complexMultiplyImaginary(&coefficients[4],&F2[6]);
    F2[6] = t;

    X[ 0] = F1[0] + F2[0];
    X[ 1] = F1[1] + F2[1];
    X[ 2] = F1[2] + F2[2];
    X[ 3] = F1[3] + F2[3];
    X[ 4] = F1[4] + F2[4];
    X[ 5] = F1[5] + F2[5];
    X[ 6] = F1[6] + F2[6];
    X[ 7] = F1[7] + F2[7];

    X[ 8] = F1[0] - F2[0];
    X[ 9] = F1[1] - F2[1];
    X[10] = F1[2] - F2[2];
    X[11] = F1[3] - F2[3];
    X[12] = F1[4] - F2[4];
    X[13] = F1[5] - F2[5];
    X[14] = F1[6] - F2[6];
    X[15] = F1[7] - F2[7];

    delete [] F1;
    delete [] F2;

    return X;
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2_E<V>::DFTRecursive(V *x,int index,int N)
{
    V *coefficients = m_coefficient[noBits(N) - 3];

    tint i;
    tint halfN = N >> 1;
    V *X = new V [N << 1];
    V *Y = &X[N];
    V *F1 = (halfN != 8) ? DFTRecursive(x,index,halfN) : DFT8(x,index);
    V *F2 = (halfN != 8) ? DFTRecursive(x,index+halfN,halfN) : DFT8(x,index+halfN);

    for(i=1;i<halfN;i++)
    {
        tint idxA = (i-1) << 1;
        tint idxB = i << 1;
        tfloat64 t = complexMultiplyReal(&coefficients[idxA],&F2[idxB]);
        F2[idxB+1] = complexMultiplyImaginary(&coefficients[idxA],&F2[idxB]);
        F2[idxB] = t;
    }

    for(i=0;i<N;i++)
    {
        X[i] = F1[i] + F2[i];
        Y[i] = F1[i] - F2[i];
    }

    return X;
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2_E<V>::DFT(V *x)
{
    return DFTRecursive(x,0,m_N);
}

//-------------------------------------------------------------------------------------------

void FFTRadix2_E_Test(tint c_N)
{
    common::Random *rand = common::Random::instance();
    rand->seed(0);

    Complex *x = new Complex [c_N];
    tfloat64 *xD = new tfloat64 [c_N * 2];
    for(tint i=0;i<c_N;i++)
    {
        xD[(i<<1)+0] = x[i].R() = rand->randomReal1();
        xD[(i<<1)+1] = x[i].I() = rand->randomReal1();
    }

    Complex *eX = DFT_N(x,c_N);

    FFTRadix2_E<tfloat64> FFT(c_N);

    tfloat64 *tX = FFT.DFT(xD);

    for(tint i=0;i<c_N;i++)
    {
        EXPECT_NEAR(eX[i].R(),tX[(i<<1)+0],c_TOLERANCE);
        EXPECT_NEAR(eX[i].I(),tX[(i<<1)+1],c_TOLERANCE);
    }

    delete [] eX;
    delete [] tX;
    delete [] x;
}

TEST(FFTRadix2,FFT16_Radix2_E)
{
    FFTRadix2_E_Test(16);
}

TEST(FFTRadix2,FFT32_Radix2_E)
{
    FFTRadix2_E_Test(32);
}

TEST(FFTRadix2,FFT64_Radix2_E)
{
    FFTRadix2_E_Test(64);
}

TEST(FFTRadix2,FFT128_Radix2_E)
{
    FFTRadix2_E_Test(128);
}

TEST(FFTRadix2,FFT256_Radix2_E)
{
    FFTRadix2_E_Test(256);
}

TEST(FFTRadix2,FFT512_Radix2_E)
{
    FFTRadix2_E_Test(512);
}

TEST(FFTRadix2,FFT1024_Radix2_E)
{
    FFTRadix2_E_Test(1024);
}

TEST(FFTRadix2,FFT2048_Radix2_E)
{
    FFTRadix2_E_Test(2048);
}

TEST(FFTRadix2,FFT4096_Radix2_E)
{
    FFTRadix2_E_Test(4096);
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

template <typename V> class FFTRadix2_F
{
    public:
        FFTRadix2_F(int N);
        virtual ~FFTRadix2_F();

        V *DFT(V *x);

    protected:

        tint m_N;
        tint *m_reverseIndex;

        V **m_coefficient;
        V **m_stackA;
        V **m_stackB;

        void initialize();
        void done();

        tint noBits(tint N) const;
        tint getReverseIndex(tint index,tint noBits) const;

        V complexMultiplyReal(const V *X,const V *Y) const;
        V complexMultiplyImaginary(const V *X,const V *Y) const;

        V *DFT4(V *x,int index,V *X);
        V *DFT8(V *x,int index,bool sFlag);
        V *DFTRecursive(V *x,int index,int N,bool sFlag);
};

//-------------------------------------------------------------------------------------------

template <typename V> FFTRadix2_F<V>::FFTRadix2_F(int N) : m_N(N),
    m_reverseIndex(0),
    m_coefficient(0),
    m_stackA(0),
    m_stackB(0)
{
    initialize();
}

//-------------------------------------------------------------------------------------------

template <typename V> FFTRadix2_F<V>::~FFTRadix2_F()
{
    done();
}

//-------------------------------------------------------------------------------------------

template <typename V> void FFTRadix2_F<V>::initialize()
{
    tint nBits = noBits(m_N);
    m_reverseIndex = new tint [m_N];
    for(tint i=0;i<m_N;i++)
    {
        m_reverseIndex[i] = getReverseIndex(i,nBits);
    }

    m_coefficient = reinterpret_cast<V **>(malloc((nBits - 2) * sizeof(V *)));
    m_stackA = reinterpret_cast<V **>(malloc((nBits - 2) * sizeof(V *)));
    m_stackB = reinterpret_cast<V **>(malloc((nBits - 3) * sizeof(V *)));

    tfloat64 halfSquared = sqrt(0.5);
    V *c2 = new V [4];
    c2[0] =  halfSquared;
    c2[1] = -halfSquared;
    c2[2] = -halfSquared;
    c2[3] = -halfSquared;
    m_coefficient[0] = c2;
    m_stackA[0] = new V [32];
    m_stackB[0] = new V [32];

    for(tint i=4;i<=nBits;i++)
    {
        tint M = 1 << i;
        tint len = (M >> 1) - 1;
        V *coefficients = new V [len << 1];
        for(tint j=1; j<(M >> 1); j++)
        {
            tfloat64 angle = (2.0 * c_PI_D * static_cast<tfloat64>(j)) / static_cast<tfloat64>(M);
            coefficients[((j-1)<<1)+0] = static_cast<V>(cos(angle));
            coefficients[((j-1)<<1)+1] = static_cast<V>(0.0 - sin(angle));
        }
        m_coefficient[i-3] = coefficients;
        if(i<nBits)
        {
            m_stackA[i-3] = new V [M<<1];
            m_stackB[i-3] = new V [M<<1];
        }
    }
}

//-------------------------------------------------------------------------------------------

template <typename V> void FFTRadix2_F<V>::done()
{
    tint nBits = noBits(m_N);

    if(m_reverseIndex!=0)
    {
        delete [] m_reverseIndex;
        m_reverseIndex = 0;
    }
    if(m_coefficient!=0)
    {
        for(tint i=3;i<=nBits;i++)
        {
            V *c = m_coefficient[i-3];
            delete [] c;
            if(i<nBits)
            {
                V *sA = m_stackA[i-3];
                delete [] sA;
                V *sB = m_stackB[i-3];
                delete [] sB;
            }
        }
        free(m_coefficient);
        m_coefficient = 0;
        free(m_stackA);
        m_stackA = 0;
        free(m_stackB);
        m_stackB = 0;
    }
}

//-------------------------------------------------------------------------------------------

template <typename V> tint FFTRadix2_F<V>::noBits(tint N) const
{
    tint count = 0;

    while(N > 1)
    {
        N >>= 1;
        count++;
    }
    return count;
}

//-------------------------------------------------------------------------------------------

template <typename V> tint FFTRadix2_F<V>::getReverseIndex(tint index,tint noBits) const
{
    tuint32 y = static_cast<tuint32>(index), x = 0;

    while(noBits>0)
    {
        x = (x << 1) | (y & 0x00000001);
        y >>= 1;
        noBits--;
    }
    return static_cast<tint>(x << 1);
}

//-------------------------------------------------------------------------------------------

template <typename V> V FFTRadix2_F<V>::complexMultiplyReal(const V *X,const V *Y) const
{
    return (X[0] * Y[0]) - (X[1] * Y[1]);
}

//-------------------------------------------------------------------------------------------

template <typename V> V FFTRadix2_F<V>::complexMultiplyImaginary(const V *X,const V *Y) const
{
    return (X[0] * Y[1]) + (X[1] * Y[0]);
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2_F<V>::DFT4(V *x,int index,V *X)
{
    tint idxA,idxB,idxC,idxD;
    V F1[4],F2[4];

    idxA = m_reverseIndex[index+0];
    idxB = m_reverseIndex[index+1];
    idxC = m_reverseIndex[index+2];
    idxD = m_reverseIndex[index+3];

    F1[0] = x[idxA] + x[idxB];
    F1[1] = x[idxA+1] + x[idxB+1];
    F1[2] = x[idxA] - x[idxB];
    F1[3] = x[idxA+1] - x[idxB+1];

    F2[0] = x[idxC] + x[idxD];
    F2[1] = x[idxC+1] + x[idxD+1];
    F2[2] = x[idxC] - x[idxD];
    F2[3] = x[idxC+1] - x[idxD+1];

    X[0] = F1[0] + F2[0];
    X[1] = F1[1] + F2[1];
    X[2] = F1[2] + F2[3];
    X[3] = F1[3] - F2[2];
    X[4] = F1[0] - F2[0];
    X[5] = F1[1] - F2[1];
    X[6] = F1[2] - F2[3];
    X[7] = F1[3] + F2[2];

    return X;
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2_F<V>::DFT8(V *x,int index,bool sFlag)
{
    V *coefficients = m_coefficient[0];
    V *X = (sFlag) ? m_stackA[0] : m_stackB[0];

    V t;
    V *F1 = DFT4(x,index,&X[16]);
    V *F2 = DFT4(x,index+4,&X[24]);

    t = complexMultiplyReal(&coefficients[0],&F2[2]);
    F2[3] = complexMultiplyImaginary(&coefficients[0],&F2[2]);
    F2[2] = t;

    t = complexMultiplyReal(&coefficients[2],&F2[6]);
    F2[7] = complexMultiplyImaginary(&coefficients[2],&F2[6]);
    F2[6] = t;

    X[ 0] = F1[0] + F2[0];
    X[ 1] = F1[1] + F2[1];
    X[ 2] = F1[2] + F2[2];
    X[ 3] = F1[3] + F2[3];
    X[ 4] = F1[4] + F2[5];
    X[ 5] = F1[5] - F2[4];
    X[ 6] = F1[6] + F2[6];
    X[ 7] = F1[7] + F2[7];

    X[ 8] = F1[0] - F2[0];
    X[ 9] = F1[1] - F2[1];
    X[10] = F1[2] - F2[2];
    X[11] = F1[3] - F2[3];
    X[12] = F1[4] - F2[5];
    X[13] = F1[5] + F2[4];
    X[14] = F1[6] - F2[6];
    X[15] = F1[7] - F2[7];

    return X;
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2_F<V>::DFTRecursive(V *x,int index,int N,bool sFlag)
{
    tint i;
    tint halfN = N >> 1;
    tint bitIndex = noBits(N) - 3;
    V *coefficients = m_coefficient[bitIndex];
    V *X;

    if(N==m_N)
    {
        X = new V [N << 1];
    }
    else
    {
        X = (sFlag) ? m_stackA[bitIndex] : m_stackB[bitIndex];
    }

    V *Y = &X[N];
    V *F1 = (halfN != 8) ? DFTRecursive(x,index,halfN,true) : DFT8(x,index,true);
    V *F2 = (halfN != 8) ? DFTRecursive(x,index+halfN,halfN,false) : DFT8(x,index+halfN,false);

    for(i=1;i<halfN;i++)
    {
        tint idxA = (i-1) << 1;
        tint idxB = i << 1;
        tfloat64 t = complexMultiplyReal(&coefficients[idxA],&F2[idxB]);
        F2[idxB+1] = complexMultiplyImaginary(&coefficients[idxA],&F2[idxB]);
        F2[idxB] = t;
    }

    for(i=0;i<N;i++)
    {
        X[i] = F1[i] + F2[i];
        Y[i] = F1[i] - F2[i];
    }

    return X;
}

//-------------------------------------------------------------------------------------------

template <typename V> V *FFTRadix2_F<V>::DFT(V *x)
{
    return DFTRecursive(x,0,m_N,false);
}

//-------------------------------------------------------------------------------------------

void FFTRadix2_F_Test(tint c_N)
{
    common::Random *rand = common::Random::instance();
    rand->seed(0);

    Complex *x = new Complex [c_N];
    tfloat64 *xD = new tfloat64 [c_N * 2];
    for(tint i=0;i<c_N;i++)
    {
        xD[(i<<1)+0] = x[i].R() = rand->randomReal1();
        xD[(i<<1)+1] = x[i].I() = rand->randomReal1();
    }

    Complex *eX = DFT_N(x,c_N);

    FFTRadix2_F<tfloat64> FFT(c_N);

    tfloat64 *tX = FFT.DFT(xD);

    for(tint i=0;i<c_N;i++)
    {
        EXPECT_NEAR(eX[i].R(),tX[(i<<1)+0],c_TOLERANCE);
        EXPECT_NEAR(eX[i].I(),tX[(i<<1)+1],c_TOLERANCE);
    }

    delete [] eX;
    delete [] tX;
    delete [] x;
}

TEST(FFTRadix2,FFT16_Radix2_F)
{
    FFTRadix2_F_Test(16);
}

TEST(FFTRadix2,FFT32_Radix2_F)
{
    FFTRadix2_F_Test(32);
}

TEST(FFTRadix2,FFT64_Radix2_F)
{
    FFTRadix2_F_Test(64);
}

TEST(FFTRadix2,FFT128_Radix2_F)
{
    FFTRadix2_F_Test(128);
}

TEST(FFTRadix2,FFT256_Radix2_F)
{
    FFTRadix2_F_Test(256);
}

TEST(FFTRadix2,FFT512_Radix2_F)
{
    FFTRadix2_F_Test(512);
}

TEST(FFTRadix2,FFT1024_Radix2_F)
{
    FFTRadix2_F_Test(1024);
}

TEST(FFTRadix2,FFT2048_Radix2_F)
{
    FFTRadix2_F_Test(2048);
}

TEST(FFTRadix2,FFT4096_Radix2_F)
{
    FFTRadix2_F_Test(4096);
}

//-------------------------------------------------------------------------------------------