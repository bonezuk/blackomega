#include "engine/inc/Sequence.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------
Sequence::Sequence() : m_bitstream(0),
    m_item(0),
    m_bitOffset(0),
    m_length(0),
    m_buffer(0),
    m_end(true)
{}

//-------------------------------------------------------------------------------------------

Sequence::~Sequence()
{}

//-------------------------------------------------------------------------------------------

void Sequence::reset()
{
    m_item = 0;
    m_bitOffset = 0;
    m_length = 0;
    m_buffer = 0;
    m_end = true;
}

//-------------------------------------------------------------------------------------------

void Sequence::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Sequence::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool Sequence::previous()
{
    tint streamID;
    bool loop = true;
    
    m_end = true;
    
    if(m_item==0)
    {
        printError("previous","The sequence has reached the end of its stream");
        return false;
    }
    
    streamID = m_item->m_stream;
    m_item->m_sequence = 0;
    do
    {
        m_item = m_item->m_prev;
        if(m_item!=0)
        {
            if(m_item->m_stream==streamID)
            {
                loop = false;
            }
        }
        else
        {
            return false;
        }
    }
    while(loop);
    
    m_item->m_sequence = this;
    m_bitOffset = m_item->m_length;
    m_length = m_item->m_length;
    m_buffer = m_item->m_buffer;
    return true;
}

//-------------------------------------------------------------------------------------------

bool Sequence::next()
{
    tint streamID;
    bool loop = true;
    
    // If there does not exist an association between the instance and the bistream then it is invalid.
    if(m_item==0 || m_bitstream==0)
    {
        m_end = true;
        return false;
    }
    
    if(m_item->m_buffer!=0)
    {
        // Seek for the next item in the stream that relates to the same stream ID.
        streamID = m_item->m_stream;
        m_item->m_sequence = 0;
        do
        {
            m_item = m_item->m_next;
            if(m_item!=0)
            {
                if(m_item->m_stream==streamID)
                {
                    loop = false;
                }
            }
            else
            {
                m_end = true;
                return false;
            }
        }
        while(loop);
    }
    
    m_item->m_sequence = this;
    m_bitOffset = m_item->m_offset;
    m_length = m_item->m_length;
    m_buffer = m_item->m_buffer;
    
    // If the stream detects that it has come across an empty buffer it
    // implies that it is looking at the last (infinite) item.
    if(m_item->m_buffer==0)
    {
        if(!m_bitstream->readInNextBuffer())
        {
            m_end = true;
            return false;
        }
    }
    m_end = false;
    return true;
}

//-------------------------------------------------------------------------------------------

