#include "network/inc/Resource.h"
#include "player/inc/Player.h"
#include "player/inc/Settings.h"
#include "player/inc/PlayerController.h"

#if defined(OMEGA_MACOSX)
#include "audioio/inc/AOQueryCoreAudio.h"
#endif

#include <QBitmap>
#include <QMessageBox>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

Player::Player(QWidget *parent,Qt::WindowFlags f) : QDialog(parent,f),
    m_playListScrollArea(0),
    m_playList(0),
    m_playControls(0),
    m_state(e_Stop),
    m_settingsDialog(0),
    m_nextCount(0),
    m_keyTimerCount(0),
    m_keyTimerState(0),
    m_chTestFlag(false),
    m_chTestPlayState(e_Stop),
    m_chTestPlayTime(0),
#if defined(OMEGA_MACOSX)
    m_remoteApple(),
#endif
    m_remoteLIRC(),
    m_currentPlayTime(0),
    m_loadFileList(),
    m_loadDirectoryList(),
    m_savePlaylistName(),
    m_savePlaylistFilter()
{
    ui.setupUi(this);
    
    m_playControls = new QPlayControls(this);
    QVBoxLayout *cLayout = new QVBoxLayout;
    cLayout->addWidget(m_playControls);
    cLayout->setSpacing(0);
    cLayout->setContentsMargins(0,0,0,0);
    ui.m_controlFrame->setLayout(cLayout);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_playListScrollArea = new QFixedHorizontalScrollArea(this);
    m_playListScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_playListScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_playListScrollArea->setBackgroundRole(QPalette::Base);
    
    m_playList = new QPlaylistWidget(m_playListScrollArea);
    m_playListScrollArea->setWidget(m_playList);
    
    QVBoxLayout *pLayout = new QVBoxLayout;
    pLayout->addWidget(m_playListScrollArea);
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0,0,0,0);
    ui.m_listFrame->setLayout(pLayout);
    
    m_playList->setPlayer(this);
    
    if(f & Qt::FramelessWindowHint)
    {
        ui.m_listSpacer->changeSize(50,1,QSizePolicy::Minimum,QSizePolicy::Minimum);
    }
    
    QObject::connect(ui.m_addFileButton,SIGNAL(clicked()),this,SLOT(onAddFiles()));
    QObject::connect(ui.m_addDirButton,SIGNAL(clicked()),this,SLOT(onAddDirectory()));
    QObject::connect(ui.m_settingsButton,SIGNAL(clicked()),this,SLOT(onSettings()));
    QObject::connect(ui.m_helpButton,SIGNAL(clicked()),this,SLOT(onHelp()));

    m_playControls->setPlaylistWidget(m_playList);
            
    QObject::connect(m_playControls,SIGNAL(onPlay()),this,SLOT(onPlay()));
    QObject::connect(m_playControls,SIGNAL(onPause()),this,SLOT(onPause()));
    QObject::connect(m_playControls,SIGNAL(onPrevious()),this,SLOT(onPrevious()));
    QObject::connect(m_playControls,SIGNAL(onNext()),this,SLOT(onNext()));
    QObject::connect(m_playControls,SIGNAL(onSeek(double)),this,SLOT(onSeek(double)));
    QObject::connect(m_playControls,SIGNAL(onVolume(float)),this,SLOT(onVolume(float)));
    QObject::connect(m_playControls,SIGNAL(onOpen()),this,SLOT(onAddFiles()));

    QObject::connect(m_playControls,SIGNAL(onCanPlay(bool)),this,SLOT(onCanPlay(bool)));
    QObject::connect(m_playControls,SIGNAL(onCanPrevious(bool)),this,SLOT(onCanPrevious(bool)));
    QObject::connect(m_playControls,SIGNAL(onCanNext(bool)),this,SLOT(onCanNext(bool)));

    QObject::connect(m_playList,SIGNAL(onPlay()),this,SLOT(onListPlay()));

    QObject::connect(m_playControls,SIGNAL(onCanPrevious(bool)),this,SLOT(onCanPlayPrevious(bool)));
    QObject::connect(m_playControls,SIGNAL(onCanPlay(bool)),this,SLOT(onCanPlayCurrent(bool)));
    QObject::connect(m_playControls,SIGNAL(onCanNext(bool)),this,SLOT(onCanPlayNext(bool)));

    QObject::connect(m_playControls,SIGNAL(onShuffle(bool)),this,SLOT(onShuffle(bool)));
    QObject::connect(m_playControls,SIGNAL(onRepeat(bool)),this,SLOT(onRepeat(bool)));
    
    onStart();
}

//-------------------------------------------------------------------------------------------

Player::~Player()
{}

//-------------------------------------------------------------------------------------------

QString Player::getCurrentDirectory() const
{
    QString dirName;
    QSettings settings;
    
    settings.beginGroup("file_settings");
    if(settings.contains("last_directory"))
    {
        dirName = settings.value("last_directory").toString();
    }
    settings.endGroup();
    if(dirName.isEmpty())
    {
        dirName = PlayerController::instance()->iTunesConfig()->getMusicDirectory();
    }
    return dirName;
}

//-------------------------------------------------------------------------------------------

