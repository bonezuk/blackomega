//-------------------------------------------------------------------------------------------
#if defined(OMEGA_IOS)
//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERIOS_PLAYERIOSAUDIOSESSION_H
#define __PLAYERAPP_PLAYERIOS_PLAYERIOSAUDIOSESSION_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOCoreAudioSessionIOS.h"
#include "playerapp/playercommon/inc/PlayListModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PlayerIOSAudioSession : public audioio::AOCoreAudioSessionIOS
{
    public:
        Q_OBJECT

    public:
        virtual ~PlayerIOSAudioSession();
        
        static QSharedPointer<PlayerIOSAudioSession> playerInstance();
        
        static QSharedPointer<PlayerIOSAudioSession> startSession(QObject *parent = 0);
        virtual bool setModelAndInit(QSharedPointer<PlayListModel>& pPLModel) = 0;
        virtual void endSession();
    
    protected:
        PlayerIOSAudioSession(QObject *parent);
    
    protected slots:
        void onPlayStateChanged();
        void updateNowPlay();    
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
