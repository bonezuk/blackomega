//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_FTP_FTPSESSION_H
#define __OMEGA_NETWORK_FTP_FTPSESSION_H
//-------------------------------------------------------------------------------------------

#include "network/ftp/inc/FTPServer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace ftp
{
//-------------------------------------------------------------------------------------------

class FTP_EXPORT FTPSession : public TCPConnServerSocket
{
    public:
        Q_OBJECT

    public:
        FTPSession(FTPServer *svr,QObject *parent = 0);
        virtual ~FTPSession();

        virtual bool process();

    protected:

        FTPServer *m_server;

        tint m_pstate;
        QList<FTPTransfer *> m_transfers;

        QString m_username;
        QString m_password;

        QString m_path;

        bool m_activeFlag;
        QString m_activeHost;
        tint m_activePort;

        FTPTransfer *m_nextTransfer;

        common::BOParse *m_lang;
        tint m_langState[33];

        QString m_movePathName;
        QString m_lastCommand;

        FTPConfiguration& config();
        const FTPConfiguration& config() const;

        virtual void initParser();

        virtual bool dirExist(const QString& path);
        virtual bool fileExist(const QString& path);
        virtual QStringList getPathComponents(const QString& path);
        virtual QString buildPathFromComponents(const QStringList& pList);
        virtual QString buildLocalPath(const QStringList& pList);
        virtual QString getDirectoryPath(const QString& path);
        virtual QString getTransferPath(const QString& path);
        virtual QString changeDirectoryPath(const QString& path,const QString& cwd,bool sysFlag = false,bool chkFlag = true);
        virtual QString getUniqueFilename(const QString& name);

        virtual void setDefault();

        virtual bool writeResponse(const common::BString& resp);
        FTPTransfer *setupTransfer();

        // Section 4.1.1 - Access Control Commands
        virtual bool processUSER(const common::BString& cmd);
        virtual bool processPASS(const common::BString& cmd);
        virtual bool processACCT(const common::BString& cmd);
        virtual bool processCWD(const common::BString& cmd);
        virtual bool processCDUP(const common::BString& cmd);
        virtual bool processSMNT(const common::BString& cmd);
        virtual bool processREIN(const common::BString& cmd);
        virtual bool processQUIT(const common::BString& cmd);

        // Section 4.1.2 - Transfer Parameter Commands
        virtual bool processPORT(const common::BString& cmd);
        virtual bool processPASV(const common::BString& cmd);
        virtual bool processTYPE(const common::BString& cmd);
        virtual bool processSTRU(const common::BString& cmd);
        virtual bool processMODE(const common::BString& cmd);

        // Section 4.1.3 - FTP Service Commands
        virtual bool processRETR(const common::BString& cmd);
        virtual bool processSTOR(const common::BString& cmd);
        virtual bool processSTOU(const common::BString& cmd);
        virtual bool processAPPE(const common::BString& cmd);
        virtual bool processALLO(const common::BString& cmd);
        virtual bool processREST(const common::BString& cmd);
        virtual bool processRNFR(const common::BString& cmd);
        virtual bool processRNTO(const common::BString& cmd);
        virtual bool processABOR(const common::BString& cmd);
        virtual bool processDELE(const common::BString& cmd);
        virtual bool processRMD(const common::BString& cmd);
        virtual bool processMKD(const common::BString& cmd);
        virtual bool processPWD(const common::BString& cmd);
        virtual bool processLIST(const common::BString& cmd);
        virtual bool processNLST(const common::BString& cmd);
        virtual bool processSITE(const common::BString& cmd);
        virtual bool processSYST(const common::BString& cmd);
        virtual bool processSTAT(const common::BString& cmd);
        virtual bool processHELP(const common::BString& cmd);
        virtual bool processNOOP(const common::BString& cmd);
        virtual bool processUnknown(const common::BString& cmd);

        virtual void buildFileList(const QString& dirName, QStringList& fileList);
        virtual void buildDirList(const QString& dirName, QStringList& dirList);
        virtual void processDirectoryDeletion(const QString& root);
        virtual void processFileDeletion(const QString& fileName);
};

//-------------------------------------------------------------------------------------------
} // namespace ftp
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