void Player::setCurrentDirectory(const QString& name,bool dirFlag) const
{
    QString dirName;
    QSettings settings;
    
    if(!dirFlag)
    {
        QDir d(name);
        dirName = d.path();
        dirName.chop(d.dirName().length());
    }
    else
    {
        dirName = name;
    }
    
    settings.beginGroup("file_settings");
    settings.setValue("last_directory",QVariant(dirName));
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------

void Player::closeEvent(QCloseEvent *e)
{
    if(e!=0)
    {
        onStop();
        QEvent *eStop = new QEvent(static_cast<QEvent::Type>(PlayerController::e_StopEvent));
        QCoreApplication::postEvent(this,eStop);
    }
    QDialog::closeEvent(e);
}

//-------------------------------------------------------------------------------------------

void Player::setupAudio()
{
    PlayerController *pCtrl = PlayerController::instance();
    QSharedPointer<audioio::AOBase> pAudio = pCtrl->audio();
    if(pAudio.data()!=0)
    {
        m_playControls->setVolume(pAudio->getVolume());
    }
    else
    {
        ui.m_controlFrame->setEnabled(false);
        ui.m_listFrame->setEnabled(false);
        ui.m_addFileButton->setEnabled(false);
        ui.m_addDirButton->setEnabled(false);
        ui.m_settingsButton->setEnabled(false);

        QMessageBox::critical(this, "No Sound Devices", "Unable to find any sound drivers or playback devices on this computer.");
    }
}

//-------------------------------------------------------------------------------------------

void Player::onStart()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onStart()\n");
#endif

    m_sbService = common::SBServiceFactory::createShared(SBFACTORYKEY,this);
    QObject::connect(m_sbService.data(),SIGNAL(onLoadFiles(const QStringList&)),this,SLOT(onAddFileSelect(const QStringList&)));
    QObject::connect(m_sbService.data(),SIGNAL(onLoadDirectory(const QString&)),this,SLOT(onAddDirectorySelect(const QString&)));
    QObject::connect(m_sbService.data(),SIGNAL(onSaveFile(const QString&,const QString&)),this,SLOT(onSavePlaylistSelect(const QString&,const QString&)));

    QSettings settings;
    settings.beginGroup("windowState");
    if(settings.contains("mainWindowGeometry"))
    {
        restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    }
    settings.endGroup();
        
    setResult(0);

    remote::KeyControlServiceSPtr pKeyService = remote::KeyControlService::instance(this);
    if(pKeyService.data()!=0)
    {
#if defined(OMEGA_WIN32)
        QObject::connect(pKeyService.data(),SIGNAL(onAddFiles()),this,SLOT(onAddFiles()));
        QObject::connect(pKeyService.data(),SIGNAL(onAddFolders()),this,SLOT(onAddDirectory()));
        QObject::connect(pKeyService.data(),SIGNAL(onSavePlaylist()),this,SLOT(onSavePlaylist()));
        QObject::connect(pKeyService.data(),SIGNAL(onPreferences()),this,SLOT(onSettings()));
        QObject::connect(pKeyService.data(),SIGNAL(onCut()),m_playList,SLOT(onCut()));
        QObject::connect(pKeyService.data(),SIGNAL(onCopy()),m_playList,SLOT(onCopy()));
        QObject::connect(pKeyService.data(),SIGNAL(onPaste()),m_playList,SLOT(onPaste()));
        QObject::connect(pKeyService.data(),SIGNAL(onSelectAll()),m_playList,SLOT(onSelectAll()));
        QObject::connect(pKeyService.data(),SIGNAL(onRemoveTracks()),m_playList,SLOT(onDelete()));
        QObject::connect(pKeyService.data(),SIGNAL(onHelp()),this,SLOT(onHelp()));
#endif
        connectRemoteInterface(pKeyService);
    }

#if defined(OMEGA_WIN32)
    if(windowFlags() & Qt::FramelessWindowHint)
    {
        ui.m_buttonFrame->setVisible(false);
    }
#elif defined(OMEGA_MACOSX)        
    ui.m_buttonFrame->setVisible(false);
    
    // Setup new remote control
    if(isRemoteAppleIR())
    {
        getRemote(false);
    }
#endif

    if(isRemoteLIRC())
    {
        getRemote(true);
    }
}

//-------------------------------------------------------------------------------------------

void Player::onStop()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onStop()\n");
#endif


    QSettings settings;
    if(!(windowState() & Qt::WindowMaximized))
    {
        settings.beginGroup("windowState");
        settings.setValue("mainWindowGeometry",saveGeometry());
        settings.endGroup();
    }

    if(m_settingsDialog!=0)
    {
        delete m_settingsDialog;
        m_settingsDialog = 0;
    }

    {
        remote::KeyControlServiceSPtr pKeyService = remote::KeyControlService::instance();
        if(pKeyService.data()!=0)
        {
            disconnectRemoteInterface(pKeyService);
        }
    }
    remote::KeyControlService::release();

    removeRemote(true);
#if defined(OMEGA_MACOSX)
    removeRemote(false);
#endif

    QObject::disconnect(ui.m_addFileButton,SIGNAL(clicked()),this,SLOT(onAddFiles()));
    QObject::disconnect(ui.m_addDirButton,SIGNAL(clicked()),this,SLOT(onAddDirectory()));
    QObject::disconnect(ui.m_settingsButton,SIGNAL(clicked()),this,SLOT(onSettings()));
    QObject::disconnect(ui.m_helpButton,SIGNAL(clicked()),this,SLOT(onHelp()));
            
    QObject::disconnect(m_playControls,SIGNAL(onPlay()),this,SLOT(onPlay()));
    QObject::disconnect(m_playControls,SIGNAL(onPause()),this,SLOT(onPause()));
    QObject::disconnect(m_playControls,SIGNAL(onPrevious()),this,SLOT(onPrevious()));
    QObject::disconnect(m_playControls,SIGNAL(onNext()),this,SLOT(onNext()));
    QObject::disconnect(m_playControls,SIGNAL(onSeek(double)),this,SLOT(onSeek(double)));
    QObject::disconnect(m_playControls,SIGNAL(onVolume(float)),this,SLOT(onVolume(float)));
    QObject::disconnect(m_playControls,SIGNAL(onOpen()),this,SLOT(onAddFiles()));

    QObject::disconnect(m_playControls,SIGNAL(onCanPlay(bool)),this,SLOT(onCanPlay(bool)));
    QObject::disconnect(m_playControls,SIGNAL(onCanPrevious(bool)),this,SLOT(onCanPrevious(bool)));
    QObject::disconnect(m_playControls,SIGNAL(onCanNext(bool)),this,SLOT(onCanNext(bool)));

    QObject::disconnect(m_playList,SIGNAL(onPlay()),this,SLOT(onListPlay()));

    QObject::disconnect(m_playControls,SIGNAL(onCanPrevious(bool)),this,SLOT(onCanPlayPrevious(bool)));
    QObject::disconnect(m_playControls,SIGNAL(onCanPlay(bool)),this,SLOT(onCanPlayCurrent(bool)));
    QObject::disconnect(m_playControls,SIGNAL(onCanNext(bool)),this,SLOT(onCanPlayNext(bool)));
    
    QObject::disconnect(m_sbService.data(),SIGNAL(onLoadFiles(const QStringList&)),this,SLOT(onAddFileSelect(const QStringList&)));
    QObject::disconnect(m_sbService.data(),SIGNAL(onLoadDirectory(const QString&)),this,SLOT(onAddDirectorySelect(const QString&)));
    QObject::disconnect(m_sbService.data(),SIGNAL(onSaveFile(const QString&,const QString&)),this,SLOT(onSavePlaylistSelect(const QString&,const QString&)));
}

//-------------------------------------------------------------------------------------------

void Player::onAddFiles()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onAddFile()\n");
#endif

    QString dirName,filter;
    
    dirName = getCurrentDirectory();
    filter  = "Music & Playlist Files (*.mp3 *.m4a *.m4b *.ogg *.flac *.wav *.aif *.aiff *.m3u *.m3u8 *.pls *.xspf *.mpc *.mp+ *.mpp *.ape *.wv);;";
    filter += "Music Files (*.mp3 *.m4a *.m4b *.ogg *.flac *.wav *.aif *.aiff *.mpc *.mp+ *.mpp *.ape *.wv);;";
    filter += "Playlist Files (*.m3u *.m3u8 *.pls *.xspf);;";
    filter += "All Files (*.*)";

    m_sbService->loadFilesDialog(this,"Add Music Files",dirName,filter);
}

