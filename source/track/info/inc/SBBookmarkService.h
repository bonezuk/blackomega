//-------------------------------------------------------------------------------------------
#ifndef __TRACK_INFO_SBBOOKMARKSERVICE_H
#define __TRACK_INFO_SBBOOKMARKSERVICE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/SBService.h"
#include "common/inc/Factory.h"
#include "track/info/inc/Info.h"

#include <QMutex>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

typedef enum
{
    e_cannotLoadPlaylist = 0,
} UserSandboxErrorMessage;

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT SBBookmarkService
{
    public:
        SBBookmarkService();
        virtual ~SBBookmarkService();

        static QString getHomeDirectory();

        static QSharedPointer<SBBookmarkService> instance();
        static void release();

        virtual bool isMusicDirectory(const QString& fileName) = 0;
        virtual bool isPlaylist(const QString& fileName) = 0;

        virtual bool has(const QString& fileName) = 0;
        virtual bool has(const QString& fileName,bool readOnlyFlag) = 0;
        virtual bool has(const QString& docFileName,const QString& refFileName) = 0;
        virtual bool has(const QString& docFileName,const QString& refFileName,bool readOnlyFlag) = 0;

        virtual bool add(const QString& fileName,bool readOnlyFlag) = 0;
        virtual bool add(const QString& docFileName,const QStringList& refFileNames,bool readOnlyFlag) = 0;
        virtual bool add(const QString& docFileName,const QString& refFileName,bool readOnlyFlag,const QByteArray& bkArray) = 0;

        virtual bool checkOut(const QString& fileName,bool readOnlyFlag) = 0;

        virtual bool checkIn(const QString& fileName,bool readOnlyFlag) = 0;

        virtual QByteArray getBookmarkArray(const QString& docFileName,const QString& refFileName) = 0;

        virtual QString userErrorMessage(UserSandboxErrorMessage err) = 0;

    protected:

        static QSharedPointer<SBBookmarkService> m_instance;
};

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(TRACK_INFO_EXPORT, SBBookmarkServiceFactory, SBBookmarkService)

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
