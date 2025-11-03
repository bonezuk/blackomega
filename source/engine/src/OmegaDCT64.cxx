#include "engine/inc/OmegaDCT64.h"

//-------------------------------------------------------------------------------------------
// Cosine definitions
//-------------------------------------------------------------------------------------------

#define D_Cos_1Pi4 (0.70710678118654752440084436210485)
#define D_Cos_3Pi4 (-0.70710678118654752440084436210485)

#define D_Cos_1Pi8 (0.92387953251128675612818318939679)
#define D_Cos_3Pi8 (0.3826834323650897717284599840304)

#define D_Delta_0Pi4 (0.50979557910415916894193980398784)
#define D_Delta_1Pi4 (0.60134488693504528054372182390922)
#define D_Delta_2Pi4 (0.89997622313641570463850954094189)
#define D_Delta_3Pi4 (2.5629154477415061787960862961777)

#define D_Delta_0Pi8 (0.50241928618815570551167011928012)
#define D_Delta_1Pi8 (0.52249861493968888062857531905669)
#define D_Delta_2Pi8 (0.56694403481635770368053791515488)
#define D_Delta_3Pi8 (0.646821783359990129548360111652)
#define D_Delta_4Pi8 (0.78815462345125022473398248719742)
#define D_Delta_5Pi8 (1.0606776859903474713404517472331)
#define D_Delta_6Pi8 (1.7224470982383339278159153641566)
#define D_Delta_7Pi8 (5.1011486186891638581062454923454)

#define D_Delta_0Pi16 (0.50060299823519630134550410676638)
#define D_Delta_1Pi16 (0.50547095989754365998444458560696)
#define D_Delta_2Pi16 (0.51544730992262454697495130564925)
#define D_Delta_3Pi16 (0.53104259108978417447573393235719)
#define D_Delta_4Pi16 (0.55310389603444452782938083813705)
#define D_Delta_5Pi16 (0.58293496820613387367383070125262)
#define D_Delta_6Pi16 (0.62250412303566481615725615676281)
#define D_Delta_7Pi16 (0.67480834145500574602596871104104)
#define D_Delta_8Pi16 (0.74453627100229844977698119197295)
#define D_Delta_9Pi16 (0.83934964541552703873926374662543)
#define D_Delta_10Pi16 (0.97256823786196069369768941405256)
#define D_Delta_11Pi16 (1.1694399334328849551557702840422)
#define D_Delta_12Pi16 (1.4841646163141662772433269374281)
#define D_Delta_13Pi16 (2.0577810099534115508565544797104)
#define D_Delta_14Pi16 (3.4076084184687187857011913334591)
#define D_Delta_15Pi16 (10.190008123548056811212109201036)

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

tint OmegaDCT64::m_DCTCounter = 0;
common::Allocation OmegaDCT64::m_DCTAllocation;

//-------------------------------------------------------------------------------------------

QMap<tint,OmegaDCT64 *> OmegaDCT64::m_DCTCollection;

//-------------------------------------------------------------------------------------------