bool Sequence::checkNext()
{
    Associate *item = m_item;
    tint streamID;
    bool loop = true;
    
    // If there does not exist an association between the instance and the bitstream then it is invalid.
    if(m_item==0 || m_bitstream==0)
    {
        return false;
    }
    
    if(item->m_buffer!=0)
    {
        streamID = m_item->m_stream;
        do
        {
            item = item->m_next;
            if(item!=0)
            {
                if(item->m_stream==streamID)
                {
                    loop = false;
                }
            }
            else
            {
                return false;
            }
        }
        while(loop);
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool Sequence::seek(tint offset)
{
    if(m_item==0)
    {
        return false;
    }
    
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
            if(static_cast<tint>(m_bitOffset - static_cast<tuint>(offset)) >= static_cast<tint>(m_item->m_offset))
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

tint Sequence::readBits(tint n)
{
    tuint newbitOffset;
    tint i;
    tuint x = 0;
    tuint *mask = Bitstream::m_mask[n][m_bitOffset & 0x00000007];
    
    if(m_end)
    {
        if(!next())
        {
            return 0;
        }
    }
    newbitOffset = m_bitOffset + static_cast<tuint>(n);
    i = m_bitOffset >> 3;
    
    if(newbitOffset <= m_length)
    {
        x |= (static_cast<tuint>(m_buffer[i++]) >> mask[0]) & mask[1];
        n -= static_cast<tint>(mask[2]);
        mask += 3;
        
        while(n>0)
        {
            x |= (static_cast<tuint>(m_buffer[i++]) << mask[0]) & mask[1];
            n -= 8;
            mask += 3;
        }
        m_bitOffset = newbitOffset;
    }
    else
    {
        if(m_bitOffset >= m_length)
        {
            if(!next())
            {
                return 0;
            }
            i = m_bitOffset >> 3;
        }
        
        m_bitOffset += mask[2];
        x |= (static_cast<tuint>(m_buffer[i++]) >> mask[0]) & mask[1];
        n -= static_cast<tint>(mask[2]);
        mask += 3;
        
        while(n>0)
        {
            if(m_bitOffset >= m_length)
            {
                if(!next())
                {
                    return 0;
                }
                i = m_bitOffset >> 3;
            }
            
            m_bitOffset += mask[2];
            x |= (static_cast<tuint>(m_buffer[i++]) << mask[0]) & mask[1];
            n -= 8;
            mask += 3;
        }
    }
    return static_cast<tint>(x);
}

//-------------------------------------------------------------------------------------------

bool Sequence::move(tint bkMark)
{
    Bookmark *book;
    
    if(m_bitstream==0)
    {
        printError("move","Sequence instance not properly associated with bitstream");
        return false;
    }
    
    if(bkMark==-1)
    {
        printError("move","Cannot move to unassociated bookmark");
        return false;
    }
    if(!m_bitstream->m_bookmarks.Exist(bkMark))
    {
        printError("move","Bookmark does not exist");
        return false;
    }
    
    book = m_bitstream->m_bookmarks.Find(bkMark);
    if(book->m_item==0)
    {
        printError("move","No item associated with bookmark");
        return false;
    }
    if(m_item!=0)
    {
        if(m_item->m_stream!=book->m_item->m_stream)
        {
            printError("move","Bookmark does not match given stream");
            return false;
        }
        m_item->m_sequence = 0;
    }
    
    m_item = book->m_item;
    m_item->m_sequence = this;
    m_bitOffset = book->m_offset;
    m_length = m_item->m_length;
    m_buffer = m_item->m_buffer;

    m_end = (m_buffer==0);

    return true;
}

//-------------------------------------------------------------------------------------------

bool Sequence::isEnd() const
{
    return m_end;
}

//-------------------------------------------------------------------------------------------

bool Sequence::isEndStream()
{
    if(m_end)
    {
        if(!next())
        {
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

tint Sequence::bookmark()
{
    tint bkID,cOffset;
    Associate *cItem;
    
    if(m_bitstream==0)
    {
        printError("bookmark","Sequence instance not properly associated with bitstream");
        return -1;
    }
    
    cOffset = static_cast<tint>(m_bitstream->m_bitOffset);
    cItem = m_bitstream->m_currentItem;
    
    if(m_bitOffset==m_length)
    {
        if(!next())
        {
            return -1;
        }
    }
    
    if(m_item!=0 && (m_item->m_offset<=m_bitOffset && m_bitOffset<m_length))
    {
        m_bitstream->m_currentItem = m_item;
        m_bitstream->m_bitOffset = m_bitOffset;
        bkID = m_bitstream->bookmark();
        m_bitstream->m_currentItem = cItem;
        m_bitstream->m_bitOffset = static_cast<tuint>(cOffset);
    }
    else
    {
        bkID = -1;
    }
    return bkID;
}

//-------------------------------------------------------------------------------------------

tint Sequence::copy(tbyte *mem,tint len)
{
    tint offset=0,a,size;
    Associate *item = m_item;
    
    if(mem==0 || len<0)
    {
        return -1;
    }
    
    a = m_bitOffset >> 3;
    
    while(offset<len && item!=0 && item->m_buffer!=0)
    {
        size = static_cast<tint>(item->m_length >> 3) - a;
        if(size > (len - offset))
        {
            size = len - offset;
        }
        
        ::memcpy(&mem[offset],&(item->m_buffer[a]),static_cast<tuint>(size));
        offset += size;
        
        if(item->m_next!=0 && item->m_buffer==item->m_next->m_buffer)
        {
            a = item->m_offset >> 3;
        }
        else
        {
            a = 0;
        }
        item = item->m_next;
    }
    return offset;
}

//-------------------------------------------------------------------------------------------

tint Sequence::browseBits(tint n)
{
    tuint newbitOffset;
    tint i;
    tuint x = 0;
    tuint *mask = Bitstream::m_mask[n][m_bitOffset & 0x00000007];
    
    if(m_end)
    {
        if(!next())
        {
            return 0;
        }
    }
    newbitOffset = m_bitOffset + static_cast<tuint>(n);
    i = m_bitOffset >> 3;
    
    if(newbitOffset <= m_length)
    {
        x |= (static_cast<tuint>(m_buffer[i++]) >> mask[0]) & mask[1];
        n -= static_cast<tint>(mask[2]);
        mask += 3;
        
        while(n>0)
        {
            x |= (static_cast<tuint>(m_buffer[i++]) << mask[0]) & mask[1];
            n -= 8;
            mask += 3;
        }
    }
    else
    {
        tint total = 0;
        tint a = -n;
        
        if(m_bitOffset >= m_length)
        {
            if(checkNext())
            {
                if(!next())
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
            i = m_bitOffset >> 3;
        }
        
        total += mask[2];
        m_bitOffset += mask[2];
        x |= (static_cast<tuint>(m_buffer[i++]) >> mask[0]) & mask[1];
        n -= static_cast<tint>(mask[2]);
        mask += 3;
        
        while(n>0)
        {
            if(m_bitOffset >= m_length)
            {
                if(checkNext())
                {
                    if(!next())
                    {
                        return 0;
                    }
                }
                else
                {
                    seek(-total);
                    return static_cast<tint>(x);
                }
                i = m_bitOffset >> 3;
            }
            
            total += mask[2];
            m_bitOffset += mask[2];
            x |= (static_cast<tuint>(m_buffer[i++]) << mask[0]) & mask[1];
            n -= 8;
            mask += 3;
        }
        
        seek(a);
    }
    return static_cast<tint>(x);
}

//-------------------------------------------------------------------------------------------

tint Sequence::readMemory(tbyte *mem,tint len)
{
    return readMemory(reinterpret_cast<tubyte *>(mem),static_cast<tuint>(len));
}

//-------------------------------------------------------------------------------------------

tint Sequence::readMemory(tubyte *mem,tuint len)
{
    tuint offset,sAmount,sRemain,sLen,amount = 0;
    
    byteAlignment();
    
    offset = m_bitOffset >> 3;
    sLen = m_length >> 3;
    
    while(amount < len)
    {
        if(offset < sLen)
        {
            sAmount = len - amount;
            sRemain = sLen - offset;
            sAmount = (sAmount > sRemain) ? sRemain : sAmount;
            
            ::memcpy(&mem[amount],&m_buffer[offset],sAmount);
            amount += sAmount;
            offset += sAmount;
        }
        else
        {
            if(next())
            {
                offset = m_bitOffset >> 3;
                sLen = m_length >> 3;
            }
            else
            {
                break;
            }
        }
    }
    m_bitOffset = offset << 3;
    return static_cast<int>(amount);
}

//-------------------------------------------------------------------------------------------

tint Sequence::readBitI()
{
    tint a,n;

    if(m_end || m_bitOffset>=m_length)
    {
        if(!next())
        {
            return 0;
        }
    }
    
    tuint32 *buffer = &(reinterpret_cast<tuint32 *>(m_buffer)[m_bitOffset >> 5]);

    a = 31 - (m_bitOffset & 0x0000001f);
    if(a)
    {
        n = static_cast<tint>(((*buffer) >> a) & 0x00000001);
    }
    else
    {
        n = static_cast<tint>((*buffer) & 0x00000001);
    }
    m_bitOffset++;
    return n;
}

//-------------------------------------------------------------------------------------------

tint Sequence::readBitsI(tint n)
{
    Bitstream::BSMask *ms = &(Bitstream::m_maskI[n][m_bitOffset & 0x0000001f]);
    tint s = m_length - m_bitOffset;
    
    if(!n)
    {
        return 0;
    }

    if(s < ms->required || m_end)
    {
        tint a,shift,offset,p = 0;
        tuint32 *buffer;
        tuint mask;
        
        if(!s || m_end)
        {
            if(!next())
            {
                return 0;
            }
            buffer = &(reinterpret_cast<tuint32 *>(m_buffer)[m_bitOffset >> 5]);
            s = m_length - m_bitOffset;
            offset = m_bitOffset & 0x0000001f;
        }
        else
        {
            buffer = &(reinterpret_cast<tuint32 *>(m_buffer)[m_bitOffset >> 5]);
            offset = m_bitOffset & 0x0000001f;
        }
        
        while(n > 0)
        {
            a = (s < (32 - offset)) ? s : (32 - offset);
            a = (a < n) ? a : n;
            mask = (a==32) ? 0xffffffff : static_cast<unsigned int>((1 << a) - 1);
            mask = mask << (n - a);
            shift = 32 - (offset + n);
            
            if(shift>0)
            {
                p |= static_cast<tint>(((*buffer) >> shift) & mask);
            }
            else if(shift<0)
            {
                shift = -shift;
                p |= static_cast<tint>(((*buffer) << shift) & mask);
            }
            else
            {
                p |= static_cast<tint>((*buffer) & mask);
            }
            s -= a;
            m_bitOffset += a;
            
            if(!s)
            {
                if(!next())
                {
                    return 0;
                }
                s = m_length - m_bitOffset;
                offset = m_bitOffset & 0x0000001f;
                buffer = &(reinterpret_cast<tuint32 *>(m_buffer)[m_bitOffset >> 5]);
            }
            else if((offset + a) >= 32)
            {
                offset = 0;
                buffer++;
            }
            else
            {
                offset += a;
            }
            n -= a;
        }
        return p;
    }
    else
    {
        tuint32 *buffer = &(reinterpret_cast<tuint32 *>(m_buffer)[m_bitOffset >> 5]);

        m_bitOffset += n;        
        if(ms->width==2)
        {
            n  = static_cast<tint>(((*buffer) << ms->rShiftA) & ms->rMaskA);
            buffer++;
            n |= static_cast<tint>(((*buffer) >> ms->rShiftB) & ms->rMaskB);
        }
        else
        {
            n  = static_cast<tint>(((*buffer) >> ms->rShiftA) & ms->rMaskA);
        }
    }
    return n;
}

//-------------------------------------------------------------------------------------------

tuint64 Sequence::read64BitsI()
{
    tuint64 x;
    tuint32 a,b;
    a = static_cast<tuint32>(readBitsI(32));
    b = static_cast<tuint32>(readBitsI(32));
    x = ((static_cast<tuint64>(a) << 32) & 0xffffffff00000000ULL) | (static_cast<tuint64>(b) & 0x00000000ffffffffULL);
    return x;
}

//-------------------------------------------------------------------------------------------

tint Sequence::readBitsIL(tint n)
{
    tint x = readBitsI(n);
    x = ((x << 24) & 0xff000000 ) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
    return x;
}

//-------------------------------------------------------------------------------------------

tint Sequence::readBitsSignI(tint n)
{
    tint x = readBitsI(n);
    x <<= (32 - n);
    x >>= (32 - n);
    return x;
}

//-------------------------------------------------------------------------------------------

bool Sequence::readBookmarkMemory(tint bkStart,tint bkEnd,common::Array<tubyte,tubyte>& arr)
{
    bool res = false;
    
    if(move(bkStart))
    {
        Bookmark *bEnd;
        
        byteAlignment();
        if(m_bitstream->m_bookmarks.Exist(bkEnd))
        {
            bEnd = m_bitstream->m_bookmarks.Find(bkEnd);
            
            if(bEnd->m_item!=0 && bEnd->m_item->m_stream==m_item->m_stream)
            {
                tubyte *mem;
                tint offset,len,lenB,oLen;
                bool litteEndian = m_bitstream->isLittleEndian(),flag = true;

                res = true;
                
                while((m_item!=bEnd->m_item || flag) && res)
                {
                    tuint32 *m,*b;

                    offset = m_bitOffset >> 3;
                    if(m_item==bEnd->m_item)
                    {
                        len = bEnd->m_offset >> 3;
                    }
                    else
                    {
                        len = m_length >> 3;
                    }

                    oLen = arr.GetSize();
                    arr.SetSize(oLen + (len - offset));
                    mem = arr.GetData();
                    mem += oLen;

                    while((offset & 0x00000003) && offset<len)
                    {
                        *mem++ = static_cast<tubyte>(readBitsI(8));
                        offset++;
                    }

                    m = reinterpret_cast<tuint32 *>(mem);
                    b = reinterpret_cast<tuint32 *>(&(m_buffer[offset]));
                    lenB = len & 0xfffffffc;
                    if(litteEndian)
                    {
                        tuint32 x;

                        while(offset < lenB)
                        {
                            x = *b++;
                            *m++ = ((x << 24) & 0xff000000 ) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00) | ((x >> 24) & 0x000000ff);
                            offset+=4;
                        }
                    }
                    else
                    {
                        while(offset < lenB)
                        {
                            *m++ = *b++;
                            offset+=4;
                        }
                    }

                    mem = reinterpret_cast<tubyte *>(m);
                    m_bitOffset = static_cast<tuint32>(offset << 3);
                    while(offset<len)
                    {
                        *mem++ = static_cast<tubyte>(readBitsI(8));
                        offset++;
                    }

                    if(m_item!=bEnd->m_item)
                    {
                        if(!next())
                        {
                            printError("readBookmarkMemory","Could not get next item in sequence");
                            res = false;
                        }
                    }
                    else
                    {
                        flag = false;
                    }
                }
                if(res)
                {
                    if(!move(bkEnd))
                    {
                        printError("readBookmarkMemory","Error placing sequence at end bookmark");
                        res = false;
                    }
                }
            }
            else
            {
                printError("readBookmarkMemory","End bookmark doesn't belong to stream");
            }
        }
        else
        {
            printError("readBookmarkMemory","Cannot find end bookmark");
        }
    }
    else
    {
        printError("readBookmarkMemory","Cannot find start bookmark");
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
