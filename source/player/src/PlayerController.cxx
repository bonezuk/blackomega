#include "network/inc/Resource.h"
#include "player/inc/PlayerController.h"
#include "widget/inc/AboutDialog.h"

#if defined(OMEGA_WIN32)
#include "audioio/inc/WasAPIIF.h"
#elif defined(OMEGA_MACOSX)
#include "audioio/inc/AOQueryCoreAudio.h"
#endif

#include "remote/inc/KeyCodeExclusion.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

PlayerController *PlayerController::m_instance = 0;

//-------------------------------------------------------------------------------------------

PlayerController::PlayerController() : QObject(),
	m_playerDialog(0),
	m_addFilesAction(0),
	m_addFolderAction(0),
	m_savePlaylistAction(0),
	m_preferenceAction(0),
	m_playAction(0),
	m_previousTrackAction(0),
	m_nextTrackAction(0),
	m_aboutAction(0),
	m_helpAction(0),
	m_cutAction(0),
	m_copyAction(0),
	m_pasteAction(0),
	m_deleteAction(0),
	m_selectAllAction(0),
#if defined (OMEGA_MACOSX)
	m_addFilesActionMacMenu(0),
	m_addFolderActionMacMenu(0),
	m_savePlaylistActionMacMenu(0),
	m_preferenceActionMacMenu(0),
	m_playActionMacMenu(0),
	m_previousTrackActionMacMenu(0),
	m_nextTrackActionMacMenu(0),
	m_aboutActionMacMenu(0),
	m_helpActionMacMenu(0),
	m_cutActionMacMenu(0),
	m_copyActionMacMenu(0),
	m_pasteActionMacMenu(0),
	m_deleteActionMacMenu(0),
	m_selectAllActionMacMenu(0),
	m_iTunesCollectionMenuMac(0),
	m_iTunesConfigMac(0),
#endif
	m_iTunesCollectionMenu(0),
	m_iTunesConfig(0),
	m_cliTimer(0),
	m_startFlag(false),
	m_freePlayerDialogs(),
	m_splashScreen(0),
	m_processThread(0)
{
	QEvent *eStartup = new QEvent(static_cast<QEvent::Type>(e_StartupEvent));
	QCoreApplication::postEvent(this,eStartup);
	QObject::connect(QCoreApplication::instance(),SIGNAL(aboutToQuit()),this,SLOT(onStop()));
}

//-------------------------------------------------------------------------------------------

PlayerController::~PlayerController()
{
	m_instance = 0;
}

//-------------------------------------------------------------------------------------------

PlayerController *PlayerController::instance()
{
	if(m_instance==0)
	{
		m_instance = new PlayerController;
	}
	return m_instance;
}

//-------------------------------------------------------------------------------------------

bool PlayerController::event(QEvent *e)
{
	if(e!=0)
	{
		tint t = e->type();
		
		if(t==static_cast<tint>(e_StartupEvent))
		{
			onStart();
			e->setAccepted(true);
			return true;
		}
	}
	return QObject::event(e);
}

//-------------------------------------------------------------------------------------------

