//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_SCALEFACTOR_H
#define __OMEGA_ENGINE_BLACKOMEGA_SCALEFACTOR_H
//-------------------------------------------------------------------------------------------

#include "engine/blackomega/inc/SideInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT ScaleFactor
{
    public:
        ScaleFactor();

        void *get();

        void read(engine::Sequence *seq);

        void set(MPHeaderInfo *p);
        void set(ScaleInfoS *p);
        void set(SideInfoS *scale,tint ch,tint gr);

    protected:

        MPHeaderInfo *m_hdr;
        tint m_grN;
        tint m_chN;
        ScaleInfoS *m_prevScale;
        ScaleInfoS m_scale;
        GrInfoS *m_gr;
        ChannelInfoS *m_ch;

        tuint m_sBuf[54];
        tuint m_n_slen2[512];
        tuint m_i_slen2[256];

        tint *m_l;
        tint *m_s0;
        tint *m_s1;
        tint *m_s2;

        void init();

        void readScaleFactorMPEG1(engine::Sequence *seq);
        void readScaleFactorMPEG2(engine::Sequence *seq);
};

//-------------------------------------------------------------------------------------------

inline void *ScaleFactor::get()
{
    return reinterpret_cast<void *>(&m_scale);
}

//-------------------------------------------------------------------------------------------

inline void ScaleFactor::set(MPHeaderInfo *p)
{
    m_hdr = p;
}

//-------------------------------------------------------------------------------------------

inline void ScaleFactor::set(ScaleInfoS *p)
{
    m_prevScale = p;
}

//-------------------------------------------------------------------------------------------

inline void ScaleFactor::set(SideInfoS *info,tint ch,tint gr)
{
    m_grN = gr;
    m_chN = ch;
    m_gr = reinterpret_cast<GrInfoS *>(&(info->ch[ch].gr[gr]));
    m_ch = reinterpret_cast<ChannelInfoS *>(&(info->ch[ch]));
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