OmegaDCT64 *OmegaDCT64::get(tint N)
{
    OmegaDCT64 *DCT;
    QMap<tint,OmegaDCT64 *>::iterator ppI;
    
    ppI = m_DCTCollection.find(N);
    if(ppI!=m_DCTCollection.end())
    {
        DCT = ppI.value();
    }
    else
    {
        DCT = new OmegaDCT64(N);
        m_DCTCollection.insert(N,DCT);
    }
    return DCT;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT64::stop()
{
    OmegaDCT64 *DCT;
    QMap<tint,OmegaDCT64 *>::iterator ppI;
    
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

OmegaDCT64::OmegaDCT64(int N) : m_alloc(),
    m_N(N),
    m_x(0),
    m_X(0),
    m_Y(0),
    m_offsetY(0),
    m_D4FactorArray(0)
{
    init();

    m_halfN = 2.0 / static_cast<tfloat64>(m_N);
}

//-------------------------------------------------------------------------------------------

OmegaDCT64::~OmegaDCT64()
{
    try
    {
        free();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

tfloat64 OmegaDCT64::dctD4Factor(int k,int N) const
{
    tfloat64 v,t,tA,tB;
    const tfloat64 c_PI64 = 3.141592653589793238464264338832795;

    tA = c_PI64 / static_cast<tfloat64>(4 * N);
    tB = static_cast<tfloat64>((2 * k) + 1);
    t = tA * tB;
    v = 1.0 / (2.0 * cos(t));
    return v;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT64::init()
{
    tint i,k,N = 32,lN = mod2(),s = 0;

    m_D4FactorArray = reinterpret_cast<tfloat64 **>(m_alloc.MemAlloc(static_cast<unsigned int>(lN - 4),sizeof(tfloat64 *)));
    for(i=5;i<=lN;++i)
    {
        tfloat64 *f;
        
        N = static_cast<tint>(pow(2.0,i));
        f = reinterpret_cast<tfloat64 *>(m_alloc.MemAllocAlign(static_cast<unsigned int>(N),sizeof(tfloat64),16));
        for(k=0;k<N;++k)
        {
            f[k] = dctD4Factor(k,N);
        }
        m_D4FactorArray[i - 5] = f;
        
        s += N << 1;
    }
    
    m_Y = reinterpret_cast<tfloat64 *>(m_alloc.MemAllocAlign(static_cast<unsigned int>(s + 128),sizeof(tfloat64),16));
    m_offsetY = 0;
    
    m_x = reinterpret_cast<tfloat64 *>(m_alloc.MemAllocAlign(static_cast<unsigned int>(N),sizeof(tfloat64),16));
    m_X = reinterpret_cast<tfloat64 *>(m_alloc.MemAllocAlign(static_cast<unsigned int>(N),sizeof(tfloat64),16));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT64::free()
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

bool OmegaDCT64::isMod2() const
{
    return isMod2(m_N);
}

//-------------------------------------------------------------------------------------------

bool OmegaDCT64::isMod2(int N) const
{
    return (mod2(N)>=0);
}

//-------------------------------------------------------------------------------------------

int OmegaDCT64::mod2() const
{
    return mod2(m_N);
}

//-------------------------------------------------------------------------------------------

int OmegaDCT64::mod2(int N) const
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

void OmegaDCT64::Type2(tfloat64 *x,tfloat64 *X,int N)
{
    Type2(x,X,N,mod2(N));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT64::Type2(tfloat64 *x,tfloat64 *X,int N,int lN)
{
    tint i,j,halfN = N >> 1;
    tfloat64 *xA,*xB,*Xa,*Xb;
    
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

void OmegaDCT64::Type3(tfloat64 *x,tfloat64 *X,int N)
{
    Type3(x,X,N,mod2(N));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT64::Type3(tfloat64 *x,tfloat64 *X,int N,int lN)
{
    tint i,j,halfN = N >> 1;
    tfloat64 *xA,*xB,*Xa,*Xb;
    
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

void OmegaDCT64::Type4(tfloat64 *x,tfloat64 *X,int N)
{
    Type4(x,X,N,mod2(N));
}

//-------------------------------------------------------------------------------------------

void OmegaDCT64::Type4(tfloat64 *x,tfloat64 *X,int N,int lN)
{
    tint i,j,halfN = N >> 1;
    tfloat64 *xA,*xB,*Xa,*Xb;
    
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
    
    tfloat64 *f = m_D4FactorArray[lN - 5];
        
    for(i=0,j=N-1;i<halfN;i++,j--)
    {
        X[i] = f[i] * (Xa[i] + Xb[i]);
        X[j] = f[j] * (Xb[i] - Xa[i]);
    }
    
    m_offsetY -= N << 1;
}

//-------------------------------------------------------------------------------------------

void OmegaDCT64::T2L16(tfloat64 *x,tfloat64 *X)
{
    tfloat64 y[32],Y[12];
    
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

void OmegaDCT64::T3L16(tfloat64 *x,tfloat64 *X)
{
    tfloat64 y[13],Y[28];

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

void OmegaDCT64::T4L16(tfloat64 *x,tfloat64 *X)
{
    tfloat64 y[29],Y[28];
    
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

tfloat64 *OmegaDCT64::input()
{
    return m_x;
}

//-------------------------------------------------------------------------------------------

const tfloat64 *OmegaDCT64::input() const
{
    return m_x;
}

//-------------------------------------------------------------------------------------------

tfloat64 *OmegaDCT64::output()
{
    return m_X;
}

//-------------------------------------------------------------------------------------------

const tfloat64 *OmegaDCT64::output() const
{
    return m_X;
}

//-------------------------------------------------------------------------------------------

tfloat64 *OmegaDCT64::TypeII(tfloat64 *x)
{
    if(x!=m_x)
    {
        ::memcpy(m_x,x,m_N * sizeof(tfloat64));
    }

    Type2(m_x,m_X,m_N);

    return m_X;
}

//-------------------------------------------------------------------------------------------

tfloat64 *OmegaDCT64::TypeIII(tfloat64 *x)
{
    if(x!=m_x)
    {
        ::memcpy(m_x,x,m_N * sizeof(tfloat64));
    }
    
    Type3(m_x,m_X,m_N);

    return m_X;
}

//-------------------------------------------------------------------------------------------

tfloat64 *OmegaDCT64::TypeIV(tfloat64 *x)
{
    if(x!=m_x)
    {
        ::memcpy(m_x,x,m_N * sizeof(tfloat64));
    }

    Type4(m_x,m_X,m_N);

    return m_X;
}

//-------------------------------------------------------------------------------------------

tfloat64 *OmegaDCT64::MDCT(tfloat64 *x)
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
    
    if(Nb==16)
    {
        T4L16(m_Y,m_X);
    }
    else
    {
        Type4(m_Y,m_X,Nb);
    }
        
    m_offsetY = 0;
    return m_X;    
}

//-------------------------------------------------------------------------------------------

tfloat64 *OmegaDCT64::InverseMDCT(tfloat64 *x)
{
    tint i,j,Na,Nb,Nc;
    
    Nb = static_cast<tint>(static_cast<tuint>(m_N) >> 1);
    
    if(x!=m_x)
    {
        ::memcpy(m_x,x,Nb * sizeof(tfloat64));
    }
    
    m_offsetY = m_N;

    if(Nb==16)
    {
        T4L16(m_x,m_Y);
    }
    else
    {
        Type4(m_x,m_Y,Nb);
    }

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

void OmegaDCT64::WMDCT(tfloat64 *x,tfloat64 *X,tint offset)
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
    
    if(Nb==16)
    {
        T4L16(m_Y,m_X);
    }
    else
    {
        Type4(m_Y,m_X,Nb);
    }
    
    m_offsetY = 0;

    for(i=0;i<Nb;++i)
    {
        X[i] = m_X[i] * 2.0f;
    }
}

//-------------------------------------------------------------------------------------------

void OmegaDCT64::WInverseMDCT(tfloat64 *x,tfloat64 *X,tint offset)
{
    tint i,j,Na,Nb,Nc;
    
    X = &X[offset];
    Nb = static_cast<tint>(static_cast<tuint>(m_N) >> 1);
    
    if(x!=m_x)
    {
        ::memcpy(m_x,x,Nb * sizeof(tfloat64));
    }
    
    m_offsetY = m_N;

    if(Nb==16)
    {
        T4L16(m_x,m_Y);
    }
    else
    {
        Type4(m_x,m_Y,Nb);
    }

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

void OmegaDCT64::VSInverseMDCT(tfloat64 *x,tfloat64 *X)
{
    tint i,j,Na,Nb,Nc;
    
    Nb = static_cast<tint>(static_cast<tuint>(m_N) >> 1);
    
    ::memcpy(m_x,x,Nb * sizeof(tfloat64));
    
    m_offsetY = m_N;

    if(Nb==16)
    {
        T4L16(m_x,m_Y);
    }
    else
    {
        Type4(m_x,m_Y,Nb);
    }

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
