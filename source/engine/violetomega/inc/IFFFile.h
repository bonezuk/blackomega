//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_VIOLETOMEGA_IFFFILE_H
#define __OMEGA_ENGINE_VIOLETOMEGA_IFFFILE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include "engine/violetomega/inc/IFFChunk.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

class VIOLETOMEGA_EXPORT IFFFile
{
    public:
        IFFFile(common::BIOStream *file);
        virtual ~IFFFile();
        
        virtual IFFChunkSPtr root();
        
        static IFFChunkSPtr createFromFactory(common::BIOStream *file,IFFChunk::EndianType type);
        
    protected:
    
        common::BIOStream *m_file;
        IFFChunkSPtr m_root;
        
        static QString headerAsBigEndian(const tbyte *mem);
        static QString headerAsLittleEndian(const tbyte *mem);
};

//-------------------------------------------------------------------------------------------

typedef QSharedPointer<IFFFile> IFFFileSPtr;

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