//-------------------------------------------------------------------------------------------

void Player::onAddFileSelect(const QStringList& fileList)
{
    m_loadFileList = fileList;
    QTimer::singleShot(200,this,SLOT(onAddFileSelectTimer()));
}

//-------------------------------------------------------------------------------------------

void Player::onAddFileSelectTimer()
{
    QStringList fileList = m_loadFileList;

    if(fileList.size() > 0)
    {
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
        for(int i=0;i<fileList.size();i++)
        {
            common::Log::g_Log.print("Player::onAddFileSelectTimer - %s\n",fileList.at(i).toUtf8().constData());
        }
#endif
    
        m_playList->addFiles(fileList,m_playList->lastItem());
        setCurrentDirectory(fileList.at(0),false);
    }
    m_loadFileList.clear();
}

//-------------------------------------------------------------------------------------------

void Player::onAddDirectory()
{
    QString dirName;
    
    dirName = getCurrentDirectory();    
    m_sbService->loadDirDialog(this,"Add Directory",dirName);
}

//-------------------------------------------------------------------------------------------

void Player::onAddDirectorySelect(const QString& dirName)
{
    m_loadDirectoryList = dirName;
    QTimer::singleShot(200,this,SLOT(onAddDirectorySelectTimer()));
}

//-------------------------------------------------------------------------------------------

void Player::onAddDirectorySelectTimer()
{
    QString dirName = m_loadDirectoryList;
    if(!dirName.isEmpty())
    {
        m_playList->addDirectory(dirName,true,m_playList->lastItem());
        setCurrentDirectory(dirName,true);
    }
    m_loadDirectoryList = "";
}

//-------------------------------------------------------------------------------------------

void Player::onSavePlaylist()
{
    QString dirName,filter;

    dirName = getCurrentDirectory();
    filter = "M3U Playlist File (*.m3u);;M3U Unicode Playlist (*.m3u8);;PLS Playlist File (*.pls);;XML Shareable Playlist (*.xspf)";
    m_sbService->saveFileDialog(this,"Save Playlist",dirName,filter);
}

//-------------------------------------------------------------------------------------------

void Player::onSavePlaylistSelect(const QString& fName,const QString& selFilter)
{
    m_savePlaylistName = fName;
    m_savePlaylistFilter = selFilter;
    QTimer::singleShot(200,this,SLOT(onSavePlaylistSelectTimer()));
}

//-------------------------------------------------------------------------------------------

void Player::onSavePlaylistSelectTimer()
{
    QString fName(m_savePlaylistName);
    QString selFilter(m_savePlaylistFilter);
    QString fileName(fName);

    m_savePlaylistName = "";
    m_savePlaylistFilter = "";

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("%s\n",fileName.toUtf8().constData());
#endif

    if(!fileName.isEmpty())
    {
        QString ext = engine::Codec::getFileExtension(fileName).toLower().trimmed();
    
#if !defined(OMEGA_MAC_STORE)
        if(ext!="m3u" && ext!="m3u8" && ext!="pls" && ext!="xspf")
        {
            if(selFilter=="XML Shareable Playlist (*.xspf)")
            {
                ext = "xspf";
            }
            else if(selFilter=="M3U Unicode Playlist (*.m3u8)")
            {
                ext = "m3u8";
            }
            else if(selFilter=="PLS Playlist File (*.pls)")
            {
                ext = "pls";
            }
            else
            {
                ext = "m3u";
            }
            if(fileName.at(fileName.size()-1)!=QChar('.'))
            {
                ext = "." + ext;
            }
            fileName += ext;
        }
        else
        {
            bool rFlag = false;

            if(selFilter=="XML Shareable Playlist (*.xspf)" && ext!="xspf")
            {
                ext = "xspf";
                rFlag = true;
            }
            else if(selFilter=="M3U Unicode Playlist (*.m3u8)" && ext!="m3u8")
            {
                ext = "m3u8";
                rFlag = true;
            }
            else if(selFilter=="PLS Playlist File (*.pls)" && ext!="pls")
            {
                ext = "pls";
                rFlag = true;
            }
            else if(selFilter=="M3U Playlist File (*.m3u)" && ext!="m3u")
            {
                ext = "m3u";
                rFlag = true;
            }
            if(rFlag)
            {
                int i;

                for(i=fileName.size()-1;i>=0;i--)
                {
                    if(fileName.at(i)==QChar('.'))
                    {
                        break;
                    }
                }
                if(i>=0)
                {
                    fileName = fileName.mid(0,i);
                }
                fileName += "." + ext;
            }
        }
#else
        if(ext!="m3u" && ext!="m3u8" && ext!="pls" && ext!="xspf")
        {
            fileName += ".xspf";
        }        
#endif
        
        m_playList->savePlaylist(fileName,false);
        setCurrentDirectory(fileName,false);
    }
}

//-------------------------------------------------------------------------------------------

void Player::onPlay()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onPlay()\n");
#endif

    stopChannelTest();

    if(PlayerController::instance()->audio().data()!=0)
    {
        if(m_state==e_Stop)
        {
            QPLItemBase *pItem;
            
            if(m_playControls->isShuffle())
            {
                m_playList->buildShuffleList();
                pItem = m_playList->nextShufflePlayItem();
                m_playList->setCurrentPlayItem(pItem);
            }
            else
            {
                pItem = m_playList->currentPlayItem();
            }
        
            if(pItem!=0)
            {
                PlayerController::instance()->audio()->open(pItem->info()->getFilename());
                m_currentPlayTime = 0;
            }
        }
        else if(m_state==e_Pause)
        {
            PlayerController::instance()->audio()->play();
        }
        else
        {
            onPause();
        }
    }
}

//-------------------------------------------------------------------------------------------

void Player::onPause()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onPause()\n");
#endif

    stopChannelTest();

    if(PlayerController::instance()->audio().data()!=0)
    {
        if(m_state==e_Play)
        {
            PlayerController::instance()->audio()->pause();
        }
        else
        {
            onPlay();
        }
    }
}

//-------------------------------------------------------------------------------------------

void Player::onPrevious()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onPrevious()\n");
#endif
    
    QPLItemBase *item = m_playList->previousPlayItem();
    
    stopChannelTest();
    if(item!=0)
    {
        playFile(item);
    }
    else
    {
        onAudioStop();
    }
    doPaintUpdate();
}

//-------------------------------------------------------------------------------------------

