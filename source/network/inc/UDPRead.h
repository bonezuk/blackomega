//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_UDPREAD_H
#define __OMEGA_NETWORK_UDPREAD_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Socket.h"
#include "network/inc/Controller.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT UDPRead : public Socket
{
    public:
        Q_OBJECT
    
    public:
        UDPRead(Service *svr,QObject *parent);
        virtual ~UDPRead();
        
        virtual bool open(const QString& host,tint port);
        virtual void close();
        
        virtual bool canRead() const;
        virtual bool canWrite() const;
        
        virtual bool doRead();
        virtual bool doWrite();
        
    protected:
        
        QString m_host;
        tint m_port;
        
        NetArray m_readArray;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint eNo) const;
        
    signals:
    
        void onRecieve(const QString&,tint,network::NetArraySPtr);
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

