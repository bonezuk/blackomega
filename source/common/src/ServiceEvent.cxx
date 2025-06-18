#include "common/inc/ServiceEvent.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------
// ServiceEvent
//-------------------------------------------------------------------------------------------

ServiceEvent::ServiceEvent(QEvent::Type type,bool useWait) : QEvent(type),
    m_waitCondition(useWait)
{
    m_threadId = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

ServiceEvent::~ServiceEvent()
{}

//-------------------------------------------------------------------------------------------

bool ServiceEvent::isWaitCondition() const
{
    return m_waitCondition;
}

//-------------------------------------------------------------------------------------------

Qt::HANDLE ServiceEvent::threadId()
{
    return m_threadId;
}

//-------------------------------------------------------------------------------------------
// ServiceWaitCondition
//-------------------------------------------------------------------------------------------

ServiceWaitCondition::ServiceWaitCondition() : m_mutex(),
    m_condition()
{}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition::~ServiceWaitCondition()
{}

//-------------------------------------------------------------------------------------------

void ServiceWaitCondition::get()
{
    m_mutex.lock();
}

//-------------------------------------------------------------------------------------------

void ServiceWaitCondition::wait()
{
    m_condition.wait(&m_mutex);
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void ServiceWaitCondition::wake()
{
    m_mutex.lock();
    m_condition.wakeAll();
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------
// ServiceEventAndCondition
//-------------------------------------------------------------------------------------------

ServiceEventAndCondition::ServiceEventAndCondition(QObject *parent) : QObject(parent),
    m_mutex(),
    m_waitConditionMap(),
    m_thread(0)
{}

//-------------------------------------------------------------------------------------------

ServiceEventAndCondition::~ServiceEventAndCondition()
{}

//-------------------------------------------------------------------------------------------

void ServiceEventAndCondition::lock()
{
    m_mutex.lock();
}

//-------------------------------------------------------------------------------------------

void ServiceEventAndCondition::unlock()
{
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void ServiceEventAndCondition::setThread(QThread *pThread)
{
    m_thread = pThread;
}

//-------------------------------------------------------------------------------------------

QThread *ServiceEventAndCondition::getThread()
{
    return m_thread;
}

//-------------------------------------------------------------------------------------------

QMap<Qt::HANDLE,ServiceWaitCondition *>& ServiceEventAndCondition::waitConditionMap()
{
    return m_waitConditionMap;
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *ServiceEventAndCondition::getCondition()
{
    ServiceWaitCondition *cond = getCondition(QThread::currentThreadId());
    cond->get();
    return cond;
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *ServiceEventAndCondition::getCondition(ServiceEvent *e)
{
    return getCondition(e->threadId());
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *ServiceEventAndCondition::getCondition(Qt::HANDLE threadId)
{
    ServiceWaitCondition *c;
    QMap<Qt::HANDLE,ServiceWaitCondition *>::iterator ppI;
    
    lock();
    ppI = waitConditionMap().find(threadId);
    if(ppI!=waitConditionMap().end())
    {
        c = *ppI;
    }
    else
    {
        c = newCondition();
        waitConditionMap().insert(threadId,c);
    }
    unlock();
    return c;
}

//-------------------------------------------------------------------------------------------

void ServiceEventAndCondition::freeConditions()
{
    QMap<Qt::HANDLE,ServiceWaitCondition *>::iterator ppI;
    
    lock();
    while(ppI = waitConditionMap().begin(),ppI!=waitConditionMap().end())
    {
        delete (*ppI);
        waitConditionMap().erase(ppI);
    }
    unlock();
}

//-------------------------------------------------------------------------------------------

void ServiceEventAndCondition::wake(ServiceEvent *evt)
{
    getCondition(evt)->wake();
}

//-------------------------------------------------------------------------------------------

bool ServiceEventAndCondition::event(QEvent *evt)
{
    bool res = false;
    ServiceEvent *pSEvent = dynamic_cast<ServiceEvent *>(evt);

    if(pSEvent!=0)
    {
        res = processEvent(pSEvent);
        if(res)
        {
            pSEvent->accept();
            if(pSEvent->isWaitCondition())
            {
                wake(pSEvent);
            }
        }
    }
    if(!res)
    {
        res = eventSuper(evt);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool ServiceEventAndCondition::eventSuper(QEvent *evt)
{
    return QObject::event(evt);
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
