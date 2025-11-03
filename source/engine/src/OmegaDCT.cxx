#include "engine/inc/OmegaDCT.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------
// DCT Coefficients used by Intel SIMD assembly
//-------------------------------------------------------------------------------------------

#define D_DCTCoefficients_IntelSIMD_Size 56

#if defined(OMEGA_INTEL)

const tfloat32 c_DCTCoefficients_IntelSIMD[ D_DCTCoefficients_IntelSIMD_Size ] = {
    1.0f, D_Cos_1Pi4, D_Cos_1Pi8,  D_Cos_3Pi8,                //  0 - 4
    1.0f, D_Cos_3Pi4, D_Cos_3Pi8, -D_Cos_1Pi8,                // 16 - 8
    0.0f, 0.0f, 0.0f, 0.0f,                                    // 32 - 12
    D_Cos_1Pi8, D_Cos_3Pi8, 1.0f, 1.0f,                        // 48 - 16
    D_Cos_3Pi8, -D_Cos_1Pi8, D_Cos_1Pi4, -D_Cos_1Pi4,         // 64 - 20
    D_Delta_0Pi4, D_Delta_1Pi4, D_Delta_2Pi4, D_Delta_3Pi4,    // 80 - 24
    D_Delta_0Pi8, D_Delta_1Pi8, D_Delta_2Pi8, D_Delta_3Pi8,    // 96 - 28
    D_Delta_4Pi8, D_Delta_5Pi8, D_Delta_6Pi8, D_Delta_7Pi8,    // 112 - 32
    D_Cos_1Pi8, D_Cos_3Pi8, D_Cos_1Pi4, -D_Cos_1Pi4,        // 128 - 36
    D_Cos_3Pi8, -D_Cos_1Pi8, 1.0f, 1.0f,                    // 144 - 40
    D_Delta_0Pi16, D_Delta_1Pi16, D_Delta_2Pi16, D_Delta_3Pi16,    // 160 - 44
    D_Delta_4Pi16, D_Delta_5Pi16, D_Delta_6Pi16, D_Delta_7Pi16,    // 176 - 48
    D_Delta_8Pi16, D_Delta_9Pi16, D_Delta_10Pi16, D_Delta_11Pi16,    // 192 - 52
    D_Delta_12Pi16, D_Delta_13Pi16, D_Delta_14Pi16, D_Delta_15Pi16    // 208 - 56
};

//-------------------------------------------------------------------------------------------
// DCT bitwise masks used by Intel SIMD assembly
//-------------------------------------------------------------------------------------------

#define D_DCTMasks_IntelSIMD_Size 16

const tuint32 c_DCTMasks_IntelSIMD[ D_DCTMasks_IntelSIMD_Size ] = {
    0x00000000, 0x00000000, 0x80000000, 0x80000000,            //  0 - 4
    0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,            // 16 - 8
    0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF,            // 32 - 12
    0x00000000, 0x00000000, 0x00000000, 0x00000000             // 48 - 16
};

#endif

//-------------------------------------------------------------------------------------------

tint OmegaDCT::m_DCTCounter = 0;
common::Allocation OmegaDCT::m_DCTAllocation;

tfloat32 *OmegaDCT::m_DCTCoefficients_IntelSIMD = 0;
tuint32 *OmegaDCT::m_DCTMasks_IntelSIMD = 0;

//-------------------------------------------------------------------------------------------
// Query x86 CPU if it supports the Intel SIMD SSE instruction set
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_INTEL)
//-------------------------------------------------------------------------------------------

bool OmegaDCT::isIntelSIMD()
{
    int res = 0;

    try
    {
        __asm
        {
            pushad
            mov        eax , 1
            CPUID
            test    edx , 0x02000000
            jnz        DCTb_1_1
            jmp        DCTb_1_2
DCTb_1_1:    mov        res , 1
DCTb_1_2:    popad
        }
    }
    catch(...)
    {
        res = 0;
    }
    return (res) ? true : false;
}

//-------------------------------------------------------------------------------------------
// Initialize coefficient and mask arrays to use 16-byte aligned memory locations.
//-------------------------------------------------------------------------------------------

