//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WIDGET_SANDBOXMAC_H
#define __OMEGA_WIDGET_SANDBOXMAC_H
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

#include "widget/inc/SandBoxBase.h"

#include <QFileDialog>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace widget
{
//-------------------------------------------------------------------------------------------

class WIDGET_EXPORT SBServiceMac : public SBServiceBase
{
    public:
        Q_OBJECT

    public:
        SBServiceMac(QObject *parent = 0);
        virtual ~SBServiceMac();

        virtual void loadDirDialog(QObject *parent,const QString& title,const QString& dirName);
        virtual void loadFilesDialog(QObject *parent,const QString& title,const QString& dirName,const QString& filter);
        virtual void saveFileDialog(QObject *parent,const QString& title,const QString& dirName,const QString& filter);

        virtual void *allocatePool();
        virtual void releasePool(void *pPtr);

    protected:
        QFileDialog *m_loadDirDialog;
        QFileDialog *m_loadFilesDialog;
        QFileDialog *m_saveFileDialog;
        void *m_sService;

        virtual QString getHomeDirectory();
        virtual QString getTempDirectory();
        virtual QString getApplicationDataDirectory();

        void *toUrl(const QString& in);
        QString fromUrl(void *in);
        void *fromFilter(const QString& filter);

#if defined(OMEGA_MAC_STORE)
        bool isReadOnly(const QString& fileName);
#endif

    public Q_SLOTS:
        void doLoadDirectory(const QString& dirName);
        void doLoadFiles(const QStringList& fileList);
        void doSaveFile(const QString& fileName);
};

//-------------------------------------------------------------------------------------------
} // namespace widget
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
