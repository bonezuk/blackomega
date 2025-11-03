#include "remote/inc/WinLIRCService.h"
#include "remote/inc/WinLIRCClient.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

CONCRETE_QOBJECT_FACTORY_CLASS_IMPL(network::ServiceFactory,network::Service, \
                                    WinLIRCServiceFactory,WinLIRCService, \
                                    "winlirc",false)

//-------------------------------------------------------------------------------------------
// WinLIRCServiceEvent
//-------------------------------------------------------------------------------------------

WinLIRCServiceEvent::WinLIRCServiceEvent(WinLIRCServiceEventType t) : QEvent(static_cast<QEvent::Type>(t))
{
    m_threadId = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

Qt::HANDLE WinLIRCServiceEvent::threadId()
{
    return m_threadId;
}

//-------------------------------------------------------------------------------------------
// WinLIRCService
//-------------------------------------------------------------------------------------------

WinLIRCService::WinLIRCService(QObject *parent) : TCPClientService(parent)
{
    m_serviceThreadId = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

WinLIRCService::~WinLIRCService()
{}

//-------------------------------------------------------------------------------------------

void WinLIRCService::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "WinLIRCService::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

WinLIRCClient *WinLIRCService::getClient()
{
    WinLIRCClient *client;
    
    if(m_serviceThreadId!=QThread::currentThreadId())
    {
        WinLIRCServiceEvent *e = new WinLIRCServiceEvent(WinLIRCServiceEvent::e_newWinLIRCClientEvent);
        network::ServiceWaitCondition *c = getCondition();
        QCoreApplication::postEvent(this,e);
        c->wait();
        client = reinterpret_cast<WinLIRCClient *>(c->result());
    }
    else
    {
        client = onGetClient();
    }
    return client;
}

//-------------------------------------------------------------------------------------------

bool WinLIRCService::event(QEvent *e)
{
    bool res;
    
    if(e!=0 && static_cast<WinLIRCServiceEvent::WinLIRCServiceEventType>(e->type())>=WinLIRCServiceEvent::e_newWinLIRCClientEvent)
    {
        WinLIRCServiceEvent::WinLIRCServiceEventType t = static_cast<WinLIRCServiceEvent::WinLIRCServiceEventType>(e->type());
        
        switch(t)
        {
            case WinLIRCServiceEvent::e_newWinLIRCClientEvent:
                {
                    WinLIRCClient *client = onGetClient();
                    processCustomEvent(reinterpret_cast<WinLIRCServiceEvent *>(e),reinterpret_cast<void *>(client));
                }
                res = true;
                break;
                
            default:
                res = QObject::event(e);
                break;
        }
    }
    else
    {
        res = QObject::event(e);
    }
    return res;
    return res;
}

//-------------------------------------------------------------------------------------------

WinLIRCClient *WinLIRCService::onGetClient()
{
    return new WinLIRCClient(this,this);
}

//-------------------------------------------------------------------------------------------

void WinLIRCService::processCustomEvent(WinLIRCServiceEvent *e,void *result)
{
    network::ServiceWaitCondition *c = getCondition(e->threadId());
    c->setResult(result);
    c->wake();
    e->accept();
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
