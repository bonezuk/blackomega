//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERMAPDATA_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERMAPDATA_H
//-------------------------------------------------------------------------------------------

#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverFloorBase.h"
#include "engine/silveromega/inc/VSilverCodecInformation.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverMapData : public VSilverInfoObject
{
    public:
        VSilverMapData(VSilverCodecData *cData,VSilverCodecInformation *info);
        virtual ~VSilverMapData();

        virtual bool isValid() const;

        virtual bool read(engine::Sequence *seq);

        // codec mapping data

        tint m_submap;
        tint m_couplingSteps;
        tint *m_couplingMagnitude;
        tint *m_couplingAngle;
        tint *m_multiplexes;
        tint *m_floorSubmap;
        tint *m_residueSubmap;

    protected:

        VSilverCodecInformation *m_information;
        VSilverCodecData *m_cData;

        bool m_read;

        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