void PlayerController::onStart()
{
	QSettings settings;
	
	m_startFlag = true;
	
#if defined(Q_OS_MAC)
	QDir d(QApplication::applicationDirPath());
	d.cdUp();
	d.cd("PlugIns");
	QApplication::setLibraryPaths(QStringList(d.absolutePath()));
#endif

	network::Resource::instance();
	network::Controller::ControllerSPtr ctrl(network::Controller::instance());
	
	QSharedPointer<ITunesConfig> pITunesConfig(new ITunesConfig);
	m_iTunesConfig = pITunesConfig;
	QSharedPointer<common::ProcessService> pService = qSharedPointerCast<common::ProcessService>(pITunesConfig);
	m_processThread = new common::ProcessThread(pService, 5000);
	m_processThread->start();
	
    if(m_audio.data()==0)
	{
		QStringList aList;

#if defined(OMEGA_WIN32)
		audioio::WasAPIIF::instance("wasapi");
#endif

#if defined(OMEGA_WIN32)
		aList << "win32";
#elif defined(OMEGA_MACOSX)
		aList << "coreaudio";
#elif defined(OMEGA_LINUX)
        aList << "alsa";
#endif

		m_audio = audioio::AOBase::get(aList.at(0));

        if(m_audio.data()!=0)
		{
			m_audio->setCrossfade(0.0);

			settings.beginGroup("audio");
			if(settings.contains("volume"))
			{
                QVariant vD(1.0);
                tfloat32 v = settings.value("volume",vD).toFloat();
				if(v<0.0f)
				{
					v = 0.0f;
				}
				else if(v>1.0f)
				{
					v = 1.0f;
				}
				m_audio->setVolume(v);
			}
			settings.endGroup();
		
            QObject::connect(m_audio.data(),SIGNAL(onStart(const QString&)),this,SLOT(onAudioStart(const QString&)));
            QObject::connect(m_audio.data(),SIGNAL(onPlay()),this,SLOT(onAudioPlay()));
            QObject::connect(m_audio.data(),SIGNAL(onPause()),this,SLOT(onAudioPause()));
            QObject::connect(m_audio.data(),SIGNAL(onStop()),this,SLOT(onAudioStop()));
            QObject::connect(m_audio.data(),SIGNAL(onTime(quint64)),this,SLOT(onAudioTime(quint64)));
            QObject::connect(m_audio.data(),SIGNAL(onReadyForNext()),this,SLOT(onAudioNext()));
            QObject::connect(m_audio.data(),SIGNAL(onNoNext()),this,SLOT(onAudioNoNext()));
            QObject::connect(m_audio.data(),SIGNAL(onCrossfade()),this,SLOT(onAudioCrossfade()));
            QObject::connect(m_audio.data(),SIGNAL(onVolumeChanged(tfloat64)),this,SLOT(onAudioVolumeChanged(tfloat64)));
		}
	}

	createActions();
	
#if defined(OMEGA_WIN32)
	m_cliTimer = new QTimer(this);
	QObject::connect(m_cliTimer,SIGNAL(timeout()),this,SLOT(onCLITimer()));
	m_cliTimer->start(50);
#endif

	newPlayer();
}

//-------------------------------------------------------------------------------------------

void PlayerController::onStop()
{
	QSettings settings;

	if(!m_startFlag)
	{
		return;
	}

	if(m_playerDialog!=0)
	{
		onFreeOldPlayers();
		m_playerDialog->m_playList->saveCurrentPlaylist();
		m_playerDialog->setVisible(false);
		delete m_playerDialog;
		m_playerDialog = 0;
	}

#if defined(OMEGA_WIN32)
	if(m_cliTimer!=0)
	{
		m_cliTimer->stop();
		delete m_cliTimer;
	}
#endif
	
	m_processThread->quit();
	m_processThread->wait();
	delete m_processThread;
	m_processThread = 0;
	
	m_iTunesConfig.clear();
		
    if(m_audio.data()!=0)
	{
		settings.beginGroup("audio");
		settings.setValue("volume",QVariant(m_audio->getVolume()));
		settings.endGroup();

        QObject::disconnect(m_audio.data(),SIGNAL(onStart(const QString&)),this,SLOT(onAudioStart(const QString&)));
        QObject::disconnect(m_audio.data(),SIGNAL(onPlay()),this,SLOT(onAudioPlay()));
        QObject::disconnect(m_audio.data(),SIGNAL(onPause()),this,SLOT(onAudioPause()));
        QObject::disconnect(m_audio.data(),SIGNAL(onStop()),this,SLOT(onAudioStop()));
        QObject::disconnect(m_audio.data(),SIGNAL(onTime(quint64)),this,SLOT(onAudioTime(quint64)));
        QObject::disconnect(m_audio.data(),SIGNAL(onReadyForNext()),this,SLOT(onAudioNext()));
        QObject::disconnect(m_audio.data(),SIGNAL(onNoNext()),this,SLOT(onAudioNoNext()));
        QObject::disconnect(m_audio.data(),SIGNAL(onCrossfade()),this,SLOT(onAudioCrossfade()));
        QObject::disconnect(m_audio.data(),SIGNAL(onVolumeChanged(tfloat64)),this,SLOT(onAudioVolumeChanged(tfloat64)));

        QSharedPointer<audioio::AOBase> eAudio;
		audioio::AOBase::end(m_audio);
		m_audio = eAudio;
	}

	network::Controller::end();

#if defined(OMEGA_WIN32)
	audioio::WasAPIIF::release();
#endif

	unregisterHelpBook();
	
	m_startFlag = false;
}

