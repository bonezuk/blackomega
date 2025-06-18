#include "engine/blackomega/inc/Reorder.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

Reorder::Reorder() : m_alloc(),
    m_hdr(0),
    m_gr(0),
    m_xr(0)
{
    init();
}

//-------------------------------------------------------------------------------------------

Reorder::~Reorder()
{
    try
    {
        m_alloc.FreeAll();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void Reorder::init()
{
    tint i,j,k,l,max,*s;
    tint q[2048];
    
    for(i=1;i<101;++i)
    {
        m_seq[i - 1] = s = reinterpret_cast<tint *>(m_alloc.MemAlloc(i*6,sizeof(tint)));
        
        max = i * 3;
        for(j=0;j<max;++j)
        {
            k = j % 3;
            l = j / 3;
            q[j] = (k * i) + l;
        }
        
        for(j=0;j<max;++j)
        {
            k = j;
            if(q[k]!=-1 && k!=q[k])
            {
                *s++ = k;
                do
                {
                    *s++ = l = q[k];
                    q[k] = -1;
                    k = l;
                } while(k!=j && k!=-1);
            }
        }
        *s = -1;
    }
}

//-------------------------------------------------------------------------------------------

void Reorder::sortBlock(tint block)
{
    tint i,j,start,width,*x;
    sample_t l,*o;
    Band *band = Band::instance();
    
    start = band->m_sfBandIndex[m_hdr->sfreq].s[block * 3];
    width = band->m_sfBandIndex[m_hdr->sfreq].s[(block * 3) + 1] - start;
    o = &m_xr[start];
    x = m_seq[width - 1];
    
    while(*x != -1)
    {
        l = o[*x];
        i = j = *x++;
        do
        {
            o[i] = o[*x];
            i = *x++;
        } while(i != j);
        o[x[-2]] = l;
    }
}

//-------------------------------------------------------------------------------------------

void Reorder::process()
{
    tint i;
    
    if(m_gr->block_type==2)
    {
        if(m_gr->mixed_block_flag)
        {
            for(i=3;i<13;++i)
            {
                sortBlock(i);
            }
        }
        else
        {
            for(i=0;i<13;++i)
            {
                sortBlock(i);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
