//-------------------------------------------------------------------------------------------
#if defined(OMEGA_IOS)
//-------------------------------------------------------------------------------------------

#include "playerapp/playerios/inc/PlayerIOSAudioSession.h"
#include "playerapp/playerios/inc/PlayerAudioIOInterface.h"

#import <Foundation/Foundation.h>
#import <AVFAudio/AVAudioSession.h>
#import <MediaPlayer/MPRemoteCommand.h>
#import <MediaPlayer/MPRemoteCommandCenter.h>
#import <MediaPlayer/MPMediaItem.h>
#import <MediaPlayer/MPNowPlayingInfoCenter.h>
#import <UiKit/UIImage.h>

//-------------------------------------------------------------------------------------------

@interface PlayerIOSInterface : NSObject
{
};
- (id) init;
- (void) audioInteruption:(NSNotification *)notification;
- (void) audioRouteChange:(NSNotification *)notification;
- (void) audioSessionReset:(NSNotification *)notification;

// Playback commands
- (MPRemoteCommandHandlerStatus) onPauseCommand:(MPRemoteCommandEvent *)event;
- (MPRemoteCommandHandlerStatus) onPlayCommand:(MPRemoteCommandEvent *)event;
- (MPRemoteCommandHandlerStatus) onStopCommand:(MPRemoteCommandEvent *)event;
- (MPRemoteCommandHandlerStatus) onTogglePlayPauseCommand:(MPRemoteCommandEvent *)event;

// Navigating Between Tracks
- (MPRemoteCommandHandlerStatus) onNextTrackCommand:(MPRemoteCommandEvent *)event;
- (MPRemoteCommandHandlerStatus) onPreviousTrackCommand:(MPRemoteCommandEvent *)event;

@end

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PlayerIOSAudioSessionImpl : public PlayerIOSAudioSession
{
    public:
        friend class PlayerIOSAudioSession;
    public:
        PlayerIOSAudioSessionImpl(QObject *parent = 0);
        virtual ~PlayerIOSAudioSessionImpl();

        virtual bool setModelAndInit(QSharedPointer<PlayListModel>& pPLModel);

        virtual void audioInteruption(NSNotification *notification);
        virtual void audioRouteChange(NSNotification *notification);
        virtual void audioSessionReset(NSNotification *notification);

        virtual MPRemoteCommandHandlerStatus onPauseCommand(MPRemoteCommandEvent *event);
        virtual MPRemoteCommandHandlerStatus onPlayCommand(MPRemoteCommandEvent *event);
        virtual MPRemoteCommandHandlerStatus onStopCommand(MPRemoteCommandEvent *event);
        virtual MPRemoteCommandHandlerStatus onTogglePlayPauseCommand(MPRemoteCommandEvent *event);

        virtual MPRemoteCommandHandlerStatus onNextTrackCommand(MPRemoteCommandEvent *event);
        virtual MPRemoteCommandHandlerStatus onPreviousTrackCommand(MPRemoteCommandEvent *event);

        virtual void onPlayStateChangedImpl();
        virtual void updateNowPlayImpl();

        virtual bool loadDevice(audioio::AOQueryDevice::Device& dev);
        virtual bool saveDevice(audioio::AOQueryDevice::Device& dev);

    protected:

        QSharedPointer<PlayListModel> m_pPLModel;
        PlayerIOSInterface *m_iosInterface;
        bool m_wasAlreadyPaused;

        virtual void printError(const char *strR, const char *strE) const;

        virtual bool init();
        virtual void close();

        virtual NSString *getNSString(const QString& qS) const;
};
//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
@implementation PlayerIOSInterface
//-------------------------------------------------------------------------------------------

- (id) init
{
    self = [super init];
    return self;
}

//-------------------------------------------------------------------------------------------

- (void) audioInteruption:(NSNotification *)notification
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        pSession->audioInteruption(notification);
    }
}

//-------------------------------------------------------------------------------------------

- (void) audioRouteChange:(NSNotification *)notification
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        pSession->audioRouteChange(notification);
    }
}

//-------------------------------------------------------------------------------------------

- (void) audioSessionReset:(NSNotification *)notification
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        pSession->audioSessionReset(notification);
    }
}

//-------------------------------------------------------------------------------------------
// Playback commands
//-------------------------------------------------------------------------------------------

- (MPRemoteCommandHandlerStatus) onPauseCommand:(MPRemoteCommandEvent *)event
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        return pSession->onPauseCommand(event);
    }
    else
    {
        return MPRemoteCommandHandlerStatusCommandFailed;
    }
}

//-------------------------------------------------------------------------------------------

