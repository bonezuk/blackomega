#include "network/inc/TCPConnClientSocket.h"
#include "network/inc/TCPService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

TCPConnClientSocket::TCPConnClientSocket(Service *svr,QObject *parent) : TCPConnectionSocket(svr,parent)
{}

//-------------------------------------------------------------------------------------------

TCPConnClientSocket::~TCPConnClientSocket()
{}

//-------------------------------------------------------------------------------------------

void TCPConnClientSocket::printError(const tchar *strR,const tchar *strE) const
{
#if defined(OMEGA_WIN32)
    tint err = static_cast<tint>(::WSAGetLastError());
#elif defined(OMEGA_POSIX)
    tint err = errno;
#endif
    Resource::instance().error("TCPConnClientSocket",strR,strE,err);
}

//-------------------------------------------------------------------------------------------

void TCPConnClientSocket::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("TCPConnClientSocket",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

bool TCPConnClientSocket::open(const tchar *name,tint port)
{
    QString n(name);
    return open(n,port);
}

//-------------------------------------------------------------------------------------------

bool TCPConnClientSocket::open(const QString& name,tint port)
{
    QString errStr;

    close();

#if defined(OMEGA_WIN32)
    m_socket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
#elif defined(OMEGA_POSIX)
    m_socket = ::socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
#endif
    if(m_socket==c_invalidSocket)
    {
        printError("open","Failed to obtain socket for client connection");
        return false;
    }

    Resource::instance().getAddress(name,port,&m_addr);

    if(::connect(m_socket,reinterpret_cast<const struct sockaddr *>(&m_addr),sizeof(struct sockaddr_in)))
    {
        errStr = "Failed to connect to port " + QString::number(port) + " at " + name;
        printError("open",errStr.toUtf8().constData());
        return false;
    }

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
            printError("open","Failed to set socket to non-blocking mode");
            return false;
        }
    }
    else
    {
        printError("open","Failed to get file descriptor flags");
        return false;
    }
#endif

    m_host = name;
    m_port = port;
    m_state = 0;
    timerOn();

    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
