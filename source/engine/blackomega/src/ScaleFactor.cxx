#include "engine/blackomega/inc/ScaleFactor.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

ScaleFactor::ScaleFactor() : m_hdr(0),
    m_grN(0),
    m_chN(0),
    m_prevScale(0),
    m_gr(0),
    m_ch(0)
{
    m_l = &m_scale.l[0];
    m_s0 = &m_scale.s[0][0];
    m_s1 = &m_scale.s[1][0];
    m_s2 = &m_scale.s[2][0];
    
    init();
}

//-------------------------------------------------------------------------------------------

void ScaleFactor::init()
{
    tuint i,j,k,l;
    
    for(i=0;i<23;++i)
    {
        m_scale.l[i] = 0;
    }
    for(j=0;j<3;++j)
    {
        for(i=0;i<13;++i)
        {
            m_scale.s[j][i] = 0;
        }
    }
    
    for(i=0;i<5;++i)
    {
        for(j=0;j<6;++j)
        {
            for(k=0;k<6;++k)
            {
                tuint n = k + (j * 6) + (i * 36);
                m_i_slen2[n] = i | (j << 3) | (k << 6) | (3 << 12);
            }
        }
    }
    
    for(i=0;i<4;++i)
    {
        for(j=0;j<4;++j)
        {
            for(k=0;k<4;++k)
            {
                tuint n = k + (j * 4) + (i * 16);
                m_i_slen2[n + 180] = i | (j << 3) | (k << 6) | (4 << 12);
            }
        }
    }
    
    for(i=0;i<4;++i)
    {
        for(j=0;j<3;++j)
        {
            tuint n = j + (i * 3);
            m_i_slen2[n + 244] = i | (j << 3) | (5 << 12);
            m_n_slen2[n + 500] = i | (j << 3) | (2 << 12) | (1 << 15);
        }
    }
    
    for(i=0;i<5;++i)
    {
        for(j=0;j<5;++j)
        {
            for(k=0;k<4;++k)
            {
                for(l=0;l<4;++l)
                {
                    tuint n = l + (k * 4) + (j * 16) + (i * 80);
                    m_n_slen2[n] = i | (j << 3) | (k << 6) | (l << 9) | (0 << 12);
                }
            }
        }
    }
    
    for(i=0;i<5;++i)
    {
        for(j=0;j<5;++j)
        {
            for(k=0;k<4;++k)
            {
                tuint n = k + (j * 4) + (i * 20);
                m_n_slen2[n + 400] = i | (j << 3) | (k << 6) | (1 << 12);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void ScaleFactor::read(engine::Sequence *seq)
{
    if(!m_hdr->lsf)
    {
        readScaleFactorMPEG1(seq);
    }
    else
    {
        readScaleFactorMPEG2(seq);
    }
}

//-------------------------------------------------------------------------------------------

void ScaleFactor::readScaleFactorMPEG1(engine::Sequence *seq)
{
    static tint slen[2][16] = {
        {0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4},
        {0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3}
    };
    
    tint i,s;
    tint sf = m_gr->scalefac_compress;
    
    if(m_gr->block_type==2)
    {
        if(m_gr->mixed_block_flag)
        {
            s = slen[0][sf];
            i = 0;
            while(i<8)
            {
                m_l[i] = seq->readBitsI(s);
                i++;
            }
            i = 3;
            while(i<6)
            {
                m_s0[i] = seq->readBitsI(s);
                m_s1[i] = seq->readBitsI(s);
                m_s2[i] = seq->readBitsI(s);
                i++;
            }
            s = slen[1][sf];
            while(i<12)
            {
                m_s0[i] = seq->readBitsI(s);
                m_s1[i] = seq->readBitsI(s);
                m_s2[i] = seq->readBitsI(s);
                i++;
            }
        }
        else
        {
            s = slen[0][sf];
            i=0;
            while(i<6)
            {
                m_s0[i] = seq->readBitsI(s);
                m_s1[i] = seq->readBitsI(s);
                m_s2[i] = seq->readBitsI(s);
                i++;
            }
            s = slen[1][sf];
            while(i<12)
            {
                m_s0[i] = seq->readBitsI(s);
                m_s1[i] = seq->readBitsI(s);
                m_s2[i] = seq->readBitsI(s);
                i++;
            }
        }
        m_s0[i] = 0;
        m_s1[i] = 0;
        m_s2[i] = 0;
    }
    else
    {
        tint *pL = &(m_prevScale->l[0]);
        
        s = slen[0][sf];
        i = 0;
        if(!m_grN || !m_ch->scfsi[0])
        {
            while(i<6)
            {
                m_l[i] = seq->readBitsI(s);
                i++;
            }
        }
        else
        {
            while(i<6)
            {
                m_l[i] = pL[i];
                i++;
            }
        }
        if(!m_grN || !m_ch->scfsi[1])
        {
            while(i<11)
            {
                m_l[i] = seq->readBitsI(s);
                i++;
            }
        }
        else
        {
            while(i<11)
            {
                m_l[i] = pL[i];
                i++;
            }
        }
        
        s = slen[1][sf];
        if(!m_grN || !m_ch->scfsi[2])
        {
            while(i<16)
            {
                m_l[i] = seq->readBitsI(s);
                i++;
            }
        }
        else
        {
            while(i<16)
            {
                m_l[i] = pL[i];
                i++;
            }
        }
        if(!m_grN || !m_ch->scfsi[3])
        {
            while(i<21)
            {
                m_l[i] = seq->readBitsI(s);
                i++;
            }
        }
        else
        {
            while(i<21)
            {
                m_l[i] = pL[i];
                i++;
            }
        }
        
        m_l[21] = 0;
        m_l[22] = 0;
    }
}

//-------------------------------------------------------------------------------------------

void ScaleFactor::readScaleFactorMPEG2(engine::Sequence *seq)
{
    static tint stab[3][6][4] = {
        {
            { 6, 11, 16, 21},
            { 6, 11, 18, 21},
            {11, 21, 21, 21},
            { 7, 14, 21, 21},
            { 6, 12, 18, 21},
            { 8, 16, 21, 21}

        },
        {
            { 3,  6,  9, 12},
            { 3,  6, 10, 12},
            { 6, 12, 12, 12},
            { 4,  8, 12, 12},
            { 4,  7, 10, 12},
            { 5,  9, 12, 12}
        },
        {
            { 1,  4,  7, 10},
            { 1,  4,  8, 10},
            { 4, 10, 10, 10},
            { 1,  6, 10, 10},
            { 1,  5,  8, 10},
            { 1,  7, 10, 10}
        }
    };
    
    tint i,num,slen,*pnt,*s0,*s1,*s2;
    
    if((m_hdr->mode_ext & 1) & m_chN)
    {
        slen = m_i_slen2[m_gr->scalefac_compress >> 1];
    }
    else
    {
        slen = m_n_slen2[m_gr->scalefac_compress];
    }
    
    m_gr->preflag = (slen >> 15) & 0x00000001;
    
    if(m_gr->block_type==2)
    {
        if(m_gr->mixed_block_flag)
        {
            pnt = stab[2][(slen >> 12) % 6];
            num = slen & 0x00000007;
            slen >>= 3;
            
            if(num)
            {
                for(i=0;i<6;++i)
                {
                    m_l[i] = seq->readBitsI(num);
                }
                m_l[7] = 0;
                m_l[8] = 0;
            }
            else
            {
                for(i=0;i<8;++i)
                {
                    m_l[0] = 0;
                }
            }
            
            s0 = &m_s0[3];
            s1 = &m_s1[3];
            s2 = &m_s2[3];
        }
        else
        {
            pnt = stab[1][(slen >> 12) % 6];
            num = slen & 0x00000007;
            slen >>= 3;
            
            s0 = &m_s0[0];
            s1 = &m_s1[0];
            s2 = &m_s2[0];
            s0[12] = 0;
            s1[12] = 0;
            s2[12] = 0;
        }
        
        i = 0;
        if(num)
        {
            while(i < pnt[0])
            {
                s0[i] = seq->readBitsI(num);
                s1[i] = seq->readBitsI(num);
                s2[i] = seq->readBitsI(num);
                i++;
            }
        }
        else
        {
            while(i < pnt[0])
            {
                s0[i] = 0;
                s1[i] = 0;
                s2[i] = 0;
                i++;
            }
        }
        
        num = slen & 0x00000007;
        slen >>= 3;
        if(num)
        {
            while(i < pnt[1])
            {
                s0[i] = seq->readBitsI(num);
                s1[i] = seq->readBitsI(num);
                s2[i] = seq->readBitsI(num);
                i++;
            }
        }
        else
        {
            while(i< pnt[1])
            {
                s0[i] = 0;
                s1[i] = 0;
                s2[i] = 0;
                i++;
            }
        }
        
        num = slen & 0x00000007;
        slen >>= 3;
        if(num)
        {
            while(i < pnt[2])
            {
                s0[i] = seq->readBitsI(num);
                s1[i] = seq->readBitsI(num);
                s2[i] = seq->readBitsI(num);
                i++;
            }
        }
        else
        {
            while(i < pnt[2])
            {
                s0[i] = 0;
                s1[i] = 0;
                s2[i] = 0;
                i++;
            }
        }
        
        num = slen & 0x00000007;
        if(num)
        {
            while(i < pnt[3])
            {
                s0[i] = seq->readBitsI(num);
                s1[i] = seq->readBitsI(num);
                s2[i] = seq->readBitsI(num);
                i++;
            }
        }
        else
        {
            while(i < pnt[3])
            {
                s0[i] = 0;
                s1[i] = 0;
                s2[i] = 0;
                i++;
            }
        }
    }
    else
    {
        pnt = stab[0][(slen >> 12) % 6];
        i = 0;
        
        num = slen & 0x00000007;
        slen >>= 3;
        if(num)
        {
            while(i < pnt[0])
            {
                m_l[i] = seq->readBitsI(num);
                i++;
            }
        }
        else
        {
            while(i < pnt[0])
            {
                m_l[i] = 0;
                i++;
            }
        }
        
        num = slen & 0x00000007;
        slen >>= 3;
        if(num)
        {
            while(i < pnt[1])
            {
                m_l[i] = seq->readBitsI(num);
                i++;
            }
        }
        else
        {
            while(i < pnt[1])
            {
                m_l[i] = 0;
                i++;
            }
        }
        
        num = slen & 0x00000007;
        slen >>= 3;
        if(num)
        {
            while(i < pnt[2])
            {
                m_l[i] = seq->readBitsI(num);
                i++;
            }
        }
        else
        {
            while(i < pnt[2])
            {
                m_l[i] = 0;
                i++;
            }
        }
        
        num = slen & 0x00000007;
        if(num)
        {
            while(i < pnt[3])
            {
                m_l[i] = seq->readBitsI(num);
                i++;
            }
        }
        else
        {
            while(i< pnt[3])
            {
                m_l[i] = 0;
                i++;
            }
        }
        
        m_l[i] = 0;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