void Player::onNext()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onNext()\n");
#endif

    QPLItemBase *item = m_playList->nextPlayItem();
    
    stopChannelTest();
    if(item!=0)
    {
        playFile(item);
    }
    else
    {
        onAudioStop();
    }
    doPaintUpdate();
}

//-------------------------------------------------------------------------------------------

void Player::onSeek(double t)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onSeek - %.8f\n",static_cast<tfloat64>(t));
#endif

    stopChannelTest();
    if(PlayerController::instance()->audio().data()!=0)
    {
        QPLItemBase *pItem = m_playList->currentPlayItem();
        common::TimeStamp sT(t);
        if(pItem!=0 && pItem->subTrackIndex()>=0)
        {
            sT += pItem->info()->child(pItem->subTrackIndex()).startTime();
        }
        PlayerController::instance()->audio()->seek(sT);
    }
}

//-------------------------------------------------------------------------------------------

void Player::onVolume(float v)
{
    if(PlayerController::instance()->audio().data()!=0)
    {
        PlayerController::instance()->audio()->setVolume(v);
    }
}

//-------------------------------------------------------------------------------------------

void Player::onListPlay()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onListPlay()\n");
#endif

    stopChannelTest();
    if(PlayerController::instance()->audio().data()!=0)
    {
        QPLItemBase *pItem = m_playList->currentPlayItem();

        if(pItem!=0)
        {
            if(m_state==e_Stop)
            {
                if(m_playControls->isShuffle())
                {
                    m_playList->buildShuffleList(pItem);
                }

                if(pItem!=0)
                {
                    if(pItem->subTrackIndex()>=0)
                    {
                        PlayerController::instance()->audio()->open(pItem->info()->getFilename(),pItem->info()->child(pItem->subTrackIndex()).startTime(),pItem->info()->child(pItem->subTrackIndex()).length());
                    }
                    else
                    {
                        PlayerController::instance()->audio()->open(pItem->info()->getFilename());
                    }
                    m_currentPlayTime = 0;
                }
            }
            else
            {
                playFile(pItem);
            }
        }
        else
        {
            PlayerController::instance()->audio()->stop();
        }
    }
}

//-------------------------------------------------------------------------------------------

void Player::onAudioStart(const QString& name)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onAudioStart - %s\n",name.toUtf8().constData());
#endif

    if(!m_chTestFlag)
    {
        QPLItemBase *pItem,*cItem;
    
        m_nextCount = 0;
        cItem = m_playList->currentPlayItem();
        pItem = m_playList->currentQueuePlayItem();
        if(pItem==0)
        {
            pItem = cItem;
        }
        if(pItem!=0)
        {
            if(pItem->info()->getFilename()==name)
            {
                m_playControls->setPlayback(pItem);
            }
            else if(cItem!=0 && cItem->info()->getFilename()==name)
            {
                pItem = cItem;
                m_playControls->setPlayback(pItem);
            }
            else
            {
                m_playControls->setPlayback(name);
            }
        }
        else if(cItem!=0)
        {
            pItem = cItem;
            m_playControls->setPlayback(pItem);
        }
        m_state = e_Play;
        m_playList->setCurrentPlayItem(pItem);
        m_playList->setCurrentQueuePlayItem(0);
        doPaintUpdate();
        PlayerController::instance()->setPlayText("Pause");
    }
}

//-------------------------------------------------------------------------------------------

void Player::onAudioNext()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onAudioNext()\n");
#endif

    if(!m_chTestFlag)
    {
        if(PlayerController::instance()->audio().data()!=0)
        {
            QPLItemBase *item = (m_playControls->isShuffle()) ? m_playList->nextShufflePlayItem() : m_playList->nextPlayItem();

            m_nextCount++;
            if(m_nextCount<=1)
            {
                if(item==0 && m_playControls->isRepeat())
                {
                    if(m_playControls->isShuffle())
                    {
                        m_playList->buildShuffleList();
                        item = m_playList->nextShufflePlayItem();
                    }
                    else
                    {
                        item = m_playList->firstPlayItem();
                    }
                }
                
                if(item!=0)
                {
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
                    common::Log::g_Log.print("Player::onAudioNext - call AOBase::next - %s\n",item->info()->getFilename().toUtf8().constData());
#endif

                    if(item->subTrackIndex()>=0)
                    {
                        PlayerController::instance()->audio()->next(item->info()->getFilename(),item->info()->child(item->subTrackIndex()).startTime(),item->info()->child(item->subTrackIndex()).length());
                    }
                    else
                    {
                        PlayerController::instance()->audio()->next(item->info()->getFilename());
                    }
                    m_playList->setCurrentQueuePlayItem(item);
                }
                
                doPaintUpdate();
            }
        }
    }
    else
    {
        if(m_settingsDialog!=0)
        {
            QString nextFile = m_settingsDialog->nextSpeakerFile();
            if(!nextFile.isEmpty())
            {
                PlayerController::instance()->audio()->next(nextFile);
                m_playControls->setPlayback(nextFile);
            }
        }
        else
        {
            stopChannelTest();
        }
    }
}

//-------------------------------------------------------------------------------------------
// TODO - the onNoNext signal is sent in response to the fact that the next codec in sequence
// could not be enqueued. As it stands calling into onAudioNext will increment the next counter
// which has already been incremented by the onReadyForNext signal. Thus the codec is not requeued
// in the approriate manner. But rather the audio device needs to be reopened at the end of the next
// codec. The current implementation probably but this needs to be fixed such the codec can be queue
// and caching started while the current one completes its playback. Once it has completed then at
// the start of the next one the audio device and cyclic buffer are reinitialized accordingly.
//-------------------------------------------------------------------------------------------

void Player::onAudioNoNext()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onAudioNoNext()\n");
#endif

    onAudioNext();
}

//-------------------------------------------------------------------------------------------

void Player::onAudioPlay()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onAudioPlay()\n");
#endif

    m_playControls->setPlaying(true);
    m_state = e_Play;
    doPaintUpdate();
    PlayerController::instance()->setPlayText("Pause");
}

//-------------------------------------------------------------------------------------------

