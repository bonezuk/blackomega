#include "network/http/inc/ResponseCodes.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace network
{
namespace http
{
//-------------------------------------------------------------------------------------------

typedef struct
{
    tint code;
    const tchar *reason;
} HTTPResponseEntry;

//-------------------------------------------------------------------------------------------

const HTTPResponseEntry g_HTTPResponses[] = {
    //1xx - Informational codes
    {100,"Continue"},
    {101,"Switching Protocols"},

    //2xx - Successful
    {200,"OK"},
    {201,"Created"},
    {202,"Accepted"},
    {203,"Non-Authoritative Information"},
    {204,"No Content"},
    {205,"Reset Content"},
    {206,"Partial Content"},

    //3xx - Redirection
    {300,"Multiple Choices"},
    {301,"Moved Permanently"},
    {302,"Found"},
    {303,"See Other"},
    {304,"Not Modified"},
    {305,"Use Proxy"},
    {307,"Temporary Redirect"},

    //4xx - Client error
    {400,"Bad Request"},
    {401,"Unauthorized"},
    {402,"Payment Required"},
    {403,"Forbidden"},
    {404,"Not Found"},
    {405,"Method Not Allowed"},
    {406,"Not Acceptable"},
    {407,"Proxy Authentication Required"},
    {408,"Request Timeout"},
    {409,"Conflict"},
    {410,"Gone"},
    {411,"Length Required"},
    {412,"Precondition Failed"},
    {413,"Request Entity Too Large"},
    {414,"Request-URI Too Long"},
    {415,"Unsupported Media Type"},
    {416,"Requested Range Not Satisfiable"},
    {417,"Expectation Failed"},

    //5xx - Server error
    {500,"Internal Server Error"},
    {501,"Not Implemented"},
    {502,"Bad Gateway"},
    {503,"Service Unavailable"},
    {504,"Gateway Timeout"},
    {505,"HTTP 1.1 only supported as chunked encoding is required by streaming service"},

    //End of server errors list.
    {-1,""}
};

//-------------------------------------------------------------------------------------------

ResponseCodes *ResponseCodes::m_instance = 0;

//-------------------------------------------------------------------------------------------

ResponseCodes::ResponseCodes() : m_blank(),
    m_reasonMap()
{
    tint i = 0;

    while(g_HTTPResponses[i].code!=-1)
    {
        m_reasonMap.insert(g_HTTPResponses[i].code,QString::fromLatin1(g_HTTPResponses[i].reason));
        i++;
    }
}

//-------------------------------------------------------------------------------------------

const QString& ResponseCodes::reason(tint code)
{
    QMap<tint,QString>::const_iterator ppI;

    if(m_instance==0)
    {
        m_instance = new ResponseCodes;
    }

    ppI = m_instance->m_reasonMap.find(code);
    if(ppI != m_instance->m_reasonMap.end())
    {
        return ppI.value();
    }
    else
    {
        return m_instance->m_blank;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace http
} // namespace network
} // namespace omega
//-------------------------------------------------------------------------------------------
