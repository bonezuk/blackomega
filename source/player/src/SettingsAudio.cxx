#include "player/inc/SettingsAudio.h"
#include "player/inc/Player.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

SettingsAudio::SettingsAudio(QSharedPointer<audioio::AOBase> pAudio,QWidget *parent,Qt::WindowFlags f) : SettingsBase(parent,f),
	m_audio(pAudio),
	m_deviceIdx(-1),
	m_device(),
	m_testIndex(0),
	m_testFilelist()
{
	init();
}

//-------------------------------------------------------------------------------------------

SettingsAudio::~SettingsAudio()
{}

//-------------------------------------------------------------------------------------------

void SettingsAudio::init()
{
    int i,nDevices;

	ui.setupUi(this);
	
	m_spkCenterButton = new QSpeakerButton(audioio::e_Center,this);
	m_spkFrontLeftButton = new QSpeakerButton(audioio::e_FrontLeft,this);
	m_spkFrontRightButton = new QSpeakerButton(audioio::e_FrontRight,this);
	m_spkSurroundLeftButton = new QSpeakerButton(audioio::e_SurroundLeft,this);
	m_spkSurroundRightButton = new QSpeakerButton(audioio::e_SurroundRight,this);
	m_spkRearLeftButton = new QSpeakerButton(audioio::e_RearLeft,this);
	m_spkRearRightButton = new QSpeakerButton(audioio::e_RearRight,this);
	m_spkSubwooferButton = new QSpeakerButton(audioio::e_LFE,this);
	m_personButton = new QSpeakerButton(audioio::e_UnknownChannel,this);

	QObject::connect(ui.m_speakerCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onSpeakerConfiguration(int)));
	QObject::connect(ui.m_stereoCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onStereoConfigutaion(int)));
	QObject::connect(ui.m_frontLeftCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onFrontLeftChannel(int)));
	QObject::connect(ui.m_frontRightCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onFrontRightChannel(int)));
	QObject::connect(ui.m_centerCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onCenterChannel(int)));
	QObject::connect(ui.m_surroundLeftCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onSurroundLeftChannel(int)));
	QObject::connect(ui.m_surroundRightCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onSurroundRightChannel(int)));
	QObject::connect(ui.m_rearLeftCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onRearLeftChannel(int)));
	QObject::connect(ui.m_rearRightCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onRearRightChannel(int)));
	QObject::connect(ui.m_subwooferCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onLFEChannel(int)));

	QObject::connect(m_spkCenterButton,SIGNAL(clicked()),this,SLOT(onTestCenter()));
	QObject::connect(m_spkFrontLeftButton,SIGNAL(clicked()),this,SLOT(onTestLeftFront()));
	QObject::connect(m_spkFrontRightButton,SIGNAL(clicked()),this,SLOT(onTestRightFront()));
	QObject::connect(m_spkSurroundLeftButton,SIGNAL(clicked()),this,SLOT(onTestLeftSurround()));
	QObject::connect(m_spkSurroundRightButton,SIGNAL(clicked()),this,SLOT(onTestRightSurround()));
	QObject::connect(m_spkRearLeftButton,SIGNAL(clicked()),this,SLOT(onTestLeftRear()));
	QObject::connect(m_spkRearRightButton,SIGNAL(clicked()),this,SLOT(onTestRightRear()));
	QObject::connect(m_spkSubwooferButton,SIGNAL(clicked()),this,SLOT(onTestSubwoofer()));
	QObject::connect(m_personButton,SIGNAL(clicked()),this,SLOT(onTestFull()));

    QObject::connect(ui.m_exclusiveFlag,SIGNAL(toggled(bool)),this,SLOT(onCheckExclusive(bool)));
	QObject::connect(ui.m_useCenter,SIGNAL(toggled(bool)),this,SLOT(onCheckUseCenter(bool)));
	QObject::connect(ui.m_useSubwoofer,SIGNAL(toggled(bool)),this,SLOT(onCheckUseLFE(bool)));
    
    QObject::connect(m_audio.data(), SIGNAL(onDeviceUpdated(int)), this, SLOT(onDeviceUpdate(int)));

	ui.m_exclusiveFlag->setText("Exclusive Mode");
	ui.m_exclusiveFlag->setToolTip("Take full control of audio device communicating with DAC in its native format\nand preventing other applications from using it during playback.");

	nDevices = m_audio->noDevices();
	if(nDevices>=0)
	{
        ui.m_audioDeviceCombo->blockSignals(true);
		for(i=0;i<nDevices;i++)
		{
#if defined(OMEGA_WIN32)
			QString deviceName = m_audio->deviceName(i);
			deviceName += (m_audio->device(i)->type()==audioio::AOQueryDevice::Device::e_deviceASIO) ? " (ASIO)" : " (WasAPI)";
			ui.m_audioDeviceCombo->addItem(deviceName);
#else
			ui.m_audioDeviceCombo->addItem(m_audio->deviceName(i));
#endif
		}
        ui.m_audioDeviceCombo->blockSignals(false);
		onDeviceChange(m_audio->currentOutputDeviceIndex());
	}
	else
	{
		setEnabled(false);
	}

    QObject::connect(ui.m_audioDeviceCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(onDeviceChange(int)));
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onSelected(int)
{}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onDeviceChange(int idx)
{
	if(idx!=m_deviceIdx && idx>=0 && idx<m_audio->noDevices())
	{
		int noChs;
		
		m_device->saveChannelMap();
		
		if(idx!=m_audio->currentOutputDeviceIndex())
		{
			m_audio->setOutputDevice(idx);
		}
		m_deviceIdx = idx;
		m_device = m_audio->device(idx);

		ui.m_audioDeviceCombo->blockSignals(true);
		if(idx!=ui.m_audioDeviceCombo->currentIndex())
		{
			ui.m_audioDeviceCombo->setCurrentIndex(idx);
		}
		ui.m_audioDeviceCombo->blockSignals(false);
		
		ui.m_exclusiveFlag->blockSignals(true);
		if(m_device->hasExclusive())
		{
			ui.m_exclusiveFlag->setEnabled(true);
			ui.m_exclusiveFlag->setChecked(m_audio->isExclusive(idx));
		}
		else
		{
			ui.m_exclusiveFlag->setEnabled(false);
			ui.m_exclusiveFlag->setChecked(m_device->isAPIExclusive());
			if(m_audio->isExclusive(idx))
			{
				audioio::AudioSettings::instance(m_device->name())->setExclusive(false);
			}
		}
		ui.m_exclusiveFlag->blockSignals(false);
		
		updateFromChannelMap();
	}
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onDeviceUpdate(int idx)
{
	int noChs, currentChs;

	if(idx == m_deviceIdx)
	{
		m_device = m_audio->device(idx);
		updateFromChannelMap();
	}
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::updateSpeakerCombo()
{
	static const char *c_speaker1 = "1 Speaker : Center";
	static const char *c_speaker2 = "2 Speaker : Stereo";
	static const char *c_speaker3 = "3 Speakers : Stereo + Center";
	static const char *c_speaker4 = "4 Speakers : Quadrophonic";
	static const char *c_speaker5 = "5 Speakers : Surround";
	static const char *c_speaker6 = "5.1 Speakers : Surround + Subwoofer";
	static const char *c_speaker7 = "7 Speakers : Full Surround";
	static const char *c_speaker8 = "7.1 Speakers : Full Surround + Subwoofer";
	
	ui.m_speakerCombo->blockSignals(true);
	ui.m_speakerCombo->clear();
	for(int i=0;i<m_device->noChannels() && i<8;i++)
	{
		const char *x = 0;
	
		switch(i)
		{
			case 0:
				x = c_speaker1;
				break;
			case 1:
				x = c_speaker2;
				break;
			case 2:
				x = c_speaker3;
				break;
			case 3:
				x = c_speaker4;
				break;
			case 4:
				x = c_speaker5;
				break;
			case 5:
				x = c_speaker6;
				break;
			case 6:
				x = c_speaker7;
				break;
			case 7:
				x = c_speaker8;
				break;
		}
		if(x!=0)
		{
			ui.m_speakerCombo->addItem(QString::fromLatin1(x));
		}
	}
	ui.m_speakerCombo->blockSignals(false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::updateStereoCombo()
{
	static const char *c_stereo1 = "Front";
	static const char *c_stereo2 = "Surround";
	static const char *c_stereo3 = "Rear";
	static const char *c_stereo4 = "Front + Surround";
	static const char *c_stereo5 = "Front + Rear";
	static const char *c_stereo6 = "Surround + Rear";
	static const char *c_stereo7 = "Front + Surround + Rear";

	int noChs = ui.m_speakerCombo->currentIndex() + 1;
	bool sFlag = false, rFlag = false;
	
	ui.m_stereoCombo->blockSignals(true);
	ui.m_stereoCombo->clear();
	if(noChs>1)
	{
		QSet<audioio::AOChannelMap::StereoType> avSet;
		QSet<audioio::AOChannelMap::StereoType>::iterator ppI;
		
		if(noChs>=4)
		{
			sFlag = true;
		}
		if(noChs>=6)
		{
			rFlag = true;
		}
		ui.m_stereoCombo->addItem(QString::fromLatin1(c_stereo1),QVariant(static_cast<int>(audioio::AOChannelMap::e_Front)));
		avSet.insert(audioio::AOChannelMap::e_Front);
		if(sFlag)
		{
			ui.m_stereoCombo->addItem(QString::fromLatin1(c_stereo2),QVariant(static_cast<int>(audioio::AOChannelMap::e_Surround)));
			avSet.insert(audioio::AOChannelMap::e_Surround);
		}
		if(rFlag)
		{
			ui.m_stereoCombo->addItem(QString::fromLatin1(c_stereo3),QVariant(static_cast<int>(audioio::AOChannelMap::e_Rear)));
			avSet.insert(audioio::AOChannelMap::e_Rear);
		}
		if(sFlag)
		{
			ui.m_stereoCombo->addItem(QString::fromLatin1(c_stereo4),QVariant(static_cast<int>(audioio::AOChannelMap::e_FrontSurround)));
			avSet.insert(audioio::AOChannelMap::e_FrontSurround);
		}
		if(rFlag)
		{
			ui.m_stereoCombo->addItem(QString::fromLatin1(c_stereo5),QVariant(static_cast<int>(audioio::AOChannelMap::e_FrontRear)));
			avSet.insert(audioio::AOChannelMap::e_FrontRear);
		}
		if(sFlag && rFlag)
		{
			ui.m_stereoCombo->addItem(QString::fromLatin1(c_stereo6),QVariant(static_cast<int>(audioio::AOChannelMap::e_SurroundRear)));
			avSet.insert(audioio::AOChannelMap::e_SurroundRear);
			ui.m_stereoCombo->addItem(QString::fromLatin1(c_stereo7),QVariant(static_cast<int>(audioio::AOChannelMap::e_FrontSurroundRear)));
			avSet.insert(audioio::AOChannelMap::e_FrontSurroundRear);
		}
		ppI = avSet.find(m_device->channelMap()->stereoType());
		if(ppI!=avSet.end())
		{
			setStereoComboFromChannelMap();
		}
		else
		{
			ui.m_stereoCombo->setCurrentIndex(0);
		}
		ui.m_stereoCombo->setEnabled(true);
	}
	else
	{
		ui.m_stereoCombo->addItem("No Stereo");
		ui.m_stereoCombo->setEnabled(false);
	}
	ui.m_stereoCombo->blockSignals(false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::setStereoComboFromChannelMap()
{
	for(int i=0;i<ui.m_stereoCombo->count();i++)
	{
		audioio::AOChannelMap::StereoType dType = static_cast<audioio::AOChannelMap::StereoType>(ui.m_stereoCombo->itemData(i).toInt());
		if(dType == m_device->channelMap()->stereoType())
		{
			ui.m_stereoCombo->setCurrentIndex(i);
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onSpeakerConfiguration(int idx)
{
	doSpeakerConfiguration(idx);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::doSpeakerConfiguration(int idx)
{
	switch(idx)
	{
		case 0:
			uiSpeaker1();
			break;
		case 1:
			uiSpeaker2();
			break;
		case 2:
			uiSpeaker3();
			break;
		case 3:
			uiSpeaker4();
			break;
		case 4:
			uiSpeaker5();
			break;
		case 5:
			uiSpeaker6();
			break;
		case 6:
			uiSpeaker7();
			break;
		case 7:
			uiSpeaker8();
			break;
	}
	updateStereoCombo();
	updateUseCenter();
	updateUseLFE();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onStereoConfigutaion(int idx)
{
	audioio::AOChannelMap::StereoType type = static_cast<audioio::AOChannelMap::StereoType>(ui.m_stereoCombo->itemData(idx).toInt());
	if(m_device->channelMap()->setStereoType(type))
	{
		m_device->saveChannelMap();
		m_audio->resetPlay();
	}
	else
	{
		updateSpeakerComboFromChannelMap();
	}
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::doSetChannel(int chIdx,int comboIdx)
{
	audioio::ChannelType chType = static_cast<audioio::ChannelType>(chIdx);
	QComboBox *chCombo = getChannelCombo(chType);
	
	if(m_device->channelMap()->setChannel(chType, chCombo->itemData(comboIdx).toInt()))
	{
		m_device->saveChannelMap();
		m_audio->resetPlay();
		updateAllChannelsFromChannelMap();
	}
	else
	{
		int mapIndex = m_device->channelMap()->channel(chType);
		
		chCombo->blockSignals(true);
		if(mapIndex >= 0)
		{
			chCombo->setCurrentIndex(mapIndex);
		}
		else
		{
			chCombo->setCurrentIndex(chCombo->count() - 1);
		}
		chCombo->blockSignals(false);
	}
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::getChannelResources(audioio::ChannelType chType,QString& eImageN,QString& dImageN)
{
	switch(chType)
	{
		case audioio::e_FrontLeft:
			eImageN = ":/speakers/Resources/speakers/frontLeft.png";
			dImageN = ":/speakers/Resources/speakers/frontLeftDisabled.png";
			break;
		case audioio::e_FrontRight:
			eImageN = ":/speakers/Resources/speakers/frontRight.png";
			dImageN = ":/speakers/Resources/speakers/frontRightDisabled.png";
			break;
		case audioio::e_Center:
			eImageN = ":/speakers/Resources/speakers/centre.png";
			dImageN = ":/speakers/Resources/speakers/centreDisabled.png";
			break;
		case audioio::e_LFE:
			eImageN = ":/speakers/Resources/speakers/subwoofer.png";
			dImageN = ":/speakers/Resources/speakers/subwooferDisabled.png";
			break;
		case audioio::e_SurroundLeft:
		case audioio::e_RearLeft:
			eImageN = ":/speakers/Resources/speakers/surroundLeft.png";
			dImageN = ":/speakers/Resources/speakers/surroundLeftDisabled.png";
			break;
		case audioio::e_SurroundRight:
		case audioio::e_RearRight:
			eImageN = ":/speakers/Resources/speakers/surroundRight.png";
			dImageN = ":/speakers/Resources/speakers/surroundRightDisabled.png";
			break;
		default:
			break;
	}
}

//-------------------------------------------------------------------------------------------

QLabel *SettingsAudio::getChannelLabel(audioio::ChannelType chType)
{
	QLabel *cLabel = 0;
	
	switch(chType)
	{
		case audioio::e_FrontLeft:
			cLabel = ui.m_frontLeftLabel;
			break;
		case audioio::e_FrontRight:
			cLabel = ui.m_frontRightLabel;
			break;
		case audioio::e_Center:
			cLabel = ui.m_centerLabel;
			break;
		case audioio::e_LFE:
			cLabel = ui.m_subwooferLabel;
			break;
		case audioio::e_SurroundLeft:
			cLabel = ui.m_surroundLeftLabel;
			break;
		case audioio::e_RearLeft:
			cLabel = ui.m_rearLeftLabel;
			break;
		case audioio::e_SurroundRight:
			cLabel = ui.m_surroundRightLabel;
			break;
		case audioio::e_RearRight:
			cLabel = ui.m_rearRightLabel;
			break;
		default:
			break;
	}
	return cLabel;
}

//-------------------------------------------------------------------------------------------

QSpeakerButton *SettingsAudio::getChannelButton(audioio::ChannelType chType)
{
	QSpeakerButton *spkButton = 0;

	switch(chType)
	{
		case audioio::e_FrontLeft:
			spkButton = m_spkFrontLeftButton;
			break;
		case audioio::e_FrontRight:
			spkButton = m_spkFrontRightButton;
			break;
		case audioio::e_Center:
			spkButton = m_spkCenterButton;
			break;
		case audioio::e_LFE:
			spkButton = m_spkSubwooferButton;
			break;
		case audioio::e_SurroundLeft:
			spkButton = m_spkSurroundLeftButton;
			break;
		case audioio::e_RearLeft:
			spkButton = m_spkRearLeftButton;
			break;
		case audioio::e_SurroundRight:
			spkButton = m_spkSurroundRightButton;
			break;
		case audioio::e_RearRight:
			spkButton = m_spkRearRightButton;
			break;
		default:
			break;
	}
	return spkButton;
}

//-------------------------------------------------------------------------------------------

QComboBox *SettingsAudio::getChannelCombo(audioio::ChannelType chType)
{
	QComboBox *cCombo = 0;
	
	switch(chType)
	{
		case audioio::e_FrontLeft:
			cCombo = ui.m_frontLeftCombo;
			break;
		case audioio::e_FrontRight:
			cCombo = ui.m_frontRightCombo;
			break;
		case audioio::e_Center:
			cCombo = ui.m_centerCombo;
			break;
		case audioio::e_LFE:
			cCombo = ui.m_subwooferCombo;
			break;
		case audioio::e_SurroundLeft:
			cCombo = ui.m_surroundLeftCombo;
			break;
		case audioio::e_RearLeft:
			cCombo = ui.m_rearLeftCombo;
			break;
		case audioio::e_SurroundRight:
			cCombo = ui.m_surroundRightCombo;
			break;
		case audioio::e_RearRight:
			cCombo = ui.m_rearRightCombo;
			break;
		default:
			break;
	}
	return cCombo;
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onFrontLeftChannel(int idx)
{
	doSetChannel(static_cast<int>(audioio::e_FrontLeft),idx);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onFrontRightChannel(int idx)
{
	doSetChannel(static_cast<int>(audioio::e_FrontRight),idx);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onCenterChannel(int idx)
{
	doSetChannel(static_cast<int>(audioio::e_Center),idx);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onSurroundLeftChannel(int idx)
{
	doSetChannel(static_cast<int>(audioio::e_SurroundLeft),idx);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onSurroundRightChannel(int idx)
{
	doSetChannel(static_cast<int>(audioio::e_SurroundRight),idx);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onRearLeftChannel(int idx)
{
	doSetChannel(static_cast<int>(audioio::e_RearLeft),idx);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onRearRightChannel(int idx)
{
	doSetChannel(static_cast<int>(audioio::e_RearRight),idx);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onLFEChannel(int idx)
{
	doSetChannel(static_cast<int>(audioio::e_LFE),idx);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::uiSpeaker1()
{
	updateSpeaker(audioio::e_FrontLeft,ui.m_frontLeftCombo,false);
	updateSpeaker(audioio::e_FrontRight,ui.m_frontRightCombo,false);
	updateSpeaker(audioio::e_Center,ui.m_centerCombo,true);
	updateSpeaker(audioio::e_SurroundLeft,ui.m_surroundLeftCombo,false);
	updateSpeaker(audioio::e_SurroundRight,ui.m_surroundRightCombo,false);
	updateSpeaker(audioio::e_RearLeft,ui.m_rearLeftCombo,false);
	updateSpeaker(audioio::e_RearRight,ui.m_rearRightCombo,false);
	updateSpeaker(audioio::e_LFE,ui.m_subwooferCombo,false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::uiSpeaker2()
{
	updateSpeaker(audioio::e_FrontLeft,ui.m_frontLeftCombo,true);
	updateSpeaker(audioio::e_FrontRight,ui.m_frontRightCombo,true);
	updateSpeaker(audioio::e_Center,ui.m_centerCombo,false);
	updateSpeaker(audioio::e_SurroundLeft,ui.m_surroundLeftCombo,false);
	updateSpeaker(audioio::e_SurroundRight,ui.m_surroundRightCombo,false);
	updateSpeaker(audioio::e_RearLeft,ui.m_rearLeftCombo,false);
	updateSpeaker(audioio::e_RearRight,ui.m_rearRightCombo,false);
	updateSpeaker(audioio::e_LFE,ui.m_subwooferCombo,false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::uiSpeaker3()
{
	updateSpeaker(audioio::e_FrontLeft,ui.m_frontLeftCombo,true);
	updateSpeaker(audioio::e_FrontRight,ui.m_frontRightCombo,true);
	updateSpeaker(audioio::e_Center,ui.m_centerCombo,true);
	updateSpeaker(audioio::e_SurroundLeft,ui.m_surroundLeftCombo,false);
	updateSpeaker(audioio::e_SurroundRight,ui.m_surroundRightCombo,false);
	updateSpeaker(audioio::e_RearLeft,ui.m_rearLeftCombo,false);
	updateSpeaker(audioio::e_RearRight,ui.m_rearRightCombo,false);
	updateSpeaker(audioio::e_LFE,ui.m_subwooferCombo,false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::uiSpeaker4()
{
	updateSpeaker(audioio::e_FrontLeft,ui.m_frontLeftCombo,true);
	updateSpeaker(audioio::e_FrontRight,ui.m_frontRightCombo,true);
	updateSpeaker(audioio::e_Center,ui.m_centerCombo,false);
	updateSpeaker(audioio::e_SurroundLeft,ui.m_surroundLeftCombo,true);
	updateSpeaker(audioio::e_SurroundRight,ui.m_surroundRightCombo,true);
	updateSpeaker(audioio::e_RearLeft,ui.m_rearLeftCombo,false);
	updateSpeaker(audioio::e_RearRight,ui.m_rearRightCombo,false);
	updateSpeaker(audioio::e_LFE,ui.m_subwooferCombo,false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::uiSpeaker5()
{
	updateSpeaker(audioio::e_FrontLeft,ui.m_frontLeftCombo,true);
	updateSpeaker(audioio::e_FrontRight,ui.m_frontRightCombo,true);
	updateSpeaker(audioio::e_Center,ui.m_centerCombo,true);
	updateSpeaker(audioio::e_SurroundLeft,ui.m_surroundLeftCombo,true);
	updateSpeaker(audioio::e_SurroundRight,ui.m_surroundRightCombo,true);
	updateSpeaker(audioio::e_RearLeft,ui.m_rearLeftCombo,false);
	updateSpeaker(audioio::e_RearRight,ui.m_rearRightCombo,false);
	updateSpeaker(audioio::e_LFE,ui.m_subwooferCombo,false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::uiSpeaker6()
{
	updateSpeaker(audioio::e_FrontLeft,ui.m_frontLeftCombo,true);
	updateSpeaker(audioio::e_FrontRight,ui.m_frontRightCombo,true);
	updateSpeaker(audioio::e_Center,ui.m_centerCombo,true);
	updateSpeaker(audioio::e_SurroundLeft,ui.m_surroundLeftCombo,true);
	updateSpeaker(audioio::e_SurroundRight,ui.m_surroundRightCombo,true);
	updateSpeaker(audioio::e_RearLeft,ui.m_rearLeftCombo,false);
	updateSpeaker(audioio::e_RearRight,ui.m_rearRightCombo,false);
	updateSpeaker(audioio::e_LFE,ui.m_subwooferCombo,true);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::uiSpeaker7()
{
	updateSpeaker(audioio::e_FrontLeft,ui.m_frontLeftCombo,true);
	updateSpeaker(audioio::e_FrontRight,ui.m_frontRightCombo,true);
	updateSpeaker(audioio::e_Center,ui.m_centerCombo,true);
	updateSpeaker(audioio::e_SurroundLeft,ui.m_surroundLeftCombo,true);
	updateSpeaker(audioio::e_SurroundRight,ui.m_surroundRightCombo,true);
	updateSpeaker(audioio::e_RearLeft,ui.m_rearLeftCombo,true);
	updateSpeaker(audioio::e_RearRight,ui.m_rearRightCombo,true);
	updateSpeaker(audioio::e_LFE,ui.m_subwooferCombo,false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::uiSpeaker8()
{
	updateSpeaker(audioio::e_FrontLeft,ui.m_frontLeftCombo,true);
	updateSpeaker(audioio::e_FrontRight,ui.m_frontRightCombo,true);
	updateSpeaker(audioio::e_Center,ui.m_centerCombo,true);
	updateSpeaker(audioio::e_SurroundLeft,ui.m_surroundLeftCombo,true);
	updateSpeaker(audioio::e_SurroundRight,ui.m_surroundRightCombo,true);
	updateSpeaker(audioio::e_RearLeft,ui.m_rearLeftCombo,true);
	updateSpeaker(audioio::e_RearRight,ui.m_rearRightCombo,true);
	updateSpeaker(audioio::e_LFE,ui.m_subwooferCombo,true);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::updateSpeaker(audioio::ChannelType chType,QComboBox *speakerCombo,bool enableFlag)
{
	QString eImageN,dImageN;

	getChannelResources(chType,eImageN,dImageN);
	speakerCombo->blockSignals(true);
	speakerCombo->clear();
	if(enableFlag)
	{
		tint chIdx = m_device->channelMap()->channel(chType);
	
		for(int i=0;i<m_device->noChannels();i++)
		{
			speakerCombo->addItem(QString::number(i), QVariant(i));
		}
		speakerCombo->addItem("Speaker Disabled", QVariant(-1));
		if(chIdx >= 0)
		{
			speakerCombo->setCurrentIndex(chIdx);
			getChannelButton(chType)->setEnabled(true);
		}
		else
		{
			speakerCombo->setCurrentIndex(m_device->noChannels());
			getChannelButton(chType)->setEnabled(false);
		}
	}
	else
	{
		speakerCombo->addItem("Disabled");
		speakerCombo->setCurrentIndex(0);
		getChannelButton(chType)->setEnabled(false);
	}
	speakerCombo->setEnabled(enableFlag);
	speakerCombo->blockSignals(false);
}

//-------------------------------------------------------------------------------------------

QStringList SettingsAudio::channelTestPlaylist(audioio::ChannelType type)
{
	static QString centerFile("(01)-center.wav");
	static QString leftFrontFile("(02)-left_front.wav");
	static QString rightFrontFile("(03)-right_front.wav");
	static QString leftSurroundFile("(04)-left_surround.wav");
	static QString rightSurroundFile("(05)-right_surround.wav");
	static QString leftRearFile("(06)-left_rear.wav");
	static QString rightRearFile("(07)-right_rear.wav");
	static QString subwooferFile("(08)-subwoofer.wav");

	int i;
	QString spName;
	QStringList chList;
	
	switch(type)
	{
		case audioio::e_FrontLeft:
			spName = leftFrontFile;
			break;
		case audioio::e_FrontRight:
			spName = rightFrontFile;
			break;
		case audioio::e_Center:
			spName = centerFile;
			break;
		case audioio::e_LFE:
			spName = subwooferFile;
			break;
		case audioio::e_SurroundLeft:
			spName = leftSurroundFile;
			break;
		case audioio::e_SurroundRight:
			spName = rightSurroundFile;
			break;
		case audioio::e_RearLeft:
			spName = leftRearFile;
			break;
		case audioio::e_RearRight:
			spName = rightRearFile;
			break;
		case audioio::e_UnknownChannel:
		default:
			break;
	}

	if(type==audioio::e_UnknownChannel)
	{
		if(getChannelButton(audioio::e_Center)->isEnabled())
		{
			chList.append(centerFile);
		}
		if(getChannelButton(audioio::e_FrontLeft)->isEnabled())
		{
			chList.append(leftFrontFile);
		}
		if(getChannelButton(audioio::e_FrontRight)->isEnabled())
		{
			chList.append(rightFrontFile);
		}
		if(getChannelButton(audioio::e_SurroundLeft)->isEnabled())
		{
			chList.append(leftSurroundFile);
		}
		if(getChannelButton(audioio::e_SurroundRight)->isEnabled())
		{
			chList.append(rightSurroundFile);
		}
		if(getChannelButton(audioio::e_RearLeft)->isEnabled())
		{
			chList.append(leftRearFile);
		}
		if(getChannelButton(audioio::e_RearRight)->isEnabled())
		{
			chList.append(rightRearFile);
		}
		if(getChannelButton(audioio::e_LFE)->isEnabled())
		{
			chList.append(subwooferFile);
		}
	}
	else
	{
		for(i=0;i<3;i++)
		{
			chList.append(spName);
		}
	}

	for(i=0;i<chList.size();i++)
	{
		QString& fName = chList[i];
		fName = ":/surround/Resources/surround/" + fName;
	}
	return chList;
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onTestCenter()
{
	Player *playerDlg = getPlayerDialog();
	m_testIndex = 0;
	m_testFilelist = channelTestPlaylist(audioio::e_Center);
	playerDlg->startChannelTest();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onTestLeftFront()
{
	Player *playerDlg = getPlayerDialog();
	m_testIndex = 0;
	m_testFilelist = channelTestPlaylist(audioio::e_FrontLeft);
	playerDlg->startChannelTest();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onTestRightFront()
{
	Player *playerDlg = getPlayerDialog();
	m_testIndex = 0;
	m_testFilelist = channelTestPlaylist(audioio::e_FrontRight);
	playerDlg->startChannelTest();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onTestLeftSurround()
{
	Player *playerDlg = getPlayerDialog();
	m_testIndex = 0;
	m_testFilelist = channelTestPlaylist(audioio::e_SurroundLeft);
	playerDlg->startChannelTest();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onTestRightSurround()
{
	Player *playerDlg = getPlayerDialog();
	m_testIndex = 0;
	m_testFilelist = channelTestPlaylist(audioio::e_SurroundRight);
	playerDlg->startChannelTest();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onTestLeftRear()
{
	Player *playerDlg = getPlayerDialog();
	m_testIndex = 0;
	m_testFilelist = channelTestPlaylist(audioio::e_RearLeft);
	playerDlg->startChannelTest();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onTestRightRear()
{
	Player *playerDlg = getPlayerDialog();
	m_testIndex = 0;
	m_testFilelist = channelTestPlaylist(audioio::e_RearRight);
	playerDlg->startChannelTest();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onTestSubwoofer()
{
	Player *playerDlg = getPlayerDialog();
	m_testIndex = 0;
	m_testFilelist = channelTestPlaylist(audioio::e_LFE);
	playerDlg->startChannelTest();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onTestFull()
{
	Player *playerDlg = getPlayerDialog();
	m_testIndex = 0;
	m_testFilelist = channelTestPlaylist(audioio::e_UnknownChannel);
	playerDlg->startChannelTest();
}

//-------------------------------------------------------------------------------------------

QString SettingsAudio::nextSpeakerFile()
{
	QString tName;
	
	if(m_testIndex<m_testFilelist.size())
	{
		tName = m_testFilelist.at(m_testIndex);
	}
	m_testIndex++;
	return tName;
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onCheckExclusive(bool checked)
{
	tint noChs;
	
	audioio::AudioSettings::instance(m_device->name())->setExclusive(checked);
	updateFromChannelMap();
	m_audio->resetPlay();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::updateSpeakerComboFromChannelMap()
{
	int idx;

	if(m_device->noChannels() != ui.m_speakerCombo->count())
	{
		updateSpeakerCombo();
	}
	idx = m_device->channelMap()->noMappedChannels() - 1;
	ui.m_speakerCombo->blockSignals(true);
	ui.m_speakerCombo->setCurrentIndex(idx);
	ui.m_speakerCombo->blockSignals(false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::updateStereoComboFromChannelMap()
{
	ui.m_stereoCombo->blockSignals(true);
	setStereoComboFromChannelMap();
	ui.m_stereoCombo->blockSignals(false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::updateChannelAtFromChannelMap(audioio::ChannelType chType)
{
	updateSpeaker(chType, getChannelCombo(chType), m_device->channelMap()->isValidChannel(chType));
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::updateAllChannelsFromChannelMap()
{
	for(int chIdx = 0; chIdx < 8; chIdx++)
	{
		updateChannelAtFromChannelMap(static_cast<audioio::ChannelType>(chIdx));
	}
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::updateFromChannelMap()
{
	updateSpeakerComboFromChannelMap();
	updateStereoComboFromChannelMap();
	updateUseCenter();
	updateUseLFE();
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::updateUseCenter()
{
	tint noChannelsUsed;
	bool isEnabled;
	
	ui.m_useCenter->blockSignals(true);
	noChannelsUsed = ui.m_speakerCombo->currentIndex() + 1;
	isEnabled = (noChannelsUsed == 3 || noChannelsUsed >= 5) ? true : false;
	if(isEnabled)
	{
		if(m_device->channelMap()->isStereoCenter())
		{
			ui.m_useCenter->setCheckState(Qt::Checked);
		}
		else
		{
			ui.m_useCenter->setCheckState(Qt::Unchecked);
		}
		ui.m_useCenter->setEnabled(true);
	}
	else
	{
		ui.m_useCenter->setEnabled(false);
		ui.m_useCenter->setCheckState(Qt::Unchecked);
	}
	ui.m_useCenter->blockSignals(false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::updateUseLFE()
{
	tint noChannelsUsed;
	bool isEnabled;
	
	ui.m_useSubwoofer->blockSignals(true);
	noChannelsUsed = ui.m_speakerCombo->currentIndex() + 1;
	isEnabled = (noChannelsUsed == 6 || noChannelsUsed == 8) ? true : false;
	if(isEnabled)
	{
		if(m_device->channelMap()->isStereoLFE())
		{
			ui.m_useSubwoofer->setCheckState(Qt::Checked);
		}
		else
		{
			ui.m_useSubwoofer->setCheckState(Qt::Unchecked);
		}
		ui.m_useSubwoofer->setEnabled(true);
	}
	else
	{
		ui.m_useSubwoofer->setEnabled(false);
		ui.m_useSubwoofer->setCheckState(Qt::Unchecked);
	}
	ui.m_useSubwoofer->blockSignals(false);
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onCheckUseCenter(bool checked)
{
	if(m_device->channelMap()->setStereoCenter(checked))
	{
		m_device->saveChannelMap();
		m_audio->resetPlay();
	}
	else
	{
		ui.m_useCenter->blockSignals(true);
		ui.m_useCenter->setChecked(m_device->channelMap()->isStereoCenter());
		ui.m_useCenter->blockSignals(false);
	}
}

//-------------------------------------------------------------------------------------------

void SettingsAudio::onCheckUseLFE(bool checked)
{
	if(m_device->channelMap()->setStereoLFE(checked))
	{
		m_device->saveChannelMap();
		m_audio->resetPlay();
	}
	else
	{
		ui.m_useSubwoofer->blockSignals(true);
		ui.m_useSubwoofer->setChecked(m_device->channelMap()->isStereoLFE());
		ui.m_useSubwoofer->blockSignals(false);
	}
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
