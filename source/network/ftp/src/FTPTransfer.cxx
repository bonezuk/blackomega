#include "network/ftp/inc/FTPTransfer.h"
#include "network/ftp/inc/FTPServer.h"
#include "network/ftp/inc/FTPSession.h"
#include "network/ftp/inc/FTPService.h"

#if defined(OMEGA_POSIX)
#include <stdio.h>
#include <dirent.h>
#endif
#include <inttypes.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace ftp
{
//-------------------------------------------------------------------------------------------
// FTPTransferServer
//-------------------------------------------------------------------------------------------

FTPTransferServer::FTPTransferServer(FTPService *service) : TCPServerSocket(reinterpret_cast<Service *>(service), reinterpret_cast<QObject *>(service)),
    m_ftpTransfer(0)
{}

//-------------------------------------------------------------------------------------------

FTPTransferServer::~FTPTransferServer()
{}

//-------------------------------------------------------------------------------------------

void FTPTransferServer::printError(const tchar *strR,const tchar *strE) const
{
#if defined(OMEGA_WIN32)
    tint err = static_cast<tint>(::WSAGetLastError());
#elif defined(OMEGA_POSIX)
    tint err = errno;
#endif
    Resource::instance().error("FTPTransferServer",strR,strE,err);
}

//-------------------------------------------------------------------------------------------

void FTPTransferServer::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("FTPTransferServer",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

TCPConnServerSocket *FTPTransferServer::newIO()
{
    FTPTransferConnection *io;
    
    if(m_ftpTransfer != 0)
    {
        io = new FTPTransferConnection(m_ftpTransfer,this);
        io->lock();
    }
    else
    {
        QString err = QString("No associated FTP transfer identified with port %1").arg(port());
        printError("newIO", err.toUtf8().constData());
        io = 0;
    }
    return reinterpret_cast<TCPConnServerSocket *>(io);
}

//-------------------------------------------------------------------------------------------

void FTPTransferServer::setTransfer(FTPTransfer *transfer)
{
    m_ftpTransfer = transfer;
}

//-------------------------------------------------------------------------------------------

void FTPTransferServer::releaseTransfer()
{
    m_ftpTransfer = 0;
}

//-------------------------------------------------------------------------------------------
// FTPTransferConnection
//-------------------------------------------------------------------------------------------

FTPTransferConnection::FTPTransferConnection(FTPTransfer *ftpTransfer,QObject *parent) : TCPConnServerSocket(reinterpret_cast<Service *>(ftpTransfer->service()),parent),
    m_ftpTransfer(ftpTransfer)
{}

//-------------------------------------------------------------------------------------------

FTPTransferConnection::~FTPTransferConnection()
{}

//-------------------------------------------------------------------------------------------

void FTPTransferConnection::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "FTPTransferConnection::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void FTPTransferConnection::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("FTPTransferConnection",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

bool FTPTransferConnection::open(socket_type serverS)
{
    bool res = false;
    
    if(TCPConnServerSocket::open(serverS))
    {
        if(m_ftpTransfer->m_serverConnection!=0)
        {
            printError("open","File transfer connection already establised");
            close();
        }
        else
        {
            m_ftpTransfer->m_serverConnection = this;
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// FTPTransferServerPool
//-------------------------------------------------------------------------------------------

FTPTransferServerPool::FTPTransferServerPool(FTPService *service) : m_service(service),
    m_serverPool(),
    m_usedServers()
{}

//-------------------------------------------------------------------------------------------

FTPTransferServerPool::~FTPTransferServerPool()
{
    FTPTransferServerPool::closeAllServers();
}

//-------------------------------------------------------------------------------------------

void FTPTransferServerPool::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "FTPTransferConnection::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

FTPTransferServer *FTPTransferServerPool::getServer(FTPTransfer *ftpTransfer, int port)
{
    FTPTransferServer *server;
    QMap<int, FTPTransferServer *>::iterator ppI;
    
    ppI = m_serverPool.find(port);
    if(ppI != m_serverPool.end())
    {
        server = ppI.value();
        m_serverPool.erase(ppI);
        m_usedServers.insert(port, server);
    }
    else
    {
        ppI = m_usedServers.find(port);
        if(ppI != m_usedServers.end())
        {
            server = 0;
        }
        else
        {
            server = new FTPTransferServer(m_service);
            if(server->open(port))
            {
                m_usedServers.insert(port, server);
            }
            else
            {
                QString err = QString("Failed to create FTP passive server on port %1").arg(port);
                printError("getServer", err.toUtf8().constData());
                server->close();
                delete server;
                server = 0;
            }
        }
    }
    if(server != 0)
    {
        server->setTransfer(ftpTransfer);
    }
    return server;
}

//-------------------------------------------------------------------------------------------

void FTPTransferServerPool::releaseServer(FTPTransferServer *server)
{
    QMap<int, FTPTransferServer *>::iterator ppI;
    
    if(server != 0)
    {
        server->releaseTransfer();
        ppI = m_usedServers.find(server->port());
        if(ppI != m_usedServers.end())
        {
            m_usedServers.erase(ppI);
            m_serverPool.insert(server->port(), server);
        }
        else
        {
            printError("releaseServer", "Give FTP transfer server is not used");
        }
    }
}

//-------------------------------------------------------------------------------------------

void FTPTransferServerPool::closeServerMap(QMap<int, FTPTransferServer *>& serverMap)
{
    for(QMap<int, FTPTransferServer *>::iterator ppI = serverMap.begin(); ppI != serverMap.end(); ppI++)
    {
        FTPTransferServer *server = ppI.value();
        server->close();
        delete server;
    }
    serverMap.clear();
}

//-------------------------------------------------------------------------------------------

void FTPTransferServerPool::closeAllServers()
{
    if(!m_usedServers.isEmpty())
    {
        printError("closeAllServers", "Some FTP transfer servers in use");
        closeServerMap(m_usedServers);
    }
    closeServerMap(m_serverPool);
}

//-------------------------------------------------------------------------------------------
// FTPTransfer
//-------------------------------------------------------------------------------------------

FTPTransfer::FTPTransfer(FTPService *service,FTPServer *server) : QObject(),
    m_ftpService(service),
    m_ftpServer(server),
    m_state(0),
    m_uploadFlag(false),
    m_directoryFlag(false),
    m_fileName(),
    m_fileIO(0),
    m_serverSocket(0),
    m_serverConnection(0),
    m_clientConnection(0),
    m_timeoutFlag(false),
    m_timeoutTime(0),
    m_dirWriteFlag(false),
    m_appendFlag(false),
    m_nameListFlag(false)
{}

//-------------------------------------------------------------------------------------------

FTPTransfer::~FTPTransfer()
{
    FTPTransfer::close();
}

//-------------------------------------------------------------------------------------------

void FTPTransfer::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "FTPTransfer::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

FTPService *FTPTransfer::service()
{
    return m_ftpService;
}

//-------------------------------------------------------------------------------------------

FTPServer *FTPTransfer::server()
{
    return m_ftpServer;
}

//-------------------------------------------------------------------------------------------

bool FTPTransfer::isUpload() const
{
    return m_uploadFlag;
}

//-------------------------------------------------------------------------------------------

bool FTPTransfer::isDirectory() const
{
    return m_directoryFlag;
}

//-------------------------------------------------------------------------------------------

const QString& FTPTransfer::name() const
{
    return m_fileName;
}

//-------------------------------------------------------------------------------------------

tint FTPTransfer::port() const
{
    if(m_serverSocket!=0)
    {
        return m_serverSocket->port();
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

bool FTPTransfer::setPassive(tint& port)
{
    tint portMax = port + 10;
    
    if(m_state!=0)
    {
        printError("setPassive","Transfer process has already started");
        return false;
    }
    if(m_clientConnection!=0)
    {
        printError("setPassive","Cannot set transfer to passive as already set to active");
        return false;
    }
    if(m_serverSocket!=0)
    {
        port = m_serverSocket->port();
        return true;
    }
    
    while(port < portMax && m_serverSocket == 0)
    {
        m_serverSocket = m_ftpServer->transferServerPool().getServer(this, port);
        if(m_serverSocket == 0)
        {
            port++;
        }
    }
    if(m_serverSocket == 0)
    {
        printError("setPassive","Failed to create passive data connection");
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool FTPTransfer::setActive(const QString& host,tint port)
{
    if(m_state!=0)
    {
        printError("setActive","Transfer process has already started");
        return false;
    }
    if(m_serverSocket!=0)
    {
        printError("setActive","Cannot set transfer to active as already passive");
        return false;
    }
    if(m_clientConnection!=0)
    {
        return true;
    }
    
    m_clientConnection = new TCPConnClientSocket(reinterpret_cast<Service *>(m_ftpService),this);
    if(!m_clientConnection->open(host,port))
    {
        QString err;
        err = "Cannot establish connection to '" + host + "' on port " + QString::number(port);
        printError("setActive",err.toLatin1().constData());
        delete m_clientConnection;
        m_clientConnection = 0;
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void FTPTransfer::setAppend(bool v)
{
    m_appendFlag = v;
}

//-------------------------------------------------------------------------------------------

void FTPTransfer::setNameList(bool v)
{
    m_nameListFlag = v;
}

//-------------------------------------------------------------------------------------------

bool FTPTransfer::setUpload(const QString& name)
{
    tint fileFlags = common::e_BIOStream_FileWrite;
    bool appendF = m_appendFlag;
    
    if(m_state!=0)
    {
        printError("setUpload","Transfer process has already started");
        return false;
    }
    if(m_fileIO!=0)
    {
        printError("setUpload","File for transfer has already been set");
        return false;
    }
    
    if(m_appendFlag && common::DiskOps::exist(name))
    {
        appendF = true;
    }
    else
    {
        fileFlags |= common::e_BIOStream_FileCreate;
    }
    m_fileIO = new common::BIOBufferedStream(fileFlags);
    
    if(m_fileIO->open(name))
    {
        if(appendF)
        {
            if(!m_fileIO->seek(0,common::e_Seek_End))
            {
                printError("setUpload","Error seeking to end of file");
                return false;
            }
        }
    }
    else
    {
        QString err;
        if(appendF)
        {
            err = "Failed to open destination file '";
        }
        else
        {
            err = "Failed to create upload destination file '";
        }
        err += name + "'";
        printError("setUpload",err.toLatin1().constData());
        delete m_fileIO;
        m_fileIO = 0;
        return false;
    }
    m_uploadFlag = true;
    return true;
}

//-------------------------------------------------------------------------------------------

bool FTPTransfer::setDownload(const QString& name)
{
    if(m_state!=0)
    {
        printError("setDownload","Transfer process has already started");
        return false;
    }
    if(m_fileIO!=0)
    {
        printError("setDownload","File for transfer has already been set");
        return false;
    }
    
    m_fileIO = new common::BIOBufferedStream(common::e_BIOStream_FileRead);
    if(!m_fileIO->open(name))
    {
        QString err;
        err = "Failed to open download source file '" + name + "'";
        printError("setDownload",err.toLatin1().constData());
        delete m_fileIO;
        m_fileIO = 0;
        return false;
    }
    m_uploadFlag = false;
    return true;
}

//-------------------------------------------------------------------------------------------

bool FTPTransfer::setDirectory(const QString& name)
{
    bool res = true;
    
    if(common::DiskOps::exist(name))
    {
        m_directoryFlag = true;
        m_uploadFlag = false;
        m_fileName = name;
        m_dirWriteFlag = false;
    }
    else
    {
        QString err;
        err = "Could not find file resource '" + name + "'";
        printError("setDirectory",err.toUtf8().constData());
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void FTPTransfer::close()
{
    if(m_fileIO!=0)
    {
        m_fileIO->close();
        delete m_fileIO;
        m_fileIO = 0;
    }
    if(m_serverConnection!=0)
    {
        m_serverConnection->close();
        delete m_serverConnection;
        m_serverConnection = 0;
    }
    if(m_serverSocket!=0)
    {
        m_ftpServer->transferServerPool().releaseServer(m_serverSocket);
        m_serverSocket = 0;
    }
    if(m_clientConnection!=0)
    {
        m_clientConnection->close();
        delete m_clientConnection;
        m_clientConnection = 0;
    }
}

//-------------------------------------------------------------------------------------------

void FTPTransfer::startTimer(const common::TimeStamp& timeOut)
{
    m_timeoutTime = common::TimeStamp::now() + timeOut;
    m_timeoutFlag = true;
}

//-------------------------------------------------------------------------------------------

void FTPTransfer::endTimer()
{
    m_timeoutFlag = false;
}

//-------------------------------------------------------------------------------------------

bool FTPTransfer::hasTimeout() const
{
    if(m_timeoutFlag)
    {
        common::TimeStamp n(common::TimeStamp::now());
        
        if(n > m_timeoutTime)
        {
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

tint FTPTransfer::process()
{
    if(!m_timeoutFlag)
    {
        common::TimeStamp t;
        t.minute(2);
        startTimer(t);
    }
    
    if(m_directoryFlag)
    {
        if(m_serverSocket!=0 && m_serverConnection!=0)
        {
            return processDirectory(m_serverConnection);
        }
        else if(m_clientConnection!=0)
        {
            return processDirectory(m_clientConnection);
        }
        else
        {
            return hasTimeout() ? -1 : 1;
        }
    }
    else if(m_serverSocket!=0 && m_fileIO!=0)
    {
        if(m_serverConnection==0)
        {
            return hasTimeout() ? -1 : 1;
        }
        else
        {
            if(m_uploadFlag)
            {
                return processUpload(m_serverConnection);
            }
            else
            {
                return processDownload(m_serverConnection);
            }
        }
    }
    else if(m_clientConnection!=0 && m_fileIO!=0)
    {
        if(m_uploadFlag)
        {
            return processUpload(m_clientConnection);
        }
        else
        {
            return processDownload(m_clientConnection);
        }
    }
    else
    {
        return hasTimeout() ? -1 : 1;
    }
}

//-------------------------------------------------------------------------------------------

tint FTPTransfer::processUpload(TCPConnectionSocket *com)
{
    tint amount,fAmount,total = 0;
    tchar mem[512];
    bool loop = true;
    
    if(com->timeout())
    {
        printError("processUpload","Connection has timed out");
        return -1;
    }
    
    while(loop)
    {
        amount = com->read(mem,512);
        if(amount>=0)
        {
            if(amount>0)
            {
                fAmount = m_fileIO->write(mem,amount);
                if(amount==fAmount)
                {
                    total += amount;
                }
                else
                {
                    printError("processUpload","Error writing data to file");
                    return -1;
                }
            }
            else
            {
                loop = false;
            }
        }
        else
        {
            printError("processUpload","Error reading from connection");
            return -1;
        }
    }
    
    if(amount>0)
    {
        com->timerOff();
    }
    else
    {
        if(!com->isTimer())
        {
            com->timerOn();
        }
    }
    
    if(com->state() & Socket::c_socketStateClose)
    {
        m_fileIO->close();
        m_ftpServer->signalUploadComplete(m_fileIO->name());
        return 0;
    }
    else
    {
        return 1;
    }
}

//-------------------------------------------------------------------------------------------

tint FTPTransfer::processDownload(TCPConnectionSocket *com)
{
    tint amount,total;
    tchar mem[512];
    
    if(com->timeout())
    {
        printError("processDownload","Connection has timed out");
        return -1;
    }
    if(com->state() & Socket::c_socketStateClose)
    {
        printError("processDownload","Download aborted as connection has been closed");
        return -1;
    }
    
    if(!com->isTimer() && !m_fileIO->eof())
    {
        com->timerOn();
    }
    
    total = com->writeQueueSize();
    while(total<1048576 && !m_fileIO->eof())
    {
        amount = m_fileIO->read(mem,512);
        if(amount>=0)
        {
            if(amount>0)
            {
                if(com->write(mem,amount))
                {
                    com->timerOff();
                    total += amount;
                }
                else
                {
                    printError("processDownload","Error writing file data to connection");
                    return -1;
                }
            }
        }
        else
        {
            printError("processDownload","Error reading from source file");
            return -1;
        }
    }
    
    if(!total && m_fileIO->eof())
    {
        com->close();
        return 0;
    }
    else
    {
        return 1;
    }
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

tint FTPTransfer::processDirectory(TCPConnectionSocket *com)
{
    if(!m_dirWriteFlag)
    {
        LPCWSTR wStr;
        HANDLE h;
        struct _stat fileStat;
        WIN32_FIND_DATAW fData;
        QString tmp,fullPath(m_fileName);
    
        common::DiskOps::formatDirectoryPath(fullPath);
        wStr = reinterpret_cast<LPCWSTR>(fullPath.utf16());
        if(::_wstat(wStr,&fileStat)!=0)
        {
            return -1;
        }
        if(S_IFDIR & fileStat.st_mode)
        {
            ::memset(&fData,0,sizeof(WIN32_FIND_DATAW));
            tmp = fullPath + "\\*";
        
            wStr = reinterpret_cast<LPCWSTR>(tmp.utf16());
            h = ::FindFirstFileW(wStr,&fData);
            if(h!=INVALID_HANDLE_VALUE)
            {    
                do
                {
                    QString cName,entry;
                
                    cName = QString::fromUtf16(reinterpret_cast<const char16_t *>(fData.cFileName));
                    if(cName!="." && cName!="..")
                    {
                        tmp = fullPath + "\\" + cName;
                        entry = printFile(tmp);
                        if(!entry.isEmpty())
                        {
                            common::BString e;
                        
                            e = entry.toUtf8().constData();
                            e += "\r\n";
                            if(!(com->write(e.getString(),e.len())))
                            {
                                return -1;
                            }
                        }
                    }
                } while(::FindNextFileW(h,&fData));
                ::FindClose(h);
            }
            else
            {
                return -1;
            }
        }
        else if(S_IFREG & fileStat.st_mode)
        {
            QString entry = printFile(m_fileName);
            if(!entry.isEmpty())
            {
                common::BString e;
            
                e = entry.toUtf8().constData();
                e += "\r\n";
                if(!(com->write(e.getString(),e.len())))
                {
                    return -1;
                }
            }
        }
        m_dirWriteFlag = true;
        return 1;
    }
    else
    {
        if(com->isTimer())
        {
            com->timerOn();
        }
        if(com->writeQueueSize() > 0)
        {
            if(com->timeout())
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 0;
        }
    }
}

//-------------------------------------------------------------------------------------------
// drwxr-sr-x  2 smaclean games  4096 May 12  2009 help
//-------------------------------------------------------------------------------------------

QString FTPTransfer::printFile(const QString& name)
{
    static const char *months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    
    tint i,len;
    LPCWSTR wStr;
    struct _stat fileStat;
    QString n,fSize;
    
    wStr = reinterpret_cast<LPCWSTR>(name.utf16());
    if(::_wstat(wStr,&fileStat)==0 && ((fileStat.st_mode & S_IFDIR) || (fileStat.st_mode & S_IFREG)))
    {
        if(m_nameListFlag)
        {
            n = name;
        }
        else
        {
            bool rFlag = (fileStat.st_mode & _S_IREAD) ? true : false;
            bool wFlag = (fileStat.st_mode & _S_IWRITE) ? true : false;
            bool eFlag = (fileStat.st_mode & _S_IEXEC) ? true : false;
            common::TimeStamp mTime;
        
            if(fileStat.st_mode & _S_IFDIR)
            {
                n = "d";
            }
            else
            {
                n = "-";
            }
            for(i=0;i<3;i++)
            {
                n += rFlag ? "r" : "-";
                n += wFlag ? "w" : "-";
                n += wFlag ? "x" : "-";
            }
            
            fSize = QString::number(fileStat.st_nlink);
            len = 4 - fSize.length();
            for(i=0;i<len;i++)
            {
                n += " ";
            }
            n += " " + fSize;
            
            n += " " + Resource::instance().username() + " " + Resource::instance().username();
        
            fSize = QString::number(fileStat.st_size);
            len = 10 - fSize.length();
            for(i=0;i<len;i++)
            {
                n += " ";
            }
            n += " " + fSize;
            
            mTime.set(&fileStat.st_mtime);
            n += " " + QString::fromLatin1(months[mTime.month()-1]) + " ";
            if(mTime.day()<10)
            {
                n += " ";
            }
            n += QString::number(mTime.day()) + " " + QString::number(mTime.year()) + " ";
        
            for(i=name.length()-1;i>=0;i--)
            {
                if(name.at(i)=='/' || name.at(i)=='\\')
                {
                    break;
                }    
            }
            n += name.mid(i+1);
        }
    }
    return n;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

tint FTPTransfer::processDirectory(TCPConnectionSocket *com)
{
    if(!m_dirWriteFlag)
    {
        DIR *h;
        struct dirent *entry;
        struct stat fileStat;
        QString tmp,fullPath(m_fileName),e;
        
        common::DiskOps::formatDirectoryPath(fullPath);
        if(::stat(fullPath.toUtf8().constData(),&fileStat)==0)
        {
            if(S_IFDIR & fileStat.st_mode)
            {
                h = ::opendir(fullPath.toUtf8().constData());
                if(h!=0)
                {
                    while(entry=::readdir(h),entry!=0)
                    {
                        tmp = QString::fromUtf8(entry->d_name);
                        if(tmp!="." && tmp!="..")
                        {
                            tmp = fullPath + "/" + tmp;
                            e = printFile(tmp);
                            if(!e.isEmpty())
                            {
                                e += "\r\n";
                                if(!(com->write(e.toUtf8().constData(),e.toUtf8().length())))
                                {
                                    return -1;
                                }
                            }
                        }
                    }
                    ::closedir(h);
                }
            }
            else if(S_IFREG & fileStat.st_mode)
            {
                e = printFile(m_fileName);
                if(!e.isEmpty())
                {
                    e += "\r\n";
                    if(com->write(e.toUtf8().constData(),e.toUtf8().length())!=e.toUtf8().length())
                    {
                        return -1;
                    }
                }                
            }
        }
        else
        {
            return -1;
        }
        m_dirWriteFlag = true;
        return 1;
    }
    else
    {
        if(com->isTimer())
        {
            com->timerOn();
        }
        if(com->writeQueueSize() > 0)
        {
            if(com->timeout())
            {
                return -1;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 0;
        }
    }
}

//-------------------------------------------------------------------------------------------

QString FTPTransfer::printFile(const QString& name)
{
    static const char *months[12] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    
    tint i,len;
    QString n,fSize;
    struct stat fileStat;
    struct passwd *userInfo;
    struct group *groupInfo;
    
    if(::stat(name.toUtf8().constData(),&fileStat)==0 && ((fileStat.st_mode & S_IFDIR) || (fileStat.st_mode & S_IFREG)))
    {
        if(m_nameListFlag)
        {
            n = name;
        }
        else
        {
            common::TimeStamp mTime;
            
            n  = (fileStat.st_mode & S_IFDIR) ? "d" : "-";
            n += (fileStat.st_mode & S_IRUSR) ? "r" : "-";
            n += (fileStat.st_mode & S_IWUSR) ? "w" : "-";
            n += (fileStat.st_mode & S_IXUSR) ? "x" : "-";
            n += (fileStat.st_mode & S_IRGRP) ? "r" : "-";
            n += (fileStat.st_mode & S_IWGRP) ? "w" : "-";
            n += (fileStat.st_mode & S_IXGRP) ? "x" : "-";
            n += (fileStat.st_mode & S_IROTH) ? "r" : "-";
            n += (fileStat.st_mode & S_IWOTH) ? "w" : "-";
            n += (fileStat.st_mode & S_IWOTH) ? "x" : "-";
        
            fSize = QString::number(fileStat.st_nlink);
            len = 4 - fSize.length();
            for(i=0;i<len;i++)
            {
                n += " ";
            }
            n += " " + fSize;
        
            userInfo = getpwuid(fileStat.st_uid);
            if(userInfo!=0)
            {
                fSize = userInfo->pw_name;
            }
            else
            {
                fSize = QString::number(fileStat.st_uid);
            }
            len = 12 - fSize.length();
            for(i=0;i<len;i++)
            {
                n += " ";
            }
            n += " " + fSize;
        
            groupInfo = getgrgid(fileStat.st_gid);
            if(groupInfo!=0)
            {
                fSize = QString::fromUtf8(groupInfo->gr_name);
            }
            else
            {
                fSize = QString::number(fileStat.st_gid);
            }
            len = 12 - fSize.length();
            for(i=0;i<len;i++)
            {
                n += " ";
            }
            n += " " + fSize;
            
            fSize = QString::number(static_cast<tint>(fileStat.st_size));
            len = 10 - fSize.length();
            for(i=0;i<len;i++)
            {
                n += " ";
            }
            n += " " + fSize;
#if defined(OMEGA_MACOSX) || defined(OMEGA_IOS)
            mTime.set(&fileStat.st_mtimespec);
#else
            mTime.set(&fileStat.st_mtime);
#endif
            n += " ";
            n += months[mTime.month()-1];
            n += " ";
            if(mTime.day()<10)
            {
                n += " ";
            }
            n += QString::number(mTime.day()) + " " + QString::number(mTime.year()) + " ";
        
            for(i=name.length()-1;i>=0;i--)
            {
                if(name.at(i)=='/' || name.at(i)=='\\')
                {
                    break;
                }    
            }
            n += name.mid(i+1);
        }
    }
    return n;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace ftp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
