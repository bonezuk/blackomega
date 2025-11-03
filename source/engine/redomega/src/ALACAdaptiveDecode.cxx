#include "engine/redomega/inc/ALACAdaptiveDecode.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

ALACAdaptiveDecode::ALACAdaptiveDecode() : m_mb(0),
    m_mb0(0),
    m_pb(0),
    m_kb(0),
    m_wb(0),
    m_qb(0),
    m_fw(0),
    m_sw(0),
    m_maxrun(0)
{}

//-------------------------------------------------------------------------------------------

ALACAdaptiveDecode::~ALACAdaptiveDecode()
{}

//-------------------------------------------------------------------------------------------

void ALACAdaptiveDecode::set(const ALACContainer& container,tint p,tint numSamples)
{
    const ALACSpecificConfig& config = container.config();

    m_mb = config.mb();
    m_mb0 = config.mb();
    m_pb = p;
    m_kb = config.kb();
    m_wb = (1 << m_kb) - 1;
    m_qb = ALAC_QB - m_pb;
    m_fw = numSamples;
    m_sw = numSamples;
    m_maxrun = config.maxRun();
}

//-------------------------------------------------------------------------------------------

tuint ALACAdaptiveDecode::readDyn32Bits(ALACSequence *seq,tuint m,tuint k,tint maxBits)
{
    tuint v = 0;

    while(v<9 && seq->readBitI())
    {
        v++;
    }
    if(v==9)
    {
        v = static_cast<tuint>(seq->readBitsI(maxBits));
    }
    else
    {
        if(k!=1)
        {
            tint x = seq->readBitsI(k);
            v *= static_cast<tuint>(m);
            if(x>=2)
            {
                v += x-1;
            }
            else
            {
                seq->seek(-1);
            }
        }
    }
    return v;
}

//-------------------------------------------------------------------------------------------

tuint ALACAdaptiveDecode::readDyn(ALACSequence *seq,tuint m,tuint k)
{
    tuint v = 0;

    while(v<9 && seq->readBitI())
    {
        v++;
    }
    if(v==9)
    {
        v = static_cast<tuint>(seq->readBitsI(16));
    }
    else
    {
        tint x = seq->readBitsI(k);
        v = (v * m) + (x - 1);
        if(x<2)
        {
            v -= (x - 1);
            seq->seek(-1);
        }
    }
    return v;
}

//-------------------------------------------------------------------------------------------

void ALACAdaptiveDecode::decode(ALACSequence *seq,tint maxSize,tint *out,tint len)
{
    tint i;
    tuint mb = static_cast<tuint>(m_mb0);
    tuint pb = static_cast<tuint>(m_pb);
    tuint kb = static_cast<tuint>(m_kb);
    tuint wb = m_wb;
    tint del,zmode;

    zmode = 0;

    i = 0;
    while(i<len && !seq->isEnd())
    {
        tuint m,k,n,nA,mz,j;

        m = mb >> ALAC_QBSHIFT;
        k = lg3a(m);
        k = alacMin(k,kb);
        m = (1 << k) - 1;

        n = readDyn32Bits(seq,m,k,maxSize);
        nA = n + zmode;
        del = static_cast<tint>((nA+1) >> 1);
        if(nA & 1)
        {
            del = -del;
        }
        out[i++] = del;

        mb = pb * (n+zmode) + mb - ((pb * mb) >> ALAC_QBSHIFT);
        if(n>0x0000ffff)
        {
            mb = 0x0000ffff;
        }

        zmode = 0;

        if((static_cast<tuint>(mb<<2) < ALAC_QB) && i<len)
        {
            zmode = 1;
            k = lead(mb) - ALAC_BITOFF + ((mb + ALAC_MOFF) >> ALAC_MDENSHIFT);
            mz = ((1<<k)-1) & wb;

            n = readDyn(seq,mz,k);
            if((i+n)>static_cast<tuint>(len))
            {
                n = len - i;
            }
            for(j=0;j<n;j++)
            {
                out[i++] = 0;
            }
            if(n>=65535)
            {
                zmode = 0;
            }
            mb = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