void Player::onAudioStop()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onAudioStop()\n");
#endif

    if(!m_chTestFlag)
    {
        if(PlayerController::instance()->audio().data()!=0)
        {
            QPLItemBase *item = (m_playControls->isShuffle()) ? m_playList->nextShufflePlayItem() : m_playList->nextPlayItem();

            if(item!=0)
            {
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
                common::Log::g_Log.print("Player::onAudioStop - 1 call AOBase::next %s\n",item->info()->getFilename().toUtf8().constData());
#endif

                if(item->subTrackIndex()>=0)
                {
                    PlayerController::instance()->audio()->next(item->info()->getFilename(),item->info()->child(item->subTrackIndex()).startTime(),item->info()->child(item->subTrackIndex()).length());
                }
                else
                {
                    PlayerController::instance()->audio()->next(item->info()->getFilename());
                }
                m_playList->setCurrentQueuePlayItem(item);
            }
            else
            {
                if(m_playControls->isRepeat())
                {
                    if(m_playControls->isShuffle())
                    {
                        m_playList->buildShuffleList();
                        item = m_playList->nextShufflePlayItem();
                    }
                    else
                    {
                        item = m_playList->firstPlayItem();
                    }
                    
                    if(item!=0)
                    {
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
                common::Log::g_Log.print("Player::onAudioStop - 2 call AOBase::next %s\n",item->info()->getFilename().toUtf8().constData());
#endif
                    
                        if(item->subTrackIndex()>=0)
                        {
                            PlayerController::instance()->audio()->next(item->info()->getFilename(),item->info()->child(item->subTrackIndex()).startTime(),item->info()->child(item->subTrackIndex()).length());
                        }
                        else
                        {
                            PlayerController::instance()->audio()->next(item->info()->getFilename());
                        }
                        m_playList->setCurrentQueuePlayItem(item);
                    }
                }
                else
                {
                    m_playList->setPlayItemToStart();
                    m_playControls->setPlayback(QString());
                    m_state = e_Stop;
                }
            }
            doPaintUpdate();
        }
        setWindowTitle("Black Omega");
        PlayerController::instance()->setPlayText("Play");
    }
    else
    {
        if(m_settingsDialog!=0)
        {
            QString nextFile = m_settingsDialog->nextSpeakerFile();
            if(!nextFile.isEmpty())
            {
                PlayerController::instance()->audio()->open(nextFile);
                m_playControls->setPlayback(nextFile);
            }
            else
            {
                stopChannelTest();
            }
        }
        else
        {
            stopChannelTest();
        }
    }
}

//-------------------------------------------------------------------------------------------

void Player::onAudioPause()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onAudioPause()\n");
#endif

    m_playControls->setPlaying(false);
    m_state = e_Pause;
    doPaintUpdate();
    setWindowTitle("Black Omega");
    PlayerController::instance()->setPlayText("Play");
}

//-------------------------------------------------------------------------------------------

void Player::onAudioTime(quint64 t)
{
    QString wTitle;
    QPLItemBase *pItem = m_playList->currentPlayItem();
    common::TimeStamp pT(t),dT;

    if(!m_chTestFlag)
    {
        m_chTestPlayTime = pT;
    }

    if(pItem!=0 && pItem->subTrackIndex()>=0)
    {
        const track::info::Info::ChildInfo& c = pItem->info()->child(pItem->subTrackIndex());
        m_playControls->setTime(pT - c.startTime());
        m_currentPlayTime = pT - c.startTime();
    }
    else
    {
        m_playControls->setTime(pT);
        m_currentPlayTime = pT;
    }
    
    if(pItem!=0)
    {
        if(pItem->subTrackIndex()>=0)
        {
            const track::info::Info::ChildInfo& c = pItem->info()->child(pItem->subTrackIndex());

            if(pT > c.startTime())
            {
                dT = pT - c.startTime();
            }
            else
            {
                dT = 0;
            }

            if(m_playControls->timeToEndFlag())
            {
                if(c.length() >= dT)
                {
                    dT = c.length() - dT;
                }
                else
                {
                    dT = 0;
                }
            }
            else
            {
                if(dT > c.length())
                {
                    dT = c.length();
                }
            }
            wTitle = c.name();
        }
        else
        {
            if(m_playControls->timeToEndFlag())
            {
                if(pItem->info()->length() >= pT)
                {
                    dT = pItem->info()->length() - pT;
                }
                else
                {
                    dT = 0;
                }
            }
            else
            {
                dT = pT;
                if(dT > pItem->info()->length())
                {
                    dT = pItem->info()->length();
                }
            }
            wTitle = pItem->info()->title();
        }
    }
    else
    {
        dT = pT;
        wTitle = "";
    }

    if(!wTitle.isEmpty())
    {
        wTitle = m_playList->getTrackLength(dT) + " " + wTitle;
    }
    else
    {
        wTitle = m_playList->getTrackLength(dT);
    }
    setWindowTitle(wTitle);
}

//-------------------------------------------------------------------------------------------

void Player::onAudioCrossfade()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onAudioCrossfade()\n");
#endif
}

//-------------------------------------------------------------------------------------------

void Player::onAudioVolumeChanged(tfloat64 vol)
{
    m_playControls->setVolume(static_cast<tfloat32>(vol));
}

//-------------------------------------------------------------------------------------------

void Player::playFile(QPLItemBase *item)
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::playFile()\n");
#endif

    if(item!=0 && PlayerController::instance()->audio().data()!=0)
    {
        if(!(m_state==e_Play && (item->type()==QPLItemBase::e_AlbumTrack || item->type()==QPLItemBase::e_Single)))
        {
            if(item->type()==QPLItemBase::e_Album)
            {
                item = m_playList->nextPlayItem(item);
                if(item!=0 && (item->type()==QPLItemBase::e_AlbumTrack || item->type()==QPLItemBase::e_Single))
                {
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
                    common::Log::g_Log.print("Player::playFile:1 call AOBase::open - %s\n",item->info()->getFilename().toUtf8().constData());
#endif

                    m_state = e_Stop;
                    if(item->subTrackIndex()>=0)
                    {
                        PlayerController::instance()->audio()->open(item->info()->getFilename(),item->info()->child(item->subTrackIndex()).startTime(),item->info()->child(item->subTrackIndex()).length());
                    }
                    else
                    {
                        PlayerController::instance()->audio()->open(item->info()->getFilename());
                    }
                    m_playList->setCurrentPlayItem(item);
                }
                else
                {
                    onStop();
                }
            }
            else
            {
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
                    common::Log::g_Log.print("Player::playFile:2 call AOBase::open - %s\n",item->info()->getFilename().toUtf8().constData());
#endif

                m_state = e_Stop;
                if(item->subTrackIndex()>=0)
                {
                    PlayerController::instance()->audio()->open(item->info()->getFilename(),item->info()->child(item->subTrackIndex()).startTime(),item->info()->child(item->subTrackIndex()).length());
                }
                else
                {
                    PlayerController::instance()->audio()->open(item->info()->getFilename());
                }
                m_playList->setCurrentPlayItem(item);
            }
        }
        else
        {
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
            common::Log::g_Log.print("Player::playFile:1 call AOBase::nextCrossfade - %s\n",item->info()->getFilename().toUtf8().constData());
#endif

            if(item->subTrackIndex()>=0)
            {
                PlayerController::instance()->audio()->nextCrossfade(item->info()->getFilename(),item->info()->child(item->subTrackIndex()).startTime(),item->info()->child(item->subTrackIndex()).length());
            }
            else
            {
                PlayerController::instance()->audio()->nextCrossfade(item->info()->getFilename());
            }
            m_playList->setCurrentPlayItem(item);
        }
        m_currentPlayTime = 0;
    }
}

