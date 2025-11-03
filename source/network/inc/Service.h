//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_SERVICE_H
#define __OMEGA_NETWORK_SERVICE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "common/inc/Factory.h"

#include <QMap>
#include <QSet>
#include <QThread>
#include <QEvent>
#include <QMutex>
#include <QTimer>
#include <QWaitCondition>
#include <QCoreApplication>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
//-------------------------------------------------------------------------------------------

class Controller;
class TCPConnectionSocket;

//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT ServiceWaitCondition
{
    public:
        ServiceWaitCondition();

        void get();
        void wait();
        void wake();

        void *result();
        void setResult(void *p);

    protected:

        QMutex m_mutex;
        QWaitCondition m_condition;
        void *m_result;
};

//-------------------------------------------------------------------------------------------

class NETWORK_EXPORT Service : public QObject
{
    public:
        Q_OBJECT

    public:
        friend class Controller;

    public:
        typedef Service* ServicePtr;
        typedef QSharedPointer<ServicePtr> ServiceSPtr;

    public:
        Service(QObject *parent = 0);
        virtual ~Service();

        Controller *controller() const;
        tint id() const;
        const QString& name() const;

        virtual bool start();
        virtual void stop();

        virtual void doWrite();
        virtual void doRead();

        virtual void addConnection(TCPConnectionSocket *s);
        virtual void delConnection(TCPConnectionSocket *s);

        virtual bool process() = 0;

    protected:

        static tint m_newID;

        Controller *m_controller;
        tint m_id;
        QString m_name;

        QMutex m_mutex;
        QMap<Qt::HANDLE,ServiceWaitCondition *> m_waitConditionMap;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual ServiceWaitCondition *getCondition();
        virtual ServiceWaitCondition *getCondition(Qt::HANDLE id);
        virtual void freeConditions();
};

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(NETWORK_EXPORT,ServiceFactory,Service)

//-------------------------------------------------------------------------------------------
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

