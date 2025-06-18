//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_SILVEROMEGA_VSILVERCONTAINER_H
#define __OMEGA_ENGINE_SILVEROMEGA_VSILVERCONTAINER_H
//-------------------------------------------------------------------------------------------

#include "engine/silveromega/inc/SilverOmegaDLL.h"
#include "engine/silveromega/inc/VSilverCodecInformation.h"
#include "engine/silveromega/inc/VSilverCodecComments.h"
#include "engine/silveromega/inc/VSilverCodecData.h"
#include "engine/silveromega/inc/VSilverOgg.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace silveromega
{
//-------------------------------------------------------------------------------------------

class SILVEROMEGA_EXPORT VSilverContainer
{
    public:
        VSilverContainer();
        virtual ~VSilverContainer();
        
        bool read(VSilverOgg *stream);
        
        bool setup();

        bool readInformation(Sequence *seq);
        bool readComments(Sequence *seq);
        bool readData(Sequence *seq);
        
        VSilverCodecInformation *m_information;
        VSilverCodecComments *m_comments;
        VSilverCodecData *m_data;
        
    protected:
    
        void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace silveromega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
