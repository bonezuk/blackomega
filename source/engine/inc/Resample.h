//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_RESAMPLE_H
#define __OMEGA_ENGINE_RESAMPLE_H
//-------------------------------------------------------------------------------------------

#include <math.h>

#include "engine/inc/KaiserFilter.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

#define dKaiser_Npc 4096

#define KAISER_MIN(a,b) (((a)<(b)) ? (a) : (b))
#define KAISER_MAX(a,b) (((a)>(b)) ? (a) : (b))

//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT Resample
{
    public:
        Resample();
        Resample(const Resample& rhs);
        ~Resample();

        const Resample& operator = (const Resample& rhs);

        bool open(tint highQuality,tfloat64 minFactor,tfloat64 maxFactor);
        void close();

        tint filterWidth() const;

        bool interpolate() const;
        void setInterpolate(bool v);

        tint process(tfloat64 factor,sample_t *inBuffer,tint inBufferLen,tint lastFlag,tint& inBufferUsed,sample_t *outBuffer,tint outBufferLen);

    protected:

        sample_t *m_imp;
        sample_t *m_impD;
        sample_t m_lpScl;
        tint m_nmult;
        tint m_nwing;

        tfloat64 m_minFactor;
        tfloat64 m_maxFactor;

        tint m_xSize;
        sample_t *m_x;
        tint m_xP;
        tint m_xRead;
        tint m_xOff;

        tint m_ySize;
        sample_t *m_y;
        tint m_yP;

        tfloat64 m_time;
        bool m_interFlag;

        void printError(const tchar *strR,const tchar *strE) const;

        void allocate();
        void deallocate();

        void copy(const Resample& rhs);

        tfloat64 iZero(tfloat64 x);

        void lrsLpFilter(sample_t *c,tint N,tfloat64 freq,tfloat64 beta,tint num);

        sample_t lrsFilterUp(sample_t *imp,sample_t *impD,tint nwing,bool interp,sample_t *xP,sample_t pH,tint inc);
        sample_t lrsFilterUD(sample_t *imp,sample_t *impD,tint nwing,bool interp,sample_t *xP,sample_t pH,tint inc,sample_t dhb);

        tint lrsSrcUp(sample_t *X,sample_t *Y,tfloat64 factor,tfloat64& currentTime,tint nx,tint nwing,sample_t lpScl,sample_t *imp,sample_t *impD,bool interp);
        tint lrsSrcUD(sample_t *X,sample_t *Y,tfloat64 factor,tfloat64& currentTime,tint nx,tint nwing,sample_t lpScl,sample_t *imp,sample_t *impD,bool interp);
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