- (MPRemoteCommandHandlerStatus) onPlayCommand:(MPRemoteCommandEvent *)event
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        return pSession->onPlayCommand(event);
    }
    else
    {
        return MPRemoteCommandHandlerStatusCommandFailed;
    }
}

//-------------------------------------------------------------------------------------------

- (MPRemoteCommandHandlerStatus) onStopCommand:(MPRemoteCommandEvent *)event
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        return pSession->onStopCommand(event);
    }
    else
    {
        return MPRemoteCommandHandlerStatusCommandFailed;
    }
}

//-------------------------------------------------------------------------------------------

- (MPRemoteCommandHandlerStatus) onTogglePlayPauseCommand:(MPRemoteCommandEvent *)event
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        return pSession->onTogglePlayPauseCommand(event);
    }
    else
    {
        return MPRemoteCommandHandlerStatusCommandFailed;
    }
}

//-------------------------------------------------------------------------------------------
// Navigating Between Tracks
//-------------------------------------------------------------------------------------------

- (MPRemoteCommandHandlerStatus) onNextTrackCommand:(MPRemoteCommandEvent *)event
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        return pSession->onNextTrackCommand(event);
    }
    else
    {
        return MPRemoteCommandHandlerStatusCommandFailed;
    }
}

//-------------------------------------------------------------------------------------------

- (MPRemoteCommandHandlerStatus) onPreviousTrackCommand:(MPRemoteCommandEvent *)event
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        return pSession->onPreviousTrackCommand(event);
    }
    else
    {
        return MPRemoteCommandHandlerStatusCommandFailed;
    }
}

//-------------------------------------------------------------------------------------------
@end
//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlayerIOSAudioSession::PlayerIOSAudioSession(QObject *parent) : audioio::AOCoreAudioSessionIOS(parent)
{}

//-------------------------------------------------------------------------------------------

PlayerIOSAudioSession::~PlayerIOSAudioSession()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<PlayerIOSAudioSession> PlayerIOSAudioSession::playerInstance()
{
    return m_instance.dynamicCast<PlayerIOSAudioSession>();
}

//-------------------------------------------------------------------------------------------

QSharedPointer<PlayerIOSAudioSession> PlayerIOSAudioSession::startSession(QObject *parent)
{
    QSharedPointer<PlayerIOSAudioSession> pSession = playerInstance();

    if(!pSession.isNull())
    {
        pSession->endSession();
    }

    QSharedPointer<PlayerIOSAudioSessionImpl> pSessionImpl(new PlayerIOSAudioSessionImpl(parent));
    m_instance = pSessionImpl.dynamicCast<audioio::AOCoreAudioSessionIOS>();
    return playerInstance();
}

//-------------------------------------------------------------------------------------------

void PlayerIOSAudioSession::endSession()
{
    close();
    m_instance.clear();
}

//-------------------------------------------------------------------------------------------

void PlayerIOSAudioSession::onPlayStateChanged()
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        pSession->onPlayStateChangedImpl();
    }
}

//-------------------------------------------------------------------------------------------

void PlayerIOSAudioSession::updateNowPlay()
{
    QSharedPointer<omega::PlayerIOSAudioSessionImpl> pSession = omega::PlayerIOSAudioSession::playerInstance().dynamicCast<omega::PlayerIOSAudioSessionImpl>();
    if(!pSession.isNull())
    {
        pSession->updateNowPlayImpl();
    }
}

//-------------------------------------------------------------------------------------------
// PlayerIOSAudioSessionImpl
//-------------------------------------------------------------------------------------------

PlayerIOSAudioSessionImpl::PlayerIOSAudioSessionImpl(QObject *parent) : PlayerIOSAudioSession(parent),
    m_pPLModel(),
    m_iosInterface(nil),
    m_wasAlreadyPaused(false)
{}

//-------------------------------------------------------------------------------------------

PlayerIOSAudioSessionImpl::~PlayerIOSAudioSessionImpl()
{}

//-------------------------------------------------------------------------------------------

void PlayerIOSAudioSessionImpl::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "PlayerIOSAudioSessionImpl::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool PlayerIOSAudioSessionImpl::setModelAndInit(QSharedPointer<PlayListModel>& pPLModel)
{
    m_pPLModel = pPLModel;
    return init();
}

//-------------------------------------------------------------------------------------------

