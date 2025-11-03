#include "network/ftp/inc/FTPSession.h"
#include "common/inc/DiskIF.h"
#include "common/inc/DiskOps.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace ftp
{
//-------------------------------------------------------------------------------------------

FTPSession::FTPSession(FTPServer *svr,QObject *parent) : TCPConnServerSocket(svr->service(),parent),
    m_server(svr),
    m_pstate(4),
    m_transfers(),
    m_username(),
    m_password(),
    m_path("/"),
    m_activeFlag(false),
    m_activeHost(),
    m_activePort(0),
    m_nextTransfer(0),
    m_lang(0),
    m_movePathName(),
    m_lastCommand()
{
    initParser();
    setDefault();
}

//-------------------------------------------------------------------------------------------

FTPSession::~FTPSession()
{
    QList<FTPTransfer *>::iterator ppI;
    
    if(m_nextTransfer!=0)
    {
        delete m_nextTransfer;
        m_nextTransfer = 0;
    }
    for(ppI=m_transfers.begin();ppI!=m_transfers.end();ppI++)
    {
        FTPTransfer *trans = *ppI;
        delete trans;
    }
    m_transfers.clear();
    delete m_lang;
}

//-------------------------------------------------------------------------------------------

FTPConfiguration& FTPSession::config()
{
    return m_server->config();
}

//-------------------------------------------------------------------------------------------

const FTPConfiguration& FTPSession::config() const
{
    return m_server->config();
}

//-------------------------------------------------------------------------------------------

void FTPSession::initParser()
{
    m_lang = new common::BOParse;
    m_langState[ 0] = m_lang->String("USER");
    m_langState[ 1] = m_lang->String("PASS");
    m_langState[ 2] = m_lang->String("ACCT");
    m_langState[ 3] = m_lang->String("CWD");
    m_langState[ 4] = m_lang->String("CDUP");
    m_langState[ 5] = m_lang->String("SMNT");
    m_langState[ 6] = m_lang->String("REIN");
    m_langState[ 7] = m_lang->String("QUIT");
    m_langState[ 8] = m_lang->String("PORT");
    m_langState[ 9] = m_lang->String("PASV");
    m_langState[10] = m_lang->String("TYPE");
    m_langState[11] = m_lang->String("STRU");
    m_langState[12] = m_lang->String("MODE");
    m_langState[13] = m_lang->String("RETR");
    m_langState[14] = m_lang->String("STOR");
    m_langState[15] = m_lang->String("STOU");
    m_langState[16] = m_lang->String("APPE");
    m_langState[17] = m_lang->String("ALLO");
    m_langState[18] = m_lang->String("REST");
    m_langState[19] = m_lang->String("RNFR");
    m_langState[20] = m_lang->String("RNTO");
    m_langState[21] = m_lang->String("ABOR");
    m_langState[22] = m_lang->String("DELE");
    m_langState[23] = m_lang->String("RMD");
    m_langState[24] = m_lang->String("MKD");
    m_langState[25] = m_lang->String("PWD");
    m_langState[26] = m_lang->String("LIST");
    m_langState[27] = m_lang->String("NLST");
    m_langState[28] = m_lang->String("SITE");
    m_langState[29] = m_lang->String("SYST");
    m_langState[30] = m_lang->String("STAT");
    m_langState[31] = m_lang->String("HELP");
    m_langState[32] = m_lang->String("NOOP");
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool FTPSession::dirExist(const QString& path)
{
    LPCWSTR wStr;
    struct _stat pathStat;
    
    wStr = reinterpret_cast<LPCWSTR>(path.utf16());
    if(::_wstat(wStr,&pathStat)==0)
    {
        if(S_IFDIR & pathStat.st_mode)
        {
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

bool FTPSession::fileExist(const QString& path)
{
    LPCWSTR wStr;
    struct _stat pathStat;
    
    wStr = reinterpret_cast<LPCWSTR>(path.utf16());
    if(::_wstat(wStr,&pathStat)==0)
    {
        if(S_IFREG & pathStat.st_mode)
        {
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

bool FTPSession::dirExist(const QString& path)
{
    struct stat pathStat;
    
    if(::stat(path.toUtf8().constData(),&pathStat)==0)
    {
        if(S_IFDIR & pathStat.st_mode)
        {
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

bool FTPSession::fileExist(const QString& path)
{
    struct stat pathStat;
    
    if(::stat(path.toUtf8().constData(),&pathStat)==0)
    {
        if(S_IFREG & pathStat.st_mode)
        {
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

QStringList FTPSession::getPathComponents(const QString& path)
{
    tint i,s;
    QChar c;
    QStringList pList;
    
    for(i=0,s=0;i<path.length();i++)
    {
        c = path.at(i);
        if(c==QChar('/') || c==QChar('\\'))
        {
            if(i>s)
            {
                pList.append(path.mid(s,i-s));
            }
            s = i+1;
        }
    }
    if(i>s)
    {
        pList.append(path.mid(s,i-s));
    }
    return pList;
}

//-------------------------------------------------------------------------------------------

QString FTPSession::buildLocalPath(const QStringList& pList)
{
    QString p;
    QStringList::const_iterator ppI;

    for(ppI=pList.begin();ppI!=pList.end();ppI++)
    {
        p += "/" + *ppI;
    }
    if(p.isEmpty())
    {
        p = "/";
    }
#if defined(OMEGA_POSIX)
    else if(p.length() >= 2 && p.left(2)=="//")
    {
        p = p.mid(1);
    }
#endif
    return p;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

QString FTPSession::buildPathFromComponents(const QStringList& pList)
{
    tint i;
    QString p;
    QStringList::const_iterator ppI;
    
    for(i=0,ppI=pList.begin();ppI!=pList.end();ppI++,i++)
    {
        if(i)
        {
            p += "\\";
        }
        p += *ppI;
    }
    return p;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

QString FTPSession::buildPathFromComponents(const QStringList& pList)
{
    QString p;
    QStringList::const_iterator ppI;
    
    for(ppI=pList.begin();ppI!=pList.end();ppI++)
    {
        p += "/" + *ppI;
    }
    if(p.isEmpty())
    {
        p = "/";
    }
    else if(p.length() >= 2 && p.left(2)=="//")
    {
        p = p.mid(1);
    }
    return p;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

QString FTPSession::getDirectoryPath(const QString& path)
{
    QString p;
    QStringList pList = getPathComponents(path);
    
    pList.prepend(config().rootPath());
    p = buildPathFromComponents(pList);
    if(!dirExist(p))
    {
        return QString();
    }
    else
    {
        return p;
    }
}

//-------------------------------------------------------------------------------------------

QString FTPSession::getTransferPath(const QString& path)
{
    tint i;
    QString p;
    QStringList pList = getPathComponents(path);
    QStringList rList = getPathComponents(config().rootPath());
    QStringList::iterator ppI;
    
    for(i=0;i<rList.size();i++)
    {
        if(pList.size()>0)
        {
            if(pList.at(0)==rList.at(i))
            {
                pList.removeFirst();
            }
            else
            {
                return QString();
            }
        }
        else
        {
            return QString();
        }
    }
    
    p += "/";
    for(ppI=pList.begin();ppI!=pList.end();ppI++)
    {
        p += "/" + *ppI;
    }
    return p;
}

//-------------------------------------------------------------------------------------------

QString FTPSession::changeDirectoryPath(const QString& path,const QString& cwd,bool sysFlag,bool chkFlag)
{
    QStringList pList;
    QStringList cList = getPathComponents(cwd);
    QStringList::const_iterator ppI;
    QString nPath;
    
    if(cwd.isEmpty() || !(cwd.at(0)==QChar('\\') || cwd.at(0)==QChar('/')))
    {
        pList = getPathComponents(path);
    }
        
    for(ppI=cList.begin();ppI!=cList.end();ppI++)
    {
        const QString& p = *ppI;
        
        if(p!=".")
        {
            if(p=="..")
            {
                if(!pList.isEmpty())
                {
                    pList.removeLast();
                }
            }
            else
            {
                pList.append(p);
            }
        }
    }
    pList.prepend(config().rootPath());
    
    nPath = buildPathFromComponents(pList);
    if(!chkFlag || common::DiskOps::exist(nPath))
    {
        if(!sysFlag)
        {
            pList.removeFirst();
            if(!pList.isEmpty())
            {
                nPath = buildLocalPath(pList);
            }
            else
            {
                nPath = "/";
            }
        }
        return nPath;
    }
    else
    {
        return QString();
    }
}

//-------------------------------------------------------------------------------------------

QString FTPSession::getUniqueFilename(const QString& name)
{
    tint i,end;
    QString dirName,fileName,ext,n;
    
    if(!fileExist(name))
    {
        return name;
    }
    
    end = name.length();
    for(i=name.length()-1;i>=0 && dirName.isEmpty();i--)
    {
        if(name.at(i)==QChar('/') || name.at(i)==QChar('\\'))
        {
            dirName = name.mid(0,i);
            fileName = name.mid(i+1,end-(i+1));
        }
        else if(name.at(i)==QChar('.') && ext.isEmpty())
        {
            if(i<name.length()-1)
            {
                ext = name.mid(i+1);
            }
        }
    }
    
    while(true)
    {
        n = dirName + c_Separator + fileName + QString::number(i);
        if(!ext.isEmpty())
        {
            n = "." + ext;
        }
        if(!fileExist(n))
        {
            return n;
        }
        i++;
    }
    
}

//-------------------------------------------------------------------------------------------

void FTPSession::setDefault()
{
    m_path = "/";
    m_activeFlag = false;
    m_activeHost = "";
    m_activePort = 0;
    if(m_nextTransfer!=0)
    {
        delete m_nextTransfer;
        m_nextTransfer = 0;
    }
    m_movePathName = "";
    m_lastCommand = "";
}

//-------------------------------------------------------------------------------------------

bool FTPSession::writeResponse(const common::BString& resp)
{
    common::BString r(resp);
    
    r += "\r\n";
    
    if(write(r.getString(),r.len()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

FTPTransfer *FTPSession::setupTransfer()
{
    FTPTransfer *trans = m_nextTransfer;
    bool res = true;
    
    m_nextTransfer = 0;
    if(trans==0)
    {
        common::BString resp;
        
        trans = new FTPTransfer(reinterpret_cast<FTPService *>(m_server->service()),m_server);
        
        if(!m_activeFlag)
        {
            tuint32 lIP = Resource::instance().localIP();
            struct sockaddr_in ipAddr;
            common::BString resp,sIP;
            QString ipStr;
            
            Resource::instance().setAddressIP(lIP,&ipAddr);
            ipStr = Resource::instance().networkIPAddressToString(ipAddr.sin_addr);
            sIP = ipStr.toLatin1().constData();

            if(Resource::instance().isIPAddress(QString::fromLatin1(sIP.getString())))
            {    
                tint port;

                port = config().passivePort();
                if(m_transfers.size()>0)
                {
                    QList<FTPTransfer *>::iterator ppI;

                    for(ppI=m_transfers.begin();ppI!=m_transfers.end();ppI++)
                    {
                        const FTPTransfer *trans = *ppI;
                        tint p = trans->port();

                        if(p!=0)
                        {
                            port = p + 1;
                        }
                    }
                }

                if(trans->setPassive(port))
                {
                    tint i;
                    tchar *x = sIP.getString();

                    for(i=0;i<sIP.length();i++)
                    {
                        if(x[i]=='.')
                        {
                            x[i] = ',';
                        }
                    }
                    resp  = "227 Entering Passive Move (" + sIP + ",";
                    resp += common::BString::Int((port >> 8) & 0x000000ff) + "," + common::BString::Int(port & 0x000000ff) + ")";
                }
                else
                {
                    resp = "421 Failed to create data port to listen on.";
                    res = false;
                }
            }
            else
            {
                resp = "421 Failed to get local network address.";
                res = false;
            }
        }
        else
        {
            if(!trans->setActive(m_activeHost,m_activePort))
            {
                resp = "425 Can't open data connection.";
                res = false;
            }
        }

        if(!resp.isEmpty() && !writeResponse(resp))
        {
            res = false;
        }
    }
    if(!res && trans!=0)
    {
        delete trans;
        trans = 0;
    }
    return trans;
}

//-------------------------------------------------------------------------------------------

bool FTPSession::process()
{
    bool res = true;
    
    if(m_pstate==4)
    {
        common::BString resp;
        resp = "220 Omega FTP";
        writeResponse(resp);
        m_pstate = 0;
    }

    for(QList<FTPTransfer *>::iterator ppI = m_transfers.begin(); ppI != m_transfers.end();)
    {
        tint tRes;
        FTPTransfer *transfer = *ppI;
        
        tRes = transfer->process();
        if(tRes<=0)
        {
            QString r,name;
            common::BString resp;
            
            name = getTransferPath(transfer->name());
            
            if(tRes==0)
            {
                if(!name.isEmpty() && !transfer->isDirectory())
                {
                    r = "226 Data " + name + " successfully transferred.";
                }
                else
                {
                    r = "226 Data successfully transferred.";
                }
            }
            else
            {
                if(!name.isEmpty())
                {
                    r = "426 Connection closed; " + name + " transfer aborted.";
                }
                else
                {    
                    r = "426 Connection closed; transfer aborted.";
                }
            }
            resp = r.toUtf8().constData();
            if(!writeResponse(resp))
            {
                return false;
            }
            ppI = m_transfers.erase(ppI);
            delete transfer;            
        }
        else
        {
            ppI++;
        }
    }
    
    if(m_pstate!=3 && !(state() & Socket::c_socketStateClose))
    {
        common::BString inst;
        
        while(canGetNextLine())
        {
            if(getNextLine(inst))
            {
                common::BO_Parse_Unit *lItem = m_lang->Lexical(inst.getString());
                
                if(lItem!=0)
                {
                    if(lItem->state==m_langState[0])
                    {
                        res = processUSER(inst);
                    }
                    else if(lItem->state==m_langState[1])
                    {
                        res = processPASS(inst);
                    }
                    else if(lItem->state==m_langState[2])
                    {
                        res = processACCT(inst);
                    }
                    else if(lItem->state==m_langState[3])
                    {
                        res = processCWD(inst);
                    }
                    else if(lItem->state==m_langState[4])
                    {
                        res = processCDUP(inst);
                    }
                    else if(lItem->state==m_langState[5])
                    {
                        res = processSMNT(inst);
                    }
                    else if(lItem->state==m_langState[6])
                    {
                        res = processREIN(inst);
                    }
                    else if(lItem->state==m_langState[7])
                    {
                        res = processQUIT(inst);
                    }
                    else if(lItem->state==m_langState[8])
                    {
                        res = processPORT(inst);
                    }
                    else if(lItem->state==m_langState[9])
                    {
                        res = processPASV(inst);
                    }
                    else if(lItem->state==m_langState[10])
                    {
                        res = processTYPE(inst);
                    }
                    else if(lItem->state==m_langState[11])
                    {
                        res = processSTRU(inst);
                    }
                    else if(lItem->state==m_langState[12])
                    {
                        res = processMODE(inst);
                    }
                    else if(lItem->state==m_langState[13])
                    {
                        res = processRETR(inst);
                    }
                    else if(lItem->state==m_langState[14])
                    {
                        res = processSTOR(inst);
                    }
                    else if(lItem->state==m_langState[15])
                    {
                        res = processSTOU(inst);
                    }
                    else if(lItem->state==m_langState[16])
                    {
                        res = processAPPE(inst);
                    }
                    else if(lItem->state==m_langState[17])
                    {
                        res = processALLO(inst);
                    }
                    else if(lItem->state==m_langState[18])
                    {
                        res = processREST(inst);
                    }
                    else if(lItem->state==m_langState[19])
                    {
                        res = processRNFR(inst);
                    }
                    else if(lItem->state==m_langState[20])
                    {
                        res = processRNTO(inst);
                    }
                    else if(lItem->state==m_langState[21])
                    {
                        res = processABOR(inst);
                    }
                    else if(lItem->state==m_langState[22])
                    {
                        res = processDELE(inst);
                    }
                    else if(lItem->state==m_langState[23])
                    {
                        res = processRMD(inst);
                    }
                    else if(lItem->state==m_langState[24])
                    {
                        res = processMKD(inst);
                    }
                    else if(lItem->state==m_langState[25])
                    {
                        res = processPWD(inst);
                    }
                    else if(lItem->state==m_langState[26])
                    {
                        res = processLIST(inst);
                    }
                    else if(lItem->state==m_langState[27])
                    {
                        res = processNLST(inst);
                    }
                    else if(lItem->state==m_langState[28])
                    {
                        res = processSITE(inst);
                    }
                    else if(lItem->state==m_langState[29])
                    {
                        res = processSYST(inst);
                    }
                    else if(lItem->state==m_langState[30])
                    {
                        res = processSTAT(inst);
                    }
                    else if(lItem->state==m_langState[31])
                    {
                        res = processHELP(inst);
                    }
                    else if(lItem->state==m_langState[32])
                    {
                        res = processNOOP(inst);
                    }
                    else
                    {
                        res = processUnknown(inst);
                    }
                    m_lastCommand = QString::fromUtf8(inst.mid(lItem->start + lItem->length).getString());                    
                }
                else
                {
                    res = processUnknown(inst);
                }
                if(!res)
                {
                    m_pstate = 3;
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        if(m_transfers.isEmpty() && (writeQueueSize()==0 || (state() & Socket::c_socketStateClose)))
        {
            close();
            return false;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processUSER(const common::BString& cmd)
{
    common::BString user,resp;
    
    if(m_pstate==0 || m_pstate==1)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[0])
        {
            user = cmd.mid(lItem->start + lItem->length).trim();
            if(!user.isEmpty())
            {
                m_username = QString::fromUtf8(user.getString());
                resp = "331 User name okay, need password.";
                m_pstate = 1;
            }
            else
            {
                resp = "501 No user name given.";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "230 User logged in proceed.";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processPASS(const common::BString& cmd)
{
    common::BString pass,resp;
    bool res = true;
    
    if(m_pstate==1)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[1])
        {
            pass = cmd.mid(lItem->start + lItem->length).trim();
            m_password = QString::fromUtf8(pass.getString());
            
            if(config().isUser(m_username,m_password))
            {
                resp = "230 User logged in, proceed.";
                setDefault();
                m_pstate = 2;
            }
            else
            {
                resp = "421 Unknown user";
                m_pstate = 0;
                res = false;
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        if(m_pstate>1)
        {
            resp = "230 User logged in, proceed.";
        }
        else
        {
            resp = "503 No user name has been given.";
        }
    }
    if(!writeResponse(resp))
    {
        return false;
    }
    return res;    
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processACCT(const common::BString& cmd)
{
    common::BString resp;
    resp = "202 Command not implemented, superfluous at this site.";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processCWD(const common::BString& cmd)
{
    QString nPath,cPath;
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[3])
        {
            cPath = QString::fromUtf8(cmd.mid(lItem->start + lItem->length).trim().getString());
            nPath = changeDirectoryPath(m_path,cPath);
            if(!nPath.isEmpty())
            {
                m_path = nPath;
                resp = "250 Changed directory successfully.";
            }
            else
            {
                resp = "550 Could not find directory.";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processCDUP(const common::BString& cmd)
{
    QString nPath,cPath;
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[4])
        {
            cPath = "..";
            nPath = changeDirectoryPath(m_path,cPath);
            if(!nPath.isEmpty())
            {
                m_path = nPath;
                resp = "250 Changed directory successfully.";
            }
            else
            {
                resp = "550 Could not find directory.";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processSMNT(const common::BString& cmd)
{
    common::BString resp;
    resp = "202 Command not implemented, superfluous at this site.";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processREIN(const common::BString& cmd)
{
    common::BString resp;
    bool res = true;
    
    if(m_pstate==2)
    {
        setDefault();
        resp = "220 Service ready for new user.";
    }
    else
    {
        resp = "421 Not logged in.";
        res = false;
    }
    if(!writeResponse(resp))
    {
        return false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processQUIT(const common::BString& cmd)
{
    common::BString resp;
    resp = "221 Goodbye.";
    m_pstate = 3;
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processPORT(const common::BString& cmd)
{
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[8])
        {
            tint i,start,state,no;
            const tchar *x = static_cast<const tchar *>(cmd);
            common::BString h1,h2,h3,h4,p1,p2,ipHost;
            
            i = lItem->start + lItem->length;
            start = i;
            state = 0;
            no = 0;
            
            while(i<=cmd.length() && no<6)
            {
                if(i<cmd.length() && (x[i]>='0' && x[i]<='9'))
                {
                    if(state==0)
                    {
                        start = i;
                        state = 1;
                    }
                }
                else
                {
                    if(state==1)
                    {
                        common::BString num;
                        
                        if(i<cmd.length())
                        {
                            num = cmd.mid(start,i-start);
                        }
                        else
                        {
                            num = cmd.mid(start);
                        }
                        
                        switch(no)
                        {
                            case 0:
                                h1 = num;
                                break;
                            case 1:
                                h2 = num;
                                break;
                            case 2:
                                h3 = num;
                                break;
                            case 3:
                                h4 = num;
                                break;
                            case 4:
                                p1 = num;
                                break;
                            case 5:
                                p2 = num;
                                break;
                        }
                        
                        state = 0;
                        no++;
                    }
                }
                i++;
            }
            
            ipHost = h1 + "." + h2 + "." + h3 + "." + h4;
            if(Resource::instance().isIPAddress(QString::fromLatin1(ipHost.getString())))
            {
                if(m_nextTransfer!=0)
                {
                    delete m_nextTransfer;
                    m_nextTransfer = 0;
                }
                
                m_activeFlag = true;
                m_activeHost = QString::fromLatin1(ipHost.getString());
                m_activePort = ((p1.Atoi() << 8) & 0x0000ff00) + (p2.Atoi() & 0x000000ff);
                resp = "200 Command okay.";
            }
            else
            {
                resp = "501 Syntax error in parameters or arguements.";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processPASV(const common::BString& cmd)
{
    common::BString resp;
    bool res = true;

    if(m_pstate==2)
    {
        tuint32 lIP = Resource::instance().localIP(&m_address);
        struct sockaddr_in ipAddr;
        common::BString sIP;
        QString ipStr;
            
        Resource::instance().setAddressIP(htonl(lIP),&ipAddr);
        ipStr = Resource::instance().networkIPAddressToString(ipAddr.sin_addr);
        sIP = ipStr.toLatin1().constData();
        
        if(Resource::instance().isIPAddress(QString::fromLatin1(sIP.getString())))
        {    
            tint port;
                
            port = config().passivePort();
            if(m_transfers.size()>0)
            {
                QList<FTPTransfer *>::iterator ppI;
                
                for(ppI=m_transfers.begin();ppI!=m_transfers.end();ppI++)
                {
                    const FTPTransfer *trans = *ppI;
                    tint p = trans->port();
                    
                    if(p!=0)
                    {
                        port = p + 1;
                    }
                }
            }
            
            if(m_nextTransfer!=0)
            {
                delete m_nextTransfer;
            }
            m_nextTransfer = new FTPTransfer(reinterpret_cast<FTPService *>(m_server->service()),m_server);
            if(m_nextTransfer->setPassive(port))
            {
                tint i;
                tchar *x = sIP.getString();
            
                for(i=0;i<sIP.length();i++)
                {
                    if(x[i]=='.')
                    {
                        x[i] = ',';
                    }
                }
                resp  = "227 Entering Passive Move (" + sIP + ",";
                resp += common::BString::Int((port >> 8) & 0x000000ff) + "," + common::BString::Int(port & 0x000000ff) + ")";
                m_activeFlag = false;
            }
            else
            {
                resp = "421 Failed to create data port to listen on.";
                res = false;
            }
        }
        else
        {
            resp = "421 Failed to get local network address.";
            res = false;
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    if(!writeResponse(resp))
    {
        return false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processTYPE(const common::BString& cmd)
{
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[10])
        {
            common::BString typeS(cmd.mid(lItem->start + lItem->length).trim().ucase());
            
            if(!typeS.isEmpty())
            {
                if(typeS[0]=='A')
                {
                    resp = "200 TYPE is now 8-bit ASCII";
                }
                else if(typeS[0]=='I')
                {
                    resp = "200 TYPE is now 8-bit binary";
                }
                else
                {
                    resp = "504 Command not implemented for that parameter";
                }
            }
            else
            {
                resp = "200 TYPE is now 8-bit binary";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processSTRU(const common::BString& cmd)
{
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[11])
        {
            common::BString typeS(cmd.mid(lItem->start + lItem->length).trim().ucase());
            
            if(typeS.isEmpty())
            {
                if(typeS[0]=='F')
                {
                    resp = "200 Structure set to file";
                }
                else
                {
                    resp = "504 Command not implemented for that parameter";
                }
            }
            else
            {
                resp = "200 Structure set to file";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processMODE(const common::BString& cmd)
{
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[12])
        {
            common::BString typeS(cmd.mid(lItem->start + lItem->length).trim().ucase());
            
            if(typeS.isEmpty())
            {
                if(typeS[0]=='S')
                {
                    resp = "200 MODE is stream";
                }
                else
                {
                    resp = "504 Command not implemented for that parameter";
                }
            }
            else
            {
                resp = "200 MODE is stream";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    return writeResponse(resp);    
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processRETR(const common::BString& cmd)
{
    common::BString resp;
    bool res = true;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[13])
        {
            QString cmdName,fileName;
            
            cmdName = QString::fromUtf8(static_cast<const char *>(cmd.mid(lItem->start + lItem->length).trim()));
            fileName = changeDirectoryPath(m_path,cmdName,true);
            if(!fileName.isEmpty() && fileExist(fileName))
            {
                FTPTransfer *trans = setupTransfer();
                
                if(trans!=0)
                {
                    if(trans->setDownload(fileName))
                    {
                        if(m_transfers.size()<5)
                        {
                            resp = "150 Accepted data connection";
                            m_transfers.append(trans);
                        }
                        else
                        {
                            resp = "450 Maximum number of file transactions reached";
                            delete trans;
                        }
                    }
                    else
                    {
                        resp = "450 Requested file action not taken. Cannot open file ";
                        resp += cmdName.toUtf8().constData();
                        resp += " to read";
                        delete trans;
                    }
                }
                else
                {
                    res = false;
                }
            }
            else
            {
                resp = "450 Requested file action not taken. Cannot find file ";
                resp += cmdName.toUtf8().constData();
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    if(!resp.isEmpty())
    {
        if(!writeResponse(resp))
        {
            return false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processSTOR(const common::BString& cmd)
{
    common::BString resp;
    bool res = true;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[14])
        {
            QString cmdName,fileName;
            
            cmdName = QString::fromUtf8(static_cast<const char *>(cmd.mid(lItem->start + lItem->length).trim()));
            fileName = changeDirectoryPath(m_path,cmdName,true,false);
            if(!fileName.isEmpty())
            {
                if(m_server->canFileBeUploaded(fileName))
                {
                    FTPTransfer *trans = setupTransfer();
                    if(trans!=0)
                    {
                        if(trans->setUpload(fileName))
                        {
                            if(m_transfers.size()<5)
                            {
                                resp = "150 Accepted data connection";
                                m_transfers.append(trans);
                            }
                            else
                            {
                                resp = "450 Maximum number of file transactions reached";
                                delete trans;
                            }
                        }
                        else
                        {
                            resp = "450 Requested file action not taken. Cannot open file ";
                            resp += cmdName.toUtf8().constData();
                            resp += " to read";
                            delete trans;
                        }
                    }
                    else
                    {
                        res = false;
                    }
                }
                else
                {
                    resp = "450 Cannot upload unsupported file type.";
                }
            }
            else
            {
                resp = "450 Requested file action not taken. Cannot path to file ";
                resp += cmdName.toUtf8().constData();
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    if(!resp.isEmpty())
    {
        if(!writeResponse(resp))
        {
            return false;
        }
    }
    return res;    
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processSTOU(const common::BString& cmd)
{
    common::BString resp;
    bool res = true;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[15])
        {
            QString cmdName,fileName;
            
            cmdName = QString::fromUtf8(static_cast<const char *>(cmd.mid(lItem->start + lItem->length).trim()));
            fileName = changeDirectoryPath(m_path,cmdName,true,false);
            if(!fileName.isEmpty())
            {
                FTPTransfer *trans = setupTransfer();
                
                if(trans!=0)
                {
                    fileName = getUniqueFilename(fileName);
                    if(trans->setUpload(fileName))
                    {
                        if(m_transfers.size()<5)
                        {
                            resp = "150 Accepted data connection";
                            m_transfers.append(trans);
                        }
                        else
                        {
                            resp = "450 Maximum number of file transactions reached";
                            delete trans;
                        }
                    }
                    else
                    {
                        resp = "450 Requested file action not taken. Cannot open file ";
                        resp += cmdName.toUtf8().constData();
                        resp += " to read";
                        delete trans;
                    }
                }
                else
                {
                    res = false;
                }
            }
            else
            {
                resp = "450 Requested file action not taken. Cannot path to file ";
                resp += cmdName.toUtf8().constData();
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    if(!resp.isEmpty())
    {
        if(!writeResponse(resp))
        {
            return false;
        }
    }
    return res;    
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processAPPE(const common::BString& cmd)
{
    common::BString resp;
    bool res = true;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[16])
        {
            QString cmdName,fileName;
            
            cmdName = QString::fromUtf8(static_cast<const char *>(cmd.mid(lItem->start + lItem->length).trim()));
            fileName = changeDirectoryPath(m_path,cmdName,true);
            if(!fileName.isEmpty())
            {
                FTPTransfer *trans = setupTransfer();
                if(trans!=0)
                {
                    trans->setAppend(true);
                    if(trans->setUpload(fileName))
                    {
                        if(m_transfers.size()<5)
                        {
                            resp = "150 Accepted data connection";
                            m_transfers.append(trans);
                        }
                        else
                        {
                            resp = "450 Maximum number of file transactions reached";
                            delete trans;
                        }
                    }
                    else
                    {
                        resp = "450 Requested file action not taken. Cannot open file ";
                        resp += cmdName.toUtf8().constData();
                        resp += " to read";
                        delete trans;
                    }
                }
                else
                {
                    res = false;
                }
            }
            else
            {
                resp = "450 Requested file action not taken. Cannot path to file ";
                resp += cmdName.toUtf8().constData();
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    if(!resp.isEmpty())
    {
        if(!writeResponse(resp))
        {
            return false;
        }
    }
    return res;    
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processALLO(const common::BString& cmd)
{
    common::BString resp;
    resp = "200 Command okay.";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processREST(const common::BString& cmd)
{
    common::BString resp;
    resp = "502 Command not implemented.";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processRNFR(const common::BString& cmd)
{
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[19])
        {
            QString nPath;
            
            nPath = QString::fromUtf8(cmd.mid(lItem->start + lItem->length).trim().getString());
            nPath = changeDirectoryPath(m_path,nPath,true);
            if(!nPath.isEmpty())
            {
                if(common::DiskOps::exist(nPath))
                {
                    m_movePathName = nPath;
                    resp = "350 Requested file action pending further information.";
                }
                else
                {
                    resp = "550 Request file not found.";
                }
            }
            else
            {
                resp = "550 Request file not found.";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processRNTO(const common::BString& cmd)
{
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[20])
        {
            if(!m_movePathName.isEmpty())
            {
                QString nPath;
                
                nPath = QString::fromUtf8(cmd.mid(lItem->start + lItem->length).trim().getString());
                nPath = changeDirectoryPath(m_path,nPath,true,false);
                
                if(!nPath.isEmpty())
                {
#if defined(OMEGA_WIN32)
                    LPCWSTR wSource = reinterpret_cast<LPCWSTR>(m_movePathName.utf16());
                    LPCWSTR wDest = reinterpret_cast<LPCWSTR>(nPath.utf16());
                    if(::MoveFileW(wSource,wDest)!=0)
#elif defined(OMEGA_POSIX)
                    if(::rename(m_movePathName.toUtf8().constData(),nPath.toUtf8().constData())==0)
#endif
                    {
                        resp = "250 Successfully renamed file.";
                    }
                    else
                    {
                        resp = "450 Failed to move the requested file.";
                    }
                }
                else
                {
                    resp = "450 Failed to move the requested file.";
                }
            }
            else
            {
                resp = "503 No path name specified";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processABOR(const common::BString& cmd)
{
    common::BString resp;
    
    if(m_pstate==2)
    {
        QString lCmd = m_lastCommand.toUpper();
        
        if((lCmd=="RETR" ||
            lCmd=="STOR" ||
            lCmd=="STOU" ||
            lCmd=="APPE") && m_transfers.size()>0)
        {
            FTPTransfer *trans = m_transfers.takeLast();
            trans->close();
            delete trans;
            resp = "226 Successfully aborted previous command";
        }
        else
        {
            resp = "226 The previous command already complete";
        }
    }
    else
    {
        resp = "500 Not logged in";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processDELE(const common::BString& cmd)
{
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[22])
        {
            QString cmdName,fileName;
            
            cmdName = QString::fromUtf8(static_cast<const char *>(cmd.mid(lItem->start + lItem->length).trim()));
            fileName = changeDirectoryPath(m_path,cmdName,true);
            
            if(!fileName.isEmpty())
            {
                processFileDeletion(fileName);
                resp = "250 File deleted";
            }
            else
            {
                resp = "550 Could not find file to delete";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized";
        }
    }
    else
    {
        resp = "530 Not logged in";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processRMD(const common::BString& cmd)
{
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[23])
        {
            QString cmdName,fileName;
            
            cmdName = QString::fromUtf8(static_cast<const char *>(cmd.mid(lItem->start + lItem->length).trim()));
            fileName = changeDirectoryPath(m_path,cmdName,true);
            
            if(!fileName.isEmpty() && dirExist(fileName))
            {
                processFileDeletion(fileName);
                resp = "250 Successfully deleted directory";
            }
            else
            {
                resp = "550 Cannot find directory to remove";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized";
        }
    }
    else
    {
        resp = "530 Not logged in";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processMKD(const common::BString& cmd)
{
    common::BString resp;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[24])
        {
            QString cmdName,fileName;
            
            cmdName = QString::fromUtf8(static_cast<const char *>(cmd.mid(lItem->start + lItem->length).trim()));
            fileName = changeDirectoryPath(m_path,cmdName,true,false);
            
            if(!fileName.isEmpty())
            {
                if(common::DiskOps::path(fileName,true))
                {
                    fileName = changeDirectoryPath(m_path,cmdName);
                    resp = "257 \"";
                    resp += fileName.toUtf8().constData();
                    resp += "\" Directory successfully created";
                }
                else
                {
                    resp = "550 Error creating directory";
                }
            }
            else
            {
                resp = "550 Cannot find path to create";
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized";
        }
    }
    else
    {
        resp = "530 Not logged in";
    }
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processPWD(const common::BString& cmd)
{
    common::BString resp;
    resp = "257 \"";
    resp += m_path.toUtf8().constData();
    resp += "\"";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processLIST(const common::BString& cmd)
{
    common::BString resp;
    bool res = true;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[26])
        {
            QString cmdName,fileName;
            
            cmdName = QString::fromUtf8(static_cast<const char *>(cmd.mid(lItem->start + lItem->length).trim()));
            fileName = changeDirectoryPath(m_path,cmdName,true);
            if(!fileName.isEmpty())
            {
                FTPTransfer *trans = setupTransfer();
                
                if(trans!=0)
                {
                    if(trans->setDirectory(fileName))
                    {
                        if(m_transfers.size()<5)
                        {
                            resp = "150 Accepted data connection";
                            m_transfers.append(trans);
                        }
                        else
                        {
                            resp = "450 Maximum number of file transactions reached";
                            delete trans;
                        }
                    }
                    else
                    {
                        resp = "450 Requested file action not taken. Cannot open ";
                        resp += cmdName.toUtf8().constData();
                        resp += " to read";
                        delete trans;
                    }
                } 
                else
                {
                    res = false;
                }
            }
            else
            {
                resp = "450 Requested file action not taken. Cannot path to file ";
                resp += cmdName.toUtf8().constData();
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    if(!resp.isEmpty())
    {
        if(!writeResponse(resp))
        {
            return false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processNLST(const common::BString& cmd)
{
    common::BString resp;
    bool res = true;
    
    if(m_pstate==2)
    {
        common::BO_Parse_Unit *lItem = m_lang->Lexical(cmd.getString());
        
        if(lItem!=0 && lItem->state==m_langState[27])
        {
            QString cmdName,fileName;
            
            cmdName = QString::fromUtf8(static_cast<const char *>(cmd.mid(lItem->start + lItem->length).trim()));
            fileName = changeDirectoryPath(m_path,cmdName,true);
            if(!fileName.isEmpty())
            {
                FTPTransfer *trans = setupTransfer();
                
                if(trans!=0)
                {
                    trans->setNameList(true);
                    if(trans->setDirectory(fileName))
                    {
                        if(m_transfers.size()<5)
                        {
                            resp = "150 Accepted data connection";
                            m_transfers.append(trans);
                        }
                        else
                        {
                            resp = "450 Maximum number of file transactions reached";
                            delete trans;
                        }
                    }
                    else
                    {
                        resp = "450 Requested file action not taken. Cannot open ";
                        resp += cmdName.toUtf8().constData();
                        resp += " to read";
                        delete trans;
                    }
                } 
                else
                {
                    res = false;
                }
            }
            else
            {
                resp = "450 Requested file action not taken. Cannot path to file ";
                resp += cmdName.toUtf8().constData();
            }
        }
        else
        {
            resp = "500 Syntax error, command unrecognized.";
        }
    }
    else
    {
        resp = "530 Not logged in.";
    }
    if(!resp.isEmpty())
    {
        if(!writeResponse(resp))
        {
            return false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processSITE(const common::BString& cmd)
{
    common::BString resp;
    resp = "202 Command not implemented, superfluous at this site.";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processSYST(const common::BString& cmd)
{
    common::BString resp;
    resp = "215 UNIX Type: L8";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processSTAT(const common::BString& cmd)
{
    common::BString resp;
    resp = "502 Command not supported";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processHELP(const common::BString& cmd)
{
    common::BString resp;
    resp = "502 Command not supported";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processNOOP(const common::BString& cmd)
{
    common::BString resp;
    resp = "200 OK";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

bool FTPSession::processUnknown(const common::BString& cmd)
{
    common::BString resp;
    resp = "502 Command not supported";
    return writeResponse(resp);
}

//-------------------------------------------------------------------------------------------

void FTPSession::buildFileList(const QString& dirName, QStringList& fileList)
{
    common::DiskIFSPtr pDisk = common::DiskIF::instance();
    
    if(pDisk->isDirectory(dirName))
    {
        common::DiskIF::DirHandle h = pDisk->openDirectory(dirName);
        if(h != common::DiskIF::invalidDirectory())
        {
            QString name;
            QStringList dirList;
            
            while(name = pDisk->nextDirectoryEntry(h), !name.isEmpty())
            {
                QString fullName = common::DiskOps::mergeName(dirName, name);
                if(pDisk->isFile(fullName))
                {
                    fileList.append(fullName);
                }
                else if(pDisk->isDirectory(fullName))
                {
                    dirList.append(fullName);
                }
            }
            pDisk->closeDirectory(h);
            
            for(QStringList::iterator ppI = dirList.begin(); ppI != dirList.end(); ppI++)
            {
                buildFileList(*ppI, fileList);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void FTPSession::buildDirList(const QString& dirName, QStringList& dirList)
{
    common::DiskIFSPtr pDisk = common::DiskIF::instance();
    
    if(pDisk->isDirectory(dirName))
    {
        common::DiskIF::DirHandle h = pDisk->openDirectory(dirName);
        if(h != common::DiskIF::invalidDirectory())
        {
            QString name;
            QStringList dList;
            
            while(name = pDisk->nextDirectoryEntry(h), !name.isEmpty())
            {
                QString fullName = common::DiskOps::mergeName(dirName, name);
                if(pDisk->isDirectory(fullName))
                {
                    dList.append(fullName);
                }
            }
            pDisk->closeDirectory(h);
            
            for(QStringList::iterator ppI = dirList.begin(); ppI != dirList.end(); ppI++)
            {
                buildDirList(*ppI, dirList);
            }
        }
        dirList.append(dirName);
    }
}

//-------------------------------------------------------------------------------------------

void FTPSession::processDirectoryDeletion(const QString& root)
{
    common::DiskIFSPtr pDisk = common::DiskIF::instance();
    
    if(pDisk->isDirectory(root))
    {
        QStringList fileList, dirList;
        
        buildFileList(root, fileList);
        for(QStringList::iterator ppI = fileList.begin(); ppI != fileList.end(); ppI++)
        {
            QString fileName = *ppI;
            m_server->signalRemoveFile(fileName);
            common::DiskOps::deleteFile(fileName);            
        }
        
        buildDirList(root, dirList);
        for(QStringList::iterator ppI = dirList.begin(); ppI != dirList.end(); ppI++)
        {
            QString dirName = *ppI;
            common::DiskOps::remove(dirName);
        }
    }
}

//-------------------------------------------------------------------------------------------

void FTPSession::processFileDeletion(const QString& fileName)
{
    common::DiskIFSPtr pDisk = common::DiskIF::instance();
    
    if(pDisk->isDirectory(fileName))
    {
        processDirectoryDeletion(fileName);
    }
    else if(pDisk->isFile(fileName))
    {
        m_server->signalRemoveFile(fileName);
        common::DiskOps::deleteFile(fileName);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace ftp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
