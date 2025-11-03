//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERCOMMON_OMEGAPLWEBINTERFACE_H
#define __PLAYERAPP_PLAYERCOMMON_OMEGAPLWEBINTERFACE_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"

#include "common/inc/Log.h"
#include "playerapp/playercommon/inc/PlayerCommonDLL.h"

#if QT_VERSION >= 0x050000
#include <QJsonDocument>
#else
#include <QJsonDocument.h>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT OmegaPLWebInterface
{
    public:
        OmegaPLWebInterface();
        virtual ~OmegaPLWebInterface();

        // { "function": "getFullPlaylist" }
        virtual QJsonDocument getFullPlaylist() = 0;

        // { "function": "getPlaybackState" }
        virtual QJsonDocument getPlaybackState() = 0;

        // { "function": "onPressPlay" }
        virtual void onPressPlay() = 0;

        // { "function": "onStartPlaying", "id": 12345 }
        virtual void onStartPlaying(tuint64 id) = 0;

    protected:

        virtual void printError(const char *strR, const char *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
