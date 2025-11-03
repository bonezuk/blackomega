//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_SBSERVICE_H
#define __OMEGA_COMMON_SBSERVICE_H
//-------------------------------------------------------------------------------------------

#include <QString>
#include <QObject>

#include "common/inc/TimeStamp.h"
#include "common/inc/Factory.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

#if defined(OMEGA_MACOSX)
#define SBFACTORYKEY "mac"
#else
#define SBFACTORYKEY "win"
#endif

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT SBService : public QObject
{
    public:
        Q_OBJECT

    public:
        SBService(QObject *parent = 0);
        virtual ~SBService();

        static QString homeDirectory();
        static QString tempDirectory();
        static QString applicationDataDirectory();

        virtual void loadDirDialog(QObject *parent,const QString& title,const QString& dirName) = 0;
        virtual void loadFilesDialog(QObject *parent,const QString& title,const QString& dirName,const QString& filter) = 0;
        virtual void saveFileDialog(QObject *parent,const QString& title,const QString& dirName,const QString& filter) = 0;

        virtual void *allocatePool() = 0;
        virtual void releasePool(void *pPtr) = 0;

    protected:

        virtual QString getHomeDirectory() = 0;
        virtual QString getTempDirectory() = 0;
        virtual QString getApplicationDataDirectory() = 0;

    signals:

        void onLoadDirectory(const QString& dirName);
        void onLoadFiles(const QStringList& fileList);
        void onSaveFile(const QString& fileName,const QString& selFilter);
};

typedef QSharedPointer<SBService> SBServicePtr;

ABSTRACT_FACTORY_CLASS(COMMON_EXPORT,SBServiceFactory,SBService);

//-------------------------------------------------------------------------------------------

class COMMON_EXPORT SBBookmark
{
    public:
        SBBookmark();
        virtual ~SBBookmark();

        static QSharedPointer<SBBookmark> get();

        static bool isReadOnly(const QString& fileName);

        virtual bool has(const QString& fileName,bool readOnlyFlag) = 0;
        virtual bool has(const QString& docFileName,const QString& refFileName,bool readOnlyFlag) = 0;

        virtual bool add(const QString& fileName,bool readOnlyFlag) = 0;
        virtual bool add(const QString& docFileName,const QStringList& refFileNames,bool readOnlyFlag) = 0;

        virtual bool checkOut(const QString& fileName,bool readOnlyFlag) = 0;
        virtual bool checkIn(const QString& fileName,bool readOnlyFlag) = 0;

#if defined(OMEGA_MAC_STORE)
        static QString getFilename(const QString& fileName);
#endif

};

ABSTRACT_FACTORY_CLASS(COMMON_EXPORT,SBBookmarkFactory,SBBookmark);

typedef QSharedPointer<SBBookmark> SBBookmarkPtr;

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
