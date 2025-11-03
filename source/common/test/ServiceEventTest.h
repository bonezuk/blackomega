//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_SERVICEEVENTTEST_H
#define __OMEGA_COMMON_SERVICEEVENTTEST_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/ServiceEvent.h"

#include <QCoreApplication>
#include <QTimer>
#include <QSharedPointer>

using namespace omega::common;
using namespace testing;

//-------------------------------------------------------------------------------------------

class ServiceDataTester
{
    public:
        ServiceDataTester();
        ServiceDataTester(const ServiceDataTester& rhs);

        const ServiceDataTester& operator = (const ServiceDataTester& rhs);

        int& id();
        const int& idConst() const;
    protected:

        int m_id;
};

//-------------------------------------------------------------------------------------------

class ServiceWaitConditionThreadTester : public QThread
{
    public:
        ServiceWaitConditionThreadTester(ServiceWaitCondition *cond);
        virtual ~ServiceWaitConditionThreadTester();

        ServiceWaitCondition *condition();

    protected:
        ServiceWaitCondition *m_conditionA;
        ServiceWaitCondition *m_conditionB;

        virtual void run();
};

//-------------------------------------------------------------------------------------------

class ServiceEventAndConditionTester;

typedef ServiceEventThread<ServiceEventAndConditionTester> ServiceEventAndConditionTesterThread;

//-------------------------------------------------------------------------------------------

class ServiceEventAndConditionTester : public ServiceEventAndCondition
{
    public:
        Q_OBJECT

    public:
        ServiceEventAndConditionTester(QObject *parent = 0);
        virtual ~ServiceEventAndConditionTester();

        static QSharedPointer<ServiceEventAndConditionTester> instance();
        static void release(QSharedPointer<ServiceEventAndConditionTester> tester);

        virtual int doTest();

    protected:
        ServiceEventAndConditionTesterThread *m_thread;

        virtual ServiceWaitCondition *newCondition();
        virtual bool processEvent(ServiceEvent *evt);
};

//-------------------------------------------------------------------------------------------

class ServiceEventAndConditionApplicationTest : public QCoreApplication
{
    public:
        Q_OBJECT

    public:
        ServiceEventAndConditionApplicationTest(int& argc,char **argv);
        virtual ~ServiceEventAndConditionApplicationTest();

        int result();

    protected:
        QSharedPointer<ServiceEventAndConditionTester> m_tester;
        int m_result;

    protected Q_SLOTS:
        void onInit();
        void onRunTest();
        void onShutdown();
};

//-------------------------------------------------------------------------------------------

class ServiceWaitConditionMock : public ServiceWaitCondition
{
    public:
        MOCK_METHOD0(get,void());
        MOCK_METHOD0(wait,void());
        MOCK_METHOD0(wake,void());
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
