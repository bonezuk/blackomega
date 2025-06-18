//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_WINLIRCCLIENT_H
#define __OMEGA_REMOTE_WINLIRCCLIENT_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPConnClientSocket.h"
#include "remote/inc/RemoteIF.h"
#include "remote/inc/WinLIRCService.h"
#include "common/inc/TimeStamp.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT WinLIRCClient : public network::TCPConnClientSocket
{
    public:
        Q_OBJECT
    
    public:
        class LIRCCommand;
    
    public:
        WinLIRCClient(network::Service *svr,QObject *parent = 0);
        virtual ~WinLIRCClient();
        
        virtual bool isConnected();
        virtual void connect(const QString& host,int port);
        virtual void shutdown();
        
        virtual bool process();
        
    protected:
        
        int m_processState;
        
        QMutex m_processQueueMutex;
        QList<LIRCCommand> m_processQueue;
        
        common::TimeStamp m_connectReferenceTime;
        
        virtual void processNotConnected(const LIRCCommand& cmd);
        virtual void processConnected();

        virtual int getProcessState() const;
        virtual void setProcessState(int nState);
        
        virtual void lockProcessQueue();
        virtual void unlockProcessQueue();
        virtual QList<LIRCCommand>& getProcessQueue();
        
        virtual network::socket_type getSocket();
        
        virtual LIRCCommand getNextCommand();
        virtual void queueProcessCommand(const LIRCCommand& cmd);
        
        virtual void processLine(const QString& line);
        virtual bool parseIRLine(const QString& line,int& code,int& repeat,QString& cmd,QString& remote) const;
        virtual bool isValidCode(const QString& code) const;
        virtual QStringList getStringParts(const QString& line) const;

        virtual common::TimeStamp getCurrentReferenceTime();
        virtual common::TimeStamp getConnectReferenceTime();
        virtual void setConnectReferenceTime();
        
        virtual bool connectSocketProcess();
        
        virtual void emitRemoteCommand(QString cmd,int repeat);
        virtual void emitRemoteStatus(QString text,int statusCode);
        
    signals:
    
        void remoteCommand(QString cmd,int repeat);
        void remoteStatus(QString text,int statusCode);
};

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT WinLIRCClient::LIRCCommand
{
    public:

        typedef enum
        {
            e_OpenConnection = 1,
            e_ShutdownClient,
            e_UnknownCommand = 0
        } Command;

    public:
        LIRCCommand();
        LIRCCommand(const LIRCCommand& rhs);
        virtual ~LIRCCommand();
        
        const LIRCCommand& operator = (const LIRCCommand& rhs);
        
        virtual Command& command();
        virtual const Command& command() const;
        virtual QString& host();
        virtual const QString& host() const;
        virtual int& port();
        virtual const int& port() const;
        
    protected:
    
        Command m_command;
        QString m_host;
        int m_port;
        
        void copy(const LIRCCommand& rhs);
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

