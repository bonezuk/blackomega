#include "network/http/inc/HTTPConnection.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------
// HTTPConnection::Message
//-------------------------------------------------------------------------------------------

HTTPConnection::Message::Message(MessageType t) : m_type(t),
    m_str(),
    m_header(),
    m_body()
{}

//-------------------------------------------------------------------------------------------

HTTPConnection::MessageType HTTPConnection::Message::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

const QString& HTTPConnection::Message::string() const
{
    return m_str;
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::Message::setString(const QString& x)
{
    m_str = x;
}

//-------------------------------------------------------------------------------------------

const Unit& HTTPConnection::Message::header() const
{
    return m_header;
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::Message::setHeader(const Unit& x)
{
    m_header = x;
}

//-------------------------------------------------------------------------------------------

NetArraySPtr HTTPConnection::Message::body() const
{
    return m_body;
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::Message::setBody(NetArraySPtr x)
{
    m_body = x;
}

//-------------------------------------------------------------------------------------------
// HTTPConnection
//-------------------------------------------------------------------------------------------

const tint c_HTTPConnection_MaxPostSize = 10000000;

//-------------------------------------------------------------------------------------------

HTTPConnection::HTTPConnection(HTTPServer *server,Service *svr,QObject *parent) : TCPConnServerSocket(svr,parent),
    m_server(server),
    m_mutex(),
    m_processList(),
    m_state(e_StartSession),
    m_persistent(true),
    m_chunked(false),
    m_request(),
    m_requestBody(),
    m_requestBodyLength(0),
    m_response(),
    m_receiver(0),
    m_process(true)
{}

//-------------------------------------------------------------------------------------------

HTTPConnection::~HTTPConnection()
{
    try
    {
        resetState();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::printError(const tchar *strR,const tchar *strE) const
{
#if defined(OMEGA_WIN32)
    tint err = static_cast<tint>(::WSAGetLastError());
#elif defined(OMEGA_POSIX)
    tint err = errno;
#endif
    Resource::instance().error("HTTPConnection",strR,strE,err);
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::printError(const tchar *strR,const tchar *strE,tint eNo) const
{
    Resource::instance().error("HTTPConnection",strR,strE,eNo);
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::postResponse(const Unit& x)
{
    Message *msg = new Message(e_PostHeader);
    msg->setHeader(x);
    m_mutex.lock();
    m_processList.append(msg);
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::postBody(NetArraySPtr x)
{
    Message *msg = new Message(e_PostBody);
    msg->setBody(x);
    m_mutex.lock();
    m_processList.append(msg);
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::postChunk(NetArraySPtr mem)
{
    QString str;
    postChunk(str, mem);
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::postChunk(const QString& str,NetArraySPtr mem)
{
    Message *msg = new Message(e_PostChunk);
    msg->setString(str);
    msg->setBody(mem);
    m_mutex.lock();
    m_processList.append(msg);
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::complete()
{
    Message *msg = new Message(e_Complete);
    m_mutex.lock();
    m_processList.append(msg);
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

int HTTPConnection::getMessageQueueSize()
{
    int s;
    
    m_mutex.lock();
    s = m_processList.size();
    m_mutex.unlock();
    return s;
}

//-------------------------------------------------------------------------------------------

HTTPConnection::Message *HTTPConnection::getMessage()
{
    Message *msg;
    
    m_mutex.lock();
    if(m_processList.size() > 0)
    {
        msg = m_processList.takeFirst();
    }
    else
    {
        msg = 0;
    }
    m_mutex.unlock();
    return msg;
}

//-------------------------------------------------------------------------------------------

bool HTTPConnection::process()
{
    if(!TCPConnectionSocket::process())
    {
        return false;
    }

    if(m_state==e_CompleteOnSend && m_sQueue.size()==0)
    {
        if(m_persistent)
        {
            m_state = e_StartSession;
        }
        else
        {
            return false;
        }
    }
    
    if(m_process)
    {
        m_process = processMain();
        
        if(!m_process && !isLocked())
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        if(isLocked())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

//-------------------------------------------------------------------------------------------

bool HTTPConnection::processMain()
{
    bool loop = true;

    if(m_state==e_RecieveRequest || m_state==e_RecieveRequestData)
    {
        if(timeout())
        {
            return false;
        }
    }
    
    do
    {
        loop = false;

        switch(m_state)
        {
            case e_StartSession:
                resetState();
                m_state = e_RecieveRequest;
                loop = true;
                break;
                
            case e_RecieveRequest:
                if(!getRequest(loop))
                {
                    return false;
                }
                break;
                
            case e_RecieveRequestData:
                if(!getRequestData(loop))
                {
                    return false;
                }
                break;
                
            case e_ProcessRequest:
                if(!doRequest(loop))
                {
                    return false;
                }
                break;
                
            case e_PostResponse:
                if(!doPostRequest(loop))
                {
                    return false;
                }
                break;
                
            case e_PostData:
                if(!doPostData(loop))
                {
                    return false;
                }
                break;
                
            case e_CompleteOnSend:
                break;
                
            default:
                printError("mainProcess","Unknown connection state");
                return false;
        }
    }
    while(loop);
    
    return true;
}

//-------------------------------------------------------------------------------------------

bool HTTPConnection::getRequest(bool& loop)
{
    bool done = false;
    QString line;
    
    loop = false;
    
    while(!done && canGetNextLine())
    {
        if(!getNextLine(line))
        {
            return false;
        }
        if(!m_request.insertLine(line,done))
        {
            return false;
        }
    }
    if(done)
    {
        if(m_request.type()==Unit::e_Request)
        {
            tint major,minor,length;
    
            if(m_request.exist("Connection"))
            {
                QString cn;
            
                cn = m_request.data("Connection").toLower();
                if(cn=="keep-alive" && m_request.exist("keep-alive"))
                {
                    tint tOut;
                
                    tOut = m_request.data("keep-alive").toInt();
                    m_time += tOut;
                }
            }
            
            m_request.getVersion(major,minor);
            if(minor==0)
            {
                m_persistent = false;
            }
            else
            {
                line = m_request.data("Connection");
                if(!line.isEmpty() && line=="close")
                {
                    m_persistent = false;
                }
                else
                {
                    m_persistent = true;
                }
            }
            m_response.setVersion(major,minor);
            
            if(m_request.exist("Content-Length"))
            {
                length = m_request.data("Content-Length").toInt();
                if(length>0)
                {
                    if(length <= c_HTTPConnection_MaxPostSize)
                    {
                        m_requestBodyLength = length;
                        m_state = e_RecieveRequestData;
                    }
                    else
                    {
                        sendErrorResponse(403);
                        printError("getRequest","Request received to accept POST request data that is too large");
                    }
                }
                else
                {
                    m_requestBody.SetSize(0);
                    m_requestBodyLength = 0;
                    m_state = e_ProcessRequest;
                }
            }
            else
            {
                m_state = e_ProcessRequest;
            }
            loop = true;
        }
        else
        {
            printError("getRequest","HTTP request expected from client");
            return false;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool HTTPConnection::getRequestData(bool& loop)
{
    if(m_state!=e_RecieveRequestData)
    {
        printError("getRequestData","Can only receive data while in request data mode");
        return false;
    }
    if(m_requestBodyLength<=0)
    {
        printError("getRequestData","No length of data to receive defined");
        return false;
    }
    
    if(canRead(m_requestBodyLength))
    {
        m_requestBody.SetSize(m_requestBodyLength);
        if(read(m_requestBody.GetData(),m_requestBodyLength))
        {
            m_state = e_ProcessRequest;
            loop = true;
        }
        else
        {
            printError("getRequestData","Failed to recieve required data from request");
            return false;
        }
    }
    else
    {
        loop = false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool HTTPConnection::doRequest(bool& loop)
{
    if(m_state!=e_ProcessRequest)
    {
        printError("doRequest","Connection not in process request state");
        return false;
    }
    
    loop = true;
    
    if(m_request.request()==Unit::e_Get || m_request.request()==Unit::e_Post)
    {
        NetArraySPtr rBody(new NetArray);
        rBody->AppendRaw(reinterpret_cast<const tchar *>(m_requestBody.GetData()),m_requestBody.GetSize());

        m_receiver = m_server->getReceive(m_request.resource(),this);
        if(m_receiver!=0)
        {
            m_receiver->setAddress(m_host,m_port,&m_address);
            m_receiver->setHeader(m_request);
            m_receiver->setBody(rBody);
            m_receiver->process();
            
            m_state = e_PostResponse;
        }
        else
        {
            return sendErrorResponse(404);
        }
    }
    else
    {
        return sendErrorResponse(501);
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool HTTPConnection::doPostRequest(bool& loop)
{
    Message *msg;
    bool res = false;
    
    if(m_state!=e_PostResponse)
    {
        printError("doPostRequest","Cannot post response in given state");
        return false;
    }
    
    msg = getMessage();
    if(msg==0)
    {
        return true;
    }
    
    if(msg->type()==e_PostHeader)
    {
        tint code;
        QString line,str;
        QByteArray arr;
        
        m_response = msg->header();
        line = m_response.data("Connection").toLower();
        if(!line.isEmpty() && line=="close")
        {
            m_persistent = false;
        }
        
        code = m_response.response();
        if(!(code>=100 && code<300))
        {
            m_response.add("Connection","close");
            m_persistent = false;
        }
        
        m_response.print(str);
        arr = str.toUtf8();
        if(write(arr.constData(),arr.length()))
        {
            if(m_response.response()!=100)
            {
                m_state = e_PostData;
            }
            loop = true;
            res = true;
        }
        else
        {
            printError("doPostRequest","Failed to send response");
            sendErrorResponse(500);
        }
    }
    else
    {
        printError("doPostRequest","Trying to post data before processing the response");
        sendErrorResponse(500);
    }
    delete msg;
    return res;
}

//-------------------------------------------------------------------------------------------

bool HTTPConnection::doPostData(bool& loop)
{
    Message *msg;
    bool res = true;
    
    if(m_state!=e_PostData)
    {
        printError("doPostData","Cannot post data in given state");
        return false;
    }
    
    msg = getMessage();
    if(msg==0)
    {
        loop = false;
        return true;
    }
    
    switch(msg->type())
    {
        case e_PostHeader:
            m_state = e_PostResponse;
            loop = true;
            break;
            
        case e_PostBody:
            {
                NetArraySPtr mem(msg->body());
                
                if(mem->GetData()!=0 && mem->GetSize()>0)
                {
                    if(!write(mem->GetData(),mem->GetSize()))
                    {
                        printError("doPostData","Error in sending data to client");
                        res = false;
                    }
                }
                loop = true;
            }
            break;
            
        case e_PostChunk:
            {
                common::BString hStr,newLine("\r\n");
                
                m_chunked = true;
                hStr  = common::BString::HexInt(msg->body()->GetSize());
                if(!msg->string().isEmpty())
                {
                    hStr += ";";
                    hStr += msg->string().toUtf8().constData();
                }
                hStr += newLine;
                if(write(static_cast<const tchar *>(hStr),hStr.GetLength()))
                {
                    if(write(msg->body()->GetData(),msg->body()->GetSize()))
                    {
                        if(!write(static_cast<const tchar *>(newLine),newLine.GetLength()))
                        {
                            printError("doPostData","Error in sending data to client");
                            res = false;
                        }
                    }
                    else
                    {
                        printError("doPostData","Error in sending data to client");
                        res = false;
                    }
                }
                else
                {
                    printError("doPostData","Error in sending data to client");
                    res = false;
                }
                loop = true;
            }
            break;
            
        case e_Complete:
            if(m_chunked)
            {
                common::BString completeStr("0\r\n\r\n");
                if(!write(static_cast<const tchar *>(completeStr),completeStr.GetLength()))
                {
                    printError("doPostData","Error in sending last chunk to client");
                    res = false;
                }
            }
            m_state = e_CompleteOnSend;
            loop = true;
            break;
    }
    delete msg;
    return res;
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::resetState()
{
    Message *msg;
    Unit blank;
    
    m_mutex.lock();
    if(m_receiver!=0)
    {
        m_server->freeReceive(m_receiver);
        m_receiver = 0;
    }
    while(m_processList.size() > 0)
    {
        msg = m_processList.takeFirst();
        delete msg;    
    }
    
    m_state = e_StartSession;
    m_persistent = true;
    m_chunked = false;
    m_request = blank;
    m_response = blank;
    m_requestBody.SetSize(0);
    m_requestBodyLength = 0;
    
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

void HTTPConnection::releaseReceiver()
{
    m_receiver = 0;
}

//-------------------------------------------------------------------------------------------

bool HTTPConnection::sendErrorResponse(tint code)
{
    QString s;
    m_response.response(code);
    m_response.print(s);
    QByteArray a(s.toUtf8());

    if(write(a.constData(),a.length()))
    {
        printError("sendErrorResponse","Failed to send response to client");
        return false;
    }
    m_state = e_CompleteOnSend;
    return true;
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
