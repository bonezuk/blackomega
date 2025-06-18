//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_SETTINGSFILE_H
#define __OMEGA_PLAYER_SETTINGSFILE_H
//-------------------------------------------------------------------------------------------

#include "player/inc/SettingsBase.h"
#include "player/ui_SettingsFile.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class SettingsFile : public SettingsBase
{
    public:
        Q_OBJECT

    public:
        SettingsFile(QWidget *parent,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~SettingsFile();

        virtual void onSelected(int index);

    protected:
    
        Ui::SettingsFile ui;
            
#if defined(OMEGA_WIN32)
        RegisterFileType::FileType getTableFileType(const QString& ext);
        bool getLocalUser();
        bool getExplorerFlag();
        bool getShellDefault();
#endif

        void populateFileTypeTable();
        void addFileTypeRow(const QString& typeStr,const QString& description);
        QStringList getExtensions(const QString& text);

    protected slots:
    
        void onFileTab(int index);
        void onCheckChange(QTableWidgetItem *item);
        void onExplorerCheck();
        void onDirectoryCheck();
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
