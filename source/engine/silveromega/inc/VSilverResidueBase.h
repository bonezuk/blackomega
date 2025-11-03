//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERRESIDUEBASE_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERRESIDUEBASE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Allocation.h"
#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverFloorBase.h"
#include "engine/silveromega/inc/VSilverCodebook.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverResidueBase : public VSilverInfoObject
{
    public:
        VSilverResidueBase(VSilverCodecData *cData);
        virtual ~VSilverResidueBase();
        
        virtual bool isValid() const;
        
        virtual bool read(engine::Sequence *seq);
        
        virtual bool setup();
        
        virtual void decode(engine::Sequence *seq,tfloat32 **out,tint *nonzero,tint ch);
        
        tint m_type;
        
        tint m_begin;
        tint m_end;
        
        tint m_grouping;
        tint m_partitions;
        tint m_groupbook;
        
        tint *m_secondstages;
        
        tint m_numBooklist;
        tint *m_booklist;
        
    protected:
    
        VSilverCodecData *m_cData;
        
        common::Allocation m_alloc;
        
        tint m_stages;
        VSilverCodebook *m_phrasebook;
        VSilverCodebook ***m_partbooks;
        tint m_partvals;
        tint **m_decodeMap;
        tint m_n;
        
        tint m_resSamplesPerPartition;
        tint m_resPartitionsPerWord;
        tint m_resN;
        tint m_resPartvals;
        tint m_resPartwords;
        
        tint ***m_resPartword;
        tint **m_res2Partword;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual void decode0(engine::Sequence *seq,tfloat32 **out,tint *nonzero,tint ch);
        virtual void decode1(engine::Sequence *seq,tfloat32 **out,tint *nonzero,tint ch);
        virtual void decode2(engine::Sequence *seq,tfloat32 **out,tint *nonzero,tint ch);
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
