#include "network/inc/MemoryPacket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

const tint c_ioMemoryDefaultSize = 1024;

//-------------------------------------------------------------------------------------------

MemoryPacket::MemoryPacket() : m_memory(0),
    m_length(c_ioMemoryDefaultSize),
    m_wPosition(0),
    m_rPosition(0),
    m_tPosition(0),
    m_transaction(false)
{
    m_memory = new tbyte [static_cast<size_t>(m_length)];
}

//-------------------------------------------------------------------------------------------

MemoryPacket::MemoryPacket(tint len) : m_memory(0),
    m_length(len),
    m_wPosition(0),
    m_rPosition(0),
    m_tPosition(0),
    m_transaction(false)
{
    m_memory = new tbyte [static_cast<size_t>(m_length)];
}

//-------------------------------------------------------------------------------------------

MemoryPacket::MemoryPacket(const MemoryPacket& rhs) : m_memory(0),
    m_length(c_ioMemoryDefaultSize),
    m_wPosition(0),
    m_rPosition(0),
    m_tPosition(0),
    m_transaction(false)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

MemoryPacket::~MemoryPacket()
{
    if(m_memory!=0)
    {
        delete [] m_memory;
        m_memory = 0;
    }
}

//-------------------------------------------------------------------------------------------

void MemoryPacket::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "MemoryPacket::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void MemoryPacket::copy(const MemoryPacket& rhs)
{
    if(m_memory!=0)
    {
        delete [] m_memory;
    }

    m_length = rhs.m_length;
    m_wPosition = rhs.m_wPosition;
    m_rPosition = rhs.m_rPosition;
    m_tPosition = rhs.m_tPosition;
    m_transaction = rhs.m_transaction;
    m_memory = new tbyte [static_cast<size_t>(m_length)];

    ::memcpy(m_memory,rhs.m_memory,static_cast<size_t>(m_length));
}

//-------------------------------------------------------------------------------------------

const MemoryPacket& MemoryPacket::operator = (const MemoryPacket& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

tint MemoryPacket::write(const tbyte *mem,tint len)
{
    tint amount;

    if(mem==0 || len<0)
    {
        printError("write","No buffer given to write from");
        return -1;
    }
    if(m_transaction)
    {
        printError("write","Read transaction in progress");
        return -1;
    }

    amount = m_length - m_wPosition;
    if(amount > len)
    {
        amount = len;
    }

    if(amount > 0)
    {
        ::memcpy(&m_memory[m_wPosition],mem,static_cast<size_t>(amount));
        m_wPosition += amount;
    }
    else
    {
        amount = 0;
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

tint MemoryPacket::write(const tubyte *mem,tint len)
{
    return write(reinterpret_cast<const tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tbyte *MemoryPacket::memoryToWrite(tint& len)
{
    if(m_transaction)
    {
        printError("memoryToWrite","Read transaction in progress");
        return 0;
    }
    len = m_length - m_wPosition;
    if(len<=0)
    {
        return 0;
    }
    return &m_memory[m_wPosition];
}

//-------------------------------------------------------------------------------------------

void MemoryPacket::writeIncrement(tint n)
{
    if(n < 0)
    {
        n = 0;
    }
    if(n > (m_length - m_wPosition))
    {
        n = m_length - m_wPosition;
    }
    m_wPosition += n;
}

//-------------------------------------------------------------------------------------------

tint MemoryPacket::writeRemaining()
{
    return (m_length - m_wPosition);
}

//-------------------------------------------------------------------------------------------

tint MemoryPacket::read(tbyte *mem,tint len)
{
    tint amount;

    if(mem==0 || len<0)
    {
        printError("read","No buffer given to read into");
        return -1;
    }

    amount = m_wPosition - m_rPosition;
    if(amount > len)
    {
        amount = len;
    }

    if(amount > 0)
    {
        ::memcpy(mem,&m_memory[m_rPosition],static_cast<size_t>(amount));
        m_rPosition += amount;
    }
    else
    {
        amount = 0;
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

tint MemoryPacket::read(tubyte *mem,tint len)
{
    return read(reinterpret_cast<tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tbyte *MemoryPacket::memoryToRead(tint& len)
{
    len = m_wPosition - m_rPosition;
    if(len<=0)
    {
        return 0;
    }
    return &m_memory[m_rPosition];
}

//-------------------------------------------------------------------------------------------

void MemoryPacket::readIncrement(tint n)
{
    if(n < 0)
    {
        n = 0;
    }
    if(n > (m_wPosition - m_rPosition))
    {
        n = m_wPosition - m_rPosition;
    }
    m_rPosition += n;
}

//-------------------------------------------------------------------------------------------

tint MemoryPacket::readRemaining()
{
    return (m_wPosition - m_rPosition);
}

//-------------------------------------------------------------------------------------------

void MemoryPacket::transaction()
{
    m_tPosition = m_rPosition;
    m_transaction = true;
}

//-------------------------------------------------------------------------------------------

void MemoryPacket::commit()
{
    m_transaction = false;
}

//-------------------------------------------------------------------------------------------

void MemoryPacket::commit(tint n)
{
    if(n < 0)
    {
        n = 0;
    }
    if(n > (m_rPosition - m_tPosition))
    {
        n = m_rPosition - m_tPosition;
    }
    m_rPosition = m_tPosition + n;
    m_transaction = false;
}

//-------------------------------------------------------------------------------------------

void MemoryPacket::rollback()
{
    m_rPosition = m_tPosition;
    m_transaction = false;
}

//-------------------------------------------------------------------------------------------

void MemoryPacket::reset()
{
    m_wPosition = 0;
    m_rPosition = 0;
    m_tPosition = 0;
    m_transaction = false;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
