#include "network/inc/Controller.h"
#include "network/inc/Service.h"

//#define OMEGA_NETWORK_CONTROLLER_DB 1
#if defined(OMEGA_NETWORK_CONTROLLER_DB)
#define DB(x) x
#else
#define DB(x)
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------
// ControllerEvent
//-------------------------------------------------------------------------------------------

ControllerEvent::ControllerEvent(ControllerEventType t) : QEvent(static_cast<QEvent::Type>(t)),
    m_threadId(QThread::currentThreadId()),
    m_name(),
    m_service(0)
{}

//-------------------------------------------------------------------------------------------

Qt::HANDLE ControllerEvent::threadId()
{
    return m_threadId;
}

//-------------------------------------------------------------------------------------------

const QString& ControllerEvent::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

void ControllerEvent::name(const QString& n)
{
    m_name = n;
}

//-------------------------------------------------------------------------------------------

Service::ServicePtr ControllerEvent::service() const
{
    return m_service;
}

//-------------------------------------------------------------------------------------------

void ControllerEvent::service(Service::ServicePtr s)
{
    m_service = s;
}

//-------------------------------------------------------------------------------------------
// ControllerWaitCondition
//-------------------------------------------------------------------------------------------

ControllerWaitCondition::ControllerWaitCondition() : m_mutex(),
    m_condition(),
    m_service()
{}

//-------------------------------------------------------------------------------------------

void ControllerWaitCondition::get()
{
    m_mutex.lock();
}

//-------------------------------------------------------------------------------------------

