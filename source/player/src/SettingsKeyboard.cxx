#include "player/inc/SettingsKeyboard.h"
#if defined(OMEGA_WIN32)
#include <shlobj.h>
#endif
#include "player/inc/Player.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

SettingsKeyboard::SettingsKeyboard(QWidget *parent,Qt::WindowFlags f) : SettingsBase(parent,f),
    m_hasKeyModel(false),
    m_remoteButtonTime(0)
{
    ui.setupUi(this);
    
    m_cacheLIRCHost = "localhost";
    m_cacheLIRCPort = 8765;

    QObject::connect(ui.m_appleIRRemote,SIGNAL(toggled(bool)),this,SLOT(onRemoteAppleEnable()));
    QObject::connect(ui.m_lircGroup,SIGNAL(toggled(bool)),this,SLOT(onRemoteLIRCEnable()));
    QObject::connect(ui.m_lircHostEdit,SIGNAL(editingFinished()),this,SLOT(onRemoteLIRCSetHostAndPort()));
    QObject::connect(ui.m_lircPortEdit,SIGNAL(editingFinished()),this,SLOT(onRemoteLIRCSetHostAndPort()));

    m_delegate = new remote::QKeyLineEditDelegate(this);
    ui.m_assignTable->setItemDelegate(m_delegate);
    ui.m_assignTable->setSelectionMode(QAbstractItemView::NoSelection);
    ui.m_assignTable->horizontalHeader()->setStretchLastSection(true);
    ui.m_assignTable->horizontalHeader()->setVisible(false);
    setupKeyboardView();

    m_pixmapOK = new QPixmap(":/winlirc/Resources/winlirc/remote_ok.png");
    m_pixmapRecv = new QPixmap(":/winlirc/Resources/winlirc/remote_recv.png");

    m_lircTimer = new QTimer(this);
    QObject::connect(m_lircTimer,SIGNAL(timeout()),this,SLOT(onRemoteLIRCTimer()));

    connectLIRCRemote();
}

//-------------------------------------------------------------------------------------------

