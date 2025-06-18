#include "network/http/inc/HTTPService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

CONCRETE_QOBJECT_FACTORY_CLASS_IMPL(ServiceFactory,Service, \
                                    HTTPServiceFactory,HTTPService, \
                                    "http_server",false)

//-------------------------------------------------------------------------------------------
// HTTPServiceEvent
//-------------------------------------------------------------------------------------------

HTTPServiceEvent::HTTPServiceEvent(HTTPServiceEventType t) : QEvent(static_cast<QEvent::Type>(t)),
    m_port(0),
    m_server(0)
{
    m_threadId = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

Qt::HANDLE HTTPServiceEvent::threadId()
{
    return m_threadId;
}

//-------------------------------------------------------------------------------------------

tint HTTPServiceEvent::port() const
{
    return m_port;
}

//-------------------------------------------------------------------------------------------

void HTTPServiceEvent::port(tint x)
{
    m_port = x;
}

//-------------------------------------------------------------------------------------------

HTTPServer *HTTPServiceEvent::getServer()
{
    return m_server;
}

//-------------------------------------------------------------------------------------------

void HTTPServiceEvent::setServer(HTTPServer *x)
{
    m_server = x;
}

//-------------------------------------------------------------------------------------------
// HTTPService
//-------------------------------------------------------------------------------------------

HTTPService::HTTPService(QObject *parent) : TCPService(parent)
{}

//-------------------------------------------------------------------------------------------

HTTPService::~HTTPService()
{}

//-------------------------------------------------------------------------------------------

void HTTPService::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "HTTPService::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

HTTPServer *HTTPService::getServer(tint port)
{
    HTTPServiceEvent *e = new HTTPServiceEvent(HTTPServiceEvent::e_getServerEvent);
    ServiceWaitCondition *c = getCondition();
    HTTPServer *svr;
    
    e->port(port);
    QCoreApplication::postEvent(this,e);
    c->wait();
    svr = reinterpret_cast<HTTPServer *>(c->result());
    return svr;
}

//-------------------------------------------------------------------------------------------

void HTTPService::deleteServer(HTTPServer *svr)
{
    HTTPServiceEvent *e = new HTTPServiceEvent(HTTPServiceEvent::e_deleteServerEvent);    
    e->setServer(svr);
    QCoreApplication::postEvent(this,e);
}

//-------------------------------------------------------------------------------------------

bool HTTPService::event(QEvent *e)
{
    if(e!=0 && static_cast<HTTPServiceEvent::HTTPServiceEventType>(e->type())>=HTTPServiceEvent::e_getServerEvent)
    {
        HTTPServiceEvent::HTTPServiceEventType t = static_cast<HTTPServiceEvent::HTTPServiceEventType>(e->type());
        
        switch(t)
        {
            case HTTPServiceEvent::e_getServerEvent:
                {
                    HTTPServiceEvent *sEvent = reinterpret_cast<HTTPServiceEvent *>(e);
                    HTTPServer *s;
                    s = onGetServer(sEvent->port());
                    processCustomEvent(sEvent,reinterpret_cast<void *>(s));
                }
                break;
                
            case HTTPServiceEvent::e_deleteServerEvent:
                {
                    HTTPServiceEvent *sEvent = reinterpret_cast<HTTPServiceEvent *>(e);
                    onDeleteServer(sEvent->getServer());
                    e->accept();
                }
                break;

            default:
                return QObject::event(e);
        }
        return true;
    }
    else
    {
        return QObject::event(e);
    }
}

//-------------------------------------------------------------------------------------------

void HTTPService::processCustomEvent(HTTPServiceEvent *e,void *results)
{
    ServiceWaitCondition *c;
    
    c = getCondition(e->threadId());
    c->setResult(results);
    c->wake();
    e->accept();
}

//-------------------------------------------------------------------------------------------

HTTPServer *HTTPService::onGetServer(tint port)
{
    QSet<TCPServerSocket *>::iterator ppI;
    HTTPServer *svr = 0;
    
    for(ppI=m_serverSet.begin();ppI!=m_serverSet.end() && svr==0;++ppI)
    {
        TCPServerSocket *sS = *ppI;
        
        svr = dynamic_cast<HTTPServer *>(sS);
        if(svr!=0)
        {
            if(svr->port()!=port)
            {
                svr = 0;
            }
        }
    }
    
    if(svr==0)
    {
        svr = new HTTPServer(this,this);
        if(svr->open(port))
        {
            m_serverSet.insert(reinterpret_cast<TCPServerSocket *>(svr));
        }
        else
        {
            QString errStr;
            errStr = "Error opening HTTP server on port " + QString::number(port);
            printError("onGetServer",errStr.toLatin1().constData());
            delete svr;
            svr = 0;
        }
    }
    return svr;
}

//-------------------------------------------------------------------------------------------

void HTTPService::onDeleteServer(HTTPServer *svr)
{
    if(svr!=0)
    {
        QSet<TCPServerSocket *>::iterator ppI;
        
        svr->close();
        ppI = m_serverSet.find(reinterpret_cast<TCPServerSocket *>(svr));
        if(ppI!=m_serverSet.end())
        {
            m_serverSet.erase(ppI);
        }
        delete svr;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
