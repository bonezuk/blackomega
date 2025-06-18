#include "playerapp/playercommon/inc/HTTPEventBroker.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

HTTPEventBroker::HTTPEventBroker() : m_nextID(1),
    m_connections()
{}

//-------------------------------------------------------------------------------------------

HTTPEventBroker::~HTTPEventBroker()
{}

//-------------------------------------------------------------------------------------------

void HTTPEventBroker::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "HTTPEventBroker::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool HTTPEventBroker::registerConnection(network::http::HTTPReceive *receive)
{
    network::http::Unit hdr;

    if(receive == 0)
    {
        printError("registerConnection", "No connection given");
        return false;
    }

    hdr.response(200);
    hdr.add("Connection", "keep-alive");
    hdr.add("Content-Type", "text/event-stream");
    hdr.add("Cache-Control", "no-cache");
    hdr.add("Transfer-Encoding", "chunked");

    receive->connection()->postResponse(hdr);
    m_connections.append(receive);
    return true;
}

//-------------------------------------------------------------------------------------------

void HTTPEventBroker::postEvent(const QJsonDocument& data)
{
    for(QList<network::http::HTTPReceive *>::iterator ppI = m_connections.begin(); ppI != m_connections.end();)
    {
        network::http::HTTPReceive *receive = *ppI;

        if(!receive->connection()->isError())
        {
            QString eventText;

            eventText = QString("data: %1\n\n")
                .arg(QString::fromUtf8(data.toJson(QJsonDocument::Compact)));

            QByteArray arr = eventText.toUtf8();
            network::NetArraySPtr dataArray(new network::NetArray);
            dataArray->SetSize(arr.size());
            ::memcpy(dataArray->GetData(), arr.data(), arr.size());

            receive->connection()->postChunk(dataArray);

            ppI++;
        }
        else
        {
            // Remove connection that has been closed by the client.
            receive->connection()->complete();
            receive->endProcess();
            ppI = m_connections.erase(ppI);
        }
    }
    m_nextID++;
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
