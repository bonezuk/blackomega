#include "engine/blackomega/inc/Stereo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------


const tfloat64 c_stereoRootTwo = 1.4142135623730950488016887242097;

#if defined(SINGLE_FLOAT_SAMPLE)
const sample_t c_stereoRoot2D = 1.4142135623730950488016887242097f;
const sample_t c_stereoIO2D = 0.70710678118654752440084436210485f;
const sample_t c_stereoIO1D = 0.84089641525371454303112547623321f;
#else
const sample_t c_stereoRoot2D = 1.4142135623730950488016887242097;
const sample_t c_stereoIO2D = 0.70710678118654752440084436210485;
const sample_t c_stereoIO1D = 0.84089641525371454303112547623321;
#endif

//-------------------------------------------------------------------------------------------

Stereo::Stereo() : m_hdr(0),
    m_gr(0),
    m_scaleFactor(0),
    m_msStereo(false),
    m_iStereo(false),
    m_ioA(0),
    m_xrL(0),
    m_xrR(0),
    m_bandL(0),
    m_bandS0(0),
    m_bandS1(0),
    m_bandS2(0)
{
    init();
}

//-------------------------------------------------------------------------------------------

Stereo::~Stereo()
{}

//-------------------------------------------------------------------------------------------

void Stereo::init()
{
    tint i;
    tfloat64 ratio;
    
    for(i=0;i<16;++i)
    {
        ratio = tan(static_cast<tfloat64>(i) * 0.26179938779914943653855361527329); // pi / 12
        m_lArr[i] = static_cast<sample_t>(ratio / (1.0 + ratio));
        m_rArr[i] = static_cast<sample_t>(1.0 / (1.0 + ratio));
        if(i & 0x00000001)
        {
            m_ioA1[i] = static_cast<sample_t>(pow(c_stereoIO1D,static_cast<tfloat64>(i + 1) / 2.0));
            m_ioA2[i] = static_cast<sample_t>(pow(c_stereoIO1D,static_cast<tfloat64>(i + 1) / 2.0) * c_stereoRootTwo);
            m_ioB1[i] = static_cast<sample_t>(pow(c_stereoIO2D,static_cast<tfloat64>(i + 1) / 2.0));
            m_ioB2[i] = static_cast<sample_t>(pow(c_stereoIO2D,static_cast<tfloat64>(i + 1) / 2.0) * c_stereoRootTwo);
        }
        else
        {
            m_ioA1[i] = static_cast<sample_t>(pow(c_stereoIO1D,static_cast<tfloat64>(i) / 2.0));
            m_ioA2[i] = static_cast<sample_t>(pow(c_stereoIO1D,static_cast<tfloat64>(i) / 2.0) * c_stereoRootTwo);
            m_ioB1[i] = static_cast<sample_t>(pow(c_stereoIO2D,static_cast<tfloat64>(i) / 2.0));
            m_ioB2[i] = static_cast<sample_t>(pow(c_stereoIO2D,static_cast<tfloat64>(i) / 2.0) * c_stereoRootTwo);
        }
    }
}

//-------------------------------------------------------------------------------------------

tint Stereo::mCopy(tint i,tint limit)
{
    size_t s = static_cast<size_t>((limit - i) * sizeof(sample_t));
    if(s>0)
    {
        memcpy(&m_lrL[i],&m_xrL[i],s);
        memcpy(&m_lrR[i],&m_xrR[i],s);
    }
    return limit;
}

//-------------------------------------------------------------------------------------------

tint Stereo::lCopy(tint i,tint limit,tint factor)
{
    if(factor!=7)
    {        
        if(!factor)
        {
            if(m_msStereo)
            {
                while(i<limit)
                {
                    sample_t t = m_xrL[i] * c_stereoRoot2D;
                    m_lrL[i] = t;
                    m_lrR[i] = t;
                    i++;
                }
            }
            else
            {
                size_t s = static_cast<size_t>((limit - i) * sizeof(sample_t));
                if(s>0)
                {
                    memcpy(&m_lrL[i],&m_xrL[i],s);
                    memcpy(&m_lrR[i],&m_xrL[i],s);
                }
            }
        }
        else if(factor & 0x00000001)
        {
            sample_t k = m_ioA[factor];
            
            if(m_msStereo)
            {
                while(i<limit)
                {
                    sample_t t = m_xrL[i];
                    m_lrL[i] = t * k;
                    m_lrR[i] = t * c_stereoRoot2D;
                    i++;
                }
            }
            else
            {
                while(i<limit)
                {
                    sample_t t = m_xrL[i];
                    m_lrL[i] = t * k;
                    m_lrR[i] = t;
                    i++;
                }
            }
        }
        else
        {
            sample_t k = m_ioA[factor];
            
            if(m_msStereo)
            {
                while(i<limit)
                {
                    sample_t t = m_xrL[i];
                    m_lrL[i] = t * c_stereoRoot2D;
                    m_lrR[i] = t * k;
                    i++;
                }
            }
            else
            {
                while(i<limit)
                {
                    sample_t t = m_xrL[i];
                    m_lrL[i] = t;
                    m_lrR[i] = t * k;
                    i++;
                }
            }
        }
        return limit;
    }
    else
    {
        return mCopy(i,limit);
    }
}

