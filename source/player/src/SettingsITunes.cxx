#include "player/inc/SettingsITunes.h"
#include "track/db/inc/ITunesLocation.h"
#include "common/inc/CommonDirectoriesForFiles.h"
#include "player/inc/Settings.h"
#include "common/inc/SBService.h"

#include <QMessageBox>
#include <QCoreApplication>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace player
{
//-------------------------------------------------------------------------------------------

SettingsITunes::SettingsITunes(QWidget *parent,Qt::WindowFlags f) : SettingsBase(parent,f),
    m_loadDirDialog(0),
    m_progressDialog(0),
    m_loadFileList()
{
    ui.setupUi(this);

    QObject::connect(ui.m_list,SIGNAL(itemSelectionChanged()),this,SLOT(onListSelected()));
    QObject::connect(ui.m_addButton,SIGNAL(clicked()),this,SLOT(onAdd()));
    QObject::connect(ui.m_removeButton,SIGNAL(clicked()),this,SLOT(onRemove()));
#if defined(OMEGA_WIN32)
    QObject::connect(this,SIGNAL(onLoadDirectory(const QString&)),this,SLOT(doLoadDirectory(const QString&)));
#endif
}

//-------------------------------------------------------------------------------------------

SettingsITunes::~SettingsITunes()
{
    if(m_loadDirDialog!=0)
    {
        delete m_loadDirDialog;
        m_loadDirDialog = 0;
    }
    if(m_progressDialog!=0)
    {
        delete m_progressDialog;
        m_progressDialog = 0;
    }
}

//-------------------------------------------------------------------------------------------

void SettingsITunes::onSelected(int index)
{
    if(index==3)
    {
        populateDBList();
    }
}

//-------------------------------------------------------------------------------------------

void SettingsITunes::populateDBList()
{
    QSet<int> IDs;
    QSet<int>::iterator ppI;
    track::db::ITunesLocation location;

    ui.m_list->clear();

    IDs = location.getLocationIDSet();
    for(ppI=IDs.begin();ppI!=IDs.end();ppI++)
    {
        QString dirName = location.getFileLocation(*ppI);
        if(dirName.isEmpty())
        {
            dirName = location.getDirectoryLocation(*ppI);
        }
        else
        {
            common::CommonDirectoriesForFiles cDir;
            dirName = cDir.path(dirName,1);
        }

        if(!dirName.isEmpty())
        {
            QListWidgetItem *item = new QListWidgetItem(dirName,ui.m_list);
            item->setData(Qt::UserRole,QVariant(*ppI));
        }
    }

    ui.m_addButton->setEnabled(true);
    ui.m_removeButton->setEnabled(false);
}

//-------------------------------------------------------------------------------------------

void SettingsITunes::onListSelected()
{
    ui.m_removeButton->setEnabled(!ui.m_list->selectedItems().isEmpty());
}

//-------------------------------------------------------------------------------------------

void SettingsITunes::onAdd()
{
    processAddDirectory();
}

//-------------------------------------------------------------------------------------------

void SettingsITunes::onRemove()
{
    track::db::ITunesLocation location;
    QList<QListWidgetItem *> selectedItems = ui.m_list->selectedItems();

    ui.m_list->blockSignals(true);
    for(QList<QListWidgetItem *>::iterator ppI=selectedItems.begin();ppI!=selectedItems.end();ppI++)
    {
        QListWidgetItem *item = *ppI;
        int ID = item->data(Qt::UserRole).toInt();
        location.clearLocation(ID);
    }
    ui.m_list->blockSignals(false);

    populateDBList();
}

//-------------------------------------------------------------------------------------------

QString SettingsITunes::defaultMusicFolder()
{
    QString mDir;
    QStringList mDirList;
#if QT_VERSION >= 0x050000
    mDirList = QStandardPaths::standardLocations(QStandardPaths::MusicLocation);
#else
    mDirList += QDir::homePath();
#endif
    if(mDirList.size()>0)
    {
        mDir = mDirList.at(0);
    }
    else
    {
        mDir = common::SBService::homeDirectory();
    }
    return mDir;
}

//-------------------------------------------------------------------------------------------

bool SettingsITunes::scanProgress(void *pUserData)
{
    SettingsITunes *iTunesSettings = reinterpret_cast<SettingsITunes *>(pUserData);
    return iTunesSettings->scanProgressImpl();
}

//-------------------------------------------------------------------------------------------

bool SettingsITunes::scanProgressImpl()
{
    bool flag = true;

    if(m_progressDialog!=0)
    {
        flag = (!m_progressDialog->isCancelled()) ? true : false;
        if(flag)
        {
            QCoreApplication::processEvents();
        }
    }
    return flag;
}

//-------------------------------------------------------------------------------------------

QString SettingsITunes::findITunesDB(const QString& dirName)
{
    track::db::ITunesLocation location;

    if(m_progressDialog!=0)
    {
        delete m_progressDialog;
    }
    m_progressDialog = new ProgressMessageDialog("Scanning directory for iTunes database...",this);
    m_progressDialog->setModal(true);
    m_progressDialog->show();

    QString dbFileName = location.findITunesDBInDirectory(dirName,SettingsITunes::scanProgress,(void *)this);

    m_progressDialog->hide();
    delete m_progressDialog;
    m_progressDialog = 0;

    return dbFileName;
}

//-------------------------------------------------------------------------------------------

void SettingsITunes::processAddDirectories(const QStringList& dirList)
{
    for(QStringList::const_iterator ppI=dirList.begin();ppI!=dirList.end();ppI++)
    {
        const QString& dirName = *ppI;
        QString dbFileName = findITunesDB(dirName);
        if(!dbFileName.isEmpty())
        {
            QSet<int> IDs;
            track::db::ITunesLocation location;
            bool duplicate = false;

            IDs = location.getLocationIDSet();
            for(QSet<int>::iterator ppI=IDs.begin();ppI!=IDs.end();ppI++)
            {
                int ID = *ppI;
                QString fName = location.getFileLocation(ID);
                if(!fName.isEmpty() && fName==dbFileName)
                {
                    duplicate = true;
                }
            }

            if(!duplicate)
            {
                common::CommonDirectoriesForFiles cDir;
                QString dName = cDir.path(dbFileName,1);
                int newID = location.addFileLocation(dbFileName);
                QListWidgetItem *item = new QListWidgetItem(dName,ui.m_list);
                item->setData(Qt::UserRole,QVariant(newID));
            }
            else
            {
                QString err = "The iTunes database in directory '" + dirName + "' already added to collection";
                QMessageBox::information(this,"ITunes Configuration",err);
            }
        }
        else
        {
            QString err = "Unable to locate iTunes database in directory '" + dirName + "'";
            QMessageBox::information(this,"ITunes Configuration",err);
        }
    }
}

//-------------------------------------------------------------------------------------------

void SettingsITunes::processAddDirectory()
{
    QString dirName = defaultMusicFolder();
    QString title = "Select iTunes Directory";

#if defined(OMEGA_WIN32)
    QString dName = QFileDialog::getExistingDirectory(this,title,dirName);
    if(!dName.isEmpty())
    {
        Q_EMIT onLoadDirectory(dName);
    }
#else
    if(m_loadDirDialog!=0)
    {
        delete m_loadDirDialog;
        m_loadDirDialog = 0;
    }
    m_loadDirDialog = new QFileDialog(this,Qt::Sheet);
    m_loadDirDialog->setAcceptMode(QFileDialog::AcceptOpen);
    m_loadDirDialog->setFileMode(QFileDialog::Directory);
    m_loadDirDialog->setViewMode(QFileDialog::Detail);
    m_loadDirDialog->setWindowTitle(title);
    m_loadDirDialog->setDirectory(dirName);
    m_loadDirDialog->open(this,SLOT(doLoadDirectory(const QString&)));
#endif
}

//-------------------------------------------------------------------------------------------

void SettingsITunes::doLoadDirectory(const QString& dirName)
{
    m_loadFileList.clear();
    m_loadFileList.append(dirName);
    QTimer::singleShot(100,this,SLOT(onLoadTimer()));
}

//-------------------------------------------------------------------------------------------

void SettingsITunes::onLoadTimer()
{
    QStringList dirList;
    dirList.append(m_loadFileList);
    m_loadFileList.clear();
    processAddDirectories(dirList);
}

//-------------------------------------------------------------------------------------------
} // namespace player
} // namespace omega
//-------------------------------------------------------------------------------------------
