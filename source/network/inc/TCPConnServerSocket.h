//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_TCPCONNSERVERSOCKET_H
#define __OMEGA_NETWORK_TCPCONNSERVERSOCKET_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPConnectionSocket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT TCPConnServerSocket : public TCPConnectionSocket
{
    public:
        Q_OBJECT

    public:
        TCPConnServerSocket(Service *svr,QObject *parent = 0);
        virtual ~TCPConnServerSocket();

        virtual bool open(socket_type serverS);

    protected:

        struct sockaddr_in m_address;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint eNo) const;
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