//-------------------------------------------------------------------------------------------

tint Stereo::iCopy(tint i,tint limit,tint factor)
{
    if(factor!=7)
    {
        sample_t k = m_lArr[factor];
        sample_t l = m_rArr[factor];
        
        while(i<limit)
        {
            sample_t t = m_xrL[i];
            m_lrL[i] = t * k;
            m_lrR[i] = t * l;
            i++;
        }
        return limit;
    }
    else
    {
        return mCopy(i,limit);
    }
}

//-------------------------------------------------------------------------------------------

void Stereo::mixLong()
{
    ScaleInfoS *scaleInfo = reinterpret_cast<ScaleInfoS *>(m_scaleFactor->get());
    tint i,j,*sb;
    Band *band = Band::instance();
    
    if(m_bandL==21)
    {
        m_bandL = 22;
    }
    
    i = mCopy(0,band->m_sfBandIndex[m_hdr->sfreq].l[m_bandL]);
    if(m_hdr->lsf)
    {
        j = m_bandL;
        sb = &(band->m_sfBandIndex[m_hdr->sfreq].l[j + 1]);
        while(j < 22)
        {
            tint factor;
            
            if(j==21)
            {
                factor = scaleInfo->l[20];
            }
            else
            {
                factor = scaleInfo->l[j];
            }
            i = lCopy(i,*sb,factor);
            j++;
            sb++;
        }
    }
    else
    {
        j = m_bandL;
        sb = &(band->m_sfBandIndex[m_hdr->sfreq].l[j + 1]);
        while(j < 22)
        {
            tint factor;
            
            if(j==21)
            {
                factor = scaleInfo->l[20];
            }
            else
            {
                factor = scaleInfo->l[j];
            }
            i = iCopy(i,*sb,factor);
            j++;
            sb++;
        }        
    }
}

//-------------------------------------------------------------------------------------------

void Stereo::mixShort()
{
    ScaleInfoS *scaleInfo = reinterpret_cast<ScaleInfoS *>(m_scaleFactor->get());
    tint i,j,*sb;
    tint sMax;
    Band *band = Band::instance();
    
    m_bandS0 = (m_bandS0==12) ? 13 : m_bandS0;
    m_bandS1 = (m_bandS1==12) ? 13 : m_bandS1;
    m_bandS2 = (m_bandS2==12) ? 13 : m_bandS2;
    
    sMax = shortMax();
    
    sb = &(band->m_sfBandIndex[m_hdr->sfreq].s[sMax * 3]);
    i = mCopy(0,*sb++);
    for(j=sMax;j<13;++j)
    {
        tint factor;
        
        if(j<m_bandS0)
        {
            i = mCopy(i,*sb);
        }
        else
        {
            factor = (j==12) ? scaleInfo->s[0][11] : scaleInfo->s[0][j];
            if(m_hdr->lsf)
            {
                i = lCopy(i,*sb,factor);
            }
            else
            {
                i = iCopy(i,*sb,factor);
            }
        }
        sb++;
        
        if(j<m_bandS1)
        {
            i = mCopy(i,*sb);
        }
        else
        {
            factor = (j==12) ? scaleInfo->s[1][11] : scaleInfo->s[1][j];
            if(m_hdr->lsf)
            {
                i = lCopy(i,*sb,factor);
            }
            else
            {
                i = iCopy(i,*sb,factor);
            }
        }
        sb++;
        
        if(j<m_bandS2)
        {
            i = mCopy(i,*sb);
        }
        else
        {
            factor = (j==12) ? scaleInfo->s[2][11] : scaleInfo->s[2][j];
            if(m_hdr->lsf)
            {
                i = lCopy(i,*sb,factor);
            }
            else
            {
                i = iCopy(i,*sb,factor);
            }
        }
        sb++;
    }
}

//-------------------------------------------------------------------------------------------

