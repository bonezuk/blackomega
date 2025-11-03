#include "remote/inc/WinLIRCRemote.h"
#include "remote/inc/WinLIRCSettings.h"
#include "remote/inc/KeyControlService.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(RemoteIFFactory,RemoteIF, \
                            WinLIRCRemoteFactory,WinLIRCRemote, \
                            "winlirc_remote",false)

//-------------------------------------------------------------------------------------------

WinLIRCRemote::WinLIRCRemote(QObject *parent) : KeyCodeStateEngine(parent),
    m_service(),
    m_client(),
    m_status("No connection to LIRC server"),
    m_statusCode(0),
    m_isRemoteBeingAssignedTo(false)
{
    m_repeatTime = WinLIRCSettings::instance().repeatTime();
    m_delayTime  = WinLIRCSettings::instance().delayTime();
}

//-------------------------------------------------------------------------------------------

WinLIRCRemote::~WinLIRCRemote()
{}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "WinLIRCRemote::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool WinLIRCRemote::start()
{
    bool res = false;

    if(setupTimer())
    {
        network::Controller::ControllerSPtr networkCtrl = network::Controller::instance();

        if(networkCtrl.data()!=0)
        {
            if(KeyControlService::instance().data()!=0)
            {
                m_keyAssignment = KeyAssignment::instance();
                if(m_keyAssignment.data()!=0)
                {
                    m_service = dynamic_cast<WinLIRCService *>(networkCtrl->newService("winlirc"));
                    if(m_service!=0)
                    {
                        m_client = m_service->getClient();
                        if(m_client!=0)
                        {
                            QObject::connect(m_client,SIGNAL(remoteCommand(QString,int)),this,SLOT(remoteCommand(QString,int)));
                            QObject::connect(m_client,SIGNAL(remoteStatus(QString,int)),this,SLOT(remoteStatus(QString,int)));

                            m_client->connect(host(),port());
                            startTimer();
                            res = true;
                        }
                        else
                        {
                            printError("start","Failed to get WinLIRC connection client");
                        }
                    }
                    else
                    {
                        printError("start","Failed to start WinLIRC network service");
                    }
                }
                else
                {
                    printError("start","Failed to get key assignment map");
                }
            }
            else
            {
                printError("start","KeyboardControlService must be initialized before WinLIRCRemote service");
            }
        }
        else
        {
            printError("start","Error getting network service layer");
        }
    }
    else
    {
        printError("start","Error setting up timer");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::stop()
{
    network::Controller::ControllerSPtr networkCtrl = network::Controller::instance();

    if(networkCtrl.data()!=0)
    {
        if(m_service!=0)
        {
            if(m_client!=0)
            {
                QObject::disconnect(m_client,SIGNAL(remoteCommand(QString,int)),this,SLOT(remoteCommand(QString,int)));
                QObject::disconnect(m_client,SIGNAL(remoteStatus(QString,int)),this,SLOT(remoteStatus(QString,int)));

                m_client->shutdown();
                m_client = 0;
            }
            networkCtrl->deleteService(m_service);
        }
    }
    freeTimer();
}

//-------------------------------------------------------------------------------------------

QString WinLIRCRemote::host()
{
    return WinLIRCSettings::instance().host();
}

//-------------------------------------------------------------------------------------------

int WinLIRCRemote::port()
{
    return WinLIRCSettings::instance().port();
}

//-------------------------------------------------------------------------------------------

QString WinLIRCRemote::status(int& icon)
{
    icon = m_statusCode;
    return m_status;
}

//-------------------------------------------------------------------------------------------

bool WinLIRCRemote::isConnected()
{
    return m_client->isConnected();
}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::connect(const QString& host,int port)
{
    saveConnectionSettings(host,port);
    m_client->connect(host,port);
}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::saveConnectionSettings(const QString& host,int port)
{
    WinLIRCSettings::instance().setHost(host);
    WinLIRCSettings::instance().setPort(port);
}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::setRepeatTime(double rTime)
{
    WinLIRCSettings::instance().setRepeatTime(rTime);
    m_repeatTime = rTime;
}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::setDelayTime(double dTime)
{
    WinLIRCSettings::instance().setDelayTime(dTime);
    m_delayTime = dTime;
}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::startAssignmentToRemote()
{
    m_isRemoteBeingAssignedTo = true;
}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::endAssignmentToRemote()
{
    m_isRemoteBeingAssignedTo = false;
}

//-------------------------------------------------------------------------------------------

bool WinLIRCRemote::isRemoteBeingAssignedTo() const
{
    return m_isRemoteBeingAssignedTo;
}

//-------------------------------------------------------------------------------------------
void WinLIRCRemote::remoteEvent(RemoteEvent *e)
{}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::remoteCommand(QString cmd,int repeat)
{
    if(!isRemoteBeingAssignedTo())
    {
        KeyCode keyCode(cmd);
        processRemoteCommand(keyCode,repeat);
    }
    else
    {
        emitOnProgrammed(cmd,repeat);
    }
}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::emitOnProgrammed(const QString& cmd,int repeat)
{
    emit onProgrammed(cmd,repeat);
}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::onTimerImplementation()
{
    onProcessTimer();
}

//-------------------------------------------------------------------------------------------

void WinLIRCRemote::remoteStatus(QString text,int statusCode)
{
    m_status = text;
    m_statusCode = statusCode;
    emit onStatusUpdate(text,statusCode);
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------

