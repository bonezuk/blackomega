//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERAPP_PLAYLISTWEBMODELPLA_H
#define __PLAYERAPP_PLAYERAPP_PLAYLISTWEBMODELPLA_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayListWebModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PlayListWebModelPLA : public PlayListWebModel
{
    public:
        PlayListWebModelPLA(QObject *parent = 0);
        PlayListWebModelPLA(QVector<QPair<track::db::DBInfoSPtr,tint> >& playList, QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent = 0);
        virtual ~PlayListWebModelPLA();
        
        virtual bool initialise();
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