//-------------------------------------------------------------------------------------------

void PlayerController::onPlayerDone(int result)
{}

//-------------------------------------------------------------------------------------------

void PlayerController::onFreeOldPlayers()
{
	int i;
	
	for(i=0;i<m_freePlayerDialogs.size();i++)
	{
		Player *playerDlg = m_freePlayerDialogs.takeLast();
		delete playerDlg;
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::newPlayer()
{
	Player *playerDlg;
	QSettings wSettings;

#if defined(OMEGA_MACOSX)
	playerDlg = new Player(0,Qt::Window);
#else
	playerDlg = new Player(0,Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
#endif
    
	m_iTunesConfig->setPlaylistWidget(playerDlg->m_playList,m_iTunesCollectionMenu);

    if(m_splashScreen!=0)
	{
		m_splashScreen->finish(playerDlg);
	}

	playerDlg->setupAudio();

	if(m_playerDialog!=0)
	{
		playerDlg->m_playList->clone(m_playerDialog->m_playList);
		m_playerDialog->setVisible(false);
		m_freePlayerDialogs.append(m_playerDialog);
		QTimer::singleShot(100,this,SLOT(onFreeOldPlayers()));
	}
	else
	{
        if(m_audio.data()!=0)
		{
			playerDlg->m_playList->loadCurrentPlaylist();
		}	
	}
	
	updateShuffle(playerDlg->isShuffle());
	updateRepeat(playerDlg->isRepeat());
	
	m_playerDialog = playerDlg;
	QObject::connect(m_playerDialog,SIGNAL(finished(int)),this,SLOT(onPlayerDone(int)));
	m_playerDialog->show();
}

//-------------------------------------------------------------------------------------------

void PlayerController::createActions()
{
	m_addFilesAction = new QAction(tr("&Add File"),this);
#if defined(OMEGA_WIN32)
	m_addFilesAction->setShortcuts(QKeySequence::Open);
	m_addFilesAction->setIcon(QIcon(":/player/Resources/add_files3.png"));
#endif
	connect(m_addFilesAction,SIGNAL(triggered()),this,SLOT(onAddFiles()));
	
	m_addFolderAction = new QAction(tr("Add &Folder"),this);
#if defined(OMEGA_WIN32)
	m_addFolderAction->setShortcut(tr("Ctrl+F"));
	m_addFolderAction->setIcon(QIcon(":/player/Resources/add_dir3.png"));
#endif
	connect(m_addFolderAction,SIGNAL(triggered()),this,SLOT(onAddDirectory()));
	
	m_savePlaylistAction = new QAction(tr("Save Playlist"),this);
#if defined(OMEGA_WIN32)
	m_savePlaylistAction->setShortcut(tr("Ctrl+S"));
	m_savePlaylistAction->setIcon(QIcon(":/player/Resources/save_playlist.png"));
#endif
	connect(m_savePlaylistAction,SIGNAL(triggered()),this,SLOT(onSavePlaylist()));
	
	m_preferenceAction = new QAction(tr("Preferences"),this);
#if defined(OMEGA_WIN32)
	m_preferenceAction->setShortcut(tr("Ctrl+,"));
	m_preferenceAction->setIcon(QIcon(":/player/Resources/settings1.png"));
#endif
	connect(m_preferenceAction,SIGNAL(triggered()),this,SLOT(onSettings()));

	m_playAction = new QAction(tr("Play"),this);
	connect(m_playAction,SIGNAL(triggered()),this,SLOT(onPlay()));
	m_playAction->setEnabled(false);

	m_previousTrackAction = new QAction(tr("Previous"),this);
	connect(m_previousTrackAction,SIGNAL(triggered()),this,SLOT(onPrevious()));
	m_previousTrackAction->setEnabled(false);

	m_nextTrackAction = new QAction(tr("Next"),this);
	connect(m_nextTrackAction,SIGNAL(triggered()),this,SLOT(onNext()));
	m_nextTrackAction->setEnabled(false);

	m_shuffleAction = new QAction(tr("Shuffle"),this);
	m_shuffleAction->setCheckable(true);
	connect(m_shuffleAction,SIGNAL(toggled(bool)),this,SLOT(onShuffle(bool)));
	m_shuffleAction->setEnabled(true);
	
	m_repeatAction = new QAction(tr("Repeat"),this);
	m_repeatAction->setCheckable(true);
	connect(m_repeatAction,SIGNAL(toggled(bool)),this,SLOT(onRepeat(bool)));
	m_repeatAction->setEnabled(true);

	m_cutAction = new QAction(tr("Cu&t"),this);
#if defined(OMEGA_WIN32)
	m_cutAction->setShortcuts(QKeySequence::Cut);
	m_cutAction->setIcon(QIcon(":/player/Resources/cut.png"));
#endif
	connect(m_cutAction,SIGNAL(triggered()),this,SLOT(onCut()));
	m_cutAction->setEnabled(false);

	m_copyAction = new QAction(tr("&Copy"),this);
#if defined(OMEGA_WIN32)
	m_copyAction->setShortcuts(QKeySequence::Copy);
	m_copyAction->setIcon(QIcon(":/player/Resources/page_copy.png"));
#endif
	connect(m_copyAction,SIGNAL(triggered()),this,SLOT(onCopy()));
	m_copyAction->setEnabled(false);

	m_pasteAction = new QAction(tr("&Paste"),this);
#if defined(OMEGA_WIN32)
	m_pasteAction->setShortcuts(QKeySequence::Paste);
	m_pasteAction->setIcon(QIcon(":player/Resources/page_paste.png"));
#endif
	connect(m_pasteAction,SIGNAL(triggered()),this,SLOT(onPaste()));
	m_pasteAction->setEnabled(false);
	
	m_deleteAction = new QAction(tr("Remove Tracks"),this);
#if defined(OMEGA_WIN32)
	m_deleteAction->setShortcuts(QKeySequence::Delete);
#endif
	connect(m_deleteAction,SIGNAL(triggered()),this,SLOT(onDelete()));
	m_deleteAction->setEnabled(false);

	m_selectAllAction = new QAction(tr("Select All"),this);
#if defined(OMEGA_WIN32)
	m_selectAllAction->setShortcuts(QKeySequence::SelectAll);
#endif
	connect(m_selectAllAction,SIGNAL(triggered()),this,SLOT(onSelectAll()));

	m_iTunesCollectionMenu = new QMenu("iTunes Collection");

	m_helpAction = new QAction(tr("Black Omega Help"),this);
#if defined(OMEGA_WIN32)
	m_helpAction->setShortcut(Qt::Key_F1);
	m_helpAction->setIcon(QIcon(":/player/Resources/help.png"));
#endif
	connect(m_helpAction,SIGNAL(triggered()),this,SLOT(onHelp()));

	m_aboutAction = new QAction(tr("About Black Omega"),this);
#if defined(OMEGA_WIN32)
	m_aboutAction->setIcon(QIcon(":/player/Resources/key-icon.png"));
#endif
	connect(m_aboutAction,SIGNAL(triggered()),this,SLOT(onAbout()));


#if defined(OMEGA_MACOSX)

	m_addFilesActionMacMenu = new QAction(tr("&Add File"),this);
	m_addFilesActionMacMenu->setShortcuts(QKeySequence::Open);
	connect(m_addFilesActionMacMenu,SIGNAL(triggered()),this,SLOT(onAddFiles()));
	
	m_addFolderActionMacMenu = new QAction(tr("Add &Folder"),this);
	m_addFolderActionMacMenu->setShortcut(tr("Ctrl+F"));
	connect(m_addFolderActionMacMenu,SIGNAL(triggered()),this,SLOT(onAddDirectory()));
	
	m_savePlaylistActionMacMenu = new QAction(tr("Save Playlist"),this);
	m_savePlaylistActionMacMenu->setShortcut(tr("Ctrl+S"));
	connect(m_savePlaylistActionMacMenu,SIGNAL(triggered()),this,SLOT(onSavePlaylist()));
	
	m_preferenceActionMacMenu = new QAction(tr("Preferences"),this);
	m_preferenceActionMacMenu->setShortcut(tr("Ctrl+,"));
	connect(m_preferenceActionMacMenu,SIGNAL(triggered()),this,SLOT(onSettings()));

	m_playActionMacMenu = new QAction(tr("Play"),this);
	connect(m_playActionMacMenu,SIGNAL(triggered()),this,SLOT(onPlay()));
	m_playActionMacMenu->setEnabled(false);

	m_previousTrackActionMacMenu = new QAction(tr("Previous"),this);
	connect(m_previousTrackActionMacMenu,SIGNAL(triggered()),this,SLOT(onPrevious()));
	m_previousTrackActionMacMenu->setEnabled(false);

	m_nextTrackActionMacMenu = new QAction(tr("Next"),this);
	connect(m_nextTrackActionMacMenu,SIGNAL(triggered()),this,SLOT(onNext()));
	m_nextTrackActionMacMenu->setEnabled(false);

	m_shuffleActionMacMenu = new QAction(tr("Shuffle"),this);
	m_shuffleActionMacMenu->setCheckable(true);
	connect(m_shuffleActionMacMenu,SIGNAL(toggled(bool)),this,SLOT(onShuffle(bool)));
	m_shuffleActionMacMenu->setEnabled(true);
	
	m_repeatActionMacMenu = new QAction(tr("Repeat"),this);
	m_repeatActionMacMenu->setCheckable(true);
	connect(m_repeatActionMacMenu,SIGNAL(toggled(bool)),this,SLOT(onRepeat(bool)));
	m_repeatActionMacMenu->setEnabled(true);

	m_cutActionMacMenu = new QAction(tr("Cu&t"),this);
 	m_cutActionMacMenu->setShortcuts(QKeySequence::Cut);
	connect(m_cutActionMacMenu,SIGNAL(triggered()),this,SLOT(onCut()));
	m_cutActionMacMenu->setEnabled(false);

	m_copyActionMacMenu = new QAction(tr("&Copy"),this);
 	m_copyActionMacMenu->setShortcuts(QKeySequence::Copy);
	connect(m_copyActionMacMenu,SIGNAL(triggered()),this,SLOT(onCopy()));
	m_copyActionMacMenu->setEnabled(false);

	m_pasteActionMacMenu = new QAction(tr("&Paste"),this);
	m_pasteActionMacMenu->setShortcuts(QKeySequence::Paste);
	connect(m_pasteActionMacMenu,SIGNAL(triggered()),this,SLOT(onPaste()));
	m_pasteActionMacMenu->setEnabled(false);
	
	m_deleteActionMacMenu = new QAction(tr("Remove Tracks"),this);
	m_deleteActionMacMenu->setShortcuts(QKeySequence::Delete);
	connect(m_deleteActionMacMenu,SIGNAL(triggered()),this,SLOT(onDelete()));
	m_deleteActionMacMenu->setEnabled(false);

	m_selectAllActionMacMenu = new QAction(tr("Select All"),this);
	m_selectAllActionMacMenu->setShortcuts(QKeySequence::SelectAll);
	connect(m_selectAllActionMacMenu,SIGNAL(triggered()),this,SLOT(onSelectAll()));

	m_iTunesCollectionMenuMac = new QMenu("iTunes Collection");

	m_helpActionMacMenu = new QAction(tr("Black Omega Help"),this);
	m_helpActionMacMenu->setShortcut(Qt::Key_F1);
	connect(m_helpActionMacMenu,SIGNAL(triggered()),this,SLOT(onHelp()));

	m_aboutActionMacMenu = new QAction(tr("About Black Omega"),this);
	connect(m_aboutActionMacMenu,SIGNAL(triggered()),this,SLOT(onAbout()));

	QMenuBar *mainMenuBar = new QMenuBar(0);
	
	QMenu *fileMenu = mainMenuBar->addMenu(tr("&File"));
	fileMenu->addAction(m_addFilesActionMacMenu);
	fileMenu->addAction(m_addFolderActionMacMenu);
	fileMenu->addSeparator();
	fileMenu->addAction(m_savePlaylistActionMacMenu);
	//fileMenu->addSeparator();
	//fileMenu->addMenu(m_iTunesCollectionMenuMac);

	QMenu *editMenu = mainMenuBar->addMenu(tr("&Edit"));
	editMenu->addAction(m_cutActionMacMenu);
	editMenu->addAction(m_copyActionMacMenu);
	editMenu->addAction(m_pasteActionMacMenu);
	editMenu->addSeparator();
	editMenu->addAction(m_selectAllActionMacMenu);
	editMenu->addSeparator();
	editMenu->addAction(m_deleteActionMacMenu);

	QMenu *ctrlMenu = mainMenuBar->addMenu(tr("&Control"));
	ctrlMenu->addAction(m_playActionMacMenu);
	ctrlMenu->addAction(m_previousTrackActionMacMenu);
	ctrlMenu->addAction(m_nextTrackActionMacMenu);
	ctrlMenu->addSeparator();
	ctrlMenu->addAction(m_shuffleActionMacMenu);
	ctrlMenu->addAction(m_repeatActionMacMenu);
	
	QMenu *helpMenu = mainMenuBar->addMenu(tr("&Help"));
	helpMenu->addAction(m_aboutActionMacMenu);
	helpMenu->addAction(m_preferenceActionMacMenu);
	
	//QMenu *hMenu = mainMenuBar->addMenu(tr("Help"));
	helpMenu->addAction(m_helpActionMacMenu);
	helpMenu->addSeparator();
	
	registerHelpBook();
#endif

	defineKeyExclusions();
}

//-------------------------------------------------------------------------------------------

void PlayerController::defineKeyExclusions()
{
	remote::KeyCodeExclusionSPtr pKeyExclusion = remote::KeyCodeExclusion::instance();
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_O)),"Add File");
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_F)),"Add Folder");
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_S)),"Save Playlist");
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_Comma)),"Preferences");
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_X)),"Cut");
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_C)),"Copy");
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_V)),"Paste");
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::Key_Backspace)),"Remove Tracks");
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::Key_Delete)),"Remove Tracks");
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::ControlModifier) | static_cast<tuint32>(Qt::Key_A)),"Select All");
	pKeyExclusion->add(remote::KeyCode(static_cast<tuint32>(Qt::Key_F1)),"Show Help");
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAbout()
{
	widget::AboutDialog aboutDlg(m_playerDialog);
	aboutDlg.exec();
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAudioStart(const QString& name)
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAudioStart(name);
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAudioNext()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAudioNext();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAudioNoNext()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAudioNoNext();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAudioPlay()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAudioPlay();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAudioStop()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAudioStop();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAudioPause()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAudioPause();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAudioTime(quint64 t)
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAudioTime(t);
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAudioCrossfade()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAudioCrossfade();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAudioVolumeChanged(tfloat64 vol)
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAudioVolumeChanged(vol);
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAddFiles()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAddFiles();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onAddDirectory()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onAddDirectory();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onSavePlaylist()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onSavePlaylist();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onPlay()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onPlay();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onPrevious()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onPrevious();
	}	
}

