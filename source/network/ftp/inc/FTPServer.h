//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_FTP_FTPSERVER_H
#define __OMEGA_NETWORK_FTP_FTPSERVER_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPServerSocket.h"
#include "network/inc/TCPService.h"
#include "network/ftp/inc/FTPTransfer.h"
#include "network/ftp/inc/FTPConfiguration.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace ftp
{
//-------------------------------------------------------------------------------------------

class FTP_EXPORT FTPFileFilter
{
    public:
        FTPFileFilter();
        virtual ~FTPFileFilter();
        
        virtual bool canFileBeUploaded(const QString& fileName) = 0;
};

//-------------------------------------------------------------------------------------------

class FTP_EXPORT FTPServer : public TCPServerSocket
{
    public:
        Q_OBJECT
        
        friend class FTPTransfer;
        friend class FTPSession;
        
    public:
        FTPServer(FTPService *svr,QObject *parent = 0);
        virtual ~FTPServer();
        
        virtual FTPConfiguration& config();
        virtual const FTPConfiguration& config() const;
        
        virtual bool canFileBeUploaded(const QString& fileName);
        virtual void setFileFilter(FTPFileFilter *filter);
        
        FTPTransferServerPool& transferServerPool();
        
        virtual void close();
        
    protected:
        
        FTPConfiguration m_config;
        FTPFileFilter *m_filter;
        FTPTransferServerPool m_pool;
        
        virtual TCPConnServerSocket *newIO();
        virtual void signalUploadComplete(const QString& fileName);
        virtual void signalRemoveFile(const QString& fileName);
        
    signals:
        void uploaded(const QString& fileName);
        void remove(const QString& fileName);
};

//-------------------------------------------------------------------------------------------
} // namespace ftp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

