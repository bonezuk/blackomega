//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERCOMMON_OMEGAWEBSERVICE_H
#define __PLAYERAPP_PLAYERCOMMON_OMEGAWEBSERVICE_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"
#include "playerapp/playercommon/inc/HTTPFileTransfer.h"
#include "playerapp/playercommon/inc/OmegaPLWebInterface.h"
#include "playerapp/playercommon/inc/WebEventRegisterInterface.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT OmegaWebService : public QObject
{
    Q_OBJECT
    public:
        static const int c_serverPort = 5121;

    public:
        OmegaWebService(const QString& rootDir, QObject *parent = 0);
        virtual ~OmegaWebService();

    protected:
        QString m_rootDir;
        network::http::HTTPService *m_webService;
        network::http::HTTPServer *m_webServer;
        QSharedPointer<OmegaPLWebInterface> m_pWebInterface;
        QSharedPointer<WebEventRegisterInterface> m_pWebEvents;

        virtual void printError(const tchar *strR, const tchar *strE) const;
        virtual bool isValidSetup() const;
        virtual void postResponse(network::http::HTTPReceive *receive, int code);

        virtual bool setupWebEvents() = 0;
        virtual void setupPLWebInterface() = 0;
        virtual void stopEvents() = 0;

    protected slots:
        void onStartService();
        void onStopService();

        void onWebRoot(network::http::HTTPReceive *recieve);
        void onPlaylistAPI(network::http::HTTPReceive *receive);
        void onEventStream(network::http::HTTPReceive *receive);
        void onStateAPI(network::http::HTTPReceive *receive);
        void onPressPlay(network::http::HTTPReceive *receive);
        void onStartPlayback(network::http::HTTPReceive *receive);
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
