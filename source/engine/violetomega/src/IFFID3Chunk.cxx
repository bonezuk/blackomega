#include "engine/violetomega/inc/IFFID3Chunk.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(IFFChunkFactory,IFFChunk, \
                            IFFID3ChunkFactory,IFFID3Chunk, \
                            "ID3 ",false)

//-------------------------------------------------------------------------------------------

IFFID3Chunk::IFFID3Chunk() : IFFChunk()
{}

//-------------------------------------------------------------------------------------------

IFFID3Chunk::~IFFID3Chunk()
{}

//-------------------------------------------------------------------------------------------

common::BIOMemory *IFFID3Chunk::file()
{
    QByteArray arr(size(),'\0');
    common::BIOMemory *pFile = 0;

    if(filePositionToStart() && m_file->read(arr.data(),size())==size())
    {
        pFile = new common::BIOMemory(arr);
    }
    return pFile;
}

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