//-------------------------------------------------------------------------------------------

void PlayerController::onNext()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onNext();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onHelp()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onHelp();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onUpdateSoftware()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onUpdateSoftware();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onCut()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->m_playList->onCut();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onCopy()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->m_playList->onCopy();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onPaste()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->m_playList->onPaste();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onDelete()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->m_playList->onDelete();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onSelectAll()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->m_playList->onSelectAll();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onCLITimer()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onCLITimer();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onSettings()
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->onSettings();
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::setPlayText(const QString& text)
{
	m_playAction->setText(text);
#if defined(OMEGA_MACOSX)
	m_playActionMacMenu->setText(text);
#endif
}

//-------------------------------------------------------------------------------------------

void PlayerController::createContextMenu(QMenu& m,bool pasteFlag)
{
	m.addAction(m_addFilesAction);
	m.addAction(m_addFolderAction);
	m.addMenu(m_iTunesCollectionMenu);
	m.addSeparator();
	m.addAction(m_savePlaylistAction);
	m.addSeparator();

	QMenu *ctrlMenu = m.addMenu(tr("Control"));
	ctrlMenu->addAction(m_playAction);
	ctrlMenu->addAction(m_previousTrackAction);
	ctrlMenu->addAction(m_nextTrackAction);
	ctrlMenu->addSeparator();
	ctrlMenu->addAction(m_shuffleAction);
	ctrlMenu->addAction(m_repeatAction);

	m.addSeparator();

	m.addAction(m_cutAction);
	m.addAction(m_copyAction);
	m.addAction(m_pasteAction);
	m_pasteAction->setEnabled(pasteFlag);
	m.addSeparator();
	m.addAction(m_selectAllAction);
	m.addSeparator();
	m.addAction(m_deleteAction);

	m.addSeparator();
	m.addAction(m_preferenceAction);
	m.addAction(m_aboutAction);
	m.addSeparator();
	m.addAction(m_helpAction);
}

