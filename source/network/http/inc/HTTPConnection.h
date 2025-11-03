//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_NETWORK_HTTP_HTTPCONNECTION_H
#define __OMEGA_NETWORK_HTTP_HTTPCONNECTION_H
//-------------------------------------------------------------------------------------------

#include "network/inc/TCPConnServerSocket.h"
#include "network/http/inc/HTTPServer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

class HTTP_EXPORT HTTPConnection : public TCPConnServerSocket
{
    public:
        Q_OBJECT

    public:
        typedef enum
        {
            e_StartSession = 0,
            e_RecieveRequest,
            e_RecieveRequestData,
            e_ProcessRequest,
            e_PostResponse,
            e_PostData,
            e_CompleteOnSend
        } State;

        typedef enum
        {
            e_PostHeader = 0,
            e_PostBody,
            e_PostChunk,
            e_Complete
        } MessageType;

        class Message;

    public:
        HTTPConnection(HTTPServer *server,Service *svr,QObject *parent = 0);
        virtual ~HTTPConnection();

        virtual bool process();

        virtual void postResponse(const Unit& x);
        virtual void postBody(NetArraySPtr x);
        virtual void postChunk(NetArraySPtr mem);
        virtual void postChunk(const QString& str,NetArraySPtr mem);
        virtual void complete();

        virtual Message *getMessage();
        virtual int getMessageQueueSize();

        virtual void releaseReceiver();

    protected:

        HTTPServer *m_server;

        QMutex m_mutex;
        QList<Message *> m_processList;

        State m_state;
        bool m_persistent;
        bool m_chunked;

        Unit m_request;
        NetArray m_requestBody;
        tint m_requestBodyLength;

        Unit m_response;

        HTTPReceive *m_receiver;

        bool m_process;

        virtual void printError(const tchar *strR,const tchar *strE) const;
        virtual void printError(const tchar *strR,const tchar *strE,tint eNo) const;

        virtual bool processMain();

        virtual bool getRequest(bool& loop);
        virtual bool getRequestData(bool& loop);
        virtual bool doRequest(bool& loop);
        virtual bool doPostRequest(bool& loop);
        virtual bool doPostData(bool& loop);

        virtual bool sendErrorResponse(tint code);

        virtual void resetState();
};

//-------------------------------------------------------------------------------------------

class HTTP_EXPORT HTTPConnection::Message
{
    public:
        Message(MessageType t);

        MessageType type() const;

        const QString& string() const;
        void setString(const QString& x);

        const Unit& header() const;
        void setHeader(const Unit& x);

        NetArraySPtr body() const;
        void setBody(NetArraySPtr x);

    protected:

        MessageType m_type;
        QString m_str;
        Unit m_header;
        NetArraySPtr m_body;
};

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

