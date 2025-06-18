#include "remote/inc/QKeyLineEdit.h"
#include "remote/inc/WinLIRCRemote.h"
#include "player/inc/Settings.h"
#include "player/inc/Player.h"

#if defined(OMEGA_MAC_STORE)
#include "player/inc/SettingsITunesMac.h"
#endif

#include "player/inc/PlayerController.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------
// Settings
//-------------------------------------------------------------------------------------------

Settings::Settings(QSharedPointer<audioio::AOBase> pAudio,Player *player) : QMainWindow(player,Qt::CustomizeWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setWindowTitle("Preferences");
    
    m_centralWidget = new SettingsCentralWidget(pAudio,player,this);
    
#if defined(OMEGA_MACOSX)
    setUnifiedTitleAndToolBarOnMac(true);
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    QToolBar *toolBar = addToolBar("Preference");
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    toolBar->setIconSize(QSize(48,48));

    m_actionGeneral = toolBar->addAction(QIcon(":/player/Resources/setBlank.png"),"General");
    QObject::connect(m_actionGeneral,SIGNAL(triggered()),m_centralWidget,SLOT(onGeneralPage()));
    QObject::connect(m_actionGeneral,SIGNAL(triggered()),this,SLOT(onGeneralPage()));

    m_actionAudio = toolBar->addAction(QIcon(":/player/Resources/setBlank.png"),"Audio");
    QObject::connect(m_actionAudio,SIGNAL(triggered()),m_centralWidget,SLOT(onAudioPage()));
    QObject::connect(m_actionAudio,SIGNAL(triggered()),this,SLOT(onAudioPage()));
    
    m_actionControl = toolBar->addAction(QIcon(":/player/Resources/setBlank.png"),"Controls");
    QObject::connect(m_actionControl,SIGNAL(triggered()),m_centralWidget,SLOT(onKeyboardPage()));
    QObject::connect(m_actionControl,SIGNAL(triggered()),this,SLOT(onKeyboardPage()));

    m_actionITunes = toolBar->addAction(QIcon(":/player/Resources/setBlank.png"),"iTunes");
    QObject::connect(m_actionITunes,SIGNAL(triggered()),m_centralWidget,SLOT(onITunesPage()));
    QObject::connect(m_actionITunes,SIGNAL(triggered()),this,SLOT(onITunesPage()));

    setActionStyleSheet(m_actionGeneral,"General",true);
    setActionStyleSheet(m_actionAudio,"Audio",false);
    setActionStyleSheet(m_actionControl,"Control",false);
    setActionStyleSheet(m_actionITunes,"ITunes",false);    
#endif
    
    setCentralWidget(m_centralWidget);
    QSettings settings;
    settings.beginGroup("settingsWindowState");
    if(settings.contains("mainWindowGeometry"))
    {
        restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
    }
    settings.endGroup();
    resize(minimumSize());
}

//-------------------------------------------------------------------------------------------

Settings::~Settings()
{}

//-------------------------------------------------------------------------------------------

QString Settings::nextSpeakerFile()
{
    return m_centralWidget->nextSpeakerFile();
}

//-------------------------------------------------------------------------------------------

