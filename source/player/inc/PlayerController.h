//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_PLAYERCONTROLLER_H
#define __OMEGA_PLAYER_PLAYERCONTROLLER_H
//-------------------------------------------------------------------------------------------

#include "player/inc/Player.h"

#include <QSplashScreen>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class PlayerController : public QObject
{
    public:
        Q_OBJECT

    public:
        typedef enum
        {
            e_StartupEvent = QEvent::User + 10000,
            e_StopEvent
        } PlayerControllerEvent;

        virtual ~PlayerController();

        static PlayerController *instance();

        QSharedPointer<audioio::AOBase> audio();

        QTimer *cliTimer();

        ITunesConfig *iTunesConfig();

        void setPlayText(const QString& text);
        void createContextMenu(QMenu& m,bool pasteFlag);
        void onCanPlay(bool f);
        void onCanPrevious(bool f);
        void onCanNext(bool f);

        void clipboardActive(bool flag);
        void pasteActive(bool flag);

        void progressStart();
        void progressEnd(bool pasteFlag);

        void updateShuffle(bool flag);
        void updateRepeat(bool flag);

        void setStartupSplashScreen(QSplashScreen& screen);

    protected:
        static PlayerController *m_instance;

        QSharedPointer<audioio::AOBase> m_audio;

        Player *m_playerDialog;

        QAction *m_addFilesAction;
        QAction *m_addFolderAction;
        QAction *m_savePlaylistAction;
        QAction *m_preferenceAction;
        QAction *m_playAction;
        QAction *m_previousTrackAction;
        QAction *m_nextTrackAction;
        QAction *m_aboutAction;
        QAction *m_helpAction;
        QAction *m_cutAction;
        QAction *m_copyAction;
        QAction *m_pasteAction;
        QAction *m_deleteAction;
        QAction *m_selectAllAction;
        QAction *m_shuffleAction;
        QAction *m_repeatAction;

#if defined (OMEGA_MACOSX)
        QAction *m_addFilesActionMacMenu;
        QAction *m_addFolderActionMacMenu;
        QAction *m_savePlaylistActionMacMenu;
        QAction *m_preferenceActionMacMenu;
        QAction *m_playActionMacMenu;
        QAction *m_previousTrackActionMacMenu;
        QAction *m_nextTrackActionMacMenu;
        QAction *m_aboutActionMacMenu;
        QAction *m_helpActionMacMenu;
        QAction *m_cutActionMacMenu;
        QAction *m_copyActionMacMenu;
        QAction *m_pasteActionMacMenu;
        QAction *m_deleteActionMacMenu;
        QAction *m_selectAllActionMacMenu;
        QAction *m_shuffleActionMacMenu;
        QAction *m_repeatActionMacMenu;

        QMenu *m_iTunesCollectionMenuMac;
        ITunesConfig *m_iTunesConfigMac;
#endif


        QMenu *m_iTunesCollectionMenu;
        QSharedPointer<ITunesConfig> m_iTunesConfig;

        QTimer *m_cliTimer;

        bool m_startFlag;

        QList<Player *> m_freePlayerDialogs;

        QSplashScreen *m_splashScreen;

        common::ProcessThread *m_processThread;

        PlayerController();

        void onStart();

        void createActions();

        virtual bool event(QEvent *e);

        void newPlayer();

        void defineKeyExclusions();

    protected Q_SLOTS:
        void onStop();

        void onAudioStart(const QString& name);
        void onAudioNext();
        void onAudioNoNext();
        void onAudioPlay();
        void onAudioStop();
        void onAudioPause();
        void onAudioTime(quint64 t);
        void onAudioCrossfade();
        void onAudioVolumeChanged(tfloat64 vol);

        void onAddFiles();
        void onAddDirectory();
        void onSavePlaylist();
        void onPlay();
        void onPrevious();
        void onNext();
        void onHelp();
        void onUpdateSoftware();
        void onCut();
        void onCopy();
        void onPaste();
        void onDelete();
        void onSelectAll();
        void onSettings();
        void onAbout();

        void onCLITimer();

        void onPlayerDone(int result);
        void onFreeOldPlayers();

        void onShuffle(bool flag);
        void onRepeat(bool flag);
};

//-------------------------------------------------------------------------------------------

inline ITunesConfig *PlayerController::iTunesConfig()
{
    return m_iTunesConfig.data();
}

//-------------------------------------------------------------------------------------------

inline QTimer *PlayerController::cliTimer()
{
    return m_cliTimer;
}

//-------------------------------------------------------------------------------------------

inline QSharedPointer<audioio::AOBase> PlayerController::audio()
{
    return m_audio;
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
