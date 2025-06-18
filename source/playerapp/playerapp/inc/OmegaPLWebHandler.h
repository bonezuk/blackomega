//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERAPP_OMEGAPLWEBHANDLER_H
#define __PLAYERAPP_PLAYERAPP_OMEGAPLWEBHANDLER_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/OmegaWebInterface.h"
#include "playerapp/playercommon/inc/OmegaPLWebInterface.h"

#include <QSharedPointer>

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class OmegaPLWebHandler : public OmegaPLWebInterface
{
    public:
        OmegaPLWebHandler(QSharedPointer<OmegaWebInterface>& pPLWebInterface);
        virtual ~OmegaPLWebHandler();
        
        virtual QJsonDocument getFullPlaylist();
        
        virtual QJsonDocument getPlaybackState();
        
        virtual void onPressPlay();
        
        virtual void onStartPlaying(tuint64 id);
        
    protected:
        QSharedPointer<OmegaWebInterface> m_pPLWebInterface;
    
        virtual void printError(const char *strR, const char *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

