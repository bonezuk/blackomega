//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERFLOORDATA1_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERFLOORDATA1_H
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

class SILVEROMEGA_EXPORT VSilverFloorData1 : public VSilverFloorBase
{
    public:
        VSilverFloorData1(VSilverCodecInformation *cInfo,VSilverCodecData *cData);
        virtual ~VSilverFloorData1();

        virtual bool isValid() const;

        virtual bool read(engine::Sequence *seq);

        virtual bool setup();

        virtual void *decode(VSilverCodecData *info,engine::Sequence *seq);

        virtual void product(VSilverWindow *win,void *memo,tfloat32 *out);

        tint m_partitions;
        tint *m_partitionClass;

        tint m_multiplier;

        tint m_maxClass;

        tint m_classDimensions[16];
        tint m_classSub[16];
        tint m_classBook[16];
        tint m_classSubbook[16][8];

        tint m_postList[65];

        tint m_count;

        tint m_loNeighbour[63];
        tint m_hiNeighbour[63];

        tint m_posts;

        tint m_forwardIndex[65];

    protected:

        bool m_read;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        tint renderPoint(tint x0,tint x1,tint y0,tint y1,tint x);
        void renderLine(tint x0,tint x1,tint y0,tint y1,tfloat32 *d);
};

//-------------------------------------------------------------------------------------------

inline tint VSilverFloorData1::renderPoint(tint x0,tint x1,tint y0,tint y1,tint x)
{
    tint off;

    y0 &= 0x00007fff;
    y1 &= 0x00007fff;

    off = (abs(y1 - y0) * (x - x0)) / (x1 - x0);
    if(y1 < y0)
    {
        return (y0 - off);
    }
    else
    {
        return (y0 + off);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

