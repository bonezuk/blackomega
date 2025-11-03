//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_REMOTE_KEYCODESTATEENGINE_H
#define __OMEGA_REMOTE_KEYCODESTATEENGINE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include <QMap>

#include "common/inc/TimeStamp.h"
#include "remote/inc/KeyCodesContainer.h"
#include "remote/inc/RemoteIF.h"
#include "remote/inc/KeyAssignment.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

class REMOTE_EXPORT KeyCodeStateEngine : public RemoteIF
{
    public:
        Q_OBJECT

    public:
        KeyCodeStateEngine(QObject *parent = 0);
        virtual ~KeyCodeStateEngine();

        virtual double getDelayTime() const;
        virtual double getRepeatTime() const;

    protected:
        QMap<KeyCode,QPair<int,common::TimeStamp> > m_keyStateMap;
        double m_delayTime;
        double m_repeatTime;
        KeyAssignmentSPtr m_keyAssignment;

        virtual QMap<KeyCode,QPair<int,common::TimeStamp> >& getKeyStateMap();
        virtual const QMap<KeyCode,QPair<int,common::TimeStamp> >& getKeyStateMapConst() const;

        virtual int getKeyState(const KeyCode& cmd) const;
        virtual common::TimeStamp getKeyTime(const KeyCode& cmd) const;

        virtual void createKeyState(const KeyCode& cmd,int state,const common::TimeStamp& refTime);
        virtual void updateKeyState(const KeyCode& cmd,int state);
        virtual void updateKeyState(const KeyCode& cmd,int state,const common::TimeStamp& refTime);
        virtual void removeKeyState(const KeyCode& cmd);

        virtual common::TimeStamp getReferenceTime() const;

        virtual bool isInTimeWindow(const KeyCode& cmd,double windowLength,double lowerLimit,double upperLimit) const;
        virtual bool isDelayWindow(const KeyCode& cmd) const;
        virtual bool isDelayWindowBottom(const KeyCode& cmd) const;
        virtual bool isDelayWindowTop(const KeyCode& cmd) const;
        virtual bool isRepeatWindow(const KeyCode& cmd) const;
        virtual bool isFastSignal(const KeyCode& cmd,int repeat) const;

        virtual void processRemoteCommand(const KeyCode& cmd,int repeat);
        virtual void processRemoteButtonNotPressed(const KeyCode& cmd,int repeat);
        virtual void processRemoteInitialSignalReceived(const KeyCode& cmd,int repeat);
        virtual void processRemoteFastRepeatSignal(const KeyCode& cmd,int repeat);
        virtual void processRemoteSlowRepeatSignal(const KeyCode& cmd,int repeat);
        virtual void processRemoteRepeatInTopDelayWindow(const KeyCode& cmd,int repeat);
        virtual void processRemoteRegularHoldSignal(const KeyCode& cmd,int repeat);

        virtual void onProcessTimer();

        virtual void processTimerCommand(const KeyCode& cmd);
        virtual void processTimerButtonNotPressed(const KeyCode& cmd);
        virtual void processTimerInitialSignalReceived(const KeyCode& cmd);
        virtual void processTimerFastRepeatSignal(const KeyCode& cmd);
        virtual void processTimerSlowRepeatSignal(const KeyCode& cmd);
        virtual void processTimerRepeatInTopDelayWindow(const KeyCode& cmd);
        virtual void processTimerRegularHoldSignal(const KeyCode& cmd);

        virtual void processReleaseCommand(const KeyCode& cmd);
        virtual void processReleaseButtonNotPressed(const KeyCode& cmd);
        virtual void processReleaseInitialSignalRecieved(const KeyCode& cmd);
        virtual void processReleaseFastRepeatSignal(const KeyCode& cmd);
        virtual void processReleaseSlowRepeatSignal(const KeyCode& cmd);
        virtual void processReleaseRepeatInTopDelayWindow(const KeyCode& cmd);
        virtual void processReleaseHoldSignalNotEmitted(const KeyCode& cmd);
        virtual void processReleaseHoldSignalEmitted(const KeyCode& cmd);

        virtual KeyAssignmentSPtr getKeyAssignment();

        virtual void processClick(const KeyCode& cmd);
        virtual void processHold(const KeyCode& cmd);

        virtual void printTimeDifference(const tchar *routine,const KeyCode& cmd) const;

        virtual void processUnassignedClick(const KeyCode& cmd);

    Q_SIGNALS:
        void onAddFiles();
        void onAddFolders();
        void onSavePlaylist();
        void onPreferences();
        void onCut();
        void onCopy();
        void onPaste();
        void onSelectAll();
        void onRemoveTracks();
        void onHelp();
};

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
