#include "playerapp/playerios/inc/PlayerUISettings.h"
#include "playerapp/playerios/inc/PlayerIOSTrackDBManager.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------
// PlayerFileFilter
//-------------------------------------------------------------------------------------------

PlayerFileFilter::PlayerFileFilter() : network::ftp::FTPFileFilter()
{}

//-------------------------------------------------------------------------------------------

PlayerFileFilter::~PlayerFileFilter()
{}

//-------------------------------------------------------------------------------------------

bool PlayerFileFilter::canFileBeUploaded(const QString& fileName)
{
    tint i;
    QString ext;
    bool res;

    for(i=fileName.length()-2;i>=0 && ext.isEmpty();--i)
    {
        if(fileName.at(i)==QChar('.'))
        {
            ext = fileName.mid(i + 1).toLower();
        }
    }
    if(ext=="mp3" || ext=="ogg" || ext=="wav" || ext=="m4a" || ext=="m4b" || ext=="flac" || ext=="aif" || ext=="aiff" || ext=="mpc" || ext=="mpp" || ext=="mp+" || ext=="ape")
    {
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// PlayerUISettings
//-------------------------------------------------------------------------------------------

PlayerUISettings::PlayerUISettings(QObject *parent) : QObject(parent),
    m_ftpService(0),
    m_ftpServer(0),
    m_ftpStatusMsg("FTP server is disabled"),
    m_models()
{}

//-------------------------------------------------------------------------------------------

PlayerUISettings::~PlayerUISettings()
{
    m_models.clear();
}

//-------------------------------------------------------------------------------------------

void PlayerUISettings::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "PlayerUISettings::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool PlayerUISettings::isFTPEnabled()
{
    return (m_ftpServer != 0) ? true : false;
}

//-------------------------------------------------------------------------------------------

void PlayerUISettings::enableFTPServer(bool enable)
{
    if(enable && m_ftpServer == 0)
    {
        startFTPServer();
        emit ftpServerChanged((m_ftpServer != 0) ? true : false);
    }
    else if(!enable && m_ftpServer != 0)
    {
        stopFTPServer();
        emit ftpServerChanged(false);
    }
}

//-------------------------------------------------------------------------------------------

QString PlayerUISettings::getFTPStatus()
{
    return m_ftpStatusMsg;
}

//-------------------------------------------------------------------------------------------

bool PlayerUISettings::startFTPServer()
{
    bool res = false;

    try
    {
        network::Resource& nResource = network::Resource::instance();
        network::Controller::ControllerSPtr ctrl(network::Controller::instance());

        if(ctrl.data()!=0)
        {
            m_ftpService = dynamic_cast<network::ftp::FTPService *>(ctrl->newService("ftp_server"));
            if(m_ftpService != 0)
            {
                m_ftpServer = m_ftpService->getServer(c_ftpPort);
                if(m_ftpServer != 0)
                {
                    PlayerFileFilter *filter = new PlayerFileFilter();

                    m_ftpServer->config().setRootPath(PlayerIOSUtils::appDataDirectory());
                    // m_ftpServer->config().setRootPath(PlayerIOSUtils::musicDirectory());

                    m_ftpServer->config().setPassivePort(c_ftpPassivePort);
                    m_ftpServer->setFileFilter(filter);
                    QObject::connect(m_ftpServer, SIGNAL(uploaded(const QString&)), PlayerIOSTrackDBManager::instance(), SLOT(addUploadedFile(const QString&)));
                    QObject::connect(m_ftpServer, SIGNAL(remove(const QString&)), PlayerIOSTrackDBManager::instance(), SLOT(onDeleteFile(const QString&)));
                    res = true;
                }
                else
                {
                    QString err = QString("Failed to start FTP server on port %1").arg(c_ftpPort);
                    printError("startFTPServer", err.toUtf8().constData());
                }
            }
            else
            {
                printError("startFTPServer", "Failed to get FTP service");
            }
        }
        else
        {
            printError("startFTPServer", "Failed to instantiate network controller");
        }

    }
    catch(const common::Exception& e)
    {
        QString err = QString("Exception thrown in starting FTP server. %1").arg(static_cast<const char *>(e.error()));
        printError("startFTPServer", err.toUtf8().constData());
        res = false;
    }

    if(res)
    {
        m_ftpStatusMsg = QString::fromUtf8("FTP server on port %1").arg(c_ftpPort);
    }
    else
    {
        m_ftpStatusMsg = QString::fromUtf8("Failed to start FTP server");
    }
    emit ftpStatusChanged(m_ftpStatusMsg);
    return res;
}

//-------------------------------------------------------------------------------------------

void PlayerUISettings::stopFTPServer()
{
    if(m_ftpService != 0)
    {
        network::Controller::ControllerSPtr ctrl(network::Controller::instance());

        if(m_ftpServer != 0)
        {
            QObject::disconnect(m_ftpServer, SIGNAL(uploaded(const QString&)), PlayerIOSTrackDBManager::instance(), SLOT(addUploadedFile(const QString&)));
            QObject::disconnect(m_ftpServer, SIGNAL(remove(const QString&)), PlayerIOSTrackDBManager::instance(), SLOT(onDeleteFile(const QString&)));
            m_ftpService->deleteServer(m_ftpServer);
            m_ftpServer = 0;
        }
        ctrl->deleteService(m_ftpService);
        m_ftpService = 0;
        m_ftpStatusMsg = QString::fromUtf8("FTP server is disabled");
        emit ftpStatusChanged(m_ftpStatusMsg);
    }
}

//-------------------------------------------------------------------------------------------

void PlayerUISettings::registerModel(QSharedPointer<QOmegaListModel> pModel)
{
    if(!pModel.isNull())
    {
        m_models.append(pModel);
    }
}

//-------------------------------------------------------------------------------------------

void PlayerUISettings::onRebuildDatabase()
{
    QList<QSharedPointer<QOmegaListModel> >::iterator ppI;

    for(ppI = m_models.begin(); ppI != m_models.end(); ppI++)
    {
        QSharedPointer<QOmegaListModel> pModel = *ppI;
        pModel->clear();
    }

    PlayerIOSTrackDBManager::instance()->rebuildDatabase();

    for(ppI = m_models.begin(); ppI != m_models.end(); ppI++)
    {
        QSharedPointer<QOmegaListModel> pModel = *ppI;
        pModel->reload();
    }
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
