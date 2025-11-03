#include "playerapp/playercommon/inc/OmegaWebService.h"
#include "playerapp/playercommon/inc/OmegaPLWebInterface.h"

#include <QFileInfo>

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

OmegaWebService::OmegaWebService(const QString& rootDir, QObject *parent) : QObject(parent),
    m_rootDir(rootDir),
    m_webService(0),
    m_webServer(0),
    m_pWebInterface(),
    m_pWebEvents()
{
    QTimer::singleShot(10, this, SLOT(onStartService()));
    QObject::connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(onStopService()));
}

//-------------------------------------------------------------------------------------------

OmegaWebService::~OmegaWebService()
{}

//-------------------------------------------------------------------------------------------

void OmegaWebService::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "OmegaWebService::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool OmegaWebService::isValidSetup() const
{
    QFileInfo root(m_rootDir);
    
    if(!root.isDir())
    {
        QString err = QString("Given www content directory '%1' is not a directory").arg(m_rootDir);
        printError("isValidSetup", err.toUtf8().constData());
        return false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void OmegaWebService::onStartService()
{
    bool res = false;
    
    if(!isValidSetup())
    {
        printError("onStartService", "Invalid configuration, cannot start server");
        QCoreApplication::instance()->quit();
        return;
    }
    
    if(!setupWebEvents())
    {
        printError("onStartService", "No web events service");
        QCoreApplication::instance()->quit();
        return;    
    }
    
    try
    {
        network::Resource& nResource = network::Resource::instance();
        network::Controller::ControllerSPtr ctrl(network::Controller::instance());
        
        if(ctrl.data()!=0)
        {
            m_webService = dynamic_cast<network::http::HTTPService *>(ctrl->newService("http_server"));
            if(m_webService!=0)
            {
                m_webServer = m_webService->getServer(c_serverPort);
                if(m_webServer!=0)
                {
                    m_webServer->connect("/",this,SLOT(onWebRoot(network::http::HTTPReceive *)));
                    m_webServer->connect("/playlist",this,SLOT(onPlaylistAPI(network::http::HTTPReceive *)));
                    m_webServer->connect("/event",this,SLOT(onEventStream(network::http::HTTPReceive *)));
                    m_webServer->connect("/state",this,SLOT(onStateAPI(network::http::HTTPReceive *)));
                    
                    m_webServer->connect("/pressplay",this,SLOT(onPressPlay(network::http::HTTPReceive *)));
                    m_webServer->connect("/startplayback",this,SLOT(onStartPlayback(network::http::HTTPReceive *)));
                    
                    setupPLWebInterface();
                                        
                    QString successMsg = QString("Web service successfully started on port=%1").arg(c_serverPort);
                    common::Log::g_Log << successMsg << common::c_endl;
                    res = true;
                }
                else
                {
                    QString err = QString("Failed to start HTTP server on port %1").arg(c_serverPort);
                    printError("startService", err.toUtf8().constData());
                }
            }
            else
            {
                printError("startService", "Failed to instantiate HTTP service");
            }
        }
        else
        {
            printError("startService", "Failed to instantiate network controller");
        }    
    }
    catch(common::Exception&)
    {
        res = false;
    }
    
    if(!res)
    {
        QCoreApplication::instance()->quit();
    }
}

//-------------------------------------------------------------------------------------------

void OmegaWebService::onStopService()
{
    stopEvents();
    
    network::Controller::ControllerSPtr ctrl(network::Controller::instance());
    if(m_webService!=0)
    {
        if(m_webServer!=0)
        {
            m_webService->deleteServer(m_webServer);
            m_webServer = 0;
        }
        ctrl->deleteService(m_webService);
        m_webService = 0;
    }
    network::Controller::end();
    
    common::Log::g_Log << "Web service shut down" << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void OmegaWebService::onWebRoot(network::http::HTTPReceive *recieve)
{
    HTTPFileTransfer transfer(m_rootDir);
    transfer.process(recieve);
}

//-------------------------------------------------------------------------------------------

void OmegaWebService::postResponse(network::http::HTTPReceive *receive, int code)
{
    network::http::Unit hdr;
    
    hdr.response(code);
    hdr.add("Connection","close");
    receive->connection()->postResponse(hdr);
}

//-------------------------------------------------------------------------------------------

void OmegaWebService::onPlaylistAPI(network::http::HTTPReceive *receive)
{
    QJsonDocument doc;
    
    doc = m_pWebInterface->getFullPlaylist();
    if(doc.isArray())
    {
        network::http::Unit hdr;
        network::NetArraySPtr dataArray(new network::NetArray);                
        QByteArray arr;
        
        arr = doc.toJson(QJsonDocument::Indented);
        dataArray->SetSize(arr.size());
        ::memcpy(dataArray->GetData(), arr.data(), arr.size());
        
        hdr.response(200);
        hdr.add("Content-Type", "application/json");
        hdr.add("Content-Length", QString::number(arr.size()));
        hdr.add("Cache-Control", "no-cache");
        hdr.add("Connection","close");
        
        receive->connection()->postResponse(hdr);
        receive->connection()->postBody(dataArray);
    }
    else
    {
        printError("onPlaylistAPI", "Failed to get full playlist");
        postResponse(receive, 500);
    }
    receive->connection()->complete();
    receive->endProcess();
}

//-------------------------------------------------------------------------------------------

void OmegaWebService::onEventStream(network::http::HTTPReceive *receive)
{
    if(!m_pWebEvents.isNull())
    {
        m_pWebEvents->registerConnection(receive);
    }
}

//-------------------------------------------------------------------------------------------

void OmegaWebService::onStateAPI(network::http::HTTPReceive *receive)
{
    QJsonDocument doc;
    
    doc = m_pWebInterface->getPlaybackState();
    if(doc.isObject())
    {
        network::http::Unit hdr;
        network::NetArraySPtr dataArray(new network::NetArray);                
        QByteArray arr;
        
        arr = doc.toJson(QJsonDocument::Indented);
        dataArray->SetSize(arr.size());
        ::memcpy(dataArray->GetData(), arr.data(), arr.size());
        
        hdr.response(200);
        hdr.add("Content-Type", "application/json");
        hdr.add("Content-Length", QString::number(arr.size()));
        hdr.add("Cache-Control", "no-cache");
        hdr.add("Connection","close");
        
        receive->connection()->postResponse(hdr);
        receive->connection()->postBody(dataArray);
    }
    else
    {
        printError("onStateAPI", "Failed to get playback state");
        postResponse(receive, 500);
    }
    receive->connection()->complete();
    receive->endProcess();
}

//-------------------------------------------------------------------------------------------

void OmegaWebService::onPressPlay(network::http::HTTPReceive *receive)
{
    m_pWebInterface->onPressPlay();
    postResponse(receive, 200);
    receive->connection()->complete();
    receive->endProcess();
}

//-------------------------------------------------------------------------------------------

void OmegaWebService::onStartPlayback(network::http::HTTPReceive *receive)
{
    bool okFlag = true;
    QString idStr = receive->header().query().data("id");
    
    if(!idStr.isEmpty())
    {
        tuint64 id = static_cast<tuint64>(idStr.toULongLong(&okFlag));
        if(id > 0 && okFlag)
        {
            m_pWebInterface->onStartPlaying(id);
            postResponse(receive, 200);
        }
        else
        {
            postResponse(receive, 400);
        }
    }
    else
    {
        postResponse(receive, 400);
    }
    receive->connection()->complete();
    receive->endProcess();
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
