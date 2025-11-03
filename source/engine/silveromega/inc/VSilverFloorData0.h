//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERFLOORDATA0_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERFLOORDATA0_H
//-------------------------------------------------------------------------------------------

#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverFloorBase.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverFloorData0 : public VSilverFloorBase
{
    public:
        VSilverFloorData0(VSilverCodecInformation *cInfo,VSilverCodecData *cData);
        virtual ~VSilverFloorData0();

        virtual bool isValid() const;

        virtual bool read(engine::Sequence *seq);

        virtual void *decode(VSilverCodecData *info,engine::Sequence *seq);

        virtual void product(VSilverWindow *win,void *memo,tfloat32 *out);

        // codec floor0 data

        tint m_order;
        tint m_rate;
        tint m_barkmap;
        tint m_amplitudeBits;
        tint m_amplitudeOffset;
        tint m_numBooks;
        tint *m_books;

    protected:

        tint *m_linearMap[2];
        tint m_linearN[2];

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual void initLSPMap(VSilverWindow *win);

        virtual void lspToCurve(tfloat32 *curve,tint *map,tint n,tint ln,tfloat32 *lsp,tint m,tfloat32 a,tfloat32 ampoffset);
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

