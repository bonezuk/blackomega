#include "engine/inc/SequenceMemory.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

common::Mutex SequenceMemory::m_mutex;
tint SequenceMemory::m_nextBookmark = 0;

//-------------------------------------------------------------------------------------------

SequenceMemory::SequenceMemory(common::Array<tubyte,tubyte>& array) : Sequence(),
    m_sArray(new common::Array<tubyte,tubyte>()),
    m_bookmarks(),
    m_originalLen(0)
{
    m_sArray->Copy(array);
    m_originalLen = m_sArray->GetSize() << 3;
    m_length = static_cast<tuint>(m_originalLen);
    m_buffer = m_sArray->GetData();
    m_end = (!m_length);
}

//-------------------------------------------------------------------------------------------

SequenceMemory::SequenceMemory(common::Array<tubyte,tubyte>& array,tint bitLength) : Sequence(),
    m_sArray(new common::Array<tubyte,tubyte>()),
    m_bookmarks(),
    m_originalLen(bitLength)
{
    m_sArray->Copy(array);
    m_length = static_cast<tuint>(m_originalLen);
    m_buffer = m_sArray->GetData();
    m_end = (!m_length);
}

//-------------------------------------------------------------------------------------------

SequenceMemory::SequenceMemory(QSharedPointer<common::Array<tubyte,tubyte> >& array) : Sequence(),
    m_sArray(array),
    m_bookmarks(),
    m_originalLen(0)
{
    m_originalLen = m_sArray->GetSize() << 3;
    m_length = static_cast<tuint>(m_originalLen);
    m_buffer = m_sArray->GetData();
    m_end = (!m_length);
}

//-------------------------------------------------------------------------------------------

SequenceMemory::SequenceMemory(QSharedPointer<common::Array<tubyte,tubyte> >& array,tint bitLength) : Sequence(),
    m_sArray(array),
    m_bookmarks(),
    m_originalLen(bitLength)
{
    m_length = static_cast<tuint>(m_originalLen);
    m_buffer = m_sArray->GetData();
    m_end = (!m_length);
}

//-------------------------------------------------------------------------------------------

SequenceMemory::~SequenceMemory()
{}

//-------------------------------------------------------------------------------------------

void SequenceMemory::reset()
{
    m_item = 0;
    m_bitOffset = 0;
    m_length = static_cast<tuint>(m_originalLen);
    m_buffer = m_sArray->GetData();
    m_end = (!m_length);
}

//-------------------------------------------------------------------------------------------

void SequenceMemory::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "SequenceMemory::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool SequenceMemory::previous()
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool SequenceMemory::next()
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool SequenceMemory::checkNext()
{
    return false;
}

//-------------------------------------------------------------------------------------------

tint SequenceMemory::bookmark()
{
    tint bkMark;
    
    m_mutex.lock();
    bkMark = m_nextBookmark++;
    m_mutex.unlock();
    m_bookmarks.insert(bkMark,m_bitOffset);
    return bkMark;
}

//-------------------------------------------------------------------------------------------

bool SequenceMemory::move(tint bkMark)
{
    QMap<tint,tint>::const_iterator ppI;
    
    ppI = m_bookmarks.find(bkMark);
    if(ppI!=m_bookmarks.end())
    {
        m_bitOffset = static_cast<tuint>(ppI.value());
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

tint SequenceMemory::copy(tbyte *mem,tint len)
{
    return readMemory(mem,len);
}

//-------------------------------------------------------------------------------------------

bool SequenceMemory::seek(tint offset)
{    
    if(offset>=0)
    {
        while(offset>0)
        {
            if((m_bitOffset + static_cast<tuint>(offset)) < m_length)
            {
                m_bitOffset += static_cast<tuint>(offset);
                offset = 0;
            }
            else
            {
                offset -= static_cast<tint>(m_length - m_bitOffset);
                if(!next())
                {
                    m_end = true;
                    return false;
                }
            }
        }
    }
    else
    {
        offset = -offset;
        
        while(offset>0)
        {
            if(static_cast<tint>(m_bitOffset - static_cast<tuint>(offset)) >= 0)
            {
                m_bitOffset -= static_cast<tuint>(offset);
                m_end = false;
                offset = 0;
            }
            else
            {
                offset -= static_cast<tint>(m_bitOffset - m_item->m_offset);
                if(!previous())
                {
                    m_end = true;
                    return false;
                }
            }
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void SequenceMemory::generateArray(const tubyte *mem,tint len,QSharedPointer<common::Array<tubyte,tubyte> >& array)
{
    tint nLen = (len + 3) & 0xfffffffc;
    
    array->SetSize(nLen);
    if(Bitstream::isLittleEndian())
    {
        tint i = 0,t = len >> 2;
        tuint32 x,*m = reinterpret_cast<tuint32 *>(array->GetData());
        const tuint32 *n = reinterpret_cast<const tuint32 *>(mem);
        
        while(i < t)
        {
            x = n[i];
            m[i] = ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
            i++;
        }
        
        t = len & 0x00000003;
        if(t)
        {
            x = n[i];
            switch(t)
            {
                case 1:
                    x &= 0x000000ff;
                    break;
                
                case 2:
                    x &= 0x0000ffff;
                    break;
                
                case 3:
                    x &= 0x00ffffff;
                    break;

                default:
                    break;
            }
            m[i] = ((x << 24) & 0xff000000) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
        }
    }
    else
    {
        tubyte *nMem = array->GetData();
        memcpy(nMem,mem,static_cast<size_t>(len));
    }
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
