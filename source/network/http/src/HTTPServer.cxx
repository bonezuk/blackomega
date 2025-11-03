#include "network/http/inc/HTTPServer.h"
#include "network/http/inc/HTTPConnection.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------
// HTTPReceive
//-------------------------------------------------------------------------------------------

HTTPReceive::HTTPReceive(QObject *parent) : QObject(parent),
    m_host(),
    m_port(0),
    m_header(),
    m_body(),
    m_connection(0),
    m_lockFlag(false)
{
    ::memset(&m_address,0,sizeof(struct sockaddr_in));
}

//-------------------------------------------------------------------------------------------

HTTPReceive::~HTTPReceive()
{}

//-------------------------------------------------------------------------------------------

void HTTPReceive::process()
{
    Q_EMIT onReceive(this);
}

//-------------------------------------------------------------------------------------------

const QString& HTTPReceive::host() const
{
    return m_host;
}

//-------------------------------------------------------------------------------------------

const tint& HTTPReceive::port() const
{
    return m_port;
}

//-------------------------------------------------------------------------------------------

const struct sockaddr_in *HTTPReceive::address() const
{
    return &m_address;
}

//-------------------------------------------------------------------------------------------

void HTTPReceive::setAddress(const QString& h,tint p,struct sockaddr_in *addr)
{
    m_host = h;
    m_port = p;
    ::memcpy(&m_address,addr,sizeof(struct sockaddr_in));
}

//-------------------------------------------------------------------------------------------

void HTTPReceive::endProcess()
{
    if(m_lockFlag)
    {
        m_connection->unlock();
    }
}

//-------------------------------------------------------------------------------------------

void HTTPReceive::setLockFlag(bool v)
{
    m_lockFlag = v;
}

//-------------------------------------------------------------------------------------------
// HTTPServer::ResourceSlot
//-------------------------------------------------------------------------------------------

HTTPServer::ResourceSlot::ResourceSlot() : m_receiver(0),
    m_method(),
    m_type(Qt::AutoConnection)
{}

//-------------------------------------------------------------------------------------------
// HTTPServer
//-------------------------------------------------------------------------------------------

HTTPServer::HTTPServer(Service *svr,QObject *parent) : TCPServerSocket(svr,parent),
    m_default(0),
    m_resource(),
    m_connectionReceiveMap()
{}

//-------------------------------------------------------------------------------------------

HTTPServer::~HTTPServer()
{
    HTTPServer::freeReceivers();
}

//-------------------------------------------------------------------------------------------

void HTTPServer::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "HTTPServer::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void HTTPServer::connect(const QObject *receiver,const char *method,Qt::ConnectionType type)
{
    QString root;
    connect(root,receiver,method,type);
}

//-------------------------------------------------------------------------------------------

void HTTPServer::connect(const QString& root,const QObject *receiver,const char *method,Qt::ConnectionType type)
{
    ResourceSlot *rSlot = new ResourceSlot;

    rSlot->m_receiver = receiver;
    rSlot->m_method = method;
    rSlot->m_type = type;

    if(root.isEmpty())
    {
        m_default = rSlot;
    }
    else
    {
        m_resource.add(root,rSlot);
    }
}

//-------------------------------------------------------------------------------------------

HTTPReceive *HTTPServer::getReceive(const QString& url,HTTPConnection *conn)
{
    QString tail;
    ResourceSlot *rSlot = m_resource.get(url,tail);
    HTTPReceive *rcv;

    if(rSlot==0)
    {
        rSlot = m_default;
    }
    if(rSlot!=0)
    {
        rcv = new HTTPReceive;
        rcv->setUrl(url);
        rcv->setConnection(conn);
        if(rSlot->m_receiver->thread()!=QThread::currentThread())
        {
            rcv->setLockFlag(true);
            rcv->connection()->lock();
        }
        QObject::connect(rcv,SIGNAL(onReceive(network::http::HTTPReceive *)),rSlot->m_receiver,static_cast<const tchar *>(rSlot->m_method),rSlot->m_type);

        m_connectionReceiveMap.insert(rcv, conn);
    }
    else
    {
        rcv = 0;
    }
    return rcv;
}

//-------------------------------------------------------------------------------------------

void HTTPServer::freeReceive(HTTPReceive *rec, bool delMap)
{
    if(rec!=0)
    {
        QString tail;
        QMap<HTTPReceive *, HTTPConnection *>::iterator ppI;
        ResourceSlot *rSlot = m_resource.get(rec->url(),tail);

        if(delMap)
        {
            ppI = m_connectionReceiveMap.find(rec);
            if(ppI != m_connectionReceiveMap.end())
            {
                m_connectionReceiveMap.erase(ppI);
            }
        }

        if(rSlot!=0)
        {
            rSlot = m_default;
        }
        if(rSlot!=0)
        {
            QObject::disconnect(rec,SIGNAL(onReceive(network::http::HTTPReceive *)),rSlot->m_receiver,static_cast<const tchar *>(rSlot->m_method));
        }
        delete rec;
    }
}

//-------------------------------------------------------------------------------------------

void HTTPServer::freeReceivers()
{
    QMap<HTTPReceive *, HTTPConnection *>::iterator ppI;

    for(ppI = m_connectionReceiveMap.begin(); ppI != m_connectionReceiveMap.end(); ppI++)
    {
        ppI.value()->releaseReceiver();
        HTTPServer::freeReceive(ppI.key(), false);
    }
    m_connectionReceiveMap.clear();
}

//-------------------------------------------------------------------------------------------

TCPConnServerSocket *HTTPServer::newIO()
{
    HTTPConnection *io = new HTTPConnection(this,m_service,this);
    return reinterpret_cast<TCPConnServerSocket *>(io);
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
