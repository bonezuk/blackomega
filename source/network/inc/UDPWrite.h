//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_UDPWRITE_H
#define __OMEGA_NETWORK_UDPWRITE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Socket.h"
#include "network/inc/Controller.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT UDPWrite : public Socket
{
    public:
        Q_OBJECT
        
        class SPacket
        {
            public:
                SPacket(const QString& host,tint port,NetArraySPtr data);
                SPacket(const SPacket& rhs);
                
                const SPacket& operator = (const SPacket& rhs);
                
                QString m_host;
                tint m_port;
                NetArraySPtr m_data;
                
            protected:
            
                void copy(const SPacket& rhs);
        };
        
    public:
        UDPWrite(Service *svr,QObject *parent);
        virtual ~UDPWrite();
        
        virtual bool open(const QString& host,tint port);
        virtual void close();
        
        virtual bool canRead() const;
        virtual bool canWrite() const;
        
        virtual bool doRead();
        virtual bool doWrite();
        
        virtual bool send(NetArraySPtr d);
        virtual bool send(const QString& host,tint port,NetArraySPtr d);
        
    protected:
        
        QString m_host;
        tint m_port;
        QList<SPacket> m_outPackets;
        tint m_maxLength;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint eNo) const;
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

