//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_WINLIRCSETTINGS_H
#define __OMEGA_REMOTE_WINLIRCSETTINGS_H
//-------------------------------------------------------------------------------------------

#include "remote/inc/KeyStateParser.h"

#include <QtCore/QString>
#include <QtCore/QSettings>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT WinLIRCSettings
{
    public:
        WinLIRCSettings();
        virtual ~WinLIRCSettings();

        static WinLIRCSettings& instance();

        virtual QString host();
        virtual void setHost(const QString& name);

        virtual int port();
        virtual void setPort(int portNum);

        virtual QString programCommands();
        virtual void setProgramCommands(const QString& cmdXml);

        virtual double repeatTime();
        virtual void setRepeatTime(double rTime);

        virtual double delayTime();
        virtual void setDelayTime(double dTime);

    protected:

        static WinLIRCSettings *m_instance;

        static void setInstance(WinLIRCSettings *ins);

        virtual double getTimeValue(const QString& name,double defaultTime);
        virtual void setTimeValue(const QString& name,double tTime);
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

