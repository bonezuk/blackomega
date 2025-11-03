#include "common/test/ServiceEventTest.h"

#include <QStringList>

//-------------------------------------------------------------------------------------------

class ServiceEventAndConditionUnitTest : public ServiceEventAndCondition
{
    public:
        ServiceEventAndConditionUnitTest();

        virtual ServiceWaitCondition *testGetCondition();
        virtual ServiceWaitCondition *testGetCondition(ServiceEvent *e);
        virtual ServiceWaitCondition *testGetCondition(Qt::HANDLE threadId);
        virtual void testFreeConditions();

        virtual void testWake(ServiceEvent *evt);

        virtual bool testEvent(QEvent *evt);

    protected:

        virtual ServiceWaitCondition *newCondition();
        virtual bool processEvent(ServiceEvent *evt);
};

//-------------------------------------------------------------------------------------------

ServiceEventAndConditionUnitTest::ServiceEventAndConditionUnitTest() : ServiceEventAndCondition()
{}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *ServiceEventAndConditionUnitTest::testGetCondition()
{
    return getCondition();
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *ServiceEventAndConditionUnitTest::testGetCondition(ServiceEvent *e)
{
    return getCondition(e);
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *ServiceEventAndConditionUnitTest::testGetCondition(Qt::HANDLE threadId)
{
    return getCondition(threadId);
}

//-------------------------------------------------------------------------------------------

void ServiceEventAndConditionUnitTest::testFreeConditions()
{
    freeConditions();
}

//-------------------------------------------------------------------------------------------

void ServiceEventAndConditionUnitTest::testWake(ServiceEvent *evt)
{
    wake(evt);
}

//-------------------------------------------------------------------------------------------

bool ServiceEventAndConditionUnitTest::testEvent(QEvent *evt)
{
    return event(evt);
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *ServiceEventAndConditionUnitTest::newCondition()
{
    return new ServiceWaitCondition();
}

//-------------------------------------------------------------------------------------------

bool ServiceEventAndConditionUnitTest::processEvent(ServiceEvent *evt)
{
    return false;
}

//-------------------------------------------------------------------------------------------
// ServiceEvent
//-------------------------------------------------------------------------------------------

TEST(ServiceEvent,definedEventAndNoWait)
{
    ServiceEvent *e = new ServiceEvent(QEvent::Leave,false);

    EXPECT_TRUE(e->type()==QEvent::Leave);
    EXPECT_FALSE(e->isWaitCondition());
    EXPECT_TRUE(e->threadId()==QThread::currentThreadId());

    QEvent *qEvt = reinterpret_cast<QEvent *>(e);
    ServiceEvent *evt = dynamic_cast<ServiceEvent *>(qEvt);
    EXPECT_TRUE(evt==e);

    delete e;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEvent,userEventAndWait)
{
    typedef enum
    {
        e_userEvent = QEvent::User + 200
    } ServiceEventType;

    ServiceEvent *e = new ServiceEvent(static_cast<QEvent::Type>(e_userEvent),true);

    EXPECT_TRUE(static_cast<ServiceEventType>(e->type())==e_userEvent);
    EXPECT_TRUE(e->isWaitCondition());
    EXPECT_TRUE(e->threadId()==QThread::currentThreadId());

    QEvent *qEvt = reinterpret_cast<QEvent *>(e);
    ServiceEvent *evt = dynamic_cast<ServiceEvent *>(qEvt);
    EXPECT_TRUE(evt==e);

    delete e;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEvent,dynamicCast)
{
    ServiceEvent *e = new ServiceEvent(QEvent::Leave,false);

    QEvent *qEvt = reinterpret_cast<QEvent *>(e);
    ServiceEvent *evt = dynamic_cast<ServiceEvent *>(qEvt);
    EXPECT_TRUE(evt==e);

    delete evt;
}

//-------------------------------------------------------------------------------------------
// ServiceDataEvent
//-------------------------------------------------------------------------------------------

ServiceDataTester::ServiceDataTester() : m_id(0)
{}

//-------------------------------------------------------------------------------------------

ServiceDataTester::ServiceDataTester(const ServiceDataTester& rhs) : m_id(rhs.m_id)
{}

//-------------------------------------------------------------------------------------------

const ServiceDataTester& ServiceDataTester::operator = (const ServiceDataTester& rhs)
{
    if(this!=&rhs)
    {
        m_id = rhs.m_id;
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

int& ServiceDataTester::id()
{
    return m_id;
}

//-------------------------------------------------------------------------------------------

const int& ServiceDataTester::idConst() const
{
    return m_id;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceDataEvent,inheritance)
{
    ServiceDataEvent<ServiceDataTester> *e = new ServiceDataEvent<ServiceDataTester>(QEvent::Leave,true);

    EXPECT_TRUE(e->type()==QEvent::Leave);
    EXPECT_TRUE(e->isWaitCondition());
    EXPECT_TRUE(e->threadId()==QThread::currentThreadId());

    delete e;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceDataEvent,setAndAccessData)
{
    ServiceDataEvent<ServiceDataTester> *e = new ServiceDataEvent<ServiceDataTester>(QEvent::Leave,true);
    e->data().id() = 100;

    EXPECT_TRUE(e->data().id()==100);

    ServiceDataTester d(e->data());
    EXPECT_TRUE(d.id()==100);

    delete e;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceDataEvent,setAndAccessDataConst)
{
    ServiceDataEvent<ServiceDataTester> *e = new ServiceDataEvent<ServiceDataTester>(QEvent::Leave,true);
    e->data().id() = 100;

    EXPECT_TRUE(e->dataConst().idConst()==100);

    ServiceDataTester d(e->dataConst());
    EXPECT_TRUE(d.id()==100);

    delete e;
}

//-------------------------------------------------------------------------------------------

ServiceWaitConditionThreadTester::ServiceWaitConditionThreadTester(ServiceWaitCondition *cond) : QThread(),
    m_conditionA(cond)
{
    m_conditionB = new ServiceWaitCondition;
}

//-------------------------------------------------------------------------------------------

ServiceWaitConditionThreadTester::~ServiceWaitConditionThreadTester()
{
    delete m_conditionA;
    delete m_conditionB;
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *ServiceWaitConditionThreadTester::condition()
{
    return m_conditionB;
}

//-------------------------------------------------------------------------------------------

void ServiceWaitConditionThreadTester::run()
{
    m_conditionB->get();
    m_conditionA->wake();
    m_conditionB->wait();
}

//-------------------------------------------------------------------------------------------

TEST(ServiceWaitCondition,getWaitWake)
{
    ServiceWaitCondition *cond = new ServiceWaitCondition;
    ServiceWaitConditionThreadTester *testThread = new ServiceWaitConditionThreadTester(cond);

    cond->get();
    testThread->start();
    cond->wait();

    testThread->condition()->wake();
    testThread->wait();

    delete testThread;
}

//-------------------------------------------------------------------------------------------

class ServiceDataWaitConditionThreadTester : public QThread
{
    public:
        ServiceDataWaitConditionThreadTester(ServiceDataWaitCondition<ServiceDataTester> *cond);

    protected:

        ServiceDataWaitCondition<ServiceDataTester> *m_condition;

        virtual void run();
};

//-------------------------------------------------------------------------------------------

ServiceDataWaitConditionThreadTester::ServiceDataWaitConditionThreadTester(ServiceDataWaitCondition<ServiceDataTester> *cond) : m_condition(cond)
{}

//-------------------------------------------------------------------------------------------

void ServiceDataWaitConditionThreadTester::run()
{
    m_condition->data().id() = 200;
    m_condition->wake();
}

//-------------------------------------------------------------------------------------------

TEST(ServiceDataWaitCondition,getWaitWakeData)
{
    ServiceDataWaitCondition<ServiceDataTester> *cond = new ServiceDataWaitCondition<ServiceDataTester>();
    ServiceDataWaitConditionThreadTester *testThread = new ServiceDataWaitConditionThreadTester(cond);

    cond->get();
    testThread->start();
    cond->wait();

    EXPECT_TRUE(cond->data().id()==200);

    testThread->wait();
    delete testThread;
    delete cond;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceDataWaitCondition,getWaitWakeDataConst)
{
    ServiceDataWaitCondition<ServiceDataTester> *cond = new ServiceDataWaitCondition<ServiceDataTester>();
    ServiceDataWaitConditionThreadTester *testThread = new ServiceDataWaitConditionThreadTester(cond);

    cond->get();
    testThread->start();
    cond->wait();

    EXPECT_TRUE(cond->dataConst().idConst()==200);

    testThread->wait();
    delete testThread;
    delete cond;
}

//-------------------------------------------------------------------------------------------

ServiceEventAndConditionTester::ServiceEventAndConditionTester(QObject *parent) : ServiceEventAndCondition(parent),
    m_thread(0)
{}

//-------------------------------------------------------------------------------------------

ServiceEventAndConditionTester::~ServiceEventAndConditionTester()
{
    freeConditions();
    if(m_thread!=0)
    {
        delete m_thread;
    }
}

//-------------------------------------------------------------------------------------------

QSharedPointer<ServiceEventAndConditionTester> ServiceEventAndConditionTester::instance()
{
    QSharedPointer<ServiceEventAndConditionTester> tester;
    ServiceEventAndConditionTesterThread *thread = new ServiceEventAndConditionTesterThread;
    if(thread->ignite())
    {
        tester = thread->service();
        tester->setThread(thread);
    }
    return tester;
}

//-------------------------------------------------------------------------------------------

void ServiceEventAndConditionTester::release(QSharedPointer<ServiceEventAndConditionTester> tester)
{
    if(!tester.isNull())
    {
        QThread *pThread = tester->getThread();
        QSharedPointer<ServiceEventAndConditionTester> eTester;
        pThread->quit();
        pThread->wait();
        tester = eTester;
    }
}

//-------------------------------------------------------------------------------------------

ServiceWaitCondition *ServiceEventAndConditionTester::newCondition()
{
    return new ServiceDataWaitCondition<ServiceDataTester>();
}

//-------------------------------------------------------------------------------------------

bool ServiceEventAndConditionTester::processEvent(ServiceEvent *evt)
{
    ServiceDataEvent<ServiceDataTester> *e = dynamic_cast<ServiceDataEvent<ServiceDataTester> *>(evt);
    bool res = false;

    if(e!=0)
    {
        if(e->type()==(QEvent::User + 1234))
        {
            ServiceDataWaitCondition<ServiceDataTester> *cond = dynamic_cast<ServiceDataWaitCondition<ServiceDataTester> *>(getCondition(e));
            cond->data().id() = e->data().id() * 2;
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

int ServiceEventAndConditionTester::doTest()
{
    ServiceDataWaitCondition<ServiceDataTester> *cond = dynamic_cast<ServiceDataWaitCondition<ServiceDataTester> *>(getCondition());
    ServiceDataEvent<ServiceDataTester> *e = new ServiceDataEvent<ServiceDataTester>(static_cast<QEvent::Type>(QEvent::User + 1234),true);
    e->data().id() = 100;
    QCoreApplication::postEvent(this,e);
    cond->wait();
    return cond->data().id();
}

//-------------------------------------------------------------------------------------------

ServiceEventAndConditionApplicationTest::ServiceEventAndConditionApplicationTest(int& argc,char **argv) : QCoreApplication(argc,argv),
    m_tester(),
    m_result(0)
{
    QTimer::singleShot(50,this,SLOT(onInit()));
}

//-------------------------------------------------------------------------------------------

ServiceEventAndConditionApplicationTest::~ServiceEventAndConditionApplicationTest()
{}

//-------------------------------------------------------------------------------------------

int ServiceEventAndConditionApplicationTest::result()
{
    return m_result;
}

//-------------------------------------------------------------------------------------------

void ServiceEventAndConditionApplicationTest::onInit()
{
    m_tester = ServiceEventAndConditionTester::instance();
    QTimer::singleShot(50,this,SLOT(onRunTest()));
}

//-------------------------------------------------------------------------------------------

void ServiceEventAndConditionApplicationTest::onRunTest()
{
    m_result = m_tester->doTest();
    QTimer::singleShot(50,this,SLOT(onShutdown()));
}

//-------------------------------------------------------------------------------------------

void ServiceEventAndConditionApplicationTest::onShutdown()
{
    ServiceEventAndConditionTester::release(m_tester);
    quit();
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,run)
{
    QStringList libraryPath = QCoreApplication::libraryPaths();
    {
        int argc = 0;
        char **argv = 0;
        ServiceEventAndConditionApplicationTest test(argc,argv);
        test.exec();
        EXPECT_TRUE(test.result()==200);
    }
    QCoreApplication::setLibraryPaths(libraryPath);
}

//-------------------------------------------------------------------------------------------

class ServiceEventAndConditioningMockTest : public ServiceEventAndConditionUnitTest
{
    public:
        MOCK_METHOD0(lock,void());
        MOCK_METHOD0(unlock,void());
        MOCK_METHOD0(waitConditionMap,QMap<Qt::HANDLE,ServiceWaitCondition *>&());
        MOCK_METHOD0(newCondition,ServiceWaitCondition*());
};

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,getConditionWhenNoneForCallingThread)
{
    QMap<Qt::HANDLE,ServiceWaitCondition *> conditionMap;

    ServiceWaitConditionMock cond;
    EXPECT_CALL(cond,get()).Times(1);

    ServiceEventAndConditioningMockTest service;
    EXPECT_CALL(service,waitConditionMap()).WillRepeatedly(ReturnRef(conditionMap));
    EXPECT_CALL(service,lock()).Times(1);
    EXPECT_CALL(service,unlock()).Times(1);
    EXPECT_CALL(service,newCondition()).Times(1).WillOnce(Return(&cond));

    ServiceWaitCondition *condA = service.testGetCondition();

    EXPECT_TRUE(condA==&cond);
    EXPECT_TRUE(conditionMap.size()==1);
    EXPECT_TRUE(conditionMap.find(QThread::currentThreadId()).value()==&cond);
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,getConditionWhenAllocatedForCallingThread)
{
    ServiceWaitConditionMock cond;
    EXPECT_CALL(cond,get()).Times(1);

    QMap<Qt::HANDLE,ServiceWaitCondition *> conditionMap;
    conditionMap.insert(QThread::currentThreadId(),&cond);

    ServiceEventAndConditioningMockTest service;
    EXPECT_CALL(service,waitConditionMap()).WillRepeatedly(ReturnRef(conditionMap));
    EXPECT_CALL(service,lock()).Times(1);
    EXPECT_CALL(service,unlock()).Times(1);

    ServiceWaitCondition *condA = service.testGetCondition();

    EXPECT_TRUE(condA==&cond);
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,getConditionFromEventWhenNoneForCallingThread)
{
    ServiceEvent *evt = new ServiceEvent(QEvent::Leave,true);

    QMap<Qt::HANDLE,ServiceWaitCondition *> conditionMap;

    ServiceWaitConditionMock cond;

    ServiceEventAndConditioningMockTest service;
    EXPECT_CALL(service,waitConditionMap()).WillRepeatedly(ReturnRef(conditionMap));
    EXPECT_CALL(service,lock()).Times(1);
    EXPECT_CALL(service,unlock()).Times(1);
    EXPECT_CALL(service,newCondition()).Times(1).WillOnce(Return(&cond));

    ServiceWaitCondition *condA = service.testGetCondition(evt);

    EXPECT_TRUE(condA==&cond);
    EXPECT_TRUE(conditionMap.size()==1);
    EXPECT_TRUE(conditionMap.find(QThread::currentThreadId()).value()==&cond);

    delete evt;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,getConditionFromEventWhenAllocatedForCallingThread)
{
    ServiceEvent *evt = new ServiceEvent(QEvent::Leave,true);

    ServiceWaitConditionMock cond;

    QMap<Qt::HANDLE,ServiceWaitCondition *> conditionMap;
    conditionMap.insert(QThread::currentThreadId(),&cond);

    ServiceEventAndConditioningMockTest service;
    EXPECT_CALL(service,waitConditionMap()).WillRepeatedly(ReturnRef(conditionMap));
    EXPECT_CALL(service,lock()).Times(1);
    EXPECT_CALL(service,unlock()).Times(1);

    ServiceWaitCondition *condA = service.testGetCondition(evt);

    EXPECT_TRUE(condA==&cond);

    delete evt;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,wakeUsingEvent)
{
    ServiceEvent *evt = new ServiceEvent(QEvent::Leave,true);

    ServiceWaitConditionMock cond;
    EXPECT_CALL(cond,wake()).Times(1);

    QMap<Qt::HANDLE,ServiceWaitCondition *> conditionMap;
    conditionMap.insert(QThread::currentThreadId(),&cond);

    ServiceEventAndConditioningMockTest service;
    EXPECT_CALL(service,waitConditionMap()).WillRepeatedly(ReturnRef(conditionMap));
    EXPECT_CALL(service,lock()).Times(1);
    EXPECT_CALL(service,unlock()).Times(1);

    service.testWake(evt);

    delete evt;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,freeConditions)
{
    Qt::HANDLE idA = (Qt::HANDLE)(123);
    Qt::HANDLE idB = (Qt::HANDLE)(456);
    Qt::HANDLE idC = (Qt::HANDLE)(789);

    ServiceWaitCondition *condA = new ServiceWaitCondition;
    ServiceWaitCondition *condB = new ServiceWaitCondition;
    ServiceWaitCondition *condC = new ServiceWaitCondition;

    QMap<Qt::HANDLE,ServiceWaitCondition *> conditionMap;
    conditionMap.insert(idA,condA);
    conditionMap.insert(idB,condB);
    conditionMap.insert(idC,condC);

    ServiceEventAndConditioningMockTest service;
    EXPECT_CALL(service,waitConditionMap()).WillRepeatedly(ReturnRef(conditionMap));
    EXPECT_CALL(service,lock()).Times(1);
    EXPECT_CALL(service,unlock()).Times(1);

    service.testFreeConditions();

    EXPECT_TRUE(conditionMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

class ServiceEventAndConditionEventTest : public ServiceEventAndConditionUnitTest
{
    public:
        MOCK_METHOD1(processEvent,bool(ServiceEvent *evt));
        MOCK_METHOD1(eventSuper,bool(QEvent *evt));
        MOCK_METHOD1(wake,void(ServiceEvent *evt));
};

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,eventForQEvent)
{
    QEvent *qEvent = new QEvent(QEvent::Leave);

    ServiceEventAndConditionEventTest service;
    EXPECT_CALL(service,eventSuper(qEvent)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(service.testEvent(qEvent));

    delete qEvent;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,eventForServiceEventAndNoProcessingAndNoWaitCondition)
{
    ServiceEvent *e = new ServiceEvent(static_cast<QEvent::Type>(QEvent::User + 1234),true);
    QEvent *qEvent = reinterpret_cast<QEvent *>(e);

    ServiceEventAndConditionEventTest service;
    EXPECT_CALL(service,processEvent(e)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(service,eventSuper(qEvent)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(service.testEvent(e));
    EXPECT_TRUE(e->isAccepted());

    delete e;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,eventForServiceEventAndNoProcessingAndWaitCondition)
{
    ServiceEvent *e = new ServiceEvent(static_cast<QEvent::Type>(QEvent::User + 1234),true);
    QEvent *qEvent = reinterpret_cast<QEvent *>(e);

    ServiceEventAndConditionEventTest service;
    EXPECT_CALL(service,processEvent(e)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(service,eventSuper(qEvent)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(service.testEvent(e));
    EXPECT_TRUE(e->isAccepted());

    delete e;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,eventForServiceEventAndProcessingAndNoWaitCondition)
{
    ServiceEvent *e = new ServiceEvent(static_cast<QEvent::Type>(QEvent::User + 1234),false);

    ServiceEventAndConditionEventTest service;
    EXPECT_CALL(service,processEvent(e)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(service.testEvent(e));
    EXPECT_TRUE(e->isAccepted());

    delete e;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,eventForServiceEventAndProcessingAndWaitCondition)
{
    ServiceEvent *e = new ServiceEvent(static_cast<QEvent::Type>(QEvent::User + 1234),true);

    ServiceEventAndConditionEventTest service;
    EXPECT_CALL(service,processEvent(e)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(service,wake(e)).Times(1);

    EXPECT_TRUE(service.testEvent(e));
    EXPECT_TRUE(e->isAccepted());

    delete e;
}

//-------------------------------------------------------------------------------------------

TEST(ServiceEventAndCondition,eventForServiceDataEventAndProcessingAndWaitCondition)
{
    ServiceDataEvent<ServiceDataTester> *e = new ServiceDataEvent<ServiceDataTester>(static_cast<QEvent::Type>(QEvent::User + 1234),true);

    ServiceEventAndConditionEventTest service;
    EXPECT_CALL(service,processEvent(e)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(service,wake(e)).Times(1);

    EXPECT_TRUE(service.testEvent(e));
    EXPECT_TRUE(e->isAccepted());

    delete e;
}

//-------------------------------------------------------------------------------------------
