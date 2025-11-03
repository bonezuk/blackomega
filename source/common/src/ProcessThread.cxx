#include "common/inc/ProcessThread.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

ProcessService::ProcessService(QObject *parent) : QObject(parent)
{}

//-------------------------------------------------------------------------------------------

ProcessService::~ProcessService()
{}

//-------------------------------------------------------------------------------------------

ProcessThread::ProcessThread(QSharedPointer<ProcessService> pService,int period,QObject *parent) : QThread(parent),
    m_pService(pService),
    m_period(period),
    m_timer(0)
{}

//-------------------------------------------------------------------------------------------

ProcessThread::~ProcessThread()
{}

//-------------------------------------------------------------------------------------------

void ProcessThread::run()
{
    m_timer = new QTimer(this);
    QObject::connect(m_timer,SIGNAL(timeout()),this,SLOT(onTimer()));
    m_timer->setInterval(m_period);
    m_timer->setSingleShot(false);
    m_timer->start();
    
    exec();
    
    m_timer->stop();
    QObject::disconnect(m_timer,SIGNAL(timeout()),this,SLOT(onTimer()));
    delete m_timer;
}

//-------------------------------------------------------------------------------------------

void ProcessThread::onTimer()
{
    m_pService->process();
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

