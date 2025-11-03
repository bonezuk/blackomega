//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_WINLIRCDUMMYSERVER_H
#define __OMEGA_REMOTE_WINLIRCDUMMYSERVER_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPServerSocket.h"
#include "network/inc/TCPService.h"
#include "common/inc/XMLNodeParser.h"
#include "common/inc/TimeStamp.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class WinLIRCDummyServer;
class WinLIRCDummyService;

//-------------------------------------------------------------------------------------------

class WinLIRCDummyServer : public network::TCPServerSocket
{
    public:
        Q_OBJECT
        
    public:
        WinLIRCDummyServer(const QMap<double,QPair<QString,int> >& cmdMap,network::Service *svr,QObject *parent = 0);
        virtual ~WinLIRCDummyServer();
        
    protected:
    
        QMap<double,QPair<QString,int> > m_commandMap;
        
        virtual network::TCPConnServerSocket *newIO();
};

//-------------------------------------------------------------------------------------------

class WinLIRCDummySession : public network::TCPConnServerSocket
{
    public:
        Q_OBJECT
        
    public:
        WinLIRCDummySession(const QMap<double,QPair<QString,int> >& cmdMap,WinLIRCDummyService *svr,QObject *parent = 0);
        virtual ~WinLIRCDummySession();
        
        virtual bool process();
        
    public:
    
        common::TimeStamp m_startTime;
        QMap<double,QPair<QString,int> > m_commandMap;
};

//-------------------------------------------------------------------------------------------

class WinLIRCDummyService : public network::TCPService
{
    public:
        Q_OBJECT
        
    public:
        WinLIRCDummyService(QObject *parent = 0);
        virtual ~WinLIRCDummyService();

        virtual void run(const QString& xmlCmd);
        virtual void wait();
        
        virtual void complete();
        
    protected:
    
        QMutex m_waitMutex;
        QWaitCondition m_waitCondition;
        
        virtual bool event(QEvent *e);
        virtual void processRunEvent(const QString& xmlCommand);
        virtual void processShutdownEvent();
};

//-------------------------------------------------------------------------------------------

class WinLIRCDummyCommands : public common::XMLNodeParser
{
    public:
        WinLIRCDummyCommands();
        virtual ~WinLIRCDummyCommands();
        
        virtual int load(const QString& xmlText,QMap<double,QPair<QString,int> >& cmdMap);
        
    protected:
        
        QMap<double,QPair<QString,int> > m_commandMap;
    
        virtual bool isRootNode(xmlNodePtr pNode) const;
        virtual void processNode(xmlNodePtr pNode);
};

//-------------------------------------------------------------------------------------------

class WinLIRCDummyServiceEvent : public QEvent
{
    public:
        
        typedef enum
        {
            e_runWinLIRCServiceEventType = QEvent::User + 1070,
            e_shutdownWinLIRCServiceEventType
        } WinLIRCServiceEventType;
        
    public:
        WinLIRCDummyServiceEvent(WinLIRCServiceEventType t);
        
        QString& xmlCommand();
        const QString& xmlCommand() const;
        
    protected:
    
        QString m_xmlCommand;
};

//-------------------------------------------------------------------------------------------

class WinLIRCDummyApplication : public QObject
{
    public:
        Q_OBJECT
    public:
        WinLIRCDummyApplication(QObject *parent = 0);
        virtual ~WinLIRCDummyApplication();
        
    protected slots:
        void onExecute();
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