//-------------------------------------------------------------------------------------------

void PlayerController::onCanPlay(bool f)
{
	m_playAction->setEnabled(f);
#if defined(OMEGA_MACOSX)
	m_playActionMacMenu->setEnabled(f);
#endif
}

//-------------------------------------------------------------------------------------------

void PlayerController::onCanPrevious(bool f)
{
	m_previousTrackAction->setEnabled(f);
#if defined(OMEGA_MACOSX)
	m_previousTrackActionMacMenu->setEnabled(f);
#endif
}

//-------------------------------------------------------------------------------------------

void PlayerController::onCanNext(bool f)
{
	m_nextTrackAction->setEnabled(f);
#if defined(OMEGA_MACOSX)
	m_nextTrackActionMacMenu->setEnabled(f);
#endif
}

//-------------------------------------------------------------------------------------------

void PlayerController::clipboardActive(bool flag)
{
	m_cutAction->setEnabled(flag);
	m_copyAction->setEnabled(flag);
	m_deleteAction->setEnabled(flag);
#if defined(OMEGA_MACOSX)
	m_cutActionMacMenu->setEnabled(flag);
	m_copyActionMacMenu->setEnabled(flag);
	m_deleteActionMacMenu->setEnabled(flag);
#endif
}

//-------------------------------------------------------------------------------------------

