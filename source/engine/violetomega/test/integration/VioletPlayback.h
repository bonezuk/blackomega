//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_VIOLETOMEGA_INTEGRATION_VIOLETPLAYBACK_H
#define __OMEGA_ENGINE_VIOLETOMEGA_INTEGRATION_VIOLETPLAYBACK_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOBase.h"
#include "engine/blackomega/inc/MPCodec.h"
#include "engine/blueomega/inc/WaveEngine.h"
#include "engine/silveromega/inc/SilverCodec.h"
#include "engine/whiteomega/inc/WhiteCodec.h"
#include "engine/greenomega/inc/GreenCodec.h"
#include "engine/redomega/inc/RedCodec.h"
#include "engine/violetomega/inc/VioletCodec.h"

#include <QtCore/QCoreApplication>
#include <QTimer>
#include <QThread>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace testspace
{
namespace audio
{
//-------------------------------------------------------------------------------------------

class VioletPlayback : public QCoreApplication
{
    public:
        Q_OBJECT

    public:
        VioletPlayback(int argc,char **argv);
        virtual ~VioletPlayback();

    protected:
        tint m_songIdx;
        QSharedPointer<audioio::AOBase> m_audio;
        common::TimeStamp m_playTime;
        bool m_playZeroFlag;

    protected Q_SLOTS:
        void onInit();
        void onStop();

        void onPause();
        void onUnpause();
        void onReset();

        void onSeek();

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
} // namespace audio
} // namespace testspace
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
