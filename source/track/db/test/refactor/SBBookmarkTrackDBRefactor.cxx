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


class TRACK_DB_EXPORT SBBookmarkService
{
    public:
        SBBookmarkService();
        virtual ~SBBookmarkService();

        static QString getHomeDirectory();

        bool has(const QString& fileName);
        bool has(const QString& docFilename,const QString& refFilename);
        bool hasAny(const QString& fileName);

        bool add(const QString& fileName,bool readOnlyFlag);
        bool add(const QString& fileName,const QString& docFiles,bool readOnlyFlag);
        bool add(const QString& fileName,const QStringList& docFiles,bool readOnlyFlag);
        bool add(const QString& docFile,const QString& fileName,const QByteArray& bkArray);

        bool checkOut(const QString& fileName);
        bool checkIn(const QString& fileName);

        QString userErrorMessage(UserSandboxErrorMessage err);

        bool isPlaylist(const QString& fileName);
        bool isMusicDirectory(const QString& fileName);

        QByteArray getBookmarkArray(const QString& docFile,const QString& refFile);

    protected:

        SQLiteDatabase *m_db;

        QMutex m_mutex;
        QMap<QString,QMap<QString,QPair<void *,int> > > m_refCountMap;
        QMap<QString,QSet<QString> > m_docMap;

        void printError(const char *strR,const char *strE) const;
        void printError(const char *strR,void *err) const;

        QString key(const QString& fileName);
        QString key(void *fileUrl);
        QString pathFromKey(const QString& k);
        void *toUrl(const QString& fileName);
        bool hasURL(const QString& key);
        bool hasURL(const QString& docKey,const QString& refKey);
        bool addToDB(void *bkData,const QString& kRef,const QString& dRef,bool readOnlyFlag);
        void updateAccessTime(const QString& keyRef,const QString& docRef);
        void getAccessMap(const QString& fileName,QMap<QString,QPair<int,common::TimeStamp> >& chMap);
        bool canCheckIn(const QString& refKey,const QString& docKey);
        bool doCheckIn(const QString& refKey,const QString& docKey);
        void shutdown();
};

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "track/db/inc/SBBookmarkTrackDB.h"
#include "common/inc/TimeStamp.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

SBBookmarkService::SBBookmarkService() : m_mutex(QMutex::Recursive),
    m_refCountMap(),
    m_docMap()
{
    m_db = TrackDB::instance()->db();
}

//-------------------------------------------------------------------------------------------

SBBookmarkService::~SBBookmarkService()
{
    shutdown();
}

//-------------------------------------------------------------------------------------------

