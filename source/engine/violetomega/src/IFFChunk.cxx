#include "engine/violetomega/inc/IFFChunk.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(IFFChunkFactory,IFFChunk)

//-------------------------------------------------------------------------------------------

IFFChunk::IFFChunk() : m_file(0),
    m_ID(0xffffffff),
    m_size(0),
    m_littleEndian(false),
    m_bkDataStart(-1),
    m_bkDataEnd(-1)
{}

//-------------------------------------------------------------------------------------------

IFFChunk::IFFChunk(IFFID id,common::BIOStream *file) : m_file(file),
    m_ID(id),
    m_size(0),
    m_littleEndian(false),
    m_bkDataStart(-1),
    m_bkDataEnd(-1)
{}

//-------------------------------------------------------------------------------------------

IFFChunk::IFFChunk(common::BIOStream *file,bool littleEndian) : m_file(file),
    m_ID(0xffffffff),
    m_size(0),
    m_littleEndian(littleEndian),
    m_bkDataStart(-1),
    m_bkDataEnd(-1)
{}

//-------------------------------------------------------------------------------------------

IFFChunk::~IFFChunk()
{}

//-------------------------------------------------------------------------------------------

tuint32 IFFChunk::read32BitUnsigned(const tbyte *mem) const
{
    return (m_littleEndian) ?  to32BitUnsignedFromLittleEndian(mem) : to32BitUnsignedFromBigEndian(mem);
}

//-------------------------------------------------------------------------------------------

tint32 IFFChunk::read32BitSigned(const tbyte *mem) const
{
    return (m_littleEndian) ?  to32BitSignedFromLittleEndian(mem) : to32BitSignedFromBigEndian(mem);
}

//-------------------------------------------------------------------------------------------

tuint16 IFFChunk::read16BitUnsigned(const tbyte *mem) const
{
    return (m_littleEndian) ?  to16BitUnsignedFromLittleEndian(mem) : to16BitUnsignedFromBigEndian(mem);
}

//-------------------------------------------------------------------------------------------

tint16 IFFChunk::read16BitSigned(const tbyte *mem) const
{
    return (m_littleEndian) ?  to16BitSignedFromLittleEndian(mem) : to16BitSignedFromBigEndian(mem);
}

//-------------------------------------------------------------------------------------------

bool IFFChunk::readHeader()
{
    tbyte mem[8];
    bool res = false;

    if(m_file->read(mem,8)==8)
    {
        if(m_ID==0xffffffff)
        {
            m_ID = read32BitUnsigned(mem);
            res = true;
        }
        else
        {
            IFFID hdrID = to32BitUnsignedFromLittleEndian(mem);

            if(hdrID==m_ID)
            {
                m_littleEndian = true;
                res = true;
            }
            else
            {
                hdrID = to32BitUnsignedFromBigEndian(mem);
                if(hdrID==m_ID)
                {
                    m_littleEndian = false;
                    res = true;
                }
            }
        }

        if(res)
        {
            m_size = static_cast<tint>(read32BitUnsigned(&mem[4]));
            m_bkDataStart = m_file->bookmark();
            m_bkDataEnd = m_file->bookmark(m_size);
            if(m_bkDataStart<0 || m_bkDataEnd<0)
            {
                res = false;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

IFFID IFFChunk::id() const
{
    return m_ID;
}

//-------------------------------------------------------------------------------------------

tint IFFChunk::size() const
{
    return m_size;
}

//-------------------------------------------------------------------------------------------

void IFFChunk::setup(common::BIOStream *file,bool littleEndian)
{
    m_file = file;
    m_littleEndian = littleEndian;
}

//-------------------------------------------------------------------------------------------

bool IFFChunk::filePositionToStart()
{
    return m_file->position(m_bkDataStart);
}

//-------------------------------------------------------------------------------------------

bool IFFChunk::filePositionToEnd()
{
    return m_file->position(m_bkDataEnd);
}

//-------------------------------------------------------------------------------------------

IFFChunk::EndianType IFFChunk::endianType() const
{
    return (m_littleEndian) ? e_EndianLittle : e_EndianBig;
}

//-------------------------------------------------------------------------------------------

bool IFFChunk::scan()
{
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

