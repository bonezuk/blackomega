//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_DISCOGSSERVICETEST_H
#define __OMEGA_TRACK_MODEL_DISCOGSSERVICETEST_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "network/http/inc/Query.h"
#include "network/http/inc/Unit.h"
#include "common/inc/Log.h"

#include <QCoreApplication>
#include <QSslSocket>
#include <QTcpSocket>
#include <QTimer>
#include <QJsonDocument>

#if QT_VERSION >= 0x050000
#include <QRandomGenerator>
#endif

#include <time.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class DiscogsServiceTest : public QCoreApplication
{
    public:
        Q_OBJECT
    public:
        DiscogsServiceTest(int testNo, int& argc, char **argv);
        virtual ~DiscogsServiceTest();

    public slots:
        void onRequestToken();
        void onAccessToken();
        void onIdentity();
        void onReleaseBSG();

    private:
        void printError(const char *strR, const char *strE) const;
        void addUserAgent(network::http::Unit& req);
        QString generateNOnceToken();
        QString generateTimeStamp();
        bool readResponse(QSslSocket *socket, network::http::Unit& response);
        void requestHeaderForOAuthToken(network::http::Unit& req);
        void resourceHeaderForOAuth(network::http::Unit& req, const QString& url, const QString& token, const QString& tokenSecret);
        bool processOAuthResponseToken(QSslSocket *socket, QPair<QString,QString>& oAuthToken);
        void accessHeaderForOAuthToken(network::http::Unit& req, const QString& token, const QString& tokenSecret, const QString& verifier);
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