SettingsKeyboard::~SettingsKeyboard()
{
    delete m_pixmapOK;
    delete m_pixmapRecv;
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::onSelected(int index)
{
    onRemoteTab(index);
}

//-------------------------------------------------------------------------------------------

bool SettingsKeyboard::isRemoteAppleIR()
{
    return isRemote("appleIR",true);
}

//-------------------------------------------------------------------------------------------

bool SettingsKeyboard::isRemoteLIRC()
{
    return isRemote("lircIR",false);
}

//-------------------------------------------------------------------------------------------

bool SettingsKeyboard::isRemote(const QString& remoteName,bool defaultFlag)
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

void SettingsKeyboard::setRemoteAppleIR(bool flag)
{
    setRemote("appleIR",flag);
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::setRemoteLIRC(bool flag)
{
    setRemote("lircIR",flag);
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::setRemote(const QString& remoteName,bool flag)
{
    QSettings settings;
    settings.beginGroup("remote");
    settings.setValue(remoteName,QVariant(flag));
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::onRemoteTab(int index)
{
    if(index==2)
    {
        setupRemoteAppleIR();
        setupRemoteLIRC();
        setupKeyboardView();
        m_remoteButtonTime = 0;
        m_lircTimer->start(50);
    }
    else
    {
        m_delegate->resetEditor();
        writeKeyboardSettings();
        m_lircTimer->stop();
    }
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::setupRemoteAppleIR()
{
    Player *playerDlg = getPlayerDialog();
    
    ui.m_appleIRRemote->blockSignals(true);

#if defined(OMEGA_MACOSX)
    if(remote::RemoteIF::isServiceAvailable("apple_remote"))
    {
        ui.m_appleIRRemote->setVisible(true);
        ui.m_appleIRRemote->setEnabled(true);
        if(playerDlg->m_remoteApple.data()!=0)
        {
            ui.m_appleIRRemote->setCheckState(Qt::Checked);
            setRemoteAppleIR(true);
        }
        else
        {
            ui.m_appleIRRemote->setCheckState(Qt::Unchecked);
            setRemoteAppleIR(false);
        }
    }
    else
#endif
    {
        ui.m_appleIRRemote->setEnabled(false);
        ui.m_appleIRRemote->setVisible(false);
    }

    ui.m_appleIRRemote->blockSignals(false);
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::setupRemoteLIRC()
{
    bool enable = false;
    Player *playerDlg = getPlayerDialog();

    if(playerDlg->m_remoteLIRC.data()!=0)
    {
        if(isRemoteLIRC())
        {
            enable = true;
        }
        else
        {
            playerDlg->removeRemote(true);
        }
    }
    else
    {
        if(isRemoteLIRC())
        {
            setRemoteLIRC(false);
        }
    }
    
    if(enable)
    {
        remote::WinLIRCRemoteSPtr pRemote = playerDlg->m_remoteLIRC.dynamicCast<remote::WinLIRCRemote>();
    
        blockSignalsRemote(true);
        ui.m_lircGroup->setEnabled(true);
        ui.m_lircGroup->setChecked(true);
        
        ui.m_lircHostEdit->setText(pRemote->host());
        ui.m_lircHostEdit->setEnabled(true);
        ui.m_lircPortEdit->setValue(pRemote->port());
        ui.m_lircPortEdit->setEnabled(true);

        ui.m_lircStatusText->setEnabled(true);
        ui.m_lircStatusIcon->setEnabled(true);
        setLIRCStatus(pRemote);
        blockSignalsRemote(false);
    }
    else
    {
        blockSignalsRemote(true);
        ui.m_lircGroup->setEnabled(true);
        ui.m_lircGroup->setChecked(false);
        ui.m_lircHostEdit->setEnabled(false);
        ui.m_lircPortEdit->setEnabled(false);
        ui.m_lircStatusText->setEnabled(false);
        ui.m_lircStatusIcon->setEnabled(false);
        blockSignalsRemote(false);
    }

    m_cacheLIRCHost = ui.m_lircHostEdit->text().toLower().trimmed();
    m_cacheLIRCPort = ui.m_lircPortEdit->value();
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::setLIRCStatus(remote::WinLIRCRemoteSPtr pRemote)
{
    int iconType = 0;
    QString status = pRemote->status(iconType);
    setLIRCStatus(iconType,status);
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::setLIRCStatus(int iconType,const QString& status)
{
    bool enable = true;
    bool lircControlEnable = false;
    
    switch(iconType)
    {
        case 1:
            {
                QPixmap pic(":/player/Resources/hourglass.png");
                ui.m_lircStatusIcon->setPixmap(pic);
                enable = false;
            }
            break;
            
        case 2:
            {
                QPixmap pic(":/player/Resources/tick.png");
                ui.m_lircStatusIcon->setPixmap(pic);
                lircControlEnable = true;
            }
            break;
            
        default:
            {
                QPixmap pic(":/player/Resources/exclamation.png");
                ui.m_lircStatusIcon->setPixmap(pic);
            }
            break;
    }
    ui.m_lircStatusText->setText(status);
    
    ui.m_lircHostEdit->setEnabled(enable);
    ui.m_lircPortEdit->setEnabled(enable);
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::blockSignalsRemote(bool flag)
{
    ui.m_lircGroup->blockSignals(flag);
    ui.m_lircHostEdit->blockSignals(flag);
    ui.m_lircPortEdit->blockSignals(flag);
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::onRemoteAppleEnable()
{
#if defined(OMEGA_MACOSX)
    Player *playerDlg = getPlayerDialog();

    ui.m_appleIRRemote->blockSignals(true);
    if(ui.m_appleIRRemote->isChecked())
    {
        remote::RemoteIFSPtr pRemote = playerDlg->getRemote(false);
        if(pRemote.data()==0)
        {
            ui.m_appleIRRemote->setCheckState(Qt::Unchecked);
        }
    }
    else
    {
        playerDlg->removeRemote(false);
    }
    setRemoteAppleIR(ui.m_appleIRRemote->isChecked());
    ui.m_appleIRRemote->blockSignals(false);
#endif
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::onRemoteLIRCEnable()
{
    Player *playerDlg = getPlayerDialog();
    
    blockSignalsRemote(true);
    if(ui.m_lircGroup->isChecked())
    {
        remote::RemoteIFSPtr pRemote = playerDlg->getRemote(true);
        if(pRemote.data()!=0)
        {
            connectLIRCRemote();
        }
        else
        {
            ui.m_lircGroup->setChecked(false);
        }
    }
    else
    {
        disconnectLIRCRemote();
        playerDlg->removeRemote(true);
        setLIRCStatus(0,"No connection to LIRC server");
    }
    setRemoteLIRC(ui.m_lircGroup->isChecked());
    blockSignalsRemote(false);
    
    setupRemoteLIRC();
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::onRemoteLIRCSetHostAndPort()
{
    Player *playerDlg = getPlayerDialog();
    remote::WinLIRCRemoteSPtr pRemote = playerDlg->getRemote(true).dynamicCast<remote::WinLIRCRemote>();
    QString host = ui.m_lircHostEdit->text().toLower().trimmed();
    int port = ui.m_lircPortEdit->value();
    if(m_cacheLIRCHost!=host || m_cacheLIRCPort!=port)
    {
        pRemote->connect(host,port);
        m_cacheLIRCHost = host;
        m_cacheLIRCPort = port;
    }
    setLIRCStatus(pRemote);
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::onRemoteLIRCConnect(bool connected)
{
    Player *playerDlg = getPlayerDialog();
    remote::WinLIRCRemoteSPtr pRemote = playerDlg->getRemote(true).dynamicCast<remote::WinLIRCRemote>();
    setLIRCStatus(pRemote);
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::connectLIRCRemote()
{
    Player *playerDlg = getPlayerDialog();
    if(playerDlg!=0)
    {
        if(playerDlg->m_remoteLIRC.data()!=0)
        {
            remote::WinLIRCRemoteSPtr pRemote = playerDlg->m_remoteLIRC.dynamicCast<remote::WinLIRCRemote>();
            if(pRemote.data()!=0)
            {
                QObject::connect(pRemote.data(),SIGNAL(onConnect(bool)),this,SLOT(onRemoteLIRCConnect(bool)));
                QObject::connect(pRemote.data(),SIGNAL(onStatusUpdate(QString,int)),this,SLOT(onRemoteLIRCStatusUpdate(QString,int)));
                QObject::connect(pRemote.data(),SIGNAL(onProgrammed(QString,int)),this,SLOT(onRemoteLIRCButton(QString,int)));
                m_remoteButtonTime = 0;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::disconnectLIRCRemote()
{
    Player *playerDlg = getPlayerDialog();
    if(playerDlg!=0)
    {
        if(playerDlg->m_remoteLIRC.data()!=0)
        {
            remote::WinLIRCRemoteSPtr pRemote = playerDlg->m_remoteLIRC.dynamicCast<remote::WinLIRCRemote>();
            if(pRemote.data()!=0)
            {
                m_remoteButtonTime = 0;
                QObject::disconnect(pRemote.data(),SIGNAL(onProgrammed(QString,int)),this,SLOT(onRemoteLIRCButton(QString,int)));
                QObject::disconnect(pRemote.data(),SIGNAL(onConnect(bool)),this,SLOT(onRemoteLIRCConnect(bool)));
                QObject::disconnect(pRemote.data(),SIGNAL(onStatusUpdate(QString,int)),this,SLOT(onRemoteLIRCStatusUpdate(QString,int)));
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::onRemoteLIRCStatusUpdate(QString statusText,int icon)
{
    setLIRCStatus(icon,statusText);
}

//-------------------------------------------------------------------------------------------

double SettingsKeyboard::sliderToValue(QSlider *slider,double min,double max)
{
    double iMin = static_cast<double>(slider->minimum());
    double iMax = static_cast<double>(slider->maximum());
    double iVal = static_cast<double>(slider->value());
    double v = (((iVal - iMin) / (iMax - iMin)) * (max - min)) + min;
    return v;
}

//-------------------------------------------------------------------------------------------

int SettingsKeyboard::valueToSlider(QSlider *slider,double min,double max,double value)
{
    double iMin = static_cast<double>(slider->minimum());
    double iMax = static_cast<double>(slider->maximum());
    
    if(value < min)
    {
        value = min;
    }
    if(value > max)
    {
        value = max;
    }
    
    double iVal = (((value - min) / (max - min)) * (iMax - iMin)) + iMin;
    int iV = static_cast<double>(iVal);
    if((iVal - static_cast<double>(iV)) > 0.5)
    {
        iV++;
    }
    if(iV < slider->minimum())
    {
        iV = slider->minimum();
    }
    if(iV > slider->maximum())
    {
        iV = slider->maximum();
    }
    return iV;
}


//-------------------------------------------------------------------------------------------

void SettingsKeyboard::setupKeyboardView()
{
    remote::KeyAssignmentSPtr pKeyAssignment = remote::KeyAssignment::instance();
    if(pKeyAssignment.data()!=0)
    {
        writeKeyboardSettings();

        QAbstractItemModel *oldModel = ui.m_assignTable->model();
        QStandardItemModel *model = pKeyAssignment->saveToModel(this);
        
        ui.m_assignTable->setModel(model);
        m_hasKeyModel = true;
        
        if(oldModel!=0)
        {
            delete oldModel;
        }
    }
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::writeKeyboardSettings()
{
    remote::KeyAssignmentSPtr pKeyAssignment = remote::KeyAssignment::instance();
    if(pKeyAssignment.data()!=0 && m_hasKeyModel)
    {
        pKeyAssignment->loadFromModel(ui.m_assignTable->model());
    }
}

//-------------------------------------------------------------------------------------------

remote::WinLIRCRemoteSPtr SettingsKeyboard::getWinLIRCRemote()
{
    remote::WinLIRCRemoteSPtr pRemote;
    Player *playerDlg = getPlayerDialog();
    if(playerDlg!=0)
    {
        if(playerDlg->m_remoteLIRC.data()!=0)
        {
            pRemote = playerDlg->m_remoteLIRC.dynamicCast<remote::WinLIRCRemote>();
        }
    }
    return pRemote;
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::onRemoteLIRCButton(QString cmd,int repeat)
{
    m_remoteButtonTime = common::TimeStamp::reference();
    onRemoteLIRCTimer();
}

//-------------------------------------------------------------------------------------------

void SettingsKeyboard::onRemoteLIRCTimer()
{
    common::TimeStamp timeOut = common::TimeStamp::reference() - 0.05;

    if(m_remoteButtonTime==0 || timeOut > m_remoteButtonTime)
    {
        ui.m_lircRecieveIcon->setPixmap(*m_pixmapOK);
    }
    else
    {
        ui.m_lircRecieveIcon->setPixmap(*m_pixmapRecv);
    }
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
