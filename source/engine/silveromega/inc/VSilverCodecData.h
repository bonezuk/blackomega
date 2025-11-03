//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERCODECDATA_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERCODECDATA_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Allocation.h"
#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverCodebook.h"
#include "engine/silveromega/inc/VSilverFloorData0.h"
#include "engine/silveromega/inc/VSilverFloorData1.h"
#include "engine/silveromega/inc/VSilverResidueBase.h"
#include "engine/silveromega/inc/VSilverMapData.h"
#include "engine/silveromega/inc/VSilverCodecInformation.h"
#include "engine/silveromega/inc/VSilverModeData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverCodecData : public VSilverInfoObject
{
    public:
        VSilverCodecData(VSilverCodecInformation *info);
        virtual ~VSilverCodecData();
        
        virtual bool isValid() const;
        
        virtual bool read(engine::Sequence *seq);
        
        virtual bool setup();
        
    public:
    
        common::Allocation m_alloc;
        
        VSilverCodecInformation *m_information;
        
        tint m_numCodebooks;
        VSilverCodebook **m_codebooks;
        
        tint m_numFloors;
        VSilverFloorBase **m_floors;
        
        tint m_numResidues;
        VSilverResidueBase **m_residues;
        
        tint m_numMappings;
        VSilverMapData **m_mappings;
        
        tint m_numModes;
        VSilverModeData **m_modes;
        
        tint m_iLog_vorbis_mode_count;
        
    protected:
    
        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
