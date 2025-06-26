#include "network/http/inc/HTTPClientService.h"
#include "network/http/inc/HTTPClient.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

CONCRETE_QOBJECT_FACTORY_CLASS_IMPL(ServiceFactory,Service, \
                                    HTTPClientServiceFactory,HTTPClientService, \
                                    "http_client",false)

//-------------------------------------------------------------------------------------------
// HTTPClientServiceEvent
//-------------------------------------------------------------------------------------------

HTTPClientServiceEvent::HTTPClientServiceEvent(HTTPClientServiceEventType t) : QEvent(static_cast<QEvent::Type>(t))
{
    m_threadId = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

Qt::HANDLE HTTPClientServiceEvent::threadId()
{
    return m_threadId;
}

//-------------------------------------------------------------------------------------------
// HTTPClientService
//-------------------------------------------------------------------------------------------

HTTPClientService::HTTPClientService(QObject *parent) : TCPClientService(parent)
{
    m_serviceThreadId = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

HTTPClientService::~HTTPClientService()
{}

//-------------------------------------------------------------------------------------------

void HTTPClientService::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "HTTPClientService::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

HTTPClient *HTTPClientService::getClient()
{
    HTTPClient *client;

    if(m_serviceThreadId!=QThread::currentThreadId())
    {
        HTTPClientServiceEvent *e = new HTTPClientServiceEvent(HTTPClientServiceEvent::e_newHTTPClientEvent);
        ServiceWaitCondition *c = getCondition();
        QCoreApplication::postEvent(this,e);
        c->wait();
        client = reinterpret_cast<HTTPClient *>(c->result());
    }
    else
    {
        client = onGetClient();
    }
    return client;
}

//-------------------------------------------------------------------------------------------

bool HTTPClientService::event(QEvent *e)
{
    if(e!=0 && static_cast<HTTPClientServiceEvent::HTTPClientServiceEventType>(e->type())>=HTTPClientServiceEvent::e_newHTTPClientEvent)
    {
        HTTPClientServiceEvent::HTTPClientServiceEventType t = static_cast<HTTPClientServiceEvent::HTTPClientServiceEventType>(e->type());

        switch(t)
        {
            case HTTPClientServiceEvent::e_newHTTPClientEvent:
                {
                    HTTPClient *client = onGetClient();
                    processCustomEvent(reinterpret_cast<HTTPClientServiceEvent *>(e),reinterpret_cast<void *>(client));
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

void HTTPClientService::processCustomEvent(HTTPClientServiceEvent *e,void *result)
{
    ServiceWaitCondition *c;

    c = getCondition(e->threadId());
    c->setResult(result);
    c->wake();
    e->accept();
}

//-------------------------------------------------------------------------------------------

HTTPClient *HTTPClientService::onGetClient()
{
    HTTPClient *c = new HTTPClient(this,this);
    return c;
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
