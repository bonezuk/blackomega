//-------------------------------------------------------------------------------------------
#ifndef __TRACK_DB_SBBOOKMARKTRACKDB_H
#define __TRACK_DB_SBBOOKMARKTRACKDB_H
//-------------------------------------------------------------------------------------------

#include "common/inc/SBService.h"
#include "track/db/inc/TrackDB.h"

#include <QMutex>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT SBBookmarkTrackDB : public common::SBBookmark
{
    public:
        SBBookmarkTrackDB();
        virtual ~SBBookmarkTrackDB();

        virtual bool has(const QString& fileName,bool readOnlyFlag);
        virtual bool has(const QString& docFileName,const QString& refFileName,bool readOnlyFlag);

        virtual bool add(const QString& fileName,bool readOnlyFlag);
        virtual bool add(const QString& docFileName,const QStringList& refFileNames,bool readOnlyFlag);

        virtual bool checkOut(const QString& fileName,bool readOnlyFlag);
        virtual bool checkIn(const QString& fileName,bool readOnlyFlag);
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
