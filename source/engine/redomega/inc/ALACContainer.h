//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REDOMEGA_ALACCONTAINER_H
#define __OMEGA_ENGINE_REDOMEGA_ALACCONTAINER_H
//-------------------------------------------------------------------------------------------

#include "engine/redomega/inc/ALACDescription.h"
#include "engine/redomega/inc/ALACSpecificConfig.h"
#include "engine/redomega/inc/ALACFrameIndex.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

class REDOMEGA_EXPORT ALACContainer
{
    public:
        ALACContainer();
        virtual ~ALACContainer();
        
        const ALACDescription& description() const;
        const ALACSpecificConfig& config() const;
        const ALACFrameIndex& frameIndex() const;
        
        virtual bool init(engine::File *f);
        
        const tint& dataOffset() const;
        const tint& dataLength() const;
        
    protected:
    
        ALACDescription m_description;
        ALACSpecificConfig m_config;
        ALACFrameIndex m_frameIndex;
        tint m_dataOffset;
        tint m_dataLength;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------

inline const ALACDescription& ALACContainer::description() const
{
    return m_description;
}

//-------------------------------------------------------------------------------------------

inline const ALACSpecificConfig& ALACContainer::config() const
{
    return m_config;
}

//-------------------------------------------------------------------------------------------

inline const ALACFrameIndex& ALACContainer::frameIndex() const
{
    return m_frameIndex;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACContainer::dataOffset() const
{
    return m_dataOffset;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACContainer::dataLength() const
{
    return m_dataLength;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

