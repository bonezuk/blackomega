//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERINFOOBJECT_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERINFOOBJECT_H
//-------------------------------------------------------------------------------------------

#include <QString>
#include "engine/inc/Sequence.h"
#include "engine/silveromega/inc/SilverOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverInfoObject
{
    public:
        VSilverInfoObject();
        virtual ~VSilverInfoObject();
        
        virtual bool isValid() const;
        
        virtual bool read(engine::Sequence *seq);
        
        virtual bool setup();
        
    protected:
    
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual void skipPacketIdentifer(engine::Sequence *seq) const;
        virtual bool completePacket(engine::Sequence *seq) const;
        virtual void readString(engine::Sequence *seq,QString& out) const;
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

