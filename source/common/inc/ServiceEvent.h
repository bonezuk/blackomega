//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_SERVICEEVENT_H
#define __OMEGA_COMMON_SERVICEEVENT_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BString.h"

#include <QEvent>
#include <QtGlobal>
#include <QRecursiveMutex>
#include <QWaitCondition>
#include <QThread>
#include <QMap>
#include <QSharedPointer>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

class COMMON_EXPORT ServiceEvent : public QEvent
{
    public:
        ServiceEvent(QEvent::Type type,bool useWait);
        virtual ~ServiceEvent();
        
        virtual bool isWaitCondition() const;
        
        virtual Qt::HANDLE threadId();
        
    protected:
    
        Qt::HANDLE m_threadId;
        bool m_waitCondition;
};

//-------------------------------------------------------------------------------------------

template <typename X> class ServiceDataEvent : public ServiceEvent
{
    public:
        ServiceDataEvent(QEvent::Type type,bool useWait);
        virtual ~ServiceDataEvent();

        virtual X& data();
        virtual const X& dataConst() const;

    protected:
    
        X m_data;
};

//-------------------------------------------------------------------------------------------

template <typename X> ServiceDataEvent<X>::ServiceDataEvent(QEvent::Type type,bool useWait) : ServiceEvent(type,useWait),
    m_data()
{}

//-------------------------------------------------------------------------------------------

template <typename X> ServiceDataEvent<X>::~ServiceDataEvent()
{}

//-------------------------------------------------------------------------------------------

template <typename X> X& ServiceDataEvent<X>::data()
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

template <typename X> const X& ServiceDataEvent<X>::dataConst() const
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT ServiceWaitCondition
{
    public:
        ServiceWaitCondition();
        virtual ~ServiceWaitCondition();
        
        virtual void get();
        virtual void wait();
        virtual void wake();
        
    protected:

        QMutex m_mutex;
        QWaitCondition m_condition;
};

//-------------------------------------------------------------------------------------------

template <typename X> class ServiceDataWaitCondition : public ServiceWaitCondition
{
    public:
        ServiceDataWaitCondition();
        virtual ~ServiceDataWaitCondition();
        
        virtual X& data();
        virtual const X& dataConst() const;
        
    protected:
        
        X m_data;
};

//-------------------------------------------------------------------------------------------

template <typename X> ServiceDataWaitCondition<X>::ServiceDataWaitCondition() : ServiceWaitCondition()
{}

//-------------------------------------------------------------------------------------------

template <typename X> ServiceDataWaitCondition<X>::~ServiceDataWaitCondition()
{}

//-------------------------------------------------------------------------------------------

template <typename X> X& ServiceDataWaitCondition<X>::data()
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

template <typename X> const X& ServiceDataWaitCondition<X>::dataConst() const
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT ServiceEventAndCondition : public QObject
{
    public:
        Q_OBJECT

    public:
        ServiceEventAndCondition(QObject *parent = 0);
        virtual ~ServiceEventAndCondition();
        
    protected:
        
        QRecursiveMutex m_mutex;
        QMap<Qt::HANDLE,ServiceWaitCondition *> m_waitConditionMap;
        
        QThread *m_thread;
        
        virtual void lock();
        virtual void unlock();
        
        virtual QMap<Qt::HANDLE,ServiceWaitCondition *>& waitConditionMap();
        
        virtual void setThread(QThread *pThread);
        virtual QThread *getThread();
        
        virtual ServiceWaitCondition *newCondition() = 0;
        
        virtual ServiceWaitCondition *getCondition();
        virtual ServiceWaitCondition *getCondition(ServiceEvent *e);
        virtual ServiceWaitCondition *getCondition(Qt::HANDLE threadId);
        virtual void freeConditions();
        
        virtual void wake(ServiceEvent *evt);
        
        virtual bool event(QEvent *evt);
        virtual bool eventSuper(QEvent *evt);
        
        virtual bool processEvent(ServiceEvent *evt) = 0;
};

//-------------------------------------------------------------------------------------------

template <typename X> class ServiceEventThread : public QThread
{
    public:
        ServiceEventThread(QObject *parent = 0);
        virtual ~ServiceEventThread();
        
        virtual bool ignite();
        
        virtual QSharedPointer<X> service();
        
    protected:
    
        QMutex m_mutex;
        QWaitCondition m_condition;
        QSharedPointer<X> m_service;
        
        virtual bool startService();
        virtual void stopService();
        
        virtual void run();
};

//-------------------------------------------------------------------------------------------

template <typename X> ServiceEventThread<X>::ServiceEventThread(QObject *parent) : QThread(parent),
    m_mutex(),
    m_condition(),
    m_service()
{}

//-------------------------------------------------------------------------------------------

template <typename X> ServiceEventThread<X>::~ServiceEventThread()
{}

//-------------------------------------------------------------------------------------------

template <typename X> bool ServiceEventThread<X>::ignite()
{
    m_mutex.lock();
    start();
    m_condition.wait(&m_mutex);
    m_mutex.unlock();
    return (!m_service.isNull()) ? true : false;
}

//-------------------------------------------------------------------------------------------

template <typename X> void ServiceEventThread<X>::run()
{
    bool startFlag;

    startFlag = startService();
    QThread::yieldCurrentThread();
    m_condition.wakeAll();
    if(startFlag)
    {
        exec();
        stopService();
    }
}

//-------------------------------------------------------------------------------------------

template <typename X> QSharedPointer<X> ServiceEventThread<X>::service()
{
    return m_service;
}

//-------------------------------------------------------------------------------------------

template <typename X> bool ServiceEventThread<X>::startService()
{
    QSharedPointer<X> pService(new X);
    m_service = pService;
    return true;
}

//-------------------------------------------------------------------------------------------

template <typename X> void ServiceEventThread<X>::stopService()
{
    QSharedPointer<X> pEmpty(new X);
    m_service = pEmpty;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