void Stereo::mixMulti()
{
    static const tint multiChangePoint[9] = { 8, 8, 8, 6, 6, 6, 6, 6, 3 };
    
    ScaleInfoS *scaleInfo = reinterpret_cast<ScaleInfoS *>(m_scaleFactor->get());
    tint i=0,j,*sb;
    tint sMax;
    Band *band = Band::instance();
    
    m_bandS0 = (m_bandS0==12) ? 13 : m_bandS0;
    m_bandS1 = (m_bandS1==12) ? 13 : m_bandS1;
    m_bandS2 = (m_bandS2==12) ? 13 : m_bandS2;
    
    if(m_bandS0 > m_bandS1)
    {
        sMax = (m_bandS0 > m_bandS2) ? m_bandS0 : m_bandS2;
    }
    else
    {
        sMax = (m_bandS1 > m_bandS2) ? m_bandS1 : m_bandS2;
    }
    
    if(sMax<=3)
    {
        tint mPt = multiChangePoint[m_hdr->sfreq];
        if(m_bandL<=mPt)
        {
            sb = &(band->m_sfBandIndex[m_hdr->sfreq].l[m_bandL]);
            i = mCopy(0,*sb++);
            if(m_hdr->lsf)
            {
                for(j=m_bandL;j<mPt;++j,++sb)
                {
                    i = lCopy(i,*sb,scaleInfo->l[j]);
                }
            }
            else
            {
                for(j=m_bandL;j<mPt;++j,++sb)
                {
                    i = iCopy(i,*sb,scaleInfo->l[j]);
                }
            }
        }
    }
    
    j = shortMax();
    sb = &(band->m_sfBandIndex[m_hdr->sfreq].s[j * 3]);
    i = mCopy(i,*sb++);
    while(j < 13)
    {
        tint factor;
        
        if(j<m_bandS0)
        {
            i = mCopy(i,*sb);
        }
        else
        {
            factor = (j==12) ? scaleInfo->s[0][11] : scaleInfo->s[0][j];
            if(m_hdr->lsf)
            {
                i = lCopy(i,*sb,factor);
            }
            else
            {
                i = iCopy(i,*sb,factor);
            }
        }
        sb++;
        
        if(j<m_bandS1)
        {
            i = mCopy(i,*sb);
        }
        else
        {
            factor = (j==12) ? scaleInfo->s[1][11] : scaleInfo->s[1][j];
            if(m_hdr->lsf)
            {
                i = lCopy(i,*sb,factor);
            }
            else
            {
                i = iCopy(i,*sb,factor);
            }
        }
        sb++;
        
        if(j<m_bandS2)
        {
            i = mCopy(i,*sb);
        }
        else
        {
            factor = (j==12) ? scaleInfo->s[2][11] : scaleInfo->s[2][j];
            if(m_hdr->lsf)
            {
                i = lCopy(i,*sb,factor);
            }
            else
            {
                i = iCopy(i,*sb,factor);
            }
        }
        sb++;
        
        j++;
    }
}

//-------------------------------------------------------------------------------------------

tint Stereo::shortMax()
{
    if(m_bandS0 < m_bandS1)
    {
        return (m_bandS0<m_bandS2) ? m_bandS0 : m_bandS2;
    }
    else
    {
        return (m_bandS1<m_bandS2) ? m_bandS1 : m_bandS2;
    }
}

//-------------------------------------------------------------------------------------------

void Stereo::process()
{
    tint i;
    
    if(m_hdr->mode==MPG_MD_JOINT_STEREO)
    {
        m_iStereo = (m_hdr->mode_ext & 0x00000001) ? true : false;
        m_msStereo = (m_hdr->mode_ext & 0x00000002) ? true : false;        
    }
    else
    {
        m_iStereo = false;
        m_msStereo = false;
    }
    
    if(m_gr->scalefac_compress & 0x00000001)
    {
        m_ioA = (m_msStereo) ? m_ioB2 : m_ioB1;
    }
    else
    {
        m_ioA = (m_msStereo) ? m_ioA2 : m_ioA1;
    }
    
    if(m_msStereo)
    {
        for(i=0;i<SBLIMIT*SSLIMIT;++i)
        {
            sample_t t0 = m_xrL[i] / c_stereoRoot2D;
            sample_t t1 = m_xrR[i] / c_stereoRoot2D;
            m_xrL[i] = t0 + t1;
            m_xrR[i] = t0 - t1;
        }
    }
    
    if(m_hdr->stereo==2 && m_iStereo)
    {
        if(m_gr->block_type==2)
        {
            if(m_gr->mixed_block_flag)
            {
                mixMulti();
            }
            else
            {
                mixShort();
            }
        }
        else
        {
            mixLong();
        }
    }
    else
    {
        if(m_hdr->stereo==2)
        {
            i = mCopy(0,SBLIMIT * SSLIMIT);
        }
        else
        {
            memcpy(m_lrL,m_xrL,SBLIMIT * SSLIMIT * sizeof(sample_t));
            memcpy(m_lrR,m_xrL,SBLIMIT * SSLIMIT * sizeof(sample_t));
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
