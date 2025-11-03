//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYERAPP_PLAYERIOS_PLAYERIOSTRACKDBMANAGER_H
#define __OMEGA_PLAYERAPP_PLAYERIOS_PLAYERIOSTRACKDBMANAGER_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"
#include "playerapp/playerios/inc/PlayerIOSUtils.h"
#include "track/db/inc/TrackDB.h"
#include "track/db/inc/DBInfo.h"

#include <QTimer>

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PlayerIOSTrackDBManager : public QObject
{
    public:
        Q_OBJECT
    public:
        virtual ~PlayerIOSTrackDBManager();

        static PlayerIOSTrackDBManager *instance(QObject *parent = 0);
        void release();

        virtual void rebuildDatabase();

    public slots:
        void addUploadedFile(const QString& fileName);
        void onDeleteFile(const QString& fileName);

    protected slots:
        void onFlushLog();

    signals:
        void newtrack(const QString& fileName);
        void removetrack(const QString& fileName);

    private:
        static PlayerIOSTrackDBManager *m_instance;
        QTimer *m_logFlushTimer;

        PlayerIOSTrackDBManager(QObject *parent = 0);
        virtual void printError(const tchar *strR, const tchar *strE) const;
        virtual QString trackDBPath() const;
        virtual bool open();
        virtual void close();

        virtual void renameDBFile(const QString& fileDBName);
        virtual void buildDBForDirectory(const QString& dirName);
        virtual bool defineMountpoint();
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
