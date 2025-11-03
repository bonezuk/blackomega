//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_WINLIRCREMOTE_H
#define __OMEGA_REMOTE_WINLIRCREMOTE_H
//-------------------------------------------------------------------------------------------

#include "remote/inc/WinLIRCClient.h"
#include "common/inc/TimeStamp.h"
#include "network/inc/Controller.h"
#include "remote/inc/KeyCodeStateEngine.h"

#include <QtCore/QSettings>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT WinLIRCRemote : public KeyCodeStateEngine
{
    public:
        Q_OBJECT

    public:
        WinLIRCRemote(QObject *parent = 0);
        virtual ~WinLIRCRemote();

        virtual bool start();
        virtual void stop();

        virtual void remoteEvent(RemoteEvent *e);

        virtual QString host();
        virtual int port();
        virtual QString status(int& icon);

        virtual bool isConnected();
        virtual void connect(const QString& host,int port);

        virtual void startAssignmentToRemote();
        virtual void endAssignmentToRemote();
        virtual bool isRemoteBeingAssignedTo() const;

        virtual void setDelayTime(double dTime);
        virtual void setRepeatTime(double rTime);

    protected:

        WinLIRCService *m_service;
        WinLIRCClient *m_client;
        QString m_status;
        int m_statusCode;
        bool m_isRemoteBeingAssignedTo;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual void saveConnectionSettings(const QString& host,int port);

        virtual void onTimerImplementation();

        virtual void emitOnProgrammed(const QString& cmd,int repeat);

    protected slots:

        void remoteCommand(QString cmd,int repeat); //
        void remoteStatus(QString text,int statusCode);

    signals:

        void onConnect(bool isConnected);
        void onProgrammed(QString button,int repeat);
        void onStatusUpdate(QString text,int statusCode);
};

typedef QSharedPointer<WinLIRCRemote> WinLIRCRemoteSPtr;

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

