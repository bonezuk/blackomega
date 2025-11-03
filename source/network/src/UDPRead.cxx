#include "network/inc/UDPRead.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

UDPRead::UDPRead(Service *svr,QObject *parent) : Socket(svr,parent),
    m_host(),
    m_port(0),
    m_readArray()
{}

//-------------------------------------------------------------------------------------------

UDPRead::~UDPRead()
{
    try
    {
        UDPRead::close();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void UDPRead::printError(const tchar *strR,const tchar *strE) const
{
#if defined(OMEGA_WIN32)
    tint err = static_cast<tint>(::WSAGetLastError());
#elif defined(OMEGA_POSIX)
    tint err = errno;
#endif
    Resource::instance().error("UDPRead",strR,strE,err);
}

//-------------------------------------------------------------------------------------------

void UDPRead::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("UDPRead",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

bool UDPRead::open(const QString& host,tint port)
{
    tint maxLen = 0;
#if defined(OMEGA_WIN32)
    tint len;
#elif defined(OMEGA_POSIX)
    socklen_t len;
#endif
    struct sockaddr_in addr;
    tuint reUse = 1;

    close();

    m_socket = ::socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(m_socket==c_invalidSocket)
    {
        printError("open","Failed to obtain UDP socket");
        return false;
    }

    if(::setsockopt(m_socket,SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<const tchar *>(&reUse),sizeof(reUse))!=0)
    {
        printError("open","Error in reusing address on UDP socket");
        return false;
    }

    ::memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
#if defined(OMEGA_WIN32)
    addr.sin_port = ::htons(static_cast<u_short>(port));
    addr.sin_addr.S_un.S_addr = ::htonl(INADDR_ANY);
#elif defined(OMEGA_POSIX)
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
#endif

    if(::bind(m_socket,reinterpret_cast<struct sockaddr *>(&addr),sizeof(addr))!=0)
    {
        printError("open","Failed to bind scoket to UDP port");
        return false;
    }

    len = sizeof(tint);
#if defined(OMEGA_WIN32)
    if(::getsockopt(m_socket,SOL_SOCKET,SO_MAX_MSG_SIZE,reinterpret_cast<tchar *>(&maxLen),&len)!=0)
    {
        printError("open","Failed to get maximum message size");
        return false;
    }
#elif defined(OMEGA_POSIX)
    if(::getsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,reinterpret_cast<tchar *>(&maxLen),&len)!=0)
    {
        printError("open","Failed to get maximum message size");
        return false;
    }
#endif
    m_readArray.SetSize(maxLen);

    if(Resource::instance().isMulticastIP(host))
    {
        struct ip_mreq mreq;

#if defined(OMEGA_WIN32)
        mreq.imr_multiaddr.S_un.S_addr = ::inet_addr(host.toLatin1().constData());
        mreq.imr_interface.S_un.S_addr = ::htonl(INADDR_ANY);
#elif defined(OMEGA_POSIX)
        mreq.imr_multiaddr.s_addr = ::inet_addr(host.toLatin1().constData());
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
#endif

        if(::setsockopt(m_socket,IPPROTO_IP,IP_ADD_MEMBERSHIP,reinterpret_cast<tchar *>(&mreq),sizeof(mreq))!=0)
        {
            printError("open","Error adding socketing to multicast session");
            return false;
        }
    }

    int resizeA = 131072*2,resizeB = 0;

    if(::setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,reinterpret_cast<tchar *>(&resizeA),sizeof(int))==0)
    {
        len = sizeof(int);
        ::getsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,reinterpret_cast<tchar *>(&resizeB),&len);
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
        printError("open","Failed to get descriptor flags");
        return false;
    }
#endif

    m_host = host;
    m_port = port;
    m_state = 0;

    return true;
}

//-------------------------------------------------------------------------------------------

void UDPRead::close()
{
    if(m_socket!=c_invalidSocket)
    {
#if defined(OMEGA_WIN32)
        if(::closesocket(m_socket)!=0)
        {
            printError("close","Error closing socket");
        }
#elif defined(OMEGA_POSIX)
        if(::close(m_socket)!=0)
        {
            printError("close","Error closing socket");
        }
#endif

        m_host = "";
        m_port = 0;
        m_socket = c_invalidSocket;
    }
}

//-------------------------------------------------------------------------------------------

bool UDPRead::canRead() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool UDPRead::canWrite() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool UDPRead::doRead()
{
    tint len;
    struct sockaddr_in addr;
#if defined(OMEGA_WIN32)
    tint fromLen;
#elif defined(OMEGA_POSIX)
    socklen_t fromLen;
#endif
    bool res = true;

    if(!(m_state & c_socketStateRead))
    {
        return (m_state & c_socketStateClose) ? false : true;
    }

    while((m_state & c_socketStateRead) && !(m_state & c_socketStateClose))
    {
        fromLen = sizeof(addr);
        len = ::recvfrom(m_socket,reinterpret_cast<tchar *>(m_readArray.GetData()),m_readArray.GetSize(),0,reinterpret_cast<struct sockaddr *>(&addr),&fromLen);
#if defined(OMEGA_WIN32)
        if(len==SOCKET_ERROR)
        {
            tint err = ::WSAGetLastError();
            if(err==WSAEWOULDBLOCK)
            {
                m_state ^= c_socketStateRead;
            }
            else
            {
                m_state ^= c_socketStateClose;
                printError("doRead","Error in reading data from UDP socket",err);
                res = false;
            }
        }
#elif defined(OMEGA_POSIX)
        if(len==-1)
        {
#if defined(OMEGA_LINUX)
            if(errno==EAGAIN || errno==EWOULDBLOCK)
#else
            if(errno==EAGAIN)
#endif
            {
                m_state ^= c_socketStateRead;
            }
            else if(errno!=EINTR)
            {
                m_state ^= c_socketStateClose;
                printError("doRead","Error in reading data from UDP socket",errno);
                res = false;
            }
        }
#endif
        else if(len>0)
        {
            int port;
            QString host;
            NetArraySPtr arr(new NetArray);

            arr->AppendRaw(reinterpret_cast<const tchar *>(m_readArray.GetData()),len);
            Resource::instance().findAddress(host,port,&addr,false);
            Q_EMIT onRecieve(host,port,arr);
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool UDPRead::doWrite()
{
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
