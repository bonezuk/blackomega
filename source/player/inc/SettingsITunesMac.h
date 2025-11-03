//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_SETTINGSITUNESMAC_H
#define __OMEGA_PLAYER_SETTINGSITUNESMAC_H
//-------------------------------------------------------------------------------------------

#include "player/inc/SettingsITunes.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class SettingsITunesMac : public SettingsITunes
{
    public:
        Q_OBJECT
        
    public:
        SettingsITunesMac(QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~SettingsITunesMac();
        
        void onFolderOpen(const QStringList& files);
        void onCancel();

    protected:
        
        void *m_loader;
        
        void *toUrl(const QString& fileName);
        
        virtual void processAddDirectory();
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

