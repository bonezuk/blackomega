//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERCOMMON_PLAYERTRACKDBMANAGER_H
#define __PLAYERAPP_PLAYERCOMMON_PLAYERTRACKDBMANAGER_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"
#include "common/inc/DiskIF.h"
#include "common/inc/AbstractProgressInterface.h"
#include "track/db/inc/TrackDB.h"
#include "track/db/inc/DBInfo.h"
#include "playerapp/playercommon/inc/PlayerCommonDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT PlayerTrackDBManager : public QObject
{
    public:
        Q_OBJECT

    public:
        PlayerTrackDBManager(QObject *parent = 0);
        virtual ~PlayerTrackDBManager();

        virtual bool buildDatabaseFromMountpoints(const QString& trackDBPath, const QStringList& mountPoints, common::AbstractProgressInterface *progress = 0);

    protected:

        virtual void printError(const tchar *strR, const tchar *strE) const;
        virtual bool isProgressCancelled(common::AbstractProgressInterface *progress);
        virtual void scanDirectoryRecursive(const QString& path, QStringList& fileList, int level, common::AbstractProgressInterface *progress, int mountNo, int mountTotal);
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
