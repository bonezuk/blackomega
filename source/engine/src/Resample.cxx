#include "engine/inc/Resample.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

Resample::Resample() : m_imp(0),
    m_impD(0),
    m_lpScl(c_zeroSample),
    m_nmult(0),
    m_nwing(0),
    m_minFactor(0.0),
    m_maxFactor(0.0),
    m_xSize(0),
    m_x(0),
    m_xP(0),
    m_xRead(0),
    m_xOff(0),
    m_ySize(0),
    m_y(0),
    m_yP(0),
    m_time(0.0),
    m_interFlag(false)
{}

//-------------------------------------------------------------------------------------------

Resample::Resample(const Resample& rhs) : m_imp(0),
    m_impD(0),
    m_lpScl(c_zeroSample),
    m_nmult(0),
    m_nwing(0),
    m_minFactor(0.0),
    m_maxFactor(0.0),
    m_xSize(0),
    m_x(0),
    m_xP(0),
    m_xRead(0),
    m_xOff(0),
    m_ySize(0),
    m_y(0),
    m_yP(0),
    m_time(0.0),
    m_interFlag(false)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

Resample::~Resample()
{
    try
    {
        deallocate();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void Resample::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Resample::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

const Resample& Resample::operator = (const Resample& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void Resample::copy(const Resample& rhs)
{
    m_minFactor = rhs.m_minFactor;
    m_maxFactor = rhs.m_maxFactor;
    m_nmult = rhs.m_nmult;
    m_lpScl = rhs.m_lpScl;
    m_nwing = rhs.m_nwing;
    
    allocate();
    
    ::memcpy(m_imp,rhs.m_imp,m_nwing * sizeof(sample_t));
    ::memcpy(m_impD,rhs.m_impD,m_nwing * sizeof(sample_t));
    
    m_xOff = rhs.m_xOff;
    m_xSize = rhs.m_xSize;
    ::memcpy(m_x,rhs.m_x,(m_xSize + m_xOff) * sizeof(sample_t));
    m_xP = rhs.m_xP;
    m_xRead = rhs.m_xRead;
    
    m_ySize = rhs.m_ySize;
    ::memcpy(m_y,rhs.m_y,m_ySize * sizeof(sample_t));
    m_yP = rhs.m_yP;
    
    m_time = rhs.m_time;
}

//-------------------------------------------------------------------------------------------

void Resample::allocate()
{
    deallocate();
    
    m_imp = new sample_t [m_nwing];
    ::memset(m_imp,0,m_nwing * sizeof(sample_t));
    m_impD = new sample_t [m_nwing];
    ::memset(m_impD,0,m_nwing * sizeof(sample_t));
    m_x = new sample_t [m_xSize + m_xOff];
    ::memset(m_x,0,(m_xSize + m_xOff) * sizeof(sample_t));
    m_y = new sample_t [m_ySize];
    ::memset(m_y,0,m_ySize * sizeof(sample_t));
}

//-------------------------------------------------------------------------------------------

void Resample::deallocate()
{
    if(m_imp!=0)
    {
        delete [] m_imp;
        m_imp = 0;
    }
    if(m_impD!=0)
    {
        delete [] m_impD;
        m_impD = 0;
    }
    if(m_x!=0)
    {
        delete [] m_x;
        m_x = 0;
    }
    if(m_y!=0)
    {
        delete [] m_y;
        m_y = 0;
    }
}

//-------------------------------------------------------------------------------------------

tfloat64 Resample::iZero(tfloat64 x)
{
    const tfloat64 c_iZeroEpsilon = 1E-21;
    tfloat64 sum,u,halfx,temp;
    tint n = 1;

    sum = u = 1.0;
    halfx = x / 2.0;

    do
    {
        temp = halfx / static_cast<tfloat64>(n);
        n++;
        temp *= temp;
        u *= temp;
        sum += u;
    } while(u >= (c_iZeroEpsilon * sum));
    
    return sum;
}

//-------------------------------------------------------------------------------------------

void Resample::lrsLpFilter(sample_t *c,tint N,tfloat64 freq,tfloat64 beta,tint num)
{
    tint i;
    tfloat64 iBeta,tA,tB,inm;
    
    c[0] = static_cast<sample_t>(2.0 * freq);
    for(i=1;i<N;++i)
    {
        tA = c_PI_D * static_cast<tfloat64>(i) / static_cast<tfloat64>(num);
        c[i] = static_cast<sample_t>(sin(2.0 * tA * freq) / tA);
    }
    
    iBeta = c_plusOneSample / iZero(beta);
    inm = c_plusOneSample / static_cast<tfloat64>(N - 1);
    for(i=1;i<N;++i)
    {
        tA = static_cast<tfloat64>(i) * inm;
        tB = 1.0 - (tA * tA);
        tB = (tB<0.0) ? 0.0 : tB;
        c[i] *= static_cast<sample_t>(iZero(beta * sqrt(tB)) * iBeta);
    }
}

//-------------------------------------------------------------------------------------------

sample_t Resample::lrsFilterUp(sample_t *imp,sample_t *impD,tint nwing,bool interp,sample_t *xP,sample_t pH,tint inc)
{
    sample_t *hP,*hDP = 0,*end;
    sample_t a = c_zeroSample;
    sample_t v,t;
    
    pH *= dKaiser_Npc;
    
    v = 0.0f;
    hP = &imp[static_cast<tint>(pH)];
    end = &imp[nwing];
    if(interp)
    {
        hDP = &impD[static_cast<tint>(pH)];
        a = pH - floor(pH);
    }
    
    if(inc==1)
    {
        end--;
        if(pH==0.0)
        {
            hP += dKaiser_Npc;
            hDP += dKaiser_Npc;
        }
    }
    
    if(interp)
    {
        while(hP < end)
        {
            t = *hP;
            t += (*hDP) * a;
            hDP += dKaiser_Npc;
            t *= *xP;
            v += t;
            hP += dKaiser_Npc;
            xP += inc;
        }
    }
    else
    {
        while(hP < end)
        {
            t = *hP;
            t *= *xP;
            v += t;
            hP += dKaiser_Npc;
            xP += inc;
        }
    }
    return v;
}

//-------------------------------------------------------------------------------------------

sample_t Resample::lrsFilterUD(sample_t *imp,sample_t *impD,tint nwing,bool interp,sample_t *xP,sample_t pH,tint inc,sample_t dhb)
{
    sample_t a,v,t;
    sample_t *hP,*hDP,*end;
    sample_t hO;
    
    v = c_zeroSample;
    hO = pH * dhb;
    end = &imp[nwing];
    if(inc==1)
    {
        end--;
        if(pH==c_zeroSample)
        {
            hO += dhb;
        }
    }
    
    if(interp)
    {
        while(hP=&imp[static_cast<tint>(hO)],hP<end)
        {
            t = *hP;
            hDP = &impD[static_cast<tint>(hO)];
            a =  hO - floor(hO);
            t += (*hDP) * a;
            t *= *xP;
            v += t;
            hO += dhb;
            xP += inc;
        }
    }
    else
    {
        while(hP=&imp[static_cast<tint>(hO)],hP<end)
        {
            t = *hP;
            t *= *xP;
            v += t;
            hO += dhb;
            xP += inc;
        }
    }
    return v;
}

//-------------------------------------------------------------------------------------------

tint Resample::lrsSrcUp(sample_t *X,sample_t *Y,tfloat64 factor,tfloat64& currentTime,tint nx,tint nwing,sample_t lpScl,sample_t *imp,sample_t *impD,bool interp)
{
    sample_t v,*xP,*yStart;
    tfloat64 dt,endTime;
    
    dt = 1.0 / factor;
    
    yStart = Y;
    endTime = currentTime + static_cast<tint>(nx);
    while(currentTime < endTime)
    {
        tfloat64 leftPhase  = currentTime - floor(currentTime);
        tfloat64 rightPhase = 1.0 - leftPhase;
        
        xP = &X[static_cast<tint>(currentTime)];
        // Perform left-wing inner product
        v  = lrsFilterUp(imp,impD,nwing,interp,xP,static_cast<sample_t>(leftPhase),-1);
        // Perform right-wing inner product
        v += lrsFilterUp(imp,impD,nwing,interp,xP+1,static_cast<sample_t>(rightPhase),1);
        v *= lpScl;
        *Y++ = v;
        currentTime += dt;
    }
    return static_cast<tint>(Y - yStart);
}

//-------------------------------------------------------------------------------------------

tint Resample::lrsSrcUD(sample_t *X,sample_t *Y,tfloat64 factor,tfloat64& currentTime,tint nx,tint nwing,sample_t lpScl,sample_t *imp,sample_t *impD,bool interp)
{
    sample_t v,*xP,*yStart,dh;
    tfloat64 dt,endTime;
    
    dt = 1.0 / factor;
    dh = KAISER_MIN(dKaiser_Npc,factor * dKaiser_Npc);
    
    yStart = Y;
    endTime = currentTime + static_cast<tint>(nx);
    while(currentTime < endTime)
    {
        tfloat64 leftPhase  = currentTime - floor(currentTime);
        tfloat64 rightPhase = 1.0 - leftPhase;
        
        xP = &X[static_cast<tint>(currentTime)];
        // Perform left-wing inner product
        v  = lrsFilterUD(imp,impD,nwing,interp,xP,static_cast<sample_t>(leftPhase),-1,dh);
        // Perform right-wing inner product
        v += lrsFilterUD(imp,impD,nwing,interp,xP+1,static_cast<sample_t>(rightPhase),1,dh);
        v *= lpScl;
        *Y++ = v;
        currentTime += dt;
    }
    return static_cast<tint>(Y - yStart);
}

//-------------------------------------------------------------------------------------------

bool Resample::open(tint highQuality,tfloat64 minFactor,tfloat64 maxFactor)
{
    tfloat64 rollOff,beta;
    sample_t *imp64;
    tint i,xOffMin,xOffMax;
    
    deallocate();
    
    if(minFactor<=0.0 || maxFactor<=0.0 || maxFactor<minFactor)
    {
        printError("open","minFactor and maxFactor must be positive real numbers, and maxFactor should larger than minFactor");
        return false;
    }
    
    m_minFactor = minFactor;
    m_maxFactor = maxFactor;
    m_nmult = (highQuality) ? 35 : 11;
    m_lpScl = c_plusOneSample;
    m_nwing = dKaiser_Npc * (m_nmult - 1) / 2;
    
    m_imp = new sample_t [m_nwing];
    m_impD = new sample_t [m_nwing];
    rollOff = 0.90;
    beta = 6.0;
    imp64 = new sample_t [m_nwing];
    lrsLpFilter(imp64,m_nwing,0.5 * rollOff,beta,dKaiser_Npc);
    for(i=0;i<m_nwing;++i)
    {
        m_imp[i] = static_cast<sample_t>(imp64[i]);
    }
    for(i=0;i<m_nwing-1;++i)
    {
        m_impD[i] = m_imp[i+1] - m_imp[i];
    }
    m_impD[m_nwing-1] = c_zeroSample - m_imp[m_nwing-1];
    delete [] imp64;
    
    xOffMin = static_cast<tint>((static_cast<tfloat64>(m_nmult + 1) / 2.0) * KAISER_MAX(1.0,1.0/minFactor)) + 10;
    xOffMax = static_cast<tint>((static_cast<tfloat64>(m_nmult + 1) / 2.0) * KAISER_MAX(1.0,1.0/maxFactor)) + 10;
    m_xOff  = KAISER_MAX(xOffMin,xOffMax);
    m_xSize = KAISER_MAX(2 * m_xOff + 10,4096);
    m_x = new sample_t [m_xSize + m_xOff];
    m_xP = m_xOff;
    m_xRead = m_xOff;
    ::memset(m_x,0,(m_xSize + m_xOff) * sizeof(sample_t));
    
    m_ySize = static_cast<tint>(static_cast<tfloat64>(m_xSize) * maxFactor + 2.0);
    m_y = new sample_t [m_ySize];
    m_yP = 0;
    
    m_time = m_xOff;
    
    return true;
}

//-------------------------------------------------------------------------------------------

void Resample::close()
{
    deallocate();
}

//-------------------------------------------------------------------------------------------

bool Resample::interpolate() const
{
    return m_interFlag;
}

//-------------------------------------------------------------------------------------------

void Resample::setInterpolate(bool v)
{
    m_interFlag = v;
}

//-------------------------------------------------------------------------------------------

tint Resample::filterWidth() const
{
    return m_xOff;
}

//-------------------------------------------------------------------------------------------

tint Resample::process(tfloat64 factor,sample_t *inBuffer,tint inBufferLen,tint lastFlag,tint& inBufferUsed,sample_t *outBuffer,tint outBufferLen)
{
    tint i,len,nx;
    tint nout,ncreep,nreuse,outSampleCount;
    sample_t lpScl;
    
    inBufferUsed = 0;
    outSampleCount = 0;
    
    if(factor<m_minFactor || factor>m_maxFactor)
    {
        printError("process","factor must be between minimum and maximum factor");
        return false;
    }
    
    if(m_yP && (outBufferLen-outSampleCount)>0)
    {
        len = KAISER_MIN(outBufferLen-outSampleCount,m_yP);
        for(i=0;i<len;++i)
        {
            outBuffer[outSampleCount + i] = m_y[i];
        }
        outSampleCount += len;
        for(i=0;i<(m_yP-len);++i)
        {
            m_y[i] = m_y[i+len];
        }
        m_yP -= len;
    }
    
    if(m_yP)
    {
        return outSampleCount;
    }
    
    if(factor < 1.0)
    {
        lpScl = m_lpScl * static_cast<sample_t>(factor);
    }
    else
    {
        lpScl = m_lpScl;
    }
    
    while(true)
    {
        len = m_xSize - m_xRead;
        if(len >= (inBufferLen - inBufferUsed))
        {
            len = inBufferLen - inBufferUsed;
        }
        for(i=0;i<len;++i)
        {
            m_x[m_xRead + i] = inBuffer[inBufferUsed + i];
        }
        inBufferUsed += len;
        m_xRead += len;
        
        if(lastFlag && inBufferUsed==inBufferLen)
        {
            nx = m_xRead - m_xOff;
            for(i=0;i<m_xOff;++i)
            {
                m_x[m_xRead + i] = c_zeroSample;
            }
        }
        else
        {
            nx = m_xRead - 2 * m_xOff;
        }
        
        if(nx<=0)
        {
            break;
        }
        
        if(factor>=1.0)
        {
            nout = lrsSrcUp(m_x,m_y,factor,m_time,nx,m_nwing,lpScl,m_imp,m_impD,m_interFlag);
        }
        else
        {
            nout = lrsSrcUD(m_x,m_y,factor,m_time,nx,m_nwing,lpScl,m_imp,m_impD,m_interFlag);
        }
        
        m_time -= static_cast<tfloat64>(nx);
        m_xP += nx;
        
        ncreep = static_cast<tint>(m_time) - m_xOff;
        if(ncreep)
        {
            m_time -= static_cast<tfloat64>(ncreep);
            m_xP += ncreep;
        }
        
        nreuse = m_xRead - (m_xP - m_xOff);
        for(i=0;i<nreuse;++i)
        {
            m_x[i] = m_x[i + (m_xP - m_xOff)];
        }
        m_xRead = nreuse;
        m_xP = m_xOff;
        
        if(nout > m_ySize)
        {
            printError("process","Output array overflow");
            return -1;
        }
        
        m_yP = nout;
        if(m_yP && (outBufferLen-outSampleCount)>0)
        {
            len = KAISER_MIN(outBufferLen-outSampleCount,m_yP);
            for(i=0;i<len;++i)
            {
                outBuffer[outSampleCount + i] = m_y[i];
            }
            outSampleCount += len;
            for(i=0;i<(m_yP-len);++i)
            {
                m_y[i] = m_y[i+len];
            }
            m_yP -= len;
        }
        
        if(m_yP)
        {
            break;
        }
    }
    
    return outSampleCount;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