void OmegaDCT::startIntelSIMD()
{
    if(m_DCTCounter==0)
    {
        tint i;

        m_DCTCoefficients_IntelSIMD = reinterpret_cast<tfloat32 *>(m_DCTAllocation.MemAllocAlign(D_DCTCoefficients_IntelSIMD_Size,sizeof(tfloat32),16));
        for(i=0;i<D_DCTCoefficients_IntelSIMD_Size;++i)
        {
            m_DCTCoefficients_IntelSIMD[i] = c_DCTCoefficients_IntelSIMD[i];
        }

        m_DCTMasks_IntelSIMD = reinterpret_cast<tuint32 *>(m_DCTAllocation.MemAllocAlign(D_DCTMasks_IntelSIMD_Size,sizeof(tuint32),16));
        for(i=0;i<D_DCTMasks_IntelSIMD_Size;++i)
        {
            m_DCTMasks_IntelSIMD[i] = c_DCTMasks_IntelSIMD[i];
        }
    }
    m_DCTCounter++;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::finishIntelSIMD()
{
    m_DCTCounter--;
    if(m_DCTCounter==0)
    {
        if(m_DCTCoefficients_IntelSIMD!=0)
        {
            m_DCTAllocation.Free(m_DCTCoefficients_IntelSIMD);
            m_DCTCoefficients_IntelSIMD = 0;
        }
        if(m_DCTMasks_IntelSIMD!=0)
        {
            m_DCTAllocation.Free(m_DCTMasks_IntelSIMD);
            m_DCTMasks_IntelSIMD = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

QMap<tint,OmegaDCT *> OmegaDCT::m_DCTCollection;

//-------------------------------------------------------------------------------------------

OmegaDCT *OmegaDCT::get(tint N)
{
    OmegaDCT *DCT;
    QMap<tint,OmegaDCT *>::iterator ppI;

    ppI = m_DCTCollection.find(N);
    if(ppI!=m_DCTCollection.end())
    {
        DCT = ppI.value();
    }
    else
    {
        DCT = new OmegaDCT(N);
        m_DCTCollection.insert(N,DCT);
    }
    return DCT;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::stop()
{
    OmegaDCT *DCT;
    QMap<tint,OmegaDCT *>::iterator ppI;

    for(ppI=m_DCTCollection.begin();ppI!=m_DCTCollection.end();++ppI)
    {
        DCT = ppI.value();
        delete DCT;
    }
    m_DCTCollection.clear();
}

//-------------------------------------------------------------------------------------------
// DCT
//-------------------------------------------------------------------------------------------

OmegaDCT::OmegaDCT(int N) : m_alloc(),
    m_isSIMD(false),
    m_N(N),
    m_x(0),
    m_X(0),
    m_Y(0),
    m_offsetY(0),
    m_D4FactorArray(0)
{
    init();

#if defined(OMEGA_INTEL)
    m_isSIMD = isIntelSIMD();

    if(m_isSIMD)
    {
        startIntelSIMD();
    }
#endif

    m_halfN = 2.0f / static_cast<tfloat32>(m_N);
}

//-------------------------------------------------------------------------------------------

OmegaDCT::~OmegaDCT()
{
    try
    {
#if defined(OMEGA_INTEL)
        if(m_isSIMD)
        {
            finishIntelSIMD();
        }
#endif
        free();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

tfloat64 OmegaDCT::dctD4Factor(int k,int N) const
{
    tfloat64 v,t,tA,tB;
    const tfloat64 c_PI64 = 3.141592653589793238464264338832795f;

    tA = c_PI64 / static_cast<tfloat64>(4 * N);
    tB = static_cast<tfloat64>((2 * k) + 1);
    t = tA * tB;
    v = 1.0 / (2.0 * cos(t));
    return v;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::init()
{
    tint i,k,N = 32,lN = mod2(),s = 0;

    m_D4FactorArray = reinterpret_cast<tfloat32 **>(m_alloc.MemAlloc(static_cast<unsigned int>(lN - 4),sizeof(tfloat32 *)));
    for(i=5;i<=lN;++i)
    {
        tfloat32 *f;

        N = static_cast<tint>(pow(2.0,i));
        f = reinterpret_cast<tfloat32 *>(m_alloc.MemAllocAlign(static_cast<unsigned int>(N),sizeof(tfloat32),16));
        for(k=0;k<N;++k)
        {
            f[k] = static_cast<tfloat32>(dctD4Factor(k,N));
        }
        m_D4FactorArray[i - 5] = f;

        s += N << 1;
    }

    m_Y = reinterpret_cast<tfloat32 *>(m_alloc.MemAllocAlign(static_cast<unsigned int>(s + 128),sizeof(tfloat32),16));
    m_offsetY = 0;

    m_x = reinterpret_cast<tfloat32 *>(m_alloc.MemAllocAlign(static_cast<unsigned int>(N),sizeof(tfloat32),16));
    m_X = reinterpret_cast<tfloat32 *>(m_alloc.MemAllocAlign(static_cast<unsigned int>(N),sizeof(tfloat32),16));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::free()
{
    tint i,lN = mod2();

    if(m_D4FactorArray!=0)
    {
        for(i=5;i<=lN;++i)
        {
            m_alloc.Free(m_D4FactorArray[i - 5]);
        }
        m_alloc.Free(m_D4FactorArray);
        m_D4FactorArray = 0;
    }
    if(m_Y!=0)
    {
        m_alloc.Free(m_Y);
        m_Y = 0;
    }
    if(m_x!=0)
    {
        m_alloc.Free(m_x);
        m_x = 0;
    }
    if(m_X!=0)
    {
        m_alloc.Free(m_X);
        m_X = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool OmegaDCT::isMod2() const
{
    return isMod2(m_N);
}

//-------------------------------------------------------------------------------------------

bool OmegaDCT::isMod2(int N) const
{
    return (mod2(N)>=0);
}

//-------------------------------------------------------------------------------------------

int OmegaDCT::mod2() const
{
    return mod2(m_N);
}

//-------------------------------------------------------------------------------------------

int OmegaDCT::mod2(int N) const
{
    static tuint32 mask[32] = {
        0x00000001, 0x00000002, 0x00000004, 0x00000008,
        0x00000010, 0x00000020, 0x00000040, 0x00000080,
        0x00000100, 0x00000200, 0x00000400, 0x00000800,
        0x00001000, 0x00002000, 0x00004000, 0x00008000,
        0x00010000, 0x00020000, 0x00040000, 0x00080000,
        0x00100000, 0x00200000, 0x00400000, 0x00800000,
        0x01000000, 0x02000000, 0x04000000, 0x08000000,
        0x10000000, 0x20000000, 0x40000000, 0x80000000
    };

    tuint32 i,n = static_cast<tuint32>(N);
    tint res = -1;

    for(i=0;i<32;++i)
    {
        if((n&mask[i]) && !(n^mask[i]))
        {
            res = i;
            break;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type2(tfloat32 *x,tfloat32 *X,int N)
{
    Type2(x,X,N,mod2(N));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type2(tfloat32 *x,tfloat32 *X,int N,int lN)
{
    tint i,j,halfN = N >> 1;
    tfloat32 *xA,*xB,*Xa,*Xb;

    xA = &m_Y[m_offsetY];
    xB = &m_Y[m_offsetY + halfN];
    Xa = &m_Y[m_offsetY + N];
    Xb = &m_Y[m_offsetY + N + halfN];

    m_offsetY += N << 1;

    for(i=0,j=N-1;i<halfN;i++,j--)
    {
        xA[i] = x[i] + x[j];
        xB[i] = x[i] - x[j];
    }

    if(halfN==16)
    {
        T2L16(xA,Xa);
        T4L16(xB,Xb);
    }
    else
    {
        Type2(xA,Xa,halfN,lN-1);
        Type4(xB,Xb,halfN,lN-1);
    }


    for(i=0,j=0;i<halfN;++i,j+=2)
    {
        X[j] = Xa[i];
        X[j+1] = Xb[i];
    }

    m_offsetY -= N << 1;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type3(tfloat32 *x,tfloat32 *X,int N)
{
    Type3(x,X,N,mod2(N));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type3(tfloat32 *x,tfloat32 *X,int N,int lN)
{
    tint i,j,halfN = N >> 1;
    tfloat32 *xA,*xB,*Xa,*Xb;

    xA = &m_Y[m_offsetY];
    xB = &m_Y[m_offsetY + halfN];
    Xa = &m_Y[m_offsetY + N];
    Xb = &m_Y[m_offsetY + N + halfN];

    m_offsetY += N << 1;

    for(i=0,j=0;i<halfN;++i,j+=2)
    {
        xB[i] = x[j];
        xA[i] = x[j+1];
    }

    if(halfN==16)
    {
        T4L16(xA,Xa);
        T3L16(xB,Xb);
    }
    else
    {
        Type4(xA,Xa,halfN,lN-1);
        Type3(xB,Xb,halfN,lN-1);
    }

    for(i=0,j=N-1;i<halfN;i++,j--)
    {
        X[i] = Xa[i] + Xb[i];
        X[j] = Xb[i] - Xa[i];
    }

    m_offsetY -= N << 1;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type4(tfloat32 *x,tfloat32 *X,int N)
{
    Type4(x,X,N,mod2(N));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type4(tfloat32 *x,tfloat32 *X,int N,int lN)
{
    tint i,j,halfN = N >> 1;
    tfloat32 *xA,*xB,*Xa,*Xb;

    xA = &m_Y[m_offsetY];
    xB = &m_Y[m_offsetY + halfN];
    Xa = &m_Y[m_offsetY + N];
    Xb = &m_Y[m_offsetY + N + halfN];

    m_offsetY += N << 1;

    xA[0] = x[0] + x[1];
    xB[0] = x[0];
    for(i=1,j=2;i<halfN;i++,j+=2)
    {
        xA[i] = x[j] + x[j+1];
        xB[i] = x[j-1] + x[j];
    }

    if(halfN==16)
    {
        T4L16(xA,Xa);
        T3L16(xB,Xb);
    }
    else
    {
        Type4(xA,Xa,halfN,lN-1);
        Type3(xB,Xb,halfN,lN-1);
    }

    tfloat32 *f = m_D4FactorArray[lN - 5];

    for(i=0,j=N-1;i<halfN;i++,j--)
    {
        X[i] = f[i] * (Xa[i] + Xb[i]);
        X[j] = f[j] * (Xb[i] - Xa[i]);
    }

    m_offsetY -= N << 1;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::T2L16(tfloat32 *x,tfloat32 *X)
{
    tfloat32 y[32],Y[12];

    y[ 0] = x[0] + x[15];
    y[ 1] = x[1] + x[14];
    y[ 2] = x[2] + x[13];
    y[ 3] = x[3] + x[12];
    y[ 4] = x[4] + x[11];
    y[ 5] = x[5] + x[10];
    y[ 6] = x[6] + x[9];
    y[ 7] = x[7] + x[8];
    y[ 8] = x[0] - x[15];
    y[ 9] = x[1] - x[14];
    y[10] = x[2] - x[13];
    y[11] = x[3] - x[12];
    y[12] = x[4] - x[11];
    y[13] = x[5] - x[10];
    y[14] = x[6] - x[9];
    y[15] = x[7] - x[8];

    y[16] = y[0] + y[7];
    y[17] = y[1] + y[6];
    y[18] = y[2] + y[5];
    y[19] = y[3] + y[4];
    y[20] = y[0] - y[7];
    y[21] = y[1] - y[6];
    y[22] = y[2] - y[5];
    y[23] = y[3] - y[4];

    y[24] = y[16] + y[19];
    y[25] = y[17] + y[18];
    y[26] = y[16] - y[19];
    y[27] = y[17] - y[18];

    X[ 0] = y[24] + y[25];
    X[ 8] = (y[24] * D_Cos_1Pi4) + (y[25] * D_Cos_3Pi4);
    X[ 4] = (y[26] * D_Cos_1Pi8) + (y[27] * D_Cos_3Pi8);
    X[12] = (y[26] * D_Cos_3Pi8) - (y[27] * D_Cos_1Pi8);

    y[28] = y[20] + y[21];
    y[29] = y[22] + y[23];
    y[30] = y[20];
    y[31] = (y[21] + y[22]) * D_Cos_1Pi4;

    Y[0] = (y[28] * D_Cos_1Pi8) + (y[29] * D_Cos_3Pi8);
    Y[1] = (y[28] * D_Cos_3Pi8) - (y[29] * D_Cos_1Pi8);
    Y[2] = y[30] + y[31];
    Y[3] = y[30] - y[31];

    X[ 2] = D_Delta_0Pi4 * (Y[0] + Y[2]);
    X[ 6] = D_Delta_1Pi4 * (Y[1] + Y[3]);
    X[10] = D_Delta_2Pi4 * (Y[3] - Y[1]);
    X[14] = D_Delta_3Pi4 * (Y[2] - Y[0]);

    y[0] = y[8] + y[9];
    y[1] = y[10] + y[11];
    y[2] = y[12] + y[13];
    y[3] = y[14] + y[15];
    y[4] = y[8];
    y[5] = y[9] + y[10];
    y[6] = y[11] + y[12];
    y[7] = y[13] + y[14];

    y[ 8] = y[0] + y[1];
    y[ 9] = y[2] + y[3];
    y[10] = y[0];
    y[11] = (y[1] + y[2]) * D_Cos_1Pi4;

    Y[ 8] = (y[8] * D_Cos_1Pi8) + (y[9] * D_Cos_3Pi8);
    Y[ 9] = (y[8] * D_Cos_3Pi8) - (y[9] * D_Cos_1Pi8);
    Y[10] = y[10] + y[11];
    Y[11] = y[10] - y[11];

    Y[0] = D_Delta_0Pi4 * (Y[ 8] + Y[10]);
    Y[1] = D_Delta_1Pi4 * (Y[ 9] + Y[11]);
    Y[2] = D_Delta_2Pi4 * (Y[11] - Y[ 9]);
    Y[3] = D_Delta_3Pi4 * (Y[10] - Y[ 8]);

    y[0] = y[6] * D_Cos_1Pi4;
    Y[ 8] = (y[5] * D_Cos_1Pi8) + (y[7] * D_Cos_3Pi8);
    Y[ 9] = (y[5] * D_Cos_3Pi8) - (y[7] * D_Cos_1Pi8);
    Y[10] = y[4] + y[0];
    Y[11] = y[4] - y[0];

    Y[4] = Y[ 8] + Y[10];
    Y[5] = Y[ 9] + Y[11];
    Y[6] = Y[11] - Y[ 9];
    Y[7] = Y[10] - Y[ 8];

    X[1] = D_Delta_0Pi8 * (Y[0] + Y[4]);
    X[3] = D_Delta_1Pi8 * (Y[1] + Y[5]);
    X[5] = D_Delta_2Pi8 * (Y[2] + Y[6]);
    X[7] = D_Delta_3Pi8 * (Y[3] + Y[7]);
    X[9] = D_Delta_4Pi8 * (Y[7] - Y[3]);
    X[11] = D_Delta_5Pi8 * (Y[6] - Y[2]);
    X[13] = D_Delta_6Pi8 * (Y[5] - Y[1]);
    X[15] = D_Delta_7Pi8 * (Y[4] - Y[0]);
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::T3L16(tfloat32 *x,tfloat32 *X)
{
    tfloat32 y[13],Y[28];

    y[0] = x[1] + x[3];
    y[1] = x[5] + x[7];
    y[2] = x[9] + x[11];
    y[3] = x[13] + x[15];
    y[4] = x[1];
    y[5] = x[3] + x[5];
    y[6] = x[7] + x[9];
    y[7] = x[11] + x[13];

    y[ 8] = y[0] + y[1];
    y[ 9] = y[2] + y[3];
    y[10] = y[0];
    y[11] = (y[1] + y[2]) * D_Cos_1Pi4;

    Y[16] = (y[8] * D_Cos_1Pi8) + (y[9] * D_Cos_3Pi8);
    Y[17] = (y[8] * D_Cos_3Pi8) - (y[9] * D_Cos_1Pi8);
    Y[18] = y[10] + y[11];
    Y[19] = y[10] - y[11];

    Y[20] = D_Delta_0Pi4 * (Y[16] + Y[18]);
    Y[21] = D_Delta_1Pi4 * (Y[17] + Y[19]);
    Y[22] = D_Delta_2Pi4 * (Y[19] - Y[17]);
    Y[23] = D_Delta_3Pi4 * (Y[18] - Y[16]);

    y[12] = y[6] * D_Cos_1Pi4;
    Y[ 8] = (y[5] * D_Cos_1Pi8) + (y[7] * D_Cos_3Pi8);
    Y[ 9] = (y[5] * D_Cos_3Pi8) - (y[7] * D_Cos_1Pi8);
    Y[10] = y[4] + y[12];
    Y[11] = y[4] - y[12];

    Y[16] = Y[ 8] + Y[10];
    Y[17] = Y[ 9] + Y[11];
    Y[18] = Y[11] - Y[ 9];
    Y[19] = Y[10] - Y[ 8];

    Y[0] = D_Delta_0Pi8 * (Y[20] + Y[16]);
    Y[1] = D_Delta_1Pi8 * (Y[21] + Y[17]);
    Y[2] = D_Delta_2Pi8 * (Y[22] + Y[18]);
    Y[3] = D_Delta_3Pi8 * (Y[23] + Y[19]);
    Y[4] = D_Delta_4Pi8 * (Y[19] - Y[23]);
    Y[5] = D_Delta_5Pi8 * (Y[18] - Y[22]);
    Y[6] = D_Delta_6Pi8 * (Y[17] - Y[21]);
    Y[7] = D_Delta_7Pi8 * (Y[16] - Y[20]);

    y[0] = x[2] + x[6];
    y[1] = x[10] + x[14];
    y[2] = x[2];
    y[3] = (x[6] + x[10]) * D_Cos_1Pi4;

    Y[16] = (y[0] * D_Cos_1Pi8) + (y[1] * D_Cos_3Pi8);
    Y[17] = (y[0] * D_Cos_3Pi8) - (y[1] * D_Cos_1Pi8);
    Y[18] = y[2] + y[3];
    Y[19] = y[2] - y[3];

    Y[20] = D_Delta_0Pi4 * (Y[16] + Y[18]);
    Y[21] = D_Delta_1Pi4 * (Y[17] + Y[19]);
    Y[22] = D_Delta_2Pi4 * (Y[19] - Y[17]);
    Y[23] = D_Delta_3Pi4 * (Y[18] - Y[16]);

    y[0] = x[8] * D_Cos_1Pi4;
    Y[24] = (x[4] * D_Cos_1Pi8) + (x[12] * D_Cos_3Pi8);
    Y[25] = (x[4] * D_Cos_3Pi8) - (x[12] * D_Cos_1Pi8);
    Y[26] = x[0] + y[0];
    Y[27] = x[0] - y[0];

    Y[16] = Y[24] + Y[26];
    Y[17] = Y[25] + Y[27];
    Y[18] = Y[27] - Y[25];
    Y[19] = Y[26] - Y[24];

    Y[ 8] = Y[20] + Y[16];
    Y[ 9] = Y[21] + Y[17];
    Y[10] = Y[22] + Y[18];
    Y[11] = Y[23] + Y[19];
    Y[12] = Y[19] - Y[23];
    Y[13] = Y[18] - Y[22];
    Y[14] = Y[17] - Y[21];
    Y[15] = Y[16] - Y[20];

    X[ 0] = Y[0] + Y[8];
    X[ 1] = Y[1] + Y[9];
    X[ 2] = Y[2] + Y[10];
    X[ 3] = Y[3] + Y[11];
    X[ 4] = Y[4] + Y[12];
    X[ 5] = Y[5] + Y[13];
    X[ 6] = Y[6] + Y[14];
    X[ 7] = Y[7] + Y[15];
    X[ 8] = Y[15] - Y[7];
    X[ 9] = Y[14] - Y[6];
    X[10] = Y[13] - Y[5];
    X[11] = Y[12] - Y[4];
    X[12] = Y[11] - Y[3];
    X[13] = Y[10] - Y[2];
    X[14] = Y[9] - Y[1];
    X[15] = Y[8] - Y[0];
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::T4L16(tfloat32 *x,tfloat32 *X)
{
    tfloat32 y[29],Y[28];

    y[ 0] = x[0] + x[1];
    y[ 1] = x[2] + x[3];
    y[ 2] = x[4] + x[5];
    y[ 3] = x[6] + x[7];
    y[ 4] = x[8] + x[9];
    y[ 5] = x[10] + x[11];
    y[ 6] = x[12] + x[13];
    y[ 7] = x[14] + x[15];
    y[ 8] = x[0];
    y[ 9] = x[1] + x[2];
    y[10] = x[3] + x[4];
    y[11] = x[5] + x[6];
    y[12] = x[7] + x[8];
    y[13] = x[9] + x[10];
    y[14] = x[11] + x[12];
    y[15] = x[13] + x[14];

    y[16] = y[0] + y[1];
    y[17] = y[2] + y[3];
    y[18] = y[4] + y[5];
    y[19] = y[6] + y[7];
    y[20] = y[0];
    y[21] = y[1] + y[2];
    y[22] = y[3] + y[4];
    y[23] = y[5] + y[6];

    y[24] = y[16] + y[17];
    y[25] = y[18] + y[19];
    y[26] = y[16];
    y[27] = (y[17] + y[18]) * D_Cos_1Pi4;

    Y[16] = (y[24] * D_Cos_1Pi8) + (y[25] * D_Cos_3Pi8);
    Y[17] = (y[24] * D_Cos_3Pi8) - (y[25] * D_Cos_1Pi8);
    Y[18] = y[26] + y[27];
    Y[19] = y[26] - y[27];

    Y[20] = D_Delta_0Pi4 * (Y[16] + Y[18]);
    Y[21] = D_Delta_1Pi4 * (Y[17] + Y[19]);
    Y[22] = D_Delta_2Pi4 * (Y[19] - Y[17]);
    Y[23] = D_Delta_3Pi4 * (Y[18] - Y[16]);

    y[28] = y[22] * D_Cos_1Pi4;
    Y[16] = (y[21] * D_Cos_1Pi8) + (y[23] * D_Cos_3Pi8);
    Y[17] = (y[21] * D_Cos_3Pi8) - (y[23] * D_Cos_1Pi8);
    Y[18] = y[20] + y[28];
    Y[19] = y[20] - y[28];

    Y[24] = Y[16] + Y[18];
    Y[25] = Y[17] + Y[19];
    Y[26] = Y[19] - Y[17];
    Y[27] = Y[18] - Y[16];

    Y[0] = D_Delta_0Pi8 * (Y[20] + Y[24]);
    Y[1] = D_Delta_1Pi8 * (Y[21] + Y[25]);
    Y[2] = D_Delta_2Pi8 * (Y[22] + Y[26]);
    Y[3] = D_Delta_3Pi8 * (Y[23] + Y[27]);
    Y[4] = D_Delta_4Pi8 * (Y[27] - Y[23]);
    Y[5] = D_Delta_5Pi8 * (Y[26] - Y[22]);
    Y[6] = D_Delta_6Pi8 * (Y[25] - Y[21]);
    Y[7] = D_Delta_7Pi8 * (Y[24] - Y[20]);

    y[16] = y[9] + y[11];
    y[17] = y[13] + y[15];
    y[18] = y[9];
    y[19] = (y[11] + y[13]) * D_Cos_1Pi4;

    Y[20] = (y[16] * D_Cos_1Pi8) + (y[17] * D_Cos_3Pi8);
    Y[21] = (y[16] * D_Cos_3Pi8) - (y[17] * D_Cos_1Pi8);
    Y[22] = y[18] + y[19];
    Y[23] = y[18] - y[19];

    Y[16] = D_Delta_0Pi4 * (Y[20] + Y[22]);
    Y[17] = D_Delta_1Pi4 * (Y[21] + Y[23]);
    Y[18] = D_Delta_2Pi4 * (Y[23] - Y[21]);
    Y[19] = D_Delta_3Pi4 * (Y[22] - Y[20]);

    y[20] = y[12] * D_Cos_1Pi4;
    Y[24] = (y[10] * D_Cos_1Pi8) + (y[14] * D_Cos_3Pi8);
    Y[25] = (y[10] * D_Cos_3Pi8) - (y[14] * D_Cos_1Pi8);
    Y[26] = y[8] + y[20];
    Y[27] = y[8] - y[20];

    Y[20] = Y[24] + Y[26];
    Y[21] = Y[25] + Y[27];
    Y[22] = Y[27] - Y[25];
    Y[23] = Y[26] - Y[24];

    Y[ 8] = Y[16] + Y[20];
    Y[ 9] = Y[17] + Y[21];
    Y[10] = Y[18] + Y[22];
    Y[11] = Y[19] + Y[23];
    Y[12] = Y[23] - Y[19];
    Y[13] = Y[22] - Y[18];
    Y[14] = Y[21] - Y[17];
    Y[15] = Y[20] - Y[16];

    X[ 0] = D_Delta_0Pi16 * (Y[0] + Y[8]);
    X[ 1] = D_Delta_1Pi16 * (Y[1] + Y[9]);
    X[ 2] = D_Delta_2Pi16 * (Y[2] + Y[10]);
    X[ 3] = D_Delta_3Pi16 * (Y[3] + Y[11]);
    X[ 4] = D_Delta_4Pi16 * (Y[4] + Y[12]);
    X[ 5] = D_Delta_5Pi16 * (Y[5] + Y[13]);
    X[ 6] = D_Delta_6Pi16 * (Y[6] + Y[14]);
    X[ 7] = D_Delta_7Pi16 * (Y[7] + Y[15]);
    X[ 8] = D_Delta_8Pi16 * (Y[15] - Y[7]);
    X[ 9] = D_Delta_9Pi16 * (Y[14] - Y[6]);
    X[10] = D_Delta_10Pi16 * (Y[13] - Y[5]);
    X[11] = D_Delta_11Pi16 * (Y[12] - Y[4]);
    X[12] = D_Delta_12Pi16 * (Y[11] - Y[3]);
    X[13] = D_Delta_13Pi16 * (Y[10] - Y[2]);
    X[14] = D_Delta_14Pi16 * (Y[9] - Y[1]);
    X[15] = D_Delta_15Pi16 * (Y[8] - Y[0]);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_INTEL)
//-------------------------------------------------------------------------------------------

void OmegaDCT::Type2_IntelSIMD(tfloat32 *x,tfloat32 *X,int N)
{
    Type2_IntelSIMD(x,X,N,mod2(N));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type2_IntelSIMD(tfloat32 *x,tfloat32 *X,int N,int lN)
{
    tint halfN = N >> 1;
    tfloat32 *xA,*xB,*Xa,*Xb,*x2;

    xA = &m_Y[m_offsetY];
    xB = &m_Y[m_offsetY + halfN];
    Xa = &m_Y[m_offsetY + N];
    Xb = &m_Y[m_offsetY + N + halfN];
    x2 = &x[N];

    m_offsetY += N << 1;

    __asm
    {
        pushad
        emms

        mov        eax , halfN
        mov        ebx , xA
        mov        ecx , xB
        mov        edx , x
        mov        edi    , x2

T2I_1:    sub        edi , 64

        movaps    xmm0 , [edx]            // xmm0 - x[0] : x[1] : x[2] : x[3]
        movaps    xmm1 , [edi + 48]        // xmm1 - x[4] : x[5] : x[6] : x[7]
        shufps    xmm1 , xmm1    , 0x1B        // xmm1 - x[7] : x[6] : x[5] : x[4] - 00 01 10 11
        movaps    xmm2 , xmm1                // xmm2 - x[7] : x[6] : x[5] : x[4]
        addps    xmm1 , xmm0                // xmm1 - xA[0] : xA[1] : xA[2] : xA[3]
        subps    xmm0 , xmm2                // xmm0 - xB[0] : xB[1] : xB[2] : xB[3]
        movaps    [ebx] , xmm1
        movaps    [ecx] , xmm0

        movaps    xmm0 , [edx + 16]
        movaps    xmm1 , [edi + 32]
        shufps    xmm1 , xmm1 , 0x1B
        movaps    xmm2 , xmm1
        addps    xmm1 , xmm0
        subps    xmm0 , xmm2
        movaps    [ebx + 16] , xmm1
        movaps    [ecx + 16] , xmm0

        movaps    xmm0 , [edx + 32]
        movaps    xmm1 , [edi + 16]
        shufps    xmm1 , xmm1 , 0x1B
        movaps    xmm2 , xmm1
        addps    xmm1 , xmm0
        subps    xmm0 , xmm2
        movaps    [ebx + 32] , xmm1
        movaps    [ecx + 32] , xmm0

        movaps    xmm0 , [edx + 48]
        movaps    xmm1 , [edi]
        shufps    xmm1 , xmm1 , 0x1B
        movaps    xmm2 , xmm1
        addps    xmm1 , xmm0
        subps    xmm0 , xmm2
        movaps    [ebx + 48] , xmm1
        movaps    [ecx + 48] , xmm0

        add        ebx , 64
        add        ecx , 64
        add        edx , 64
        sub        eax , 16
        jnz        T2I_1

        emms
        popad
    }

    if(halfN==16)
    {
        T2L16_IntelSIMD(xA,Xa,&m_Y[m_offsetY]);
        T4L16_IntelSIMD(xB,Xb,&m_Y[m_offsetY]);
    }
    else
    {
        Type2_IntelSIMD(xA,Xa,halfN,lN-1);
        Type4_IntelSIMD(xB,Xb,halfN,lN-1);
    }

    __asm
    {
        pushad
        emms

        mov        eax , halfN
        mov        ebx , Xa
        mov        ecx , Xb
        mov        edx , X

T2I_2:    movaps    xmm0 , [ebx]        // xmm0 - Xa[0] : Xa[1] : Xa[2] : Xa[3]
        movaps    xmm1 , [ecx]        // xmm1 - Xb[0] : Xb[1] : Xb[2] : Xb[3]
        movaps    xmm2 , xmm0            // xmm2 - Xa[0] : Xa[1] : Xa[2] : Xa[3]
        shufps    xmm0 , xmm1 , 0x44    // xmm0 - Xa[0] : Xa[1] : Xb[0] : Xb[1] - 01 00 01 00
        shufps    xmm2 , xmm1 , 0xEE    // xmm2 - Xa[2] : Xa[3] : Xb[2] : Xb[3] - 11 10 11 10
        shufps    xmm0 , xmm0 , 0xD8    // xmm0 - Xa[0] : Xb[0] : Xa[1] : Xb[1] - 11 01 10 00
        shufps    xmm2 , xmm2 , 0xD8    // xmm2 - Xa[2] : Xb[2] : Xa[3] : Xb[3] - 11 01 10 00
        movaps    [edx] , xmm0
        movaps    [edx + 16] , xmm2

        movaps    xmm0 , [ebx + 16]
        movaps    xmm1 , [ecx + 16]
        movaps    xmm2 , xmm0
        shufps    xmm0 , xmm1 , 0x44
        shufps    xmm2 , xmm1 , 0xEE
        shufps    xmm0 , xmm0 , 0xD8
        shufps    xmm2 , xmm2 , 0xD8
        movaps    [edx + 32] , xmm0
        movaps    [edx + 48] , xmm2

        movaps    xmm0 , [ebx + 32]
        movaps    xmm1 , [ecx + 32]
        movaps    xmm2 , xmm0
        shufps    xmm0 , xmm1 , 0x44
        shufps    xmm2 , xmm1 , 0xEE
        shufps    xmm0 , xmm0 , 0xD8
        shufps    xmm2 , xmm2 , 0xD8
        movaps    [edx + 64] , xmm0
        movaps    [edx + 80] , xmm2

        movaps    xmm0 , [ebx + 48]
        movaps    xmm1 , [ecx + 48]
        movaps    xmm2 , xmm0
        shufps    xmm0 , xmm1 , 0x44
        shufps    xmm2 , xmm1 , 0xEE
        shufps    xmm0 , xmm0 , 0xD8
        shufps    xmm2 , xmm2 , 0xD8
        movaps    [edx + 96] , xmm0
        movaps    [edx + 112] , xmm2

        add        ebx , 64
        add        ecx , 64
        add        edx , 128
        sub        eax , 16
        jnz        T2I_2

        emms
        popad
    }

    m_offsetY -= N << 1;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type3_IntelSIMD(tfloat32 *x,tfloat32 *X,int N)
{
    Type3_IntelSIMD(x,X,N,mod2(N));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type3_IntelSIMD(tfloat32 *x,tfloat32 *X,int N,int lN)
{
    tint halfN = N >> 1;
    tfloat32 *xA,*xB,*Xa,*Xb,*X2;

    xA = &m_Y[m_offsetY];
    xB = &m_Y[m_offsetY + halfN];
    Xa = &m_Y[m_offsetY + N];
    Xb = &m_Y[m_offsetY + N + halfN];
    X2 = &X[N];

    m_offsetY += N << 1;

    __asm
    {
        pushad
        emms

        mov        eax , halfN
        mov        ebx , xA
        mov        ecx    , xB
        mov        edx    , x

T3I_1:    movaps    xmm0 , [edx]            // xmm0 - x[0] : x[1] : x[2] : x[3]
        movaps    xmm1 , [edx + 16]        // xmm1 - x[4] : x[5] : x[6] : x[7]
        movaps    xmm2 , xmm0                // xmm2 - x[0] : x[1] : x[2] : x[3]
        shufps    xmm0 , xmm1 , 0x88        // xmm0 - x[0] : x[2] : x[4] : x[6] - 10 00 10 00
        shufps    xmm2 , xmm1 , 0xDD        // xmm2 - x[1] : x[3] : x[5] : x[7] - 11 01 11 01
        movaps    [ecx] , xmm0            // xmm0 - xB[0] : xB[1] : xB[2] : xB[3]
        movaps    [ebx] , xmm2            // xmm2 - xA[0] : xA[1] : xA[2] : xA[3]

        movaps    xmm0 , [edx + 32]
        movaps    xmm1 , [edx + 48]
        movaps    xmm2 , xmm0
        shufps    xmm0 , xmm1 , 0x88
        shufps    xmm2 , xmm1 , 0xDD
        movaps    [ecx + 16] , xmm0
        movaps    [ebx + 16] , xmm2

        movaps    xmm0 , [edx + 64]
        movaps    xmm1 , [edx + 80]
        movaps    xmm2 , xmm0
        shufps    xmm0 , xmm1 , 0x88
        shufps    xmm2 , xmm1 , 0xDD
        movaps    [ecx + 32] , xmm0
        movaps    [ebx + 32] , xmm2

        movaps    xmm0 , [edx + 96]
        movaps    xmm1 , [edx + 112]
        movaps    xmm2 , xmm0
        shufps    xmm0 , xmm1 , 0x88
        shufps    xmm2 , xmm1 , 0xDD
        movaps    [ecx + 48] , xmm0
        movaps    [ebx + 48] , xmm2

        add        ebx , 64
        add        ecx , 64
        add        edx , 128
        sub        eax , 16
        jnz        T3I_1

        emms
        popad
    }

    if(halfN==16)
    {
        T4L16_IntelSIMD(xA,Xa,&m_Y[m_offsetY]);
        T3L16_IntelSIMD(xB,Xb,&m_Y[m_offsetY]);
    }
    else
    {
        Type4_IntelSIMD(xA,Xa,halfN,lN-1);
        Type3_IntelSIMD(xB,Xb,halfN,lN-1);
    }

    __asm
    {
        pushad
        emms

        mov        eax , halfN
        mov        ebx , X
        mov        ecx , X2
        mov        edx , Xa
        mov        edi , Xb

T3I_2:    sub        ecx , 64

        movaps    xmm0 , [edx]            // xmm0 - Xa[0] : Xa[1] : Xa[2] : Xa[3]
        movaps    xmm1 , [edi]            // xmm1 - Xb[0] : Xb[1] : Xb[2] : Xb[3]
        movaps    xmm2 , xmm1                // xmm2 - Xb[0] : Xb[1] : Xb[2] : Xb[3]
        subps    xmm2 , xmm0                // xmm2 - Xb[0]-Xa[0] : Xb[1]-Xa[1] : Xb[2]-Xa[2] : Xb[3]-Xa[3] = X[4] : X[5] : X[6] : X[7]
        addps    xmm0 , xmm1                // xmm0 - Xa[0]+Xb[0] : Xa[1]+Xb[1] : Xa[2]+Xb[2] : Xa[3]+Xb[3] = X[0] : X[1] : X[2] : X[3]
        shufps    xmm2 , xmm2 , 0x1B        // xmm2 - X[7] : X[6] : X[5] : X[4] - 00 01 10 11
        movaps    [ebx] , xmm0
        movaps    [ecx+48] , xmm2

        movaps    xmm0 , [edx+16]
        movaps    xmm1 , [edi+16]
        movaps    xmm2 , xmm1
        subps    xmm2 , xmm0
        addps    xmm0 , xmm1
        shufps    xmm2 , xmm2 , 0x1B
        movaps    [ebx+16] , xmm0
        movaps    [ecx+32] , xmm2

        movaps    xmm0 , [edx+32]
        movaps    xmm1 , [edi+32]
        movaps    xmm2 , xmm1
        subps    xmm2 , xmm0
        addps    xmm0 , xmm1
        shufps    xmm2 , xmm2 , 0x1B
        movaps    [ebx+32] , xmm0
        movaps    [ecx+16] , xmm2

        movaps    xmm0 , [edx+48]
        movaps    xmm1 , [edi+48]
        movaps    xmm2 , xmm1
        subps    xmm2 , xmm0
        addps    xmm0 , xmm1
        shufps    xmm2 , xmm2 , 0x1B
        movaps    [ebx+48] , xmm0
        movaps    [ecx] , xmm2

        add        edx , 64
        add        edi , 64
        add        ebx , 64
        sub        eax , 16
        jnz        T3I_2

        emms
        popad
    }

    m_offsetY -= N << 1;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type4_IntelSIMD(tfloat32 *x,tfloat32 *X,int N)
{
    Type4_IntelSIMD(x,X,N,mod2(N));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::Type4_IntelSIMD(tfloat32 *x,tfloat32 *X,int N,int lN)
{
    tint halfN = N >> 1;
    tfloat32 *xA,*xB,*Xa,*Xb;
    tuint *mask = m_DCTMasks_IntelSIMD;

    xA = &m_Y[m_offsetY];
    xB = &m_Y[m_offsetY + halfN];
    Xa = &m_Y[m_offsetY + N];
    Xb = &m_Y[m_offsetY + N + halfN];

    m_offsetY += N << 1;

    __asm
    {
        pushad
        emms

        mov        eax , halfN
        mov        ebx , x
        mov        ecx , xA
        mov        edx , xB

        mov        edi , mask
        andps    xmm2 , [edi]

T4I_1:    //    xmm2 - x[j-7] : x[j-5] : x[j-3] : x[j-1] -> x[j+1] : x[j+3] : x[j+5] : x[j+7]

        // x[j+0] : x[j+2] : x[j+4] : x[j+6]
        // x[j+1] : x[j+3] : x[j+5] : x[j+7]
        // x[j-1] : x[j+1] : x[j+3] : x[j+5]

        movaps    xmm0 , [ebx]        // xmm0 - x[j+0] : x[j+1] : x[j+2] : x[j+3]
        movaps    xmm1 , [ebx+16]        // xmm1 - x[j+4] : x[j+5] : x[j+6] : x[j+7]
        shufps    xmm2 , xmm0 , 0x5F    // xmm2 - x[j-1] : x[j-1] : x[j+1] : x[j+1] - 01 01 11 11
        movaps    xmm3 , xmm0            // xmm3 - x[j+0] : x[j+1] : x[j+2] : x[j+3]
        shufps    xmm0 , xmm1 , 0x88    // xmm0 - x[j+0] : x[j+2] : x[j+4] : x[j+6] - 10 00 10 00
        shufps    xmm3 , xmm1 , 0xDD    // xmm3 - x[j+1] : x[j+3] : x[j+5] : x[j+7] - 11 01 11 01
        shufps    xmm2 , xmm3 , 0x98    // xmm2 - x[j-1] : x[j+1] : x[j+3] : x[j+5] - 10 01 10 00
        addps    xmm2 , xmm0            // xmm2 - x[j-1]+x[j+0] : x[j+1]+x[j+2] : x[j+3]+x[j+4] : x[j+5]+x[j+6] = xB[i+0] : xB[i+1] : xB[i+2] : xB[i+3]
        addps    xmm0 , xmm3            // xmm0 - x[j+0]+x[j+1] : x[j+2]+x[j+3] : x[j+4]+x[j+5] : x[j+6]+x[j+7] = xA[i+0] : xA[i+1] : xA[i+2] : xA[i+3]
        movaps    [ecx] , xmm0
        movaps    [edx] , xmm2
        movaps    xmm2 , xmm3            // xmm2 - x[j+1] : x[j+3] : x[j+5] : x[j+7]

        movaps    xmm0 , [ebx+32]
        movaps    xmm1 , [ebx+48]
        shufps    xmm2 , xmm0 , 0x5F
        movaps    xmm3 , xmm0
        shufps    xmm0 , xmm1 , 0x88
        shufps    xmm3 , xmm1 , 0xDD
        shufps    xmm2 , xmm3 , 0x98
        addps    xmm2 , xmm0
        addps    xmm0 , xmm3
        movaps    [ecx+16] , xmm0
        movaps    [edx+16] , xmm2
        movaps    xmm2 , xmm3

        movaps    xmm0 , [ebx+64]
        movaps    xmm1 , [ebx+80]
        shufps    xmm2 , xmm0 , 0x5F
        movaps    xmm3 , xmm0
        shufps    xmm0 , xmm1 , 0x88
        shufps    xmm3 , xmm1 , 0xDD
        shufps    xmm2 , xmm3 , 0x98
        addps    xmm2 , xmm0
        addps    xmm0 , xmm3
        movaps    [ecx+32] , xmm0
        movaps    [edx+32] , xmm2
        movaps    xmm2 , xmm3

        movaps    xmm0 , [ebx+96]
        movaps    xmm1 , [ebx+112]
        shufps    xmm2 , xmm0 , 0x5F
        movaps    xmm3 , xmm0
        shufps    xmm0 , xmm1 , 0x88
        shufps    xmm3 , xmm1 , 0xDD
        shufps    xmm2 , xmm3 , 0x98
        addps    xmm2 , xmm0
        addps    xmm0 , xmm3
        movaps    [ecx+48] , xmm0
        movaps    [edx+48] , xmm2
        movaps    xmm2 , xmm3

        add        ebx , 128
        add        ecx , 64
        add        edx , 64
        sub        eax , 16
        jnz        T4I_1

        emms
        popad
    }

    if(halfN==16)
    {
        T4L16_IntelSIMD(xA,Xa,&m_Y[m_offsetY]);
        T3L16_IntelSIMD(xB,Xb,&m_Y[m_offsetY]);
    }
    else
    {
        Type4_IntelSIMD(xA,Xa,halfN,lN-1);
        Type3_IntelSIMD(xB,Xb,halfN,lN-1);
    }

    tuint32    NOffset = static_cast<tuint32>(N) << 2;
    tuint32    halfNOffset = static_cast<tuint32>(halfN) << 2;
    tfloat32 *f = m_D4FactorArray[lN - 5];

    __asm
    {
        pushad
        emms

        mov        eax , 0
        mov        ebx , NOffset

T4I_2:    sub        ebx , 64

        mov        ecx , Xa
        mov        edx , Xb

        movaps    xmm0 , [edx + eax]        // xmm0 - Xb[ 0] : Xb[ 1] : Xb[ 2] : Xb[ 3]
        movaps    xmm1 , [edx + eax + 16]    // xmm1 - Xb[ 4] : Xb[ 5] : Xb[ 6] : Xb[ 7]
        movaps    xmm2 , [edx + eax + 32]    // xmm2 - Xb[ 8] : Xb[ 9] : Xb[10] : Xb[11]
        movaps    xmm3 , [edx + eax + 48]    // xmm3 - Xb[12] : Xb[13] : Xb[14] : Xb[15]
        movaps    xmm4 , xmm0                // xmm4 - Xb[ 0] : Xb[ 1] : Xb[ 2] : Xb[ 3]
        movaps    xmm5 , xmm1                // xmm5 - Xb[ 4] : Xb[ 5] : Xb[ 6] : Xb[ 7]
        movaps    xmm6 , xmm2                // xmm6 - Xb[ 8] : Xb[ 9] : Xb[10] : Xb[11]
        movaps    xmm7 , xmm3                // xmm7 - Xb[12] : Xb[13] : Xb[14] : Xb[15]

        addps    xmm0 , [ecx + eax]        // xmm0 - Xb[ 0]+Xa[ 0] : Xb[ 1]+Xa[ 1] : Xb[ 2]+Xa[ 2] : Xb[ 3]+Xa[ 3] = Xc[ 0] : Xc[ 1] : Xc[ 2] : Xc[ 3]
        addps    xmm1 , [ecx + eax + 16]    // xmm1 - Xb[ 4]+Xa[ 4] : Xb[ 5]+Xa[ 5] : Xb[ 6]+Xa[ 6] : Xb[ 7]+Xa[ 7] = Xc[ 4] : Xc[ 5] : Xc[ 6] : Xc[ 7]
        addps    xmm2 , [ecx + eax + 32]    // xmm2 - Xb[ 8]+Xa[ 8] : Xb[ 9]+Xa[ 9] : Xb[10]+Xa[10] : Xb[11]+Xa[11] = Xc[ 8] : Xc[ 9] : Xc[10] : Xc[11]
        addps    xmm3 , [ecx + eax + 48]    // xmm3 - Xb[12]+Xa[12] : Xb[13]+Xa[13] : Xb[14]+Xa[14] : Xb[15]+Xa[15] = Xc[12] : Xc[13] : Xc[14] : Xc[15]

        subps    xmm4 , [ecx + eax]        // xmm4 - Xb[ 0]-Xa[ 0] : Xb[ 1]-Xa[ 1] : Xb[ 2]-Xa[ 2] : Xb[ 3]-Xa[ 3] = Xd[ 0] : Xd[ 1] : Xd[ 2] : Xd[ 3]
        subps    xmm5 , [ecx + eax + 16]    // xmm5 - Xb[ 4]-Xa[ 4] : Xb[ 5]-Xa[ 5] : Xb[ 6]-Xa[ 6] : Xb[ 7]-Xa[ 7] = Xd[ 4] : Xd[ 5] : Xd[ 6] : Xd[ 7]
        subps    xmm6 , [ecx + eax + 32]    // xmm6 - Xb[ 8]-Xa[ 8] : Xb[ 9]-Xa[ 9] : Xb[10]-Xa[10] : Xb[11]-Xa[11] = Xd[ 8] : Xd[ 9] : Xd[10] : Xd[11]
        subps    xmm7 , [ecx + eax + 48]    // xmm7 - Xb[12]-Xa[12] : Xb[13]-Xa[13] : Xb[14]-Xa[14] : Xb[15]-Xa[15] = Xd[12] : Xd[13] : Xd[14] : Xd[15]

        //  X[ 0] = f[ 0] * (Xb[ 0] + Xa[ 0]) = f[ 0] * Xc[ 0]
        //  X[ 1] = f[ 1] * (Xb[ 1] + Xa[ 1]) = f[ 1] * Xc[ 1]
        //  X[ 2] = f[ 2] * (Xb[ 2] + Xa[ 2]) = f[ 2] * Xc[ 2]
        //  X[ 3] = f[ 3] * (Xb[ 3] + Xa[ 3]) = f[ 3] * Xc[ 3]
        //  X[ 4] = f[ 4] * (Xb[ 4] + Xa[ 4]) = f[ 4] * Xc[ 4]
        //  X[ 5] = f[ 5] * (Xb[ 5] + Xa[ 5]) = f[ 5] * Xc[ 5]
        //  X[ 6] = f[ 6] * (Xb[ 6] + Xa[ 6]) = f[ 6] * Xc[ 6]
        //  X[ 7] = f[ 7] * (Xb[ 7] + Xa[ 7]) = f[ 7] * Xc[ 7]
        //  X[ 8] = f[ 8] * (Xb[ 7] - Xa[ 7]) = f[ 8] * Xd[ 7]
        //  X[ 9] = f[ 9] * (Xb[ 6] - Xa[ 6]) = f[ 9] * Xd[ 6]
        //  X[10] = f[10] * (Xb[ 5] - Xa[ 5]) = f[10] * Xd[ 5]
        //  X[11] = f[11] * (Xb[ 4] - Xa[ 4]) = f[11] * Xd[ 4]
        //  X[12] = f[12] * (Xb[ 3] - Xa[ 3]) = f[12] * Xd[ 3]
        //  X[13] = f[13] * (Xb[ 2] - Xa[ 2]) = f[13] * Xd[ 2]
        //  X[14] = f[14] * (Xb[ 1] - Xa[ 1]) = f[14] * Xd[ 1]
        //  X[15] = f[15] * (Xb[ 0] - Xa[ 0]) = f[15] * Xd[ 0]

        shufps    xmm4 , xmm4    , 0x1B        // xmm4 - Xd[ 3] : Xd[ 2] : Xd[ 1] : Xd[ 0] - 00 01 10 11
        shufps    xmm5 , xmm5 , 0x1B        // xmm5 - Xd[ 7] : Xd[ 6] : Xd[ 5] : Xd[ 4]
        shufps    xmm6 , xmm6 , 0x1B        // xmm6 - Xd[11] : Xd[10] : Xd[ 9] : Xd[ 8]
        shufps    xmm7 , xmm7 , 0x1B        // xmm7 - Xd[15] : Xd[14] : Xd[13] : Xd[12]

        mov        ecx , X
        mov        edx , f

        mulps    xmm0 , [edx + eax]        // xmm0 - X[ 0] : X[ 1] : X[ 2] : X[ 3]
        mulps    xmm1 , [edx + eax + 16]    // xmm1 - X[ 4] : X[ 5] : X[ 6] : X[ 7]
        mulps    xmm2 , [edx + eax + 32]    // xmm2 - X[ 8] : X[ 9] : X[10] : X[11]
        mulps    xmm3 , [edx + eax + 48]    // xmm3 - X[12] : X[13] : X[14] : X[15]

        mulps    xmm7 , [edx + ebx]        // xmm7 - X[16] : X[17] : X[18] : X[19]
        mulps    xmm6 , [edx + ebx + 16]    // xmm6 - X[20] : X[21] : X[22] : X[23]
        mulps    xmm5 , [edx + ebx + 32]    // xmm5 - X[24] : X[25] : X[26] : X[27]
        mulps    xmm4 , [edx + ebx + 48] // xmm4 - X[28] : X[29] : X[30] : X[31]

        movaps    [ecx + eax] , xmm0
        movaps    [ecx + eax + 16] , xmm1
        movaps    [ecx + eax + 32] , xmm2
        movaps    [ecx + eax + 48] , xmm3

        movaps    [ecx + ebx + 48] , xmm4
        movaps    [ecx + ebx + 32] , xmm5
        movaps    [ecx + ebx + 16] , xmm6
        movaps    [ecx + ebx] , xmm7

        add        eax , 64
        cmp        eax , halfNOffset
        jb        T4I_2

        emms
        popad
    }

    m_offsetY -= N << 1;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::T2L16_IntelSIMD(tfloat32 *x,tfloat32 *X,tfloat32 *y)
{
    tfloat32 *co = m_DCTCoefficients_IntelSIMD;
    tuint *mask = m_DCTMasks_IntelSIMD;

    __asm
    {
        pushad
        emms

        mov        eax,x
        mov        ebx,y
        mov        ecx,co
        mov        edx,mask

        movaps    xmm1 , [eax]        // x[ 0] : x[ 1] : x[ 2] : x[ 3]
        movaps    xmm2 , [eax + 16]    // x[ 4] : x[ 5] : x[ 6] : x[ 7]
        movaps    xmm3 , [eax + 32]    // x[ 8] : x[ 9] : x[10] : x[11]
        movaps    xmm4 , [eax + 48]    // x[12] : x[13] : x[14] : x[15]

        //* y[ 0] = x[0] + x[15];
        //* y[ 1] = x[1] + x[14];
        //* y[ 2] = x[2] + x[13];
        //* y[ 3] = x[3] + x[12];
        //* y[ 4] = x[4] + x[11];
        //* y[ 5] = x[5] + x[10];
        //* y[ 6] = x[6] + x[9];
        //* y[ 7] = x[7] + x[8];
        //* y[ 8] = x[0] - x[15];
        //* y[ 9] = x[1] - x[14];
        //* y[10] = x[2] - x[13];
        //* y[11] = x[3] - x[12];
        //* y[12] = x[4] - x[11];
        //* y[13] = x[5] - x[10];
        //* y[14] = x[6] - x[9];
        //* y[15] = x[7] - x[8];

        // x[ 0] : x[ 1] : x[ 2] : x[ 3]
        // x[15] : x[14] : x[13] : x[12]
        // x[ 4] : x[ 5] : x[ 6] : x[ 7]
        // x[11] : x[10] : x[ 9] : x[ 8]

        shufps    xmm3 , xmm3 , 0x1B    // xmm3 - x[11] : x[10] : x[ 9] : x[ 8] - 00 01 10 11
        shufps    xmm4 , xmm4 , 0x1B    // xmm4 - x[15] : x[14] : x[13] : x[12] - 00 01 10 11
        movaps    xmm5 , xmm3            // xmm5 - x[11] : x[10] : x[ 9] : x[ 8]
        movaps    xmm6 , xmm4            // xmm6 - x[15] : x[14] : x[13] : x[12]
        addps    xmm4 , xmm1            // xmm4 - x[15]+x[0] : x[14]+x[1] : x[13]+x[2] : x[12]+x[3] = y[0] : y[1] : y[2] : y[3]
        addps    xmm3 , xmm2            // xmm3 - x[11]+x[4] : x[10]+x[5] : x[ 9]+x[6] : x[ 8]+x[7] = y[4] : y[5] : y[6] : y[7]
        subps    xmm1 , xmm6            // xmm1 - x[0]-x[15] : x[1]-x[14] : x[2]-x[13] : x[3]-x[12] = y[8] : y[9] : y[10] : y[11]
        subps    xmm2 , xmm5            // xmm2 - x[4]-x[11] : x[5]-x[10] : x[6]-x[ 9] : x[7]-x[ 8] = y[12] : y[13] : y[14] : y[15]

        //* y[16] = y[0] + y[7];
        //* y[17] = y[1] + y[6];
        //* y[18] = y[2] + y[5];
        //* y[19] = y[3] + y[4];
        //* y[20] = y[0] - y[7];
        //* y[21] = y[1] - y[6];
        //* y[22] = y[2] - y[5];
        //* y[23] = y[3] - y[4];

        // xmm4 - y[0] : y[1] : y[2] : y[3]
        // xmm3 - y[4] : y[5] : y[6] : y[7]

        shufps    xmm3 , xmm3 , 0x1B    // xmm3 - y[7] : y[6] : y[5] : y[4] - 00 01 10 11
        movaps    xmm5 , xmm4            // xmm5 - y[0] : y[1] : y[2] : y[3]
        addps    xmm4 , xmm3            // xmm4 - y[0]+y[7] : y[1]+y[6] : y[2]+y[5] : y[3]+y[4] = y[16] : y[17] : y[18] : y[19]
        subps    xmm5 , xmm3            // xmm5 - y[0]-y[7] : y[1]-y[6] : y[2]-y[5] : y[3]-y[4] = y[20] : y[21] : y[22] : y[23]

        //* y[24] = y[16] + y[19];
        //* y[25] = y[17] + y[18];
        //* y[26] = y[16] - y[19];
        //* y[27] = y[17] - y[18];

        // xmm4 - y[16] : y[17] : y[18] : y[19]
        // xmm5 - y[20] : y[21] : y[22] : y[23]

        movaps    xmm0 , xmm4         // xmm0 - y[16] : y[17] : y[18] : y[19]
        shufps    xmm0 , xmm0 , 0x44    // xmm0 - y[16] : y[17] : y[16] : y[17] - 01 00 01 00
        shufps    xmm4 , xmm4 , 0xBB    // xmm4 - y[19] : y[18] : y[19] : y[18] - 10 11 10 11
        xorps    xmm4 , [edx]        // xmm4 - y[19] : y[18] : -y[19] : -y[18]
        addps    xmm0 , xmm4            // xmm0 - y[16]+y[19] : y[17]+y[18] : y[18]-y[19] : y[19]-y[18] = y[24] : y[25] : y[26] : y[27]

        //* X[ 0] = y[24] + y[25];
        //* X[ 8] = (y[24] * c_Cos_1Pi4) + (y[25] * c_Cos_3Pi4);
        //* X[ 4] = (y[26] * c_Cos_1Pi8) + (y[27] * c_Cos_3Pi8);
        //* X[12] = (y[26] * c_Cos_3Pi8) - (y[27] * c_Cos_1Pi8);

        // xmm0 - y[24] : y[25] : y[26] : y[27]

        movaps    xmm3 , xmm0            // xmm3 - y[24] : y[25] : y[26] : y[27]
        shufps    xmm3 , xmm3 , 0xA0    // xmm3 - y[24] : y[24] : y[26] : y[26] - 10 10 00 00
        shufps    xmm0 , xmm0 , 0xF5    // xmm0 - y[25] : y[25] : y[27] : y[27] - 11 11 01 01
        mulps    xmm3 , [ecx]        // xmm3 - y[24] : (y[24] * c_Cos_1Pi4) : (y[26] * c_Cos_1Pi8) : (y[26] * c_Cos_3Pi8)
        mulps    xmm0 , [ecx + 16]    // xmm0 - y[25] : (y[25] * c_Cos_3Pi4) : (y[27] * c_Cos_3Pi8) : (y[27] * -c_Cos_1Pi8)
        addps    xmm3, xmm0            // xmm3 - X[ 0] : X[ 8] : X[ 4] : X[12]
        movaps    [ebx] , xmm3        // (ebx + 0) - X[ 0] : X[ 8] : X[ 4] : X[12]

        //* y[28] = y[20] + y[21];
        //* y[29] = y[22] + y[23];
        //* y[30] = y[20];
        //* y[31] = y[21] + y[22];

        // xmm5 - y[20] : y[21] : y[22] : y[23]

        movaps    xmm6 , xmm5                    // xmm6 - y[20] : y[21] : y[22] : y[23]
        shufps    xmm6 , [ecx + 32] , 0x0A    // xmm6 - y[22] : y[22] : 0.0f : 0.0f - 00 00 10 10
        shufps    xmm6 , xmm5 , 0xD2            // xmm6 - 0.0f  : y[22] : y[21] : y[23] - 11 01 00 10
        shufps    xmm5 , xmm5 , 0x84            // xmm5 - y[20] : y[21] : y[20] : y[22] - 10 00 01 00
        addps    xmm5 , xmm6                    // xmm5 - y[30] : y[31] : y[28] : y[29]

        //* Y[0] = (y[28] * c_Cos_1Pi8) + (y[29] * c_Cos_3Pi8);
        //* Y[1] = (y[28] * c_Cos_3Pi8) - (y[29] * c_Cos_1Pi8);
        //* Y[2] = y[30] + (y[31] * c_Cos_1Pi4);
        //* Y[3] = y[30] - (y[31] * c_Cos_1Pi4);

        movaps    xmm6 , xmm5            // xmm5 - y[30] : y[31] : y[28] : y[29]
        shufps    xmm6 , xmm6    , 0x0A    // xmm6 - y[28] : y[28] : y[30] : y[30] : 00 00 10 10
        shufps    xmm5 , xmm5    , 0x5F    // xmm5 - y[29] : y[29] : y[31] : y[31] : 01 01 11 11
        mulps    xmm6 , [ecx + 48]    // xmm6 - y[28]*c_Cos_1Pi8 : y[28]*c_Cos_3Pi8 : y[30] : y[30]
        mulps    xmm5 , [ecx + 64]    // xmm5 - y[29]*c_Cos_3Pi8 : y[29]*-c_Cos_1Pi8 : y[31]*c_Cos_1Pi4 : y[31]*-c_Cos_1Pi4
        addps    xmm6 , xmm5            // xmm6 - Y[0] : Y[1] : Y[2] : Y[3]

        //* X[ 2] = c_Delta_0Pi4 * (Y[0] + Y[2]);
        //* X[ 6] = c_Delta_1Pi4 * (Y[1] + Y[3]);
        //* X[10] = c_Delta_2Pi4 * (Y[3] - Y[1]);
        //* X[14] = c_Delta_3Pi4 * (Y[2] - Y[0]);

        movaps    xmm5 , xmm6            // xmm5 - Y[0] : Y[1] : Y[2] : Y[3]
        shufps    xmm5 , xmm5    , 0xB4    // xmm5 - Y[0] : Y[1] : Y[3] : Y[2] - 10 11 01 00
        shufps    xmm6 , xmm6 , 0x1E    // xmm6 - Y[2] : Y[3] : Y[1] : Y[0] - 00 01 11 10
        xorps    xmm6 , [edx]        // xmm6 - Y[2] : Y[3] : -Y[1] : -Y[0]
        addps    xmm5 , xmm6            // xmm5 - Y[0]+Y[2] : Y[1]+Y[3] : Y[3]-Y[1] : Y[2]-Y[0]
        mulps    xmm5 , [ecx + 80]    // xmm6 - X[ 2] : X[ 6] : X[10] : X[14]
        movaps    [ebx + 16] , xmm5    // (ebx + 16) - X[2] : X[6] : X[14] : X[10]

        //* y[0] = y[8] + y[9];
        //* y[1] = y[10] + y[11];
        //* y[2] = y[12] + y[13];
        //* y[3] = y[14] + y[15];
        //* y[4] = 0.0f + y[8];
        //* y[5] = y[9] + y[10];
        //* y[6] = y[11] + y[12];
        //* y[7] = y[13] + y[14];

        // xmm1 - y[ 8] : y[ 9] : y[10] : y[11]
        // xmm2 - y[12] : y[13] : y[14] : y[15]

        // y[ 8] : y[10] : y[12] : y[14]
        // 0.0f  : y[ 9] : y[11] : y[13]

        movaps    xmm3 , xmm1            // xmm3 - y[ 8] : y[ 9] : y[10] : y[11]
        shufps    xmm3 , xmm2 , 0x88    // xmm3 - y[ 8] : y[10] : y[12] : y[14] - 10 00 10 00
        shufps    xmm1 , xmm2 , 0xDD    // xmm1 - y[ 9] : y[11] : y[13] : y[15] - 11 01 11 01
        movaps    xmm0 , xmm1            // xmm0 - y[ 9] : y[11] : y[13] : y[15]
        shufps    xmm0 , xmm0    , 0x90    // xmm0 - y[ 9] : y[ 9] : y[11] : y[13] - 10 01 00 00
        andps    xmm0 , [edx + 16]    // xmm0 - 0.0f  : y[ 9] : y[11] : y[13]
        addps    xmm1 , xmm3            // xmm1 - y[9]+y[8] : y[11]+y[10] : y[13]+y[12] : y[15]+y[14] = y[0] : y[1] : y[2] : y[3]
        addps    xmm3 , xmm0            // xmm3 - 0.0f+y[8] : y[9]+y[10] : y[11]+y[12] : y[13]+y[14]  = y[4] : y[5] : y[6] : y[7]

        // xmm1 - y[0] : y[1] : y[2] : y[3]

        //* y[ 8] = y[0] + y[1];
        //* y[ 9] = y[2] + y[3];
        //* y[10] = y[0];
        //* y[11] = y[1] + y[2];

        movaps    xmm0 , xmm1            // xmm0 - y[0] : y[1] : y[2] : y[3]
        shufps    xmm0 , xmm0    , 0x48    // xmm0 - y[0] : y[2] : y[0] : y[1] - 01 00 10 00
        shufps    xmm1 , xmm1 , 0x8D    // xmm1 - y[1] : y[3] : y[0] : y[2] - 10 00 11 01
        andps    xmm1 , [edx + 32]    // xmm1 - y[1] : y[3] : 0.0f : y[2]
        addps    xmm0 , xmm1            // xmm0 - y[0]+y[1] : y[2]+y[3] : y[0]+0.0f : y[1]+y[2] = y[8] : y[9] : y[10] : y[11]

        //* Y[ 8] = (y[8] * c_Cos_1Pi8) + (y[9] * c_Cos_3Pi8);
        //* Y[ 9] = (y[8] * c_Cos_3Pi8) - (y[9] * c_Cos_1Pi8);
        //* Y[10] = y[10] + (y[11] * c_Cos1P4);
        //* Y[11] = y[10] - (y[11] * c_Cos1P4);

        movaps    xmm1 , xmm0            // xmm1 - y[ 8] : y[ 9] : y[10] : y[11]
        shufps    xmm0 , xmm0 , 0xA0    // xmm0 - y[ 8] : y[ 8] : y[10] : y[10] - 10 10 00 00
        shufps    xmm1 , xmm1 , 0xF5    // xmm1 - y[ 9] : y[ 9] : y[11] : y[11] - 11 11 01 01
        mulps    xmm0 , [ecx + 48]    // xmm0 - y[8]*c_Cos_1Pi8 : y[8]*c_Cos_3Pi8 : y[10] : y[10]
        mulps    xmm1 , [ecx + 64]    // xmm1 - y[9]*c_Cos_3Pi8 : y[9]*-c_Cos_1Pi8 : y[11]*c_Cos_1Pi4 : y[11]*-c_Cos_1Pi4
        addps    xmm0 , xmm1            // xmm0 - Y[ 8] : Y[ 9] : Y[10] : Y[11]

        //* Y[0] = c_Delta_0Pi4 * (Y[ 8] + Y[10]);
        //* Y[1] = c_Delta_1Pi4 * (Y[ 9] + Y[11]);
        //* Y[2] = c_Delta_2Pi4 * (Y[11] - Y[ 9]);
        //* Y[3] = c_Delta_3Pi4 * (Y[10] - Y[ 8]);

        movaps    xmm1 , xmm0            // xmm1 - Y[ 8] : Y[ 9] : Y[10] : Y[11]
        shufps    xmm1 , xmm1 , 0x1E    // xmm1 - Y[10] : Y[11] : Y[ 9] : Y[ 8] - 00 01 11 10
        xorps    xmm1 , [edx]        // xmm1 - Y[10] : Y[11] : -Y[9] : -Y[8]
        shufps    xmm0 , xmm0 , 0xB4    // xmm0 - Y[ 8] : Y[ 9] : Y[11] : Y[10] - 10 11 01 00
        addps    xmm0 , xmm1            // xmm0 - Y[8]+Y[10] : Y[9]+Y[11] : Y[11]-Y[9] : Y[10]-Y[8]
        mulps    xmm0 , [ecx + 80]    // xmm0 - Y[0] : Y[1] : Y[2] : Y[3]

        //* Y[ 8] = (y[5] * c_Cos_1Pi8) + (y[7] * c_Cos_3Pi8);
        //* Y[ 9] = (y[5] * c_Cos_3Pi8) - (y[7] * c_Cos_1Pi8);
        //* Y[10] = y[4] + (y[6] * c_Cos_1Pi4);
        //* Y[11] = y[4] - (y[6] * c_Cos_1Pi4);

        // xmm2 - y[4] : y[5] : y[6] : y[7]

        movaps    xmm1 , xmm3            // xmm1 - y[ 4] : y[ 5] : y[ 6] : y[ 7]
        shufps    xmm1 , xmm1    , 0x05    // xmm1 - y[ 5] : y[ 5] : y[ 4] : y[ 4] - 00 00 01 01
        shufps    xmm3 , xmm3 , 0xAF    // xmm3 - y[ 7] : y[ 7] : y[ 6] : y[ 6] - 10 10 11 11
        mulps    xmm1 , [ecx + 48]    // xmm1 - y[5]*c_Cos_1Pi8 : y[5]*c_Cos_3Pi8 : y[4] : y[4]
        mulps    xmm3 , [ecx + 64]    // xmm3 - y[7]*c_Cos_3Pi8 : y[7]*-c_Cos_1Pi8 : y[6]*c_Cos_1Pi4 : y[6]*-c_Cos_1Pi4
        addps    xmm1 , xmm3            // xmm1 - Y[8] : Y[9] : Y[10] : Y[11]

        //* Y[4] = Y[ 8] + Y[10];
        //* Y[5] = Y[ 9] + Y[11];
        //* Y[6] = Y[11] - Y[ 9];
        //* Y[7] = Y[10] - Y[ 8];

        movaps    xmm2 , xmm1            // xmm2 - Y[ 8] : Y[ 9] : Y[10] : Y[11]
        shufps    xmm1 , xmm1    , 0xB4    // xmm1 - Y{ 8] : Y[ 9] : Y[11] : Y[10] - 10 11 01 00
        shufps    xmm2 , xmm2 , 0x1E    // xmm2 - Y[10] : Y[11] : Y[ 9] : Y[ 8] - 00 01 11 10
        xorps    xmm2 , [edx]        // xmm2 - Y[10] : Y[11] : -Y[9] : -Y[8]
        addps    xmm1 , xmm2            // xmm1 - Y[ 4] : Y[ 5] : Y[ 6] : Y[ 7]

        //* X[1] = c_Delta_0Pi8 * (Y[0] + Y[4]);
        //* X[3] = c_Delta_1Pi8 * (Y[1] + Y[5]);
        //* X[5] = c_Delta_2Pi8 * (Y[2] + Y[6]);
        //* X[7] = c_Delta_3Pi8 * (Y[3] + Y[7]);
        //* X[9] = c_Delta_4Pi8 * (Y[7] - Y[3]);
        //* X[11] = c_Delta_5Pi8 * (Y[6] - Y[2]);
        //* X[13] = c_Delta_6Pi8 * (Y[5] - Y[1]);
        //* X[15] = c_Delta_7Pi8 * (Y[4] - Y[0]);

        // xmm0 - Y[ 0] : Y[ 1] : Y[ 2] : Y[ 3]
        // xmm1 - Y[ 4] : Y[ 5] : Y[ 6] : Y[ 7]

        movaps    xmm2 , xmm0            // xmm2 - Y[0] : Y[1] : Y[2] : Y[3]
        movaps    xmm3 , xmm1            // xmm3 - Y[4] : Y[5] : Y[6] : Y[7]
        shufps    xmm2 , xmm2 , 0x1B    // xmm2 - Y[3] : Y[2] : Y[1] : Y[0] - 00 01 10 11
        shufps    xmm3 , xmm3 , 0x1B    // xmm3 - Y[7] : Y[6] : Y[5] : Y[4] - 00 01 10 11
        addps    xmm0 , xmm1            // xmm0 - Y[0]+Y[4] : Y[1]+Y[5] : Y[2]+Y[6] : Y[3]+Y[7]
        subps    xmm3 , xmm2            // xmm3 - Y[4]-Y[3] : Y[5]-Y[2] : Y[6]-Y[1] : Y[7]-Y[0]
        mulps    xmm0 , [ecx + 96]    // xmm0 - X[1] : X[3] : X[5] : X[7]
        mulps    xmm3 , [ecx + 112]    // xmm3 - X[9] : X[11] : X[13] : X[15]

        movaps    xmm1 , [ebx]        // xmm1 - X[ 0] : X[ 8] : X[ 4] : X[12]
        movaps    xmm2 , [ebx+16]        // xmm2 - X[ 2] : X[ 6] : X[14] : X[10]

        // xmm0 - X[ 1] : X[ 3] : X[ 5] : X[ 7]
        // xmm1 - X[ 0] : X[ 8] : X[ 4] : X[12]
        // xmm2 - X[ 2] : X[ 6] : X[10] : X[14]
        // xmm3 - X[ 9] : X[11] : X[13] : X[15]

        // X[ 0] : X[ 2] : X[ 4] : X[ 6]
        // X[ 8] : X[10] : X[12] : X[14]

        movaps    xmm4 , xmm1            // xmm4 - X[ 0] : X[ 8] : X[ 4] : X[12]
        shufps    xmm4 , xmm2 , 0x48    // xmm4 - X[ 0] : X[ 4] : X[ 2] : X[ 6] - 01 00 10 00
        shufps    xmm4 , xmm4 , 0xD8    // xmm4 - X[ 0] : X[ 2] : X[ 4] : X[ 6] - 11 01 10 00
        shufps    xmm1 , xmm2 , 0xED    // xmm1 - X[ 8] : X[12] : X[10] : X[14] - 11 10 11 01
        shufps    xmm1 , xmm1 , 0xD8    // xmm1 - X[ 8] : X[10] : X[12] : X[14] - 11 01 10 00

        // xmm0 - X[ 1] : X[ 3] : X[ 5] : X[ 7]
        // xmm4 - X[ 0] : X[ 2] : X[ 4] : X[ 6]
        // xmm1 - X[ 8] : X[10] : X[12] : X[14]
        // xmm3 - X[ 9] : X[11] : X[13] : X[15]

        movaps    xmm2 , xmm0         // xmm2 - X[ 1] : X[ 3] : X[ 5] : X[ 7]
        unpckhps    xmm2 , xmm4        // xmm2 - X[ 5] : X[ 4] : X[ 7] : X[ 6]
        unpcklps    xmm0 , xmm4        // xmm0 - X[ 1] : X[ 0] : X[ 3] : X[ 2]
        shufps    xmm0 , xmm0 , 0xB1    // xmm0 - X[ 0] : X[ 1] : X[ 2] : X[ 3] - 10 11 00 01
        shufps    xmm2 , xmm2 , 0xB1    // xmm2 - X[ 4] : X[ 5] : X[ 6] : X[ 7] - 10 11 00 01

        movaps    xmm4 , xmm3            // xmm4 - X[ 9] : X[11] : X[13] : X[15]
        unpckhps    xmm4 , xmm1        // xmm4 - X[13] : X[12] : X[15] : X[14]
        unpcklps    xmm3 , xmm1        // xmm3 - X[ 9] : X[ 8] : X[11] : X[10]
        shufps    xmm3 , xmm3 , 0xB1    // xmm3 - X[ 8] : X[ 9] : X[10] : X[11] - 10 11 00 01
        shufps    xmm4 , xmm4 , 0xB1    // xmm4 - X[12] : X[13] : X[14] : X[15] - 10 11 00 01

        mov        eax , X
        movaps    [eax] , xmm0
        movaps    [eax+16] , xmm2
        movaps    [eax+32] , xmm3
        movaps    [eax+48] , xmm4

        emms
        popad
    }
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::T3L16_IntelSIMD(tfloat32 *x,tfloat32 *X,tfloat32 *y)
{
    tfloat32 *co = m_DCTCoefficients_IntelSIMD;
    tuint *mask = m_DCTMasks_IntelSIMD;

    __asm
    {
        pushad
        emms

        mov        eax , x
        mov        ebx , y
        mov        ecx , co
        mov        edx , mask

        // x[ 1] : x[ 5] : x[ 9] : x[13]
        // x[ 3] : x[ 7] : x[11] : x[15]
        // x[ 2] : x[10] : x[ 4] : x[ 8]
        // x[ 6] : x[14] : x[12] : x[ 0]

        movaps    xmm0 , [eax]        // xmm0 - x[ 0] : x[ 1] : x[ 2] : x[ 3]
        movaps    xmm1 , [eax + 16]    // xmm1 - x[ 4] : x[ 5] : x[ 6] : x[ 7]
        movaps    xmm2 , [eax + 32]    // xmm2 - x[ 8] : x[ 9] : x[10] : x[11]
        movaps    xmm3 , [eax + 48]    // xmm3 - x[12] : x[13] : x[14] : x[15]

        movaps    xmm4 , xmm0            // xmm4 - x[ 0] : x[ 1] : x[ 2] : x[ 3]
        shufps    xmm4 , xmm1 , 0xDD    // xmm4 - x[ 1] : x[ 3] : x[ 5] : x[ 7] - 11 01 11 01
        shufps    xmm0 , xmm1 , 0x88    // xmm0 - x[ 0] : x[ 2] : x[ 4] : x[ 6] - 10 00 10 00
        movaps    xmm5 , xmm2            // xmm5 - x[ 8] : x[ 9] : x[10] : x[11]
        shufps    xmm5 , xmm3 , 0xDD    // xmm5 - x[ 9] : x[11] : x[13] : x[15] - 11 01 11 01
        shufps    xmm2 , xmm3 , 0x88    // xmm2 - x[ 8] : x[10] : x[12] : x[14] - 10 00 10 00

        // xmm4 - x[ 1] : x[ 3] : x[ 5] : x[ 7]
        // xmm5 - x[ 9] : x[11] : x[13] : x[15]

        movaps    xmm1 , xmm4            // xmm1 - x[ 1] : x[ 3] : x[ 5] : x[ 7]
        shufps    xmm1 , xmm5 , 0x88    // xmm1 - x[ 1] : x[ 5] : x[ 9] : x[13] - 10 00 10 00
        shufps    xmm4 , xmm5 , 0xDD    // xmm4 - x[ 3] : x[ 7] : x[11] : x[15] - 11 01 11 01

        // xmm0 - x[ 0] : x[ 2] : x[ 4] : x[ 6]
        // xmm2 - x[ 8] : x[10] : x[12] : x[14]

        movaps    xmm3 , xmm0            // xmm3 - x[ 0] : x[ 2] : x[ 4] : x[ 6]
        shufps    xmm3 , xmm2 , 0x49    // xmm3 - x[ 2] : x[ 4] : x[ 8] : x[10] - 01 00 10 01
        shufps    xmm3 , xmm3 , 0x9C    // xmm3 - x[ 2] : x[10] : x[ 4] : x[ 8] - 10 01 11 00
        movaps    [ebx] , xmm3        // (ebx) - x[ 2] : x[10] : x[ 4] : x[ 8]
        shufps    xmm0 , xmm2 , 0xEC    // xmm0 - x[ 0] : x[ 6] : x[12] : x[14] - 11 10 11 00
        shufps    xmm0 , xmm0 , 0x2D    // xmm0 - x[ 6] : x[14] : x[12] : x[ 0] - 00 10 11 01
        movaps    [ebx + 16] , xmm0    // (ebx+16) - x[ 6] : x[14] : x[12] : x[ 0]


        //* y[0] = x[1] + x[3];
        //* y[1] = x[5] + x[7];
        //* y[2] = x[9] + x[11];
        //* y[3] = x[13] + x[15];
        //* y[4] = 0.0f + x[1];
        //* y[5] = x[3] + x[5];
        //* y[6] = x[7] + x[9];
        //* y[7] = x[11] + x[13];

        // xmm1 - x[ 1] : x[ 5] : x[ 9] : x[13]
        // xmm4 - x[ 3] : x[ 7] : x[11] : x[15]

        movaps    xmm0 , xmm4            // xmm0 - x[ 3] : x[ 7] : x[11] : x[15]
        shufps    xmm4 , xmm4 , 0x90    // xmm4 - x[ 3] : x[ 3] : x[ 7] : x[11] - 10 01 00 00
        andps    xmm4 , [edx + 16]    // xmm4 - 0.0f  : x[ 3] : x[ 7] : x[11]
        addps    xmm0 , xmm1            // xmm0 - x[1]+x[3] : x[5]+x[7] : x[9]+x[11] : x[13]+x[15] - y[0] : y[1] : y[2] : y[3]
        addps    xmm4 , xmm1            // xmm4 - x[1] : x[3]+x[5] : x[7]+x[9] : x[11]+x[13] - y[4] : y[5] : y[6] : y[7]

        // xmm0 - y[0] : y[1] : y[2] : y[3]
        // xmm4 - y[4] : y[5] : y[6] : y[7]

        //* y[ 8] = y[0] + y[1];
        //* y[ 9] = y[2] + y[3];
        //* y[10] = y[0];
        //* y[11] = y[1] + y[2];

        movaps    xmm1 , xmm0            // xmm1 - y[0] : y[1] : y[2] : y[3]
        shufps    xmm1 , xmm1    , 0x48    // xmm1 - y[0] : y[2] : y[0] : y[1] - 01 00 10 00
        shufps    xmm0 , xmm0 , 0x9D    // xmm0 - y[1] : y[3] : y[1] : y[2] - 10 01 11 01
        andps    xmm0 , [edx + 32]    // xmm0 - y[1] : y[3] : 0.0f : y[2]
        addps    xmm1 , xmm0            // xmm1 - y[0]+y[1] : y[2]+y[3] : y[0] : y[1]+y[2] = y[8] : y[9] : y[10] : y[11]

        //* Y[16] = (y[8] * D_Cos_1Pi8) + (y[9] * D_Cos_3Pi8);
        //* Y[17] = (y[8] * D_Cos_3Pi8) - (y[9] * D_Cos_1Pi8);
        //* Y[18] = y[10] + (y[11] * D_Cos_1Pi4);
        //* Y[19] = y[10] - (y[11] * D_Cos_1Pi4);

        movaps    xmm0 , xmm1            // xmm0 - y[ 8] : y[ 9] : y[10] : y[11]
        shufps    xmm0 , xmm0 , 0xA0    // xmm0 - y[ 8] : y[ 8] : y[10] : y[10] - 10 10 00 00
        shufps    xmm1 , xmm1 , 0xF5    // xmm1 - y[ 9] : y[ 9] : y[11] : y[11] - 11 11 01 01
        mulps    xmm0 , [ecx + 48]    // xmm0 - y[8]*D_Cos_1Pi8 : y[8]*D_Cos_3Pi8 : y[10] : y[10]
        mulps    xmm1 , [ecx + 64]    // xmm1 - y[9]*D_Cos_3Pi8 : y[9]*-D_Cos_1Pi8 : y[11]*D_Cos_1Pi4 : y[11]*-D_Cos_1Pi4
        addps    xmm0 , xmm1            // xmm0 - Y[16] : Y[17] : Y[18] : Y[19]

        //* Y[20] = D_Delta_0Pi4 * (Y[16] + Y[18]);
        //* Y[21] = D_Delta_1Pi4 * (Y[17] + Y[19]);
        //* Y[22] = D_Delta_2Pi4 * (Y[19] - Y[17]);
        //* Y[23] = D_Delta_3Pi4 * (Y[18] - Y[16]);

        movaps    xmm1 , xmm0            // xmm1 - Y[16] : Y[17] : Y[18] : Y[19]
        shufps    xmm1 , xmm1 , 0x1E    // xmm1 - Y[18] : Y[19] : Y[17] : Y[16] - 00 01 11 10
        shufps    xmm0 , xmm0 , 0xB4    // xmm0 - Y[16] : Y[17] : Y[19] : Y[18] - 10 11 01 00
        xorps    xmm1 , [edx]        // xmm1 - Y[18] : Y[19] : -Y[17] : -Y[16]
        addps    xmm0 , xmm1            // xmm0 - Y[16]+Y[18] : Y[17]+Y[19] : Y[19]-Y[17] : Y[18]-Y[16]
        mulps    xmm0 , [ecx + 80]    // xmm0 - Y[20] : Y[21] : Y[22] : Y[23]

        //* Y[ 8] = (y[5] * D_Cos_1Pi8) + (y[7] * D_Cos_3Pi8);
        //* Y[ 9] = (y[5] * D_Cos_3Pi8) - (y[7] * D_Cos_1Pi8);
        //* Y[10] = y[4] + (y[6] * D_Cos_1Pi4);
        //* Y[11] = y[4] - (y[6] * D_Cos_1Pi4);

        movaps    xmm5 , xmm4            // xmm5 - y[4] : y[5] : y[6] : y[7]
        shufps    xmm4 , xmm4 , 0x05    // xmm4 - y[5] : y[5] : y[4] : y[4] - 00 00 01 01
        shufps    xmm5 , xmm5 , 0XAF    // xmm5 - y[7] : y[7] : y[6] : y[6] - 10 10 11 11
        mulps    xmm4 , [ecx + 48]    // xmm4 - y[5]*D_Cos_1Pi8 : y[5]*D_Cos_3Pi8 : y[4] : y[4]
        mulps    xmm5 , [ecx + 64]    // xmm5 - y[7]*D_Cos_3Pi8 : y[7]*-D_Cos_1Pi8 : y[6]*D_Cos_1Pi4 : y[6]*-D_Cos_1Pi4
        addps    xmm4 , xmm5            // xmm4 - Y[ 8] : Y[ 9] : Y[10] : Y[11]

        //* Y[16] = Y[ 8] + Y[10];
        //* Y[17] = Y[ 9] + Y[11];
        //* Y[18] = Y[11] - Y[ 9];
        //* Y[19] = Y[10] - Y[ 8];

        movaps    xmm5 , xmm4            // xmm5 - Y[ 8] : Y[ 9] : Y[10] : Y[11]
        shufps    xmm4 , xmm4 , 0xB4    // xmm4 - Y[ 8] : Y[ 9] : Y[11] : Y[10] - 10 11 01 00
        shufps    xmm5 , xmm5 , 0x1E    // xmm5 - Y[10] : Y[11] : Y[ 9] : Y[ 8] - 00 01 11 10
        xorps    xmm5 , [edx]        // xmm5 - Y[10] : Y[11] : -Y[9] : -Y[8]
        addps    xmm4 , xmm5            // xmm4 - Y[16] : Y[17] : Y[18] : Y[19]

        //* Y[0] = D_Delta_0Pi8 * (Y[20] + Y[16]);
        //* Y[1] = D_Delta_1Pi8 * (Y[21] + Y[17]);
        //* Y[2] = D_Delta_2Pi8 * (Y[22] + Y[18]);
        //* Y[3] = D_Delta_3Pi8 * (Y[23] + Y[19]);
        //* Y[4] = D_Delta_4Pi8 * (Y[19] - Y[23]);
        //* Y[5] = D_Delta_5Pi8 * (Y[18] - Y[22]);
        //* Y[6] = D_Delta_6Pi8 * (Y[17] - Y[21]);
        //* Y[7] = D_Delta_7Pi8 * (Y[16] - Y[20]);

        // xmm4 - Y[16] : Y[17] : Y[18] : Y[19]
        // xmm0 - Y[20] : Y[21] : Y[22] : Y[23]

        movaps    xmm1 , xmm4            // xmm1 - Y[16] : Y[17] : Y[18] : Y[19]
        shufps    xmm1 , xmm1 , 0x1B    // xmm1 - Y[19] : Y[18] : Y[17] : Y[16] - 00 01 10 11
        movaps    xmm2 , xmm0            // xmm2 - Y[20] : Y[21] : Y[22] : Y[23]
        shufps    xmm2 , xmm2 , 0x1B    // xmm2 - Y[23] : Y[22] : Y[21] : Y[20] - 00 01 10 11
        addps    xmm0 , xmm4            // xmm0 - Y[20]+Y[16] : Y[21]+Y[17] : Y[22]+Y[18] : Y[23]+Y[19]
        subps    xmm1 , xmm2            // xmm1 - Y[19]-Y[23] : Y[18]-Y[22] : Y[17]-Y[21] : Y[16]-Y[20]
        mulps    xmm0 , [ecx + 96]    // xmm0 - Y[ 0] : Y[ 1] : Y[ 2] : Y[ 3]
        mulps    xmm1 , [ecx + 112]    // xmm1 - Y[ 4] : Y[ 5] : Y[ 6] : Y[ 7]

        //* y[0] = x[2] + x[6];
        //* y[1] = x[10] + x[14];
        //* y[2] = x[2];
        //* y[3] = x[10] + x[6];

        movaps    xmm2 , [ebx]        // xmm2 - x[ 2] : x[10] : x[ 4] : x[ 8]
        movaps    xmm3 , [ebx + 16]    // xmm3 - x[ 6] : x[14] : x[12] : x[ 0]
        movaps    xmm4 , xmm2            // xmm4 - x[ 2] : x[10] : x[ 4] : x[ 8]
        shufps    xmm4 , xmm4 , 0x44    // xmm4 - x[ 2] : x[10] : x[ 2] : x[10] - 01 00 01 00
        movaps    xmm5 , xmm3            // xmm5 - x[ 6] : x[14] : x[12] : x[ 0]
        shufps    xmm5 , xmm5    , 0x14    // xmm5 - x[ 6] : x[14] : x[14] : x[ 6] - 00 01 01 00
        andps    xmm5 , [edx + 32]    // xmm5 - x[ 6] : x[14] : 0.0f  : x[ 6]
        addps    xmm4 , xmm5            // xmm4 - x[2]+x[6] : x[10]+x[14] : x[2] : x[10]+x[6] - y[0] : y[1] : y[2] : y[3]

        //* Y[16] = (y[0] * D_Cos_1Pi8) + (y[1] * D_Cos_3Pi8);
        //* Y[17] = (y[0] * D_Cos_3Pi8) - (y[1] * D_Cos_1Pi8);
        //* Y[18] = y[2] + (y[3] * D_Cos_1Pi4);
        //* Y[19] = y[2] - (y[3] * D_Cos_1Pi4);

        movaps    xmm5 , xmm4            // xmm5 - y[ 0] : y[ 1] : y[ 2] : y[ 3]
        shufps    xmm4 , xmm4 , 0xA0    // xmm4 - y[ 0] : y[ 0] : y[ 2] : y[ 2] - 10 10 00 00
        shufps    xmm5 , xmm5 , 0xF5    // xmm5 - y[ 1] : y[ 1] : y[ 3] : y[ 3] - 11 11 01 01
        mulps    xmm4 , [ecx + 48]    // xmm4 - y[0]*D_Cos_1Pi8 : y[0]*D_Cos_3Pi8 : y[2] : y[2]
        mulps    xmm5 , [ecx + 64]    // xmm5 - y[1]*D_Cos_3Pi8 : y[1]*-D_Cos_1Pi8 : y[3]*D_Cos_1Pi4 : y[3]*-D_Cos_1Pi4
        addps    xmm4 , xmm5            // xmm4 - Y[16] : Y[17] : Y[18] : Y[19]

        //* Y[20] = D_Delta_0Pi4 * (Y[16] + Y[18]);
        //* Y[21] = D_Delta_1Pi4 * (Y[17] + Y[19]);
        //* Y[22] = D_Delta_2Pi4 * (Y[19] - Y[17]);
        //* Y[23] = D_Delta_3Pi4 * (Y[18] - Y[16]);

        movaps    xmm5 , xmm4            // xmm5 - Y[16] : Y[17] : Y[18] : Y[19]
        shufps    xmm4 , xmm4 , 0xB4    // xmm4 - Y[16] : Y[17] : Y[19] : Y[18] - 10 11 01 00
        shufps    xmm5 , xmm5 , 0x1E    // xmm5 - Y[18] : Y[19] : Y[17] : Y[16] - 00 01 11 10
        xorps    xmm5 , [edx]        // xmm5 - Y[18] : Y[19] : -Y[17] : -Y[16]
        addps    xmm4 , xmm5            // xmm4 - Y[16]+Y[18] : Y[17]+Y[19] : Y[19]-Y[17] : Y[18]-Y[16]
        mulps    xmm4 , [ecx + 80]    // xmm4 - Y[20] : Y[21] : Y[22] : Y[23]

        //* Y[24] =  (x[4] * D_Cos_1Pi8) + (x[12] * D_Cos_3Pi8);
        //* Y[25] =  (x[4] * D_Cos_3Pi8) - (x[12] * D_Cos_1Pi8);
        //* Y[26] =  (x[8] * D_Cos_1Pi4) + x[0]
        //* Y[27] = -(x[8] * D_Cos_1Pi4) + x[0];

        // xmm2 - x[ 2] : x[10] : x[ 4] : x[ 8]
        // xmm3 - x[ 6] : x[14] : x[12] : x[ 0]

        shufps    xmm2 , xmm2 , 0xFA        // xmm2 - x[ 4] : x[ 4] : x[ 8] : x[ 8] - 11 11 10 10
        shufps    xmm3 , xmm3 , 0xFA        // xmm3 - x[12] : x[12] : x[ 0] : x[ 0] - 11 11 10 10
        mulps    xmm2 , [ecx + 128]        // xmm2 - x[4]*D_Cos_1Pi8 : x[4]*D_Cos_3Pi8 : x[8]*D_Cos_1Pi4 : x[8]*-D_Cos_1Pi4
        mulps    xmm3 , [ecx + 144]        // xmm3 - x[12]*D_Cos_3Pi8 : x[12]*D_Cos_1Pi8 : x[0] : x[0]
        addps    xmm2 , xmm3                // xmm2 - Y[24] : Y[25] : Y[26] : Y[27]

        //* Y[16] = Y[24] + Y[26];
        //* Y[17] = Y[25] + Y[27];
        //* Y[18] = Y[27] - Y[25];
        //* Y[19] = Y[26] - Y[24];

        movaps    xmm3 , xmm2                // xmm3 - Y[24] : Y[25] : Y[26] : Y[27]
        shufps    xmm2 , xmm2 , 0xB4        // xmm2 - Y[24] : Y[25] : Y[27] : Y[26] - 10 11 01 00
        shufps    xmm3 , xmm3 , 0x1E        // xmm3 - Y[26] : Y[27] : Y[25] : Y[24] - 00 01 11 10
        xorps    xmm3 , [edx]            // xmm3 - Y[26] : Y[27] : -Y[25] : -Y[24]
        addps    xmm2 , xmm3                // xmm2 - Y[24]+Y[26] : Y[25]+Y[27] : Y[27]-Y[25] : Y[26]-Y[24] = Y[16] : Y[17] : Y[18] : Y[19]

        //* Y[ 8] = Y[20] + Y[16];
        //* Y[ 9] = Y[21] + Y[17];
        //* Y[10] = Y[22] + Y[18];
        //* Y[11] = Y[23] + Y[19];
        //* Y[12] = Y[19] - Y[23];
        //* Y[13] = Y[18] - Y[22];
        //* Y[14] = Y[17] - Y[21];
        //* Y[15] = Y[16] - Y[20];

        // xmm2 - Y[16] : Y[17] : Y[18] : Y[19]
        // xmm4 - Y[20] : Y[21] : Y[22] : Y[23]

        movaps    xmm3 , xmm2                // xmm3 - Y[16] : Y[17] : Y[18] : Y[19]
        shufps    xmm3 , xmm3 , 0x1B        // xmm3 - Y[19] : Y[18] : Y[17] : Y[16] - 00 01 10 11
        movaps    xmm5 , xmm4                // xmm5 - Y[20] : Y[21] : Y[22] : Y[23]
        shufps    xmm5 , xmm5 , 0x1B        // xmm5 - Y[23] : Y[22] : Y[21] : Y[20] - 00 01 10 11
        addps    xmm4 , xmm2                // xmm4 - Y[20]+Y[16] : Y[21]+Y[17] : Y[22]+Y[18] : Y[23]+Y[19] = Y[ 8] : Y[ 9] : Y[10] : Y[11]
        subps    xmm3 , xmm5                // xmm3 - Y[19]-Y[23] : Y[18]-Y[22] : Y[17]-Y[21] : Y[16]-Y[20] = Y[12] : Y[13] : Y[14] : Y[15]

        //* X[ 0] = Y[0] + Y[8];
        //* X[ 1] = Y[1] + Y[9];
        //* X[ 2] = Y[2] + Y[10];
        //* X[ 3] = Y[3] + Y[11];
        //* X[ 4] = Y[4] + Y[12];
        //* X[ 5] = Y[5] + Y[13];
        //* X[ 6] = Y[6] + Y[14];
        //* X[ 7] = Y[7] + Y[15];
        //* X[ 8] = Y[15] - Y[7];
        //* X[ 9] = Y[14] - Y[6];
        //* X[10] = Y[13] - Y[5];
        //* X[11] = Y[12] - Y[4];
        //* X[12] = Y[11] - Y[3];
        //* X[13] = Y[10] - Y[2];
        //* X[14] = Y[9] - Y[1];
        //* X[15] = Y[8] - Y[0];

        // xmm0 - Y[ 0] : Y[ 1] : Y[ 2] : Y[ 3]
        // xmm1 - Y[ 4] : Y[ 5] : Y[ 6] : Y[ 7]
        // xmm4 - Y[ 8] : Y[ 9] : Y[10] : Y[11]
        // xmm3 - Y[12] : Y[13] : Y[14] : Y[15]

        movaps    xmm2 , xmm3                // xmm2 - Y[12] : Y[13] : Y[14] : Y[15]
        shufps    xmm2 , xmm2 , 0x1B        // xmm2 - Y[15] : Y[14] : Y[13] : Y[12] - 00 01 10 11
        movaps    xmm5 , xmm4                // xmm4 - Y[ 8] : Y[ 9] : Y[10] : Y[11]
        shufps    xmm5 , xmm5 , 0x1B        // xmm5 - Y[11] : Y[10] : Y[ 9] : Y[ 8] - 00 01 10 11
        movaps    xmm6 , xmm1                // xmm1 - Y[ 4] : Y[ 5] : Y[ 6] : Y[ 7]
        shufps    xmm6 , xmm6 , 0x1B        // xmm6 - Y[ 7] : Y[ 6] : Y[ 5] : Y[ 4] - 00 01 10 11
        movaps    xmm7 , xmm0                // xmm7 - Y[ 0] : Y[ 1] : Y[ 2] : Y[ 3]
        shufps    xmm7 , xmm7 , 0x1B        // xmm7 - Y[ 3] : Y[ 2] : Y[ 1] : Y[ 0] - 00 01 10 11
        addps    xmm0 , xmm4                // xmm0 - Y[ 0]+Y[ 8] : Y[ 1]+Y[ 9] : Y[ 2]+Y[10] : Y[ 3]+Y[11] = X[ 0] : X[ 1] : X[ 2] : X[ 3]
        addps    xmm1 , xmm3                // xmm1 - Y[ 4]+Y[12] : Y[ 5]+Y[13] : Y[ 6]+Y[14] : Y[ 7]+Y[15] = X[ 4] : X[ 5] : X[ 6] : X[ 7]
        subps    xmm2 , xmm6                // xmm2 - Y[15]-Y[ 7] : Y[14]-Y[ 6] : Y[13]-Y[ 5] : Y[12]-Y[ 4] = X[ 8] : X[ 9] : X[10] : X[11]
        subps    xmm5 , xmm7                // xmm5 - Y[11]-Y[ 3] : Y[10]-Y[ 2] : Y[ 9]-Y[ 1] : Y[ 8]-Y[ 0] = X[12] : X[13] : X[14] : X[15]

        mov        eax , X
        movaps    [eax] , xmm0            // (eax +  0) : X[ 0] : X[ 1] : X[ 2] : X[ 3]
        movaps    [eax + 16] , xmm1        // (eax + 16) : X[ 4] : X[ 5] : X[ 6] : X[ 7]
        movaps    [eax + 32] , xmm2        // (eax + 32) : X[ 8] : X[ 9] : X[10] : X[11]
        movaps    [eax + 48] , xmm5        // (eax + 48) : X[12] : X[13] : X[14] : X[15]

        emms
        popad
    }
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::T4L16_IntelSIMD(tfloat32 *x,tfloat32 *X,tfloat32 *y)
{
    tfloat32 *co = m_DCTCoefficients_IntelSIMD;
    tuint *mask = m_DCTMasks_IntelSIMD;

    __asm
    {
        pushad
        emms

        mov        eax , x
        mov        ebx , y
        mov        ecx , co
        mov        edx , mask

        //* y[ 0] = x[0] + x[1];
        //* y[ 1] = x[2] + x[3];
        //* y[ 2] = x[4] + x[5];
        //* y[ 3] = x[6] + x[7];
        //* y[ 4] = x[8] + x[9];
        //* y[ 5] = x[10] + x[11];
        //* y[ 6] = x[12] + x[13];
        //* y[ 7] = x[14] + x[15];
        //* y[ 8] =        x[0];
        //* y[ 9] = x[1] + x[2];
        //* y[10] = x[3] + x[4];
        //* y[11] = x[5] + x[6];
        //* y[12] = x[7] + x[8];
        //* y[13] = x[9] + x[10];
        //* y[14] = x[11] + x[12];
        //* y[15] = x[13] + x[14];

        movaps    xmm0 , [eax]                // xmm0 - x[ 0] : x[ 1] : x[ 2] : x[ 3]
        movaps    xmm1 , [eax + 16]            // xmm1 - x[ 4] : x[ 5] : x[ 6] : x[ 7]
        movaps    xmm2 , [eax + 32]            // xmm2 - x[ 8] : x[ 9] : x[10] : x[11]
        movaps    xmm3 , [eax + 48]            // xmm3 - x[12] : x[13] : x[14] : x[15]
        movaps    xmm6 , xmm1                    // xmm6 - x[ 4] : x[ 5] : x[ 6] : x[ 7]
        shufps    xmm6 , xmm2 , 0xDD            // xmm6 - x[ 5] : x[ 7] : x[ 9] : x[11] - 11 01 11 01
        movaps    xmm4 , xmm0                    // xmm4 - x[ 0] : x[ 1] : x[ 2] : x[ 3]
        shufps    xmm4 , xmm1 , 0x88            // xmm4 - x[ 0] : x[ 2] : x[ 4] : x[ 6] - 10 00 10 00
        shufps    xmm0 , xmm1 , 0xDD            // xmm0 - x[ 1] : x[ 3] : x[ 5] : x[ 7] - 11 01 11 01
        movaps    xmm5 , xmm2                    // xmm5 - x[ 8] : x[ 9] : x[10] : x[11]
        shufps    xmm5 , xmm3    , 0x88            // xmm5 - x[ 8] : x[10] : x[12] : x[14] - 10 00 10 00
        shufps    xmm2 , xmm3 , 0xDD            // xmm2 - x[ 9] : x[11] : x[13] : x[15] - 11 01 11 01

        // xmm4 - x[ 0] : x[ 2] : x[ 4] : x[ 6]
        // xmm0 - x[ 1] : x[ 3] : x[ 5] : x[ 7]
        // xmm5 - x[ 8] : x[10] : x[12] : x[14]
        // xmm2 - x[ 9] : x[11] : x[13] : x[15]
        // xmm6 - x[ 5] : x[ 7] : x[ 9] : x[11]

        movaps    xmm7 , xmm0                    // xmm7 - x[ 1] : x[ 3] : x[ 5] : x[ 7]
        shufps    xmm7 , xmm7    , 0x90            // xmm7 - x[ 1] : x[ 1] : x[ 3] : x[ 5] - 10 01 00 00
        andps    xmm7 , [edx + 16]            // xmm7 - 0.0f  : x[ 1] : x[ 3] : x[ 5]
        shufps    xmm6 , xmm2 , 0x99            // xmm6 - x[ 7] : x[ 9] : x[11] : x[13] - 10 01 10 01

        addps    xmm0 , xmm4                    // xmm0 - y[ 0] : y[ 1] : y[ 2] : y[ 3]
        addps    xmm2 , xmm5                    // xmm2 - y[ 4] : y[ 5] : y[ 6] : y[ 7]
        addps    xmm7 , xmm4                    // xmm7 - y[ 8] : y[ 9] : y[10] : y[11]
        addps    xmm6 , xmm5                    // xmm6 - y[12] : y[13] : y[14] : y[15]

        // DO NOT TOUCH xmm6 AND xmm7

        //* y[16] = y[0] + y[1];
        //* y[17] = y[2] + y[3];
        //* y[18] = y[4] + y[5];
        //* y[19] = y[6] + y[7];
        //* y[20] =        y[0];
        //* y[21] = y[1] + y[2];
        //* y[22] = y[3] + y[4];
        //* y[23] = y[5] + y[6];

        // xmm0 - y[ 0] : y[ 1] : y[ 2] : y[ 3]
        // xmm2 - y[ 4] : y[ 5] : y[ 6] : y[ 7]

        movaps    xmm1 , xmm0                    // xmm1 - y[ 0] : y[ 1] : y[ 2] : y[ 3]
        shufps    xmm0 , xmm2 , 0x88            // xmm0 - y[ 0] : y[ 2] : y[ 4] : y[ 6] - 10 00 10 00
        shufps    xmm1 , xmm2 , 0xDD            // xmm1 - y[ 1] : y[ 3] : y[ 5] : y[ 7] - 11 01 11 01
        movaps    xmm2 , xmm1                    // xmm2 - y[ 1] : y[ 3] : y[ 5] : y[ 7]
        shufps    xmm2 , xmm2 , 0x90            // xmm2 - y[ 1] : y[ 1] : y[ 3] : y[ 5] - 10 01 00 00
        andps    xmm2 , [edx + 16]            // xmm2 - 0.0f  : y[ 1] : y[ 3] : y[ 5]
        addps    xmm1 , xmm0                    // xmm1 - y[16] : y[17] : y[18] : y[19]
        addps    xmm2 , xmm0                    // xmm2 - y[20] : y[21] : y[22] : y[23]

        //* y[24] = y[16] + y[17];
        //* y[25] = y[18] + y[19];
        //* y[26] = y[16];
        //* y[27] = y[17] + y[18];

        movaps    xmm0 , xmm1                    // xmm0 - y[16] : y[17] : y[18] : y[19]
        shufps    xmm0 , xmm0 , 0x48            // xmm0 - y[16] : y[18] : y[16] : y[17] - 01 00 10 00
        shufps    xmm1 , xmm1 , 0xBD            // xmm1 - y[17] : y[19] : y[19] : y[18] - 10 11 11 01
        andps    xmm1 , [edx + 32]            // xmm1 - y[17] : y[19] : 0.0f  : y[18]
        addps    xmm0 , xmm1                    // xmm0 - y[24] : y[25] : y[26] : y[27]

        //* Y[16] = (y[24] * D_Cos_1Pi8) + (y[25] * D_Cos_3Pi8);
        //* Y[17] = (y[24] * D_Cos_3Pi8) - (y[25] * D_Cos_1Pi8);
        //* Y[18] = y[26] + (y[27] * D_Cos_1Pi4);
        //* Y[19] = y[26] - (y[27] * D_Cos_1Pi4);

        movaps    xmm1 , xmm0                    // xmm1 - y[24] : y[25] : y[26] : y[27]
        shufps    xmm0 , xmm0 , 0xA0            // xmm0 - y[24] : y[24] : y[26] : y[26] - 10 10 00 00
        shufps    xmm1 , xmm1 , 0xF5            // xmm1 - y[25] : y[25] : y[27] : y[27] - 11 11 01 01
        mulps    xmm0 , [ecx + 48]            // xmm0 - y[24]*D_Cos_1Pi8 : y[24]*D_Cos_3Pi8 : y[26] : y[26]
        mulps    xmm1 , [ecx + 64]            // xmm1 - y[25]*D_Cos_3Pi8 : y[25]*-D_Cos_1Pi8 : y[27]*D_Cos_1Pi4 : y[27]*-D_Cos_1Pi4
        addps    xmm0 , xmm1                    // xmm0 - Y[16] : Y[17] : Y[18] : Y[19]

        //* Y[20] = D_Delta_0Pi4 * (Y[16] + Y[18]);
        //* Y[21] = D_Delta_1Pi4 * (Y[17] + Y[19]);
        //* Y[22] = D_Delta_2Pi4 * (Y[19] - Y[17]);
        //* Y[23] = D_Delta_3Pi4 * (Y[18] - Y[16]);

        movaps    xmm1 , xmm0                    // xmm1 - Y[16] : Y[17] : Y[18] : Y[19]
        shufps    xmm0 , xmm0 , 0xB4            // xmm0 - Y[16] : Y[17] : Y[19] : Y[18] - 10 11 01 00
        shufps    xmm1 , xmm1 , 0x1E            // xmm1 - Y[18] : Y[19] : Y[17] : Y[16] - 00 01 11 10
        xorps    xmm1 , [edx]                // xmm1 - Y[18] : Y[19] : -Y[17] : -Y[16]
        addps    xmm0 , xmm1                    // xmm0 - Y[16]+Y[18] : Y[17]+Y[19] : Y[18]-Y[17] : Y[19]-Y[16]
        mulps    xmm0 , [ecx + 80]            // xmm0 - Y[20] : Y[21] : Y[22] : Y[23]

        //* Y[16] = (y[21] * D_Cos_1Pi8) + (y[23] * D_Cos_3Pi8);
        //* Y[17] = (y[21] * D_Cos_3Pi8) - (y[23] * D_Cos_1Pi8);
        //* Y[18] = y[20] + (y[22] * D_Cos_1Pi4);
        //* Y[19] = y[20] - (y[22] * D_Cos_1Pi4);

        // xmm2 - y[20] : y[21] : y[22] : y[23]

        movaps    xmm1 , xmm2                    // xmm1 - y[20] : y[21] : y[22] : y[23]
        shufps    xmm1 , xmm1 , 0x05            // xmm1 - y[21] : y[21] : y[20] : y[20] - 00 00 01 01
        shufps    xmm2 , xmm2 , 0xAF            // xmm2 - y[23] : y[23] : y[22] : y[22] - 10 10 11 11
        mulps    xmm1 , [ecx + 48]            // xmm1 - y[21]*D_Cos_1Pi8 : y[21]*D_Cos_3Pi8 : y[20] : y[20]
        mulps    xmm2 , [ecx + 64]            // xmm2 - y[23]*D_Cos_3Pi8 : y[23]*-D_Cos_1Pi8 : y[22]*D_Cos_1Pi4 : y[22]*-D_Cos_1Pi4
        addps    xmm1 , xmm2                    // xmm1 - Y[16] : Y[17] : Y[18] : Y[19]

        //* Y[24] = Y[16] + Y[18];
        //* Y[25] = Y[17] + Y[19];
        //* Y[26] = Y[19] - Y[17];
        //* Y[27] = Y[18] - Y[16];

        movaps    xmm2 , xmm1                    // xmm2 - Y[16] : Y[17] : Y[18] : Y[19]
        shufps    xmm1 , xmm1 , 0xB4            // xmm1 - Y[16] : Y[17] : Y[19] : Y[18] - 10 11 01 00
        shufps    xmm2 , xmm2 , 0x1E            // xmm2 - Y[18] : Y[19] : Y[17] : Y[16] - 00 01 11 10
        xorps    xmm2 , [edx]                // xmm2 - Y[18] : Y[19] :-Y[17] :-Y[16]
        addps    xmm1 , xmm2                    // xmm1 - Y[24] : Y[25] : Y[26] : Y[27]

        //* Y[0] = D_Delta_0Pi8 * (Y[20] + Y[24]);
        //* Y[1] = D_Delta_1Pi8 * (Y[21] + Y[25]);
        //* Y[2] = D_Delta_2Pi8 * (Y[22] + Y[26]);
        //* Y[3] = D_Delta_3Pi8 * (Y[23] + Y[27]);
        //* Y[4] = D_Delta_4Pi8 * (Y[27] - Y[23]);
        //* Y[5] = D_Delta_5Pi8 * (Y[26] - Y[22]);
        //* Y[6] = D_Delta_6Pi8 * (Y[25] - Y[21]);
        //* Y[7] = D_Delta_7Pi8 * (Y[24] - Y[20]);

        // xmm0 - Y[20] : Y[21] : Y[22] : Y[23]
        // xmm1 - Y[24] : Y[25] : Y[26] : Y[27]

        movaps    xmm2 , xmm0                    // xmm2 - Y[20] : Y[21] : Y[22] : Y[23]
        movaps    xmm3 , xmm1                    // xmm3 - Y[24] : Y[25] : Y[26] : Y[27]
        shufps    xmm2 , xmm2 , 0x1B            // xmm2 - Y[23] : Y[22] : Y[21] : Y[20] - 00 01 10 11
        shufps    xmm1 , xmm1 , 0x1B            // xmm1 - Y[27] : Y[26] : Y[25] : Y[24] - 00 01 10 11
        addps    xmm0 , xmm3                    // xmm0 - Y[20]+Y[24] : Y[21]+Y[25] : Y[22]+Y[26] : Y[23]+Y[27]
        subps    xmm1 , xmm2                    // xmm1 - Y[27]-Y[23] : Y[26]-Y[22] : Y[25]-Y[21] : Y[24]-Y[20]
        mulps    xmm0 , [ecx + 96]            // xmm0 - Y[ 0] : Y[ 1] : Y[ 2] : Y[ 3]
        mulps    xmm1 , [ecx + 112]            // xmm1 - Y[ 4] : Y[ 5] : Y[ 6] : Y[ 7]

        // DO NOT TOUCH xmm0 AND xmm1

        //* y[16] = y[ 9] + y[11];
        //* y[17] = y[13] + y[15];
        //* y[18] = y[ 9];
        //* y[19] = y[11] + y[13];

        // xmm7 - y[ 8] : y[ 9] : y[10] : y[11]
        // xmm6 - y[12] : y[13] : y[14] : y[15]

        movaps    xmm5 , xmm7                    // xmm5 - y[ 8] : y[ 9] : y[10] : y[11]
        shufps    xmm5 , xmm6 , 0x88            // xmm5 - y[ 8] : y[10] : y[12] : y[14] - 10 00 10 00
        shufps    xmm7 , xmm6 , 0xDD            // xmm7 - y[ 9] : y[11] : y[13] : y[15] - 11 01 11 01
        movaps    xmm6 , xmm7                    // xmm6 - y[ 9] : y[11] : y[13] : y[15]
        shufps    xmm6 , xmm6 , 0x48            // xmm6 - y[ 9] : y[13] : y[ 9] : y[11] - 01 00 10 00
        shufps    xmm7 , xmm7 , 0xBD            // xmm7 - y[11] : y[15] : y[15] : y[13] - 10 11 11 01
        andps    xmm7 , [edx + 32]            // xmm7 - y[11] : y[15] : 0.0f  : y[13]
        addps    xmm6 , xmm7                    // xmm6 - y[16] : y[17] : y[18] : y[19]

        //* Y[20] = (y[16] * D_Cos_1Pi8) + (y[17] * D_Cos_3Pi8);
        //* Y[21] = (y[16] * D_Cos_3Pi8) - (y[17] * D_Cos_1Pi8);
        //* Y[22] = y[18] + (y[19] * D_Cos_1Pi4);
        //* Y[23] = y[18] - (y[19] * D_Cos_1Pi4);

        movaps    xmm7 , xmm6                    // xmm7 - y[16] : y[17] : y[18] : y[19]
        shufps    xmm6 , xmm6 , 0xA0            // xmm6 - y[16] : y[16] : y[18] : y[18] - 10 10 00 00
        shufps    xmm7 , xmm7 , 0xF5            // xmm7 - y[17] : y[17] : y[19] : y[19] - 11 11 01 01
        mulps    xmm6 , [ecx + 48]            // xmm6 - y[16]*D_Cos_1Pi8 : y[16]*D_Cos_3Pi8 : y[18] : y[18]
        mulps    xmm7 , [ecx + 64]             // xmm7 - y[17]*D_Cos_3Pi8 : y[17]*-D_Cos_1Pi8 : y[19]*D_Cos_1Pi4 : y[19]*-D_Cos_1Pi4
        addps    xmm6 , xmm7                    // xmm6 - Y[20] : Y[21] : Y[22] : Y[23]

        //* Y[16] = D_Delta_0Pi4 * (Y[20] + Y[22]);
        //* Y[17] = D_Delta_1Pi4 * (Y[21] + Y[23]);
        //* Y[18] = D_Delta_2Pi4 * (Y[23] - Y[21]);
        //* Y[19] = D_Delta_3Pi4 * (Y[22] - Y[20]);

        movaps    xmm7 , xmm6                    // xmm7 - Y[20] : Y[21] : Y[22] : Y[23]
        shufps    xmm7 , xmm7 , 0xB4            // xmm7 - Y[20] : Y[21] : Y[23] : Y[22] - 10 11 01 00
        shufps    xmm6 , xmm6 , 0x1E            // xmm6 - Y[22] : Y[23] : Y[21] : Y[20] - 00 01 11 10
        xorps    xmm6 , [edx]                // xmm6 - Y[22] : Y[23] :-Y[21] :-Y[20]
        addps    xmm7 , xmm6                    // xmm7 - Y[20]+Y[22] : Y[21]+Y[23] : Y[23]-Y[21] : Y[22]-Y[20]
        mulps    xmm7 , [ecx + 80]            // xmm7 - Y[16] : Y[17] : Y[18] : Y[19]

        //* Y[24] = (y[10] * D_Cos_1Pi8) + (y[14] * D_Cos_3Pi8);
        //* Y[25] = (y[10] * D_Cos_3Pi8) - (y[14] * D_Cos_1Pi8);
        //* Y[26] = y[8] + (y[12] * D_Cos_1Pi4);
        //* Y[27] = y[8] - (y[12] * D_Cos_1Pi4);

        movaps    xmm6 , xmm5                    // xmm6 - y[ 8] : y[10] : y[12] : y[14]
        shufps    xmm5 , xmm5 , 0x05            // xmm5 - y[10] : y[10] : y[ 8] : y[ 8] - 00 00 01 01
        shufps    xmm6 , xmm6 , 0xAF            // xmm6 - y[14] : y[14] : y[12] : y[12] - 10 10 11 11
        mulps    xmm5 , [ecx + 48]            // xmm5 - y[10]*D_Cos_1Pi8 : y[10]*D_Cos_3Pi8 : y[8] : y[8]
        mulps    xmm6 , [ecx + 64]            // xmm6 - y[14]*D_Cos_3Pi8 : y[14]*-D_Cos_1Pi8 : y[12]*D_Cos_1Pi4 : y[12]*-D_Cos_1Pi4
        addps    xmm5 , xmm6                    // xmm5 - Y[24] : Y[25] : Y[26] : Y[27]

        //* Y[20] = Y[24] + Y[26];
        //* Y[21] = Y[25] + Y[27];
        //* Y[22] = Y[27] - Y[25];
        //* Y[23] = Y[26] - Y[24];

        movaps    xmm6 , xmm5                    // xmm6 - Y[24] : Y[25] : Y[26] : Y[27]
        shufps    xmm6 , xmm6 , 0xB4            // xmm6 - Y[24] : Y[25] : Y[27] : Y[26] - 10 11 01 00
        shufps    xmm5 , xmm5 , 0x1E            // xmm5 - Y[26] : Y[27] : Y[25] : Y[24] - 00 01 11 10
        xorps    xmm5 , [edx]                // xmm5 - Y[26] : Y[27] :-Y[25] :-Y[24]
        addps    xmm6 , xmm5                    // xmm6 - Y[20] : Y[21] : Y[22] : Y[23]

        //* Y[ 8] = Y[16] + Y[20];
        //* Y[ 9] = Y[17] + Y[21];
        //* Y[10] = Y[18] + Y[22];
        //* Y[11] = Y[19] + Y[23];
        //* Y[12] = Y[23] - Y[19];
        //* Y[13] = Y[22] - Y[18];
        //* Y[14] = Y[21] - Y[17];
        //* Y[15] = Y[20] - Y[16];

        // xmm7 - Y[16] : Y[17] : Y[18] : Y[19]
        // xmm6 - Y[20] : Y[21] : Y[22] : Y[23]

        movaps    xmm4 , xmm6                    // xmm4 - Y[20] : Y[21] : Y[22] : Y[23]
        movaps    xmm5 , xmm7                    // xmm5 - Y[16] : Y[17] : Y[18] : Y[19]
        shufps    xmm4 , xmm4 , 0x1B            // xmm4 - Y[23] : Y[22] : Y[21] : Y[20] - 00 01 10 11
        shufps    xmm5 , xmm5 , 0x1B            // xmm5 - Y[19] : Y[18] : Y[17] : Y[16] - 00 01 10 11
        addps    xmm7 , xmm6                    // xmm7 - Y[ 8] : Y[ 9] : Y[10] : Y[11]
        subps    xmm4 , xmm5                 // xmm4 - Y[12] : Y[13] : Y[14] : Y[15]

        //* X[ 0] = D_Delta_0Pi16 * (Y[0] + Y[8]);
        //* X[ 1] = D_Delta_1Pi16 * (Y[1] + Y[9]);
        //* X[ 2] = D_Delta_2Pi16 * (Y[2] + Y[10]);
        //* X[ 3] = D_Delta_3Pi16 * (Y[3] + Y[11]);
        //* X[ 4] = D_Delta_4Pi16 * (Y[4] + Y[12]);
        //* X[ 5] = D_Delta_5Pi16 * (Y[5] + Y[13]);
        //* X[ 6] = D_Delta_6Pi16 * (Y[6] + Y[14]);
        //* X[ 7] = D_Delta_7Pi16 * (Y[7] + Y[15]);
        //* X[ 8] = D_Delta_8Pi16 * (Y[15] - Y[7]);
        //* X[ 9] = D_Delta_9Pi16 * (Y[14] - Y[6]);
        //* X[10] = D_Delta_10Pi16 * (Y[13] - Y[5]);
        //* X[11] = D_Delta_11Pi16 * (Y[12] - Y[4]);
        //* X[12] = D_Delta_12Pi16 * (Y[11] - Y[3]);
        //* X[13] = D_Delta_13Pi16 * (Y[10] - Y[2]);
        //* X[14] = D_Delta_14Pi16 * (Y[9] - Y[1]);
        //* X[15] = D_Delta_15Pi16 * (Y[8] - Y[0]);

        // xmm0 - Y[ 0] : Y[ 1] : Y[ 2] : Y[ 3]
        // xmm1 - Y[ 4] : Y[ 5] : Y[ 6] : Y[ 7]
        // xmm7 - Y[ 8] : Y[ 9] : Y[10] : Y[11]
        // xmm4 - Y[12] : Y[13] : Y[14] : Y[15]

        movaps    xmm2 , xmm4                    // xmm4 - Y[12] : Y[13] : Y[14] : Y[15]
        shufps    xmm2 , xmm2 , 0x1B            // xmm2 - Y[15] : Y[14] : Y[13] : Y[12] - 00 01 10 11
        movaps    xmm3 , xmm7                    // xmm3 - Y[ 8] : Y[ 9] : Y[12] : Y[11]
        shufps    xmm3 , xmm3 , 0x1B            // xmm3 - Y[11] : Y[10] : Y[ 9] : Y[ 8] - 00 01 10 11
        movaps    xmm5 , xmm1                    // xmm5 - Y[ 4] : Y[ 5] : Y[ 6] : Y[ 7]
        shufps    xmm5 , xmm5 , 0x1B            // xmm5 - Y[ 7] : Y[ 6] : Y[ 5] : Y[ 4] - 00 01 10 11
        movaps    xmm6 , xmm0                    // xmm6 - Y[ 0] : Y[ 1] : Y[ 2] : Y[ 3]
        shufps    xmm6 , xmm6 , 0x1B            // xmm6 - Y[ 3] : Y[ 2] : Y[ 1] : Y[ 0] - 00 01 10 11

        addps    xmm0 , xmm7                    // xmm0 - Y[ 0]+Y[ 8] : Y[ 1]+Y[ 9] : Y[ 2]+Y[10] : Y[ 3]+Y[11]
        addps    xmm1 , xmm4                    // xmm1 - Y[ 4]+Y[12] : Y[ 5]+Y[13] : Y[ 6]+Y[14] : Y[ 7]+Y[15]
        subps    xmm2 , xmm5                    // xmm2 - Y[15]-Y[ 7] : Y[14]-Y[ 6] : Y[13]-Y[ 5] : Y[12]-Y[ 4]
        subps    xmm3 , xmm6                    // xmm3 - Y[11]-Y[ 3] : Y[10]-Y[ 2] : Y[ 9]-Y[ 1] : Y[ 8]-Y[ 0]
        mulps    xmm0 , [ecx + 160]            // xmm0 - X[ 0] : X[ 1] : X[ 2] : X[ 3]
        mulps    xmm1 , [ecx + 176]            // xmm1 - X[ 4] : X[ 5] : X[ 6] : X[ 7]
        mulps    xmm2 , [ecx + 192]            // xmm2 - X[ 8] : X[ 9] : X[10] : X[11]
        mulps    xmm3 , [ecx + 208]            // xmm3 - X[12] : X[13] : X[14] : X[15]

        mov        eax , X
        movaps    [eax] , xmm0
        movaps    [eax+16] , xmm1
        movaps    [eax+32] , xmm2
        movaps    [eax+48] , xmm3

        emms
        popad
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

tfloat32 *OmegaDCT::input()
{
    return m_x;
}

//-------------------------------------------------------------------------------------------

const tfloat32 *OmegaDCT::input() const
{
    return m_x;
}

//-------------------------------------------------------------------------------------------

tfloat32 *OmegaDCT::output()
{
    return m_X;
}

//-------------------------------------------------------------------------------------------

const tfloat32 *OmegaDCT::output() const
{
    return m_X;
}

//-------------------------------------------------------------------------------------------

tfloat32 *OmegaDCT::TypeII(tfloat32 *x)
{
    if(x!=m_x)
    {
        ::memcpy(m_x,x,m_N * sizeof(tfloat32));
    }

#if defined(OMEGA_INTEL)
    if(!m_isSIMD)
    {
        Type2(m_x,m_X,m_N);
    }
    else
    {
        Type2_IntelSIMD(m_x,m_X,m_N);
    }
#else
    Type2(m_x,m_X,m_N);
#endif
    return m_X;
}

//-------------------------------------------------------------------------------------------

tfloat32 *OmegaDCT::TypeIII(tfloat32 *x)
{
    if(x!=m_x)
    {
        ::memcpy(m_x,x,m_N * sizeof(tfloat32));
    }

#if defined(OMEGA_INTEL)
    if(!m_isSIMD)
    {
        Type3(m_x,m_X,m_N);
    }
    else
    {
        Type3_IntelSIMD(m_x,m_X,m_N);
    }
#else
    Type3(m_x,m_X,m_N);
#endif
    return m_X;
}

//-------------------------------------------------------------------------------------------

tfloat32 *OmegaDCT::TypeIV(tfloat32 *x)
{
    if(x!=m_x)
    {
        ::memcpy(m_x,x,m_N * sizeof(tfloat32));
    }

#if defined(OMEGA_INTEL)
    if(!m_isSIMD)
    {
        Type4(m_x,m_X,m_N);
    }
    else
    {
        Type4_IntelSIMD(m_x,m_X,m_N);
    }
#else
    Type4(m_x,m_X,m_N);
#endif
    return m_X;
}

//-------------------------------------------------------------------------------------------

tfloat32 *OmegaDCT::MDCT(tfloat32 *x)
{
    tint i,idxA,idxB,Na,Nb,Nc;

    Na = static_cast<tint>(static_cast<tuint>(m_N) >> 2);
    Nb = static_cast<tint>(static_cast<tuint>(m_N) >> 1);
    Nc = 3 * Na;

    i = 0;
    idxA = Nc;
    idxB = Nc - 1;
    while(i < Na)
    {
        m_Y[i] = -x[idxA] - x[idxB];
        idxA++;
        idxB--;
        ++i;
    }
    idxA = 0;
    while(i < Nb)
    {
        m_Y[i] = x[idxA] - x[idxB];
        idxA++;
        idxB--;
        ++i;
    }

    m_offsetY = Nb;

#if defined(OMEGA_INTEL)
    if(!m_isSIMD)
    {
        if(Nb==16)
        {
            T4L16(m_Y,m_X);
        }
        else
        {
            Type4(m_Y,m_X,Nb);
        }
    }
    else
    {
        if(Nb==16)
        {
            T4L16_IntelSIMD(m_Y,m_X,&m_Y[m_offsetY]);
        }
        else
        {
            Type4_IntelSIMD(m_Y,m_X,Nb);
        }
    }
#else
    if(Nb==16)
    {
        T4L16(m_Y,m_X);
    }
    else
    {
        Type4(m_Y,m_X,Nb);
    }
#endif

    m_offsetY = 0;
    return m_X;
}

//-------------------------------------------------------------------------------------------

tfloat32 *OmegaDCT::InverseMDCT(tfloat32 *x)
{
    tint i,j,Na,Nb,Nc;

    Nb = static_cast<tint>(static_cast<tuint>(m_N) >> 1);

    if(x!=m_x)
    {
        ::memcpy(m_x,x,Nb * sizeof(tfloat32));
    }

    m_offsetY = m_N;
#if defined(OMEGA_INTEL)
    if(!m_isSIMD)
    {
        if(Nb==16)
        {
            T4L16(m_x,m_Y);
        }
        else
        {
            Type4(m_x,m_Y,Nb);
        }
    }
    else
    {
        if(Nb==16)
        {
            T4L16_IntelSIMD(m_x,m_Y,&m_Y[m_offsetY]);
        }
        else
        {
            Type4_IntelSIMD(m_x,m_Y,Nb);
        }
    }
#else
    if(Nb==16)
    {
        T4L16(m_x,m_Y);
    }
    else
    {
        Type4(m_x,m_Y,Nb);
    }
#endif
    m_offsetY = 0;

    for(i=0,j=m_N-1;i<Nb;++i,--j)
    {
        m_Y[j] = -m_Y[i];
    }

    Na = m_N / 4;
    Nb = (3 * m_N) / 4;
    Nc = (7 * m_N) / 4;

    for(i=0;i<Nb;++i)
    {
        m_X[i] = m_Y[i + Na];
    }
    for(i=Nb;i<m_N;++i)
    {
        m_X[i] = m_Y[Nc - 1 - i];
    }
    return m_X;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::WMDCT(tfloat32 *x,tfloat32 *X,tint offset)
{
    tint i,idxA,idxB,Na,Nb,Nc;

    X = &X[offset];

    Na = static_cast<tint>(static_cast<tuint>(m_N) >> 2);
    Nb = static_cast<tint>(static_cast<tuint>(m_N) >> 1);
    Nc = 3 * Na;

    i = 0;
    idxA = Nc;
    idxB = Nc - 1;
    while(i < Na)
    {
        m_Y[i] = -x[idxA] - x[idxB];
        idxA++;
        idxB--;
        ++i;
    }
    idxA = 0;
    while(i < Nb)
    {
        m_Y[i] = x[idxA] - x[idxB];
        idxA++;
        idxB--;
        ++i;
    }

    m_offsetY = Nb;

#if defined(OMEGA_INTEL)
    if(!m_isSIMD)
    {
        if(Nb==16)
        {
            T4L16(m_Y,m_X);
        }
        else
        {
            Type4(m_Y,m_X,Nb);
        }
    }
    else
    {
        if(Nb==16)
        {
            T4L16_IntelSIMD(m_Y,m_X,&m_Y[m_offsetY]);
        }
        else
        {
            Type4_IntelSIMD(m_Y,m_X,Nb);
        }
    }
#else
    if(Nb==16)
    {
        T4L16(m_Y,m_X);
    }
    else
    {
        Type4(m_Y,m_X,Nb);
    }
#endif

    m_offsetY = 0;

    for(i=0;i<Nb;++i)
    {
        X[i] = m_X[i] * 2.0f;
    }
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::WInverseMDCT(tfloat32 *x,tfloat32 *X,tint offset)
{
    tint i,j,Na,Nb,Nc;

    X = &X[offset];
    Nb = static_cast<tint>(static_cast<tuint>(m_N) >> 1);

    if(x!=m_x)
    {
        ::memcpy(m_x,x,Nb * sizeof(tfloat32));
    }

    m_offsetY = m_N;
#if defined(OMEGA_INTEL)
    if(!m_isSIMD)
    {
        if(Nb==16)
        {
            T4L16(m_x,m_Y);
        }
        else
        {
            Type4(m_x,m_Y,Nb);
        }
    }
    else
    {
        if(Nb==16)
        {
            T4L16_IntelSIMD(m_x,m_Y,&m_Y[m_offsetY]);
        }
        else
        {
            Type4_IntelSIMD(m_x,m_Y,Nb);
        }
    }
#else
    if(Nb==16)
    {
        T4L16(m_x,m_Y);
    }
    else
    {
        Type4(m_x,m_Y,Nb);
    }
#endif
    m_offsetY = 0;

    for(i=0,j=m_N-1;i<Nb;++i,--j)
    {
        m_Y[j] = -m_Y[i];
    }

    Na = m_N / 4;
    Nb = (3 * m_N) / 4;
    Nc = (7 * m_N) / 4;

    for(i=0;i<Nb;++i)
    {
        X[i] = m_Y[i + Na] * m_halfN;
    }
    for(i=Nb;i<m_N;++i)
    {
        X[i] = m_Y[Nc - 1 - i] * m_halfN;
    }
}

//-------------------------------------------------------------------------------------------

void OmegaDCT::VSInverseMDCT(tfloat32 *x,tfloat32 *X)
{
    tint i,j,Na,Nb,Nc;

    Nb = static_cast<tint>(static_cast<tuint>(m_N) >> 1);

    ::memcpy(m_x,x,Nb * sizeof(tfloat32));

    m_offsetY = m_N;
#if defined(OMEGA_INTEL)
    if(!m_isSIMD)
    {
        if(Nb==16)
        {
            T4L16(m_x,m_Y);
        }
        else
        {
            Type4(m_x,m_Y,Nb);
        }
    }
    else
    {
        if(Nb==16)
        {
            T4L16_IntelSIMD(m_x,m_Y,&m_Y[m_offsetY]);
        }
        else
        {
            Type4_IntelSIMD(m_x,m_Y,Nb);
        }
    }
#else
    if(Nb==16)
    {
        T4L16(m_x,m_Y);
    }
    else
    {
        Type4(m_x,m_Y,Nb);
    }
#endif
    m_offsetY = 0;

    for(i=0,j=m_N-1;i<Nb;++i,--j)
    {
        m_Y[j] = -m_Y[i];
    }

    Na = m_N / 4;
    Nb = (3 * m_N) / 4;
    Nc = (7 * m_N) / 4;

    for(i=0;i<Nb;++i)
    {
        X[i] = m_Y[i + Na];
    }
    for(i=Nb;i<m_N;++i)
    {
        X[i] = m_Y[Nc - 1 - i];
    }
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
