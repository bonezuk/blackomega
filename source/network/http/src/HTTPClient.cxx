#include "network/http/inc/HTTPClient.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------
// HTTPCTransaction
//-------------------------------------------------------------------------------------------

HTTPCTransaction::HTTPCTransaction(HTTPClient *client,tint id) : m_client(client),
    m_id(id),
    m_request(),
    m_requestData(),
    m_response(),
    m_responseData()
{}

//-------------------------------------------------------------------------------------------

HTTPCTransaction::HTTPCTransaction(const HTTPCTransaction& rhs) : m_client(0),
    m_id(0),
    m_request(),
    m_requestData(),
    m_response(),
    m_responseData()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

HTTPCTransaction::~HTTPCTransaction()
{}

//-------------------------------------------------------------------------------------------

const HTTPCTransaction& HTTPCTransaction::operator = (const HTTPCTransaction& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void HTTPCTransaction::copy(const HTTPCTransaction& rhs)
{
    m_client = rhs.m_client;
    m_id = rhs.m_id;
    m_request = rhs.m_request;
    m_response = rhs.m_response;
    m_requestData.Copy(rhs.m_requestData);
    m_responseData.Copy(rhs.m_responseData);
}

//-------------------------------------------------------------------------------------------

tint HTTPCTransaction::id() const
{
    return m_id;
}

//-------------------------------------------------------------------------------------------

Unit& HTTPCTransaction::request()
{
    return m_request;
}

//-------------------------------------------------------------------------------------------

const Unit& HTTPCTransaction::request() const
{
    return m_request;
}

//-------------------------------------------------------------------------------------------

NetArray& HTTPCTransaction::requestData()
{
    return m_requestData;
}

//-------------------------------------------------------------------------------------------

const NetArray& HTTPCTransaction::requestData() const
{
    return m_requestData;
}

//-------------------------------------------------------------------------------------------

Unit& HTTPCTransaction::response()
{
    return m_response;
}

//-------------------------------------------------------------------------------------------

const Unit& HTTPCTransaction::response() const
{
    return m_response;
}

//-------------------------------------------------------------------------------------------

NetArray& HTTPCTransaction::responseData()
{
    return m_responseData;
}

//-------------------------------------------------------------------------------------------

const NetArray& HTTPCTransaction::responseData() const
{
    return m_responseData;
}

//-------------------------------------------------------------------------------------------
// HTTPClient
//-------------------------------------------------------------------------------------------

HTTPClient::HTTPClient(Service *svr,QObject *parent) : TCPConnClientSocket(svr,parent),
    m_httpCService(0),
    m_transactions(),
    m_hostName(),
    m_hostPort(80),
    m_proxyName(),
    m_proxyPort(80),
    m_currentID(0),
    m_state(5),
    m_bodyOffset(0),
    m_bodyLength(0),
    m_chunkState(0),
    m_chunkLine(),
    m_chunkArray()
{
    m_threadId = QThread::currentThreadId();
}

//-------------------------------------------------------------------------------------------

HTTPClient::~HTTPClient()
{
    try
    {
        QList<HTTPCTransaction *>::iterator ppI;

        while(ppI=m_transactions.begin(),ppI!=m_transactions.end())
        {
            HTTPCTransaction *trans = *ppI;
            m_transactions.erase(ppI);
            delete trans;
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void HTTPClient::printError(const tchar *strR,const tchar *strE) const
{
#if defined(OMEGA_WIN32)
    tint err = static_cast<tint>(::WSAGetLastError());
#elif defined(OMEGA_POSIX)
    tint err = errno;
#endif
    Resource::instance().error("HTTPClient",strR,strE,err);
}

//-------------------------------------------------------------------------------------------

void HTTPClient::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("HTTPClient",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

void HTTPClient::setHost(const tchar *host,tint port)
{
    setHost(QString::fromLatin1(host),port);
}

//-------------------------------------------------------------------------------------------

void HTTPClient::setHost(const QString& host,tint port)
{
    m_hostName = host;
    m_hostPort = port;
}

//-------------------------------------------------------------------------------------------

void HTTPClient::setProxy(const tchar *host,tint port)
{
    setProxy(QString::fromLatin1(host),port);
}

//-------------------------------------------------------------------------------------------

void HTTPClient::setProxy(const QString& host,tint port)
{
    m_proxyName = host;
    m_proxyPort = port;
}

//-------------------------------------------------------------------------------------------

tint HTTPClient::newTransaction()
{
    tint id;
    HTTPCTransaction *trans;

    id = m_transactions.size();
    trans = new HTTPCTransaction(this,id);
    m_transactions.append(trans);
    return id;
}

//-------------------------------------------------------------------------------------------

HTTPCTransaction& HTTPClient::transaction(tint id)
{
    HTTPCTransaction *trans;

    trans = m_transactions.at(id);
    return *trans;
}

//-------------------------------------------------------------------------------------------

void HTTPClient::run()
{
    if(m_threadId!=QThread::currentThreadId())
    {
        QEvent *e = new QEvent(static_cast<QEvent::Type>(e_onHTTPClientRun));
        QCoreApplication::postEvent(this,e);
    }
    else
    {
        doRun();
    }
}

//-------------------------------------------------------------------------------------------

bool HTTPClient::event(QEvent *e)
{
    if(e!=0 && static_cast<HTTPClientEventType>(e->type())>=e_onHTTPClientRun)
    {
        HTTPClientEventType t = static_cast<HTTPClientEventType>(e->type());

        switch(t)
        {
            case e_onHTTPClientRun:
                doRun();
                break;

            default:
                return QObject::event(e);
        }
        return true;
    }
    else
    {
        return QObject::event(e);
    }
}

//-------------------------------------------------------------------------------------------

void HTTPClient::doRun()
{
    m_state = 0;
}

//-------------------------------------------------------------------------------------------

bool HTTPClient::process()
{
    bool loop = true,res = true;

    if(m_state >= 5)
    {
        return true;
    }

    if(isError())
    {
        QString err("Error in socket layer");
        emit onError(this,err);
        return false;
    }

    while(loop && res)
    {
        if(m_currentID < m_transactions.size())
        {
            switch(m_state)
            {
                case 0:
                    res = doConnect();
                    break;

                case 1:
                    doRequest();
                    break;

                case 2:
                    doResponse(loop);
                    break;

                case 3:
                    doResData(loop);
                    break;

                case 4:
                    doResChunked(loop);
                    break;
            }
        }
        else
        {
            loop = false;
            res = false;
        }
    }

    if(!res)
    {
        emit onComplete(this);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool HTTPClient::doConnect()
{
    QList<HTTPCTransaction *>::iterator ppI;
    QString cHost,err;
    bool res = true;

    if(m_hostPort==80)
    {
        cHost = m_hostName;
    }
    else
    {
        cHost = m_hostName + ":" + QString::number(m_hostPort);
    }

    for(ppI=m_transactions.begin();ppI!=m_transactions.end();++ppI)
    {
        HTTPCTransaction *trans = *ppI;

        trans->request().setVersion(1,1);
        trans->request().add("Host",cHost);
        if(trans==m_transactions.last())
        {
            trans->request().add("Connection","close");
        }
        else
        {
            trans->request().add("Connection","keep-alive");
        }
        if(!m_proxyName.isEmpty())
        {
            trans->request().setProxyConnection(m_hostName,m_hostPort);
        }
    }

    if(!m_proxyName.isEmpty())
    {
        res = open(m_proxyName,m_proxyPort);
    }
    else
    {
        res = open(m_hostName,m_hostPort);
    }

    if(res)
    {
        m_state = 1;
    }
    else
    {
        err = "Failed to connect to host '" + m_hostName + "'";
        emit onError(this,err);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void HTTPClient::doRequest()
{
    HTTPCTransaction *trans = m_transactions.at(m_currentID);
    Unit& req = trans->request();
    NetArray& reqData = trans->requestData();
    QString err;
    bool res = true;

    if(reqData.GetSize() > 0)
    {
        req.add("Content-Length",QString::number(reqData.GetSize()));
    }
    else
    {
        req.add("Content-Length","0");
    }

    QString reqString;
    req.print(reqString);
    QByteArray reqArr(reqString.toUtf8());
    if(write(reqArr.constData(),reqArr.length()))
    {
        if(reqData.GetSize() > 0)
        {
            if(write(reqData.GetData(),reqData.GetSize()))
            {
                m_state = 2;
            }
            else
            {
                err = "Error writing request data to host '" + m_hostName + "'";
                res = false;
            }
        }
        else
        {
            m_state = 2;
        }
    }
    else
    {
        err = "Error writing request to host '" + m_hostName + "'";
        res = false;
    }

    if(!res)
    {
        emit onTransactionError(trans,err);
        m_currentID++;
        m_state = 1;
    }
}

//-------------------------------------------------------------------------------------------

void HTTPClient::doResponse(bool& loop)
{
    HTTPCTransaction *trans = m_transactions.at(m_currentID);
    Unit& response = trans->response();
    QString line,err;
    bool loopF,end = false,res = true;

    do
    {
        loopF = false;

        while(!end && res)
        {
            if(canGetNextLine())
            {
                if(getNextLine(line))
                {
                    if(!response.insertLine(line,end))
                    {
                        err = "Failed to parse reponse line \"" + line + "\" from host '" + m_hostName + "'";
                        res = false;
                    }
                }
                else
                {
                    end = true;
                }
            }
            else
            {
                break;
            }
        }

        if(end && response.type()==Unit::e_Response && response.response()==100)
        {
            Unit blank;
            response = blank;
            end = false;
            loopF = true;
        }
    } while(loopF);

    if(end && res)
    {
        if(response.type()==Unit::e_Response)
        {
            if(isBody(response))
            {
                m_bodyOffset = 0;
                m_bodyLength = response.data("Content-Length").toInt();
                trans->responseData().SetSize(m_bodyLength);
                m_state = 3;
            }
            else if(isChunked(response))
            {
                m_bodyOffset = 0;
                m_state = 4;
            }
            else
            {
                emit onTransaction(trans);
                if(response.data("Connection").toLower().trimmed()=="close")
                {
                    m_currentID++;
                }
                m_state = 1;
            }
        }
        else
        {
            err = "Response expected from server '" + m_hostName + "'";
            res = false;
        }
    }
    else
    {
        loop = false;
    }

    if(!res)
    {
        emit onTransactionError(trans,err);
        m_currentID++;
        m_state = 1;
    }
}

//-------------------------------------------------------------------------------------------

bool HTTPClient::isBody(const Unit& item) const
{
    tint code = item.response();

    if(code>=200 && code!=204 && code!=304)
    {
        QString tType;

        tType = item.data("Transfer-Encoding").toLower();
        if(!tType.isEmpty() && tType!="identity")
        {
            return true;
        }

        if(item.data("Content-Length").toInt() > 0)
        {
            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

bool HTTPClient::isChunked(const Unit& item) const
{
    QString tType;

    tType = item.data("Transfer-Encoding").toLower();
    if(!tType.isEmpty() && tType!="identity")
    {
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------------------------

void HTTPClient::doResData(bool& loop)
{
    tint len,amount = m_bodyLength - m_bodyOffset;
    HTTPCTransaction *trans = m_transactions.at(m_currentID);
    NetArray& data = trans->responseData();
    tchar *x = reinterpret_cast<tchar *>(data.GetData());
    QString err;
    bool res = true;

    if(amount > 0)
    {
        len = available();
        if(len > 0)
        {
            amount = (len > amount) ? amount : len;
            len = read(&x[m_bodyOffset],amount);
            if(len>=0)
            {
                m_bodyOffset += len;
            }
            else
            {
                err = "Error reading data from input queue";
                res = false;
            }
        }
        else
        {
            loop = false;
        }
    }
    else
    {
        emit onTransaction(trans);
        m_currentID++;
        m_state = 1;
    }

    if(!res)
    {
        emit onTransactionError(trans,err);
        m_currentID++;
        m_state = 1;
    }
}

//-------------------------------------------------------------------------------------------

void HTTPClient::doResChunked(bool& loop)
{
    HTTPCTransaction *trans = m_transactions.at(m_currentID);
    QString err;
    bool res = true;

    switch(m_chunkState)
    {
        case 0:
            if(canGetNextLine())
            {
                QString line;

                if(getNextLine(line))
                {
                    m_bodyLength = 0;
                    if(parseChunkHeader(line,m_bodyLength,m_chunkLine))
                    {
                        if(m_bodyLength > 0)
                        {
                            m_bodyOffset = 0;
                            m_chunkState = 1;
                            m_chunkArray.SetSize(m_bodyLength);
                        }
                        else
                        {
                            m_chunkState = 2;
                        }
                    }
                    else
                    {
                        err = "Failed to parse chunk header";
                        res = false;
                    }
                }
                else
                {
                    loop = false;
                }
            }
            else
            {
                loop = false;
            }
            break;

        case 1:
            {
                tint len,amount = m_bodyLength - m_bodyOffset;
                tchar *x = reinterpret_cast<tchar *>(m_chunkArray.GetData());

                if(amount > 0)
                {
                    len = available();
                    if(len > 0)
                    {
                        amount = (len > amount) ? amount : len;
                        len = read(&x[m_bodyOffset],amount);
                        if(len >= 0)
                        {
                            m_bodyOffset += len;
                        }
                        else
                        {
                            err = "Error reading chunk data from input queue";
                            res = false;
                        }
                    }
                    else
                    {
                        loop = false;
                    }
                }
                else
                {
                    emit onTransactionChunk(trans,m_chunkLine,&m_chunkArray);
                    m_chunkState = 0;
                }
            }
            break;

        case 2:
            if(canGetNextLine())
            {
                QString line;

                if(getNextLine(line))
                {
                    m_currentID++;
                    m_state = 1;
                }
                else
                {
                    loop = false;
                }
            }
            else
            {
                loop = false;
            }
            break;
    }

    if(!res)
    {
        emit onTransactionError(trans,err);
        m_currentID++;
        m_state = 1;
    }
}

//-------------------------------------------------------------------------------------------

bool HTTPClient::parseChunkHeader(const QString& str,tint& size,QString& field)
{
    QByteArray arr(str.toUtf8());
    tint start = 0,i = 0,state = 0;
    const tchar *s = arr.constData();
    bool res = false;

    if(arr.length() > 0)
    {
        while(s[i]!='\0')
        {
            switch(state)
            {
                case 0:
                    if((s[i]>='0' && s[i]<='9') || (s[i]>='a' && s[i]<='f') || (s[i]>='A' && s[i]<='F'))
                    {
                        size = 0;
                        state = 1;
                        i--;
                    }
                    break;

                case 1:
                    if(s[i]>='0' && s[i]<='9')
                    {
                        size <<= 4;
                        size += static_cast<tint>(s[i] - '0');
                    }
                    else if(s[i]>='a' && s[i]<='f')
                    {
                        size <<= 4;
                        size += static_cast<tint>(s[i] - 'a') + 10;
                    }
                    else if(s[i]>='A' && s[i]<='F')
                    {
                        size <<= 4;
                        size += static_cast<tint>(s[i] - 'A') + 10;
                    }
                    else if(s[i]==';')
                    {
                        state = 3;
                        start = i + 1;
                    }
                    else
                    {
                        state = 2;
                    }
                    break;

                case 2:
                    if(s[i]==';')
                    {
                        state = 3;
                        start = i + 1;
                    }
                    break;

                default:
                    break;
            }
            i++;
        }

        if(state>=1)
        {
            if(state==3)
            {
                field = str.mid(start);
            }
            else
            {
                field = "";
            }
            res = true;
        }
        else
        {
            printError("parseChunkHeader","No chunk size found");
        }
    }
    else
    {
        printError("parseChunkHeader","Line has no content");
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
