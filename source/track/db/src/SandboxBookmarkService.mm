#include "track/db/inc/SandboxBookmarkService.h"
#include "common/inc/TimeStamp.h"
#include "common/inc/DiskOps.h"
#include "common/inc/CommonDirectoriesForFiles.h"

#include <sys/types.h>
#include <pwd.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(track::info::SBBookmarkServiceFactory, track::info::SBBookmarkService, \
                            SandboxBookmarkServiceFactory, SandboxBookmarkService, \
                            "trackdb",false)

//-------------------------------------------------------------------------------------------

SandboxBookmarkService::SandboxBookmarkService() : track::info::SBBookmarkService(),
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    m_mutex(),
#else
    m_mutex(QMutex::Recursive),
#endif
    m_referenceCountMap()
{}

//-------------------------------------------------------------------------------------------

SandboxBookmarkService::~SandboxBookmarkService()
{
    SandboxBookmarkService::shutdown();
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::lock()
{
    m_mutex.lock();
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::unlock()
{
    m_mutex.unlock();
}

//-------------------------------------------------------------------------------------------

QString SandboxBookmarkService::key(const QString& fileName)
{
    return key(toUrl(fileName));
}

//-------------------------------------------------------------------------------------------

QString SandboxBookmarkService::key(NSURL *fileUrl)
{
    QString uKey;
    if(fileUrl!=nil)
    {
        const char *x = [[fileUrl absoluteString] UTF8String];
        uKey = QString::fromUtf8(x);
    }
    return uKey;
}

//-------------------------------------------------------------------------------------------

QString SandboxBookmarkService::pathFromKey(const QString& k)
{
    QString path;
    common::BString x(k.toUtf8().constData());
    NSURL *u = [NSURL URLWithString:[NSString stringWithUTF8String:(x.getString())]];
    NSString *nS = [u path];
    if(nS!=nil)
    {
        const char *y = [nS UTF8String];
        path = QString::fromUtf8(y);
    }
    return path;
}

//-------------------------------------------------------------------------------------------

NSURL *SandboxBookmarkService::toUrl(const QString& fileName)
{
    NSURL *u;

    if(!fileName.isEmpty())
    {
#if defined(OMEGA_MAC_STORE)
        common::BString cStr = fileName.toUtf8().constData();
        const char *x = cStr.getString();
        NSString *nS = [NSString stringWithUTF8String:x];
        u = [NSURL fileURLWithPath:nS];
#else
        common::BString cStr = "file://";
        cStr += fileName.toUtf8().constData();
        NSString *nS = [NSString stringWithUTF8String:cStr.c_str()];
        u = [NSURL URLWithString:nS];
#endif
    }
    else
    {
        u = nil;
    }
    return u;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::has(const QString& fileName)
{
    return has(fileName,false);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::has(const QString& fName,bool readOnlyFlag)
{
    QString fileName = formatFileName(fName);
    bool res = false;

    if(readOnlyFlag)
    {
        if(!isBookmarkRequired(fileName,readOnlyFlag))
        {
            res = true;
        }
    }

    if(!res)
    {
        int level = 0;
        QString fPath;
        common::CommonDirectoriesForFiles commonDir;

        do
        {
            fPath = commonDir.path(fileName,level);
            if(!fPath.isEmpty())
            {
                res = hasURL(key(fPath),readOnlyFlag);
            }
            level++;
        } while(!fPath.isEmpty() && !res);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::hasURL(const QString& key,bool readOnlyFlag)
{
    bool access;
    QString cmdQ,uStr;
    SQLiteQuery uKeyQ(getDB());
    bool res;

    cmdQ = "SELECT url, access FROM sandBoxURL WHERE url='" + TrackDB::dbString(key) + "' AND docUrl=''";
    uKeyQ.prepare(cmdQ);
    uKeyQ.bind(uStr);
    uKeyQ.bind(access);
    if(uKeyQ.next())
    {
        res = canAccess(readOnlyFlag,access);
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::has(const QString& docFileName,const QString& refFileName)
{
    return has(docFileName,refFileName,false);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::has(const QString& dFileName,const QString& rFileName,bool readOnlyFlag)
{
    QString docFileName = formatFileName(dFileName);
    QString refFileName = formatFileName(rFileName);
    if(readOnlyFlag)
    {
        if(!isBookmarkRequired(refFileName,readOnlyFlag))
        {
            return true;
        }
    }
    return hasURL(key(docFileName),key(refFileName),readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::hasURL(const QString& docKey,const QString& refKey,bool readOnlyFlag)
{
    bool access;
    QString cmdQ,uStr;
    SQLiteQuery uKeyQ(getDB());
    bool res;

    cmdQ = "SELECT url, access FROM sandBoxURL WHERE url='" + TrackDB::dbString(refKey) + "' AND docUrl='" + TrackDB::dbString(docKey) + "'";
    uKeyQ.prepare(cmdQ);
    uKeyQ.bind(uStr);
    uKeyQ.bind(access);
    if(uKeyQ.next())
    {
        res = canAccess(readOnlyFlag,access);
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::isMusicDirectory(const QString& fileName)
{
    QString musicDir = common::DiskOps::mergeName(getHomeDirectory(),"Music").trimmed();
    bool res = false;

    if(fileName.trimmed().length() >= musicDir.length())
    {
        QString pName = fileName.trimmed().mid(0,musicDir.length());
        if(pName==musicDir)
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::isPlaylist(const QString& fileName)
{
    int i;
    QString ext;

    for(i=fileName.length()-1;i>=0;i--)
    {
        if(fileName.at(i)==QChar('.'))
        {
            ext = fileName.mid(i+1).trimmed().toLower();
            if(ext=="m3u" || ext=="pls" || ext=="m3u8" || ext=="xspf")
            {
                return true;
            }
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

SQLiteDatabase *SandboxBookmarkService::getDB()
{
    return TrackDB::instance()->db();
}

//-------------------------------------------------------------------------------------------

SQLiteInsert *SandboxBookmarkService::createDBInsert()
{
    return new SQLiteInsert(getDB());
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::removeDBInsert(SQLiteInsert *pInsert)
{
    delete pInsert;
}

//-------------------------------------------------------------------------------------------

SQLiteQuery *SandboxBookmarkService::createDBQuery()
{
    return new SQLiteQuery(getDB());
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::removeDBQuery(SQLiteQuery *pQuery)
{
    delete pQuery;
}

//-------------------------------------------------------------------------------------------

common::TimeStamp SandboxBookmarkService::currentAccessTime()
{
    return common::TimeStamp::now();
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::addToDB(const QString& fileUrl,const QString& docUrl,bool readOnlyFlag,const QByteArray& bkArray)
{
    SQLiteInsert *dInsert = createDBInsert();
    bool res = false;

    try
    {
        QString fileUrlV = TrackDB::dbString(fileUrl);
        QString docUrlV  = TrackDB::dbString(docUrl);

        getDB()->exec("SAVEPOINT addBookmark");

        QString cmdD = "DELETE FROM sandBoxURL WHERE url='" + fileUrlV + "' AND docUrl='" + docUrlV + "'";
        getDB()->exec(cmdD);

        QString cmdI = "INSERT INTO sandBoxURL VALUES(?,?,?,?,?)";
        dInsert->prepare(cmdI);

        dInsert->bind(fileUrlV);
        dInsert->bind(docUrlV);

        int access = (readOnlyFlag) ? 1 : 0;
        dInsert->bind(access);

        common::TimeStamp accessTime = currentAccessTime();
        tuint64 accessTimeI = static_cast<tuint64>(accessTime);
        dInsert->bind(accessTimeI);

        QByteArray bookmarkArray(bkArray);
        dInsert->bind(bookmarkArray);

        if(dInsert->next())
        {
            getDB()->exec("RELEASE addBookmark");
            res = true;
        }
        else
        {
            getDB()->exec("ROLLBACK TO SAVEPOINT addBookmark");
        }
    }
    catch(const SQLiteException& e)
    {
        getDB()->exec("ROLLBACK TO SAVEPOINT addBookmark");
        res = false;
    }
    removeDBInsert(dInsert);
    return res;
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::updateAccessTime(const QString& keyRef,const QString& docRef)
{
    common::TimeStamp accessTime = common::TimeStamp::now();
    updateAccessTime(keyRef,docRef,accessTime);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::updateAccessTime(const QString& keyRef,const QString& docRef,const common::TimeStamp& accessTime)
{
    try
    {
        QString cmdU;
        tint64 accessTimeI = static_cast<tint64>(static_cast<tuint64>(accessTime));

        cmdU = "UPDATE sandBoxURL SET accessTime=" + QString::number(accessTimeI) + " WHERE url='" + TrackDB::dbString(keyRef) + "' AND docUrl='" + TrackDB::dbString(docRef) + "'";
        getDB()->exec("SAVEPOINT updateAccessTime");
        getDB()->exec(cmdU);
        getDB()->exec("RELEASE updateAccessTime");
    }
    catch(const SQLiteException& e)
    {
        getDB()->exec("ROLLBACK TO SAVEPOINT updateAccessTime");
    }
}

//-------------------------------------------------------------------------------------------

NSData *SandboxBookmarkService::securityBookmarkFromURL(const QString& fileName,NSURLBookmarkCreationOptions options)
{
    NSError *err = nil;
    NSURL *url = (NSURL *)toUrl(fileName);
    return [url bookmarkDataWithOptions:options includingResourceValuesForKeys:nil relativeToURL:nil error:&err];
}

//-------------------------------------------------------------------------------------------

NSData *SandboxBookmarkService::securityBookmarkFromURL(const QString& docFileName,const QString& refFileName,NSURLBookmarkCreationOptions options)
{
    NSError *err = nil;
    NSURL *docUrl = (NSURL *)toUrl(docFileName);
    NSURL *refUrl = (NSURL *)toUrl(refFileName);
    return [refUrl bookmarkDataWithOptions:options includingResourceValuesForKeys:nil relativeToURL:docUrl error:&err];
}

//-------------------------------------------------------------------------------------------

QByteArray SandboxBookmarkService::securityBookmark(const QString& fileName,bool readOnlyFlag)
{
    NSData *bkData;
    NSURLBookmarkCreationOptions bkOptions;
    QByteArray bkArray;

    bkOptions = NSURLBookmarkCreationWithSecurityScope;
    if(readOnlyFlag)
    {
        bkOptions |= NSURLBookmarkCreationSecurityScopeAllowOnlyReadAccess;
    }

    bkData = securityBookmarkFromURL(fileName,bkOptions);
    if(bkData != nil)
    {
        const char *bkMem = (const char *)[bkData bytes];
        int bkLen = [bkData length];
        bkArray = QByteArray::fromRawData(bkMem,bkLen);
    }
    return bkArray;
}

//-------------------------------------------------------------------------------------------

QByteArray SandboxBookmarkService::securityBookmark(const QString& docFileName,const QString& refFileName,bool readOnlyFlag)
{
    NSData *bkData;
    NSURLBookmarkCreationOptions bkOptions;
    QByteArray bkArray;

    bkOptions = NSURLBookmarkCreationWithSecurityScope;
    if(readOnlyFlag)
    {
        bkOptions |= NSURLBookmarkCreationSecurityScopeAllowOnlyReadAccess;
    }

    bkData = securityBookmarkFromURL(docFileName,refFileName,bkOptions);
    if(bkData != nil)
    {
        const char *bkMem = (const char *)[bkData bytes];
        int bkLen = [bkData length];
        bkArray = QByteArray::fromRawData(bkMem,bkLen);
    }
    return bkArray;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::addRootDocument(const QString& fileName,bool readOnlyFlag)
{
    return add(fileName,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::add(const QString& fName,bool readOnlyFlag)
{
    QString fileName = formatFileName(fName);
    bool res = false;

    if(!fileName.isEmpty())
    {
        if(!has(fileName,readOnlyFlag))
        {
            QByteArray bkArray = securityBookmark(fileName,readOnlyFlag);
            if(!bkArray.isEmpty())
            {
                QString fUrl = key(fileName);
                QString dUrl = "";

                res = addToDB(fUrl,dUrl,readOnlyFlag,bkArray);
            }
        }
        else
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::add(const QString& dFileName,const QStringList& refFileNames,bool readOnlyFlag)
{
    QString docFileName = formatFileName(dFileName);
    bool res = true;

    if(!docFileName.isEmpty())
    {
        if(addRootDocument(docFileName,readOnlyFlag))
        {
            QString docUrl = key(docFileName);

            for(QStringList::const_iterator ppI=refFileNames.begin();ppI!=refFileNames.end() && res;ppI++)
            {
                const QString refFileName = formatFileName(*ppI);

                if(!has(docFileName,refFileName,readOnlyFlag))
                {
                    QByteArray bkArray = securityBookmark(docFileName,refFileName,readOnlyFlag);
                    if(!bkArray.isEmpty())
                    {
                        QString refUrl = key(refFileName);
                        res = addToDB(refUrl,docUrl,readOnlyFlag,bkArray);
                    }
                    else
                    {
                        res = false;
                    }
                }
            }
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    return res;
}


//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::getAccessMap(const QString& fileName,QMap<QString,QPair<bool,common::TimeStamp> >& chMap)
{
    bool readOnlyFlag;
    tint64 accessTimeI;
    QString docRef,refKey,cmdQ;
    SQLiteQuery chkQ(getDB());
    common::TimeStamp accessTime;

    refKey = key(fileName);

    cmdQ = "SELECT docUrl,access,accessTime FROM sandBoxURL WHERE url='" + TrackDB::dbString(refKey) + "'";
    chkQ.prepare(cmdQ);
    chkQ.bind(docRef);
    chkQ.bind(readOnlyFlag);
    chkQ.bind(accessTimeI);
    while(chkQ.next())
    {
        docRef = TrackDB::dbStringInv(docRef);
        accessTime = static_cast<tuint64>(accessTimeI);
        chMap.insert(docRef,QPair<bool,common::TimeStamp>(readOnlyFlag,accessTime));
    }
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::canAccess(bool readOnlyFlag,bool access) const
{
    return (!readOnlyFlag && access) ? false : true;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::accessDocumentUrlForFile(const QString& fileName,bool readOnlyFlag,bool checkInFlag,QString& docFileName)
{
    QMap<QString,QPair<bool,common::TimeStamp> > accessMap;
    bool res = false;

    getAccessMap(fileName,accessMap);
    if(!accessMap.isEmpty())
    {
        QString emptyS;
        QMap<QString,QPair<bool,common::TimeStamp> >::iterator ppI;

        ppI = accessMap.find(emptyS);
        if(ppI!=accessMap.end())
        {
            if(canAccess(readOnlyFlag,ppI.value().first))
            {
                if(!checkInFlag || canCheckIn(fileName,emptyS))
                {
                    docFileName = emptyS;
                    res = true;
                }
            }
        }

        if(!res)
        {
            QString docRef;
            common::TimeStamp maxT;

            for(ppI=accessMap.begin();ppI!=accessMap.end();ppI++)
            {
                if(!ppI.key().isEmpty() && canAccess(readOnlyFlag,ppI.value().first) && ppI.value().second > maxT)
                {
                    QString dName = pathFromKey(ppI.key());
                    if(!checkInFlag || canCheckIn(fileName,dName))
                    {
                        docFileName = dName;
                        maxT = ppI.value().second;
                        res = true;
                    }
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QMap<QString,QMap<QString,QPair<void *,int> > >& SandboxBookmarkService::getReferenceCountMap()
{
    return m_referenceCountMap;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::isBookmarkCreationRequired(const QString& fileName,const QString& docFileName)
{
    QString fileUrl,docUrl;
    QMap<QString,QMap<QString,QPair<void *,int> > >::iterator ppI;
    bool res = true;

    fileUrl = key(fileName);
    if(!docFileName.isEmpty())
    {
        docUrl = key(docFileName);
    }

    ppI = getReferenceCountMap().find(fileUrl);
    if(ppI!=getReferenceCountMap().end())
    {
        QMap<QString,QPair<void *,int> >::iterator ppJ;
        QMap<QString,QPair<void *,int> >& rMap = ppI.value();

        ppJ = rMap.find(docUrl);
        if(ppJ!=rMap.end())
        {
            ppJ.value().second++;
            res = false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

NSData *SandboxBookmarkService::getBookmarkFromReference(const QString& fileName,const QString& docFileName)
{
    QString fileUrl,docUrl,cmdQ;
    NSData *bookmarkData = nil;
    SQLiteQuery *pBookmarkQuery = createDBQuery();

    try
    {
        QString cmdQ;
        QByteArray bkArray;

        fileUrl = key(fileName);
        docUrl = key(docFileName);

        cmdQ = "SELECT bookmark FROM sandBoxURL WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";

        pBookmarkQuery->prepare(cmdQ);
        pBookmarkQuery->bind(bkArray);
        if(pBookmarkQuery->next())
        {
            bookmarkData = [NSData dataWithBytes:(bkArray.data()) length:(bkArray.size())];
            updateAccessTime(fileUrl,docUrl);
        }
    }
    catch(const SQLiteException& e)
    {
        bookmarkData = nil;
    }
    removeDBQuery(pBookmarkQuery);
    return bookmarkData;
}

//-------------------------------------------------------------------------------------------

NSURL *SandboxBookmarkService::resolveSecurityScopedBookmarkData(NSData *bkData,const QString& docFileName)
{
    BOOL staleFlag;
    NSError *err = nil;
    NSURL *docUrl = toUrl(docFileName);
    return [NSURL URLByResolvingBookmarkData:bkData options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:docUrl bookmarkDataIsStale:&staleFlag error:&err];
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::startAccessingSecuredResource(NSURL *url)
{
    return ([url startAccessingSecurityScopedResource]) ? true : false;
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::addSecuredResourceToReferenceMap(const QString& fileName,const QString& docFileName,NSURL *bookmark)
{
    QString fileUrl = key(fileName);
    QString docUrl = key(docFileName);
    QMap<QString,QMap<QString,QPair<void *,int> > >::iterator ppI;

    ppI = getReferenceCountMap().find(fileUrl);
    if(ppI==getReferenceCountMap().end())
    {
        QMap<QString,QPair<void *,int> > fileReferenceMap;
        getReferenceCountMap().insert(fileUrl,fileReferenceMap);
        ppI = getReferenceCountMap().find(fileUrl);
    }

    QMap<QString,QPair<void *,int> >& rMap = ppI.value();
    rMap.insert(docUrl,QPair<void *,int>((void *)bookmark,1));

    [bookmark retain];
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::checkOutSecuredResource(const QString& fileName,const QString& docFileName,NSData *bookmarkData)
{
    NSURL *bookmarkUrl;
    bool res = false;

    bookmarkUrl = resolveSecurityScopedBookmarkData(bookmarkData,docFileName);
    if(bookmarkUrl!=nil)
    {
        if(startAccessingSecuredResource(bookmarkUrl))
        {
            addSecuredResourceToReferenceMap(fileName,docFileName,bookmarkUrl);
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::deleteBookmark(const QString& fileName,const QString& docFileName)
{
    QString cmdD;
    QString fileUrl = key(fileName);
    QString docUrl = key(docFileName);

    try
    {
        getDB()->exec("SAVEPOINT deleteBookmark");
        cmdD = "DELETE FROM sandBoxUrl WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";
        getDB()->exec(cmdD);
        getDB()->exec("RELEASE deleteBookmark");
    }
    catch(const SQLiteException& e)
    {
        getDB()->exec("ROLLBACK TO SAVEPOINT deleteBookmark");
    }
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::checkOutBookmarkResource(const QString& fileName,const QString& docFileName)
{
    bool res;

    NSData *bkData = getBookmarkFromReference(fileName,docFileName);
    if(bkData!=nil)
    {
        res = checkOutSecuredResource(fileName,docFileName,bkData);
    }
    else
    {
        res = false;
    }

    if(!res)
    {
        deleteBookmark(fileName,docFileName);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::isInDirectory(const QString& fileName,const QString& dirName) const
{
    QString path,dName;
    int level = 0;
    common::CommonDirectoriesForFiles commonDir;
    bool res = false;

    dName = commonDir.path(dirName,0);
    do
    {
        path = commonDir.path(fileName,level);
        if(!path.isEmpty() && path==dName)
        {
            res = true;
        }
        level++;
    } while(!path.isEmpty() && !res);

    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::isInAppContainer(const QString& fileName) const
{
#if defined(OMEGA_MAC_STORE) && !defined(OMEGA_STORE_DEBUG_BUILD)
    bool res = false;
    NSString *homeDir = NSHomeDirectory();

    if(homeDir!=nil)
    {
        QString hDirName = QString::fromUtf8([homeDir UTF8String]);
        res =  isInDirectory(fileName,hDirName);
    }
    if(!res)
    {
        res = isInDirectory(fileName,common::SBService::tempDirectory());
    }
    return res;
#else
    return false;
#endif
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::isBookmarkRequired(const QString& fileName,bool readOnlyFlag)
{
    bool res;

    if(isInAppContainer(fileName))
    {
        res = false;
    }
    else
    {
        if(isMusicDirectory(fileName) && !isPlaylist(fileName) && readOnlyFlag)
        {
            res = false;
        }
        else
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::checkOutRecursive(const QString& fileName,bool readOnlyFlag)
{
    return checkOut(fileName,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::checkOutParentDirectory(const QString& fileName,bool readOnlyFlag)
{
    int level = 1;
    QString dirName;
    common::CommonDirectoriesForFiles commonDir;
    bool res = false;

    do
    {
        dirName = commonDir.path(fileName,level);
        if(!dirName.isEmpty())
        {
            res = hasURL(key(dirName),readOnlyFlag);
        }
        level++;
    } while(!dirName.isEmpty() && !res);

    if(res)
    {
        res = checkOutRecursive(dirName,readOnlyFlag);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::checkOut(const QString& fName,bool readOnlyFlag)
{
    QString fileName = formatFileName(fName);
    QString docFileName;
    bool res;

    if(isBookmarkRequired(fileName,readOnlyFlag))
    {
        res = accessDocumentUrlForFile(fileName,readOnlyFlag,false,docFileName);
        if(res)
        {
            bool insertFlag;

            lock();

            insertFlag = isBookmarkCreationRequired(fileName,docFileName);
            if(!docFileName.isEmpty())
            {
                if(!checkOutRecursive(docFileName,readOnlyFlag))
                {
                    res = false;
                }
            }

            if(res && insertFlag)
            {
                res = checkOutBookmarkResource(fileName,docFileName);
            }

            unlock();
        }
        else
        {
            res = checkOutParentDirectory(fileName,readOnlyFlag);
        }
    }
    else
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::checkInDocumentAsRequired(const QString& docFileName,bool readOnlyFlag)
{
    bool res = true;

    if(!docFileName.isEmpty())
    {
        res = checkIn(docFileName,readOnlyFlag);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::stopScopedResource(NSURL *url)
{
    [url stopAccessingSecurityScopedResource];
    [url release];
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::doCheckIn(const QString& fileName,const QString& docFileName,bool readOnlyFlag)
{
    QString fileUrl,docUrl;
    QMap<QString,QMap<QString,QPair<void *,int> > >::iterator ppI;
    bool res;

    fileUrl = key(fileName);
    docUrl  = key(docFileName);

    ppI = getReferenceCountMap().find(fileUrl);
    if(ppI != getReferenceCountMap().end())
    {
        QMap<QString,QPair<void *,int> >::iterator ppJ;
        QMap<QString,QPair<void *,int> >& rMap = ppI.value();
        ppJ=rMap.find(docUrl);
        if(ppJ!=rMap.end())
        {
            QPair<void *,int>& rPair = ppJ.value();

            rPair.second--;
            if(rPair.second<=0)
            {
                NSURL *url = (NSURL *)rPair.first;
                stopScopedResource(url);
                rMap.erase(ppJ);
                if(rMap.isEmpty())
                {
                    getReferenceCountMap().erase(ppI);
                }
            }
            res = true;
        }
        else
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }
    if(res)
    {
        res = checkInDocumentAsRequired(docFileName,readOnlyFlag);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::canCheckIn(const QString& fileName,const QString& docFileName)
{
    QString fileUrl,docUrl;
    QMap<QString,QMap<QString,QPair<void *,int> > >::iterator ppI;
    bool res = false;

    fileUrl = key(fileName);
    docUrl  = key(docFileName);

    ppI = getReferenceCountMap().find(fileUrl);
    if(ppI!=getReferenceCountMap().end())
    {
        QMap<QString,QPair<void *,int> >::iterator ppJ;
        QMap<QString,QPair<void *,int> >& rMap = ppI.value();

        ppJ = rMap.find(docUrl);
        if(ppJ!=rMap.end())
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::checkInParentDirectory(const QString& fileName,bool readOnlyFlag)
{
    int level = 1;
    QString dirName,empty;
    common::CommonDirectoriesForFiles commonDir;
    bool res = false;

    do
    {
        dirName = commonDir.path(fileName,level);
        if(!dirName.isEmpty())
        {
            if(hasURL(key(dirName),readOnlyFlag) && canCheckIn(dirName,empty))
            {
                res = true;
            }
        }
        level++;
    } while(!dirName.isEmpty() && !res);

    if(res)
    {
        res = checkIn(dirName,readOnlyFlag);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::checkIn(const QString& fName,bool readOnlyFlag)
{
    QString fileName = formatFileName(fName);
    bool res = false;

    if(isBookmarkRequired(fileName,readOnlyFlag))
    {
        QString docFileName;

        lock();
        if(accessDocumentUrlForFile(fileName,readOnlyFlag,true,docFileName))
        {
            if(doCheckIn(fileName,docFileName,readOnlyFlag))
            {
                res = true;
            }
        }
        else
        {
            res = checkInParentDirectory(fileName,readOnlyFlag);
        }
        unlock();
    }
    else
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QByteArray SandboxBookmarkService::getBookmarkArray(const QString& docFileName,const QString& refFileName)
{
    QString fileUrl,docUrl,cmdQ;
    QByteArray bookmarkArray;
    SQLiteQuery *pBookmarkQuery = createDBQuery();

    try
    {
        QString cmdQ;
        QByteArray bkArray;

        fileUrl = key(refFileName);
        docUrl = key(docFileName);

        cmdQ = "SELECT bookmark FROM sandBoxURL WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";

        pBookmarkQuery->prepare(cmdQ);
        pBookmarkQuery->bind(bkArray);
        if(pBookmarkQuery->next())
        {
            bookmarkArray = bkArray;
        }
    }
    catch(const SQLiteException& e)
    {
        bookmarkArray.clear();
    }
    removeDBQuery(pBookmarkQuery);
    return bookmarkArray;
}

//-------------------------------------------------------------------------------------------

QString SandboxBookmarkService::userErrorMessage(track::info::UserSandboxErrorMessage err)
{
    QString errMsg;

    switch(err)
    {
        case track::info::e_cannotLoadPlaylist:
            errMsg  = "Sorry, M3U and PLS playlists outside of your Music folder cannot be loaded due to Apple's sandbox security restrictions. ";
            errMsg += "Please save and load your playlists as xspf files.";
            break;
    }
    return errMsg;
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkService::shutdown()
{
    QMap<QString,QMap<QString,QPair<void *,int> > >::iterator ppI;

    while(ppI = getReferenceCountMap().begin(), ppI!=getReferenceCountMap().end())
    {
        QMap<QString,QPair<void *,int> >::iterator ppJ;
        QMap<QString,QPair<void *,int> >& rMap = ppI.value();

        while(ppJ=rMap.begin(), ppJ!=rMap.end())
        {
            NSURL *url = (NSURL *)ppJ.value().first;
            rMap.erase(ppJ);
            [url release];
        }
        getReferenceCountMap().erase(ppI);
    }
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkService::add(const QString& dFileName,const QString& rFileName,bool readOnlyFlag,const QByteArray& bkArray)
{
    QString docFileName = formatFileName(dFileName);
    QString refFileName = formatFileName(rFileName);
    bool res = false;

    if(!docFileName.isEmpty() && !refFileName.isEmpty() && bkArray.size()>0)
    {
        if(has(docFileName,refFileName,readOnlyFlag))
        {
            return true;
        }

        if(add(docFileName,false))
        {
            NSURL *url;
            NSData *bkData = [NSData dataWithBytes:(const void *)(bkArray.data()) length:(bkArray.size())];

            url = resolveSecurityScopedBookmarkData(bkData,docFileName);
            if(url!=nil)
            {
                QString docUrl = key(docFileName);
                QString fileUrl = key(refFileName);

                if(addToDB(fileUrl,docUrl,readOnlyFlag,bkArray))
                {
                    res = true;
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QString SandboxBookmarkService::formatFileName(const QString& fileName) const
{
    QString fName;

    if(!fileName.isEmpty())
    {
        if(fileName.at(fileName.length()-1)==QChar('/') || fileName.at(fileName.length()-1)==QChar('\\'))
        {
            fName = fileName.mid(0,fileName.length()-1);
        }
        else
        {
            fName = fileName;
        }
    }
    return fName;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
