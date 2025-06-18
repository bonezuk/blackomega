//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_VIOLETOMEGA_IFFID3CHUNK_H
#define __OMEGA_ENGINE_VIOLETOMEGA_IFFID3CHUNK_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOMemory.h"
#include "engine/violetomega/inc/IFFChunk.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

#define IFF_ID3_ID IFF_ID('I','D','3',' ')

//-------------------------------------------------------------------------------------------

class VIOLETOMEGA_EXPORT IFFID3Chunk : public IFFChunk
{
    public:
        IFFID3Chunk();
        virtual ~IFFID3Chunk();
        
        virtual common::BIOMemory *file();
        
    protected:
};

typedef QSharedPointer<IFFID3Chunk> IFFID3ChunkSPtr;

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