bool PlayerIOSAudioSessionImpl::init()
{
    bool res = false;

    if(audioio::AOCoreAudioSessionIOS::init())
    {
        m_iosInterface = [[PlayerIOSInterface alloc] init];
        if(m_iosInterface != nil)
        {
            MPRemoteCommandCenter *commandCenter = [MPRemoteCommandCenter sharedCommandCenter];

            [commandCenter.pauseCommand addTarget:m_iosInterface action:@selector(onPauseCommand:)];
            [commandCenter.playCommand addTarget:m_iosInterface action:@selector(onPlayCommand:)];
            [commandCenter.stopCommand addTarget:m_iosInterface action:@selector(onStopCommand:)];
            [commandCenter.togglePlayPauseCommand addTarget:m_iosInterface action:@selector(onTogglePlayPauseCommand:)];

            [commandCenter.nextTrackCommand addTarget:m_iosInterface action:@selector(onNextTrackCommand:)];
            [commandCenter.previousTrackCommand addTarget:m_iosInterface action:@selector(onPreviousTrackCommand:)];

            [[NSNotificationCenter defaultCenter] addObserver:m_iosInterface selector:@selector(audioInteruption:) name:AVAudioSessionInterruptionNotification object:nil];
            [[NSNotificationCenter defaultCenter] addObserver:m_iosInterface selector:@selector(audioRouteChange:) name:AVAudioSessionRouteChangeNotification object:nil];
            [[NSNotificationCenter defaultCenter] addObserver:m_iosInterface selector:@selector(audioSessionReset:) name:AVAudioSessionMediaServicesWereResetNotification object:nil];

            QObject::connect(m_pPLModel->playbackState().data(), SIGNAL(onStateChanged()), this, SLOT(onPlayStateChanged()));
            QObject::connect(m_pPLModel->playbackState().data(), SIGNAL(onIndexChanged()), this, SLOT(updateNowPlay()));

            res = true;
        }
        else
        {
            printError("init", "Failed to allocate IOS interface");
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PlayerIOSAudioSessionImpl::close()
{
    if(m_iosInterface != nil)
    {
        MPRemoteCommandCenter *commandCenter = [MPRemoteCommandCenter sharedCommandCenter];

        QObject::disconnect(m_pPLModel.data(), SIGNAL(onStateChanged()), this, SLOT(onPlayStateChanged()));
        QObject::disconnect(m_pPLModel->playbackState().data(), SIGNAL(onIndexChanged()), this, SLOT(updateNowPlay()));

        [commandCenter.pauseCommand removeTarget:nil];
        [commandCenter.playCommand removeTarget:nil];
        [commandCenter.stopCommand removeTarget:nil];
        [commandCenter.togglePlayPauseCommand removeTarget:nil];

        [commandCenter.nextTrackCommand removeTarget:nil];
        [commandCenter.previousTrackCommand removeTarget:nil];

        [[NSNotificationCenter defaultCenter] removeObserver:m_iosInterface name:AVAudioSessionInterruptionNotification object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:m_iosInterface name:AVAudioSessionRouteChangeNotification object:nil];
        [[NSNotificationCenter defaultCenter] removeObserver:m_iosInterface name:AVAudioSessionMediaServicesWereResetNotification object:nil];

        [m_iosInterface dealloc];
        m_iosInterface = nil;
    }
    audioio::AOCoreAudioSessionIOS::close();
}

//-------------------------------------------------------------------------------------------

void PlayerIOSAudioSessionImpl::audioInteruption(NSNotification *notification)
{
    NSDictionary *interuptionDict = notification.userInfo;
    NSNumber *interNumber = (NSNumber *)[interuptionDict valueForKey:AVAudioSessionInterruptionTypeKey];
    if(interNumber != nil)
    {
        NSUInteger interruptionType = (NSUInteger)[interNumber unsignedIntValue];

        if(interruptionType == AVAudioSessionInterruptionTypeBegan)
        {
            if(m_pPLModel->playbackState()->getState() == PlaybackStateController::Play)
            {
                m_pPLModel->onPause();
                m_wasAlreadyPaused = true;
            }
            else
            {
                m_wasAlreadyPaused = false;
            }
        }
        else if(interruptionType == AVAudioSessionInterruptionTypeEnded)
        {
            NSUInteger interuptionFlags = 0;
            NSNumber *interFlags = (NSNumber *)[interuptionDict valueForKey:AVAudioSessionInterruptionOptionKey];
            if(interFlags != nil)
            {
                interuptionFlags = (NSUInteger)[interFlags unsignedIntValue];
            }
            if(interuptionFlags & AVAudioSessionInterruptionOptionShouldResume)
            {
                if(m_wasAlreadyPaused && m_pPLModel->playbackState()->getState() == PlaybackStateController::Pause)
                {
                    m_pPLModel->onPlay();
                    m_wasAlreadyPaused = false;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void PlayerIOSAudioSessionImpl::onPlayStateChangedImpl()
{
    if(m_pPLModel->playbackState()->getState() == PlaybackStateController::Play)
    {
        m_wasAlreadyPaused = false;
    }
}

//-------------------------------------------------------------------------------------------

void PlayerIOSAudioSessionImpl::audioRouteChange(NSNotification *notification)
{
    QString reasonStr;
    NSDictionary *routeDict = notification.userInfo;
    NSNumber *reasonNumber = (NSNumber *)[routeDict valueForKey:AVAudioSessionRouteChangeReasonKey];
    NSUInteger reasonCode;

    if(reasonNumber != nil)
    {
        reasonCode = (NSUInteger)[reasonNumber unsignedIntValue];
    }
    else
    {
        reasonCode = AVAudioSessionRouteChangeReasonUnknown;
    }
    switch(reasonCode)
    {
           case AVAudioSessionRouteChangeReasonNewDeviceAvailable:
               reasonStr = "A user action (such as plugging in a headset) has made a preferred audio route available.";
               break;
           case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:
               reasonStr = "The previous audio output path is no longer available";
               break;
           case AVAudioSessionRouteChangeReasonCategoryChange:
               reasonStr = "The category of the session object changed.";
               break;
           case AVAudioSessionRouteChangeReasonOverride:
               reasonStr = "The output route was overridden by the app.";
               break;
           case AVAudioSessionRouteChangeReasonWakeFromSleep:
               reasonStr = "The route changed when the device woke up from sleep.";
               break;
           case AVAudioSessionRouteChangeReasonNoSuitableRouteForCategory:
               reasonStr = "The route changed because no suitable route is available for the specified category.";
               break;
           case AVAudioSessionRouteChangeReasonUnknown:
           default:
               reasonStr = "The reason for the change is unknown";
               break;
    }
    printError("audioRouteChange", reasonStr.toUtf8().constData());
    logOutput();

    if(reasonCode!=AVAudioSessionRouteChangeReasonCategoryChange)
    {
        if(m_pPLModel->playbackState()->getState() == PlaybackStateController::Play)
        {
            m_pPLModel->onPause();
        }

        QSharedPointer<PlayerAudioIOInterface> pAudioInterface = m_pPLModel->audioInterface().dynamicCast<PlayerAudioIOInterface>();
        if(!pAudioInterface.isNull())
        {
            pAudioInterface->update();
        }
    }
}

//-------------------------------------------------------------------------------------------

void PlayerIOSAudioSessionImpl::audioSessionReset(NSNotification *notification)
{
    m_pPLModel->onStop();
    audioio::AOCoreAudioSessionIOS::init();
}

//-------------------------------------------------------------------------------------------

MPRemoteCommandHandlerStatus PlayerIOSAudioSessionImpl::onPauseCommand(MPRemoteCommandEvent *event)
{
    m_pPLModel->onPause();
    return MPRemoteCommandHandlerStatusSuccess;
}

//-------------------------------------------------------------------------------------------

MPRemoteCommandHandlerStatus PlayerIOSAudioSessionImpl::onPlayCommand(MPRemoteCommandEvent *event)
{
    m_pPLModel->onPlay();
    return MPRemoteCommandHandlerStatusSuccess;
}

//-------------------------------------------------------------------------------------------

MPRemoteCommandHandlerStatus PlayerIOSAudioSessionImpl::onStopCommand(MPRemoteCommandEvent *event)
{
    m_pPLModel->onStop();
    return MPRemoteCommandHandlerStatusSuccess;
}

//-------------------------------------------------------------------------------------------

MPRemoteCommandHandlerStatus PlayerIOSAudioSessionImpl::onTogglePlayPauseCommand(MPRemoteCommandEvent *event)
{
    m_pPLModel->onPlayPausePressed();
    return MPRemoteCommandHandlerStatusSuccess;
}

//-------------------------------------------------------------------------------------------

MPRemoteCommandHandlerStatus PlayerIOSAudioSessionImpl::onNextTrackCommand(MPRemoteCommandEvent *event)
{
    m_pPLModel->onPlayNext();
    return MPRemoteCommandHandlerStatusSuccess;
}

//-------------------------------------------------------------------------------------------

MPRemoteCommandHandlerStatus PlayerIOSAudioSessionImpl::onPreviousTrackCommand(MPRemoteCommandEvent *event)
{
    m_pPLModel->onPlayPrevious();
    return MPRemoteCommandHandlerStatusSuccess;
}


//-------------------------------------------------------------------------------------------

NSString *PlayerIOSAudioSessionImpl::getNSString(const QString& qS) const
{
    NSString *str = [NSString stringWithUTF8String:(qS.toUtf8().constData())];
    return str;
}

//-------------------------------------------------------------------------------------------

void PlayerIOSAudioSessionImpl::updateNowPlayImpl()
{
    track::db::DBInfoSPtr pInfo = m_pPLModel->itemFromId(m_pPLModel->playbackState()->getCurrentId());
    if(!pInfo.isNull())
    {
        int childIndex;
        NSMutableDictionary *songInfo = [[NSMutableDictionary alloc] init];

        childIndex = m_pPLModel->childIndexFromId(m_pPLModel->playbackState()->getCurrentId());
        if(childIndex >= 0 && childIndex < pInfo->noChildren())
        {
            if(!pInfo->child(childIndex).name().isEmpty())
            {
                NSString *trackStr = getNSString(pInfo->child(childIndex).name());
                [songInfo setObject:trackStr forKey:MPMediaItemPropertyTitle];
            }
            NSNumber *trackNumber = [NSNumber numberWithInt:(childIndex)];
            [songInfo setObject:trackNumber forKey:MPMediaItemPropertyAlbumTrackNumber];
            NSNumber *timeLength = [NSNumber numberWithDouble:(static_cast<tfloat64>(pInfo->child(childIndex).length()))];
            [songInfo setObject:timeLength forKey:MPMediaItemPropertyPlaybackDuration];
        }
        else
        {
            if(!pInfo->title().isEmpty())
            {
                [songInfo setObject:(getNSString(pInfo->title())) forKey:MPMediaItemPropertyTitle];
            }
            if(!pInfo->track().isEmpty())
            {
                NSNumber *trackNumber = [NSNumber numberWithInt:(pInfo->track().toInt())];
                [songInfo setObject:trackNumber forKey:MPMediaItemPropertyAlbumTrackNumber];
            }
            NSNumber *timeLength = [NSNumber numberWithDouble:(static_cast<tfloat64>(pInfo->length()))];
            [songInfo setObject:timeLength forKey:MPMediaItemPropertyPlaybackDuration];
        }
        if(!pInfo->artist().isEmpty())
        {
            [songInfo setObject:(getNSString(pInfo->artist())) forKey:MPMediaItemPropertyArtist];
        }
        if(!pInfo->album().isEmpty())
        {
            [songInfo setObject:(getNSString(pInfo->album())) forKey:MPMediaItemPropertyAlbumTitle];
        }
        if(!pInfo->composer().isEmpty())
        {
            [songInfo setObject:(getNSString(pInfo->composer())) forKey:MPMediaItemPropertyComposer];
        }
        if(!pInfo->genre().isEmpty())
        {
            [songInfo setObject:(getNSString(pInfo->genre())) forKey:MPMediaItemPropertyGenre];
        }
        if(!pInfo->disc().isEmpty())
        {
            NSNumber *discNumber = [NSNumber numberWithInt:(pInfo->disc().toInt())];
            [songInfo setObject:discNumber forKey:MPMediaItemPropertyDiscNumber];
        }

        if(pInfo->isImage())
        {
            track::info::Info::ImageFormat imageFormat;
            track::info::ImageInfoArray *iArray = pInfo->getImageData(imageFormat);
            if(iArray != 0 && iArray->GetData() != 0)
            {
                NSData *imageData = [NSData dataWithBytes:(iArray->GetData()) length:(iArray->GetSize())];
                UIImage *image = [UIImage imageWithData:imageData];
                if(image!=nil)
                {
                    MPMediaItemArtwork *albumArt = [[MPMediaItemArtwork alloc] initWithImage:image];
                    [songInfo setObject:albumArt forKey:MPMediaItemPropertyArtwork];
                }
            }
        }

        [[MPNowPlayingInfoCenter defaultCenter] setNowPlayingInfo:songInfo];
    }
}

//-------------------------------------------------------------------------------------------

bool PlayerIOSAudioSessionImpl::loadDevice(audioio::AOQueryDevice::Device& dev)
{
    bool res = false;
    track::db::TrackDB *trackDB = track::db::TrackDB::instance();

    if(trackDB != 0)
    {
        res = trackDB->restoreAudioDevice(dev.id(), dev);
    }
    else
    {
        printError("loadDevice", "Track database not setup");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool PlayerIOSAudioSessionImpl::saveDevice(audioio::AOQueryDevice::Device& dev)
{
    bool res = false;
    track::db::TrackDB *trackDB = track::db::TrackDB::instance();

    if(trackDB != 0)
    {
        res = trackDB->saveAudioDevice(dev);
    }
    else
    {
        printError("saveDevice", "Track database not setup");
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
