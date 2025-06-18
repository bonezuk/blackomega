//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_SETTINGSGENERAL_H
#define __OMEGA_PLAYER_SETTINGSGENERAL_H
//-------------------------------------------------------------------------------------------

#include "player/inc/SettingsBase.h"
#include "player/ui_SettingsGeneral.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class SettingsGeneral : public SettingsBase
{
    public:
        Q_OBJECT

    public:
        SettingsGeneral(QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~SettingsGeneral();

        virtual void onSelected(int index);

    protected:
        Ui::SettingsGeneral ui;

        virtual void setLoadMemoryCheck();

    protected Q_SLOTS:
        void onBufferMemory(bool checked);
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