void SBBookmarkService::printError(const char *strR,const char *strE) const
{
    common::Log::g_Log << "SBBookmarkService::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void SBBookmarkService::printError(const char *strR,void *err) const
{
    NSError *e = (NSError *)err;
    NSString *lDesc = [e localizedDescription];
    NSString *lReason = [e localizedFailureReason];
    QString eDesc,eReason;

    if(lDesc!=nil)
    {
        const char *x = [lDesc UTF8String];
        eDesc = QString::fromUtf8(x);
    }
    if(lReason!=nil)
    {
        const char *x = [lReason UTF8String];
        eReason = QString::fromUtf8(x);
    }
    eDesc += " " + eReason;
    printError(strR,eDesc.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

QString SBBookmarkService::key(const QString& fileName)
{
    return key(toUrl(fileName));
}

//-------------------------------------------------------------------------------------------

QString SBBookmarkService::key(void *fileUrl)
{
    NSURL *u = (NSURL *)fileUrl;
    const char *x = [[u absoluteString] UTF8String];
    QString uKey = QString::fromUtf8(x);
    return uKey;
}

//-------------------------------------------------------------------------------------------

QString SBBookmarkService::pathFromKey(const QString& k)
{
    QString path;
    const char *x = k.toUtf8().constData();
    NSURL *u = [NSURL URLWithString:[NSString stringWithUTF8String:x]];
    NSString *nS = [u path];
    if(nS!=nil)
    {
        const char *y = [nS UTF8String];
        path = QString::fromUtf8(y);
    }
    return path;
}

TEST(SBBookmarkService,pathFromKeyThatExists)
{
    QString trackDBName = "file://" + TrackDBTestEnviroment::instance()->getDBDirectory() + "/track.db";
    QString expectName = TrackDBTestEnviroment::instance()->getDBDirectory() + "/track.db";

    SBBookmarkService svr;
    QString path = svr.pathFromKey(trackDBName);

    EXPECT_TRUE(path==expectName);
}

TEST(SBBookmarkService,pathFromKeyThatDoesNotExist)
{
    QString trackDBName = "file://" + TrackDBTestEnviroment::instance()->getDBDirectory() + "/track_not_exist.db";

    SBBookmarkService svr;
    QString path = svr.pathFromKey(trackDBName);

    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

void *SBBookmarkService::toUrl(const QString& fileName)
{
    const char *x = fileName.toUtf8().constData();
    NSString *nS = [NSString stringWithUTF8String:x];
    NSURL *u = [NSURL fileURLWithPath:nS];
    return (void *)u;
}

TEST(SBBookmarkService,toUrl)
{
    SBBookmarkService svr;
    void *pUPtr = svr.toUrl(QString::fromLatin1("/path/to/web_root/folder/file.html"));
    EXPECT_TRUE(pUPtr!=0);
    NSURL *url = (NSURL *)pUPtr;
    EXPECT_TRUE(url!=nil);
    EXPECT_TRUE(svr.key(pUPtr)=="file:///path/to/web_root/folder/file.html");
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::has(const QString& fileName)
{
    if(isMusicDirectory(fileName) && !isPlaylist(fileName))
    {
        return true;
    }
    return hasURL(key(fileName));
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::hasURL(const QString& key)
{
    QString cmdQ,uStr;
    SQLiteQuery uKeyQ(m_db);
    bool res;

    cmdQ = "SELECT url FROM sandBoxURL WHERE url='" + TrackDB::dbString(key) + "' AND docUrl=''";
    uKeyQ.prepare(cmdQ);
    uKeyQ.bind(uStr);
    if(uKeyQ.next())
    {
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

void SBBookmarkServiceTestInsertURL(const QString& url)
{
    QString emptyDoc;
    SBBookmarkServiceTestInsertURL(url,emptyDoc);
}

void SBBookmarkServiceTestInsertURL(const QString& url,const QString& docUrl)
{
    SQLiteDatabase *db = TrackDB::instance()->db();

    QString bkData("Bookmark test data");
    QByteArray bkArray(bkData.toUtf8().constData(),bkData.toUtf8().constSize());
    int access = 1;
    common::TimeStamp accessTime = common::TimeStamp::now();
    tuint64 accessTimeI = static_cast<tuint64>(accessTime);

    QString cmdI;
    SQLiteInsert bkIns(db);

    QString kRefV = TrackDB::dbString(url);
    QString dRefV = TrackDB::dbString(docUrl);

    cmdI = "INSERT INTO sandBoxURL VALUES(?,?,?,?,?)";
    bkIns.prepare(cmdI);
    bkIns.bind(kRefV);
    bkIns.bind(dRefV);
    bkIns.bind(access);
    bkIns.bind(accessTimeI);
    bkIns.bind(bkArray);

    bkIns.next();
}

void SBBookmarkServiceTestDeleteURL(const QString& url)
{
    QString emptyDoc;
    SBBookmarkServiceTestDeleteURL(url,emptyDoc);
}

void SBBookmarkServiceTestDeleteURL(const QString& url,const QString& docUrl)
{
    QString cmdD;
    QString kRefV = TrackDB::dbString(url);
    QString dRefV = TrackDB::dbString(docUrl);

    cmdD = "DELETE FROM sandBoxURL WHERE url='" + kRefV + "' AND docUrl='" + dRefV + "'";
    Track::instance()->db()->exec(cmdD);
}

TEST(SBBookmarkService,hasURLWhenURLExists)
{
    SBBookmarkServiceTestInsertURL("file:://path/to/music.mp3");

    SBBookmarkService svr;
    EXPECT_TRUE(svr.hasURL("file:://path/to/music.mp3"));

    SBBookmarkServiceTestDeleteURL("file:://path/to/music.mp3");
}

TEST(SBBookmarkService,hasURLWhenURLDoesNotExist)
{
    SBBookmarkService svr;
    EXPECT_FALSE(svr.hasURL("file:://path/to/music.mp3"));
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::has(const QString& docFilename,const QString& refFilename)
{
    if(isMusicDirectory(refFilename) && !isPlaylist(refFilename))
    {
        return true;
    }
    return hasURL(key(docFilename),key(refFilename));
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::hasURL(const QString& docKey,const QString& refKey)
{
    QString cmdQ,uStr;
    SQLiteQuery uKeyQ(m_db);
    bool res;

    cmdQ = "SELECT url FROM sandBoxURL WHERE url='" + TrackDB::dbString(refKey) + "' AND docUrl='" + TrackDB::dbString(docKey) + "'";
    uKeyQ.prepare(cmdQ);
    uKeyQ.bind(uStr);
    if(uKeyQ.next())
    {
        res = true;
    }
    else
    {
        res = false;
    }
    return res;
}

TEST(SBBookmarkService,hasURLWithDocumentWhenURLExists)
{
    SBBookmarkServiceTestInsertURL("file:://path/to/playlist.pls","file:://path/to/music.mp3");

    SBBookmarkService svr;
    EXPECT_TRUE(svr.hasURL("file:://path/to/playlist.pls","file:://path/to/music.mp3"));

    SBBookmarkServiceTestDeleteURL("file:://path/to/playlist.pls","file:://path/to/music.mp3");
}

TEST(SBBookmarkService,hasURLWithDocumentWhenURLDoesNotExist)
{
    SBBookmarkService svr;
    EXPECT_FALSE(svr.hasURL("file:://path/to/playlist.pls","file:://path/to/music.mp3"));
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::hasAny(const QString& fileName)
{
    QString refKey = key(fileName);

    if(isMusicDirectory(fileName) && !isPlaylist(fileName))
    {
        return true;
    }
    return hasURL(refKey);
}

TEST(SBBookmarkService,hasAnyGivenMusicDirectoryAndNotPlaylist)
{
    QString fileName = SBBookmarkService::getHomeDirectory() + "/Music/iTunes/file.mp3";

    SBBookmarkService svr;
    EXPECT_TRUE(svr.hasAny(fileName));
}

TEST(SBBookmarkService,hasAnyGivenMusicDirectoryAndPlaylistWithNoBookmark)
{
    QString fileName = SBBookmarkService::getHomeDirectory() + "/Music/iTunes/file.pls";

    SBBookmarkService svr;
    EXPECT_FALSE(svr.hasAny(fileName));
}

TEST(SBBookmarkService,hasAnyGivenMusicDirectoryAndPlaylistWithBookmark)
{
    SBBookmarkService svr;

    QString fileName = SBBookmarkService::getHomeDirectory() + "/Music/iTunes/file.pls";
    QString fileKey = svr.key(fileName);

    SBBookmarkServiceTestInsertURL(filekey);

    EXPECT_TRUE(svr.hasAny(fileName));

    SBBookmarkServiceTestDeleteURL(filekey);
}

TEST(SBBookmarkService,hasAnyGivenNotMusicDirectoryAndNoBookmark)
{
    SBBookmarkService svr;
    QString fileName = "/Music/iTunes/file.pls";
    EXPECT_FALSE(svr.hasAny(fileName));
}

TEST(SBBookmarkService,hasAnyGivenNotMusicDirectoryAndBookmark)
{
    SBBookmarkService svr;

    QString fileName = "/Music/iTunes/file.pls";
    QString fileKey = svr.key(fileName);

    SBBookmarkServiceTestInsertURL(filekey);

    EXPECT_TRUE(svr.hasAny(fileName));

    SBBookmarkServiceTestDeleteURL(filekey);
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::addToDB(void *bkData,const QString& kRef,const QString& dRef,bool readOnlyFlag)
{
    NSData *bkD = (NSData *)bkData;
    QByteArray bkArray((const char *)([bkD bytes]),(int)([bkD length]));
    QString kRefV;
    QString dRefV;
    int access = (readOnlyFlag) ? 1 : 0;
    QString cmdI;
    SQLiteInsert bkIns(m_db);
    bool res = false;

    try
    {
        common::TimeStamp accessTime = common::TimeStamp::now();
        tuint64 accessTimeI = static_cast<tuint64>(accessTime);

        m_db->exec("SAVEPOINT addBookmark");

        kRefV = TrackDB::dbString(kRef);
        dRefV = TrackDB::dbString(dRef);

        cmdI = "INSERT INTO sandBoxURL VALUES(?,?,?,?,?)";
        bkIns.prepare(cmdI);
        bkIns.bind(kRefV);
        bkIns.bind(dRefV);
        bkIns.bind(access);
        bkIns.bind(accessTimeI);
        bkIns.bind(bkArray);
        if(bkIns.next())
        {
            m_db->exec("RELEASE addBookmark");
            res = true;
        }
        else
        {
            printError("addToDB","Error adding bookmark to database");
            m_db->exec("ROLLBACK TO SAVEPOINT addBookmark");
            res = false;
        }
    }
    catch(const SQLiteException& e)
    {
        QString errStr;
        m_db->exec("ROLLBACK TO SAVEPOINT addBookmark");
        errStr = "Database exception thrown. " + e.error();
        printError("addToDB",errStr.toUtf8().constData());
        res = false;
    }
    return res;
}

SQLiteDatabase *SBBookmarkService::getDB()
{
    return m_db;
}

SQLiteInsert *SBBookmarkService::createDBInsert()
{
    return new SQLiteInsert(getDB());
}

void SBBookmarkService::removeDBInsert(SQLiteInsert *pInsert)
{
    delete pInsert;
}

bool SBBookmarkService::addToDB(void *bkData,const QString& kRef,const QString& dRef,bool readOnlyFlag)
{
    bool res = false;

    try
    {
        SQLiteInsert *ins = createDBInsert();

        getDB()->exec("SAVEPOINT addBookmark");

        removeDBInsert(ins);
    }
    catch(const SQLiteException& e)
    {

    }
    return res;
}

class SBBookmarkServiceAddToDBTest : SBBookmarkServiceTest
{
    SQLiteDatabase *getDB();
    SQLiteInsert *createDBInsert();
    void removeDBInsert(SQLiteInsert *pInsert);
};

TEST(SBBookmarkService,addToDB)
{
    SQLiteDatabaseMock db;
    EXPECT_CALL(db,exec(Eq("SAVEPOINT addBookmark"))).Times(1);

    SQLiteInsertMock sInsert;

    SBBookmarkServiceAddToDBTest svr;
    EXPECT_CALL(svr,getDB()).WillRepeatedly(Return(&db));
    EXPECT_CALL(svr,createDBInsert()).Times(1).WillOnce(Return(&sInsert));
}

//-------------------------------------------------------------------------------------------

void SBBookmarkService::updateAccessTime(const QString& keyRef,const QString& docRef)
{
    try
    {
        QString cmdU;
        common::TimeStamp accessTime = common::TimeStamp::now();
        tuint64 accessTimeI = static_cast<tuint64>(accessTime);

        cmdU = "UPDATE sandBoxURL SET accessTime=" + QString::number(accessTimeI) + " WHERE url='" + TrackDB::dbString(keyRef) + "' AND docUrl='" + TrackDB::dbString(docRef) + "'";
        m_db->exec("SAVEPOINT updateAccessTime");
        m_db->exec(cmdU);
        m_db->exec("RELEASE updateAccessTime");
    }
    catch(const SQLiteException& e)
    {
        m_db->exec("ROLLBACK TO SAVEPOINT updateAccessTime");
    }
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::add(const QString& fileName,bool readOnlyFlag)
{
    bool res;

    if(!has(fileName))
    {
        NSURL *u = (NSURL *)toUrl(fileName);
        NSError *err = nil;
        NSData *bkData;
        NSURLBookmarkCreationOptions bkOptions;

        bkOptions = NSURLBookmarkCreationWithSecurityScope;
        if(readOnlyFlag)
        {
            bkOptions |= NSURLBookmarkCreationSecurityScopeAllowOnlyReadAccess;
        }

        bkData = [u bookmarkDataWithOptions:bkOptions includingResourceValuesForKeys:nil relativeToURL:nil error:&err];
        if(bkData!=nil)
        {
            QString kRef,dRef;

            const char *bkMem = (const char *)[bkData bytes];
            int bkLen = [bkData length];

            kRef = key(fileName);
            dRef = "";

            if(addToDB((void *)bkData,kRef,dRef,readOnlyFlag))
            {
                res  = true;
            }
            else
            {
                res = false;
            }
        }
        else
        {
            if(err!=nil)
            {
                printError("add",(void *)err);
            }
            else
            {
                printError("add","Failed to create security app scoped bookmark");
            }
            res = false;
        }
    }
    else
    {
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::add(const QString& fileName,const QString& docFiles,bool readOnlyFlag)
{
    QStringList dList;
    dList.append(docFiles);
    return add(fileName,dList,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::add(const QString& fileName,const QStringList& docFiles,bool readOnlyFlag)
{
    bool res = true;

    if(!fileName.isEmpty())
    {
        QStringList::const_iterator ppI;

        if(add(fileName,readOnlyFlag))
        {
            NSURL *u = (NSURL *)toUrl(fileName);

            for(ppI=docFiles.begin();ppI!=docFiles.end() && res;ppI++)
            {
                const QString& dFile = *ppI;

                if(!has(fileName,dFile))
                {
                    NSURL *rU = (NSURL *)toUrl(dFile);
                    NSError *err = nil;
                    NSData *bkData;
                    NSURLBookmarkCreationOptions bkOptions;

                    bkOptions = NSURLBookmarkCreationWithSecurityScope;
                    if(readOnlyFlag)
                    {
                        bkOptions |= NSURLBookmarkCreationSecurityScopeAllowOnlyReadAccess;
                    }

                    bkData = [rU bookmarkDataWithOptions:bkOptions includingResourceValuesForKeys:nil relativeToURL:u error:&err];
                    if(bkData!=nil)
                    {
                        QString kRef,dRef;

                        const char *bkMem = (const char *)[bkData bytes];
                        int bkLen = [bkData length];

                        kRef = key(dFile);
                        dRef = key(fileName);

                        if(addToDB((void *)bkData,kRef,dRef,readOnlyFlag))
                        {
                            res  = true;
                        }
                        else
                        {
                            res = false;
                        }
                    }
                    else
                    {
                        if(err!=nil)
                        {
                            printError("add",(void *)err);
                        }
                        else
                        {
                            printError("add","Failed to create security document scoped bookmark");
                        }
                        res = false;
                    }
                }
            }
        }
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SBBookmarkService::getAccessMap(const QString& fileName,QMap<QString,QPair<int,common::TimeStamp> >& chMap)
{
    int access;
    tuint64 accessTimeI;
    QString docRef,cmdQ;
    QString refKey = key(fileName);
    SQLiteQuery chkQ(m_db);
    common::TimeStamp accessTime;
    bool res;

    cmdQ = "SELECT docUrl,access,accessTime FROM sandBoxURL WHERE url='" + TrackDB::dbString(refKey) + "'";
    chkQ.prepare(cmdQ);
    chkQ.bind(docRef);
    chkQ.bind(access);
    chkQ.bind(accessTimeI);
    while(chkQ.next())
    {
        docRef = TrackDB::dbStringInv(docRef);
        accessTime = accessTimeI;
        chMap.insert(docRef,QPair<int,common::TimeStamp>(access,accessTime));
    }
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::checkOut(const QString& fileName)
{
    QString refKey,docRef;
    QMap<QString,QPair<int,common::TimeStamp> > chMap;
    bool res;

    if(isMusicDirectory(fileName) && !isPlaylist(fileName))
    {
        return true;
    }

    refKey = key(fileName);
    getAccessMap(fileName,chMap);

    if(!chMap.isEmpty())
    {
        QString emptyS("");
        QMap<QString,QPair<int,common::TimeStamp> >::iterator ppI;
        QMap<QString,QMap<QString,QPair<void *,int> > >::iterator ppJ;  // QMap<fileURLKey,QMap<docURLKey,QPair<NSURL of bookmark,Reference Counter>>>
        QMap<QString,QPair<void *,int> >::iterator ppK;
        bool insertFlag = false;

        m_mutex.lock();

        // 1. Find if the file is associated to document(s) and if so find an approriate document.
        ppI = chMap.find(emptyS);
        if(ppI!=chMap.end())
        {
            docRef = emptyS;
        }
        else
        {
            common::TimeStamp maxT;

            for(ppI=chMap.begin();ppI!=chMap.end();ppI++)
            {
                if(ppI.value().second > maxT)
                {
                    docRef = ppI.key();
                }
            }
        }

        res = true;

        ppJ = m_refCountMap.find(refKey);
        if(ppJ!=m_refCountMap.end())
        {
            QMap<QString,QPair<void *,int> >& rMap = ppJ.value();
            ppK = rMap.find(docRef);
            if(ppK!=rMap.end())
            {
                ppK.value().second += 1;
            }
            else
            {
                insertFlag = true;
            }
        }
        else
        {
            insertFlag = true;
        }

        if(!docRef.isEmpty())
        {
            if(!checkOut(pathFromKey(docRef)))
            {
                printError("checkOut","Failed to checkout document");
            }
        }

        if(insertFlag)
        {
            QString cmdQ;
            QByteArray bkArray;
            SQLiteQuery chkQ(m_db);
            NSURL *bkUrl,*docUrl;
            NSError *err = nil;
            NSData *bkData;
            BOOL staleFlag;

            cmdQ = "SELECT bookmark FROM sandBoxURL WHERE url='" + TrackDB::dbString(refKey) + "' AND docUrl='" + TrackDB::dbString(docRef) + "'";
            chkQ.prepare(cmdQ);
            chkQ.bind(bkArray);
            if(chkQ.next())
            {
                bkData = [NSData dataWithBytes:(bkArray.data()) length:(bkArray.size())];

                if(!docRef.isEmpty())
                {
                    NSString *nS = [NSString stringWithUTF8String:(docRef.toUtf8().constData())];
                    docUrl = [NSURL URLWithString:nS];
                }
                else
                {
                    docUrl = nil;
                }
                updateAccessTime(refKey,docRef);
            }
            else
            {
                res = false;
            }

            if(res)
            {

                const char *bkMem = (const char *)[bkData bytes];
                int bkLen = [bkData length];

                bkUrl = [NSURL URLByResolvingBookmarkData:bkData options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:docUrl bookmarkDataIsStale:&staleFlag error:&err];
                if(bkUrl!=nil)
                {
                    if([bkUrl startAccessingSecurityScopedResource])
                    {
                        ppJ = m_refCountMap.find(refKey);
                        if(ppJ==m_refCountMap.end())
                        {
                            QMap<QString,QPair<void *,int> > refMap;
                            m_refCountMap.insert(refKey,refMap);
                            ppJ = m_refCountMap.find(refKey);
                        }
                        QMap<QString,QPair<void *,int> >& rMap = ppJ.value();
                        ppK = rMap.find(docRef);
                        rMap.insert(docRef,QPair<void *,int>((void *)bkUrl,1));
                        [bkUrl retain];

                        if(!docRef.isEmpty())
                        {
                            QMap<QString,QSet<QString> >::iterator ppL;

                            ppL = m_docMap.find(docRef);
                            if(ppL!=m_docMap.end())
                            {
                                QSet<QString>& dMap = ppL.value();
                                dMap.insert(refKey);
                            }
                            else
                            {
                                QSet<QString> dSet;
                                dSet.insert(refKey);
                                m_docMap.insert(docRef,dSet);
                            }
                        }
                        res = true;
                    }
                    else
                    {
                        printError("checkOut","Failed to start accessing security scoped resource");
                        res = false;
                    }
                }
                else
                {
                    if(err!=nil)
                    {
                        printError("checkOut",(void *)err);
                    }
                    else
                    {
                        printError("checkOut","Failed to resolve security scoped bookmark");
                    }
                    res = false;
                }
            }

            if(!res)
            {
                QString cmdD;
                cmdD = "DELETE FROM sandBoxUrl WHERE url='" + TrackDB::dbString(refKey) + "' AND docUrl='" + TrackDB::dbString(docRef) + "'";
                m_db->exec(cmdD);
            }
        }

        m_mutex.unlock();
    }
    else
    {
        printError("checkOut","No bookmark found related to file");
        res = false;
    }
    return res;
}


//-------------------------------------------------------------------------------------------

bool SBBookmarkService::checkIn(const QString& fileName)
{
    QString refKey,docRef;
    QMap<QString,QPair<int,common::TimeStamp> > chMap;
    bool res;

    if(isMusicDirectory(fileName) && !isPlaylist(fileName))
    {
        return true;
    }

    refKey = key(fileName);
    getAccessMap(fileName,chMap);

    m_mutex.lock();
    if(!chMap.isEmpty())
    {
        QString emptyS("");
        QMap<QString,QPair<int,common::TimeStamp> >::iterator ppI,ppJ;
        bool flag = false;

        ppI = chMap.find(emptyS);
        if(ppI!=chMap.end())
        {
            if(canCheckIn(refKey,emptyS))
            {
                docRef = emptyS;
                flag = true;
            }
        }
        while(!flag && !chMap.isEmpty())
        {
            common::TimeStamp maxT(0);

            for(ppI=chMap.begin();ppI!=chMap.end();ppI++)
            {
                if(ppI.value().second > maxT)
                {
                    docRef = ppI.key();
                    maxT = ppI.value().second;
                    ppJ = ppI;
                }
            }
            if(canCheckIn(refKey,docRef))
            {
                flag = true;
            }
            chMap.erase(ppJ);
        }

        if(flag)
        {
            if(doCheckIn(refKey,docRef))
            {
                res = true;
            }
            else
            {
                printError("checkIn","Error checking in bookmark");
                res = false;
            }
        }
        else
        {
            printError("checkIn","Failed to find document reference for checkout");
            res = false;
        }
    }
    else
    {
        printError("checkIn","No bookmark found related to file");
        res = false;
    }
    m_mutex.unlock();
    return res;
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::canCheckIn(const QString& refKey,const QString& docKey)
{
    QMap<QString,QMap<QString,QPair<void *,int> > >::iterator ppJ;
    bool flag;

    ppJ = m_refCountMap.find(refKey);
    if(ppJ!=m_refCountMap.end())
    {
        QMap<QString,QPair<void *,int> >::iterator ppK;
        QMap<QString,QPair<void *,int> >& rMap = ppJ.value();
        ppK=rMap.find(docKey);
        if(ppK!=rMap.end())
        {
            flag = true;
        }
        else
        {
            flag = false;
        }
    }
    else
    {
        flag = false;
    }
    return flag;
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::doCheckIn(const QString& refKey,const QString& docKey)
{
    QMap<QString,QMap<QString,QPair<void *,int> > >::iterator ppJ;
    bool res;

    ppJ = m_refCountMap.find(refKey);
    if(ppJ!=m_refCountMap.end())
    {
        QMap<QString,QPair<void *,int> >::iterator ppK;
        QMap<QString,QPair<void *,int> >& rMap = ppJ.value();
        ppK=rMap.find(docKey);
        if(ppK!=rMap.end())
        {
            QPair<void *,int>& rPair = ppK.value();

            rPair.second -= 1;
            if(rPair.second<=0)
            {
                NSURL *u = (NSURL *)rPair.first;
                [u stopAccessingSecurityScopedResource];
                [u release];
                rMap.erase(ppK);
            }
            res = true;
        }
        else
        {
            printError("doCheckIn","No entry for given document reference");
            res = false;
        }
    }
    else
    {
        printError("doCheckIn","No reference entry for given file");
        res = false;
    }
    if(res && !docKey.isEmpty())
    {
        if(!checkIn(pathFromKey(docKey)))
        {
            printError("doCheckIn","Failed to check in parent document");
            return false;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void SBBookmarkService::shutdown()
{
    QMap<QString,QMap<QString,QPair<void *,int> > >::iterator ppI;
    QMap<QString,QPair<void *,int> >::iterator ppJ;

    for(ppI=m_refCountMap.begin();ppI!=m_refCountMap.end();ppI++)
    {
        QMap<QString,QPair<void *,int> >& rMap = ppI.value();
        for(ppJ=rMap.begin();ppJ!=rMap.end();ppJ++)
        {
            NSURL *u = (NSURL *)ppJ.value().first;
            [u stopAccessingSecurityScopedResource];
            [u release];
        }
    }
    m_refCountMap.clear();
}

//-------------------------------------------------------------------------------------------

QString SBBookmarkService::getHomeDirectory()
{
    QString hDir;
    NSString *homeDir;
    const struct passwd *passwd = getpwnam([NSUserName() UTF8String]);

    if(passwd!=0)
    {
        const char *homeDirC = getpwnam([NSUserName() UTF8String])->pw_dir;
        homeDir = [[NSFileManager defaultManager] stringWithFileSystemRepresentation:homeDirC length:strlen(homeDirC)];
        if(homeDir!=nil)
        {
            hDir = QString::fromUtf8([homeDir UTF8String]);
        }
    }
    return hDir;
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::isMusicDirectory(const QString& fileName)
{
    QString musicDir = common::DiskOps::mergeName(getHomeDirectory(),"Music").trimmed();
    bool res = false;

    if(fileName.trimmed().length() >= musicDir.length())
    {
        QString pName = fileName.trimmed().sub(0,musicDir.length());
        if(pName==musicDir)
        {
            res = true;
        }
    }
    return res;
}

TEST(SBBookmarkService,isMusicDirectoryGivenHome)
{
    QString dir = SBBookmarkService::getHomeDirectory() + "/";
    SBBookmarkService svr;
    EXPECT_FALSE(svr.isMusicDirectory(dir));
}

TEST(SBBookmarkService,isMusicDirectoryGivenHomeDocuments)
{
    QString dir = SBBookmarkService::getHomeDirectory() + "/Documents";
    SBBookmarkService svr;
    EXPECT_FALSE(svr.isMusicDirectory(dir));
}

TEST(SBBookmarkService,isMusicDirectoryGivenMusicRoot)
{
    QString dir = SBBookmarkService::getHomeDirectory() + "/Music";
    SBBookmarkService svr;
    EXPECT_TRUE(svr.isMusicDirectory(dir));
}

TEST(SBBookmarkService,isMusicDirectoryGivenMusicSubDirectory)
{
    QString dir = SBBookmarkService::getHomeDirectory() + "/Music/iTunes";
    SBBookmarkService svr;
    EXPECT_TRUE(svr.isMusicDirectory(dir));
}

TEST(SBBookmarkService,isMusicDirectoryGivenFileInMusicSubDirectory)
{
    QString dir = SBBookmarkService::getHomeDirectory() + "/Music/iTunes/nameOfTune.m4a";
    SBBookmarkService svr;
    EXPECT_TRUE(svr.isMusicDirectory(dir));
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::isPlaylist(const QString& fileName)
{
    int i;
    QString ext;

    for(i=fileName.length()-1;i>=0;i--)
    {
        if(fileName.at(i)==QChar('.'))
        {
            ext = fileName.mid(i+1).trimmed().toLower();
            if(ext==".m3u" || ext==".pls" || ext==".m3u8" || ext==".xspf")
            {
                return true;
            }
        }
    }
    return false;
}

TEST(SBBookmarkService,isPlaylistGivenNoFile)
{
    SBBookmarkService svr;
    EXPECT_FALSE(svr.isPlaylist();
}

TEST(SBBookmarkService,isPlaylistGivenAudioFile)
{
    SBBookmarkService svr;
    EXPECT_FALSE(svr.isPlaylist("/Users/bonez/Music/play.m4a");
}

TEST(SBBookmarkService,isPlaylistGivenM3U)
{
    SBBookmarkService svr;
    EXPECT_TRUE(svr.isPlaylist("/Users/bonez/Music/play.m3u");
}

TEST(SBBookmarkService,isPlaylistGivenM3U8)
{
    SBBookmarkService svr;
    EXPECT_TRUE(svr.isPlaylist("/Users/bonez/Music/play.M3U8");
}

TEST(SBBookmarkService,isPlaylistGivenPLS)
{
    SBBookmarkService svr;
    EXPECT_TRUE(svr.isPlaylist("/Users/bonez/Music/play.pls");
}

TEST(SBBookmarkService,isPlaylistGivenXSPF)
{
    SBBookmarkService svr;
    EXPECT_TRUE(svr.isPlaylist("/Users/bonez/Music/play.xspf");
}

//-------------------------------------------------------------------------------------------

QString SBBookmarkService::userErrorMessage(UserSandboxErrorMessage err)
{
    QString errMsg;

    switch(err)
    {
        case e_cannotLoadPlaylist:
            errMsg  = "Sorry, M3U and PLS playlists outside of your Music folder cannot be loaded due to Apple's sandbox security restrictions. ";
            errMsg += "Please save and load your playlists as xspf files.";
            break;
    }
    return errMsg;
}

//-------------------------------------------------------------------------------------------

bool SBBookmarkService::add(const QString& docFile,const QString& fileName,const QByteArray& bkArray)
{
    bool res = false;

    if(!docFile.isEmpty() && !fileName.isEmpty() && bkArray.size()>0)
    {
        if(has(docFile,fileName))
        {
            return true;
        }

        if(add(docFile,false))
        {
            NSURL *docUrl = (NSURL *)toUrl(docFile);
            NSURL *refUrl = (NSURL *)toUrl(fileName);
            NSData *bkData = [NSData dataWithBytes:(const void *)(bkArray.data()) length:(bkArray.size())];
            NSURL *bkUrl;
            BOOL staleFlag;
            NSError *err = nil;

            const char *x = (const char *)[bkData bytes];
            int xLen = [bkData length];

            bkUrl = [NSURL URLByResolvingBookmarkData:bkData options:NSURLBookmarkResolutionWithSecurityScope relativeToURL:docUrl bookmarkDataIsStale:&staleFlag error:&err];
            if(bkUrl!=nil)
            {
                QString kRef,dRef;

                kRef = key(fileName);
                dRef = key(docFile);

                if(addToDB((void *)bkData,kRef,dRef,true))
                {
                    res = true;
                }
            }
            else
            {
                if(err!=nil)
                {
                    printError("add",(void *)err);
                }
                else
                {
                    printError("add","Failed to resolve security bookmark");
                }
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QByteArray SBBookmarkService::getBookmarkArray(const QString& docFile,const QString& refFile)
{
    QString docKey,refKey,cmdQ;
    QByteArray bkArray,eArray;
    SQLiteQuery uKeyQ(m_db);

    docKey = key(docFile);
    refKey = key(refFile);

    cmdQ = "SELECT bookmark FROM sandBoxURL WHERE url='" + TrackDB::dbString(refKey) + "' AND docUrl='" + TrackDB::dbString(docKey) + "'";
    uKeyQ.prepare(cmdQ);
    uKeyQ.bind(eArray);
    if(uKeyQ.next())
    {
        bkArray = eArray;
    }
    return bkArray;
}

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
