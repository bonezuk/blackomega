//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_VIOLETOMEGA_IFFTEXTCHUNK_H
#define __OMEGA_ENGINE_VIOLETOMEGA_IFFTEXTCHUNK_H
//-------------------------------------------------------------------------------------------

#include "engine/violetomega/inc/IFFChunk.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

#define IFF_NAME_ID IFF_ID('N','A','M','E')
#define IFF_AUTH_ID IFF_ID('A','U','T','H')
#define IFF_CPRT_ID IFF_ID('(','c',')',' ')
#define IFF_ANNO_ID IFF_ID('A','N','N','O')

//-------------------------------------------------------------------------------------------

class VIOLETOMEGA_EXPORT IFFTextChunk : public IFFChunk
{
    public:
        IFFTextChunk();
        virtual ~IFFTextChunk();
        
        virtual bool scan();
        
        virtual const QString& text() const;
        
    protected:
    
        QString m_text;
};

typedef QSharedPointer<IFFTextChunk> IFFTextChunkSPtr;

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