//-------------------------------------------------------------------------------------------

void Player::onSettings()
{
    if(PlayerController::instance()->audio().data()!=0)
    {
        if(m_settingsDialog!=0)
        {
            delete m_settingsDialog;
        }
        m_settingsDialog = new Settings(PlayerController::instance()->audio(),this);
        m_settingsDialog->show();
    }
}

//-------------------------------------------------------------------------------------------

void Player::keyPressEvent(QKeyEvent *e)
{
    if(e->key()!=Qt::Key_Escape)
    {
        QDialog::keyPressEvent(e);
    }
}

//-------------------------------------------------------------------------------------------

void Player::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key()==Qt::Key_Backspace || e->key()==Qt::Key_Delete)
    {
        m_playList->doDelete();
    }
    QDialog::keyReleaseEvent(e);
}

//-------------------------------------------------------------------------------------------

void Player::mousePressEvent(QMouseEvent *e)
{
    QDialog::mousePressEvent(e);
}

//-------------------------------------------------------------------------------------------

void Player::mouseReleaseEvent(QMouseEvent *e)
{
    QDialog::mouseReleaseEvent(e);
}

//-------------------------------------------------------------------------------------------

void Player::onCLITimer()
{
#if defined(OMEGA_WIN32)
    CLIPipe *cliPipe = CLIPipe::instance();

    if(cliPipe!=0)
    {
        int timerState = 0;

        while(cliPipe->process())
        {
            int i;
            QPLItemBase *cPlayItem = m_playList->currentPlayItem();
            CLIPipe::Command cmd;
            QString pData;
            QStringList fList;
            bool enFlag = true;

            if(timerState==0)
            {
                PlayerController::instance()->cliTimer()->stop();
                timerState = 1;
            }

            while(cmd=cliPipe->next(pData),cmd!=CLIPipe::eCmdNone)
            {
                if(cmd==CLIPipe::eCmdQueueFiles)
                {
                    fList.append(pData);
                }
                else
                {
                    fList.append(pData);
                    enFlag = false;
                }
            }

            if(enFlag || cPlayItem==0)
            {
                for(i=0;i<fList.size();i++)
                {
                    const QString& name = fList.at(i);

                    switch(common::DiskOps::fileType(name))
                    {
                        case 1:
                            {
                                QStringList sList;
                                sList.append(name);
                                m_playList->addFiles(sList,m_playList->lastItem());
                            }
                            break;

                        case 2:
                            m_playList->addDirectory(name,true,m_playList->lastItem());
                            break;

                        default:
                            break;
                    }
                }
            }
            else
            {
                for(i=fList.size()-1;i>=0;i--)
                {
                    const QString& name = fList.at(i);

                    switch(common::DiskOps::fileType(name))
                    {
                        case 1:
                            {
                                QStringList sList;
                                sList.append(name);
                                m_playList->addFiles(sList,cPlayItem);
                            }
                            break;

                        case 2:
                            m_playList->addDirectory(name,true,cPlayItem);
                            break;

                        default:
                            break;
                    }
                }
            }

            if(!enFlag)
            {
                onNext();
            }
        }
        if(timerState==1)
        {
            PlayerController::instance()->cliTimer()->start(50);
        }
    }
#endif
}

//-------------------------------------------------------------------------------------------

void Player::onCanPlay(bool f)
{
    PlayerController::instance()->onCanPlay(f);
}

//-------------------------------------------------------------------------------------------

void Player::onCanPrevious(bool f)
{
    PlayerController::instance()->onCanPrevious(f);
}

//-------------------------------------------------------------------------------------------

void Player::onCanNext(bool f)
{
    PlayerController::instance()->onCanNext(f);
}

//-------------------------------------------------------------------------------------------

void Player::onHelp()
{
    showHelp();
}

//-------------------------------------------------------------------------------------------

void Player::onCanPlayPrevious(bool flag)
{
    if(flag && m_playList->previousPlayItem()==0)
    {
        flag = false;
    }
    onCanPrevious(flag);
}

//-------------------------------------------------------------------------------------------

void Player::onCanPlayCurrent(bool flag)
{
    if(flag && m_playList->currentPlayItem()==0)
    {
        flag = false;
    }
    onCanPlay(flag);
}

//-------------------------------------------------------------------------------------------

void Player::onCanPlayNext(bool flag)
{
    if(flag && m_playList->nextPlayItem()==0)
    {
        flag = false;
    }
    onCanNext(flag);
}

//-------------------------------------------------------------------------------------------

void Player::onUpdateSoftware()
{}

//-------------------------------------------------------------------------------------------

