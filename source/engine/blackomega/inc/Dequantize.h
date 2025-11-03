//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_DEQUANTIZE_H
#define __OMEGA_ENGINE_BLACKOMEGA_DEQUANTIZE_H
//-------------------------------------------------------------------------------------------

#include "engine/blackomega/inc/BandIndex.h"
#include "engine/blackomega/inc/BSequence.h"
#include "engine/blackomega/inc/SideInfo.h"
#include "engine/blackomega/inc/ScaleFactor.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT Dequantize
{
    public:
        Dequantize();
        virtual ~Dequantize();
        
        void *get();
        
        void set(MPHeaderInfo *x);
        void set(GrInfoS *x);
        void set(ScaleFactor *x);
        
        virtual bool read(engine::Sequence *s);
        
    protected:
        
        MPHeaderInfo *m_hdr;
        GrInfoS *m_gr;
        ScaleFactor *m_scaleFactor;
        
        // dequantized sample buffer
        sample_t m_ro[SBLIMIT + 1][SSLIMIT];
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual void decodeLong(BSequence *seq,tint *reg);
        virtual void decodeShort(BSequence *seq,tint *reg);
        virtual void decodeMixed(BSequence *seq,tint *reg);
        
        void boundaries(tint *reg);
};

//-------------------------------------------------------------------------------------------

inline void *Dequantize::get()
{
    return reinterpret_cast<void *>(&m_ro[0][0]);
}

//-------------------------------------------------------------------------------------------

inline void Dequantize::set(MPHeaderInfo *x)
{
    m_hdr = x;
}

//-------------------------------------------------------------------------------------------

inline void Dequantize::set(GrInfoS *x)
{
    m_gr = x;
}

//-------------------------------------------------------------------------------------------

inline void Dequantize::set(ScaleFactor *x)
{
    m_scaleFactor = x;
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

