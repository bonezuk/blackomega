#include "common/inc/BIOBufferedStream.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

BIOBufferedStream::BIOBufferedStream(tint flags) : BIOStream(flags),
    m_buffer(0),
    m_positionB(0),
    m_length(0),
    m_bufferSize(16384),
    m_readBufferFlag(false),
    m_writeBufferFlag(false),
    m_isOwnBuffer(true)
{
    m_buffer = new tubyte [ m_bufferSize ];
}

//-------------------------------------------------------------------------------------------

BIOBufferedStream::BIOBufferedStream(tint flags,tint bufferSize) : BIOStream(flags),
    m_buffer(0),
    m_positionB(0),
    m_length(0),
    m_bufferSize(bufferSize),
    m_readBufferFlag(false),
    m_writeBufferFlag(false),
    m_isOwnBuffer(true)
{
    m_buffer = new tubyte [ m_bufferSize ];
}

//-------------------------------------------------------------------------------------------

BIOBufferedStream::BIOBufferedStream(tint flags,tbyte *nBuffer,tint bufferSize) : BIOStream(flags),
    m_buffer(reinterpret_cast<tubyte *>(nBuffer)),
    m_positionB(0),
    m_length(0),
    m_bufferSize(bufferSize),
    m_readBufferFlag(false),
    m_writeBufferFlag(false),
    m_isOwnBuffer(false)
{
    m_buffer = new tubyte [ m_bufferSize ];
}

//-------------------------------------------------------------------------------------------

BIOBufferedStream::~BIOBufferedStream()
{
    try
    {
        if(m_isOwnBuffer)
        {
            delete [] m_buffer;
        }
        else if(m_buffer!=0)
        {
            free(m_buffer);
        }
        m_buffer = 0;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void BIOBufferedStream::PrintError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "BIOBufferedStream::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void BIOBufferedStream::PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2) const
{
    BIOStream::PrintError(strR,strE1,strE2);
}

//-------------------------------------------------------------------------------------------

void BIOBufferedStream::PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2,tint code) const
{
    BIOStream::PrintError(strR,strE1,strE2,code);
}

//-------------------------------------------------------------------------------------------

void BIOBufferedStream::flush()
{
    if(m_writeBufferFlag)
    {
        BIOStream::write(reinterpret_cast<const tbyte *>(m_buffer),m_positionB);
    }
    else if(m_readBufferFlag)
    {
        tint64 offset = m_positionB - m_length;
        if(offset < 0)
        {
            BIOStream::seek64(offset,e_Seek_Current);
        }
    }
    m_positionB = 0;
    m_length = 0;
    m_readBufferFlag = false;
    m_writeBufferFlag = false;
}

//-------------------------------------------------------------------------------------------

tint BIOBufferedStream::read(tbyte *mem,tint len)
{
    return read(reinterpret_cast<tubyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tint BIOBufferedStream::read(tubyte *mem,tint len)
{
    tint amount = 0;
    
    if(m_writeBufferFlag)
    {
        flush();
    }
    m_readBufferFlag = true;
    
    while(amount < len)
    {
        if(m_positionB < m_length)
        {
            tint aLen = static_cast<tint>(m_length - m_positionB);
            tint bLen = len - amount;
            tint cLen = (aLen < bLen) ? aLen : bLen;
            memcpy(&mem[amount],&m_buffer[m_positionB],static_cast<size_t>(cLen));
            amount += cLen;
            m_positionB += cLen;
        }
        else
        {
            if(!BIOStream::eof())
            {
                m_positionB = 0;
                m_length = BIOStream::read(reinterpret_cast<tbyte *>(m_buffer),m_bufferSize);                
                if(m_length < 0)
                {
                    return -1;
                }
            }
            else
            {
                break;
            }
        }
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

tint BIOBufferedStream::write(const tbyte *mem,tint len)
{
    return write(reinterpret_cast<const tubyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tint BIOBufferedStream::write(const tubyte *mem,tint len)
{
    tint amount = 0;
    
    if(m_readBufferFlag)
    {
        flush();
    }
    m_writeBufferFlag = true;
    
    while(amount < len)
    {
        if(m_positionB < m_bufferSize)
        {
            tint aLen = static_cast<tint>(m_bufferSize - m_positionB);
            tint bLen = len - amount;
            tint cLen = (aLen < bLen) ? aLen : bLen;
            memcpy(&m_buffer[m_positionB],&mem[amount],static_cast<size_t>(cLen));
            amount += cLen;
            m_positionB += cLen;
        }
        else
        {
            m_length = BIOStream::write(reinterpret_cast<const tbyte *>(m_buffer),m_positionB);
            if(m_length < 0)
            {
                return -1;
            }
            else
            {
                m_positionB = 0;
            }
        }
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

bool BIOBufferedStream::close()
{
    flush();
    return BIOStream::close();
}

//-------------------------------------------------------------------------------------------

bool BIOBufferedStream::seek64(tint64 pos,BIOStreamPosition flag)
{
    tint64 sPos,cPos = offset64();
    
    switch(flag)
    {
        case e_Seek_Start:
            sPos = pos;
            break;

        case e_Seek_End:
            sPos = size64() + pos;
            break;

        case e_Seek_Current:
        default:
            sPos = cPos + pos;
            break;
    }
    
    if(!m_writeBufferFlag)
    {
        tint64 diff = sPos - cPos;
        if(diff<0 && (-diff)<=m_positionB)
        {
            m_positionB += diff;
            return true;
        }
        else if(diff>=0 && diff<(m_length-m_positionB))
        {
            m_positionB += diff;
            return true;
        }
        else
        {
            flush();
            return BIOStream::seek64(pos,flag);
        }
    }
    else
    {
        flush();
        return BIOStream::seek64(pos,flag);
    }
}

//-------------------------------------------------------------------------------------------

bool BIOBufferedStream::sof()
{
    return BIOStream::sof();
}

//-------------------------------------------------------------------------------------------

bool BIOBufferedStream::eof()
{
    if(BIOStream::eof())
    {
        return (m_positionB >= m_length);
    }
    return false;
}

//-------------------------------------------------------------------------------------------

tint BIOBufferedStream::bookmark(tint off)
{
    tint nextID = m_nextBookmarkID;
    tint64 pos = offset64() + off;
    m_Bookmarks.insert(nextID,pos);
    m_nextBookmarkID++;
    return nextID;
}

//-------------------------------------------------------------------------------------------

tint64 BIOBufferedStream::offset64() const
{
    if(m_Position > 0)
    {
        return (m_Position - m_length) + m_positionB;
    }
    else
    {
        return m_positionB;
    }
}

//-------------------------------------------------------------------------------------------

tint64 BIOBufferedStream::size64()
{
    if(m_writeBufferFlag)
    {
        flush();
    }
    return BIOStream::size64();    
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
