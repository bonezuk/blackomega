//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WHITEOMEGA_FILLDECODE_H
#define __OMEGA_WHITEOMEGA_FILLDECODE_H
//-------------------------------------------------------------------------------------------

#include "engine/whiteomega/inc/CPEDecode.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

#define ANC_DATA 0

//-------------------------------------------------------------------------------------------

class FillDecode
{
    public:
    
        typedef enum
        {
            e_extDynamicRange = 11,
            e_extFillData     = 1,
            e_extDataElement  = 2,
            e_extFill         = 0
        } FillType;
    
    public:
        FillDecode();
        
        bool decode(Sequence *s);
        
    protected:
    
        tint extensionPayload(WSequence *seq,tint cnt);
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
