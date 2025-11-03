//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_REMOTEIF_H
#define __OMEGA_PLAYER_REMOTEIF_H
//-------------------------------------------------------------------------------------------

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "common/inc/CommonTypes.h"
#include "common/inc/Factory.h"

#include "remote/inc/RemoteDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT RemoteEvent
{
    public:
        RemoteEvent();
        virtual ~RemoteEvent();
};

//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT RemoteIF : public QObject
{
    public:
        Q_OBJECT

    public:
        RemoteIF(QObject *parent = 0);
        virtual ~RemoteIF();

        virtual bool start() = 0;
        virtual void stop() = 0;

        virtual void remoteEvent(RemoteEvent *e) = 0;

        static bool isServiceAvailable(const QString& name);

    protected:

        QTimer *m_timer;

        virtual void onTimerImplementation() = 0;

        virtual bool setupTimer();
        virtual void freeTimer();
        virtual void startTimer();
        virtual void stopTimer();

        virtual void playPauseClickSignal();
        virtual void previousTrackClickSignal();
        virtual void nextTrackClickSignal();
        virtual void seekBackSignal();
        virtual void seekForwardSignal();
        virtual void volumeDownIncrementSignal();
        virtual void volumeDownHoldSignal();
        virtual void volumeUpIncrementSignal();
        virtual void volumeUpHoldSignal();

    protected slots:

        void onTimer();

    signals:

        // Play/Pause button (Aluminium model)
        // Central button (White model)
        void playPauseClick();

        // Left button click and release
        void previousTrackClick();
        // Right button click and release
        void nextTrackClick();

        // Left button click and hold
        void seekBack();
        // Right button click and hold
        void seekForward();

        // Down button click and release
        void volumeDownIncrement();
        // Down button click and hold
        void volumeDownHold();
        // Up button click and release
        void volumeUpIncrement();
        // Up button click and hold
        void volumeUpHold();
};

typedef QSharedPointer<RemoteIF> RemoteIFSPtr;

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(REMOTE_EXPORT,RemoteIFFactory,RemoteIF)

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
