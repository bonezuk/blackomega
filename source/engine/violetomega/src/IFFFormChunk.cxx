#include "engine/violetomega/inc/IFFFormChunk.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------
// IFFFormChunk::Iterator
//-------------------------------------------------------------------------------------------

IFFFormChunk::Iterator::Iterator() : m_form(0),
    m_index(0)
{}

//-------------------------------------------------------------------------------------------

IFFFormChunk::Iterator::Iterator(const IFFFormChunk::Iterator& rhs) : m_form(rhs.m_form),
    m_index(rhs.m_index)
{}

//-------------------------------------------------------------------------------------------

IFFFormChunk::Iterator::Iterator(IFFFormChunk *pForm,int index) : m_form(pForm),
    m_index(index)
{}

//-------------------------------------------------------------------------------------------

const IFFFormChunk::Iterator& IFFFormChunk::Iterator::operator = (const IFFFormChunk::Iterator& rhs)
{
    if(this!=&rhs)
    {
        m_form = rhs.m_form;
        m_index = rhs.m_index;
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

bool IFFFormChunk::Iterator::isEqual(const IFFFormChunk::Iterator& rhs) const
{
    bool res = false;

    if(m_form!=0 && rhs.m_form!=0)
    {
        if(m_form==rhs.m_form)
        {
            int idxA = m_index;
            int idxB = rhs.m_index;

            if(idxA>=0 && idxA<m_form->numberOfChunks())
            {
                res = (idxA==idxB) ? true : false;
            }
            else if(!(idxB>=0 && idxB<m_form->numberOfChunks()))
            {
                res = true;
            }
        }
    }
    else if(m_form==0 && rhs.m_form==0)
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool IFFFormChunk::Iterator::operator == (const IFFFormChunk::Iterator& b)
{
    return isEqual(b);
}

//-------------------------------------------------------------------------------------------

bool IFFFormChunk::Iterator::operator != (const IFFFormChunk::Iterator& b)
{
    return (!isEqual(b)) ? true : false;
}

//-------------------------------------------------------------------------------------------

IFFChunkSPtr IFFFormChunk::Iterator::operator *()
{
    IFFChunkSPtr pChunk;

    if(m_form!=0)
    {
        pChunk = m_form->at(m_index);
    }
    return pChunk;
}

//-------------------------------------------------------------------------------------------

IFFChunkSPtr IFFFormChunk::Iterator::operator ++()
{
    IFFChunkSPtr pChunk;

    if(m_form!=0)
    {
        m_index++;
        pChunk = m_form->at(m_index);
    }
    return pChunk;
}

//-------------------------------------------------------------------------------------------

IFFChunkSPtr IFFFormChunk::Iterator::operator ++(int unused)
{
    IFFChunkSPtr pChunk;

    if(m_form!=0)
    {
        pChunk = m_form->at(m_index);
        m_index++;
    }
    return pChunk;
}

//-------------------------------------------------------------------------------------------

const int& IFFFormChunk::Iterator::index() const
{
    return m_index;
}

//-------------------------------------------------------------------------------------------
// IFFFormChunk
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(IFFChunkFactory,IFFChunk, \
                            IFFFormChunkFactory,IFFFormChunk, \
                            "FORM",false)

//-------------------------------------------------------------------------------------------

IFFFormChunk::IFFFormChunk() : IFFChunk(),
    m_formType(0),
    m_chunks()
{
    m_ID = IFF_ID('F','O','R','M');
}

//-------------------------------------------------------------------------------------------

IFFFormChunk::~IFFFormChunk()
{}

//-------------------------------------------------------------------------------------------

bool IFFFormChunk::isEmpty() const
{
    return m_chunks.isEmpty();
}

//-------------------------------------------------------------------------------------------

int IFFFormChunk::numberOfChunks() const
{
    return m_chunks.size();
}

//-------------------------------------------------------------------------------------------

IFFChunkSPtr IFFFormChunk::at(int index)
{
    IFFChunkSPtr pChunk;

    if(index>=0 && index<m_chunks.size())
    {
        pChunk = m_chunks[index];
    }
    return pChunk;
}

//-------------------------------------------------------------------------------------------

bool IFFFormChunk::scan()
{
    bool res = true;

    m_chunks.clear();

    if(m_file!=0 && filePositionToStart())
    {
        tbyte mem[4];

        if(m_size>=4 && m_file->read(mem,4)==4)
        {
            int amount = 4;

            m_formType = read32BitUnsigned(mem);

            while(amount<m_size && res)
            {
                IFFChunkSPtr pNChunk = IFFFile::createFromFactory(m_file,endianType());
                if(!pNChunk.isNull() && pNChunk->filePositionToEnd())
                {
                    amount += pNChunk->size() + 8;
                    m_chunks.append(pNChunk);
                }
                else
                {
                    res = false;
                }
            }

            if(res && amount!=m_size)
            {
                res = false;
            }
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

IFFFormChunk::Iterator IFFFormChunk::begin()
{
    return Iterator(this,0);
}

//-------------------------------------------------------------------------------------------

IFFFormChunk::Iterator IFFFormChunk::end()
{
    return Iterator(this,m_chunks.size());
}

//-------------------------------------------------------------------------------------------

IFFID IFFFormChunk::formType() const
{
    return m_formType;
}

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

