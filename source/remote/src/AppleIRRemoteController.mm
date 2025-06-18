#include "remote/inc/AppleIRRemoteController.h"

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace remote
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(RemoteIFFactory,RemoteIF, \
                            AppleIRRemoteControllerFactory,AppleIRRemoteController, \
                            "apple_remote",false)


//-------------------------------------------------------------------------------------------

bool RemoteIF::isServiceAvailable(const QString& name)
{
    bool res = false;

    if(name=="winlirc_remote")
    {
        res = true;
    }
    if(name=="apple_remote" && AppleRemoteService::isRemoteAvailable())
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

AppleIRRemoteController::AppleIRRemoteController(QObject *parent) : RemoteIF(parent),
    m_currentButton(AppleRemoteService::e_remoteButtonUnknown),
    m_timerEnableFlag(false)
{}

//-------------------------------------------------------------------------------------------

AppleIRRemoteController::~AppleIRRemoteController()
{}

//-------------------------------------------------------------------------------------------

bool AppleIRRemoteController::start()
{
    return (!AppleRemoteService::instance(this).isNull()) ? true : false;
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteController::stop()
{
    AppleRemoteService::release();
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteController::remoteEvent(RemoteEvent *e)
{
    AppleIRRemoteServiceEvent *pEvent = dynamic_cast<AppleIRRemoteServiceEvent *>(e);
    if(pEvent!=0)
    {
        AppleRemoteService::Button newButton = pEvent->button();
        switch(newButton)
        {
            case AppleRemoteService::e_remoteButtonCentre:
                playPauseClickSignal();
                break;
                
            case AppleRemoteService::e_remoteButtonCentreHold:
                break;
                
            case AppleRemoteService::e_remoteButtonPlusPress:
                volumeUpIncrementSignal();
                startTimer();
                break;
                
            case AppleRemoteService::e_remoteButtonPlusRelease:
                stopTimer();
                break;
                
            case AppleRemoteService::e_remoteButtonMinusPress:
                volumeDownIncrementSignal();
                startTimer();
                break;
                
            case AppleRemoteService::e_remoteButtonMinusRelease:
                stopTimer();
                break;
                
            case AppleRemoteService::e_remoteButtonLeftTouch:
                previousTrackClickSignal();
                break;
                
            case AppleRemoteService::e_remoteButtonLeftPress:
                seekBackSignal();
                startTimer();
                break;
                
            case AppleRemoteService::e_remoteButtonLeftRelease:
                stopTimer();
                break;
                
            case AppleRemoteService::e_remoteButtonRightTouch:
                nextTrackClickSignal();
                break;
                
            case AppleRemoteService::e_remoteButtonRightPress:
                seekForwardSignal();
                startTimer();
                break;
                
            case AppleRemoteService::e_remoteButtonRightRelease:
                stopTimer();
                break;
                
            case AppleRemoteService::e_remoteButtonPlay:
                playPauseClickSignal();
                break;
                
            case AppleRemoteService::e_remoteButtonPlayHold:
                break;
                
            case AppleRemoteService::e_remoteButtonMenu:
                break;
                
            case AppleRemoteService::e_remoteButtonMenuHold:
                break;
                
            case AppleRemoteService::e_remoteButtonUnknown:
            default:
                break;
        }
        m_currentButton = newButton;
    }
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteController::onTimer(RemoteIF *pRemote)
{
    AppleIRRemoteController *pAppleRemoteIF = dynamic_cast<AppleIRRemoteController *>(pRemote);
    if(pAppleRemoteIF!=0)
    {
        pAppleRemoteIF->onTimerImplementation();
    }
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteController::onTimerImplementation()
{
    if(m_timerEnableFlag)
    {
        switch(m_currentButton)
        {
            case AppleRemoteService::e_remoteButtonLeftPress:
                seekBackSignal();
                break;

            case AppleRemoteService::e_remoteButtonRightPress:
                seekForwardSignal();
                break;

            case AppleRemoteService::e_remoteButtonMinusPress:
                volumeDownIncrementSignal();
                break;
            
            case AppleRemoteService::e_remoteButtonPlusPress:
                volumeUpIncrementSignal();
                break;
            
            default:
                break;
        }
    }
}

//-------------------------------------------------------------------------------------------

bool AppleIRRemoteController::setupTimer()
{
    return true;
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteController::freeTimer()
{}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteController::startTimer()
{
    m_timerEnableFlag = true;
}

//-------------------------------------------------------------------------------------------

void AppleIRRemoteController::stopTimer()
{
    m_timerEnableFlag = false;
}

//-------------------------------------------------------------------------------------------
} // namespace remote
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
