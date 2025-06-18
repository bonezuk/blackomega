#include "network/rtp/inc/Scheduler.h"
#include "network/rtp/inc/Session.h"

#include <QCoreApplication>

//#define OMEGA_NETWORK_RTP_SCHEDULER_DB
#if defined(OMEGA_NETWORK_RTP_SCHEDULER_DB)
#define DB(x) x
#else
#define DB(x)
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace rtp
{
//-------------------------------------------------------------------------------------------

CONCRETE_QOBJECT_FACTORY_CLASS_IMPL(ServiceFactory,Service, \
                                    SchedulerFactory,Scheduler, \
                                    "rtp",false)

//-------------------------------------------------------------------------------------------
// SchedulerEvent
//-------------------------------------------------------------------------------------------

SchedulerEvent::SchedulerEvent(SchedulerEventType t) : QEvent(static_cast<QEvent::Type>(t)),
    m_resource(),
    m_host(),
    m_port(0),
    m_rHost(),
    m_rPort(0),
    m_session(0)
{
    m_threadId = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

Qt::HANDLE SchedulerEvent::threadId()
{
    return m_threadId;
}

//-------------------------------------------------------------------------------------------

const QString& SchedulerEvent::resource() const
{
    return m_resource;
}

//-------------------------------------------------------------------------------------------

void SchedulerEvent::resource(const QString& x)
{
    m_resource = x;
}

//-------------------------------------------------------------------------------------------

const QString& SchedulerEvent::host() const
{
    return m_host;
}

//-------------------------------------------------------------------------------------------

void SchedulerEvent::host(const QString& x)
{
    m_host = x;
}

//-------------------------------------------------------------------------------------------

tint SchedulerEvent::port() const
{
    return m_port;
}

//-------------------------------------------------------------------------------------------

void SchedulerEvent::port(tint x)
{
    m_port = x;
}

//-------------------------------------------------------------------------------------------

const QString& SchedulerEvent::rHost() const
{
    return m_rHost;
}

//-------------------------------------------------------------------------------------------

void SchedulerEvent::rHost(const QString& x)
{
    m_rHost = x;
}

//-------------------------------------------------------------------------------------------

tint SchedulerEvent::rPort() const
{
    return m_rPort;
}

//-------------------------------------------------------------------------------------------

void SchedulerEvent::rPort(tint x)
{
    m_rPort = x;
}

//-------------------------------------------------------------------------------------------

Session *SchedulerEvent::session() const
{
    return m_session;
}

//-------------------------------------------------------------------------------------------

void SchedulerEvent::session(Session *x)
{
    m_session = x;
}

//-------------------------------------------------------------------------------------------
// Scheduler
//-------------------------------------------------------------------------------------------

SDESBlock Scheduler::m_sdesInfo;

//-------------------------------------------------------------------------------------------

Scheduler::Scheduler(QObject *parent) : Service(parent),
    m_timeoutCount(0),
    m_sessionSet()
{}

//-------------------------------------------------------------------------------------------

Scheduler::~Scheduler()
{}

//-------------------------------------------------------------------------------------------

void Scheduler::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Scheduler::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

const SDESBlock& Scheduler::idInfo()
{
    return m_sdesInfo;
}

//-------------------------------------------------------------------------------------------

void Scheduler::setidInfo(const SDESBlock& info)
{
    m_sdesInfo = info;
}

//-------------------------------------------------------------------------------------------

bool Scheduler::event(QEvent *e)
{
    if(e!=0 && static_cast<SchedulerEvent::SchedulerEventType>(e->type())>=SchedulerEvent::e_newSenderEvent)
    {
        SchedulerEvent::SchedulerEventType t = static_cast<SchedulerEvent::SchedulerEventType>(e->type());
    
        switch(t)
        {
            case SchedulerEvent::e_newSenderEvent:
                {
                    SchedulerEvent *sEvent = reinterpret_cast<SchedulerEvent *>(e);
                    Session *s;
                    s = onNewSender(sEvent->resource(),sEvent->host(),sEvent->port(),sEvent->rHost(),sEvent->rPort());
                    if(s!=0)
                    {
                        m_sessionSet.insert(s);
                    }
                    processCustomEvent(sEvent,reinterpret_cast<void *>(s));
                }
                break;
            
            case SchedulerEvent::e_newReceiverEvent:
                {
                    SchedulerEvent *sEvent = reinterpret_cast<SchedulerEvent *>(e);
                    Session *s;
                    s = onNewReceiver(sEvent->resource(),sEvent->host(),sEvent->port(),sEvent->rHost(),sEvent->rPort());
                    if(s!=0)
                    {
                        m_sessionSet.insert(s);
                    }
                    processCustomEvent(sEvent,reinterpret_cast<void *>(s));
                }
                break;
            
            case SchedulerEvent::e_completeEvent:
                {
                    SchedulerEvent *sEvent = reinterpret_cast<SchedulerEvent *>(e);
                    Session *s = sEvent->session();
                    
                    if(s!=0)
                    {
                        onComplete(s);
                    }
                    processCustomEvent(sEvent,0);
                }
                break;
                
            case SchedulerEvent::e_resyncEvent:
                {
                    SchedulerEvent *sEvent = reinterpret_cast<SchedulerEvent *>(e);
                    Session *s = sEvent->session();
                    
                    if(s!=0)
                    {
                        onResync(s);
                    }
                }
                break;

            default:
                return QObject::event(e);
                break;
        }
        return true;
    }
    else
    {
        return QObject::event(e);
    }
}

//-------------------------------------------------------------------------------------------

void Scheduler::processCustomEvent(SchedulerEvent *e,void *result)
{
    ServiceWaitCondition *c;
    
    c = getCondition(e->threadId());
    c->setResult(result);
    c->wake();
    e->accept();
}

//-------------------------------------------------------------------------------------------

Session *Scheduler::onNewSender(const QString& resource,const QString& host,tint port,const QString& rHost,tint rPort)
{
    Session *s = 0;
    QString ext = getFileExtension(resource);
    QString err;
    
    s = SessionFactory::createUnmanaged(ext,this);
    if(s!=0)
    {
        bool res = false;
        
        if(s->start(this,resource,host,port,rHost,rPort))
        {
            res = true;
        }
        else
        {
            err  = QString::fromLatin1("Error starting RTP sender session for host '") + host;
            err += QString::fromLatin1("' on port ") + QString::number(port);
            err += QString::fromLatin1(" with resource '") + resource + QString::fromLatin1("'");
            printError("onNewSender",err.toLatin1().constData());            
        }
        
        if(!res)
        {
            delete s;
            s = 0;
        }
    }
    else
    {
        err = QString::fromLatin1("Failed to find RTP sender for '") + resource + QString::fromLatin1("'");
        printError("onNewSender",err.toLatin1().constData());
    }
    return s;
}

//-------------------------------------------------------------------------------------------

Session *Scheduler::onNewReceiver(const QString& resource,const QString& host,tint port,const QString& rHost,tint rPort)
{
    Session *s = 0;
    QString ext = getFileExtension(resource);
    QString err;
    
    s = SessionFactory::createUnmanaged(ext,this);
    if(s!=0)
    {
        bool res = false;
        
        if(s->start(this,host,port,rHost,rPort))
        {
            res = true;
        }
        else
        {
            err  = QString::fromLatin1("Error starting RTP receiver session for host '") + host;
            err += QString::fromLatin1("' on port ") + QString::number(port);
            err += QString::fromLatin1(" with resource '") + resource + QString::fromLatin1("'");
            printError("onNewSender",err.toLatin1().constData());            
        }
        
        if(!res)
        {
            delete s;
            s = 0;
        }
    }
    else
    {
        err = QString::fromLatin1("Failed to find RTP receiver for '") + resource + QString::fromLatin1("'");
        printError("onNewSender",err.toLatin1().constData());
    }
    return s;
}

//-------------------------------------------------------------------------------------------

void Scheduler::onComplete(Session *session)
{
    if(session!=0)
    {
        QSet<Session *>::iterator ppI;
        
        session->stop();
        
        ppI = m_sessionSet.find(session);
        if(ppI!=m_sessionSet.end())
        {
            m_sessionSet.erase(ppI);
        }
        
        delete session;
    }
}

//-------------------------------------------------------------------------------------------

void Scheduler::onResync(Session *session)
{
    if(session!=0)
    {
        session->resync();
    }
}

//-------------------------------------------------------------------------------------------

QString Scheduler::getFileExtension(const QString& name)
{
    tint i;
    QString ext;
    
    for(i=name.length()-2;i>=0 && ext.isEmpty();--i)
    {
        if(name.at(i)==QChar('.'))
        {
            ext = name.mid(i + 1).toLower();
        }
    }
    if(ext.isEmpty())
    {
        ext = name.toLower();
    }
    return ext;
}

//-------------------------------------------------------------------------------------------

Session *Scheduler::newSender(const QString& resource,const QString& host,tint port,const QString& rHost,tint rPort)
{
    SchedulerEvent *e = new SchedulerEvent(SchedulerEvent::e_newSenderEvent);
    ServiceWaitCondition *c = getCondition();
    Session *s;
    
    e->resource(resource);
    e->host(host);
    e->port(port);
    e->rHost(rHost);
    e->rPort(rPort);
    QCoreApplication::postEvent(this,e);
    c->wait();
    s = reinterpret_cast<Session *>(c->result());
    return s;
}

//-------------------------------------------------------------------------------------------

Session *Scheduler::newReceiver(const QString& resource,const QString& host,tint port,const QString& rHost,tint rPort)
{
    SchedulerEvent *e = new SchedulerEvent(SchedulerEvent::e_newReceiverEvent);
    ServiceWaitCondition *c = getCondition();
    Session *s;
    
    e->resource(resource);
    e->host(host);
    e->port(port);
    e->rHost(rHost);
    e->rPort(rPort);
    QCoreApplication::postEvent(this,e);
    c->wait();
    s = reinterpret_cast<Session *>(c->result());
    return s;
}

//-------------------------------------------------------------------------------------------

void Scheduler::complete(Session *session)
{
    SchedulerEvent *e = new SchedulerEvent(SchedulerEvent::e_completeEvent);
    ServiceWaitCondition *c = getCondition();
    
    e->session(session);
    QCoreApplication::postEvent(this,e);
    c->wait();
}

//-------------------------------------------------------------------------------------------

void Scheduler::resync(Session *session)
{
    SchedulerEvent *e = new SchedulerEvent(SchedulerEvent::e_resyncEvent);
    e->session(session);
    QCoreApplication::postEvent(this,e);
}

//-------------------------------------------------------------------------------------------

bool Scheduler::start()
{
    m_timeoutCount = 0;
    return true;
}

//-------------------------------------------------------------------------------------------

void Scheduler::stop()
{
    QSet<Session *>::iterator ppI = m_sessionSet.begin();
    
    while(ppI!=m_sessionSet.end())
    {
        Session *s = *ppI;
        ppI = m_sessionSet.erase(ppI);
        s->stop();
        delete s;
    }
}

//-------------------------------------------------------------------------------------------

bool Scheduler::process()
{
    if(m_sessionSet.size() > 0)
    {
        m_timeoutCount++;
        if(m_timeoutCount > 85) // check ~0.5s
        {
            // timeout has been checked - uncomment when ready
            /*
            QSet<Session *>::iterator ppI;
            
            for(ppI=m_sessionSet.begin();ppI!=m_sessionSet.end();++ppI)
            {
                Session *s = *ppI;
                s->onTimeout();
            }
            */

            m_timeoutCount = 0;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace rtp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
