//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYERAPP_PLAYERCOMMON_OMEGAPIBUSSERVICENAMES_H
#define __OMEGA_PLAYERAPP_PLAYERCOMMON_OMEGAPIBUSSERVICENAMES_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"

#if QT_VERSION >= 0x050000
#include <QJsonDocument>
#include <QJsonObject>
#else
#include <QJsonDocument.h>
#include <QJsonObject.h>
#endif

#include <QVariant>

//-------------------------------------------------------------------------------------------

#define OMEGAPLAYLISTMANAGER_SERVICE_NAME    "OmegaPlaylistManager"
#define OMEGAAUDIOINTERFACE_SERVICE_NAME     "OmegaAudioInterface"
#define OMEGAPLWEB_SERVICE_NAME              "OmegaPLWebManager"
#define OMEGAWEBEVENT_SERVICE_NAME           "OmegaWebEvent"

#define OMEGA_IPC_FUNCTION_NAME "function"

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
