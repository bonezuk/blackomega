#include "network/inc/Socket.h"
#include "network/inc/Controller.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

Socket::Socket(Service *svr,QObject *parent) : QObject(parent),
    m_service(svr),
    m_socket(c_invalidSocket),
    m_state(0)
{
    if(m_service!=0)
    {
        m_service->controller()->addSocket(m_service,this);
    }
}

//-------------------------------------------------------------------------------------------

Socket::~Socket()
{
    try
    {
        if(m_service!=0)
        {
            m_service->controller()->delSocket(m_service,this);
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void Socket::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Socket::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void Socket::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("Socket",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

void Socket::setForRead()
{
    m_state |= c_socketStateRead;
}

//-------------------------------------------------------------------------------------------

void Socket::setForWrite()
{
    m_state |= c_socketStateWrite;
}

//-------------------------------------------------------------------------------------------

void Socket::closeSocket(socket_type s)
{
    if(s != c_invalidSocket)
    {
#if defined(OMEGA_WIN32)
        ::shutdown(m_socket,SD_BOTH);
        if(::closesocket(m_socket)!=0)
        {
            printError("closeSocket","Error closing socket");
        }
#elif defined(OMEGA_POSIX)
        ::shutdown(m_socket,SHUT_RDWR);
        if(::close(m_socket)!=0)
        {
            printError("closeSocket","Error closing socket");
        }
#endif    
    }
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
