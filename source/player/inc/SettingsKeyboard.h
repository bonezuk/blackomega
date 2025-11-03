//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_SETTINGSKEYBOARD_H
#define __OMEGA_PLAYER_SETTINGSKEYBOARD_H
//-------------------------------------------------------------------------------------------

#include "remote/inc/QKeyLineEdit.h"
#include "remote/inc/WinLIRCRemote.h"
#include "player/inc/SettingsBase.h"
#include "player/ui_SettingsKeyboard.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class SettingsKeyboard : public SettingsBase, public remote::WinLIRCRemoteProxyIF
{
    public:
        Q_OBJECT

    public:
        SettingsKeyboard(QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~SettingsKeyboard();

        virtual void onSelected(int index);

        void writeKeyboardSettings();

    protected:

        Ui::SettingsKeyboard ui;
        remote::QKeyLineEditDelegate *m_delegate;
        bool m_hasKeyModel;

        QString m_cacheLIRCHost;
        tint m_cacheLIRCPort;

        QTimer *m_lircTimer;
        QPixmap *m_pixmapOK;
        QPixmap *m_pixmapRecv;

        common::TimeStamp m_remoteButtonTime;

        void onRemoteTab(int index);

        bool isRemoteAppleIR();
        bool isRemoteLIRC();
        bool isRemote(const QString& remoteName,bool defaultFlag);
        void setRemoteAppleIR(bool flag);
        void setRemoteLIRC(bool flag);
        void setRemote(const QString& remoteName,bool flag);

        void setupRemoteAppleIR();
        void setupRemoteLIRC();
        void setLIRCStatus(int iconType,const QString& status);
        void setLIRCStatus(remote::WinLIRCRemoteSPtr pRemote);
        void blockSignalsRemote(bool flag);
        void connectLIRCRemote();
        void disconnectLIRCRemote();

        double sliderToValue(QSlider *slider,double min,double max);
        int valueToSlider(QSlider *slider,double min,double max,double value);

        void setupKeyboardView();

        virtual remote::WinLIRCRemoteSPtr getWinLIRCRemote();

    protected slots:

        void onRemoteAppleEnable();
        void onRemoteLIRCEnable();
        void onRemoteLIRCSetHostAndPort();
        void onRemoteLIRCConnect(bool connected);
        void onRemoteLIRCStatusUpdate(QString statusText,int icon);
        void onRemoteLIRCButton(QString cmd,int repeat);
        void onRemoteLIRCTimer();
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

