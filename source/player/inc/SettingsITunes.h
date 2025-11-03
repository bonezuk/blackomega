//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYER_SETTINGSITUNES_H
#define __OMEGA_PLAYER_SETTINGSITUNES_H
//-------------------------------------------------------------------------------------------

#include "player/inc/SettingsBase.h"
#include "player/inc/ProgressMessageDialog.h"
#include "player/ui_SettingsITunes.h"

#include <QFileDialog>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

class SettingsITunes : public SettingsBase
{
    public:
        Q_OBJECT

    public:
        SettingsITunes(QWidget *parent = 0,Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~SettingsITunes();

        virtual void onSelected(int index);

    protected:

        Ui::SettingsITunes ui;
        QFileDialog *m_loadDirDialog;
        ProgressMessageDialog *m_progressDialog;
        QStringList m_loadFileList;

        virtual void processAddDirectory();

        void populateDBList();
        QString defaultMusicFolder();
        QString findITunesDB(const QString& dirName);
        void processAddDirectories(const QStringList& dirList);

        static bool scanProgress(void *pUserData);
        bool scanProgressImpl();

    protected slots:

        void onListSelected();
        void onAdd();
        void onRemove();
        void doLoadDirectory(const QString& dirName);
        void onLoadTimer();

    signals:

        void onLoadDirectory(const QString& dirName);
};

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
