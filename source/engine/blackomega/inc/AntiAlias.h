//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_ANTIALIAS_H
#define __OMEGA_ENGINE_BLACKOMEGA_ANTIALIAS_H
//-------------------------------------------------------------------------------------------

#include "engine/blackomega/inc/Reorder.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT AntiAlias
{
    public:
        AntiAlias();
        ~AntiAlias();
        
        sample_t *process();
        
        void set(GrInfoS *p);
        void set(sample_t *p);
        
    protected:
    
        GrInfoS *m_gr;
        sample_t *m_xr;
        sample_t m_hybrid[SBLIMIT][SSLIMIT];
        
        sample_t m_ca[8];
        sample_t m_cs[8];
        
        void init();
        void mixSection(sample_t *pt,tint sb);
};

//-------------------------------------------------------------------------------------------

inline void AntiAlias::set(GrInfoS *p)
{
    m_gr = p;
}

//-------------------------------------------------------------------------------------------

inline void AntiAlias::set(sample_t *p)
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

