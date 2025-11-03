#include "player/inc/SettingsGeneral.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

SettingsGeneral::SettingsGeneral(QWidget *parent,Qt::WindowFlags f) : SettingsBase(parent,f)
{
    ui.setupUi(this);
    QObject::connect(ui.m_loadMemCheckbox,SIGNAL(clicked(bool)),this,SLOT(onBufferMemory(bool)));
    onSelected(0);
}

//-------------------------------------------------------------------------------------------

SettingsGeneral::~SettingsGeneral()
{}

//-------------------------------------------------------------------------------------------

void SettingsGeneral::onSelected(int index)
{
    setLoadMemoryCheck();
}

//-------------------------------------------------------------------------------------------

void SettingsGeneral::setLoadMemoryCheck()
{
    bool flag;
    QSettings settings;
    settings.beginGroup("audio");
    if(settings.contains("loadFileToMemory"))
    {
        flag = settings.value("loadFileToMemory").toBool();
    }
    else
    {
        flag = false;
    }
    settings.endGroup();
    ui.m_loadMemCheckbox->setChecked(flag);
}

//-------------------------------------------------------------------------------------------

void SettingsGeneral::onBufferMemory(bool checked)
{
    QSettings settings;
    settings.beginGroup("audio");
    settings.setValue("loadFileToMemory",QVariant(checked));
    settings.endGroup();
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
