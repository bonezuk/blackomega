//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_SANDBOXBOOKMARKSERVICE_H
#define __OMEGA_TRACK_DB_SANDBOXBOOKMARKSERVICE_H
//-------------------------------------------------------------------------------------------

#include "common/inc/SBService.h"
#include "track/db/inc/TrackDB.h"
#include "track/db/inc/SBBookmarkTrackDB.h"
#include "track/info/inc/SBBookmarkService.h"

#include <QRecursiveMutex>
#include <QSet>

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT SandboxBookmarkService : public track::info::SBBookmarkService
{
    public:
        SandboxBookmarkService();
        virtual ~SandboxBookmarkService();

        virtual bool isInAppContainer(const QString& fileName) const;
        virtual bool isMusicDirectory(const QString& fileName);
        virtual bool isPlaylist(const QString& fileName);

        virtual bool has(const QString& fileName);
        virtual bool has(const QString& fileName,bool readOnlyFlag);
        virtual bool has(const QString& docFileName,const QString& refFileName);
        virtual bool has(const QString& docFileName,const QString& refFileName,bool readOnlyFlag);
        
        virtual bool add(const QString& fileName,bool readOnlyFlag);
        virtual bool add(const QString& docFileName,const QStringList& refFileNames,bool readOnlyFlag);
        virtual bool add(const QString& docFileName,const QString& refFileName,bool readOnlyFlag,const QByteArray& bkArray);
        
        virtual bool checkOut(const QString& fileName,bool readOnlyFlag);
        
        virtual bool checkIn(const QString& fileName,bool readOnlyFlag);
        
        virtual QByteArray getBookmarkArray(const QString& docFileName,const QString& refFileName);
        
        virtual QString userErrorMessage(track::info::UserSandboxErrorMessage err);
        
    protected:
    
        QRecursiveMutex m_mutex;
        QMap<QString,QMap<QString,QPair<void *,int> > > m_referenceCountMap;
        
        virtual void lock();
        virtual void unlock();
        
        virtual QString key(const QString& fileName);
        virtual QString key(NSURL *fileUrl);
        
        virtual QString pathFromKey(const QString& k);
        
        virtual NSURL *toUrl(const QString& fileName);

        virtual bool hasURL(const QString& key,bool readOnlyFlag);
        virtual bool hasURL(const QString& docKey,const QString& refKey,bool readOnlyFlag);
                
        virtual QMap<QString,QMap<QString,QPair<void *,int> > >& getReferenceCountMap();
        
        virtual SQLiteDatabase *getDB();
        virtual SQLiteInsert *createDBInsert();
        virtual void removeDBInsert(SQLiteInsert *pInsert);
        virtual SQLiteQuery *createDBQuery();
        virtual void removeDBQuery(SQLiteQuery *pQuery);
        
        virtual common::TimeStamp currentAccessTime();
        
        virtual bool addToDB(const QString& fileUrl,const QString& docUrl,bool readOnlyFlag,const QByteArray& bkArray);
        
        virtual void updateAccessTime(const QString& keyRef,const QString& docRef);
        virtual void updateAccessTime(const QString& keyRef,const QString& docRef,const common::TimeStamp& accessTime);
        
        virtual NSData *securityBookmarkFromURL(const QString& fileName,NSURLBookmarkCreationOptions options);
        virtual NSData *securityBookmarkFromURL(const QString& docFileName,const QString& refFileName,NSURLBookmarkCreationOptions options);
        
        virtual QByteArray securityBookmark(const QString& fileName,bool readOnlyFlag);
        virtual QByteArray securityBookmark(const QString& docFileName,const QString& refFileName,bool readOnlyFlag);
        
        virtual bool addRootDocument(const QString& fileName,bool readOnlyFlag);
        
        virtual void getAccessMap(const QString& fileName,QMap<QString,QPair<bool,common::TimeStamp> >& chMap);
        
        virtual bool canAccess(bool readOnlyFlag,bool access) const;
        
        virtual bool isBookmarkRequired(const QString& fileName,bool readOnlyFlag);
        
        virtual bool accessDocumentUrlForFile(const QString& fileName,bool readOnlyFlag,bool checkInFlag,QString& docFileName);
        
        virtual bool isBookmarkCreationRequired(const QString& fileName,const QString& docFileName);
        virtual NSData *getBookmarkFromReference(const QString& fileName,const QString& docFileName);
        virtual void addSecuredResourceToReferenceMap(const QString& fileName,const QString& docFileName,NSURL *bookmark);
        virtual NSURL *resolveSecurityScopedBookmarkData(NSData *bkData,const QString& docFileName);
        virtual bool startAccessingSecuredResource(NSURL *url);
        virtual bool checkOutSecuredResource(const QString& fileName,const QString& docFileName,NSData *bookmarkData);
        virtual void deleteBookmark(const QString& fileName,const QString& docFileName);
        
        virtual bool checkOutBookmarkResource(const QString& fileName,const QString& docFileName);
        virtual bool checkOutRecursive(const QString& fileName,bool readOnlyFlag);
        virtual bool checkOutParentDirectory(const QString& fileName,bool readOnlyFlag);
        
        virtual bool checkInDocumentAsRequired(const QString& docFileName,bool readOnlyFlag);
        virtual void stopScopedResource(NSURL *url);
        virtual bool doCheckIn(const QString& fileName,const QString& docFileName,bool readOnlyFlag);
        
        virtual bool canCheckIn(const QString& fileName,const QString& docFileName);
        
        virtual bool checkInParentDirectory(const QString& fileName,bool readOnlyFlag);
        
        virtual void shutdown();
        
        virtual QString formatFileName(const QString& fileName) const;
        
        virtual bool isInDirectory(const QString& fileName,const QString& dirName) const;
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
