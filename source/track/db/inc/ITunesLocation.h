//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_ITUNESLOCATION_H
#define __OMEGA_TRACK_DB_ITUNESLOCATION_H
//-------------------------------------------------------------------------------------------

#include "track/db/inc/TrackDB.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QSet>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

typedef bool (*CancelScanFunction)(void *);

//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT ITunesLocation : public QObject
{
    public:
        Q_OBJECT

    public:
        ITunesLocation();
        virtual ~ITunesLocation();

        virtual QStringList getITunesConfigFileNames();

        virtual QSet<int> getLocationIDSet();
        virtual QString getDirectoryLocation(int ID);
        virtual QString getFileLocation(int ID);

        virtual int addFileLocation(const QString& pos);
        virtual void clearLocation(int ID);

        virtual QString findITunesDBInDirectory(const QString& dirName);
        virtual QString findITunesDBInDirectory(const QString& dirName,CancelScanFunction cancelFunc,void *pUserData);

    protected:

        virtual bool isValidPList(const QString& fileName) const;
        virtual bool isLineXMLHeader(const QString& line) const;
        virtual bool isLinePListHeader(const QString& line) const;

        virtual int getLocationNextID();
        virtual int addDirectoryLocation(const QString& pos);
        virtual bool updateFileLocation(int ID,const QString& pos);
        virtual void clearLocations();
        virtual int sizeLocations();

        virtual bool isITunesXMLName(const QString& fileName) const;

        virtual QStringList defaultITunesDirectory();

        virtual QString findITunesDBInDirectoryImpl(const QString& dirName,CancelScanFunction cancelFunc,void *pUserData,int& progressCount);
        virtual QString findITunesDBInDirectoryRecursive(const QString& dirName,CancelScanFunction cancelFunc,void *pUserData,int& progressCount);

        virtual common::TimeStamp getModifiedFileTime(const QString& fileName);

        virtual void getFileAndDirectoryLocationsFromSettings(QSet<QString>& confDirSet,QSet<QString>& confFileSet);
        virtual QStringList validateDirectorySet(const QSet<QString>& dirSet);
        virtual QStringList validateFileSet(const QSet<QString>& fileSet);
        virtual QStringList getDefaultITuneDBs();
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
