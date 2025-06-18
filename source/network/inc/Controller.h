//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_CONTROLLER_H
#define __OMEGA_NETWORK_CONTROLLER_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Socket.h"
#include <QMultiMap>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class ControllerThread;

//-------------------------------------------------------------------------------------------

class ControllerEvent : public QEvent
{
    public:
    
        typedef enum
        {
            e_newService = QEvent::User + 1000,
            e_getService,
            e_deleteService
        } ControllerEventType;
        
    public:
        ControllerEvent(ControllerEventType t);
        
        Qt::HANDLE threadId();
        
        const QString& name() const;
        void name(const QString& n);
        
        Service::ServicePtr service() const;
        void service(Service::ServicePtr s);
        
    protected:
        
        Qt::HANDLE m_threadId;
        QString m_name;
        Service::ServicePtr m_service;
};

//-------------------------------------------------------------------------------------------

class ControllerWaitCondition
{
    public:
        ControllerWaitCondition();
        
        void get();
        void wait();
        void wake();
        
        Service::ServicePtr service() const;
        void service(Service::ServicePtr s);        
        
    protected:
    
        QMutex m_mutex;
        QWaitCondition m_condition;
        Service::ServicePtr m_service;
};

//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT Controller : public QObject
{
    public:
        Q_OBJECT
        
    public:
        friend class ControllerThread;
        friend class Socket;
        friend class Service;
        
        typedef Controller* ControllerPtr;
        typedef QSharedPointer<Controller> ControllerSPtr;
        
    public:
        Controller(tint id,QObject *parent = 0);
        virtual ~Controller();
        
        static ControllerSPtr instance(tint id = 1);
        static void end(tint id = 1);
        
        tint id() const;
        
        Service::ServicePtr newService(const tchar *name);
        Service::ServicePtr newService(const QString& name);
        
        Service::ServicePtr getService(const tchar *name);
        Service::ServicePtr getService(const QString& name);
        
        void deleteService(Service::ServicePtr s);
        
    protected:
        
        static QMutex m_mutex;
        static QMap<tint,ControllerSPtr> m_controlMap;
        
        tint m_id;
        ControllerThread *m_thread;
        QTimer *m_processTimer;
        QMap<tint,Service::ServicePtr> m_serviceMap;
        QSet<Socket::SocketPtr> m_socketSet;
        QMultiMap<Service::ServicePtr,Socket::SocketPtr> m_socketServiceMap;
        QMap<Qt::HANDLE,ControllerWaitCondition *> m_waitConditionMap;
        
        void printError(const tchar *strR,const tchar *strE) const;
        
        bool start();
        void stop();
        
        virtual bool event(QEvent *e);
        
        ControllerWaitCondition *getCondition();
        ControllerWaitCondition *getCondition(Qt::HANDLE id);
        void freeConditions();
        
        Service::ServicePtr onNewService(const QString& name);
        Service::ServicePtr onGetService(const QString& name);
        void onDeleteService(Service::ServicePtr s);
        
        void addSocket(Service::ServicePtr svr,Socket::SocketPtr s);
        void delSocket(Service::ServicePtr svr,Socket::SocketPtr s);
        
        void doRead(Service::ServicePtr s);
        void doWrite(Service::ServicePtr s);
        
    protected slots:
    
        void onProcess();

    signals:

        void socketError(network::Socket::SocketPtr s);
};

//-------------------------------------------------------------------------------------------

class ControllerThread : public QThread
{
    public:
        Q_OBJECT
        
        friend class Controller;
        
    public:
        ControllerThread(QObject *parent = 0);
        virtual ~ControllerThread();
        
        virtual bool ignite(tint id);
        
        Controller::ControllerSPtr controller();
        
    protected:
    
        volatile tint m_id;
        QMutex m_mutex;
        QWaitCondition m_condition;
        
        Controller::ControllerSPtr m_controller;
        
        virtual void run();
};

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

