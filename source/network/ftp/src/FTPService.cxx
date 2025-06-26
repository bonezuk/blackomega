#include "network/ftp/inc/FTPService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace ftp
{
//-------------------------------------------------------------------------------------------

CONCRETE_QOBJECT_FACTORY_CLASS_IMPL(ServiceFactory,Service, \
                                    FTPServiceFactory,FTPService, \
                                    "ftp_server",false)

//-------------------------------------------------------------------------------------------
// FTPServiceEvent
//-------------------------------------------------------------------------------------------

FTPServiceEvent::FTPServiceEvent(FTPServiceEventType t) : QEvent(static_cast<QEvent::Type>(t)),
    m_port(0),
    m_server(0)
{
    m_threadId = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

Qt::HANDLE FTPServiceEvent::threadId()
{
    return m_threadId;
}

//-------------------------------------------------------------------------------------------

tint FTPServiceEvent::port()
{
    return m_port;
}

//-------------------------------------------------------------------------------------------

void FTPServiceEvent::port(tint x)
{
    m_port = x;
}

//-------------------------------------------------------------------------------------------

FTPServer *FTPServiceEvent::getServer()
{
    return m_server;
}

//-------------------------------------------------------------------------------------------

void FTPServiceEvent::setServer(FTPServer *x)
{
    m_server = x;
}

//-------------------------------------------------------------------------------------------
// FTPService
//-------------------------------------------------------------------------------------------

FTPService::FTPService(QObject *parent) : TCPService(parent)
{}

//-------------------------------------------------------------------------------------------

FTPService::~FTPService()
{}

//-------------------------------------------------------------------------------------------

void FTPService::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "FTPService::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

FTPServer *FTPService::getServer(tint port)
{
    FTPServiceEvent *e = new FTPServiceEvent(FTPServiceEvent::e_getFTPServerEvent);
    ServiceWaitCondition *c = getCondition();
    FTPServer *svr;

    e->port(port);
    QCoreApplication::postEvent(this,e);
    c->wait();
    svr = reinterpret_cast<FTPServer *>(c->result());
    return svr;
}

//-------------------------------------------------------------------------------------------

void FTPService::deleteServer(FTPServer *svr)
{
    FTPServiceEvent *e = new FTPServiceEvent(FTPServiceEvent::e_deleteFTPServerEvent);
    e->setServer(svr);
    QCoreApplication::postEvent(this,e);
}

//-------------------------------------------------------------------------------------------

bool FTPService::event(QEvent *e)
{
    if(e!=0 && static_cast<FTPServiceEvent::FTPServiceEventType>(e->type())>=FTPServiceEvent::e_getFTPServerEvent)
    {
        FTPServiceEvent::FTPServiceEventType t = static_cast<FTPServiceEvent::FTPServiceEventType>(e->type());

        switch(t)
        {
            case FTPServiceEvent::e_getFTPServerEvent:
                {
                    FTPServiceEvent *sEvent = reinterpret_cast<FTPServiceEvent *>(e);
                    FTPServer *s;
                    s = onGetServer(sEvent->port());
                    processCustomEvent(sEvent,reinterpret_cast<void *>(s));
                }
                break;

            case FTPServiceEvent::e_deleteFTPServerEvent:
                {
                    FTPServiceEvent *sEvent = reinterpret_cast<FTPServiceEvent *>(e);
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

void FTPService::processCustomEvent(FTPServiceEvent *e,void *results)
{
    ServiceWaitCondition *c;

    c = getCondition(e->threadId());
    c->setResult(results);
    c->wake();
    e->accept();
}

//-------------------------------------------------------------------------------------------

FTPServer *FTPService::onGetServer(tint port)
{
    QSet<TCPServerSocket *>::iterator ppI;
    FTPServer *svr = 0;

    for(ppI=m_serverSet.begin();ppI!=m_serverSet.end() && svr==0;++ppI)
    {
        TCPServerSocket *sS = *ppI;

        svr = dynamic_cast<FTPServer *>(sS);
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
        fprintf(stdout, "Start FTP server on port %d\n", port);

        svr = new FTPServer(this,this);
        if(svr->open(port))
        {
            m_serverSet.insert(reinterpret_cast<TCPServerSocket *>(svr));
        }
        else
        {
            QString errStr;
            errStr = "Error opening FTP server on port " + QString::number(port);
            printError("onGetServer",errStr.toLatin1().constData());
            delete svr;
            svr = 0;
        }
    }
    return svr;
}

//-------------------------------------------------------------------------------------------

void FTPService::onDeleteServer(FTPServer *svr)
{
    if(svr!=0)
    {
        QSet<TCPServerSocket *>::iterator ppI;

        fprintf(stdout, "Stop FTP server\n");

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
} // namespace ftp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
