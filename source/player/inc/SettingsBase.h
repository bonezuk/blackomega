//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_SETTINGSBASE_H
#define __OMEGA_PLAYER_SETTINGSBASE_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "audioio/inc/AOQueryDevice.h"
#include "audioio/inc/AOBase.h"
#include "player/inc/RegisterFileTypes.h"
#include "player/inc/QSpeakerButton.h"
#include "remote/inc/WinLIRCRemote.h"

#include <QDialog>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class Player;
class SettingsCentralWidget;

//-------------------------------------------------------------------------------------------

class SettingsBase : public QWidget
{
    public:
        Q_OBJECT

    public:
        SettingsBase(QWidget *parent,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~SettingsBase();

        virtual void onSelected(int index) = 0;

    protected:

        Player *m_player;
        SettingsCentralWidget *m_central;

        Player *getPlayerDialog();
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
