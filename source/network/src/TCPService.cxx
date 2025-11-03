#include "network/inc/TCPService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

TCPService::TCPService(QObject *parent) : Service(parent),
    m_serverSet(),
    m_socketSet()
{}

//-------------------------------------------------------------------------------------------

TCPService::~TCPService()
{
    try
    {
        TCPService::stop();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void TCPService::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "TCPService::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool TCPService::process()
{
    QSet<TCPConnectionSocket *>::iterator ppI;
    bool res = true;

    for(ppI=m_socketSet.begin();ppI!=m_socketSet.end();)
    {
        TCPConnectionSocket *s = *ppI;

        if(!s->process() && !s->isLocked())
        {
            ppI = m_socketSet.erase(ppI);
            s->close();
            delete s;
            res = false;
        }
        else
        {
            ++ppI;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void TCPService::addConnection(TCPConnectionSocket *s)
{
    if(s!=0)
    {
        m_socketSet.insert(s);
    }
}

//-------------------------------------------------------------------------------------------

void TCPService::delConnection(TCPConnectionSocket *s)
{
    QSet<TCPConnectionSocket *>::iterator ppI = m_socketSet.find(s);

    if(ppI!=m_socketSet.end())
    {
        m_socketSet.erase(ppI);
    }
}

//-------------------------------------------------------------------------------------------

bool TCPService::start()
{
    return true;
}

//-------------------------------------------------------------------------------------------

void TCPService::stop()
{
    QSet<TCPConnectionSocket *>::iterator ppI;
    QSet<TCPServerSocket *>::iterator ppJ;

    while(ppI=m_socketSet.begin(),ppI!=m_socketSet.end())
    {
        TCPConnectionSocket *s = *ppI;
        m_socketSet.erase(ppI);
        s->close();
        delete s;
    }

    while(ppJ=m_serverSet.begin(),ppJ!=m_serverSet.end())
    {
        TCPServerSocket *sS = *ppJ;
        m_serverSet.erase(ppJ);
        sS->close();
        delete sS;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
