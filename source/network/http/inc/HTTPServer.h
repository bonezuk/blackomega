//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_HTTP_HTTPSERVER_H
#define __OMEGA_NETWORK_HTTP_HTTPSERVER_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPServerSocket.h"
#include "network/inc/TCPService.h"
#include "network/http/inc/Unit.h"
#include "network/http/inc/Directory.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

class HTTPConnection;

//-------------------------------------------------------------------------------------------

class HTTP_EXPORT HTTPReceive : public QObject
{
    public:
        Q_OBJECT

    public:
        HTTPReceive(QObject *parent = 0);
        virtual ~HTTPReceive();
        
        const QString& url() const;
        void setUrl(const QString& x);
        
        Unit& header();
        const Unit& header() const;
        void setHeader(const Unit& x);
        
        NetArraySPtr body() const;
        void setBody(NetArraySPtr x);
        
        HTTPConnection *connection();
        const HTTPConnection *connection() const;
        void setConnection(HTTPConnection *x);
        
        const QString& host() const;
        const tint& port() const;
        const struct sockaddr_in *address() const;
        void setAddress(const QString& h,tint p,struct sockaddr_in *addr);
        
        void setLockFlag(bool v);
        void endProcess();

        void process();

        
    protected:
        
        QString m_host;
        tint m_port;
        struct sockaddr_in m_address;
        
        QString m_url;
        Unit m_header;
        NetArraySPtr m_body;
        HTTPConnection *m_connection;
        bool m_lockFlag;
        
    signals:
    
        void onReceive(network::http::HTTPReceive *);
};

//-------------------------------------------------------------------------------------------

class HTTP_EXPORT HTTPServer : public TCPServerSocket
{
    public:
        Q_OBJECT
        
    public:
        friend class HTTPConnection;
        
        class ResourceSlot
        {
            public:
                ResourceSlot();
            
                const QObject *m_receiver;
                common::BString m_method;
                Qt::ConnectionType m_type;
        };
        
    public:
        HTTPServer(Service *svr,QObject *parent = 0);
        virtual ~HTTPServer();
        
        virtual void connect(const QObject *receiver,const char *method,Qt::ConnectionType type = Qt::AutoConnection);
        virtual void connect(const QString& root,const QObject *receiver,const char *method,Qt::ConnectionType type = Qt::AutoConnection);
                
    protected:
        
        ResourceSlot *m_default;
        Directory<ResourceSlot *> m_resource;
        QMap<HTTPReceive *, HTTPConnection *> m_connectionReceiveMap;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual HTTPReceive *getReceive(const QString& url,HTTPConnection *conn);
        virtual void freeReceive(HTTPReceive *rec, bool delMap = true);
        void freeReceivers();
        
        virtual TCPConnServerSocket *newIO();
};

//-------------------------------------------------------------------------------------------

inline const QString& HTTPReceive::url() const
{
    return m_url;
}

//-------------------------------------------------------------------------------------------

inline void HTTPReceive::setUrl(const QString& x)
{
    m_url = x;
}

//-------------------------------------------------------------------------------------------

inline Unit& HTTPReceive::header()
{
    return m_header;
}

//-------------------------------------------------------------------------------------------

inline const Unit& HTTPReceive::header() const
{
    return m_header;
}

//-------------------------------------------------------------------------------------------

inline void HTTPReceive::setHeader(const Unit& x)
{
    m_header = x;
}

//-------------------------------------------------------------------------------------------

inline NetArraySPtr HTTPReceive::body() const
{
    return m_body;
}

//-------------------------------------------------------------------------------------------

inline void HTTPReceive::setBody(NetArraySPtr x)
{
    m_body = x;
}

//-------------------------------------------------------------------------------------------

inline HTTPConnection *HTTPReceive::connection()
{
    return m_connection;
}

//-------------------------------------------------------------------------------------------

inline const HTTPConnection *HTTPReceive::connection() const
{
    return m_connection;
}

//-------------------------------------------------------------------------------------------

inline void HTTPReceive::setConnection(HTTPConnection *x)
{
    m_connection = x;
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

