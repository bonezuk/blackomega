#include "engine/violetomega/inc/IFFTextChunk.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(IFFChunkFactory,IFFChunk, \
                            IFFNameChunkFactory,IFFTextChunk, \
                            "NAME",false)

CONCRETE_FACTORY_CLASS_IMPL(IFFChunkFactory,IFFChunk, \
                            IFFAuthorChunkFactory,IFFTextChunk, \
                            "AUTH",false)

CONCRETE_FACTORY_CLASS_IMPL(IFFChunkFactory,IFFChunk, \
                            IFFCopyrightChunkFactory,IFFTextChunk, \
                            "(c) ",false)

CONCRETE_FACTORY_CLASS_IMPL(IFFChunkFactory,IFFChunk, \
                            IFFAnnotationChunkFactory,IFFTextChunk, \
                            "ANNO",false)

//-------------------------------------------------------------------------------------------

IFFTextChunk::IFFTextChunk() : IFFChunk(),
    m_text()
{}

//-------------------------------------------------------------------------------------------

IFFTextChunk::~IFFTextChunk()
{}

//-------------------------------------------------------------------------------------------

bool IFFTextChunk::scan()
{
    bool res = true;

    if(filePositionToStart() && size()>0)
    {
        char *mem = new char [size() + 1];

        if(m_file->read(mem,size())==size())
        {
            mem[size()] = '\0';
            m_text = QString::fromLatin1(mem,size());
        }
        else
        {
            res = false;
        }
        delete [] mem;
    }
    else
    {
        m_text.clear();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

const QString& IFFTextChunk::text() const
{
    return m_text;
}

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

