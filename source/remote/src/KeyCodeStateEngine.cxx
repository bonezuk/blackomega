#include "remote/inc/KeyCodeStateEngine.h"
#include "remote/inc/QKeyLineEdit.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

KeyCodeStateEngine::KeyCodeStateEngine(QObject *parent) : RemoteIF(parent),
    m_keyStateMap(),
    m_delayTime(0.5),
    m_repeatTime(0.25)
{}

//-------------------------------------------------------------------------------------------

KeyCodeStateEngine::~KeyCodeStateEngine()
{}

//-------------------------------------------------------------------------------------------

QMap<KeyCode,QPair<int,common::TimeStamp> >& KeyCodeStateEngine::getKeyStateMap()
{
    return m_keyStateMap;
}

//-------------------------------------------------------------------------------------------

const QMap<KeyCode,QPair<int,common::TimeStamp> >& KeyCodeStateEngine::getKeyStateMapConst() const
{
    return m_keyStateMap;
}

//-------------------------------------------------------------------------------------------

double KeyCodeStateEngine::getDelayTime() const
{
    return m_delayTime;
}

//-------------------------------------------------------------------------------------------

double KeyCodeStateEngine::getRepeatTime() const
{
    return m_repeatTime;
}

//-------------------------------------------------------------------------------------------

