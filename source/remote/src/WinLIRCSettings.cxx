#include "remote/inc/WinLIRCSettings.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

WinLIRCSettings *WinLIRCSettings::m_instance = 0;

//-------------------------------------------------------------------------------------------

WinLIRCSettings::WinLIRCSettings()
{}

//-------------------------------------------------------------------------------------------

WinLIRCSettings::~WinLIRCSettings()
{
    m_instance = 0;
}

//-------------------------------------------------------------------------------------------

WinLIRCSettings& WinLIRCSettings::instance()
{
    if(m_instance==0)
    {
        m_instance = new WinLIRCSettings;
    }
    return *m_instance;
}

//-------------------------------------------------------------------------------------------

void WinLIRCSettings::setInstance(WinLIRCSettings *ins)
{
    m_instance = ins;
}

//-------------------------------------------------------------------------------------------

QString WinLIRCSettings::host()
{
    QSettings settings;
    QString hostName;
    
    settings.beginGroup("remote");
    if(settings.contains("lircHost"))
    {
        hostName = settings.value("lircHost",QVariant("localhost")).toString();
    }
    else
    {
        hostName = "localhost";
    }
    settings.endGroup();
    return hostName;
}

//-------------------------------------------------------------------------------------------

void WinLIRCSettings::setHost(const QString& name)
{
    QSettings settings;
    settings.beginGroup("remote");
    settings.setValue("lircHost",QVariant(name));
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------

int WinLIRCSettings::port()
{
    QSettings settings;
    int portNumber;
    
    settings.beginGroup("remote");
    if(settings.contains("lircPort"))
    {
        portNumber = settings.value("lircPort",QVariant(8765)).toInt();
    }
    else
    {
        portNumber = 8765;
    }
    settings.endGroup();
    return portNumber;
}

//-------------------------------------------------------------------------------------------

void WinLIRCSettings::setPort(int portNum)
{
    QSettings settings;
    settings.beginGroup("remote");
    settings.setValue("lircPort",QVariant(portNum));
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------

QString WinLIRCSettings::programCommands()
{
    QSettings settings;
    QString cmdXML;
    
    settings.beginGroup("remote");
    if(settings.contains("lircCommands"))
    {
        cmdXML = settings.value("lircCommands",QVariant()).toString();
    }
    settings.endGroup();
    return cmdXML;
}

//-------------------------------------------------------------------------------------------

void WinLIRCSettings::setProgramCommands(const QString& cmdXml)
{
    if(cmdXml.isEmpty())
    {
        QSettings settings;
        settings.beginGroup("remote");
        settings.setValue("lircCommands",QVariant(cmdXml));
        settings.endGroup();
    }
}

//-------------------------------------------------------------------------------------------

double WinLIRCSettings::getTimeValue(const QString& name,double defaultTime)
{
    double tTime;
    QSettings settings;
    
    settings.beginGroup("remote");
    if(settings.contains(name))
    {
        tTime = settings.value(name,QVariant(defaultTime)).toDouble();
    }
    else
    {
        tTime = defaultTime;
    }
    return tTime;
}

//-------------------------------------------------------------------------------------------

void WinLIRCSettings::setTimeValue(const QString& name,double tTime)
{
    QSettings settings;
    settings.beginGroup("remote");
    settings.setValue(name,QVariant(tTime));
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------

double WinLIRCSettings::repeatTime()
{
    return getTimeValue("lircRepeatTime",0.5);
}

//-------------------------------------------------------------------------------------------

void WinLIRCSettings::setRepeatTime(double rTime)
{
    setTimeValue("lircRepeatName",rTime);
}

//-------------------------------------------------------------------------------------------

double WinLIRCSettings::delayTime()
{
    return getTimeValue("lircDelayTime",0.5);
}

//-------------------------------------------------------------------------------------------

void WinLIRCSettings::setDelayTime(double dTime)
{
    setTimeValue("lircDelayTime",dTime);
}


//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
