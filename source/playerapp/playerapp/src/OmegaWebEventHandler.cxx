#include "playerapp/playerapp/inc/OmegaWebEventHandler.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

OmegaWebEventHandler::OmegaWebEventHandler() : WebEventInterface(),
    WebEventRegisterInterface(),
    m_broker()
{}

//-------------------------------------------------------------------------------------------

OmegaWebEventHandler::~OmegaWebEventHandler()
{}

//-------------------------------------------------------------------------------------------

void OmegaWebEventHandler::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "OmegaWebEventHandler::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool OmegaWebEventHandler::registerConnection(network::http::HTTPReceive *receive)
{
    return m_broker.registerConnection(receive);
}

//-------------------------------------------------------------------------------------------
//    {
//        "event": "eventName",
//        "data": {
//             "data": "aboutTheEvent"
//        }
//    }
//-------------------------------------------------------------------------------------------

void OmegaWebEventHandler::sendEvent(const QJsonDocument& doc)
{
    if(!doc.isNull() && doc.isObject())
    {
        QJsonObject root = doc.object();
        
        if(root.contains("event"))
        {
            QJsonValue e = root.value("event");
            if(e.isString())
            {
                m_broker.postEvent(doc);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
