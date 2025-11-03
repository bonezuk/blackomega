//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_FTP_FTPTRANSFER_H
#define __OMEGA_NETWORK_FTP_FTPTRANSFER_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "common/inc/DiskOps.h"
#include "common/inc/BIOBufferedStream.h"
#include "common/inc/TimeStamp.h"
#include "common/inc/BOParse.h"
#include "network/inc/TCPServerSocket.h"
#include "network/inc/TCPConnClientSocket.h"
#include "network/inc/TCPConnServerSocket.h"
#include "network/ftp/inc/FTPDLL.h"

#if defined(OMEGA_POSIX)
#include <pwd.h>
#include <grp.h>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace ftp
{
//-------------------------------------------------------------------------------------------

class FTPTransfer;
class FTPService;
class FTPServer;

//-------------------------------------------------------------------------------------------

class FTP_EXPORT FTPTransferServer : public TCPServerSocket
{
    public:
        Q_OBJECT

    public:
        FTPTransferServer(FTPService *service);
        virtual ~FTPTransferServer();

        virtual void setTransfer(FTPTransfer *transfer);
        virtual void releaseTransfer();

    protected:

        FTPTransfer *m_ftpTransfer;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint eNo) const;

        virtual TCPConnServerSocket *newIO();
};

//-------------------------------------------------------------------------------------------

class FTP_EXPORT FTPTransferConnection : public TCPConnServerSocket
{
    public:
        Q_OBJECT

    public:
        FTPTransferConnection(FTPTransfer *ftpTransfer,QObject *parent = 0);
        virtual ~FTPTransferConnection();

        virtual bool open(socket_type serverS);

    protected:

        FTPTransfer *m_ftpTransfer;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint eNo) const;
};

//-------------------------------------------------------------------------------------------

class FTP_EXPORT FTPTransferServerPool
{
    public:
        FTPTransferServerPool(FTPService *service);
        virtual ~FTPTransferServerPool();

        virtual FTPTransferServer *getServer(FTPTransfer *ftpTransfer, int port);
        virtual void releaseServer(FTPTransferServer *server);

        virtual void closeAllServers();

    private:
        FTPService *m_service;
        QMap<int, FTPTransferServer *> m_serverPool;
        QMap<int, FTPTransferServer *> m_usedServers;

        virtual void printError(const tchar *strR, const tchar *strE) const;
        virtual void closeServerMap(QMap<int, FTPTransferServer *>& serverMap);
};

//-------------------------------------------------------------------------------------------

class FTP_EXPORT FTPTransfer : public QObject
{
    Q_OBJECT

    public:
        friend class FTPTransferServer;
        friend class FTPTransferConnection;

    public:
        FTPTransfer(FTPService *service,FTPServer *server);
        virtual ~FTPTransfer();

        FTPService *service();
        FTPServer *server();

        virtual tint process();

        virtual bool setPassive(tint& port);
        virtual bool setActive(const QString& host,tint port);
        virtual void setAppend(bool v);
        virtual void setNameList(bool v);

        virtual bool isUpload() const;
        virtual bool isDirectory() const;
        virtual const QString& name() const;
        virtual tint port() const;

        virtual bool setUpload(const QString& name);
        virtual bool setDownload(const QString& name);
        virtual bool setDirectory(const QString& name);

        virtual void close();

    protected:

        FTPService *m_ftpService;
        FTPServer *m_ftpServer;

        tint m_state;

        bool m_uploadFlag;
        bool m_directoryFlag;
        QString m_fileName;
        common::BIOBufferedStream *m_fileIO;

        FTPTransferServer *m_serverSocket;
        FTPTransferConnection *m_serverConnection;

        TCPConnClientSocket *m_clientConnection;

        bool m_timeoutFlag;
        common::TimeStamp m_timeoutTime;

        bool m_dirWriteFlag;
        bool m_appendFlag;
        bool m_nameListFlag;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual tint processUpload(TCPConnectionSocket *com);
        virtual tint processDownload(TCPConnectionSocket *com);

        virtual tint processDirectory(TCPConnectionSocket *com);
        virtual QString printFile(const QString& name);

        void startTimer(const common::TimeStamp& timeOut);
        void endTimer();
        bool hasTimeout() const;
};

//-------------------------------------------------------------------------------------------
} // namespace ftp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
