//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_WINLIRCSERVICE_H
#define __OMEGA_REMOTE_WINLIRCSERVICE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPClientService.h"
#include "remote/inc/RemoteDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class WinLIRCClient;

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT WinLIRCServiceEvent : public QEvent
{
    public:
        typedef enum
        {
            e_newWinLIRCClientEvent = QEvent::User + 1281
        } WinLIRCServiceEventType;
        
    public:
        WinLIRCServiceEvent(WinLIRCServiceEventType t);
        
        Qt::HANDLE threadId();
        
    protected:
    
        Qt::HANDLE m_threadId;
};

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT WinLIRCService : public network::TCPClientService
{
    public:
        Q_OBJECT
    
    public:
        WinLIRCService(QObject *parent = 0);
        virtual ~WinLIRCService();
        
        virtual WinLIRCClient *getClient();
        
    protected:
        
        Qt::HANDLE m_serviceThreadId;
    
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual bool event(QEvent *e);
        virtual WinLIRCClient *onGetClient();
        
        virtual void processCustomEvent(WinLIRCServiceEvent *e,void *result);
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
