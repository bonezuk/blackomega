//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERCOMMON_OMEGAWEBINTERFACE_H
#define __PLAYERAPP_PLAYERCOMMON_OMEGAWEBINTERFACE_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/OmegaAudioInterface.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

//-------------------------------------------------------------------------------------------
namespace orcus
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT OmegaWebInterface
{
	public:
		OmegaWebInterface();
		
		virtual int playlistSize() = 0;
		virtual QJsonDocument playlistAsJson(int fromIndex, int toIndex) = 0;
		
		virtual QJsonDocument getPlaybackState() = 0;
		
		virtual void onPressPlay() = 0;
		virtual void onStartPlaying(tuint64 id) = 0;
};

//-------------------------------------------------------------------------------------------
} // namespace orcus
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------