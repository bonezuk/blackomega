//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_TCPCONNCLIENTSOCKET_H
#define __OMEGA_NETWORK_TCPCONNCLIENTSOCKET_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPConnectionSocket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT TCPConnClientSocket : public TCPConnectionSocket
{
    public:
        Q_OBJECT
    
    public:
        TCPConnClientSocket(Service *svr,QObject *parent = 0);
        virtual ~TCPConnClientSocket();
        
        virtual bool open(const tchar *name,tint port);
        virtual bool open(const QString& name,tint port);
        
    protected:
        
        struct sockaddr_in m_addr;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint eNo) const;
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
