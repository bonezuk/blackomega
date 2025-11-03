//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_FTP_FTPSERVICE_H
#define __OMEGA_NETWORK_FTP_FTPSERVICE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPServerSocket.h"
#include "network/ftp/inc/FTPServer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace ftp
{
//-------------------------------------------------------------------------------------------

class FTP_EXPORT FTPServiceEvent : public QEvent
{
    public:

        typedef enum
        {
            e_getFTPServerEvent = QEvent::User + 1050,
            e_deleteFTPServerEvent
        } FTPServiceEventType;

    public:
        FTPServiceEvent(FTPServiceEventType t);

        Qt::HANDLE threadId();

        tint port();
        void port(tint x);

        FTPServer *getServer();
        void setServer(FTPServer *x);

    protected:

        Qt::HANDLE m_threadId;
        tint m_port;
        FTPServer *m_server;
};

//-------------------------------------------------------------------------------------------

class FTP_EXPORT FTPService : public TCPService
{
    public:
        Q_OBJECT

    public:
        FTPService(QObject *parent = 0);
        virtual ~FTPService();

        FTPServer *getServer(tint port = 21);

        void deleteServer(FTPServer *svr);

    protected:

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual bool event(QEvent *e);
        virtual void processCustomEvent(FTPServiceEvent *e,void *results);

        virtual FTPServer *onGetServer(tint port);
        virtual void onDeleteServer(FTPServer *svr);
};

//-------------------------------------------------------------------------------------------
} // namespace ftp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

