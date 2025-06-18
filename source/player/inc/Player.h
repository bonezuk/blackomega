//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_PLAYER_H
#define __OMEGA_PLAYER_PLAYER_H
//-------------------------------------------------------------------------------------------

#include "audioio/inc/AOBase.h"
#include "engine/inc/Codec.h"
#include "player/inc/QPlaylistWidget.h"
#include "player/inc/QPlayControls.h"
#include "audioio/inc/AOBase.h"
#include "engine/inc/Codec.h"
#include "player/ui_Player.h"
#include "audioio/inc/AOQueryDevice.h"
#include "player/inc/Settings.h"
#include "player/inc/CLIPipe.h"
#include "player/inc/ITunesConfig.h"
#include "remote/inc/RemoteIF.h"
#include "common/inc/SBService.h"
#include "remote/inc/KeyControlService.h"

#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QDialog>
#include <QFileDialog>
#include <QResizeEvent>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class Player : public QDialog
{
    public:
        Q_OBJECT

        friend class SettingsCentralWidget;
        friend class SettingsAudio;
        friend class SettingsKeyboard;
        friend class PlayerController;

    public:

        typedef enum
        {
            e_Play = 0,
            e_Pause,
            e_Stop
        } PlayState;

    public:
        Player(QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~Player();

        PlayState state() const;

        bool isShuffle();
        void setShuffle(bool flag);

        bool isRepeat();
        void setRepeat(bool flag);

    protected:
        Ui::Player ui;
        QScrollArea *m_playListScrollArea;
        QPlaylistWidget *m_playList;
        QPlayControls *m_playControls;

        PlayState m_state;
        Settings *m_settingsDialog;

        // Player::m_nextCount variable: Initially set to 0. The count is set to 0 when the signal for a given track
        // starting. The count is incremented by 1 on recieving the onReadyForNext signal from the audio layer. When the
        // count increases above one then no next track information is past into the audio layer via the next method.
        int m_nextCount;

        int m_keyTimerCount;
        int m_keyTimerState;

        bool m_chTestFlag;
        PlayState m_chTestPlayState;
        common::TimeStamp m_chTestPlayTime;

        common::SBServicePtr m_sbService;

#if defined(OMEGA_MACOSX)
        remote::RemoteIFSPtr m_remoteApple;
#endif
        remote::RemoteIFSPtr m_remoteLIRC;

        common::TimeStamp m_currentPlayTime;

        QStringList m_loadFileList;
        QString m_loadDirectoryList;
        QString m_savePlaylistName;
        QString m_savePlaylistFilter;

        QString getCurrentDirectory() const;
        void setCurrentDirectory(const QString& name,bool dirFlag) const;

        void onStart();
        void onStop();

        void playFile(QPLItemBase *item);

        void createMenus();

        virtual void closeEvent(QCloseEvent *e);

        virtual void keyPressEvent(QKeyEvent *e);
        virtual void keyReleaseEvent(QKeyEvent *e);

        virtual void mousePressEvent(QMouseEvent *e);
        virtual void mouseReleaseEvent(QMouseEvent *e);

        virtual void resizeEvent(QResizeEvent *e);

        void startChannelTest();
        void stopChannelTest();

        void setupAudio();

        void onAudioStart(const QString& name);
        void onAudioNext();
        void onAudioNoNext();
        void onAudioPlay();
        void onAudioStop();
        void onAudioPause();
        void onAudioTime(quint64 t);
        void onAudioCrossfade();
        void onAudioVolumeChanged(tfloat64 vol);

        common::TimeStamp getPLItemTimeStart(QPLItemBase *item) const;
        common::TimeStamp getPLItemTimeEnd(QPLItemBase *item) const;

        void setVolumeIncrement(tfloat32 deltaVolume);

        remote::RemoteIFSPtr getRemote(bool lircFlag);
        void removeRemote(bool lircFlag);
        void connectRemoteInterface(remote::RemoteIFSPtr pRemote);
        void disconnectRemoteInterface(remote::RemoteIFSPtr pRemote);

        bool isRemoteAppleIR();
        bool isRemoteLIRC();
        bool isRemote(const QString& remoteName,bool defaultFlag);

        void doPaintUpdate();

    protected Q_SLOTS:
        void onAddFiles();
        void onAddDirectory();
        void onSavePlaylist();

        void onPlay();
        void onPause();
        void onPlayPause();
        void onPrevious();
        void onNext();
        void onSeek(double t);
        void onVolume(float v);

        void onSeekBack();
        void onSeekForward();

        void onListPlay();

        void onSettings();

        void onCLITimer();

        void onCanPlay(bool f);
        void onCanPrevious(bool f);
        void onCanNext(bool f);

        void onHelp();

        void onAddFileSelect(const QStringList& fileList);
        void onAddDirectorySelect(const QString& dirName);
        void onSavePlaylistSelect(const QString& fName,const QString& selFilter);

        void onAddFileSelectTimer();
        void onAddDirectorySelectTimer();
        void onSavePlaylistSelectTimer();

        void onCanPlayPrevious(bool flag);
        void onCanPlayCurrent(bool flag);
        void onCanPlayNext(bool flag);

        void onUpdateSoftware();

        void onVolumeDecreaseIncrement();
        void onVolumeIncreaseIncrement();
        void onVolumeDecreaseHold();
        void onVolumeIncreaseHold();

        void onShuffle(bool on);
        void onRepeat(bool on);
};

//-------------------------------------------------------------------------------------------

inline Player::PlayState Player::state() const
{
    return m_state;
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
