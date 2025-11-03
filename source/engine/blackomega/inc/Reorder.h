//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_REORDER_H
#define __OMEGA_ENGINE_BLACKOMEGA_REORDER_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Allocation.h"
#include "engine/blackomega/inc/Stereo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT Reorder
{
    public:
        Reorder();
        ~Reorder();

        void process();

        void set(MPHeaderInfo *p);
        void set(GrInfoS *p);
        void set(sample_t *p);

    protected:

        common::Allocation m_alloc;
        MPHeaderInfo *m_hdr;
        GrInfoS *m_gr;
        sample_t *m_xr;

        tint *m_seq[100];

        void init();
        void sortBlock(tint block);
};

//-------------------------------------------------------------------------------------------

inline void Reorder::set(MPHeaderInfo *p)
{
    m_hdr = p;
}

//-------------------------------------------------------------------------------------------

inline void Reorder::set(GrInfoS *p)
{
    m_gr = p;
}

//-------------------------------------------------------------------------------------------

inline void Reorder::set(sample_t *p)
{
    m_xr = p;
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

