//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_VIOLETOMEGA_IFFCHUNK_H
#define __OMEGA_ENGINE_VIOLETOMEGA_IFFCHUNK_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include "common/inc/Factory.h"
#include "engine/inc/FormatType.h"
#include "engine/violetomega/inc/VioletOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

#define IFF_BE_ID(a,b,c,d) (((static_cast<tuint32>(static_cast<tint32>(d)) << 24) & 0xff000000) | \
                            ((static_cast<tuint32>(static_cast<tint32>(c)) << 16) & 0x00ff0000) | \
                            ((static_cast<tuint32>(static_cast<tint32>(b)) <<  8) & 0x0000ff00) | \
                            ((static_cast<tuint32>(static_cast<tint32>(a))      ) & 0x000000ff))

#define IFF_LE_ID(a,b,c,d) (((static_cast<tuint32>(static_cast<tint32>(a)) << 24) & 0xff000000) | \
                            ((static_cast<tuint32>(static_cast<tint32>(b)) << 16) & 0x00ff0000) | \
                            ((static_cast<tuint32>(static_cast<tint32>(c)) <<  8) & 0x0000ff00) | \
                            ((static_cast<tuint32>(static_cast<tint32>(d))      ) & 0x000000ff))


#define IFF_ID(a,b,c,d) IFF_LE_ID(a,b,c,d)

typedef tuint32 IFFID;

//-------------------------------------------------------------------------------------------

class VIOLETOMEGA_EXPORT IFFChunk
{
    public:
    
        typedef enum
        {
            e_EndianLittle = 1,
            e_EndianBig,
            e_EndianUnknown
        } EndianType;
    
    public:
        IFFChunk();
        IFFChunk(common::BIOStream *file,bool littleEndian);
        IFFChunk(IFFID id,common::BIOStream *file);
        virtual ~IFFChunk();
        
        virtual bool readHeader();
        
        virtual IFFID id() const;
        virtual tint size() const;
        virtual EndianType endianType() const;
        
        virtual void setup(common::BIOStream *file,bool littleEndian);
        
        virtual bool scan();

        virtual bool filePositionToStart();
        virtual bool filePositionToEnd();
    protected:
        
        common::BIOStream *m_file;
        
        IFFID m_ID;
        tint m_size;
        bool m_littleEndian;
        
        tint m_bkDataStart;
        tint m_bkDataEnd;
        
        virtual tuint32 read32BitUnsigned(const tbyte *mem) const;
        virtual tint32 read32BitSigned(const tbyte *mem) const;
        virtual tuint16 read16BitUnsigned(const tbyte *mem) const;
        virtual tint16 read16BitSigned(const tbyte *mem) const;
};

typedef QSharedPointer<IFFChunk> IFFChunkSPtr;

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(VIOLETOMEGA_EXPORT,IFFChunkFactory,IFFChunk)

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
