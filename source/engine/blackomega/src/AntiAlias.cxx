#include "engine/blackomega/inc/AntiAlias.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

static const tfloat64 g_Ci[8]={-0.6,-0.535,-0.33,-0.185,-0.095,-0.041,-0.0142,-0.0037};

//-------------------------------------------------------------------------------------------

AntiAlias::AntiAlias() : m_gr(0),
    m_xr(0)
{
    init();
}

//-------------------------------------------------------------------------------------------

AntiAlias::~AntiAlias()
{}

//-------------------------------------------------------------------------------------------

void AntiAlias::init()
{
    tint i;
    tfloat64 sq;
    
    for(i=0;i<8;++i)
    {
        sq = ::sqrt(1.0 + g_Ci[i] * g_Ci[i]);
        m_cs[i] = static_cast<sample_t>(1.0 / sq);
        m_ca[i] = static_cast<sample_t>(g_Ci[i] / sq);
    }
}

//-------------------------------------------------------------------------------------------

void AntiAlias::mixSection(sample_t *pt,tint sb)
{
    tint i;
    sample_t *a = pt, *b = pt;
    sample_t *c = m_cs, *d = m_ca;
    sample_t *hA, *hB;
    
    hA = hB = &m_hybrid[sb+1][0];
    for(i=0;i<8;++i)
    {
        *--hA = ((*--a) * (*c)) - ((*b) * (*d));
        *hB++ = ((*b++) * (*c++)) + ((*a) * (*d++));
    }
}

//-------------------------------------------------------------------------------------------

sample_t *AntiAlias::process()
{
    tint i;
    
    if(m_gr->block_type==2)
    {
        if(m_gr->mixed_block_flag)
        {
            mixSection(&m_xr[SSLIMIT],0);
            for(i=0;i<8;++i)
            {
                m_xr[SSLIMIT + i] = m_hybrid[1][i];
                m_xr[(SSLIMIT-1)-i] = m_hybrid[0][(SSLIMIT - 1)-i];
            }
        }
        return m_xr;
    }
    else
    {
        for(i=0;i<31;++i)
        {
            mixSection(&m_xr[SSLIMIT*(i+1)],i);
            m_hybrid[i][8] = m_xr[(SSLIMIT * i) + 8];
            m_hybrid[i][9] = m_xr[(SSLIMIT * i) + 9];
        }
        
        ::memcpy(&m_hybrid[31][8],&m_xr[(SSLIMIT * 31) + 8],(SSLIMIT - 8) * sizeof(sample_t));
        ::memcpy(&m_hybrid[0][0],&m_xr[0],8 * sizeof(sample_t));
        return reinterpret_cast<sample_t *>(&m_hybrid[0][0]);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