int KeyCodeStateEngine::getKeyState(const KeyCode& cmd) const
{
    int state;
    QMap<KeyCode,QPair<int,common::TimeStamp> >::const_iterator ppI;

    ppI = getKeyStateMapConst().find(cmd);
    if(ppI!=getKeyStateMapConst().end())
    {
        state = ppI.value().first;
    }
    else
    {
        state = 0;
    }
    return state;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp KeyCodeStateEngine::getKeyTime(const KeyCode& cmd) const
{
    common::TimeStamp t;
    QMap<KeyCode,QPair<int,common::TimeStamp> >::const_iterator ppI;

    ppI = getKeyStateMapConst().find(cmd);
    if(ppI!=getKeyStateMapConst().end())
    {
        t = ppI.value().second;
    }
    return t;
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::createKeyState(const KeyCode& cmd,int state,const common::TimeStamp& refTime)
{
    removeKeyState(cmd);
    getKeyStateMap().insert(cmd,QPair<int,common::TimeStamp>(state,refTime));
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::updateKeyState(const KeyCode& cmd,int state)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> >::iterator ppI = getKeyStateMap().find(cmd);
    if(ppI!=getKeyStateMap().end())
    {
        ppI.value().first = state;
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::updateKeyState(const KeyCode& cmd,int state,const common::TimeStamp& refTime)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> >::iterator ppI = getKeyStateMap().find(cmd);
    if(ppI!=getKeyStateMap().end())
    {
        ppI.value().first = state;
        ppI.value().second = refTime;
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::removeKeyState(const KeyCode& cmd)
{
    QMap<KeyCode,QPair<int,common::TimeStamp> >::iterator ppI = getKeyStateMap().find(cmd);
    if(ppI!=getKeyStateMap().end())
    {
        getKeyStateMap().erase(ppI);
    }
}

//-------------------------------------------------------------------------------------------

common::TimeStamp KeyCodeStateEngine::getReferenceTime() const
{
    return common::TimeStamp::reference();
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngine::isInTimeWindow(const KeyCode& cmd,double windowLength,double lowerLimit,double upperLimit) const
{
    bool res = false;
    QMap<KeyCode,QPair<int,common::TimeStamp> >::const_iterator ppI;

    ppI = getKeyStateMapConst().find(cmd);
    if(ppI!=getKeyStateMapConst().end())
    {
        common::TimeStamp rT = getReferenceTime();
        common::TimeStamp lowerT = ppI.value().second + (lowerLimit * windowLength);
        common::TimeStamp upperT = ppI.value().second + (upperLimit * windowLength);

        if(lowerT<=rT && rT<=upperT)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngine::isDelayWindow(const KeyCode& cmd) const
{
    int state;
    bool res = false;

    state = getKeyState(cmd);
    if(state>=1 && state<=4)
    {
        res = isInTimeWindow(cmd,getDelayTime(),0.0,1.0);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngine::isDelayWindowBottom(const KeyCode& cmd) const
{
    int state;
    bool res = false;

    state = getKeyState(cmd);
    if(state>=1 && state<=4)
    {
        res = isInTimeWindow(cmd,getDelayTime(),0.0,0.7);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngine::isDelayWindowTop(const KeyCode& cmd) const
{
    int state;
    bool res = false;

    state = getKeyState(cmd);
    if(state>=1 && state<=4)
    {
        res = isInTimeWindow(cmd,getDelayTime(),0.7,1.0);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngine::isRepeatWindow(const KeyCode& cmd) const
{
    int state;
    bool res = false;

    state = getKeyState(cmd);
    if(state==5)
    {
        res = isInTimeWindow(cmd,getRepeatTime(),0.0,1.0);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool KeyCodeStateEngine::isFastSignal(const KeyCode& cmd,int repeat) const
{
    bool res = false;
    QMap<KeyCode,QPair<int,common::TimeStamp> >::const_iterator ppI;

    ppI = getKeyStateMapConst().find(cmd);
    if(ppI!=getKeyStateMapConst().end() && repeat>0)
    {
        common::TimeStamp rT = getReferenceTime();
        common::TimeStamp dT = ppI.value().second;
        if(isEqual(static_cast<tfloat64>(rT),static_cast<tfloat64>(dT)))
        {
            res = true;
        }
        else if(rT > dT)
        {
            double sRatio = static_cast<double>(rT - dT) / (static_cast<double>(repeat) * getDelayTime());
            double rLowerD = 0.7 / sRatio;
            double rUpperD = 1.0 / sRatio;
            int rLower = static_cast<int>(::floor(rLowerD));
            int rUpper = static_cast<int>(::floor(rUpperD));
            res = (rLower != rUpper) ? true : false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processRemoteCommand(const KeyCode& cmd,int repeat)
{
    //printTimeDifference("processRemoteCommand",cmd);
    switch(getKeyState(cmd))
    {
        case 0:
            processRemoteButtonNotPressed(cmd,repeat);
            break;

        case 1:
            processRemoteInitialSignalReceived(cmd,repeat);
            break;

        case 2:
            processRemoteFastRepeatSignal(cmd,repeat);
            break;

        case 3:
            processRemoteSlowRepeatSignal(cmd,repeat);
            break;

        case 4:
            processRemoteRepeatInTopDelayWindow(cmd,repeat);
            break;

        case 5:
        case 6:
            processRemoteRegularHoldSignal(cmd,repeat);
            break;
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processRemoteButtonNotPressed(const KeyCode& cmd,int repeat)
{
    int state = (repeat==0) ? 1 : 5;
    createKeyState(cmd,state,getReferenceTime());
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processRemoteInitialSignalReceived(const KeyCode& cmd,int repeat)
{
    if(repeat==0)
    {
        processClick(cmd);
        updateKeyState(cmd,1,getReferenceTime());
    }
    else if(isFastSignal(cmd,repeat))
    {
        updateKeyState(cmd,2);
    }
    else
    {
        updateKeyState(cmd,3);
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processRemoteFastRepeatSignal(const KeyCode& cmd,int repeat)
{
    if(repeat==0)
    {
        processClick(cmd);
        updateKeyState(cmd,1,getReferenceTime());
    }
    else if(isFastSignal(cmd,repeat))
    {
        if(!isDelayWindowBottom(cmd))
        {
            updateKeyState(cmd,4);
        }
    }
    else
    {
        updateKeyState(cmd,3);
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processRemoteSlowRepeatSignal(const KeyCode& cmd,int repeat)
{
    if(repeat==0)
    {
        if(isDelayWindow(cmd))
        {
            processClick(cmd);
        }
        else
        {
            processHold(cmd);
        }
        updateKeyState(cmd,1,getReferenceTime());
    }
    else if(isFastSignal(cmd,repeat))
    {
        if(isDelayWindowBottom(cmd))
        {
            updateKeyState(cmd,2);
        }
        else
        {
            updateKeyState(cmd,4);
        }
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processRemoteRepeatInTopDelayWindow(const KeyCode& cmd,int repeat)
{
    if(repeat==0)
    {
        processHold(cmd);
        updateKeyState(cmd,1,getReferenceTime());
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processRemoteRegularHoldSignal(const KeyCode& cmd,int repeat)
{
    int state = (repeat==0) ? 1 : 5;
    updateKeyState(cmd,state,getReferenceTime());
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::onProcessTimer()
{
    QList<KeyCode> cmdList;
    QMap<KeyCode,QPair<int,common::TimeStamp> >::iterator ppI;
    QList<KeyCode>::iterator ppJ;

    for(ppI=getKeyStateMap().begin();ppI!=getKeyStateMap().end();ppI++)
    {
        cmdList.append(ppI.key());
    }
    for(ppJ=cmdList.begin();ppJ!=cmdList.end();ppJ++)
    {
        processTimerCommand(*ppJ);
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processTimerCommand(const KeyCode& cmd)
{
    //printTimeDifference("processTimerCommand",cmd);
    switch(getKeyState(cmd))
    {
        case 0:
            processTimerButtonNotPressed(cmd);
            break;

        case 1:
            processTimerInitialSignalReceived(cmd);
            break;

        case 2:
            processTimerFastRepeatSignal(cmd);
            break;

        case 3:
            processTimerSlowRepeatSignal(cmd);
            break;

        case 4:
            processTimerRepeatInTopDelayWindow(cmd);
            break;

        case 5:
        case 6:
            processTimerRegularHoldSignal(cmd);
            break;
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processTimerButtonNotPressed(const KeyCode& cmd)
{}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processTimerInitialSignalReceived(const KeyCode& cmd)
{
    if(!isDelayWindow(cmd))
    {
        removeKeyState(cmd);
        processClick(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processTimerFastRepeatSignal(const KeyCode& cmd)
{
    if(!isDelayWindow(cmd))
    {
        removeKeyState(cmd);
        processClick(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processTimerSlowRepeatSignal(const KeyCode& cmd)
{
    if(!isDelayWindow(cmd))
    {
        updateKeyState(cmd,6,getReferenceTime());
        processHold(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processTimerRepeatInTopDelayWindow(const KeyCode& cmd)
{
    if(!isDelayWindowTop(cmd))
    {
        updateKeyState(cmd,6,getKeyTime(cmd) + getDelayTime());
        processHold(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processTimerRegularHoldSignal(const KeyCode& cmd)
{
    if(isRepeatWindow(cmd))
    {
        updateKeyState(cmd,6);
        processHold(cmd);
    }
    else
    {
        removeKeyState(cmd);
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processReleaseCommand(const KeyCode& cmd)
{
    //printTimeDifference("processReleaseCommand",cmd);
    switch(getKeyState(cmd))
    {
        case 0:
            processReleaseButtonNotPressed(cmd);
            break;

        case 1:
            processReleaseInitialSignalRecieved(cmd);
            break;

        case 2:
            processReleaseFastRepeatSignal(cmd);
            break;

        case 3:
            processReleaseSlowRepeatSignal(cmd);
            break;

        case 4:
            processReleaseRepeatInTopDelayWindow(cmd);
            break;

        case 5:
            processReleaseHoldSignalNotEmitted(cmd);
            break;

        case 6:
            processReleaseHoldSignalEmitted(cmd);
            break;
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processReleaseButtonNotPressed(const KeyCode& cmd)
{
    removeKeyState(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processReleaseInitialSignalRecieved(const KeyCode& cmd)
{
    if(isDelayWindow(cmd))
    {
        processClick(cmd);
    }
    else
    {
        processHold(cmd);
    }
    removeKeyState(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processReleaseFastRepeatSignal(const KeyCode& cmd)
{
    processReleaseInitialSignalRecieved(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processReleaseSlowRepeatSignal(const KeyCode& cmd)
{
    processReleaseInitialSignalRecieved(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processReleaseRepeatInTopDelayWindow(const KeyCode& cmd)
{
    processReleaseInitialSignalRecieved(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processReleaseHoldSignalNotEmitted(const KeyCode& cmd)
{
    processHold(cmd);
    removeKeyState(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processReleaseHoldSignalEmitted(const KeyCode& cmd)
{
    removeKeyState(cmd);
}

//-------------------------------------------------------------------------------------------

KeyAssignmentSPtr KeyCodeStateEngine::getKeyAssignment()
{
    return m_keyAssignment;
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::printTimeDifference(const tchar *routine,const KeyCode& cmd) const
{
    QMap<KeyCode,QPair<int,common::TimeStamp> >::const_iterator ppI;
    ppI = getKeyStateMapConst().find(cmd);
    if(ppI!=getKeyStateMapConst().end())
    {
        common::TimeStamp diffT = getReferenceTime() - ppI.value().second;
        common::Log::g_Log.print("%s - (state=%d), (key=\"%s\"), time=%.5f\n",routine,ppI.value().first,
            QKeyLineEdit::textForCode(cmd.keyCode()).toUtf8().constData(),
            static_cast<tfloat64>(diffT));
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processClick(const KeyCode& cmd)
{
    switch(getKeyAssignment()->assignedTo(cmd))
    {
        case KeyAssignment::e_keyPlay:
            playPauseClickSignal();
            break;

        case KeyAssignment::e_keyPreviousTrack:
            previousTrackClickSignal();
            break;

        case KeyAssignment::e_keyNextTrack:
            nextTrackClickSignal();
            break;

        case KeyAssignment::e_keyVolumeDown:
            volumeDownIncrementSignal();
            break;

        case KeyAssignment::e_keyVolumeUp:
            volumeUpIncrementSignal();
            break;

        case KeyAssignment::e_keyUnassigned:
        default:
            processUnassignedClick(cmd);
            break;
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processHold(const KeyCode& cmd)
{
    switch(getKeyAssignment()->assignedTo(cmd))
    {
        case KeyAssignment::e_keyPlay:
            break;

        case KeyAssignment::e_keyPreviousTrack:
            seekBackSignal();
            break;

        case KeyAssignment::e_keyNextTrack:
            seekForwardSignal();
            break;

        case KeyAssignment::e_keyVolumeDown:
            volumeDownIncrementSignal();
            break;

        case KeyAssignment::e_keyVolumeUp:
            volumeUpIncrementSignal();
            break;

        case KeyAssignment::e_keyUnassigned:
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------

void KeyCodeStateEngine::processUnassignedClick(const KeyCode& cmd)
{
#if defined(OMEGA_WIN32)
    KeyCode kAddFiles(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_O));
    if(cmd==kAddFiles)
    {
        removeKeyState(cmd);
        Q_EMIT onAddFiles();
    }

    KeyCode kAddFolder(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_F));
    if(cmd==kAddFolder)
    {
        removeKeyState(cmd);
        Q_EMIT onAddFolders();
    }

    KeyCode kSavePlaylist(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_S));
    if(cmd==kSavePlaylist)
    {
        removeKeyState(cmd);
        Q_EMIT onSavePlaylist();
    }

    KeyCode kPreferences(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_Comma));
    if(cmd==kPreferences)
    {
        removeKeyState(cmd);
        Q_EMIT onPreferences();
    }

    KeyCode kCut(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_X));
    if(cmd==kCut)
    {
        removeKeyState(cmd);
        Q_EMIT onCut();
    }

    KeyCode kCopy(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_C));
    if(cmd==kCopy)
    {
        removeKeyState(cmd);
        Q_EMIT onCopy();
    }

    KeyCode kPaste(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_V));
    if(cmd==kPaste)
    {
        removeKeyState(cmd);
        Q_EMIT onPaste();
    }

    KeyCode kSelectAll(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_A));
    if(cmd==kSelectAll)
    {
        removeKeyState(cmd);
        Q_EMIT onSelectAll();
    }

    KeyCode kRemoveTracks(static_cast<tuint32>(Qt::Key_Delete));
    KeyCode kRemoveTracksBack(static_cast<tuint32>(Qt::Key_Backspace));
    if(cmd==kRemoveTracks || cmd==kRemoveTracksBack)
    {
        removeKeyState(cmd);
        Q_EMIT onRemoveTracks();
    }

    KeyCode kHelp(static_cast<tuint32>(Qt::Key_F1));
    if(cmd==kHelp)
    {
        removeKeyState(cmd);
        Q_EMIT onHelp();
    }

#endif
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
