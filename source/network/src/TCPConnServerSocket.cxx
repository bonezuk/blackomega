#include "network/inc/TCPConnServerSocket.h"
#include "network/inc/TCPService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

TCPConnServerSocket::TCPConnServerSocket(Service *svr,QObject *parent) : TCPConnectionSocket(svr,parent)
{
    ::memset(&m_address,0,sizeof(struct sockaddr_in));
}

//-------------------------------------------------------------------------------------------

TCPConnServerSocket::~TCPConnServerSocket()
{}

//-------------------------------------------------------------------------------------------

void TCPConnServerSocket::printError(const tchar *strR,const tchar *strE) const
{
#if defined(OMEGA_WIN32)
    tint err = static_cast<tint>(::WSAGetLastError());
#elif defined(OMEGA_POSIX)
    tint err = errno;
#endif
    Resource::instance().error("TCPConnServerSocket",strR,strE,err);
}

//-------------------------------------------------------------------------------------------

void TCPConnServerSocket::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("TCPConnServerSocket",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

bool TCPConnServerSocket::open(socket_type serverS)
{
    struct sockaddr_in addr;
    addrlen_type len = sizeof(struct sockaddr_in);

    close();

    m_socket = ::accept(serverS,reinterpret_cast<struct sockaddr *>(&addr),&len);
    if(m_socket==c_invalidSocket)
    {
        return false;
    }

    ::memcpy(&m_address,&addr,sizeof(struct sockaddr_in));
    Resource::instance().findAddress(m_host,m_port,&addr);

#if defined(OMEGA_WIN32)
    u_long cmdParameter = 1;

    if(::ioctlsocket(m_socket,static_cast<long>(FIONBIO),&cmdParameter)!=0)
    {
        printError("open","Failed to set socket to non-blocking mode");
        return false;
    }
#elif defined(OMEGA_POSIX)
    tint val;

    val = ::fcntl(m_socket,F_GETFL,0);
    if(val!=-1)
    {
        if(::fcntl(m_socket,F_SETFL,val | O_NONBLOCK)==-1)
        {
            printError("open","Failed to set socket non-blocking mode");
            return false;
        }
    }
    else
    {
        printError("open","Failed to get file descriptor flags");
        return false;
    }
#endif

    m_state = 0;
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
