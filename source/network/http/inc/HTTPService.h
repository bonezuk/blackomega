//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_HTTP_HTTPSERVICE_H
#define __OMEGA_NETWORK_HTTP_HTTPSERVICE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPServerSocket.h"
#include "network/http/inc/HTTPServer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

class HTTPServiceEvent : public QEvent
{
    public:
    
        typedef enum
        {
            e_getServerEvent = QEvent::User + 1020,
            e_deleteServerEvent
        } HTTPServiceEventType;
        
    public:
        HTTPServiceEvent(HTTPServiceEventType t);
        
        Qt::HANDLE threadId();
        
        tint port() const;
        void port(tint x);
        
        HTTPServer *getServer();
        void setServer(HTTPServer *x);
        
    protected:
    
        Qt::HANDLE m_threadId;
         tint m_port;
        HTTPServer *m_server;
};

//-------------------------------------------------------------------------------------------

class HTTP_EXPORT HTTPService : public TCPService
{
    public:
        Q_OBJECT
        
    public:
        HTTPService(QObject *parent = 0);
        virtual ~HTTPService();
        
        HTTPServer *getServer(tint port = 80);
        
        void deleteServer(HTTPServer *svr);
        
    protected:
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual bool event(QEvent *e);
        virtual void processCustomEvent(HTTPServiceEvent *e,void *results);
        
        virtual HTTPServer *onGetServer(tint port);
        virtual void onDeleteServer(HTTPServer *svr);
};

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

