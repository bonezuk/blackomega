//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERIOS_PLAYERAUDIOIOINTERFACE_H
#define __PLAYERAPP_PLAYERIOS_PLAYERAUDIOIOINTERFACE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include "audioio/inc/AOBase.h"

#include "engine/inc/Codec.h"
#include "track/info/inc/Info.h"
#include "common/inc/DiskOps.h"

#include "playerapp/playercommon/inc/OmegaAudioInterface.h"
#include "playerapp/playercommon/inc/OmegaPlaylistInterface.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PlayerAudioIOInterface : public OmegaAudioInterface
{
    Q_OBJECT
    
    public:
        Q_PROPERTY(bool isUpdateRunning READ getIsUpdateRunning NOTIFY onUpdateRunning)
    
    public:
        PlayerAudioIOInterface(QSharedPointer<OmegaPlaylistInterface>& pPLInterface, QObject *parent = 0);
        virtual ~PlayerAudioIOInterface();
        
        virtual void playFile(const QString& fileName, bool isNext);
        virtual void playFileWithTime(const QString& fileName, const common::TimeStamp& start,const common::TimeStamp& length, bool isNext);
        virtual void play();
        virtual void pause();
        virtual void stop();
        virtual void seek(const common::TimeStamp& t);

        virtual bool init();
        virtual void quitDaemon();
        
        virtual void update();
        virtual bool getIsUpdateRunning() const;
        
    signals:
        void onUpdateRunning();
    
    private:
    
        QSharedPointer<audioio::AOBase> m_audio;
        QSharedPointer<OmegaPlaylistInterface> m_pPLInterface;
        bool m_isUpdateRunning;
        
        virtual void printError(const char *strR, const char *strE) const;
        
    private slots:
                
        void onAudioStart(const QString& name);
        void onAudioPlay();
        void onAudioPause();
        void onAudioStop();
        void onAudioTime(quint64 t);
        void onAudioBuffer(tfloat32 percent);
        void onAudioReadyForNext();
        void onAudioNoNext();
        void onAudioCrossfade();
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

