//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_OPLAYER_OPLAYERMAIN_H
#define __OMEGA_OPLAYER_OPLAYERMAIN_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "audioio/inc/AOBase.h"
#include "engine/inc/Codec.h"
#include "track/info/inc/Info.h"
#include "common/inc/DiskOps.h"

#include <QCoreApplication>

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class OPlayer : public QCoreApplication
{
    public:
        Q_OBJECT
        
    public:
        OPlayer(int argc,char **argv);
        virtual ~OPlayer();
    
    protected:
        
        QSharedPointer<audioio::AOBase> m_audio;
        QStringList m_fileNameList;

        common::TimeStamp m_playTime;
        bool m_playZeroFlag;
        common::TimeStamp m_totalTime;
        
        bool m_printDeviceInfo;
        tint m_forceDacBits;
        
        void printTrackInfo(const QString& fileName);
        QString timeToString(const common::TimeStamp& t) const;
        
        void processArguements(int argc,char **argv);
        
    protected slots:
    
        void onInit();
        void onStop();
                
        void onAudioStart(const QString& name);
        void onAudioPlay();
        void onAudioPause();
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