void ControllerWaitCondition::wait()
{
    m_condition.wait(&m_mutex);
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void ControllerWaitCondition::wake()
{
    m_mutex.lock();
    m_condition.wakeAll();
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

Service::ServicePtr ControllerWaitCondition::service() const
{
    return m_service;
}

//-------------------------------------------------------------------------------------------

void ControllerWaitCondition::service(Service::ServicePtr s)
{
    m_service = s;
}

//-------------------------------------------------------------------------------------------
// ControllerThread
//-------------------------------------------------------------------------------------------

ControllerThread::ControllerThread(QObject *parent) : QThread(parent),
    m_id(1),
    m_mutex(),
    m_condition(),
    m_controller()
{}

//-------------------------------------------------------------------------------------------

ControllerThread::~ControllerThread()
{}

//-------------------------------------------------------------------------------------------

bool ControllerThread::ignite(tint id)
{
    m_id = id;
    m_mutex.lock();
    start();
    m_condition.wait(&m_mutex);
    m_mutex.unlock();
    return (m_controller.data()!=0) ? true : false;
}

//-------------------------------------------------------------------------------------------

Controller::ControllerSPtr ControllerThread::controller()
{
    return m_controller;
}

//-------------------------------------------------------------------------------------------

void ControllerThread::run()
{
    m_mutex.lock();
    m_mutex.unlock();

    Controller::ControllerSPtr ctrl(new Controller(static_cast<tint>(m_id)));

    m_controller = ctrl;
    m_controller->m_thread = this;
    if(m_controller->start())
    {
        m_condition.wakeAll();
        exec();
        m_controller->stop();
    }
    else
    {
        Controller::ControllerSPtr eCtrl;
        m_controller->stop();
        m_controller = eCtrl;
        m_condition.wakeAll();
    }
}

//-------------------------------------------------------------------------------------------
// Controller
//-------------------------------------------------------------------------------------------

QMutex Controller::m_mutex;
QMap<tint,Controller::ControllerSPtr> Controller::m_controlMap;

//-------------------------------------------------------------------------------------------

Controller::Controller(tint id,QObject *parent) : QObject(parent),
    m_id(id),
    m_thread(0),
    m_processTimer(0),
    m_serviceMap(),
    m_socketSet()
{}

//-------------------------------------------------------------------------------------------

Controller::~Controller()
{}

//-------------------------------------------------------------------------------------------

void Controller::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "Controller::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

Controller::ControllerSPtr Controller::instance(tint id)
{
    QMap<tint,ControllerSPtr>::iterator ppI;
    ControllerSPtr ctrl;

    m_mutex.lock();
    ppI = m_controlMap.find(id);
    if(ppI==m_controlMap.end())
    {
        ControllerThread *cThread = new ControllerThread;
        if(cThread->ignite(id))
        {
            ctrl = cThread->controller();
            m_controlMap.insert(id,ctrl);
        }
        else
        {
            delete cThread;
        }
    }
    else
    {
        ctrl = ppI.value();
    }
    m_mutex.unlock();

    return ctrl;
}

//-------------------------------------------------------------------------------------------

void Controller::end(tint id)
{
    QMap<tint,ControllerSPtr>::iterator ppI;

    m_mutex.lock();
    ppI = m_controlMap.find(id);
    if(ppI!=m_controlMap.end())
    {
        m_mutex.unlock();

        ControllerThread *cThread = ppI.value()->m_thread;

        if(cThread!=0)
        {
            cThread->quit();
            cThread->wait();
            delete cThread;
        }

        m_mutex.lock();
        m_controlMap.erase(ppI);
        m_mutex.unlock();
    }
    else
    {
        m_mutex.unlock();
    }
}

//-------------------------------------------------------------------------------------------

bool Controller::start()
{
    m_processTimer = new QTimer(this);
    QObject::connect(m_processTimer,SIGNAL(timeout()),this,SLOT(onProcess()));
    // Timer set to have regular processing but not to overload network layer.
    m_processTimer->start(50);
    return true;
}

//-------------------------------------------------------------------------------------------

void Controller::stop()
{
    QMap<tint,Service::ServicePtr>::iterator ppI;
    QSet<Socket::SocketPtr>::iterator ppJ;

    while(ppI=m_serviceMap.begin(), ppI!=m_serviceMap.end())
    {
        Service::ServicePtr s = ppI.value();
        m_serviceMap.erase(ppI);
        s->stop();
        delete s;
    }

    while(ppJ=m_socketSet.begin(), ppJ!=m_socketSet.end())
    {
        Socket::SocketPtr sk = *ppJ;
        m_socketSet.erase(ppJ);
        delete sk;
    }

    m_processTimer->stop();
    QObject::disconnect(m_processTimer,SIGNAL(timeout()),this,SLOT(onProcess()));
    delete m_processTimer;
    m_processTimer = 0;

    freeConditions();
}

//-------------------------------------------------------------------------------------------

void Controller::onProcess()
{
    tint nfds = -1;
    fd_set readS,writeS;
    QSet<Socket::SocketPtr> readSet,writeSet;
    QVector<Socket::SocketPtr> rSelect,wSelect;
    QSet<Socket::SocketPtr>::iterator ppI;
    QVector<Socket::SocketPtr>::iterator ppJ;
    QMap<tint,Service::ServicePtr>::iterator ppK;
    bool writeFlag = true;

    FD_ZERO(&readS);
    FD_ZERO(&writeS);

    for(ppI=m_socketSet.begin();ppI!=m_socketSet.end();++ppI)
    {
        Socket::SocketPtr s = *ppI;
        const tuint32& state = s->state();

        if(s->socket()!=c_invalidSocket)
        {
            tint fd = static_cast<tint>(s->socket());

            if(s->canRead())
            {
                if(state & Socket::c_socketStateRead)
                {
                    readSet.insert(s);
                }
                else
                {
                    nfds = (nfds < fd) ? fd : nfds;
                    FD_SET(fd,&readS);
                    rSelect.append(s);
                }
            }

            if(s->canWrite())
            {
                if(state & Socket::c_socketStateWrite)
                {
                    writeSet.insert(s);
                }
                else
                {
                    nfds = (nfds < fd) ? fd : nfds;
                    FD_SET(fd,&writeS);
                    wSelect.append(s);
                }
            }
        }
    }
    nfds++;

    if(nfds > 0)
    {
        tint res;
        timeval t;

        t.tv_sec = 0;
        t.tv_usec = 0;

        res = ::select(nfds,&readS,&writeS,0,&t);
        if(res > 0)
        {
            for(ppJ=rSelect.begin();ppJ!=rSelect.end();++ppJ)
            {
                Socket::SocketPtr s = *ppJ;

                if(FD_ISSET(s->socket(),&readS))
                {
                    s->setForRead();
                    readSet.insert(s);
                }
            }

            for(ppJ=wSelect.begin();ppJ!=wSelect.end();++ppJ)
            {
                Socket::SocketPtr s = *ppJ;

                if(FD_ISSET(s->socket(),&writeS))
                {
                    s->setForWrite();
                    writeSet.insert(s);
                }
            }
        }
    }

    for(ppI=readSet.begin();ppI!=readSet.end();++ppI)
    {
        Socket::SocketPtr s = *ppI;
        if(!s->doRead())
        {
            emit socketError(s);
        }
    }

    for(ppK=m_serviceMap.begin();ppK!=m_serviceMap.end();++ppK)
    {
        if(!(ppK.value()->process()))
        {
            writeFlag = false;
        }
    }

    if(writeFlag)
    {
        for(ppI=writeSet.begin();ppI!=writeSet.end();++ppI)
        {
            Socket::SocketPtr s = *ppI;

            if(m_socketSet.find(s)!=m_socketSet.end())
            {
                if(!s->doWrite())
                {
                    emit socketError(s);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Controller::doRead(Service::ServicePtr s)
{
    tint nfds = -1;
    fd_set readS;
    QSet<Socket::SocketPtr> readSet;
    QVector<Socket::SocketPtr> rSelect;
    QSet<Socket::SocketPtr>::iterator ppI;
    QVector<Socket::SocketPtr>::iterator ppJ;
    QMultiMap<Service::ServicePtr,Socket::SocketPtr>::iterator ppK;

    FD_ZERO(&readS);

    for(ppK=m_socketServiceMap.find(s);ppK!=m_socketServiceMap.end() && ppK.key()==s;++ppK)
    {
        Socket::SocketPtr s = ppK.value();
        const tuint32& state = s->state();

        if(s->socket()!=c_invalidSocket && s->canRead())
        {
            tint fd = static_cast<tint>(s->socket());

            if(state & Socket::c_socketStateRead)
            {
                readSet.insert(s);
            }
            else
            {
                nfds = (nfds < fd) ? fd : nfds;
                FD_SET(fd,&readS);
                rSelect.append(s);
            }
        }
    }
    nfds++;

    if(nfds > 0)
    {
        tint res;
        timeval t;

        t.tv_sec = 0;
        t.tv_usec = 0;

        res = ::select(nfds,&readS,0,0,&t);
        if(res > 0)
        {
            for(ppJ=rSelect.begin();ppJ!=rSelect.end();++ppJ)
            {
                Socket::SocketPtr s = *ppJ;

                if(FD_ISSET(s->socket(),&readS))
                {
                    s->setForRead();
                    readSet.insert(s);
                }
            }
        }
    }

    for(ppI=readSet.begin();ppI!=readSet.end();++ppI)
    {
        Socket::SocketPtr s = *ppI;
        if(!s->doRead())
        {
            emit socketError(s);
        }
    }
}

//-------------------------------------------------------------------------------------------

void Controller::doWrite(Service::ServicePtr s)
{
    tint nfds = -1;
    fd_set writeS;
    QSet<Socket::SocketPtr> writeSet;
    QVector<Socket::SocketPtr> wSelect;
    QSet<Socket::SocketPtr>::iterator ppI;
    QVector<Socket::SocketPtr>::iterator ppJ;
    QMultiMap<Service::ServicePtr,Socket::SocketPtr>::iterator ppK;

    FD_ZERO(&writeS);

    for(ppK=m_socketServiceMap.find(s);ppK!=m_socketServiceMap.end() && ppK.key()==s;++ppK)
    {
        Socket::SocketPtr s = ppK.value();
        const tuint32& state = s->state();

        if(s->socket()!=c_invalidSocket && s->canWrite())
        {
            tint fd = static_cast<tint>(s->socket());

            if(state & Socket::c_socketStateWrite)
            {
                writeSet.insert(s);
            }
            else
            {
                nfds = (nfds < fd) ? fd : nfds;
                FD_SET(fd,&writeS);
                wSelect.append(s);
            }
        }
    }
    nfds++;

    if(nfds > 0)
    {
        tint res;
        timeval t;

        t.tv_sec = 0;
        t.tv_usec = 0;

        res = ::select(nfds,0,&writeS,0,&t);
        if(res > 0)
        {
            for(ppJ=wSelect.begin();ppJ!=wSelect.end();++ppJ)
            {
                Socket::SocketPtr s = *ppJ;

                if(FD_ISSET(s->socket(),&writeS))
                {
                    s->setForWrite();
                    writeSet.insert(s);
                }
            }
        }
    }

    for(ppI=writeSet.begin();ppI!=writeSet.end();++ppI)
    {
        Socket::SocketPtr s = *ppI;
        if(!s->doWrite())
        {
            emit socketError(s);
        }
    }
}

//-------------------------------------------------------------------------------------------

ControllerWaitCondition *Controller::getCondition()
{
    ControllerWaitCondition *c = getCondition(QThread::currentThreadId());
    c->get();
    return c;
}

//-------------------------------------------------------------------------------------------

ControllerWaitCondition *Controller::getCondition(Qt::HANDLE id)
{
    ControllerWaitCondition *c;
    QMap<Qt::HANDLE,ControllerWaitCondition *>::iterator ppI;

    m_mutex.lock();
    ppI = m_waitConditionMap.find(id);
    if(ppI!=m_waitConditionMap.end())
    {
        c = ppI.value();
    }
    else
    {
        c = new ControllerWaitCondition;
        m_waitConditionMap.insert(id,c);
    }
    m_mutex.unlock();
    return c;
}

//-------------------------------------------------------------------------------------------

void Controller::freeConditions()
{
    QMap<Qt::HANDLE,ControllerWaitCondition *>::iterator ppI;

    m_mutex.lock();
    while(ppI = m_waitConditionMap.begin(), ppI!=m_waitConditionMap.end())
    {
        ControllerWaitCondition *c = ppI.value();
        m_waitConditionMap.erase(ppI);
        delete c;
    }
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

Service::ServicePtr Controller::newService(const tchar *name)
{
    return newService(QString::fromLatin1(name));
}

//-------------------------------------------------------------------------------------------

Service::ServicePtr Controller::newService(const QString& name)
{
    ControllerEvent *e = new ControllerEvent(ControllerEvent::e_newService);
    ControllerWaitCondition *c = getCondition();
    Service::ServicePtr svr;

    e->name(name);
    QCoreApplication::postEvent(this,e);
    c->wait();
    svr = c->service();
    return svr;
}

//-------------------------------------------------------------------------------------------

Service::ServicePtr Controller::getService(const tchar *name)
{
    return getService(QString::fromLatin1(name));
}

//-------------------------------------------------------------------------------------------

Service::ServicePtr Controller::getService(const QString& name)
{
    ControllerEvent *e = new ControllerEvent(ControllerEvent::e_getService);
    ControllerWaitCondition *c = getCondition();
    Service::ServicePtr svr;

    e->name(name);
    QCoreApplication::postEvent(this,e);
    c->wait();
    svr = c->service();
    return svr;
}

//-------------------------------------------------------------------------------------------

void Controller::deleteService(Service::ServicePtr s)
{
    ControllerEvent *e = new ControllerEvent(ControllerEvent::e_deleteService);
    ControllerWaitCondition *c = getCondition();

    e->service(s);
    QCoreApplication::postEvent(this,e);
    c->wait();
}

//-------------------------------------------------------------------------------------------

bool Controller::event(QEvent *e)
{
    if(e!=0 && static_cast<ControllerEvent::ControllerEventType>(e->type())>=ControllerEvent::e_newService)
    {
        ControllerEvent::ControllerEventType t = static_cast<ControllerEvent::ControllerEventType>(e->type());
        ControllerWaitCondition *c;

        switch(t)
        {
            case ControllerEvent::e_newService:
                {
                    ControllerEvent *cEvent = reinterpret_cast<ControllerEvent *>(e);
                    Service::ServicePtr s;
                    s = onNewService(cEvent->name());
                    c = getCondition(cEvent->threadId());
                    c->service(s);
                    c->wake();
                    e->accept();
                }
                break;

            case ControllerEvent::e_getService:
                {
                    ControllerEvent *cEvent = reinterpret_cast<ControllerEvent *>(e);
                    Service::ServicePtr s;
                    s = onGetService(cEvent->name());
                    c = getCondition(cEvent->threadId());
                    c->service(s);
                    c->wake();
                    e->accept();
                }
                break;

            case ControllerEvent::e_deleteService:
                {
                    ControllerEvent *cEvent = reinterpret_cast<ControllerEvent *>(e);
                    onDeleteService(cEvent->service());
                    c = getCondition(cEvent->threadId());
                    c->wake();
                    e->accept();
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

Service::ServicePtr Controller::onNewService(const QString& name)
{
    Service::ServicePtr svr = ServiceFactory::createUnmanaged(name,this);
    QMap<tint,Service::ServicePtr>::iterator ppI;
    QString err;

    if(svr!=0)
    {
        if(svr->start())
        {
            svr->m_name = name;
            m_serviceMap.insert(svr->id(),svr);
        }
        else
        {
            err = "Failed to start service '" + name + "'";
            printError("onNewService",err.toLatin1().constData());
            delete svr;
            svr = 0;
        }
    }
    else
    {
        err = "Failed to create service '" + name + "'";
        printError("onNewService",err.toLatin1().constData());
    }
    return svr;
}

//-------------------------------------------------------------------------------------------

Service::ServicePtr Controller::onGetService(const QString& name)
{
    Service::ServicePtr svr = 0;
    QMap<tint,Service::ServicePtr>::iterator ppI;

    for(ppI=m_serviceMap.begin();svr==0 && ppI!=m_serviceMap.end();++ppI)
    {
        if(ppI.value()->name()==name)
        {
            svr = ppI.value();
        }
    }
    if(svr==0)
    {
        svr = onNewService(name);
    }
    return svr;
}

//-------------------------------------------------------------------------------------------

void Controller::onDeleteService(Service::ServicePtr s)
{
    if(s!=0)
    {
        QMap<tint,Service::ServicePtr>::iterator ppI = m_serviceMap.find(s->id());

        s->stop();

        if(ppI!=m_serviceMap.end())
        {
            m_serviceMap.erase(ppI);
        }
        delete s;
    }
}

//-------------------------------------------------------------------------------------------

void Controller::addSocket(Service::ServicePtr svr,Socket::SocketPtr s)
{
    if(s!=0)
    {
        m_socketSet.insert(s);
        m_socketServiceMap.insert(svr,s);
    }
}

//-------------------------------------------------------------------------------------------

void Controller::delSocket(Service::ServicePtr svr,Socket::SocketPtr s)
{
    if(s!=0)
    {
        QSet<Socket::SocketPtr>::iterator ppI = m_socketSet.find(s);

        if(ppI!=m_socketSet.end())
        {
            m_socketSet.erase(ppI);
        }

        QMultiMap<Service::ServicePtr,Socket::SocketPtr>::iterator ppJ = m_socketServiceMap.find(svr);

        while(ppJ!=m_socketServiceMap.end() && ppJ.key()==svr)
        {
            if(ppJ.value()==s)
            {
                m_socketServiceMap.erase(ppJ);
                break;
            }
            ++ppJ;
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
