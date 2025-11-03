//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_MEMORYPACKET_H
#define __OMEGA_NETWORK_MEMORYPACKET_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Socket.h"
#include "network/inc/Controller.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT MemoryPacket
{
    public:
        MemoryPacket();
        MemoryPacket(tint len);
        MemoryPacket(const MemoryPacket& rhs);
        ~MemoryPacket();
        
        const MemoryPacket& operator = (const MemoryPacket& rhs);
        
        tint write(const tbyte *mem,tint len);
        tint write(const tubyte *mem,tint len);
        tbyte *memoryToWrite(tint& len);
        void writeIncrement(tint n);
        tint writeRemaining();
        
        tint read(tbyte *mem,tint len);
        tint read(tubyte *mem,tint len);
        tbyte *memoryToRead(tint& len);
        void readIncrement(tint n);
        tint readRemaining();
        
        void transaction();
        void commit();
        void commit(tint n);
        void rollback();
        void reset();
        
    protected:
    
        tbyte *m_memory;
        tint m_length;
        tint m_wPosition;
        tint m_rPosition;
        tint m_tPosition;
        bool m_transaction;
        
        void printError(const tchar *strR,const tchar *strE) const;
        
        void copy(const MemoryPacket& rhs);
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

