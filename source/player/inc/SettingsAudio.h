//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_SETTINGSAUDIO_H
#define __OMEGA_PLAYER_SETTINGSAUDIO_H
//-------------------------------------------------------------------------------------------

#include "player/inc/SettingsBase.h"
#include "player/ui_SettingsAudio.h"

#include <QDialog>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class SettingsAudio : public SettingsBase
{
    public:
        Q_OBJECT

    public:
        SettingsAudio(QSharedPointer<audioio::AOBase> pAudio,QWidget *parent,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~SettingsAudio();

        QString nextSpeakerFile();

        virtual void onSelected(int index);

    protected:

        Ui::SettingsAudio ui;

        QSharedPointer<audioio::AOBase> m_audio;

        int m_deviceIdx;
        QSharedPointer<audioio::AOQueryDevice::Device> m_device;

        QSpeakerButton *m_spkCenterButton;
        QSpeakerButton *m_spkFrontLeftButton;
        QSpeakerButton *m_spkFrontRightButton;
        QSpeakerButton *m_spkSurroundLeftButton;
        QSpeakerButton *m_spkSurroundRightButton;
        QSpeakerButton *m_spkRearLeftButton;
        QSpeakerButton *m_spkRearRightButton;
        QSpeakerButton *m_spkSubwooferButton;
        QSpeakerButton *m_personButton;

        int m_testIndex;
        QStringList m_testFilelist;

        void init();

        void updateSpeakerCombo();
        void updateStereoCombo();
        void setStereoComboFromChannelMap();

        QStringList channelTestPlaylist(audioio::ChannelType type);

        void doSetChannel(int chIdx,int comboIdx);

        void getChannelResources(audioio::ChannelType chType,QString& eImageN,QString& dImageN);
        QLabel *getChannelLabel(audioio::ChannelType chType);
        QSpeakerButton *getChannelButton(audioio::ChannelType chType);
        QComboBox *getChannelCombo(audioio::ChannelType chType);

        void doSpeakerConfiguration(int idx);
        void uiSpeaker1();
        void uiSpeaker2();
        void uiSpeaker3();
        void uiSpeaker4();
        void uiSpeaker5();
        void uiSpeaker6();
        void uiSpeaker7();
        void uiSpeaker8();

        void updateSpeaker(audioio::ChannelType chType,QComboBox *speakerCombo,bool enableFlag);

        void updateSpeakerComboFromChannelMap();
        void updateStereoComboFromChannelMap();
        void updateAllChannelsFromChannelMap();
        void updateFromChannelMap();

        void updateUseCenter();
        void updateUseLFE();

    protected slots:

        void onDeviceChange(int idx);
        void onSpeakerConfiguration(int idx);
        void onStereoConfigutaion(int idx);

        void onFrontLeftChannel(int idx);
        void onFrontRightChannel(int idx);
        void onCenterChannel(int idx);
        void onSurroundLeftChannel(int idx);
        void onSurroundRightChannel(int idx);
        void onRearLeftChannel(int idx);
        void onRearRightChannel(int idx);
        void onLFEChannel(int idx);

        void onTestCenter();
        void onTestLeftFront();
        void onTestRightFront();
        void onTestLeftSurround();
        void onTestRightSurround();
        void onTestLeftRear();
        void onTestRightRear();
        void onTestSubwoofer();
        void onTestFull();

        void onCheckExclusive(bool checked);
        void onCheckUseCenter(bool checked);
        void onCheckUseLFE(bool checked);
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