void Player::startChannelTest()
{
    if(m_settingsDialog!=0)
    {
        if(!m_chTestFlag)
        {
            QString testFilename = m_settingsDialog->nextSpeakerFile();
            if(!testFilename.isEmpty())
            {
                m_chTestFlag = true;
                m_chTestPlayState = m_state;
                PlayerController::instance()->audio()->open(testFilename);
                m_playControls->setPlayback(testFilename);
                m_currentPlayTime = 0;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void Player::stopChannelTest()
{
    if(m_chTestFlag)
    {
        QPLItemBase *pItem = m_playList->currentPlayItem();

        m_chTestFlag = false;
        m_state = m_chTestPlayState;
        if(m_state!=e_Stop && pItem!=0)
        {
            m_playControls->setPlayback(pItem);
            PlayerController::instance()->audio()->open(pItem->info()->getFilename(),m_chTestPlayTime);
            m_currentPlayTime = m_chTestPlayTime;
            if(m_state==e_Pause)
            {
                PlayerController::instance()->audio()->pause();
            }
        }
        else
        {
            m_playControls->setPlayback("");
            m_state = e_Stop;
        }
    }
}

//-------------------------------------------------------------------------------------------

void Player::resizeEvent(QResizeEvent *e)
{
    const int c_listFrameOffset = 50;

    QSize nSize = e->size();

    if(windowFlags() & Qt::FramelessWindowHint)
    {
        int ix,iy;
        QImage maskImage(e->size(),QImage::Format_ARGB32);
        QImage panelMaskImage(":/panel/Resources/panel/backAMask.png");

        maskImage.fill(Qt::black);
        for(iy=0;iy<panelMaskImage.height();iy++)
        {
            for(ix=0;ix<panelMaskImage.width();ix++)
            {
                maskImage.setPixel(ix+1,iy,panelMaskImage.pixel(ix,iy));
            }
        }

        for(ix=0;ix<nSize.width();ix++)
        {
            maskImage.setPixel(ix,0,qRgba(255,255,255,255));
        }
        for(iy=0;iy<panelMaskImage.height();iy++)
        {
            maskImage.setPixel(0,iy,qRgba(255,255,255,255));
        }

        for(iy=panelMaskImage.height();iy<nSize.height();iy++)
        {
            for(ix=0;ix<c_listFrameOffset;ix++)
            {
                maskImage.setPixel(ix,iy,qRgba(255,255,255,255));
            }
        }

        QPixmap pixMap = QPixmap::fromImage(maskImage);

        QBitmap bmMask = pixMap.createMaskFromColor(Qt::white);

        QRegion maskRegion(bmMask);
        setMask(maskRegion);

        ui.m_listFrame->setMinimumWidth(nSize.width()-c_listFrameOffset);
    }
    else
    {
        // Problem in Qt5 where window couldn't be resized down in width. Hence comment out setMinimumWidth code.
        // ui.m_listFrame->setMinimumWidth(nSize.width()-10);
    }
    QDialog::resizeEvent(e);
}

//--------------------------------------------------------------------------------------------

common::TimeStamp Player::getPLItemTimeStart(QPLItemBase *item) const
{
    common::TimeStamp startT(0);
    
    if(item!=0 && item->subTrackIndex()>=0)
    {
        startT = item->info()->child(item->subTrackIndex()).startTime();
    }
    return startT;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp Player::getPLItemTimeEnd(QPLItemBase *item) const
{
    common::TimeStamp endT(0);
    
    if(item!=0)
    {
        if(item->subTrackIndex()>=0)
        {
            const track::info::Info::ChildInfo& child = item->info()->child(item->subTrackIndex());
            endT = child.startTime() + child.length();
        }
        else
        {
            endT = item->info()->length();
        }
    }
    return endT;
}

//-------------------------------------------------------------------------------------------

void Player::connectRemoteInterface(remote::RemoteIFSPtr pRemote)
{
    QObject::connect(pRemote.data(),SIGNAL(playPauseClick()),this,SLOT(onPlayPause()));
    QObject::connect(pRemote.data(),SIGNAL(previousTrackClick()),this,SLOT(onPrevious()));
    QObject::connect(pRemote.data(),SIGNAL(nextTrackClick()),this,SLOT(onNext()));
    QObject::connect(pRemote.data(),SIGNAL(seekBack()),this,SLOT(onSeekBack()));
    QObject::connect(pRemote.data(),SIGNAL(seekForward()),this,SLOT(onSeekForward()));
    QObject::connect(pRemote.data(),SIGNAL(volumeDownIncrement()),this,SLOT(onVolumeDecreaseIncrement()));
    QObject::connect(pRemote.data(),SIGNAL(volumeDownHold()),this,SLOT(onVolumeDecreaseHold()));
    QObject::connect(pRemote.data(),SIGNAL(volumeUpIncrement()),this,SLOT(onVolumeIncreaseIncrement()));
    QObject::connect(pRemote.data(),SIGNAL(volumeUpHold()),this,SLOT(onVolumeIncreaseHold()));
}

//-------------------------------------------------------------------------------------------

void Player::disconnectRemoteInterface(remote::RemoteIFSPtr pRemote)
{
    QObject::disconnect(pRemote.data(),SIGNAL(playPauseClick()),this,SLOT(onPlayPause()));
    QObject::disconnect(pRemote.data(),SIGNAL(previousTrackClick()),this,SLOT(onPrevious()));
    QObject::disconnect(pRemote.data(),SIGNAL(nextTrackClick()),this,SLOT(onNext()));
    QObject::disconnect(pRemote.data(),SIGNAL(seekBack()),this,SLOT(onSeekBack()));
    QObject::disconnect(pRemote.data(),SIGNAL(seekForward()),this,SLOT(onSeekForward()));
    QObject::disconnect(pRemote.data(),SIGNAL(volumeDownIncrement()),this,SLOT(onVolumeDecreaseIncrement()));
    QObject::disconnect(pRemote.data(),SIGNAL(volumeDownHold()),this,SLOT(onVolumeDecreaseHold()));
    QObject::disconnect(pRemote.data(),SIGNAL(volumeUpIncrement()),this,SLOT(onVolumeIncreaseIncrement()));
    QObject::disconnect(pRemote.data(),SIGNAL(volumeUpHold()),this,SLOT(onVolumeIncreaseHold()));
}

//-------------------------------------------------------------------------------------------

remote::RemoteIFSPtr Player::getRemote(bool lircFlag)
{
    remote::RemoteIFSPtr pRemote;
    
    if(lircFlag)
    {
        if(m_remoteLIRC.data()==0 && remote::RemoteIF::isServiceAvailable("winlirc_remote"))
        {
            remote::RemoteIFSPtr pNewRemote = remote::RemoteIFFactory::createShared("winlirc_remote",this);
            pRemote = pNewRemote;
        }
        else
        {
            return m_remoteLIRC;
        }
    }
#if defined(OMEGA_MACOSX)
    else
    {
        if(m_remoteApple.data()==0 && remote::RemoteIF::isServiceAvailable("apple_remote"))
        {
            remote::RemoteIFSPtr pNewRemote(remote::RemoteIFFactory::createShared("apple_remote",this));
            pRemote = pNewRemote;
        }
        else
        {
            return m_remoteApple;
        }
    }
#endif

    bool res = false;
    
    if(pRemote.data()!=0)
    {
        if(pRemote->start())
        {
            connectRemoteInterface(pRemote);
            res = true;            
        }
    }
    
    if(res)
    {
        if(lircFlag)
        {
            m_remoteLIRC = pRemote;
        }
#if defined(OMEGA_MACOSX)
        else
        {
            m_remoteApple = pRemote;
        }
#endif
    }
    else
    {
        remote::RemoteIFSPtr pDelRemote;
        pRemote = pDelRemote;
    }
    return pRemote;
}

//-------------------------------------------------------------------------------------------

void Player::removeRemote(bool lircFlag)
{
    remote::RemoteIFSPtr pRemote,pDelRemote;

    if(lircFlag)
    {
        pRemote = m_remoteLIRC;
        m_remoteLIRC = pDelRemote;
    }
#if defined(OMEGA_MACOSX)
    else
    {
        pRemote = m_remoteApple;
        m_remoteApple = pDelRemote;
    }
#endif

    if(pRemote.data()!=0)
    {
        pRemote->stop();
        disconnectRemoteInterface(pRemote);
    }
}

//-------------------------------------------------------------------------------------------

void Player::onPlayPause()
{
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onPlayPause()\n");
#endif

    if(m_state==e_Play)
    {
        onPause();
    }
    else
    {
        onPlay();
    }
}

//-------------------------------------------------------------------------------------------

void Player::onSeekBack()
{
    const tfloat64 c_seekTimeSkip = 5.0;
    
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onSeekBack()\n");
#endif

    stopChannelTest();
    if(m_state==e_Play)
    {
        tfloat64 newT = static_cast<tfloat64>(m_currentPlayTime) - c_seekTimeSkip;
        if(newT < 0.0)
        {
            QPLItemBase *item = m_playList->previousPlayItem();
            
            if(item!=0)
            {
                common::TimeStamp sT = getPLItemTimeStart(item);
                common::TimeStamp eT = getPLItemTimeEnd(item);
                common::TimeStamp lT(0);
                common::TimeStamp seekT(0);
                
                if(eT > sT)
                {
                    lT = eT - sT;
                }
                
                newT = 0.0 - newT;
                if(lT > newT)
                {
                    seekT = lT - newT;
                }
                seekT += sT;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
                common::Log::g_Log.print("Player::onSeekBack - call AOBase::open - %s\n",item->info()->getFilename().toUtf8().constData());
#endif                
                if(item->isSubTrack())
                {
                    PlayerController::instance()->audio()->open(item->info()->getFilename(),seekT,item->info()->child(item->subTrackIndex()).length());
                }
                else
                {
                    PlayerController::instance()->audio()->open(item->info()->getFilename(),seekT);
                }
                m_playList->setCurrentPlayItem(item);
                m_currentPlayTime = seekT - sT;
            }
            else
            {
                onStop();
                m_currentPlayTime = 0;
            }
        }
        else
        {
            onSeek(newT);
            m_currentPlayTime = newT;
        }
    }
}

//-------------------------------------------------------------------------------------------

void Player::onSeekForward()
{
    const tfloat64 c_seekTimeSkip = 5.0;
    
#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
    common::Log::g_Log.print("Player::onSeekForward()\n");
#endif

    stopChannelTest();
    if(m_state==e_Play)
    {    
        QPLItemBase *cItem = m_playList->currentPlayItem();
        
        if(cItem!=0)
        {
            common::TimeStamp newT = m_currentPlayTime + c_seekTimeSkip;
            common::TimeStamp cEndT = getPLItemTimeEnd(cItem);
            if(cEndT < newT)
            {
                QPLItemBase *nItem = m_playList->nextPlayItem();
                if(nItem!=0)
                {
                    common::TimeStamp sT = getPLItemTimeStart(nItem);
                    common::TimeStamp eT = getPLItemTimeEnd(nItem);
                    common::TimeStamp lT(0);
                    common::TimeStamp seekT(0);
                    
                    if(sT < eT)
                    {
                        lT = eT - sT;
                    }
                    newT -= cEndT;
                    if(newT < lT)
                    {
                        seekT = newT;
                    }
                    seekT += sT;

#if defined(OMEGA_PLAYBACK_DEBUG_MESSAGES)
                common::Log::g_Log.print("Player::onSeekForward - call AOBase::open - %s\n",nItem->info()->getFilename().toUtf8().constData());
#endif                

                    if(nItem->isSubTrack())
                    {
                        PlayerController::instance()->audio()->open(nItem->info()->getFilename(),seekT,nItem->info()->child(nItem->subTrackIndex()).length());
                    }
                    else
                    {
                        PlayerController::instance()->audio()->open(nItem->info()->getFilename(),seekT);
                    }

                    m_playList->setCurrentPlayItem(nItem);
                    m_currentPlayTime = seekT - sT;
                }
                else
                {
                    onStop();
                    m_currentPlayTime = 0;
                }
            }
            else
            {
                onSeek(static_cast<tfloat64>(newT));
                m_currentPlayTime = newT;
            }
        }
        else
        {
            onStop();
            m_currentPlayTime = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------

void Player::setVolumeIncrement(tfloat32 deltaVolume)
{
    if(PlayerController::instance()->audio().data()!=0)
    {
        tfloat32 cVolume = PlayerController::instance()->audio()->getVolume();
        cVolume += deltaVolume;
        if(cVolume < 0.0f)
        {
            cVolume = 0.0f;
        }
        else if(cVolume > 1.0f)
        {
            cVolume = 1.0f;
        }
        PlayerController::instance()->audio()->setVolume(cVolume);
        m_playControls->setVolume(cVolume);
    }
}

//-------------------------------------------------------------------------------------------

void Player::onVolumeDecreaseIncrement()
{
    setVolumeIncrement(-0.025f);
}

//-------------------------------------------------------------------------------------------

void Player::onVolumeIncreaseIncrement()
{
    setVolumeIncrement(0.025f);
}

//-------------------------------------------------------------------------------------------

void Player::onVolumeDecreaseHold()
{
    setVolumeIncrement(-0.075f);
}

//-------------------------------------------------------------------------------------------

void Player::onVolumeIncreaseHold()
{
    setVolumeIncrement(0.075f);
}

//-------------------------------------------------------------------------------------------

bool Player::isRemoteAppleIR()
{
    return isRemote("appleIR",true);
}

//-------------------------------------------------------------------------------------------

bool Player::isRemoteLIRC()
{
    return isRemote("lircIR",false);
}

//-------------------------------------------------------------------------------------------

bool Player::isRemote(const QString& remoteName,bool defaultFlag)
{
    QSettings settings;
    bool pFlag;
    
    settings.beginGroup("remote");
    if(settings.contains(remoteName))
    {
        pFlag = settings.value(remoteName,QVariant(defaultFlag)).toBool();
    }
    else
    {
        pFlag = defaultFlag;
    }
    settings.endGroup();
    return pFlag;
}

//-------------------------------------------------------------------------------------------

void Player::doPaintUpdate()
{
    update();
    m_playListScrollArea->update();
    m_playList->update();
}

//-------------------------------------------------------------------------------------------

bool Player::isShuffle()
{
    return m_playControls->isShuffle();
}

//-------------------------------------------------------------------------------------------

void Player::setShuffle(bool flag)
{
    m_playControls->setShuffle(flag);
    m_playList->buildShuffleList();
}

//-------------------------------------------------------------------------------------------

bool Player::isRepeat()
{
    return m_playControls->isRepeat();
}

//-------------------------------------------------------------------------------------------

void Player::setRepeat(bool flag)
{
    m_playControls->setRepeat(flag);
}

//-------------------------------------------------------------------------------------------

void Player::onShuffle(bool on)
{
    m_playList->buildShuffleList();
    PlayerController::instance()->updateShuffle(on);
}

//-------------------------------------------------------------------------------------------

void Player::onRepeat(bool on)
{
    PlayerController::instance()->updateRepeat(on);
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
