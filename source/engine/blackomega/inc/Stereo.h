//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_STEREO_H
#define __OMEGA_ENGINE_BLACKOMEGA_STEREO_H
//-------------------------------------------------------------------------------------------

#include "engine/blackomega/inc/DequantizeStereo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

class Stereo
{
    public:
        Stereo();
        ~Stereo();

        void set(MPHeaderInfo *p);
        void set(GrInfoS *p);
        void set(ScaleFactor *p);
        void set(sample_t *pL,sample_t *pR);
        void set(tint bL,tint bS0,tint bS1,tint bS2);

        sample_t *getLeft();
        sample_t *getRight();

        void process();

    protected:

        MPHeaderInfo *m_hdr;
        GrInfoS *m_gr;
        ScaleFactor *m_scaleFactor;

        bool m_msStereo;
        bool m_iStereo;

        sample_t m_ioA1[16];
        sample_t m_ioA2[16];
        sample_t m_ioB1[16];
        sample_t m_ioB2[16];
        sample_t m_lArr[16];
        sample_t m_rArr[16];
        sample_t *m_ioA;

        sample_t *m_xrL;
        sample_t *m_xrR;
        sample_t m_lrL[SBLIMIT * SSLIMIT];
        sample_t m_lrR[SBLIMIT * SSLIMIT];

        tint m_bandL;
        tint m_bandS0;
        tint m_bandS1;
        tint m_bandS2;

        void init();

        tint mCopy(tint i,tint limit);
        tint iCopy(tint i,tint limit,tint factor);
        tint lCopy(tint i,tint limit,tint factor);

        void mixLong();
        void mixShort();
        void mixMulti();

        tint shortMax();
};

//-------------------------------------------------------------------------------------------

inline void Stereo::set(MPHeaderInfo *p)
{
    m_hdr = p;
}

//-------------------------------------------------------------------------------------------

inline void Stereo::set(GrInfoS *p)
{
    m_gr = p;
}

//-------------------------------------------------------------------------------------------

inline void Stereo::set(ScaleFactor *p)
{
    m_scaleFactor = p;
}

//-------------------------------------------------------------------------------------------

inline void Stereo::set(sample_t *pL,sample_t *pR)
{
    m_xrL = pL;
    m_xrR = pR;
}

//-------------------------------------------------------------------------------------------

inline void Stereo::set(tint bL,tint bS0,tint bS1,tint bS2)
{
    m_bandL = bL;
    m_bandS0 = bS0;
    m_bandS1 = bS1;
    m_bandS2 = bS2;
}

//-------------------------------------------------------------------------------------------

inline sample_t *Stereo::getLeft()
{
    return m_lrL;
}

//-------------------------------------------------------------------------------------------

inline sample_t *Stereo::getRight()
{
    return m_lrR;
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

