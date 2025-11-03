//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERFLOORBASE_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERFLOORBASE_H
//-------------------------------------------------------------------------------------------

#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverInfoObject.h"
#include "engine/silveromega/inc/VSilverCodecInformation.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

typedef enum
{
    e_floorData0,
    e_floorData1,
    e_floorUnknown
} VSilverFloorType;

#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERWINDOW_H
class VSilverWindow;
#endif

class VSilverCodecData;

//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverFloorBase : public VSilverInfoObject
{
    public:
        VSilverFloorBase(VSilverCodecInformation *cInfo,VSilverCodecData *cData,VSilverFloorType type);
        virtual ~VSilverFloorBase();

        VSilverFloorType type() const;

        virtual void *decode(VSilverCodecData *info,engine::Sequence *seq);

        virtual void product(VSilverWindow *win,void *memo,tfloat32 *out);

    protected:

        VSilverFloorType m_type;

        VSilverCodecInformation *m_cInformation;
        VSilverCodecData *m_cData;
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

