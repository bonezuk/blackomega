//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_SETTINGS_H
#define __OMEGA_PLAYER_SETTINGS_H
//-------------------------------------------------------------------------------------------

#include "player/inc/SettingsAudio.h"
#include "player/inc/SettingsFile.h"
#include "player/inc/SettingsKeyboard.h"
#include "player/inc/SettingsGeneral.h"
#include "player/inc/SettingsITunes.h"

#include <QMainWindow>
#include <QToolBar>
#include <QStackedWidget>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class Player;

//-------------------------------------------------------------------------------------------

class SettingsCentralWidget : public QWidget
{
    public:
        Q_OBJECT

    public:
        SettingsCentralWidget(QSharedPointer<audioio::AOBase> pAudio,Player *player,QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~SettingsCentralWidget();

        QString nextSpeakerFile();

        Player *getPlayer();

    protected:

        Player *m_player;

        SettingsAudio *m_widgetAudio;
        SettingsKeyboard *m_widgetKeyboard;
#if defined(OMEGA_WIN32) || defined(OMEGA_LINUX)
        SettingsFile *m_widgetFile;
        QTabWidget *m_settingTab;
#elif defined(OMEGA_MACOSX)
        QStackedWidget *m_settingStack;
#endif

        SettingsGeneral *m_widgetGeneral;
        SettingsITunes *m_widgetITunes;

        virtual void showEvent(QShowEvent *evt);
        virtual void hideEvent(QHideEvent *evt);

#if defined(OMEGA_MACOSX)
    public slots:
        void onAudioPage();
        void onKeyboardPage();
        void onGeneralPage();
        void onITunesPage();
#endif

    protected slots:

        void onTabChanged(int index);
};

//-------------------------------------------------------------------------------------------

class Settings : public QMainWindow
{
    public:
        Q_OBJECT

    public:
        Settings(QSharedPointer<audioio::AOBase> pAudio,Player *player);
        virtual ~Settings();

        QString nextSpeakerFile();

    protected:

#if defined(OMEGA_MACOSX)
        QAction *m_actionAudio;
        QAction *m_actionControl;
        QAction *m_actionGeneral;
        QAction *m_actionITunes;
#endif

        SettingsCentralWidget *m_centralWidget;

#if defined(OMEGA_MACOSX)
        void setActionStyleSheet(QAction *action,const QString& name,bool select);
#endif

        virtual void closeEvent(QCloseEvent *e);

    protected slots:

#if defined(OMEGA_MACOSX)
        void onAudioPage();
        void onKeyboardPage();
        void onGeneralPage();
        void onITunesPage();
#endif
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
