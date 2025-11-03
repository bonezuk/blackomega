//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REDOMEGA_ALACADAPTIVEDECODE_H
#define __OMEGA_ENGINE_REDOMEGA_ALACADAPTIVEDECODE_H
//-------------------------------------------------------------------------------------------

#include "engine/redomega/inc/ALACSequence.h"
#include "engine/redomega/inc/ALACContainer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

#define ALAC_QBSHIFT 9
#define ALAC_QB (1<<ALAC_QBSHIFT)
#define ALAC_PB0 40
#define ALAC_MB0 10
#define ALAC_KB0 14
#define ALAC_MAX_RUN_DEFAULT 255

#define ALAC_MMULSHIFT 2
#define ALAC_MDENSHIFT (ALAC_QBSHIFT - ALAC_MMULSHIFT - 1)
#define ALAC_MOFF ((1<<(ALAC_MDENSHIFT-2)))

#define ALAC_BITOFF 24

//-------------------------------------------------------------------------------------------

class REDOMEGA_EXPORT ALACAdaptiveDecode
{
    public:
        ALACAdaptiveDecode();
        virtual ~ALACAdaptiveDecode();

        void set(const ALACContainer& container,tint p,tint numSamples);
        void decode(ALACSequence *seq,tint maxSize,tint *out,tint len);

    protected:

        tint m_mb;
        tint m_mb0;
        tint m_pb;
        tint m_kb;
        tint m_wb;
        tint m_qb;
        tint m_fw;
        tint m_sw;
        tint m_maxrun;

        tint32 lead(tint32 x);
        tint32 lg3a(tint32 x);
        tuint readDyn32Bits(ALACSequence *seq,tuint m,tuint k,tint maxBits);
        tuint readDyn(ALACSequence *seq,tuint m,tuint k);
};

//-------------------------------------------------------------------------------------------

#define alacMin(a,b) ((a) < (b) ? (a) : (b))

//-------------------------------------------------------------------------------------------

inline tint32 ALACAdaptiveDecode::lead(tint32 x)
{
    tuint32 j=0,c = 1 << 31;

    while(!(c & x) && j<32)
    {
        c >>= 1;
        j++;
    }
    return static_cast<tint32>(j);
}

//-------------------------------------------------------------------------------------------

inline tint32 ALACAdaptiveDecode::lg3a(tint32 x)
{
    x += 3;
    x = lead(x);
    return 31 - x;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