void Settings::closeEvent(QCloseEvent *e)
{
    QSettings settings;
    if(!(windowState() & Qt::WindowMaximized))
    {
        settings.beginGroup("settingsWindowState");
        settings.setValue("mainWindowGeometry",saveGeometry());
        settings.endGroup();
    }
    QMainWindow::closeEvent(e);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

void Settings::setActionStyleSheet(QAction *action,const QString& name,bool select)
{
    QString iNormal;
    QString iPressed;
    
    iNormal = ":/player/Resources/set" + name;
    if(select)
    {
        iNormal += "Select";
    }
    iNormal += ".png";
    
    iPressed = ":/player/Resources/set" + name + "Press.png";
    
    QString styleString;
    styleString  = "QToolButton { border: 0px; background: transparent; color: #4a4a4a; width: 64px; height: 64px; image: url(" + iNormal + ") }\n";
    styleString += "QToolButton:pressed { border: 0px; background: transparent; image: url(" + iPressed + ") }\n";

    QList<QObject *>::iterator ppI;
    QList<QObject *> widgetList = action->associatedObjects();
    for(ppI=widgetList.begin();ppI!=widgetList.end();ppI++)
    {
        QToolButton *w = qobject_cast<QToolButton *>(*ppI);
        if(w!=nullptr)
        {
            w->setStyleSheet(styleString);
        }
    }
}

//-------------------------------------------------------------------------------------------

void Settings::onAudioPage()
{
    setActionStyleSheet(m_actionGeneral,"General",false);
    setActionStyleSheet(m_actionAudio,"Audio",true);
    setActionStyleSheet(m_actionControl,"Control",false);
    setActionStyleSheet(m_actionITunes,"ITunes",false);
}

//-------------------------------------------------------------------------------------------

void Settings::onKeyboardPage()
{
    setActionStyleSheet(m_actionGeneral,"General",false);
    setActionStyleSheet(m_actionAudio,"Audio",false);
    setActionStyleSheet(m_actionControl,"Control",true);
    setActionStyleSheet(m_actionITunes,"ITunes",false);
}

//-------------------------------------------------------------------------------------------

void Settings::onGeneralPage()
{
    setActionStyleSheet(m_actionGeneral,"General",true);
    setActionStyleSheet(m_actionAudio,"Audio",false);
    setActionStyleSheet(m_actionControl,"Control",false);
    setActionStyleSheet(m_actionITunes,"ITunes",false);    
}

//-------------------------------------------------------------------------------------------

void Settings::onITunesPage()
{
    setActionStyleSheet(m_actionGeneral,"General",false);
    setActionStyleSheet(m_actionAudio,"Audio",false);
    setActionStyleSheet(m_actionControl,"Control",false);
    setActionStyleSheet(m_actionITunes,"ITunes",true);
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
// SettingsCentralWidget
//-------------------------------------------------------------------------------------------

SettingsCentralWidget::SettingsCentralWidget(QSharedPointer<audioio::AOBase> pAudio,Player *player,QWidget *parent,Qt::WindowFlags f) : QWidget(parent,f),
    m_player(player)
{
    resize(650, 581 + 27 + 55);
    
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    setSizePolicy(sizePolicy);
    setMinimumSize(QSize(650, 560 + 27 + 55));
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    m_widgetGeneral = new SettingsGeneral(this);
    m_widgetAudio = new SettingsAudio(pAudio,this);
    m_widgetKeyboard = new SettingsKeyboard(this);
    
#if !defined(OMEGA_MAC_STORE)
    m_widgetITunes = new SettingsITunes(this);
#else
    m_widgetITunes = new SettingsITunesMac(this);
#endif

#if defined(OMEGA_WIN32) || defined(OMEGA_LINUX)
    
    m_widgetFile = new SettingsFile(this);
    m_settingTab = new QTabWidget(this);
    m_settingTab->addTab(m_widgetGeneral,"General");
    m_settingTab->addTab(m_widgetAudio,"Audio");
    m_settingTab->addTab(m_widgetKeyboard,"Controls");
    m_settingTab->addTab(m_widgetITunes,"iTunes");
    m_settingTab->addTab(m_widgetFile,"File Associations");
    QObject::connect(m_settingTab,SIGNAL(currentChanged(int)),this,SLOT(onTabChanged(int)));
    layout->addWidget(m_settingTab);
    
#elif defined(OMEGA_MACOSX)
    
    m_settingStack = new QStackedWidget(this);
    m_settingStack->addWidget(m_widgetGeneral);
    m_settingStack->addWidget(m_widgetAudio);
    m_settingStack->addWidget(m_widgetKeyboard);
    m_settingStack->addWidget(m_widgetITunes);
    QObject::connect(m_settingStack,SIGNAL(currentChanged(int)),this,SLOT(onTabChanged(int)));
    layout->addWidget(m_settingStack);
    
#endif
}

//-------------------------------------------------------------------------------------------

SettingsCentralWidget::~SettingsCentralWidget()
{}

//-------------------------------------------------------------------------------------------

void SettingsCentralWidget::onTabChanged(int index)
{
    if(index==0)
    {
        m_widgetGeneral->onSelected(index);
    }
    else if(index==1)
    {
        m_widgetAudio->onSelected(index);
    }
    else if(index==3)
    {
        m_widgetITunes->onSelected(index);
    }
#if defined(OMEGA_WIN32)
    else if(index==4)
    {
        m_widgetFile->onSelected(index);
    }
#endif
    m_widgetKeyboard->onSelected(index);
}

//-------------------------------------------------------------------------------------------

QString SettingsCentralWidget::nextSpeakerFile()
{
    return m_widgetAudio->nextSpeakerFile();
}

//-------------------------------------------------------------------------------------------

Player *SettingsCentralWidget::getPlayer()
{
    return m_player;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

void SettingsCentralWidget::onAudioPage()
{
    m_settingStack->setCurrentIndex(1);
}

//-------------------------------------------------------------------------------------------

void SettingsCentralWidget::onKeyboardPage()
{
    m_settingStack->setCurrentIndex(2);
}

//-------------------------------------------------------------------------------------------

void SettingsCentralWidget::onGeneralPage()
{
    m_settingStack->setCurrentIndex(0);
}

//-------------------------------------------------------------------------------------------

void SettingsCentralWidget::onITunesPage()
{
    m_settingStack->setCurrentIndex(3);
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

void SettingsCentralWidget::showEvent(QShowEvent *evt)
{
    remote::KeyControlServiceSPtr pKeyService = remote::KeyControlService::instance();
    if(pKeyService.data()!=0)
    {
        pKeyService->startAssignment();
    }

    Player *playerDlg = dynamic_cast<Player *>(getPlayer());
    if(playerDlg!=0)
    {
        if(playerDlg->m_remoteLIRC.data()!=0)
        {
            remote::WinLIRCRemoteSPtr pRemote = playerDlg->m_remoteLIRC.dynamicCast<remote::WinLIRCRemote>();
            if(pRemote.data()!=0)
            {
                pRemote->startAssignmentToRemote();
            }
        }
    }

    PlayerController::instance()->iTunesConfig()->startEdit();

    QWidget::showEvent(evt);
}

//-------------------------------------------------------------------------------------------

void SettingsCentralWidget::hideEvent(QHideEvent *evt)
{
    remote::KeyControlServiceSPtr pKeyService = remote::KeyControlService::instance();
    if(pKeyService.data()!=0)
    {
        m_widgetKeyboard->writeKeyboardSettings();
        pKeyService->endAssignment();
    }

    Player *playerDlg = dynamic_cast<Player *>(getPlayer());
    if(playerDlg!=0)
    {
        if(playerDlg->m_remoteLIRC.data()!=0)
        {
            remote::WinLIRCRemoteSPtr pRemote = playerDlg->m_remoteLIRC.dynamicCast<remote::WinLIRCRemote>();
            if(pRemote.data()!=0)
            {
                pRemote->endAssignmentToRemote();
            }
        }
    }

    PlayerController::instance()->iTunesConfig()->stopEdit();

    QWidget::hideEvent(evt);
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
