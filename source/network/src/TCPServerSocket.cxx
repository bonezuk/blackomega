#include "network/inc/TCPServerSocket.h"
#include "network/inc/TCPService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

TCPServerSocket::TCPServerSocket(Service *svr,QObject *parent) : Socket(svr,parent),
    m_port(0)
{}

//-------------------------------------------------------------------------------------------

TCPServerSocket::~TCPServerSocket()
{
    try
    {
        TCPServerSocket::close();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void TCPServerSocket::printError(const tchar *strR,const tchar *strE) const
{
#if defined(OMEGA_WIN32)
    tint err = static_cast<tint>(::WSAGetLastError());
#elif defined(OMEGA_POSIX)
    tint err = errno;
#endif
    Resource::instance().error("TCPServerSocket",strR,strE,err);
}

//-------------------------------------------------------------------------------------------

void TCPServerSocket::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("TCPServerSocket",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

bool TCPServerSocket::open(tint port)
{
    struct sockaddr_in addr;

#if defined(OMEGA_WIN32)
    m_socket = ::socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
#elif defined(OMEGA_POSIX)
    m_socket = ::socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
#endif
    if(m_socket==c_invalidSocket)
    {
        printError("open","Failed to obtain socket for server");
        return false;
    }

    m_port = port;

    ::memset(&addr,0,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
#if defined(OMEGA_WIN32)
    addr.sin_port = ::htons(static_cast<u_short>(m_port));
    addr.sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);
#elif defined(OMEGA_POSIX)
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

    if(::bind(m_socket,reinterpret_cast<const struct sockaddr *>(&addr),sizeof(struct sockaddr_in))!=0)
    {
        printError("open","Failed to bind socket to port");
        return false;
    }
    if(::listen(m_socket,16)!=0)
    {
        printError("open","Unable to listen on given TCP port");
        return false;
    }

#if defined(OMEGA_WIN23)
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

    m_state = 0;

    return true;
}

//-------------------------------------------------------------------------------------------

void TCPServerSocket::close()
{
    if(m_socket!=c_invalidSocket)
    {
        closeSocket(m_socket);
        m_socket = c_invalidSocket;
    }
}

//-------------------------------------------------------------------------------------------

bool TCPServerSocket::canRead() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool TCPServerSocket::canWrite() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

void TCPServerSocket::acceptAndCloseConnection()
{
    socket_type clientSocket;
    struct sockaddr_in addr;
    addrlen_type len = sizeof(struct sockaddr_in);

    clientSocket = ::accept(m_socket, reinterpret_cast<struct sockaddr *>(&addr), &len);
    if(clientSocket != c_invalidSocket)
    {
        closeSocket(clientSocket);
    }
}

//-------------------------------------------------------------------------------------------

bool TCPServerSocket::doRead()
{
    TCPConnServerSocket *io = newIO();

    if(io != 0)
    {
        if(!io->open(m_socket))
        {
            printError("doRead","Error getting new connection");
            delete io;
        }
    }
    else
    {
        QString err = QString("Cannot establish connection on port %1 at this time").arg(port());
        printError("doRead", err.toUtf8().constData());
        acceptAndCloseConnection();
    }
    m_state = 0;
    return true;
}

//-------------------------------------------------------------------------------------------

bool TCPServerSocket::doWrite()
{
    return true;
}

//-------------------------------------------------------------------------------------------

TCPConnServerSocket *TCPServerSocket::newIO()
{
    TCPConnServerSocket *io = new TCPConnServerSocket(m_service,this);
    return io;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
