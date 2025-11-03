#include "network/inc/Service.h"
#include "network/inc/Controller.h"
#include "network/inc/TCPConnectionSocket.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS_IMPL(ServiceFactory,Service)

//-------------------------------------------------------------------------------------------
// ServiceWaitCondition
//-------------------------------------------------------------------------------------------

ServiceWaitCondition::ServiceWaitCondition() : m_mutex(),
    m_condition(),
    m_result(0)
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

void *ServiceWaitCondition::result()
{
    return m_result;
}

//-------------------------------------------------------------------------------------------

void ServiceWaitCondition::setResult(void *p)
{
    m_result = p;
}

//-------------------------------------------------------------------------------------------
// Service
//-------------------------------------------------------------------------------------------

tint Service::m_newID = 0;

//-------------------------------------------------------------------------------------------

Service::Service(QObject *parent) : QObject(parent),
    m_controller(0),
    m_id(0),
    m_name(),
    m_mutex(),
    m_waitConditionMap()
{
    m_newID++;
    m_controller = reinterpret_cast<Controller *>(parent);
    m_id = m_newID;
}

//-------------------------------------------------------------------------------------------

Service::~Service()
{
    try
    {
        Service::freeConditions();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void Service::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Service::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *Service::getCondition()
{
    Qt::HANDLE id = QThread::currentThreadId();
    ServiceWaitCondition *c = getCondition(id);
    c->get();
    return c;
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *Service::getCondition(Qt::HANDLE id)
{
    ServiceWaitCondition *c;
    QMap<Qt::HANDLE,ServiceWaitCondition *>::iterator ppI;

    m_mutex.lock();
    ppI = m_waitConditionMap.find(id);
    if(ppI!=m_waitConditionMap.end())
    {
        c = *ppI;
    }
    else
    {
        c = new ServiceWaitCondition;
        m_waitConditionMap.insert(id,c);
    }
    m_mutex.unlock();
    return c;
}

//-------------------------------------------------------------------------------------------

void Service::freeConditions()
{
    QMap<Qt::HANDLE,ServiceWaitCondition *>::iterator ppI;

    m_mutex.lock();
    while(ppI = m_waitConditionMap.begin(),ppI!=m_waitConditionMap.end())
    {
        delete (*ppI);
        m_waitConditionMap.erase(ppI);
    }
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

Controller *Service::controller() const
{
    return m_controller;
}

//-------------------------------------------------------------------------------------------

tint Service::id() const
{
    return m_id;
}

//-------------------------------------------------------------------------------------------

const QString& Service::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

bool Service::start()
{
    return true;
}

//-------------------------------------------------------------------------------------------

void Service::stop()
{}

//-------------------------------------------------------------------------------------------

void Service::doWrite()
{
    m_controller->doWrite(this);
}

//-------------------------------------------------------------------------------------------

void Service::doRead()
{
    m_controller->doRead(this);
}

//-------------------------------------------------------------------------------------------

void Service::addConnection(TCPConnectionSocket *s)
{}

//-------------------------------------------------------------------------------------------

void Service::delConnection(TCPConnectionSocket *s)
{}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
