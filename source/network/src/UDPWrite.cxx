#include "network/inc/UDPWrite.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------
// UDPWrite::SPacket
//-------------------------------------------------------------------------------------------

UDPWrite::SPacket::SPacket(const QString& host,tint port,NetArraySPtr data) : m_host(host),
    m_port(port),
    m_data(data)
{}

//-------------------------------------------------------------------------------------------

UDPWrite::SPacket::SPacket(const SPacket& rhs) : m_host(),
    m_port(0),
    m_data()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

void UDPWrite::SPacket::copy(const SPacket& rhs)
{
    m_host = rhs.m_host;
    m_port = rhs.m_port;
    m_data = rhs.m_data;
}

//-------------------------------------------------------------------------------------------

const UDPWrite::SPacket& UDPWrite::SPacket::operator = (const SPacket& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------
// UDPWrite
//-------------------------------------------------------------------------------------------

UDPWrite::UDPWrite(Service *svr,QObject *parent) : Socket(svr,parent),
    m_host(),
    m_port(0),
    m_outPackets(),
    m_maxLength(0)
{}

//-------------------------------------------------------------------------------------------

UDPWrite::~UDPWrite()
{
    try
    {
        UDPWrite::close();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void UDPWrite::printError(const tchar *strR,const tchar *strE) const
{
#if defined(OMEGA_WIN32)
    tint err = static_cast<tint>(::WSAGetLastError());
#elif defined(OMEGA_POSIX)
    tint err = errno;
#endif
    Resource::instance().error("UDPWrite",strR,strE,err);
}

//-------------------------------------------------------------------------------------------

void UDPWrite::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("UDPWrite",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

bool UDPWrite::open(const QString& host,tint port)
{
    close();
    
    m_socket = ::socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(m_socket==c_invalidSocket)
    {
        printError("open","Failed to obtain UDP socket");
        return false;
    }
    
    {
#if defined(OMEGA_WIN32)
        tint len = sizeof(tint);
    
        if(::getsockopt(m_socket,SOL_SOCKET,SO_MAX_MSG_SIZE,reinterpret_cast<char *>(&m_maxLength),&len)!=0)
        {
            printError("open","Failed to get maximum message size");
            return false;
        }
#elif defined(OMEGA_POSIX)
        socklen_t len = sizeof(tint);
    
        if(::getsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,reinterpret_cast<char *>(&m_maxLength),&len)!=0)
        {
            printError("open","Failed to get message send size");
            return false;
        }
#endif
    }
    
    {
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
    }

    m_host = host;
    m_port = port;
    m_state = 0;
    return true;
}

//-------------------------------------------------------------------------------------------

void UDPWrite::close()
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

bool UDPWrite::canRead() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool UDPWrite::canWrite() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool UDPWrite::send(NetArraySPtr d)
{
    return send(m_host,m_port,d);
}

//-------------------------------------------------------------------------------------------

bool UDPWrite::send(const QString& host,tint port,NetArraySPtr d)
{
    if(d.data()==0)
    {
        printError("send","No data packet given");
        return false;
    }
    if(d->GetSize() < m_maxLength)
    {
        SPacket p(host,port,d);
        m_outPackets.append(p);
        return true;
    }
    else
    {
        printError("send","Given data packet exceeds the maximum transfer unit for this network");
        return false;
    }
}

//-------------------------------------------------------------------------------------------

bool UDPWrite::doRead()
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool UDPWrite::doWrite()
{
    Resource& r = Resource::instance();
    tint len;
    bool res = true;

    if(!(m_state & c_socketStateWrite))
    {
        return (m_state & c_socketStateClose) ? false : true;
    }
    
    while(m_outPackets.size()>0 && (m_state & c_socketStateWrite) && !(m_state & c_socketStateClose))
    {
        SPacket p(m_outPackets.takeFirst());
        struct sockaddr_in addr;
        
        r.getAddress(p.m_host,p.m_port,&addr);
        
        len = ::sendto(m_socket,reinterpret_cast<const tchar *>(p.m_data->GetData()),p.m_data->GetSize(),0,reinterpret_cast<struct sockaddr *>(&addr),sizeof(addr));
#if defined(OMEGA_WIN32)
        if(len==SOCKET_ERROR)
        {
            tint err = ::WSAGetLastError();
            if(err==WSAEWOULDBLOCK)
            {
                m_state ^= c_socketStateWrite;
            }
            else
            {
                m_state ^= c_socketStateClose;
                printError("doWrite","Error while sending data to socket",err);
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
                m_state ^= c_socketStateWrite;
            }
            else
            {
                m_state ^= c_socketStateClose;
                printError("doWrite","Error while sending data to socket",errno);                
                res = false;
            }
        }
#endif
        else if(len!=p.m_data->GetSize())
        {
            m_state ^= c_socketStateClose;
            printError("doWrite","Error in writing complete packet to socket");
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
