#include "network/inc/TCPClientService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

TCPClientService::TCPClientService(QObject *parent) : Service(parent),
    m_clientSet()
{}

//-------------------------------------------------------------------------------------------

TCPClientService::~TCPClientService()
{
    try
    {
        TCPClientService::stop();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void TCPClientService::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "TCPClientService::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void TCPClientService::addConnection(TCPConnectionSocket *s)
{
    m_clientSet.insert(s);
}

//-------------------------------------------------------------------------------------------

void TCPClientService::delConnection(TCPConnectionSocket *s)
{
    QSet<TCPConnectionSocket *>::iterator ppI;

    ppI = m_clientSet.find(s);
    if(ppI!=m_clientSet.end())
    {
        m_clientSet.erase(ppI);
    }
}

//-------------------------------------------------------------------------------------------

bool TCPClientService::start()
{
    return true;
}

//-------------------------------------------------------------------------------------------

void TCPClientService::stop()
{
    QSet<TCPConnectionSocket *>::iterator ppI;

    while(ppI=m_clientSet.begin(),ppI!=m_clientSet.end())
    {
        TCPConnectionSocket *s = *ppI;
        m_clientSet.erase(ppI);
        s->close();
        delete s;
    }
}

//-------------------------------------------------------------------------------------------

bool TCPClientService::process()
{
    QSet<TCPConnectionSocket *>::iterator ppI;
    bool res = true;

    for(ppI=m_clientSet.begin();ppI!=m_clientSet.end();)
    {
        TCPConnectionSocket *s = *ppI;

        if(!s->process())
        {
            if(!s->isLocked())
            {
                ppI = m_clientSet.erase(ppI);
                s->close();
                delete s;
                res = false;
            }
            else
            {
                ++ppI;
            }
        }
        else
        {
            ++ppI;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
