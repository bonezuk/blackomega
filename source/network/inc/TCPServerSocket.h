//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_TCPSERVERSOCKET_H
#define __OMEGA_NETWORK_TCPSERVERSOCKET_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Socket.h"
#include "network/inc/Controller.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class TCPService;
class TCPConnServerSocket;

//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT TCPServerSocket : public Socket
{
    public:
        Q_OBJECT

    public:
        TCPServerSocket(Service *svr,QObject *parent = 0);
        virtual ~TCPServerSocket();

        tint port() const;

        virtual bool open(tint port);
        virtual void close();

        virtual bool canRead() const;
        virtual bool canWrite() const;

        virtual bool doRead();
        virtual bool doWrite();

    protected:

        tint m_port;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint eNo) const;

        virtual TCPConnServerSocket *newIO();
        virtual void acceptAndCloseConnection();
};

//-------------------------------------------------------------------------------------------

inline tint TCPServerSocket::port() const
{
    return m_port;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