void PlayerController::pasteActive(bool flag)
{
	m_pasteAction->setEnabled(flag);
#if defined(OMEGA_MACOSX)
	m_pasteActionMacMenu->setEnabled(flag);
#endif
}

//-------------------------------------------------------------------------------------------

void PlayerController::progressStart()
{
	m_cutAction->setEnabled(false);
	m_copyAction->setEnabled(false);
	m_pasteAction->setEnabled(false);
	m_deleteAction->setEnabled(false);
	m_selectAllAction->setEnabled(false);
#if defined(OMEGA_MACOSX)
	m_cutActionMacMenu->setEnabled(false);
	m_copyActionMacMenu->setEnabled(false);
	m_pasteActionMacMenu->setEnabled(false);
	m_deleteActionMacMenu->setEnabled(false);
	m_selectAllActionMacMenu->setEnabled(false);
#endif
}

//-------------------------------------------------------------------------------------------

void PlayerController::progressEnd(bool pasteFlag)
{
	m_pasteAction->setEnabled(pasteFlag);
	m_selectAllAction->setEnabled(true);
#if defined(OMEGA_MACOSX)
	m_pasteActionMacMenu->setEnabled(pasteFlag);
	m_selectAllActionMacMenu->setEnabled(true);
#endif
}

//-------------------------------------------------------------------------------------------

