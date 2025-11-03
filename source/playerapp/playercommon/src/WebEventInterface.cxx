#include "playerapp/playercommon/inc/WebEventInterface.h"
#include <QVariantMap>

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

WebEventInterface::WebEventInterface()
{}

//-------------------------------------------------------------------------------------------

WebEventInterface::~WebEventInterface()
{}

//-------------------------------------------------------------------------------------------

void WebEventInterface::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "WebEventInterface::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

QJsonDocument WebEventInterface::createJsonEvent(const QString& eventName, const QJsonObject& data)
{
    QJsonObject eventJ;
    QJsonDocument doc;
    
    QJsonValue eventNameJ(eventName);
    eventJ.insert("event", eventNameJ);
    eventJ.insert("data", data);
    doc.setObject(eventJ);
    return doc;
}

//-------------------------------------------------------------------------------------------
//    {
//        "event": "onPlaybackTime",
//        "data": {
//             "time": 12.345
//        }
//    }
//-------------------------------------------------------------------------------------------

void WebEventInterface::onPlaybackTime(const common::TimeStamp& tS)
{
    QJsonObject dataJ;
    QJsonValue timeData(static_cast<tfloat64>(tS));
    dataJ.insert("time", timeData);
    QJsonDocument doc = createJsonEvent("onPlaybackTime", dataJ);
    sendEvent(doc);
}

//-------------------------------------------------------------------------------------------
//    {
//        "event": "onAudioPlaybackEvent",
//        "data": {
//             "isPlaying": true,
//            "id": 12345
//        }
//    }
//-------------------------------------------------------------------------------------------

void WebEventInterface::onAudioPlaybackEvent(bool isPlaying, tuint64 id)
{
    QVariantMap map;
    QString idStr = QString::number(id);
    map.insert("isPlaying", QVariant(isPlaying));
    map.insert("id", QVariant(idStr));
    QJsonObject data = QJsonObject::fromVariantMap(map);
    QJsonDocument doc = createJsonEvent("onAudioPlaybackEvent", data);
    sendEvent(doc);
}

//-------------------------------------------------------------------------------------------
//    {
//        "event": "onAudioStart",
//        "data": {
//             "id": 12345
//        }
//    }
//-------------------------------------------------------------------------------------------

void WebEventInterface::onAudioStart(tuint64 id)
{
    QVariantMap map;
    QString idStr = QString::number(id);
    map.insert("id", QVariant(idStr));
    QJsonObject data = QJsonObject::fromVariantMap(map);
    QJsonDocument doc = createJsonEvent("onAudioStart", data);
    sendEvent(doc);
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