void PlayerController::onShuffle(bool flag)
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->setShuffle(flag);
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::onRepeat(bool flag)
{
	if(m_playerDialog!=0)
	{
		m_playerDialog->setRepeat(flag);
	}
}

//-------------------------------------------------------------------------------------------

void PlayerController::updateShuffle(bool flag)
{
	m_shuffleAction->blockSignals(true);
	m_shuffleAction->setChecked(flag);
	m_shuffleAction->blockSignals(false);
#if defined (OMEGA_MACOSX)
	m_shuffleActionMacMenu->blockSignals(true);
	m_shuffleActionMacMenu->setChecked(flag);
	m_shuffleActionMacMenu->blockSignals(false);
#endif
}

//-------------------------------------------------------------------------------------------

void PlayerController::updateRepeat(bool flag)
{
	m_repeatAction->blockSignals(true);
	m_repeatAction->setChecked(flag);
	m_repeatAction->blockSignals(false);
#if defined (OMEGA_MACOSX)
	m_repeatActionMacMenu->blockSignals(true);
	m_repeatActionMacMenu->setChecked(flag);
	m_repeatActionMacMenu->blockSignals(false);
#endif
}

//-------------------------------------------------------------------------------------------

void PlayerController::setStartupSplashScreen(QSplashScreen& screen)
{
	m_splashScreen = &screen;
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
