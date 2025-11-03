#include "gtest/gtest.h"
#include "gmock/gmock.h"

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#include "track/db/inc/SandboxBookmarkService.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "track/db/test/SQLiteMock.h"
#include "common/inc/DiskOps.h"
#include "common/inc/DiskIF.h"

using namespace omega::track::db;
using namespace omega::track::model;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceTest : public SandboxBookmarkService
{
    public:
        SandboxBookmarkServiceTest();
        virtual ~SandboxBookmarkServiceTest();

        QString testPathFromKey(const QString& k);
        NSURL *testToUrl(const QString& fileName);

        QString testKey(const QString& fileName);
        QString testKey(NSURL *fileUrl);

        bool testHasURL(const QString& key,bool readOnlyFlag);
        bool testHasURL(const QString& docKey,const QString& refKey,bool readOnlyFlag);

        bool testIsMusicDirectory(const QString& fileName);
        bool testIsPlaylist(const QString& fileName);

        bool testAddToDB(const QString& fileUrl,const QString& docUrl,bool readOnlyFlag,const QByteArray& bkArray);

        void testUpdateAccessTime(const QString& keyRef,const QString& docRef);
        void testUpdateAccessTime(const QString& keyRef,const QString& docRef,const common::TimeStamp& accessTime);

        NSData *testSecurityBookmarkFromURL(const QString& fileName,NSURLBookmarkCreationOptions options);

        QByteArray testSecurityBookmark(const QString& fileName,bool readOnlyFlag);
        QByteArray testSecurityBookmark(const QString& docFileName,const QString& refFileName,bool readOnlyFlag);

        void testGetAccessMap(const QString& fileName,QMap<QString,QPair<bool,common::TimeStamp> >& chMap);

        bool testIsBookmarkRequired(const QString& fileName,bool readOnlyFlag);
        bool testCanAccess(bool readOnlyFlag,bool access) const;

        bool testAccessDocumentUrlForFile(const QString& fileName,bool readOnlyFlag,bool checkInFlag,QString& docFileName);

        bool testIsBookmarkCreationRequired(const QString& fileName,const QString& docFileName);
        NSData *testGetBookmarkFromReference(const QString& fileName,const QString& docFileName);
        void testAddSecuredResourceToReferenceMap(const QString& fileName,const QString& docFileName,NSURL *bookmark);
        bool testCheckOutSecuredResource(const QString& fileName,const QString& docFileName,NSData *bookmarkData);
        void testDeleteBookmark(const QString& fileName,const QString& docFileName);

        bool testCheckOutBookmarkResource(const QString& fileName,const QString& docFileName);

        bool testCheckInDocumentAsRequired(const QString& docFileName,bool readOnlyFlag);
        bool testDoCheckIn(const QString& fileName,const QString& docFileName,bool readOnlyFlag);
        bool testCanCheckIn(const QString& fileName,const QString& docFileName);

        void testShutdown();

        bool testCheckOutParentDirectory(const QString& fileName,bool readOnlyFlag);
        bool testCheckInParentDirectory(const QString& fileName,bool readOnlyFlag);
};

//-------------------------------------------------------------------------------------------

SandboxBookmarkServiceTest::SandboxBookmarkServiceTest() : SandboxBookmarkService()
{}

//-------------------------------------------------------------------------------------------

SandboxBookmarkServiceTest::~SandboxBookmarkServiceTest()
{}

//-------------------------------------------------------------------------------------------

QString SandboxBookmarkServiceTest::testPathFromKey(const QString& k)
{
    return pathFromKey(k);
}

//-------------------------------------------------------------------------------------------

NSURL *SandboxBookmarkServiceTest::testToUrl(const QString& fileName)
{
    return toUrl(fileName);
}

//-------------------------------------------------------------------------------------------

QString SandboxBookmarkServiceTest::testKey(const QString& fileName)
{
    return key(fileName);
}

//-------------------------------------------------------------------------------------------

QString SandboxBookmarkServiceTest::testKey(NSURL *fileUrl)
{
    return key(fileUrl);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testHasURL(const QString& key,bool readOnlyFlag)
{
    return hasURL(key,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testHasURL(const QString& docKey,const QString& refKey,bool readOnlyFlag)
{
    return hasURL(docKey,refKey,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testIsMusicDirectory(const QString& fileName)
{
    return isMusicDirectory(fileName);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testIsPlaylist(const QString& fileName)
{
    return isPlaylist(fileName);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testAddToDB(const QString& fileUrl,const QString& docUrl,bool readOnlyFlag,const QByteArray& bkArray)
{
    return addToDB(fileUrl,docUrl,readOnlyFlag,bkArray);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTest::testUpdateAccessTime(const QString& keyRef,const QString& docRef)
{
    updateAccessTime(keyRef,docRef);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTest::testUpdateAccessTime(const QString& keyRef,const QString& docRef,const common::TimeStamp& accessTime)
{
    updateAccessTime(keyRef,docRef,accessTime);
}

//-------------------------------------------------------------------------------------------

NSData *SandboxBookmarkServiceTest::testSecurityBookmarkFromURL(const QString& fileName,NSURLBookmarkCreationOptions options)
{
    return securityBookmarkFromURL(fileName,options);
}

//-------------------------------------------------------------------------------------------

QByteArray SandboxBookmarkServiceTest::testSecurityBookmark(const QString& fileName,bool readOnlyFlag)
{
    return securityBookmark(fileName,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

QByteArray SandboxBookmarkServiceTest::testSecurityBookmark(const QString& docFileName,const QString& refFileName,bool readOnlyFlag)
{
    return securityBookmark(docFileName,refFileName,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTest::testGetAccessMap(const QString& fileName,QMap<QString,QPair<bool,common::TimeStamp> >& chMap)
{
    getAccessMap(fileName,chMap);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testCanAccess(bool readOnlyFlag,bool access) const
{
    return canAccess(readOnlyFlag,access);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testAccessDocumentUrlForFile(const QString& fileName,bool readOnlyFlag,bool checkInFlag,QString& docFileName)
{
    return accessDocumentUrlForFile(fileName,readOnlyFlag,checkInFlag,docFileName);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testIsBookmarkCreationRequired(const QString& fileName,const QString& docFileName)
{
    return isBookmarkCreationRequired(fileName,docFileName);
}

//-------------------------------------------------------------------------------------------

NSData *SandboxBookmarkServiceTest::testGetBookmarkFromReference(const QString& fileName,const QString& docFileName)
{
    return getBookmarkFromReference(fileName,docFileName);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTest::testAddSecuredResourceToReferenceMap(const QString& fileName,const QString& docFileName,NSURL *bookmark)
{
    addSecuredResourceToReferenceMap(fileName,docFileName,bookmark);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testCheckOutSecuredResource(const QString& fileName,const QString& docFileName,NSData *bookmarkData)
{
    return checkOutSecuredResource(fileName,docFileName,bookmarkData);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTest::testDeleteBookmark(const QString& fileName,const QString& docFileName)
{
    deleteBookmark(fileName,docFileName);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testIsBookmarkRequired(const QString& fileName,bool readOnlyFlag)
{
    return isBookmarkRequired(fileName,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testCheckOutBookmarkResource(const QString& fileName,const QString& docFileName)
{
    return checkOutBookmarkResource(fileName,docFileName);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testCheckInDocumentAsRequired(const QString& docFileName,bool readOnlyFlag)
{
    return checkInDocumentAsRequired(docFileName,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testDoCheckIn(const QString& fileName,const QString& docFileName,bool readOnlyFlag)
{
    return doCheckIn(fileName,docFileName,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testCanCheckIn(const QString& fileName,const QString& docFileName)
{
    return canCheckIn(fileName,docFileName);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTest::testShutdown()
{
    shutdown();
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testCheckOutParentDirectory(const QString& fileName,bool readOnlyFlag)
{
    return checkOutParentDirectory(fileName,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

bool SandboxBookmarkServiceTest::testCheckInParentDirectory(const QString& fileName,bool readOnlyFlag)
{
    return checkInParentDirectory(fileName,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTestInsertURL(const QString& url,const QString& docUrl,bool readOnlyFlag)
{
    SQLiteDatabase *db = TrackDB::instance()->db();

    QString bkData("Bookmark test data");
    QByteArray bkArray(bkData.toUtf8().constData(),bkData.toUtf8().size());
    int access = (readOnlyFlag) ? 1 : 0;
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

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTestInsertURL(const QString& url,const QString& docUrl)
{
    SandboxBookmarkServiceTestInsertURL(url,docUrl,false);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTestInsertURL(const QString& url,bool readOnlyFlag)
{
    QString emptyDoc;
    SandboxBookmarkServiceTestInsertURL(url,emptyDoc,readOnlyFlag);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTestInsertURL(const QString& url)
{
    QString emptyDoc;
    SandboxBookmarkServiceTestInsertURL(url,emptyDoc);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTestDeleteURL(const QString& url,const QString& docUrl)
{
    QString cmdD;
    QString kRefV = TrackDB::dbString(url);
    QString dRefV = TrackDB::dbString(docUrl);

    cmdD = "DELETE FROM sandBoxURL WHERE url='" + kRefV + "' AND docUrl='" + dRefV + "'";
    TrackDB::instance()->db()->exec(cmdD);
}

//-------------------------------------------------------------------------------------------

void SandboxBookmarkServiceTestDeleteURL(const QString& url)
{
    QString emptyDoc;
    SandboxBookmarkServiceTestDeleteURL(url,emptyDoc);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,keyIsEmptyGivenEmpty)
{
    QString testFileName;
    QString expectFileUrl;
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testKey(testFileName)==expectFileUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,keyIsFileUrlForGivenFileNamePath)
{
    QString testFileName = QString::fromLatin1("/path/to/web_root/folder/file.html");
    QString expectFileUrl = QString::fromLatin1("file:///path/to/web_root/folder/file.html");
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testKey(testFileName)==expectFileUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,pathFromKeyThatExists)
{
    QString trackDBName = "file://" + TrackDBTestEnviroment::instance()->getDBDirectory() + "track.db";
    QString expectName = TrackDBTestEnviroment::instance()->getDBDirectory() + "track.db";

    SandboxBookmarkServiceTest svr;
    QString path = svr.testPathFromKey(trackDBName);

    EXPECT_TRUE(path==expectName);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,pathFromKeyThatDoesNotExist)
{
    QString trackDBName = "file://" + TrackDBTestEnviroment::instance()->getDBDirectory() + "track_not_exist.db";
    QString expectName = TrackDBTestEnviroment::instance()->getDBDirectory() + "track_not_exist.db";

    SandboxBookmarkServiceTest svr;
    QString path = svr.testPathFromKey(trackDBName);

    EXPECT_TRUE(path==expectName);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,toUrlWhenEmpty)
{
    SandboxBookmarkServiceTest svr;
    NSURL *url = svr.testToUrl(QString::fromLatin1(""));
    EXPECT_TRUE(url==nil);
}
//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,toUrlWithContent)
{
    SandboxBookmarkServiceTest svr;
    NSURL *url = svr.testToUrl(QString::fromLatin1("/path/to/web_root/folder/file.html"));
    EXPECT_TRUE(url!=nil);
    QString tURL = svr.testKey(url);
    EXPECT_EQ(tURL,"file:///path/to/web_root/folder/file.html");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLGetReadOnlyWhenURLIsReadOnly)
{
    SandboxBookmarkServiceTestInsertURL("file:://path/to/music.mp3",true);

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testHasURL("file:://path/to/music.mp3",true));

    SandboxBookmarkServiceTestDeleteURL("file:://path/to/music.mp3");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLGetReadWriteWhenURLIsReadOnly)
{
    SandboxBookmarkServiceTestInsertURL("file:://path/to/music.mp3",true);

    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.testHasURL("file:://path/to/music.mp3",false));

    SandboxBookmarkServiceTestDeleteURL("file:://path/to/music.mp3");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLGetReadOnlyWhenURLIsReadWrite)
{
    SandboxBookmarkServiceTestInsertURL("file:://path/to/music.mp3",false);

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testHasURL("file:://path/to/music.mp3",true));

    SandboxBookmarkServiceTestDeleteURL("file:://path/to/music.mp3");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLGetReadWriteWhenURLIsReadWrite)
{
    SandboxBookmarkServiceTestInsertURL("file:://path/to/music.mp3",false);

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testHasURL("file:://path/to/music.mp3",false));

    SandboxBookmarkServiceTestDeleteURL("file:://path/to/music.mp3");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLWhenURLExists)
{
    SandboxBookmarkServiceTestInsertURL("file:://path/to/music.mp3",false);

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testHasURL("file:://path/to/music.mp3",false));

    SandboxBookmarkServiceTestDeleteURL("file:://path/to/music.mp3");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLWhenURLDoesNotExist)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.testHasURL("file:://path/to/music.mp3",false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLDocumentGetReadOnlyWhenURLIsReadOnly)
{
    SandboxBookmarkServiceTestInsertURL("file:://path/to/music.mp3","file:://path/to/playlist.pls",true);

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testHasURL("file:://path/to/playlist.pls","file:://path/to/music.mp3",true));

    SandboxBookmarkServiceTestDeleteURL("file:://path/to/music.mp3","file:://path/to/playlist.pls");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLDocumentGetReadWriteWhenURLIsReadOnly)
{
    SandboxBookmarkServiceTestInsertURL("file:://path/to/music.mp3","file:://path/to/playlist.pls",true);

    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.testHasURL("file:://path/to/playlist.pls","file:://path/to/music.mp3",false));

    SandboxBookmarkServiceTestDeleteURL("file:://path/to/music.mp3","file:://path/to/playlist.pls");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLDocumentGetReadOnlyWhenURLIsReadWrite)
{
    SandboxBookmarkServiceTestInsertURL("file:://path/to/music.mp3","file:://path/to/playlist.pls",false);

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testHasURL("file:://path/to/playlist.pls","file:://path/to/music.mp3",true));

    SandboxBookmarkServiceTestDeleteURL("file:://path/to/music.mp3","file:://path/to/playlist.pls");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLDocumentGetReadWriteWhenURLIsReadWrite)
{
    SandboxBookmarkServiceTestInsertURL("file:://path/to/music.mp3","file:://path/to/playlist.pls",false);

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testHasURL("file:://path/to/playlist.pls","file:://path/to/music.mp3",false));

    SandboxBookmarkServiceTestDeleteURL("file:://path/to/music.mp3","file:://path/to/playlist.pls");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLWithDocumentWhenURLExists)
{
    SandboxBookmarkServiceTestInsertURL("file:://path/to/music.mp3","file:://path/to/playlist.pls",false);

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testHasURL("file:://path/to/playlist.pls","file:://path/to/music.mp3",false));

    SandboxBookmarkServiceTestDeleteURL("file:://path/to/music.mp3","file:://path/to/playlist.pls");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasURLWithDocumentWhenURLDoesNotExist)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.testHasURL("file:://path/to/playlist.pls","file:://path/to/music.mp3",false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenMusicDirectoryAndNotPlaylistReadOnly)
{
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.mp3";

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.has(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenMusicDirectoryAndNotPlaylistReadWrite)
{
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.mp3";

    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.has(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasDocumentGivenMusicDirectoryAndNotPlaylistReadOnly)
{
    QString docName = "/path/to/doc.xml";
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.mp3";

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.has(docName,fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasDocumentGivenMusicDirectoryAndNotPlaylistReadWrite)
{
    QString docName = "/path/to/doc.xml";
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.mp3";

    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.has(docName,fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenMusicDirectoryAndPlaylistWithNoBookmarkReadOnly)
{
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.pls";

    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.has(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenMusicDirectoryAndPlaylistWithNoBookmarkReadWrite)
{
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.pls";

    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.has(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasDocumentGivenMusicDirectoryAndPlaylistWithNoBookmarkReadOnly)
{
    QString docName = "/path/to/doc.xml";
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.pls";

    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.has(docName,fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasDocumentGivenMusicDirectoryAndPlaylistWithNoBookmarkReadWrite)
{
    QString docName = "/path/to/doc.xml";
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.pls";

    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.has(docName,fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenMusicDirectoryAndPlaylistWithBookmarkReadOnly)
{
    SandboxBookmarkServiceTest svr;
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.pls";
    QString fileKey = svr.testKey(fileName);

    SandboxBookmarkServiceTestInsertURL(fileKey,true);
    EXPECT_TRUE(svr.has(fileName,true));
    SandboxBookmarkServiceTestDeleteURL(fileKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenMusicDirectoryAndPlaylistWithBookmarkReadWrite)
{
    SandboxBookmarkServiceTest svr;
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.pls";
    QString fileKey = svr.testKey(fileName);

    SandboxBookmarkServiceTestInsertURL(fileKey,false);
    EXPECT_TRUE(svr.has(fileName,false));
    SandboxBookmarkServiceTestDeleteURL(fileKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasDocumentGivenMusicDirectoryAndPlaylistWithBookmarkReadOnly)
{
    QString docName = "/path/to/doc.xml";

    SandboxBookmarkServiceTest svr;
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.pls";
    QString fileKey = svr.testKey(fileName);
    QString docKey = svr.testKey(docName);

    SandboxBookmarkServiceTestInsertURL(fileKey,docKey,true);
    EXPECT_TRUE(svr.has(docName,fileName,true));
    SandboxBookmarkServiceTestDeleteURL(fileKey,docKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasDocumentGivenMusicDirectoryAndPlaylistWithBookmarkReadWrite)
{
    QString docName = "/path/to/doc.xml";

    SandboxBookmarkServiceTest svr;
    QString fileName = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/file.pls";
    QString fileKey = svr.testKey(fileName);
    QString docKey = svr.testKey(docName);

    SandboxBookmarkServiceTestInsertURL(fileKey,docKey,false);
    EXPECT_TRUE(svr.has(docName,fileName,false));
    SandboxBookmarkServiceTestDeleteURL(fileKey,docKey);
}
//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenNotMusicDirectoryAndNoBookmarkReadOnly)
{
    SandboxBookmarkServiceTest svr;
    QString fileName = "/Music/iTunes/file.pls";
    EXPECT_FALSE(svr.has(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenNotMusicDirectoryAndNoBookmarkReadWrite)
{
    SandboxBookmarkServiceTest svr;
    QString fileName = "/Music/iTunes/file.pls";
    EXPECT_FALSE(svr.has(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasDocumentGivenNotMusicDirectoryAndNoBookmarkReadOnly)
{
    QString docName = "/path/to/doc.xml";
    SandboxBookmarkServiceTest svr;
    QString fileName = "/Music/iTunes/file.pls";
    EXPECT_FALSE(svr.has(docName,fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasDocumentGivenNotMusicDirectoryAndNoBookmarkReadWrite)
{
    QString docName = "/path/to/doc.xml";
    SandboxBookmarkServiceTest svr;
    QString fileName = "/Music/iTunes/file.pls";
    EXPECT_FALSE(svr.has(docName,fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenNotMusicDirectoryAndBookmarkReadOnly)
{
    SandboxBookmarkServiceTest svr;

    QString fileName = "/Music/iTunes/file.pls";
    QString fileKey = svr.testKey(fileName);

    SandboxBookmarkServiceTestInsertURL(fileKey,true);
    EXPECT_TRUE(svr.has(fileName,true));
    SandboxBookmarkServiceTestDeleteURL(fileKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenNotMusicDirectoryAndBookmarkReadWrite)
{
    SandboxBookmarkServiceTest svr;

    QString fileName = "/Music/iTunes/file.pls";
    QString fileKey = svr.testKey(fileName);

    SandboxBookmarkServiceTestInsertURL(fileKey,false);
    EXPECT_TRUE(svr.has(fileName,false));
    SandboxBookmarkServiceTestDeleteURL(fileKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasDocumentGivenNotMusicDirectoryAndBookmarkReadOnly)
{
    SandboxBookmarkServiceTest svr;

    QString docName = "/path/to/doc.xml";
    QString fileName = "/Music/iTunes/file.pls";
    QString fileKey = svr.testKey(fileName);
    QString docKey = svr.testKey(docName);

    SandboxBookmarkServiceTestInsertURL(fileKey,docKey,true);
    EXPECT_TRUE(svr.has(docName,fileName,true));
    SandboxBookmarkServiceTestDeleteURL(fileKey,docKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasDocumentGivenNotMusicDirectoryAndBookmarkReadWrite)
{
    SandboxBookmarkServiceTest svr;

    QString docName = "/path/to/doc.xml";
    QString fileName = "/Music/iTunes/file.pls";
    QString fileKey = svr.testKey(fileName);
    QString docKey = svr.testKey(docName);

    SandboxBookmarkServiceTestInsertURL(fileKey,docKey,false);
    EXPECT_TRUE(svr.has(docName,fileName,false));
    SandboxBookmarkServiceTestDeleteURL(fileKey,docKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenReadOnlyParentDirectoryBookmarkReadOnly)
{
    SandboxBookmarkServiceTest svr;

    QString fileName = "/Volume/LaCie/Music/iTunes/iTunes Media/Albums/Compilation/Artist/Part1/track01.pls";
    QString bkDirName = "/Volume/LaCie/Music/iTunes/iTunes Media";
    QString bkDirKey = svr.testKey(bkDirName);

    SandboxBookmarkServiceTestInsertURL(bkDirKey,true);
    EXPECT_TRUE(svr.has(fileName,true));
    SandboxBookmarkServiceTestDeleteURL(bkDirKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenReadOnlyParentDirectoryBookmarkReadWrite)
{
    SandboxBookmarkServiceTest svr;

    QString fileName = "/Volume/LaCie/Music/iTunes/iTunes Media/Albums/Compilation/Artist/Part1/track01.pls";
    QString bkDirName = "/Volume/LaCie/Music/iTunes/iTunes Media";
    QString bkDirKey = svr.testKey(bkDirName);

    SandboxBookmarkServiceTestInsertURL(bkDirKey,true);
    EXPECT_FALSE(svr.has(fileName,false));
    SandboxBookmarkServiceTestDeleteURL(bkDirKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenReadWriteParentDirectoryBookmarkReadOnly)
{
    SandboxBookmarkServiceTest svr;

    QString fileName = "/Volume/LaCie/Music/iTunes/iTunes Media/Albums/Compilation/Artist/Part1/track01.pls";
    QString bkDirName = "/Volume/LaCie/Music/iTunes/iTunes Media";
    QString bkDirKey = svr.testKey(bkDirName);

    SandboxBookmarkServiceTestInsertURL(bkDirKey,false);
    EXPECT_TRUE(svr.has(fileName,true));
    SandboxBookmarkServiceTestDeleteURL(bkDirKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,hasGivenReadWriteParentDirectoryBookmarkReadWrite)
{
    SandboxBookmarkServiceTest svr;

    QString fileName = "/Volume/LaCie/Music/iTunes/iTunes Media/Albums/Compilation/Artist/Part1/track01.pls";
    QString bkDirName = "/Volume/LaCie/Music/iTunes/iTunes Media";
    QString bkDirKey = svr.testKey(bkDirName);

    SandboxBookmarkServiceTestInsertURL(bkDirKey,false);
    EXPECT_TRUE(svr.has(fileName,false));
    SandboxBookmarkServiceTestDeleteURL(bkDirKey);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isMusicDirectoryGivenHome)
{
    QString dir = SandboxBookmarkService::getHomeDirectory() + "/";
    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.testIsMusicDirectory(dir));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isMusicDirectoryGivenHomeDocuments)
{
    QString dir = SandboxBookmarkService::getHomeDirectory() + "/Documents";
    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.testIsMusicDirectory(dir));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isMusicDirectoryGivenMusicRoot)
{
    QString dir = SandboxBookmarkService::getHomeDirectory() + "/Music";
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testIsMusicDirectory(dir));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isMusicDirectoryGivenMusicSubDirectory)
{
    QString dir = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes";
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testIsMusicDirectory(dir));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isMusicDirectoryGivenFileInMusicSubDirectory)
{
    QString dir = SandboxBookmarkService::getHomeDirectory() + "/Music/iTunes/nameOfTune.m4a";
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testIsMusicDirectory(dir));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isPlaylistGivenNoFile)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.testIsPlaylist(""));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isPlaylistGivenAudioFile)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.testIsPlaylist("/Users/bonez/Music/play.m4a"));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isPlaylistGivenM3U)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testIsPlaylist("/Users/bonez/Music/play.m3u"));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isPlaylistGivenM3U8)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testIsPlaylist("/Users/bonez/Music/play.M3U8"));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isPlaylistGivenPLS)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testIsPlaylist("/Users/bonez/Music/play.pls"));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isPlaylistGivenXSPF)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testIsPlaylist("/Users/bonez/Music/play.xspf"));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceAddToDBTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(getDB,SQLiteDatabase *());
        MOCK_METHOD0(createDBInsert,SQLiteInsert *());
        MOCK_METHOD1(removeDBInsert,void(SQLiteInsert *pInsert));
};

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceAddToDBInsert
{
    public:
        QString m_url;
        QString m_urlDoc;
        int m_access;
        common::TimeStamp m_accessTime;
        QByteArray m_bookmark;

        void bindUrl(QString& v) {m_url = v;}
        void bindUrlDoc(QString& v) {m_urlDoc = v;}
        void bindAccess(int& v) {m_access = v;}
        void bindAccessTime(tuint64& v) {m_accessTime = v;}
        void bindBookmark(QByteArray& v) {m_bookmark = v;}
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addToDBFailureOnDBException)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };

    QString fileUrl = "file:://path/to/some/\'music\'/track.m4a";
    QString docUrl  = "file:://document/\'playlist\'/list.pls";
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    QString cmdI = "INSERT INTO sandBoxURL VALUES(?,?,?,?,?)";
    QString cmdD = "DELETE FROM sandBoxURL WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";

    SandboxBookmarkServiceAddToDBInsert iBind;

    SQLiteInsertMock dInsert(TrackDB::instance()->db());
    EXPECT_CALL(dInsert,prepare(Eq(cmdI))).Times(1).WillOnce(Throw(SQLiteException(TrackDB::instance()->db(),"Test")));

    QString startExecCmd = "SAVEPOINT addBookmark";
    QString endExecCmd = "ROLLBACK TO SAVEPOINT addBookmark";

    SQLiteDatabaseMock dbMock;
    EXPECT_CALL(dbMock,exec(Eq(startExecCmd))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(endExecCmd))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(cmdD))).Times(1);

    SandboxBookmarkServiceAddToDBTest svr;
    EXPECT_CALL(svr,createDBInsert()).Times(1).WillOnce(Return(&dInsert));
    EXPECT_CALL(svr,removeDBInsert(&dInsert)).Times(1);
    EXPECT_CALL(svr,getDB()).WillRepeatedly(Return(&dbMock));

    EXPECT_FALSE(svr.testAddToDB(fileUrl,docUrl,true,bkArray));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addToDBFailureOnInsert)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };

    QString fileUrl = "file:://path/to/some/\'music\'/track.m4a";
    QString docUrl  = "file:://document/\'playlist\'/list.pls";
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    QString cmdI = "INSERT INTO sandBoxURL VALUES(?,?,?,?,?)";
    QString cmdD = "DELETE FROM sandBoxURL WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";

    SandboxBookmarkServiceAddToDBInsert iBind;

    SQLiteInsertMock dInsert(TrackDB::instance()->db());
    EXPECT_CALL(dInsert,prepare(Eq(cmdI))).Times(1);
    EXPECT_CALL(dInsert,bind(A<QString&>())).Times(2);
    EXPECT_CALL(dInsert,bind(A<int&>())).Times(1);
    EXPECT_CALL(dInsert,bind(A<tuint64&>())).Times(1);
    EXPECT_CALL(dInsert,bind(A<QByteArray&>())).Times(1);
    EXPECT_CALL(dInsert,next()).Times(1).WillOnce(Return(false));

    QString startExecCmd = "SAVEPOINT addBookmark";
    QString endExecCmd = "ROLLBACK TO SAVEPOINT addBookmark";

    SQLiteDatabaseMock dbMock;
    EXPECT_CALL(dbMock,exec(Eq(startExecCmd))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(endExecCmd))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(cmdD))).Times(1);

    SandboxBookmarkServiceAddToDBTest svr;
    EXPECT_CALL(svr,createDBInsert()).Times(1).WillOnce(Return(&dInsert));
    EXPECT_CALL(svr,removeDBInsert(&dInsert)).Times(1);
    EXPECT_CALL(svr,getDB()).WillRepeatedly(Return(&dbMock));

    EXPECT_FALSE(svr.testAddToDB(fileUrl,docUrl,true,bkArray));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addToDBSuccessWithReadOnly)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };

    QString fileUrl = "file:://path/to/some/\'music\'/track.m4a";
    QString docUrl  = "file:://document/\'playlist\'/list.pls";
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    QString cmdI = "INSERT INTO sandBoxURL VALUES(?,?,?,?,?)";
    QString cmdD = "DELETE FROM sandBoxURL WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";

    SandboxBookmarkServiceAddToDBInsert iBind;

    SQLiteInsertMock dInsert(TrackDB::instance()->db());
    EXPECT_CALL(dInsert,prepare(Eq(cmdI))).Times(1);
    EXPECT_CALL(dInsert,bind(A<QString&>())).Times(2)
        .WillOnce(Invoke(&iBind,&SandboxBookmarkServiceAddToDBInsert::bindUrl))
        .WillOnce(Invoke(&iBind,&SandboxBookmarkServiceAddToDBInsert::bindUrlDoc));
    EXPECT_CALL(dInsert,bind(A<int&>())).Times(1)
        .WillOnce(Invoke(&iBind,&SandboxBookmarkServiceAddToDBInsert::bindAccess));
    EXPECT_CALL(dInsert,bind(A<tuint64&>())).Times(1)
        .WillOnce(Invoke(&iBind,&SandboxBookmarkServiceAddToDBInsert::bindAccessTime));
    EXPECT_CALL(dInsert,bind(A<QByteArray&>())).Times(1)
        .WillOnce(Invoke(&iBind,&SandboxBookmarkServiceAddToDBInsert::bindBookmark));
    EXPECT_CALL(dInsert,next()).Times(1).WillOnce(Return(true));

    QString startExecCmd = "SAVEPOINT addBookmark";
    QString endExecCmd = "RELEASE addBookmark";

    SQLiteDatabaseMock dbMock;
    EXPECT_CALL(dbMock,exec(Eq(startExecCmd))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(endExecCmd))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(cmdD))).Times(1);

    SandboxBookmarkServiceAddToDBTest svr;
    EXPECT_CALL(svr,createDBInsert()).Times(1).WillOnce(Return(&dInsert));
    EXPECT_CALL(svr,removeDBInsert(&dInsert)).Times(1);
    EXPECT_CALL(svr,getDB()).WillRepeatedly(Return(&dbMock));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docUrl,true,bkArray));

    EXPECT_TRUE(iBind.m_url==TrackDB::dbString(fileUrl));
    EXPECT_TRUE(iBind.m_urlDoc==TrackDB::dbString(docUrl));
    EXPECT_TRUE(iBind.m_access==1);

    common::TimeStamp lowerATime = common::TimeStamp::now() - 1.0;
    common::TimeStamp upperATime = common::TimeStamp::now() + 1.0;
    EXPECT_TRUE(lowerATime < iBind.m_accessTime && iBind.m_accessTime < upperATime);

    EXPECT_TRUE(iBind.m_bookmark.size()==bkArray.size());
    EXPECT_TRUE(::memcmp(iBind.m_bookmark.constData(),bkArray.constData(),bkArray.size())==0);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addToDBSuccessWithReadWrite)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };

    QString fileUrl = "file:://path/to/some/\'music\'/track.m4a";
    QString docUrl  = "file:://document/\'playlist\'/list.pls";
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    QString cmdI = "INSERT INTO sandBoxURL VALUES(?,?,?,?,?)";
    QString cmdD = "DELETE FROM sandBoxURL WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";

    SandboxBookmarkServiceAddToDBInsert iBind;

    SQLiteInsertMock dInsert(TrackDB::instance()->db());
    EXPECT_CALL(dInsert,prepare(Eq(cmdI))).Times(1);
    EXPECT_CALL(dInsert,bind(A<QString&>())).Times(2)
        .WillOnce(Invoke(&iBind,&SandboxBookmarkServiceAddToDBInsert::bindUrl))
        .WillOnce(Invoke(&iBind,&SandboxBookmarkServiceAddToDBInsert::bindUrlDoc));
    EXPECT_CALL(dInsert,bind(A<int&>())).Times(1)
        .WillOnce(Invoke(&iBind,&SandboxBookmarkServiceAddToDBInsert::bindAccess));
    EXPECT_CALL(dInsert,bind(A<tuint64&>())).Times(1)
        .WillOnce(Invoke(&iBind,&SandboxBookmarkServiceAddToDBInsert::bindAccessTime));
    EXPECT_CALL(dInsert,bind(A<QByteArray&>())).Times(1)
        .WillOnce(Invoke(&iBind,&SandboxBookmarkServiceAddToDBInsert::bindBookmark));
    EXPECT_CALL(dInsert,next()).Times(1).WillOnce(Return(true));

    QString startExecCmd = "SAVEPOINT addBookmark";
    QString endExecCmd = "RELEASE addBookmark";

    SQLiteDatabaseMock dbMock;
    EXPECT_CALL(dbMock,exec(Eq(startExecCmd))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(endExecCmd))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(cmdD))).Times(1);

    SandboxBookmarkServiceAddToDBTest svr;
    EXPECT_CALL(svr,createDBInsert()).Times(1).WillOnce(Return(&dInsert));
    EXPECT_CALL(svr,removeDBInsert(&dInsert)).Times(1);
    EXPECT_CALL(svr,getDB()).WillRepeatedly(Return(&dbMock));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docUrl,false,bkArray));

    EXPECT_TRUE(iBind.m_url==TrackDB::dbString(fileUrl));
    EXPECT_TRUE(iBind.m_urlDoc==TrackDB::dbString(docUrl));
    EXPECT_TRUE(iBind.m_access==0);

    common::TimeStamp lowerATime = common::TimeStamp::now() - 1.0;
    common::TimeStamp upperATime = common::TimeStamp::now() + 1.0;
    EXPECT_TRUE(lowerATime < iBind.m_accessTime && iBind.m_accessTime < upperATime);

    EXPECT_TRUE(iBind.m_bookmark.size()==bkArray.size());
    EXPECT_TRUE(::memcmp(iBind.m_bookmark.constData(),bkArray.constData(),bkArray.size())==0);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addToDBIntegration)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/list.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docUrl  = svr.testKey(docName);

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docUrl,true,bkArray));

    EXPECT_TRUE(svr.has(docName,fileName,true));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addToDBIntegrationWhenRecordAlreadyExists)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/list.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docUrl  = svr.testKey(docName);

    SandboxBookmarkServiceTestInsertURL(fileUrl,docUrl,true);

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docUrl,false,bkArray));

    EXPECT_TRUE(svr.has(docName,fileName,false));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docUrl);
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceSecurityBookmark : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD2(securityBookmarkFromURL,NSData *(const QString& fileName,NSURLBookmarkCreationOptions options));
        MOCK_METHOD3(securityBookmarkFromURL,NSData *(const QString& docFileName,const QString& refFileName,NSURLBookmarkCreationOptions options));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,securityBookmarkSuccessReadOnly)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    NSData *bkData = [NSData dataWithBytes:bkMem length:4];
    NSURLBookmarkCreationOptions bkOptions = NSURLBookmarkCreationWithSecurityScope | NSURLBookmarkCreationSecurityScopeAllowOnlyReadAccess;

    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceSecurityBookmark svr;
    EXPECT_CALL(svr,securityBookmarkFromURL(Eq(fileName),bkOptions)).Times(1).WillOnce(Return(bkData));

    QByteArray bkArray = svr.testSecurityBookmark(fileName,true);

    EXPECT_TRUE(bkArray.size()==4);
    EXPECT_TRUE(::memcmp(bkArray.constData(),bkMem,4)==0);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,securityBookmarkDocumentSuccessReadOnly)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    NSData *bkData = [NSData dataWithBytes:bkMem length:4];
    NSURLBookmarkCreationOptions bkOptions = NSURLBookmarkCreationWithSecurityScope | NSURLBookmarkCreationSecurityScopeAllowOnlyReadAccess;

    QString docFileName = "/playlist/document/list.pls";
    QString refFileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceSecurityBookmark svr;
    EXPECT_CALL(svr,securityBookmarkFromURL(Eq(docFileName),Eq(refFileName),bkOptions))
        .Times(1).WillOnce(Return(bkData));

    QByteArray bkArray = svr.testSecurityBookmark(docFileName,refFileName,true);

    EXPECT_TRUE(bkArray.size()==4);
    EXPECT_TRUE(::memcmp(bkArray.constData(),bkMem,4)==0);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,securityBookmarkSuccessReadWrite)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    NSData *bkData = [NSData dataWithBytes:bkMem length:4];
    NSURLBookmarkCreationOptions bkOptions = NSURLBookmarkCreationWithSecurityScope;

    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceSecurityBookmark svr;
    EXPECT_CALL(svr,securityBookmarkFromURL(Eq(fileName),bkOptions)).Times(1).WillOnce(Return(bkData));

    QByteArray bkArray = svr.testSecurityBookmark(fileName,false);

    EXPECT_TRUE(bkArray.size()==4);
    EXPECT_TRUE(::memcmp(bkArray.constData(),bkMem,4)==0);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,securityBookmarkDocumentSuccessReadWrite)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    NSData *bkData = [NSData dataWithBytes:bkMem length:4];
    NSURLBookmarkCreationOptions bkOptions = NSURLBookmarkCreationWithSecurityScope;

    QString docFileName = "/playlist/document/list.pls";
    QString refFileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceSecurityBookmark svr;
    EXPECT_CALL(svr,securityBookmarkFromURL(Eq(docFileName),Eq(refFileName),bkOptions))
        .Times(1).WillOnce(Return(bkData));

    QByteArray bkArray = svr.testSecurityBookmark(docFileName,refFileName,false);

    EXPECT_TRUE(bkArray.size()==4);
    EXPECT_TRUE(::memcmp(bkArray.constData(),bkMem,4)==0);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,securityBookmarkFailure)
{
    NSURLBookmarkCreationOptions bkOptions = NSURLBookmarkCreationWithSecurityScope;

    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceSecurityBookmark svr;
    EXPECT_CALL(svr,securityBookmarkFromURL(Eq(fileName),bkOptions)).Times(1).WillOnce(Return((NSData *)nil));

    QByteArray bkArray = svr.testSecurityBookmark(fileName,false);

    EXPECT_TRUE(bkArray.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,securityBookmarkDocumentFailure)
{
    NSURLBookmarkCreationOptions bkOptions = NSURLBookmarkCreationWithSecurityScope;

    QString docFileName = "/playlist/document/list.pls";
    QString refFileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceSecurityBookmark svr;
    EXPECT_CALL(svr,securityBookmarkFromURL(Eq(docFileName),Eq(refFileName),bkOptions))
        .Times(1).WillOnce(Return((NSData *)nil));

    QByteArray bkArray = svr.testSecurityBookmark(docFileName,refFileName,false);

    EXPECT_TRUE(bkArray.isEmpty());
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceUpdateAccessTimeTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(getDB,SQLiteDatabase *());
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,updateAccessTimeSuccess)
{
    common::TimeStamp accessTime = common::TimeStamp::now();
    tint64 accessTimeI = static_cast<tint64>(static_cast<tuint64>(accessTime));

    QString fileUrl = "file:://path/to/some/\'music\'/track.m4a";
    QString docUrl  = "file:://document/\'playlist\'/list.pls";

    QString cmdU = "UPDATE sandBoxURL SET accessTime=" + QString::number(accessTimeI) + " WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";
    QString startTransaction = "SAVEPOINT updateAccessTime";
    QString endTransaction = "RELEASE updateAccessTime";

    SQLiteDatabaseMock dbMock;
    EXPECT_CALL(dbMock,exec(Eq(startTransaction))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(cmdU))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(endTransaction))).Times(1);

    SandboxBookmarkServiceUpdateAccessTimeTest svr;
    EXPECT_CALL(svr,getDB()).WillRepeatedly(Return(&dbMock));

    svr.testUpdateAccessTime(fileUrl,docUrl,accessTime);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,updateAccessTimeFailsWithDBException)
{
    common::TimeStamp accessTime = common::TimeStamp::now();
    tint64 accessTimeI = static_cast<tint64>(static_cast<tuint64>(accessTime));

    QString fileUrl = "file:://path/to/some/\'music\'/track.m4a";
    QString docUrl  = "file:://document/\'playlist\'/list.pls";

    QString cmdU = "UPDATE sandBoxURL SET accessTime=" + QString::number(accessTimeI) + " WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";
    QString startTransaction = "SAVEPOINT updateAccessTime";
    QString endTransaction = "ROLLBACK TO SAVEPOINT updateAccessTime";

    SQLiteDatabaseMock dbMock;
    EXPECT_CALL(dbMock,exec(Eq(startTransaction))).Times(1);
    EXPECT_CALL(dbMock,exec(Eq(cmdU))).Times(1).WillOnce(Throw(SQLiteException(TrackDB::instance()->db(),"Test")));
    EXPECT_CALL(dbMock,exec(Eq(endTransaction))).Times(1);

    SandboxBookmarkServiceUpdateAccessTimeTest svr;
    EXPECT_CALL(svr,getDB()).WillRepeatedly(Return(&dbMock));

    svr.testUpdateAccessTime(fileUrl,docUrl,accessTime);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,updateAccessTimeIntegration)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };

    QString fileUrl = "file:://path/to/some/music/track.m4a";
    QString docUrl  = "file:://document/playlist/list.pls";
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docUrl,true,bkArray));

    common::TimeStamp accessTime = common::TimeStamp::now() + 100.0;
    tuint64 accessTimeI = static_cast<tuint64>(accessTime);

    svr.testUpdateAccessTime(fileUrl,docUrl,accessTime);

    tint64 testTimeIV;
    SQLiteQuery uQuery(TrackDB::instance()->db());
    QString fUrl,dUrl;
    QString cmdQ = "SELECT url,docUrl,accessTime FROM sandBoxURL WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";
    uQuery.prepare(cmdQ);
    uQuery.bind(fUrl);
    uQuery.bind(dUrl);
    uQuery.bind(testTimeIV);

    EXPECT_TRUE(uQuery.next());

    tuint64 testTimeI = static_cast<tuint64>(testTimeIV);

    EXPECT_TRUE(testTimeI==accessTimeI);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docUrl);
}


//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceAddTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD2(has,bool(const QString& fileName,bool readOnlyFlag));
        MOCK_METHOD2(securityBookmark,QByteArray(const QString& fileName,bool readOnlyFlag));
        MOCK_METHOD4(addToDB,bool(const QString& fileUrl,const QString& docUrl,bool readOnlyFlag,const QByteArray& bkArray));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addEmptyFileName)
{
    QString fileName = "";

    SandboxBookmarkServiceAddTest svr;
    EXPECT_FALSE(svr.add(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addAlreadyHasFile)
{
    QString fileName = "path/to/some/music/track.m4a";

    SandboxBookmarkServiceAddTest svr;
    EXPECT_CALL(svr,has(fileName,true)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.add(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addFailToCreateBookmark)
{
    QString fileName = "path/to/some/music/track.m4a";
    QByteArray bkArray;

    SandboxBookmarkServiceAddTest svr;
    EXPECT_CALL(svr,has(fileName,true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,securityBookmark(fileName,true)).Times(1).WillOnce(Return(bkArray));

    EXPECT_FALSE(svr.add(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addFailAddToDB)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QString fileName = "path/to/some/music/track.m4a";
    QByteArray bkArray((const char *)bkMem,4);

    SandboxBookmarkServiceAddTest svr;
    EXPECT_CALL(svr,has(fileName,false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,securityBookmark(fileName,false)).Times(1).WillOnce(Return(bkArray));

    QString fUrl = svr.testKey(fileName);
    QString dUrl = "";
    EXPECT_CALL(svr,addToDB(Eq(fUrl),Eq(dUrl),false,bkArray)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.add(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addSuccess)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QString fileName = "path/to/some/music/track.m4a";
    QByteArray bkArray((const char *)bkMem,4);

    SandboxBookmarkServiceAddTest svr;
    EXPECT_CALL(svr,has(fileName,true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,securityBookmark(fileName,true)).Times(1).WillOnce(Return(bkArray));

    QString fUrl = svr.testKey(fileName);
    QString dUrl = "";
    EXPECT_CALL(svr,addToDB(Eq(fUrl),Eq(dUrl),true,bkArray)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.add(fileName,true));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceAddDocumentTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD2(addRootDocument,bool(const QString& fileName,bool readOnlyFlag));
        MOCK_METHOD3(has,bool(const QString& docFileName,const QString& refFileName,bool readOnlyFlag));
        MOCK_METHOD3(securityBookmark,QByteArray(const QString& docFileName,const QString& refFileName,bool readOnlyFlag));
        MOCK_METHOD4(addToDB,bool(const QString& fileUrl,const QString& docUrl,bool readOnlyFlag,const QByteArray& bkArray));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addDocumentNoDocumentFileName)
{
    QString docFileName  = "";
    QStringList refFileNames;

    SandboxBookmarkServiceAddDocumentTest svr;

    EXPECT_FALSE(svr.add(docFileName,refFileNames,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addDocumentFailsToAddDocument)
{
    QString docFileName  = "/playlist/documents/list.pls";
    QStringList refFileNames;

    SandboxBookmarkServiceAddDocumentTest svr;
    EXPECT_CALL(svr,addRootDocument(Eq(docFileName),false)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.add(docFileName,refFileNames,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addDocumentEmptyReferenceList)
{
    QString docFileName  = "/playlist/documents/list.pls";
    QStringList refFileNames;

    SandboxBookmarkServiceAddDocumentTest svr;
    EXPECT_CALL(svr,addRootDocument(Eq(docFileName),false)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.add(docFileName,refFileNames,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addDocumentTwoRefBothAlreadyExist)
{
    QString docFileName  = "/playlist/documents/list.pls";
    QString refFileNameA = "/home/music/track1.m4a";
    QString refFileNameB = "/Volumes/LaCie/Music/next.mp3";

    QStringList refFileNames;
    refFileNames.append(refFileNameA);
    refFileNames.append(refFileNameB);

    SandboxBookmarkServiceAddDocumentTest svr;

    EXPECT_CALL(svr,addRootDocument(Eq(docFileName),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,has(Eq(docFileName),Eq(refFileNameA),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,has(Eq(docFileName),Eq(refFileNameB),false)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.add(docFileName,refFileNames,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addDocumentThreeRefWithSecondFailOnBookmark)
{
    tubyte bkMemA[4] = { 0x12, 0x13, 0x14, 0x15 };

    QByteArray bkArrayA((const char *)bkMemA,4);
    QByteArray bkArrayB;

    QString docFileName  = "/playlist/documents/list.pls";
    QString refFileNameA = "/home/music/track1.m4a";
    QString refFileNameB = "/Volumes/LaCie/Music/next.mp3";
    QString refFileNameC = "/path/to/music/file.wav";

    QStringList refFileNames;
    refFileNames.append(refFileNameA);
    refFileNames.append(refFileNameB);
    refFileNames.append(refFileNameC);

    SandboxBookmarkServiceAddDocumentTest svr;

    QString docKey  = svr.testKey(docFileName);
    QString refKeyA = svr.testKey(refFileNameA);

    EXPECT_CALL(svr,addRootDocument(Eq(docFileName),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,has(Eq(docFileName),Eq(refFileNameA),false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,has(Eq(docFileName),Eq(refFileNameB),false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,securityBookmark(Eq(docFileName),Eq(refFileNameA),false))
        .Times(1).WillOnce(Return(bkArrayA));
    EXPECT_CALL(svr,securityBookmark(Eq(docFileName),Eq(refFileNameB),false))
        .Times(1).WillOnce(Return(bkArrayB));
    EXPECT_CALL(svr,addToDB(Eq(refKeyA),Eq(docKey),false,bkArrayA))
        .Times(1).WillOnce(Return(true));

    EXPECT_FALSE(svr.add(docFileName,refFileNames,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addDocumentThreeRefWithSecondFailOnAddToDB)
{
    tubyte bkMemA[4] = { 0x12, 0x13, 0x14, 0x15 };
    tubyte bkMemB[4] = { 0x26, 0x27, 0x28, 0x29 };

    QByteArray bkArrayA((const char *)bkMemA,4);
    QByteArray bkArrayB((const char *)bkMemB,4);

    QString docFileName  = "/playlist/documents/list.pls";
    QString refFileNameA = "/home/music/track1.m4a";
    QString refFileNameB = "/Volumes/LaCie/Music/next.mp3";
    QString refFileNameC = "/path/to/music/file.wav";

    QStringList refFileNames;
    refFileNames.append(refFileNameA);
    refFileNames.append(refFileNameB);
    refFileNames.append(refFileNameC);

    SandboxBookmarkServiceAddDocumentTest svr;

    QString docKey  = svr.testKey(docFileName);
    QString refKeyA = svr.testKey(refFileNameA);
    QString refKeyB = svr.testKey(refFileNameB);

    EXPECT_CALL(svr,addRootDocument(Eq(docFileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,has(Eq(docFileName),Eq(refFileNameA),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,has(Eq(docFileName),Eq(refFileNameB),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,securityBookmark(Eq(docFileName),Eq(refFileNameA),true))
        .Times(1).WillOnce(Return(bkArrayA));
    EXPECT_CALL(svr,securityBookmark(Eq(docFileName),Eq(refFileNameB),true))
        .Times(1).WillOnce(Return(bkArrayB));
    EXPECT_CALL(svr,addToDB(Eq(refKeyA),Eq(docKey),true,bkArrayA))
        .Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,addToDB(Eq(refKeyB),Eq(docKey),true,bkArrayB))
        .Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.add(docFileName,refFileNames,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addDocumentThreeNewSuccessfulReferenceList)
{
    tubyte bkMemA[4] = { 0x12, 0x13, 0x14, 0x15 };
    tubyte bkMemB[4] = { 0x26, 0x27, 0x28, 0x29 };
    tubyte bkMemC[4] = { 0x3a, 0x3b, 0x3c, 0x3d };

    QByteArray bkArrayA((const char *)bkMemA,4);
    QByteArray bkArrayB((const char *)bkMemB,4);
    QByteArray bkArrayC((const char *)bkMemC,4);

    QString docFileName  = "/playlist/documents/list.pls";
    QString refFileNameA = "/home/music/track1.m4a";
    QString refFileNameB = "/Volumes/LaCie/Music/next.mp3";
    QString refFileNameC = "/path/to/music/file.wav";

    QStringList refFileNames;
    refFileNames.append(refFileNameA);
    refFileNames.append(refFileNameB);
    refFileNames.append(refFileNameC);

    SandboxBookmarkServiceAddDocumentTest svr;

    QString docKey  = svr.testKey(docFileName);
    QString refKeyA = svr.testKey(refFileNameA);
    QString refKeyB = svr.testKey(refFileNameB);
    QString refKeyC = svr.testKey(refFileNameC);

    EXPECT_CALL(svr,addRootDocument(Eq(docFileName),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,has(Eq(docFileName),Eq(refFileNameA),false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,has(Eq(docFileName),Eq(refFileNameB),false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,has(Eq(docFileName),Eq(refFileNameC),false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,securityBookmark(Eq(docFileName),Eq(refFileNameA),false))
        .Times(1).WillOnce(Return(bkArrayA));
    EXPECT_CALL(svr,securityBookmark(Eq(docFileName),Eq(refFileNameB),false))
        .Times(1).WillOnce(Return(bkArrayB));
    EXPECT_CALL(svr,securityBookmark(Eq(docFileName),Eq(refFileNameC),false))
        .Times(1).WillOnce(Return(bkArrayC));
    EXPECT_CALL(svr,addToDB(Eq(refKeyA),Eq(docKey),false,bkArrayA))
        .Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,addToDB(Eq(refKeyB),Eq(docKey),false,bkArrayB))
        .Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,addToDB(Eq(refKeyC),Eq(docKey),false,bkArrayC))
        .Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.add(docFileName,refFileNames,false));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceAddDocumentIntegrationTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD2(securityBookmark,QByteArray(const QString& fileName,bool readOnlyFlag));
        MOCK_METHOD3(securityBookmark,QByteArray(const QString& docFileName,const QString& refFileName,bool readOnlyFlag));
};

//-------------------------------------------------------------------------------------------

QByteArray SandboxBookmarkServiceGetBookmarkByQuery(const QString& docKey,const QString& refKey)
{
    QString cmdQ;
    QByteArray bkArray;
    SQLiteQuery bkQuery(TrackDB::instance()->db());

    cmdQ = "SELECT bookmark FROM sandBoxURL WHERE url='" + TrackDB::dbString(refKey) + "' AND docUrl='" + TrackDB::dbString(docKey) + "'";
    bkQuery.prepare(cmdQ);
    bkQuery.bind(bkArray);
    bkQuery.next();
    return bkArray;
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addDocumentThreeNewIntegration)
{
    tubyte bkMemA[4] = { 0x01, 0x02, 0x03, 0x04 };
    tubyte bkMemB[4] = { 0x12, 0x13, 0x14, 0x15 };
    tubyte bkMemC[4] = { 0x26, 0x27, 0x28, 0x29 };
    tubyte bkMemD[4] = { 0x3a, 0x3b, 0x3c, 0x3d };

    QByteArray bkArrayA((const char *)bkMemA,4);
    QByteArray bkArrayB((const char *)bkMemB,4);
    QByteArray bkArrayC((const char *)bkMemC,4);
    QByteArray bkArrayD((const char *)bkMemD,4);

    QString docFileName  = "/playlist/documents/list.pls";
    QString refFileNameA = "/home/music/track1.m4a";
    QString refFileNameB = "/Volumes/LaCie/Music/next.mp3";
    QString refFileNameC = "/path/to/music/file.wav";

    SandboxBookmarkServiceAddDocumentIntegrationTest svr;

    QString docKey  = svr.testKey(docFileName);
    QString refKeyA = svr.testKey(refFileNameA);
    QString refKeyB = svr.testKey(refFileNameB);
    QString refKeyC = svr.testKey(refFileNameC);

    EXPECT_CALL(svr,securityBookmark(docFileName,true))
        .Times(1).WillOnce(Return(bkArrayA));
    EXPECT_CALL(svr,securityBookmark(docFileName,refFileNameA,true))
        .Times(1).WillOnce(Return(bkArrayB));
    EXPECT_CALL(svr,securityBookmark(docFileName,refFileNameB,true))
        .Times(1).WillOnce(Return(bkArrayC));
    EXPECT_CALL(svr,securityBookmark(docFileName,refFileNameC,true))
        .Times(1).WillOnce(Return(bkArrayD));

    QStringList refFileNames;
    refFileNames.append(refFileNameA);
    refFileNames.append(refFileNameB);
    refFileNames.append(refFileNameC);

    EXPECT_TRUE(svr.add(docFileName,refFileNames,true));

    QByteArray testArrayA = SandboxBookmarkServiceGetBookmarkByQuery("",docKey);
    EXPECT_TRUE(testArrayA.size()==bkArrayA.size());
    EXPECT_TRUE(::memcmp(testArrayA.constData(),bkArrayA.constData(),bkArrayA.size())==0);

    QByteArray testArrayB = SandboxBookmarkServiceGetBookmarkByQuery(docKey,refKeyA);
    EXPECT_TRUE(testArrayB.size()==bkArrayB.size());
    EXPECT_TRUE(::memcmp(testArrayB.constData(),bkArrayB.constData(),bkArrayB.size())==0);

    QByteArray testArrayC = SandboxBookmarkServiceGetBookmarkByQuery(docKey,refKeyB);
    EXPECT_TRUE(testArrayC.size()==bkArrayC.size());
    EXPECT_TRUE(::memcmp(testArrayC.constData(),bkArrayC.constData(),bkArrayC.size())==0);

    QByteArray testArrayD = SandboxBookmarkServiceGetBookmarkByQuery(docKey,refKeyC);
    EXPECT_TRUE(testArrayD.size()==bkArrayD.size());
    EXPECT_TRUE(::memcmp(testArrayD.constData(),bkArrayD.constData(),bkArrayD.size())==0);

    SandboxBookmarkServiceTestDeleteURL(docKey);
    SandboxBookmarkServiceTestDeleteURL(refKeyA,docKey);
    SandboxBookmarkServiceTestDeleteURL(refKeyB,docKey);
    SandboxBookmarkServiceTestDeleteURL(refKeyC,docKey);
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceGetAccessMapTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(currentAccessTime,common::TimeStamp());
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getAccessMapWhenNoEntries)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QMap<QString,QPair<bool,common::TimeStamp> > accessMap;

    SandboxBookmarkServiceGetAccessMapTest svr;
    svr.testGetAccessMap(fileName,accessMap);

    EXPECT_TRUE(accessMap.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getAccessMapWhenEntriesBothDocumentAndNonReference)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceGetAccessMapTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";
    QString docNameC = "/document/playlist/listC.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);
    QString docBUrl = svr.testKey(docNameB);
    QString docCUrl = svr.testKey(docNameC);

    common::TimeStamp time1 = common::TimeStamp::now() + 100.0;
    common::TimeStamp time2 = common::TimeStamp::now() + 200.0;
    common::TimeStamp time3 = common::TimeStamp::now() + 300.0;
    common::TimeStamp time4 = common::TimeStamp::now() + 400.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(4)
        .WillOnce(Return(time1))
        .WillOnce(Return(time2))
        .WillOnce(Return(time3))
        .WillOnce(Return(time4));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",true,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docBUrl,true,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docCUrl,false,bkArray));

    QMap<QString,QPair<bool,common::TimeStamp> > accessMap;

    svr.testGetAccessMap(fileName,accessMap);

    EXPECT_TRUE(accessMap.size()==4);

    EXPECT_TRUE(accessMap.find("").value().first);
    EXPECT_TRUE(accessMap.find("").value().second==time1);

    EXPECT_FALSE(accessMap.find(docAUrl).value().first);
    EXPECT_TRUE(accessMap.find(docAUrl).value().second==time2);

    EXPECT_TRUE(accessMap.find(docBUrl).value().first);
    EXPECT_TRUE(accessMap.find(docBUrl).value().second==time3);

    EXPECT_FALSE(accessMap.find(docCUrl).value().first);
    EXPECT_TRUE(accessMap.find(docCUrl).value().second==time4);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docBUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docCUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,canAccessForReadOnlyWithReadOnlyAccess)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testCanAccess(true,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,canAccessForReadWriteWithReadOnlyAccess)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_FALSE(svr.testCanAccess(false,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,canAccessForReadOnlyWithReadWriteAccess)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testCanAccess(true,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,canAccessForReadWriteWithReadWriteAccess)
{
    SandboxBookmarkServiceTest svr;
    EXPECT_TRUE(svr.testCanAccess(false,false));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceAccessDocumentUrlForFileTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(currentAccessTime,common::TimeStamp());
        MOCK_METHOD2(canCheckIn,bool(const QString& fileName,const QString& docFileName));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileWhenNoEntriesInAccessMap)
{
    QString docFileName;
    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,true,false,docFileName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadOnlyEntryAccessForReadOnlyCheckingOut)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",true,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,false,docFileName));

    EXPECT_TRUE(docFileName=="");

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadOnlyEntryAccessForReadOnlyCheckingInCannot)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);
    QString emptyS = "";

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(emptyS))).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",true,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,true,true,docFileName));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadOnlyEntryAccessForReadOnlyCheckingInCan)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);
    QString emptyS;

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(emptyS))).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",true,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,true,docFileName));

    EXPECT_TRUE(docFileName=="");

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadOnlyEntryAccessForReadWriteCheckingOut)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",true,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,false,true,docFileName));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadOnlyEntryAccessForReadWriteCheckingIn)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",true,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,false,false,docFileName));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadWriteEntryAccessForReadOnlyCheckingOut)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,false,docFileName));

    EXPECT_TRUE(docFileName=="");

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadWriteEntryAccessForReadOnlyCheckingInCannot)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);
    QString emptyS;

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(emptyS))).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",false,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,true,true,docFileName));

    EXPECT_TRUE(docFileName=="");

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadWriteEntryAccessForReadOnlyCheckingInCan)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);
    QString emptyS;

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(emptyS))).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,true,docFileName));

    EXPECT_TRUE(docFileName=="");

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadWriteEntryAccessForReadWriteCheckingOut)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,false,false,docFileName));

    EXPECT_TRUE(docFileName=="");

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadWriteEntryAccessForReadWriteCheckingInCannot)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);
    QString emptyS;

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(emptyS))).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",false,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,false,true,docFileName));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadWriteEntryAccessForReadWriteCheckingInCan)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);
    QString emptyS;

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(emptyS))).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,false,true,docFileName));

    EXPECT_TRUE(docFileName=="");

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadOnlyEntryAccessForReadOnlyCheckingOut)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,true,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,false,docFileName));

    EXPECT_TRUE(docFileName==docNameA);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadOnlyEntryAccessForReadOnlyCheckingInCannot)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(docNameA))).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,true,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,true,true,docFileName));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadOnlyEntryAccessForReadOnlyCheckingInCan)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(docNameA))).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,true,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,true,docFileName));

    EXPECT_TRUE(docFileName==docNameA);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadOnlyEntryAccessForReadWriteCheckingIn)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,true,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,false,false,docFileName));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadOnlyEntryAccessForReadWriteCheckingOut)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,true,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,false,true,docFileName));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadWriteEntryAccessForReadOnlyCheckingIn)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,false,docFileName));

    EXPECT_TRUE(docFileName==docNameA);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadWriteEntryAccessForReadOnlyCheckingOutCannot)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(docNameA))).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,true,true,docFileName));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadWriteEntryAccessForReadOnlyCheckingOutCan)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(docNameA))).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,true,docFileName));

    EXPECT_TRUE(docFileName==docNameA);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadWriteEntryAccessForReadWriteCheckingIn)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,false,false,docFileName));

    EXPECT_TRUE(docFileName==docNameA);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadWriteEntryAccessForReadWriteCheckingOutCannot)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(docNameA))).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,false,true,docFileName));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenDocumentReadWriteEntryAccessForReadWriteCheckingOutCan)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(docNameA))).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,false,true,docFileName));

    EXPECT_TRUE(docFileName==docNameA);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadOnlyAndDocumentReadOnlyEntryAccessForReadOnly)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    common::TimeStamp time2 = common::TimeStamp::now() + 400.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(2)
        .WillOnce(Return(time1))
        .WillOnce(Return(time2));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",true,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,true,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,false,docFileName));

    EXPECT_TRUE(docFileName=="");

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadOnlyAndDocumentReadOnlyEntryAccessForReadWrite)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    common::TimeStamp time2 = common::TimeStamp::now() + 400.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(2)
        .WillOnce(Return(time1))
        .WillOnce(Return(time2));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",true,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,true,bkArray));

    QString docFileName;
    EXPECT_FALSE(svr.testAccessDocumentUrlForFile(fileName,false,false,docFileName));

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadOnlyAndDocumentReadWriteEntryAccessForReadOnly)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    common::TimeStamp time2 = common::TimeStamp::now() + 400.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(2)
        .WillOnce(Return(time1))
        .WillOnce(Return(time2));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",true,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,false,docFileName));

    EXPECT_TRUE(docFileName=="");

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenEmptyReadOnlyAndDocumentReadWriteEntryAccessForReadWrite)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    common::TimeStamp time2 = common::TimeStamp::now() + 400.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(2)
        .WillOnce(Return(time1))
        .WillOnce(Return(time2));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,"",true,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,false,false,docFileName));

    EXPECT_TRUE(docFileName==docNameA);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,"");
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenMultipleDocumentsDifferentAccessEntryAccessForReadOnly)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";
    QString docNameC = "/document/playlist/listC.pls";
    QString docNameD = "/document/playlist/listD.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);
    QString docBUrl = svr.testKey(docNameB);
    QString docCUrl = svr.testKey(docNameC);
    QString docDUrl = svr.testKey(docNameD);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    common::TimeStamp time2 = common::TimeStamp::now() + 400.0;
    common::TimeStamp time3 = common::TimeStamp::now() + 500.0;
    common::TimeStamp time4 = common::TimeStamp::now() + 200.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(4)
        .WillOnce(Return(time1))
        .WillOnce(Return(time2))
        .WillOnce(Return(time3))
        .WillOnce(Return(time4));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,true,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docBUrl,false,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docCUrl,true,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docDUrl,false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,true,false,docFileName));

    EXPECT_TRUE(docFileName==docNameC);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docBUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docCUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docDUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenMultipleDocumentsDifferentAccessEntryAccessForReadWrite)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";
    QString docNameC = "/document/playlist/listC.pls";
    QString docNameD = "/document/playlist/listD.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);
    QString docBUrl = svr.testKey(docNameB);
    QString docCUrl = svr.testKey(docNameC);
    QString docDUrl = svr.testKey(docNameD);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    common::TimeStamp time2 = common::TimeStamp::now() + 500.0;
    common::TimeStamp time3 = common::TimeStamp::now() + 200.0;
    common::TimeStamp time4 = common::TimeStamp::now() + 400.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(4)
        .WillOnce(Return(time1))
        .WillOnce(Return(time2))
        .WillOnce(Return(time3))
        .WillOnce(Return(time4));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,true,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docBUrl,false,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docCUrl,true,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docDUrl,false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,false,false,docFileName));

    EXPECT_TRUE(docFileName==docNameB);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docBUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docCUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docDUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,accessDocumentUrlForFileGivenMultipleDocumentsCheckingIn)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";
    QString docNameC = "/document/playlist/listC.pls";
    QString docNameD = "/document/playlist/listD.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);
    QString docBUrl = svr.testKey(docNameB);
    QString docCUrl = svr.testKey(docNameC);
    QString docDUrl = svr.testKey(docNameD);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    common::TimeStamp time2 = common::TimeStamp::now() + 500.0;
    common::TimeStamp time3 = common::TimeStamp::now() + 200.0;
    common::TimeStamp time4 = common::TimeStamp::now() + 400.0;

    EXPECT_CALL(svr,currentAccessTime()).Times(4)
        .WillOnce(Return(time1))
        .WillOnce(Return(time2))
        .WillOnce(Return(time3))
        .WillOnce(Return(time4));

    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(docNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(docNameB))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,canCheckIn(Eq(fileName),Eq(docNameD))).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docBUrl,false,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docCUrl,false,bkArray));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docDUrl,false,bkArray));

    QString docFileName;
    EXPECT_TRUE(svr.testAccessDocumentUrlForFile(fileName,false,true,docFileName));

    EXPECT_TRUE(docFileName==docNameD);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docBUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docCUrl);
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docDUrl);
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceIsBookmarkCreationRequiredTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(getReferenceCountMap,QMap<QString,QMap<QString,QPair<void *,int> > >&());
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkCreationRequiredNoReferenceForFile)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    SandboxBookmarkServiceIsBookmarkCreationRequiredTest svr;
    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_TRUE(svr.testIsBookmarkCreationRequired(fileName,""));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkCreationRequiredReferenceToDocumentsAccessViaEmpty)
{
    SandboxBookmarkServiceIsBookmarkCreationRequiredTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);
    QString docBUrl = svr.testKey(docNameB);

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docAUrl,QPair<void *,int>(0,1));
    rMap.insert(docBUrl,QPair<void *,int>(0,2));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_TRUE(svr.testIsBookmarkCreationRequired(fileName,""));

    EXPECT_TRUE(referenceMap.find(fileUrl).value().size()==2);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkCreationRequiredReferenceToDocumentsAccessViaNewDocument)
{
    SandboxBookmarkServiceIsBookmarkCreationRequiredTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";
    QString docNameC = "/document/playlist/listC.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);
    QString docBUrl = svr.testKey(docNameB);

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docAUrl,QPair<void *,int>(0,1));
    rMap.insert(docBUrl,QPair<void *,int>(0,2));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_TRUE(svr.testIsBookmarkCreationRequired(fileName,docNameC));

    EXPECT_TRUE(referenceMap.find(fileUrl).value().size()==2);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkCreationRequiredReferenceToDocumentsAccessViaReference)
{
    SandboxBookmarkServiceIsBookmarkCreationRequiredTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);
    QString docBUrl = svr.testKey(docNameB);

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docAUrl,QPair<void *,int>(0,1));
    rMap.insert(docBUrl,QPair<void *,int>(0,2));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_FALSE(svr.testIsBookmarkCreationRequired(fileName,docNameB));

    EXPECT_TRUE(referenceMap.find(fileUrl).value().find(docBUrl).value().second==3);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkCreationRequiredReferenceToEmptyAccessViaEmpty)
{
    SandboxBookmarkServiceIsBookmarkCreationRequiredTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString fileUrl = svr.testKey(fileName);
    QString empty;

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(empty,QPair<void *,int>(0,2));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_FALSE(svr.testIsBookmarkCreationRequired(fileName,""));

    EXPECT_TRUE(referenceMap.find(fileUrl).value().find(empty).value().second==3);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkCreationRequiredReferenceToEmptyAccessViaNewDocument)
{
    SandboxBookmarkServiceIsBookmarkCreationRequiredTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString empty;

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(empty,QPair<void *,int>(0,2));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_TRUE(svr.testIsBookmarkCreationRequired(fileName,docNameA));

    EXPECT_TRUE(referenceMap.find(fileUrl).value().size()==1);
}

//-------------------------------------------------------------------------------------------

common::TimeStamp SandboxBookmarkServiceQueryAccessTime(const QString& fileUrl,const QString& docUrl)
{
    tint64 tI;
    QString cmdQ;
    common::TimeStamp t;
    SQLiteQuery timeQuery(TrackDB::instance()->db());

    cmdQ = "SELECT accessTime FROM sandBoxURL WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docUrl) + "'";
    timeQuery.prepare(cmdQ);
    timeQuery.bind(tI);
    if(timeQuery.next())
    {
        t = static_cast<tuint64>(tI);
    }
    return t;
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceGetBookmarkFromReferenceTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(currentAccessTime,common::TimeStamp());
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getBookmarkFromReferenceNoDocumentReferenceDoesNotExist)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "";

    SandboxBookmarkServiceGetBookmarkFromReferenceTest svr;

    NSData *testData = svr.testGetBookmarkFromReference(fileName,docNameA);

    EXPECT_TRUE(testData==nil);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getBookmarkFromReferenceNoDocumentReferenceDoesExist)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    common::TimeStamp time1 = common::TimeStamp::now() - 1000.0;
    SandboxBookmarkServiceGetBookmarkFromReferenceTest svr;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    NSData *testData = svr.testGetBookmarkFromReference(fileName,docNameA);

    EXPECT_TRUE(testData!=nil);
    EXPECT_TRUE([testData length] == 4);
    EXPECT_TRUE(::memcmp(bkMem,[testData bytes],4)==0);

    common::TimeStamp aTime = SandboxBookmarkServiceQueryAccessTime(fileUrl,docAUrl);
    common::TimeStamp cLower = common::TimeStamp::now() - 1.0;
    common::TimeStamp cUpper = common::TimeStamp::now() + 1.0;
    EXPECT_TRUE(cLower < aTime && aTime < cUpper);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getBookmarkFromDocumentReferenceEntryDoesNotExist)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    SandboxBookmarkServiceGetBookmarkFromReferenceTest svr;

    NSData *testData = svr.testGetBookmarkFromReference(fileName,docNameA);

    EXPECT_TRUE(testData==nil);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getBookmarkFromDocumentReferenceEntryDoesExist)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    common::TimeStamp time1 = common::TimeStamp::now();
    SandboxBookmarkServiceGetBookmarkFromReferenceTest svr;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    NSData *testData = svr.testGetBookmarkFromReference(fileName,docNameA);

    EXPECT_TRUE(testData!=nil);
    EXPECT_TRUE([testData length] == 4);
    EXPECT_TRUE(::memcmp(bkMem,[testData bytes],4)==0);

    common::TimeStamp aTime = SandboxBookmarkServiceQueryAccessTime(fileUrl,docAUrl);
    common::TimeStamp cLower = common::TimeStamp::now() - 1.0;
    common::TimeStamp cUpper = common::TimeStamp::now() + 1.0;
    EXPECT_TRUE(cLower < aTime && aTime < cUpper);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceGetBookmarkFromReferenceDBExceptionTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(createDBQuery,SQLiteQuery*());
        MOCK_METHOD1(removeDBQuery,void(SQLiteQuery *pQuery));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getBookmarkFromReferenceGivenDatabaseException)
{
    SandboxBookmarkServiceGetBookmarkFromReferenceDBExceptionTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    QString cmdQ = "SELECT bookmark FROM sandBoxURL WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docAUrl) + "'";

    SQLiteQueryMock query(TrackDB::instance()->db());
    EXPECT_CALL(query,prepare(cmdQ)).Times(1);
    EXPECT_CALL(query,bind(A<QByteArray&>())).Times(1);
    EXPECT_CALL(query,next()).Times(1).WillOnce(Throw(SQLiteException(TrackDB::instance()->db(),"Test")));

    EXPECT_CALL(svr,createDBQuery()).Times(1).WillOnce(Return(&query));
    EXPECT_CALL(svr,removeDBQuery(&query)).Times(1);

    NSData *testData = svr.testGetBookmarkFromReference(fileName,docNameA);

    EXPECT_TRUE(testData==nil);
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceAddSecuredResourceToReferenceMapTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(getReferenceCountMap,QMap<QString,QMap<QString,QPair<void *,int> > >&());
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addSecuredResourceToReferenceMapNoReferenceWithStandaloneFile)
{
    SandboxBookmarkServiceAddSecuredResourceToReferenceMapTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = "";

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    svr.testAddSecuredResourceToReferenceMap(fileName,docNameA,url);

    EXPECT_TRUE(referenceMap.size()==1);
    EXPECT_TRUE(referenceMap.find(fileUrl)!=referenceMap.end());

    QMap<QString,QPair<void *,int> >& tMap = referenceMap.find(fileUrl).value();
    EXPECT_TRUE(tMap.size()==1);
    EXPECT_TRUE(tMap.find(docAUrl)!=tMap.end());
    EXPECT_TRUE(tMap.find(docAUrl).value().first == (void *)(url));
    EXPECT_TRUE(tMap.find(docAUrl).value().second==1);

    [url release];
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addSecuredResourceToReferenceMapNoReferenceWithDocumentAssociatedFile)
{
    SandboxBookmarkServiceAddSecuredResourceToReferenceMapTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    svr.testAddSecuredResourceToReferenceMap(fileName,docNameA,url);

    EXPECT_TRUE(referenceMap.size()==1);
    EXPECT_TRUE(referenceMap.find(fileUrl)!=referenceMap.end());

    QMap<QString,QPair<void *,int> >& tMap = referenceMap.find(fileUrl).value();
    EXPECT_TRUE(tMap.size()==1);
    EXPECT_TRUE(tMap.find(docAUrl)!=tMap.end());
    EXPECT_TRUE(tMap.find(docAUrl).value().first == (void *)(url));
    EXPECT_TRUE(tMap.find(docAUrl).value().second==1);

    [url release];
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addSecuredResourceToReferenceMapExistingReferenceWithStandaloneFile)
{
    SandboxBookmarkServiceAddSecuredResourceToReferenceMapTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "";
    QString docNameB = "/document/playlist/listB.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = "";
    QString docBUrl = svr.testKey(docNameB);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docBUrl,QPair<void *,int>(0,3));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    svr.testAddSecuredResourceToReferenceMap(fileName,docNameA,url);

    EXPECT_TRUE(referenceMap.size()==1);
    EXPECT_TRUE(referenceMap.find(fileUrl)!=referenceMap.end());

    QMap<QString,QPair<void *,int> >& tMap = referenceMap.find(fileUrl).value();
    EXPECT_TRUE(tMap.size()==2);
    EXPECT_TRUE(tMap.find(docAUrl)!=tMap.end());
    EXPECT_TRUE(tMap.find(docAUrl).value().first == (void *)(url));
    EXPECT_TRUE(tMap.find(docAUrl).value().second==1);

    EXPECT_TRUE(tMap.find(docBUrl)!=tMap.end());
    EXPECT_TRUE(tMap.find(docBUrl).value().second==3);

    [url release];
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,addSecuredResourceToReferenceMapExistingReferenceWithDocumentAssociatedFile)
{
    SandboxBookmarkServiceAddSecuredResourceToReferenceMapTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);
    QString docBUrl = svr.testKey(docNameB);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docBUrl,QPair<void *,int>(0,3));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    svr.testAddSecuredResourceToReferenceMap(fileName,docNameA,url);

    EXPECT_TRUE(referenceMap.size()==1);
    EXPECT_TRUE(referenceMap.find(fileUrl)!=referenceMap.end());

    QMap<QString,QPair<void *,int> >& tMap = referenceMap.find(fileUrl).value();
    EXPECT_TRUE(tMap.size()==2);
    EXPECT_TRUE(tMap.find(docAUrl)!=tMap.end());
    EXPECT_TRUE(tMap.find(docAUrl).value().first == (void *)(url));
    EXPECT_TRUE(tMap.find(docAUrl).value().second==1);

    EXPECT_TRUE(tMap.find(docBUrl)!=tMap.end());
    EXPECT_TRUE(tMap.find(docBUrl).value().second==3);

    [url release];
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceCheckOutSecuredResourceTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD2(resolveSecurityScopedBookmarkData,NSURL *(NSData *bkData,const QString& docFileName));
        MOCK_METHOD1(startAccessingSecuredResource,bool(NSURL *url));
        MOCK_METHOD3(addSecuredResourceToReferenceMap,void(const QString& fileName,const QString& docFileName,NSURL *bookmark));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutSecuredResourceWhenFailToGetBookmark)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    NSData *bkData = [NSData dataWithBytes:(const char *)bkMem length:4];

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutSecuredResourceTest svr;
    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(bkData,Eq(docName))).Times(1).WillOnce(Return((NSURL *)nil));

    EXPECT_FALSE(svr.testCheckOutSecuredResource(fileName,docName,bkData));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutSecuredResourceWhenFailToStartAccessToResource)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    NSData *bkData = [NSData dataWithBytes:(const char *)bkMem length:4];

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutSecuredResourceTest svr;

    QString fileUrl = svr.testKey(fileName);
    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(bkData,Eq(docName))).Times(1).WillOnce(Return(url));
    EXPECT_CALL(svr,startAccessingSecuredResource(url)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.testCheckOutSecuredResource(fileName,docName,bkData));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutSecuredResourceWhenSuccessful)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    NSData *bkData = [NSData dataWithBytes:(const char *)bkMem length:4];

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutSecuredResourceTest svr;

    QString fileUrl = svr.testKey(fileName);
    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(bkData,Eq(docName))).Times(1).WillOnce(Return(url));
    EXPECT_CALL(svr,startAccessingSecuredResource(url)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,addSecuredResourceToReferenceMap(Eq(fileName),Eq(docName),url)).Times(1);

    EXPECT_TRUE(svr.testCheckOutSecuredResource(fileName,docName,bkData));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceDeleteBookmarkTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(getDB,SQLiteDatabase *());
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,deleteBookmarkSuccess)
{
    QString cmdA,cmdB,cmdC;
    SandboxBookmarkServiceDeleteBookmarkTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    cmdA = "SAVEPOINT deleteBookmark";
    cmdB = "DELETE FROM sandBoxUrl WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docAUrl) + "'";
    cmdC = "RELEASE deleteBookmark";

    SQLiteDatabaseMock db;
    EXPECT_CALL(db,exec(cmdA)).Times(1);
    EXPECT_CALL(db,exec(cmdB)).Times(1);
    EXPECT_CALL(db,exec(cmdC)).Times(1);

    EXPECT_CALL(svr,getDB()).WillRepeatedly(Return(&db));

    svr.testDeleteBookmark(fileName,docNameA);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,deleteBookmarkDBExceptionThrown)
{
    QString cmdA,cmdB,cmdC;
    SandboxBookmarkServiceDeleteBookmarkTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    cmdA = "SAVEPOINT deleteBookmark";
    cmdB = "DELETE FROM sandBoxUrl WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docAUrl) + "'";
    cmdC = "ROLLBACK TO SAVEPOINT deleteBookmark";

    SQLiteDatabaseMock db;
    EXPECT_CALL(db,exec(cmdA)).Times(1);
    EXPECT_CALL(db,exec(cmdB)).Times(1)
        .WillOnce(Throw(SQLiteException(TrackDB::instance()->db(),"Test")));;
    EXPECT_CALL(db,exec(cmdC)).Times(1);

    EXPECT_CALL(svr,getDB()).WillRepeatedly(Return(&db));

    svr.testDeleteBookmark(fileName,docNameA);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,deleteBookmarkIntegration)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    SandboxBookmarkServiceAccessDocumentUrlForFileTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::TimeStamp time1 = common::TimeStamp::now() + 300.0;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    svr.testDeleteBookmark(fileName,docNameA);

    int count;
    QString cmdQ;
    SQLiteQuery recordQ(TrackDB::instance()->db());

    cmdQ = "SELECT count(*) FROM sandBoxUrl WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docAUrl) + "'";
    recordQ.prepare(cmdQ);
    recordQ.bind(count);

    EXPECT_TRUE(recordQ.next());
    EXPECT_TRUE(count==0);
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceIsBookmarkRequiredTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD1(isMusicDirectory,bool(const QString& fileName));
        MOCK_METHOD1(isPlaylist,bool(const QString& fileName));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkRequiredGivenNonPlaylistInMusicDirectoryReadOnly)
{
    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceIsBookmarkRequiredTest svr;
    EXPECT_CALL(svr,isMusicDirectory(Eq(fileName))).WillRepeatedly(Return(true));
    EXPECT_CALL(svr,isPlaylist(Eq(fileName))).WillRepeatedly(Return(false));

    EXPECT_FALSE(svr.testIsBookmarkRequired(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkRequiredGivenPlaylistInMusicDirectoryReadOnly)
{
    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceIsBookmarkRequiredTest svr;
    EXPECT_CALL(svr,isMusicDirectory(Eq(fileName))).WillRepeatedly(Return(true));
    EXPECT_CALL(svr,isPlaylist(Eq(fileName))).WillRepeatedly(Return(true));

    EXPECT_TRUE(svr.testIsBookmarkRequired(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkRequiredGivenNotInMusicDirectoryReadOnly)
{
    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceIsBookmarkRequiredTest svr;
    EXPECT_CALL(svr,isMusicDirectory(Eq(fileName))).WillRepeatedly(Return(false));

    EXPECT_TRUE(svr.testIsBookmarkRequired(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkRequiredGivenNonPlaylistInMusicDirectoryReadWrite)
{
    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceIsBookmarkRequiredTest svr;
    EXPECT_CALL(svr,isMusicDirectory(Eq(fileName))).WillRepeatedly(Return(true));
    EXPECT_CALL(svr,isPlaylist(Eq(fileName))).WillRepeatedly(Return(false));

    EXPECT_TRUE(svr.testIsBookmarkRequired(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkRequiredGivenPlaylistInMusicDirectoryReadWrite)
{
    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceIsBookmarkRequiredTest svr;
    EXPECT_CALL(svr,isMusicDirectory(Eq(fileName))).WillRepeatedly(Return(true));
    EXPECT_CALL(svr,isPlaylist(Eq(fileName))).WillRepeatedly(Return(true));

    EXPECT_TRUE(svr.testIsBookmarkRequired(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkRequiredGivenNotInMusicDirectoryReadWrite)
{
    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceIsBookmarkRequiredTest svr;
    EXPECT_CALL(svr,isMusicDirectory(Eq(fileName))).WillRepeatedly(Return(false));

    EXPECT_TRUE(svr.testIsBookmarkRequired(fileName,false));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceCheckOutBookmarkResource : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD2(getBookmarkFromReference,NSData *(const QString& fileName,const QString& docFileName));
        MOCK_METHOD3(checkOutSecuredResource,bool(const QString& fileName,const QString& docFileName,NSData *bookmarkData));
        MOCK_METHOD2(deleteBookmark,void(const QString& fileName,const QString& docFileName));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutBookmarkResourceFailToGetBookmark)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutBookmarkResource svr;
    EXPECT_CALL(svr,getBookmarkFromReference(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return((NSData *)nil));
    EXPECT_CALL(svr,deleteBookmark(Eq(fileName),Eq(docName))).Times(1);

    EXPECT_FALSE(svr.testCheckOutBookmarkResource(fileName,docName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutBookmarkResourceFailToCheckOutResource)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    NSData *bkData = [NSData dataWithBytes:(const char *)bkMem length:4];

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutBookmarkResource svr;
    EXPECT_CALL(svr,getBookmarkFromReference(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return(bkData));
    EXPECT_CALL(svr,checkOutSecuredResource(Eq(fileName),Eq(docName),bkData)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,deleteBookmark(Eq(fileName),Eq(docName))).Times(1);

    EXPECT_FALSE(svr.testCheckOutBookmarkResource(fileName,docName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutBookmarkResourceSuccess)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    NSData *bkData = [NSData dataWithBytes:(const char *)bkMem length:4];

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutBookmarkResource svr;
    EXPECT_CALL(svr,getBookmarkFromReference(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return(bkData));
    EXPECT_CALL(svr,checkOutSecuredResource(Eq(fileName),Eq(docName),bkData)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,deleteBookmark(Eq(fileName),Eq(docName))).Times(1);

    EXPECT_FALSE(svr.testCheckOutBookmarkResource(fileName,docName));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceCheckOutTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(lock,void());
        MOCK_METHOD0(unlock,void());
        MOCK_METHOD2(isBookmarkRequired,bool(const QString& fileName,bool readOnlyFlag));
        MOCK_METHOD4(accessDocumentUrlForFile,bool(const QString& fileName,bool readOnlyFlag,bool checkInFlag,QString& docFileName));
        MOCK_METHOD2(isBookmarkCreationRequired,bool(const QString& fileName,const QString& docFileName));
        MOCK_METHOD2(checkOutRecursive,bool(const QString& fileName,bool readOnlyFlag));
        MOCK_METHOD2(checkOutBookmarkResource,bool(const QString& fileName,const QString& docFileName));
        MOCK_METHOD2(checkOutParentDirectory,bool(const QString& fileName,bool readOnlyFlag));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutBookmarkNotRequired)
{
    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(svr.checkOut(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutNoResourceToAccessAndNoParentDirectoryToCheckOut)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "";

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),true,false,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(false)));
    EXPECT_CALL(svr,checkOutParentDirectory(Eq(fileName),true)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.checkOut(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutNoResourceToAccessParentDirectoryCheckOut)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "";

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),false,false,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(false)));
    EXPECT_CALL(svr,checkOutParentDirectory(Eq(fileName),false)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.checkOut(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutNoBookmarkRequiredAndNoDocumentReference)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "";

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),true,false,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(true)));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,isBookmarkCreationRequired(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_TRUE(svr.checkOut(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutNoBookmarkRequiredAndDocumentReferenceSuccessToCheckout)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),true,false,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(true)));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,isBookmarkCreationRequired(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,checkOutRecursive(Eq(docName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_TRUE(svr.checkOut(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutNoBookmarkRequiredAndDocumentReferenceFailsToCheckout)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),true,false,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(true)));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,isBookmarkCreationRequired(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,checkOutRecursive(Eq(docName),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_FALSE(svr.checkOut(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutBookmarkRequiredAndNoDocumentReferenceWithBookmarkCheckoutFailure)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName;

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),true,false,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(true)));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,isBookmarkCreationRequired(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkOutBookmarkResource(fileName,docName)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_FALSE(svr.checkOut(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutBookmarkRequiredAndNoDocumentReferenceWithBookmarkCheckoutSuccess)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName;

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),true,false,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(true)));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,isBookmarkCreationRequired(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkOutBookmarkResource(fileName,docName)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_TRUE(svr.checkOut(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutBookmarkRequiredAndDocumentReferenceFailsToCheckout)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),true,false,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(true)));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,isBookmarkCreationRequired(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkOutRecursive(Eq(docName),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_FALSE(svr.checkOut(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutBookmarkRequiredAndDocumentReferenceWithBookmarkCheckoutFailure)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),true,false,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(true)));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,isBookmarkCreationRequired(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkOutRecursive(Eq(docName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkOutBookmarkResource(fileName,docName)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_FALSE(svr.checkOut(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutBookmarkRequiredAndDocumentReferenceWithBookmarkCheckoutSuccess)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckOutTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),true,false,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(true)));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,isBookmarkCreationRequired(Eq(fileName),Eq(docName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkOutRecursive(Eq(docName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkOutBookmarkResource(fileName,docName)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_TRUE(svr.checkOut(fileName,true));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceCheckOutIntegrationTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(currentAccessTime,common::TimeStamp());
        MOCK_METHOD0(getReferenceCountMap,QMap<QString,QMap<QString,QPair<void *,int> > >&());
        MOCK_METHOD2(resolveSecurityScopedBookmarkData,NSURL *(NSData *bkData,const QString& docFileName));
        MOCK_METHOD1(startAccessingSecuredResource,bool(NSURL *url));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutIntegrationWithNoDocumentReference)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    common::TimeStamp time1 = common::TimeStamp::now();
    SandboxBookmarkServiceCheckOutIntegrationTest svr;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));
    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(A<NSData *>(),Eq(docNameA))).Times(1).WillOnce(Return(url));
    EXPECT_CALL(svr,startAccessingSecuredResource(url)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.checkOut(fileName,true));

    EXPECT_TRUE(referenceMap.size()==1);
    EXPECT_TRUE(referenceMap.find(fileUrl)!=referenceMap.end());

    QMap<QString,QPair<void *,int> >& rMap = referenceMap.find(fileUrl).value();
    EXPECT_TRUE(rMap.find(docAUrl).value().first == (void *)(url));
    EXPECT_TRUE(rMap.find(docAUrl).value().second == 1);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutIntegrationWithDocumentReference)
{
    const tubyte bkMemA[4] = { 0x01, 0x02, 0x03, 0x04 };
    const tubyte bkMemB[4] = { 0x11, 0x12, 0x13, 0x14 };
    QByteArray bkArrayA(reinterpret_cast<const char *>(bkMemA),4);
    QByteArray bkArrayB(reinterpret_cast<const char *>(bkMemB),4);

    common::TimeStamp time1 = common::TimeStamp::now();
    SandboxBookmarkServiceCheckOutIntegrationTest svr;
    EXPECT_CALL(svr,currentAccessTime()).Times(2).WillRepeatedly(Return(time1));

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString emptyName;

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::BString urlStrA(fileUrl.toUtf8().constData());
    NSURL *urlA = [NSURL URLWithString:[NSString stringWithUTF8String:(urlStrA.getString())]];
    common::BString urlStrB(docNameA.toUtf8().constData());
    NSURL *urlB = [NSURL URLWithString:[NSString stringWithUTF8String:(urlStrB.getString())]];

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    EXPECT_TRUE(svr.testAddToDB(docAUrl,"",false,bkArrayA));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArrayB));

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));
    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(A<NSData *>(),Eq(docNameA))).Times(1).WillOnce(Return(urlA));
    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(A<NSData *>(),Eq(emptyName))).Times(1).WillOnce(Return(urlB));
    EXPECT_CALL(svr,startAccessingSecuredResource(urlA)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,startAccessingSecuredResource(urlB)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.checkOut(fileName,true));

    EXPECT_TRUE(referenceMap.size()==2);
    EXPECT_TRUE(referenceMap.find(fileUrl)!=referenceMap.end());
    EXPECT_TRUE(referenceMap.find(docAUrl)!=referenceMap.end());

    QMap<QString,QPair<void *,int> >& rMap = referenceMap.find(fileUrl).value();
    EXPECT_TRUE(rMap.find(docAUrl).value().first == (void *)(urlA));
    EXPECT_TRUE(rMap.find(docAUrl).value().second == 1);

    QMap<QString,QPair<void *,int> >& sMap = referenceMap.find(docAUrl).value();
    EXPECT_TRUE(sMap.find(emptyName).value().first == (void *)(urlB));
    EXPECT_TRUE(sMap.find(emptyName).value().second == 1);

    SandboxBookmarkServiceTestDeleteURL(docAUrl,"");
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutIntegrationWithParentDirectory)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    common::TimeStamp time1 = common::TimeStamp::now();
    SandboxBookmarkServiceCheckOutIntegrationTest svr;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    QString fileName = "/path/to/some/music/track.m4a";
    QString parentDirName = "/path/to/some";

    QString fileUrl = svr.testKey(fileName);
    QString parentDirUrl = svr.testKey(parentDirName);
    QString emptyDocUrl;

    common::BString urlD(parentDirUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    EXPECT_TRUE(svr.testAddToDB(parentDirUrl,emptyDocUrl,true,bkArray));

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));
    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(A<NSData *>(),Eq(emptyDocUrl))).Times(1).WillOnce(Return(url));
    EXPECT_CALL(svr,startAccessingSecuredResource(url)).Times(1).WillOnce(Return(true));

    ASSERT_TRUE(svr.checkOut(fileName,true));

    EXPECT_TRUE(referenceMap.size()==1);
    EXPECT_TRUE(referenceMap.find(parentDirUrl)!=referenceMap.end());

    QMap<QString,QPair<void *,int> >& rMap = referenceMap.find(parentDirUrl).value();
    EXPECT_TRUE(rMap.find(emptyDocUrl).value().first == (void *)(url));
    EXPECT_TRUE(rMap.find(emptyDocUrl).value().second == 1);

    SandboxBookmarkServiceTestDeleteURL(parentDirUrl,emptyDocUrl);
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceCheckInDocumentAsRequiredTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD2(checkIn,bool(const QString& fileName,bool readOnlyFlag));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInDocumentAsRequiredGivenNoDocument)
{
    QString docName = "";
    SandboxBookmarkServiceCheckInDocumentAsRequiredTest svr;
    EXPECT_TRUE(svr.testCheckInDocumentAsRequired(docName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInDocumentAsRequiredGivenCheckInFails)
{
    QString docName = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckInDocumentAsRequiredTest svr;
    EXPECT_CALL(svr,checkIn(Eq(docName),false)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.testCheckInDocumentAsRequired(docName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInDocumentAsRequiredGivenCheckInSuccessful)
{
    QString docName = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckInDocumentAsRequiredTest svr;
    EXPECT_CALL(svr,checkIn(Eq(docName),true)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testCheckInDocumentAsRequired(docName,true));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceDoCheckInTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(getReferenceCountMap,QMap<QString,QMap<QString,QPair<void *,int> > >&());
        MOCK_METHOD2(checkInDocumentAsRequired,bool(const QString& docFileName,bool readOnlyFlag));
        MOCK_METHOD1(stopScopedResource,void(NSURL *url));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,doCheckInGivenNoReferenceCountEntryFoundForFile)
{
    SandboxBookmarkServiceDoCheckInTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_FALSE(svr.testDoCheckIn(fileName,docNameA,true));

    EXPECT_TRUE(referenceMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,doCheckInGivenNoReferenceCountEntryFoundForDocumentFile)
{
    SandboxBookmarkServiceDoCheckInTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docUrl  = svr.testKey(docNameA);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docUrl,QPair<void *,int>((void *)url,2));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_FALSE(svr.testDoCheckIn(fileName,docNameB,true));

    EXPECT_TRUE(referenceMap.size()==1);
    EXPECT_TRUE(referenceMap.find(fileUrl)!=referenceMap.end());

    QMap<QString,QPair<void *,int> >& tMap = referenceMap.find(fileUrl).value();
    EXPECT_TRUE(tMap.size()==1);
    EXPECT_TRUE(tMap.find(docUrl).value().first==(void *)url);
    EXPECT_TRUE(tMap.find(docUrl).value().second==2);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,doCheckInGivenCountEntryReferencesExistingCheckouts)
{
    SandboxBookmarkServiceDoCheckInTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docUrl  = svr.testKey(docName);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docUrl,QPair<void *,int>((void *)url,2));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));
    EXPECT_CALL(svr,checkInDocumentAsRequired(docName,true)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testDoCheckIn(fileName,docName,true));

    EXPECT_TRUE(referenceMap.size()==1);
    EXPECT_TRUE(referenceMap.find(fileUrl)!=referenceMap.end());

    QMap<QString,QPair<void *,int> >& tMap = referenceMap.find(fileUrl).value();
    EXPECT_TRUE(tMap.size()==1);
    EXPECT_TRUE(tMap.find(docUrl).value().first==(void *)url);
    EXPECT_TRUE(tMap.find(docUrl).value().second==1);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,doCheckInGivenCountRemoveResourceAndDocumentCheckInFailsAndOnlyMapEntry)
{
    SandboxBookmarkServiceDoCheckInTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docUrl  = svr.testKey(docName);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docUrl,QPair<void *,int>((void *)url,1));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));
    EXPECT_CALL(svr,stopScopedResource(url)).Times(1);
    EXPECT_CALL(svr,checkInDocumentAsRequired(docName,false)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.testDoCheckIn(fileName,docName,false));

    EXPECT_TRUE(referenceMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,doCheckInGivenCountRemoveResourceAndDocumentCheckInSuceedsAndOnlyMapEntry)
{
    SandboxBookmarkServiceDoCheckInTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docUrl  = svr.testKey(docName);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docUrl,QPair<void *,int>((void *)url,1));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));
    EXPECT_CALL(svr,stopScopedResource(url)).Times(1);
    EXPECT_CALL(svr,checkInDocumentAsRequired(docName,true)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testDoCheckIn(fileName,docName,true));

    EXPECT_TRUE(referenceMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,doCheckInGivenCountRemoveResourceAndDocumentCheckInSuceedsAndNotOnlyMapEntry)
{
    SandboxBookmarkServiceDoCheckInTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docUrlA = svr.testKey(docNameA);
    QString docUrlB = svr.testKey(docNameB);

    common::BString urlNameA(docUrlA.toUtf8().constData());
    NSURL *urlA = [NSURL URLWithString:[NSString stringWithUTF8String:(urlNameA.getString())]];
    common::BString urlNameB(docUrlA.toUtf8().constData());
    NSURL *urlB = [NSURL URLWithString:[NSString stringWithUTF8String:(urlNameB.getString())]];

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docUrlA,QPair<void *,int>((void *)urlA,1));
    rMap.insert(docUrlB,QPair<void *,int>((void *)urlB,1));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));
    EXPECT_CALL(svr,stopScopedResource(urlA)).Times(1);
    EXPECT_CALL(svr,checkInDocumentAsRequired(docNameA,true)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testDoCheckIn(fileName,docNameA,true));

    EXPECT_TRUE(referenceMap.size()==1);
    EXPECT_TRUE(referenceMap.find(fileUrl)!=referenceMap.end());

    QMap<QString,QPair<void *,int> >& tMap = referenceMap.find(fileUrl).value();
    EXPECT_TRUE(tMap.size()==1);
    EXPECT_TRUE(tMap.find(docUrlA)==tMap.end());
    EXPECT_TRUE(tMap.find(docUrlB)!=tMap.end());

    EXPECT_TRUE(tMap.find(docUrlB).value().first==(void *)urlB);
    EXPECT_TRUE(tMap.find(docUrlB).value().second==1);
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceCanCheckInTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(getReferenceCountMap,QMap<QString,QMap<QString,QPair<void *,int> > >&());
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,canCheckInEmptyReferenceMap)
{
    SandboxBookmarkServiceCanCheckInTest svr;

    QString fileNameA = "/path/to/some/music/trackA.m4a";
    QString docNameA  = "/document/playlist/listA.pls";

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_FALSE(svr.testCanCheckIn(fileNameA,docNameA));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,canCheckInEntriesInReferenceMapButNotForFile)
{
    SandboxBookmarkServiceCanCheckInTest svr;

    QString fileNameA = "/path/to/some/music/trackA.m4a";
    QString fileNameB = "/path/to/some/music/trackB.m4a";
    QString fileNameC = "/path/to/some/music/trackC.m4a";
    QString docNameA  = "/document/playlist/listA.pls";
    QString docNameB  = "/document/playlist/listB.pls";

    QString fileUrlA = svr.testKey(fileNameA);
    QString fileUrlB = svr.testKey(fileNameB);
    QString docUrlA  = svr.testKey(docNameA);
    QString docUrlB  = svr.testKey(docNameB);

    common::BString urlNameA(docUrlA.toUtf8().constData());
    NSURL *urlA = [NSURL URLWithString:[NSString stringWithUTF8String:(urlNameA.getString())]];
    common::BString urlNameB(docUrlA.toUtf8().constData());
    NSURL *urlB = [NSURL URLWithString:[NSString stringWithUTF8String:(urlNameB.getString())]];

    QMap<QString,QPair<void *,int> > rMap,sMap;
    rMap.insert(docUrlA,QPair<void *,int>((void *)urlA,1));
    sMap.insert(docUrlB,QPair<void *,int>((void *)urlB,1));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrlA,rMap);
    referenceMap.insert(fileUrlB,sMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_TRUE(svr.testCanCheckIn(fileNameA,docNameA));
    EXPECT_TRUE(svr.testCanCheckIn(fileNameB,docNameB));

    EXPECT_FALSE(svr.testCanCheckIn(fileNameC,docNameA));
    EXPECT_FALSE(svr.testCanCheckIn(fileNameC,docNameB));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,canCheckInEntryForFileInReferenceMapButNotForGivenDocument)
{
    SandboxBookmarkServiceCanCheckInTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";
    QString docNameB = "/document/playlist/listB.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docUrl  = svr.testKey(docName);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docUrl,QPair<void *,int>((void *)url,1));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_FALSE(svr.testCanCheckIn(fileName,docNameB));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,canCheckInDocumentForGivenEmptyDocument)
{
    SandboxBookmarkServiceCanCheckInTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "";

    QString fileUrl = svr.testKey(fileName);
    QString docUrl  = svr.testKey(docName);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docUrl,QPair<void *,int>((void *)url,1));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_TRUE(svr.testCanCheckIn(fileName,docName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,canCheckInDocumentForGivenDocument)
{
    SandboxBookmarkServiceCanCheckInTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docUrl  = svr.testKey(docName);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QPair<void *,int> > rMap;
    rMap.insert(docUrl,QPair<void *,int>((void *)url,1));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrl,rMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    EXPECT_TRUE(svr.testCanCheckIn(fileName,docName));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceCheckInTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(lock,void());
        MOCK_METHOD0(unlock,void());
        MOCK_METHOD2(isBookmarkRequired,bool(const QString& fileName,bool readOnlyFlag));
        MOCK_METHOD4(accessDocumentUrlForFile,bool(const QString& fileName,bool readOnlyFlag,bool checkInFlag,QString& docFileName));
        MOCK_METHOD3(doCheckIn,bool(const QString& fileName,const QString& docFileName,bool readOnlyFlag));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInWhenBookmarkNotRequired)
{
    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceCheckInTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(svr.checkIn(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInWhenCannotAccessDocumentUrl)
{
    QString fileName = "/path/to/some/music/track.m4a";

    SandboxBookmarkServiceCheckInTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),false,true,A<QString&>())).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_FALSE(svr.checkIn(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInWhenCannotDoCheckIn)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckInTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),false,true,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(true)));
    EXPECT_CALL(svr,doCheckIn(Eq(fileName),Eq(docName),false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_FALSE(svr.checkIn(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInWhenCanDoCheckIn)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docName  = "/document/playlist/listA.pls";

    SandboxBookmarkServiceCheckInTest svr;
    EXPECT_CALL(svr,isBookmarkRequired(Eq(fileName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,lock()).Times(1);
    EXPECT_CALL(svr,accessDocumentUrlForFile(Eq(fileName),true,true,A<QString&>())).Times(1).WillOnce(DoAll(SetArgReferee<3>(docName),Return(true)));
    EXPECT_CALL(svr,doCheckIn(Eq(fileName),Eq(docName),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,unlock()).Times(1);

    EXPECT_TRUE(svr.checkIn(fileName,true));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceCheckInIntegrationTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(currentAccessTime,common::TimeStamp());
        MOCK_METHOD0(getReferenceCountMap,QMap<QString,QMap<QString,QPair<void *,int> > >&());
        MOCK_METHOD2(resolveSecurityScopedBookmarkData,NSURL *(NSData *bkData,const QString& docFileName));
        MOCK_METHOD1(startAccessingSecuredResource,bool(NSURL *url));
        MOCK_METHOD1(stopScopedResource,void(NSURL *url));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutAndInIntegrationWithNoDocumentReference)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    common::TimeStamp time1 = common::TimeStamp::now();
    SandboxBookmarkServiceCheckInIntegrationTest svr;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::BString urlD(fileUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));
    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(A<NSData *>(),Eq(docNameA))).Times(1).WillOnce(Return(url));
    EXPECT_CALL(svr,startAccessingSecuredResource(url)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,stopScopedResource(url)).Times(1);

    EXPECT_TRUE(svr.checkOut(fileName,true));
    EXPECT_TRUE(svr.checkIn(fileName,true));

    EXPECT_TRUE(referenceMap.size()==0);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutAndInIntegrationWithDocumentReference)
{
    const tubyte bkMemA[4] = { 0x01, 0x02, 0x03, 0x04 };
    const tubyte bkMemB[4] = { 0x11, 0x12, 0x13, 0x14 };
    QByteArray bkArrayA(reinterpret_cast<const char *>(bkMemA),4);
    QByteArray bkArrayB(reinterpret_cast<const char *>(bkMemB),4);

    common::TimeStamp time1 = common::TimeStamp::now();
    SandboxBookmarkServiceCheckInIntegrationTest svr;
    EXPECT_CALL(svr,currentAccessTime()).Times(2).WillRepeatedly(Return(time1));

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";
    QString emptyName;

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::BString urlStrA(fileUrl.toUtf8().constData());
    NSURL *urlA = [NSURL URLWithString:[NSString stringWithUTF8String:(urlStrA.getString())]];
    common::BString urlStrB(docNameA.toUtf8().constData());
    NSURL *urlB = [NSURL URLWithString:[NSString stringWithUTF8String:(urlStrB.getString())]];

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    EXPECT_TRUE(svr.testAddToDB(docAUrl,"",false,bkArrayA));
    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArrayB));

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));
    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(A<NSData *>(),Eq(docNameA))).Times(1).WillOnce(Return(urlA));
    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(A<NSData *>(),Eq(emptyName))).Times(1).WillOnce(Return(urlB));
    EXPECT_CALL(svr,startAccessingSecuredResource(urlA)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,startAccessingSecuredResource(urlB)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,stopScopedResource(urlA)).Times(1);
    EXPECT_CALL(svr,stopScopedResource(urlB)).Times(1);

    EXPECT_TRUE(svr.checkOut(fileName,true));
    EXPECT_TRUE(svr.checkIn(fileName,true));

    EXPECT_TRUE(referenceMap.size()==0);

    SandboxBookmarkServiceTestDeleteURL(docAUrl,"");
    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutAndInIntegrationWithParentDirectoryBookmark)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    common::TimeStamp time1 = common::TimeStamp::now();
    SandboxBookmarkServiceCheckInIntegrationTest svr;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    QString parentDirName = "/path/to/some";
    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "";

    QString parentDirUrl = svr.testKey(parentDirName);
    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    common::BString urlD(parentDirUrl.toUtf8().constData());
    NSURL *url = [NSURL URLWithString:[NSString stringWithUTF8String:(urlD.getString())]];

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;

    EXPECT_TRUE(svr.testAddToDB(parentDirUrl,docAUrl,false,bkArray));

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));
    EXPECT_CALL(svr,resolveSecurityScopedBookmarkData(A<NSData *>(),Eq(docNameA))).Times(1).WillOnce(Return(url));
    EXPECT_CALL(svr,startAccessingSecuredResource(url)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,stopScopedResource(url)).Times(1);

    EXPECT_TRUE(svr.checkOut(fileName,true));
    EXPECT_TRUE(svr.checkIn(fileName,true));

    EXPECT_TRUE(referenceMap.size()==0);

    SandboxBookmarkServiceTestDeleteURL(parentDirUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

class SBBookmarkServiceGetBookmarkArrayTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(currentAccessTime,common::TimeStamp());
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getBookmarkArrayNoDocumentReferenceDoesNotExist)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "";

    SBBookmarkServiceGetBookmarkArrayTest svr;

    QByteArray testData = svr.getBookmarkArray(docNameA,fileName);

    EXPECT_TRUE(testData.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getBookmarkArrayNoDocumentReferenceDoesExist)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    common::TimeStamp time1 = common::TimeStamp::now();
    SBBookmarkServiceGetBookmarkArrayTest svr;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    QByteArray testData = svr.getBookmarkArray(docNameA,fileName);

    EXPECT_TRUE(testData.size()==4);
    EXPECT_TRUE(::memcmp(bkMem,testData.constData(),4)==0);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getBookmarkArrayFromDocumentReferenceEntryDoesNotExist)
{
    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    SBBookmarkServiceGetBookmarkArrayTest svr;

    QByteArray testData = svr.getBookmarkArray(docNameA,fileName);

    EXPECT_TRUE(testData.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getBookmarkArrayFromDocumentReferenceEntryDoesExist)
{
    const tubyte bkMem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray bkArray(reinterpret_cast<const char *>(bkMem),4);

    common::TimeStamp time1 = common::TimeStamp::now();
    SBBookmarkServiceGetBookmarkArrayTest svr;
    EXPECT_CALL(svr,currentAccessTime()).Times(1).WillOnce(Return(time1));

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    EXPECT_TRUE(svr.testAddToDB(fileUrl,docAUrl,false,bkArray));

    QByteArray testData = svr.getBookmarkArray(docNameA,fileName);

    EXPECT_TRUE(testData.size()==4);
    EXPECT_TRUE(::memcmp(bkMem,testData.constData(),4)==0);

    SandboxBookmarkServiceTestDeleteURL(fileUrl,docAUrl);
}

//-------------------------------------------------------------------------------------------

class SBBookmarkServiceGetBookmarkArrayDBExceptionTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(createDBQuery,SQLiteQuery*());
        MOCK_METHOD1(removeDBQuery,void(SQLiteQuery *pQuery));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,getBookmarkArrayGivenDatabaseException)
{
    SBBookmarkServiceGetBookmarkArrayDBExceptionTest svr;

    QString fileName = "/path/to/some/music/track.m4a";
    QString docNameA = "/document/playlist/listA.pls";

    QString fileUrl = svr.testKey(fileName);
    QString docAUrl = svr.testKey(docNameA);

    QString cmdQ = "SELECT bookmark FROM sandBoxURL WHERE url='" + TrackDB::dbString(fileUrl) + "' AND docUrl='" + TrackDB::dbString(docAUrl) + "'";

    SQLiteQueryMock query(TrackDB::instance()->db());
    EXPECT_CALL(query,prepare(cmdQ)).Times(1);
    EXPECT_CALL(query,bind(A<QByteArray&>())).Times(1);
    EXPECT_CALL(query,next()).Times(1).WillOnce(Throw(SQLiteException(TrackDB::instance()->db(),"Test")));

    EXPECT_CALL(svr,createDBQuery()).Times(1).WillOnce(Return(&query));
    EXPECT_CALL(svr,removeDBQuery(&query)).Times(1);

    QByteArray testData = svr.getBookmarkArray(docNameA,fileName);

    EXPECT_TRUE(testData.isEmpty());
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceShutdownTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD0(getReferenceCountMap,QMap<QString,QMap<QString,QPair<void *,int> > >& ());
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,shutdownWithNoContent)
{
    SandboxBookmarkServiceShutdownTest svr;
    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    svr.testShutdown();

    EXPECT_TRUE(referenceMap.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,shutdownWithMultipleFilesAndDocuments)
{
    SandboxBookmarkServiceShutdownTest svr;

    QString fileNameA = "/path/to/some/music/trackA.m4a";
    QString fileNameB = "/path/to/some/music/trackB.m4a";
    QString fileNameC = "/path/to/some/music/trackC.m4a";
    QString docNameA  = "/document/playlist/listA.pls";
    QString docNameB  = "/document/playlist/listB.pls";
    QString docNameC  = "/document/playlist/listC.pls";
    QString docNameD  = "/document/playlist/listD.pls";

    QString fileUrlA = svr.testKey(fileNameA);
    QString fileUrlB = svr.testKey(fileNameB);
    QString fileUrlC = svr.testKey(fileNameC);
    QString docUrlA  = svr.testKey(docNameA);
    QString docUrlB  = svr.testKey(docNameB);
    QString docUrlC  = svr.testKey(docNameC);
    QString docUrlD  = svr.testKey(docNameD);

    common::BString urlNameA(docUrlA.toUtf8().constData());
    NSURL *urlA = [NSURL URLWithString:[NSString stringWithUTF8String:(urlNameA.getString())]];
    [urlA retain];

    common::BString urlNameB(docUrlB.toUtf8().constData());
    NSURL *urlB = [NSURL URLWithString:[NSString stringWithUTF8String:(urlNameB.getString())]];
    [urlB retain];

    common::BString urlNameC(docUrlC.toUtf8().constData());
    NSURL *urlC = [NSURL URLWithString:[NSString stringWithUTF8String:(urlNameC.getString())]];
    [urlC retain];

    common::BString urlNameD(docUrlD.toUtf8().constData());
    NSURL *urlD = [NSURL URLWithString:[NSString stringWithUTF8String:(urlNameD.getString())]];
    [urlD retain];

    QMap<QString,QPair<void *,int> > aMap,bMap,cMap;
    aMap.insert(docUrlA,QPair<void *,int>((void *)urlA,1));
    bMap.insert(docUrlB,QPair<void *,int>((void *)urlB,2));
    cMap.insert(docUrlC,QPair<void *,int>((void *)urlC,3));
    cMap.insert(docUrlD,QPair<void *,int>((void *)urlD,4));

    QMap<QString,QMap<QString,QPair<void *,int> > > referenceMap;
    referenceMap.insert(fileUrlA,aMap);
    referenceMap.insert(fileUrlB,bMap);
    referenceMap.insert(fileUrlC,cMap);

    EXPECT_CALL(svr,getReferenceCountMap()).WillRepeatedly(ReturnRef(referenceMap));

    svr.testShutdown();

    EXPECT_TRUE(referenceMap.size()==0);
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceCheckOutParentDirectoryTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD2(hasURL,bool(const QString& key,bool readOnlyFlag));
        MOCK_METHOD2(checkOutRecursive,bool(const QString& fileName,bool readOnlyFlag));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutParentDirectoryGivenNoURLBookmarkIsFound)
{
    SandboxBookmarkServiceCheckOutParentDirectoryTest svr;

    QString fileName = "/Users/bonez/Music/Groups/A/Artist/track01.mp3";
    QString dirNameA = "/Users/bonez/Music/Groups/A/Artist";
    QString dirNameB = "/Users/bonez/Music/Groups/A";
    QString dirNameC = "/Users/bonez/Music/Groups";
    QString dirNameD = "/Users/bonez/Music";
    QString dirNameE = "/Users/bonez";

    QString dirUrlA = svr.testKey(dirNameA);
    QString dirUrlB = svr.testKey(dirNameB);
    QString dirUrlC = svr.testKey(dirNameC);
    QString dirUrlD = svr.testKey(dirNameD);
    QString dirUrlE = svr.testKey(dirNameE);

    EXPECT_CALL(svr,hasURL(Eq(dirUrlA),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlB),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlC),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlD),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlE),true)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.testCheckOutParentDirectory(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutParentDirectoryGivenURLFoundButUnableToCheckOut)
{
    SandboxBookmarkServiceCheckOutParentDirectoryTest svr;

    QString fileName = "/Users/bonez/Music/Groups/A/Artist/track01.mp3";
    QString dirNameA = "/Users/bonez/Music/Groups/A/Artist";
    QString dirNameB = "/Users/bonez/Music/Groups/A";
    QString dirNameC = "/Users/bonez/Music/Groups";

    QString dirUrlA = svr.testKey(dirNameA);
    QString dirUrlB = svr.testKey(dirNameB);
    QString dirUrlC = svr.testKey(dirNameC);

    EXPECT_CALL(svr,hasURL(Eq(dirUrlA),false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlB),false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlC),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkOutRecursive(Eq(dirNameC),false)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.testCheckOutParentDirectory(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkOutParentDirectoryGivenURLFoundAndCheckOutSuccessful)
{
    SandboxBookmarkServiceCheckOutParentDirectoryTest svr;

    QString fileName = "/Users/bonez/Music/Groups/A/Artist/track01.mp3";
    QString dirNameA = "/Users/bonez/Music/Groups/A/Artist";

    QString dirUrlA = svr.testKey(dirNameA);

    EXPECT_CALL(svr,hasURL(Eq(dirUrlA),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkOutRecursive(Eq(dirNameA),false)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testCheckOutParentDirectory(fileName,false));
}

//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceCheckInParentDirectoryTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_METHOD2(hasURL,bool(const QString& key,bool readOnlyFlag));
        MOCK_METHOD2(canCheckIn,bool(const QString& fileName,const QString& docFileName));
        MOCK_METHOD2(checkIn,bool(const QString& fileName,bool readOnlyFlag));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInParentDirectoryGivenNoURLBookmarks)
{
    SandboxBookmarkServiceCheckInParentDirectoryTest svr;

    QString fileName = "/Users/bonez/Music/Groups/A/Artist/track01.mp3";
    QString dirNameA = "/Users/bonez/Music/Groups/A/Artist";
    QString dirNameB = "/Users/bonez/Music/Groups/A";
    QString dirNameC = "/Users/bonez/Music/Groups";
    QString dirNameD = "/Users/bonez/Music";
    QString dirNameE = "/Users/bonez";

    QString dirUrlA = svr.testKey(dirNameA);
    QString dirUrlB = svr.testKey(dirNameB);
    QString dirUrlC = svr.testKey(dirNameC);
    QString dirUrlD = svr.testKey(dirNameD);
    QString dirUrlE = svr.testKey(dirNameE);

    EXPECT_CALL(svr,hasURL(Eq(dirUrlA),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlB),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlC),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlD),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlE),true)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.testCheckInParentDirectory(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInParentDirectoryGivenSomeURLBookmarksButNoneCheckouted)
{
    SandboxBookmarkServiceCheckInParentDirectoryTest svr;

    QString fileName = "/Users/bonez/Music/Groups/A/Artist/track01.mp3";
    QString dirNameA = "/Users/bonez/Music/Groups/A/Artist";
    QString dirNameB = "/Users/bonez/Music/Groups/A";
    QString dirNameC = "/Users/bonez/Music/Groups";
    QString dirNameD = "/Users/bonez/Music";
    QString dirNameE = "/Users/bonez";
    QString docName;

    QString dirUrlA = svr.testKey(dirNameA);
    QString dirUrlB = svr.testKey(dirNameB);
    QString dirUrlC = svr.testKey(dirNameC);
    QString dirUrlD = svr.testKey(dirNameD);
    QString dirUrlE = svr.testKey(dirNameE);

    EXPECT_CALL(svr,hasURL(Eq(dirUrlA),false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlB),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlC),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlD),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlE),false)).Times(1).WillOnce(Return(false));

    EXPECT_CALL(svr,canCheckIn(Eq(dirNameB),Eq(docName))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,canCheckIn(Eq(dirNameC),Eq(docName))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,canCheckIn(Eq(dirNameD),Eq(docName))).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.testCheckInParentDirectory(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInParentDirectoryGivenCheckoutBookmarkButCheckInFails)
{
    SandboxBookmarkServiceCheckInParentDirectoryTest svr;

    QString fileName = "/Users/bonez/Music/Groups/A/Artist/track01.mp3";
    QString dirNameA = "/Users/bonez/Music/Groups/A/Artist";
    QString dirNameB = "/Users/bonez/Music/Groups/A";
    QString docName;

    QString dirUrlA = svr.testKey(dirNameA);
    QString dirUrlB = svr.testKey(dirNameB);

    EXPECT_CALL(svr,hasURL(Eq(dirUrlA),false)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlB),false)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,canCheckIn(Eq(dirNameB),Eq(docName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkIn(Eq(dirNameB),false)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(svr.testCheckInParentDirectory(fileName,false));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,checkInParentDirectoryGivenCheckoutBookmarkAndCheckInSucess)
{
    SandboxBookmarkServiceCheckInParentDirectoryTest svr;

    QString fileName = "/Users/bonez/Music/Groups/A/Artist/track01.mp3";
    QString dirNameA = "/Users/bonez/Music/Groups/A/Artist";
    QString dirNameB = "/Users/bonez/Music/Groups/A";
    QString docName;

    QString dirUrlA = svr.testKey(dirNameA);
    QString dirUrlB = svr.testKey(dirNameB);

    EXPECT_CALL(svr,hasURL(Eq(dirUrlA),true)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,hasURL(Eq(dirUrlB),true)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,canCheckIn(Eq(dirNameB),Eq(docName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(svr,checkIn(Eq(dirNameB),true)).Times(1).WillOnce(Return(true));

    EXPECT_TRUE(svr.testCheckInParentDirectory(fileName,true));
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MAC_STORE)
//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isInAppContainerGivenEmptyFileName)
{
    QString fileName;
    SandboxBookmarkService svr;
    EXPECT_FALSE(svr.isInAppContainer(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isInAppContainerGivenFileInDirectoryUserHome)
{
    QString fileName = common::DiskOps::mergeName(track::info::SBBookmarkService::getHomeDirectory(),"Music").trimmed();
    SandboxBookmarkService svr;
    EXPECT_FALSE(svr.isInAppContainer(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isInAppContainerGivenFileIsContainerDirectory)
{
    NSString *homeDir = NSHomeDirectory();
    QString fileName = QString::fromUtf8([homeDir UTF8String]);
    SandboxBookmarkService svr;
    EXPECT_TRUE(svr.isInAppContainer(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isInAppContainerGivenFileIsInRootOfContainerDirectory)
{
    NSString *homeDir = NSHomeDirectory();
    QString fileName = common::DiskOps::mergeName(QString::fromUtf8([homeDir UTF8String]),"file.txt").trimmed();
    SandboxBookmarkService svr;
    EXPECT_TRUE(svr.isInAppContainer(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isInAppContainerGivenFileIsInSubDirectoryOfContainerDirectory)
{
    NSString *homeDir = NSHomeDirectory();
    QString fileName = common::DiskOps::mergeName(QString::fromUtf8([homeDir UTF8String]),"/music/temp/file.txt").trimmed();
    SandboxBookmarkService svr;
    EXPECT_TRUE(svr.isInAppContainer(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isInAppContainerGivenFileIsContainerTempDirectory)
{
    QString fileName = common::SBService::tempDirectory();
    SandboxBookmarkService svr;
    EXPECT_TRUE(svr.isInAppContainer(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isInAppContainerGivenFileIsInRootOfContainerTempDirectory)
{
    QString fileName = common::DiskOps::mergeName(common::SBService::tempDirectory(),"file.txt").trimmed();
    SandboxBookmarkService svr;
    EXPECT_TRUE(svr.isInAppContainer(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isInAppContainerGivenFileIsInSubDirectoryOfContainerTempDirectory)
{
    NSString *homeDir = NSHomeDirectory();
    QString fileName = common::DiskOps::mergeName(common::SBService::tempDirectory(),"/music/temp/file.txt").trimmed();
    SandboxBookmarkService svr;
    EXPECT_TRUE(svr.isInAppContainer(fileName));
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

class SandboxBookmarkServiceIsBookmarkRequiredAppTest : public SandboxBookmarkServiceTest
{
    public:
        MOCK_CONST_METHOD1(isInAppContainer,bool(const QString& fileName));
        MOCK_METHOD1(isMusicDirectory,bool(const QString& fileName));
        MOCK_METHOD1(isPlaylist,bool(const QString& fileName));
};

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkRequiredWhenNoInAppContainer)
{
    QString fileName = "/Users/bonez/test/file.txt";

    SandboxBookmarkServiceIsBookmarkRequiredAppTest svr;
    EXPECT_CALL(svr,isInAppContainer(Eq(fileName))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(svr,isMusicDirectory(Eq(fileName))).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(svr.testIsBookmarkRequired(fileName,true));
}

//-------------------------------------------------------------------------------------------

TEST(SandboxBookmarkService,isBookmarkRequiredWhenInAppContainer)
{
    QString fileName = "/Users/bonez/test/file.txt";

    SandboxBookmarkServiceIsBookmarkRequiredAppTest svr;
    EXPECT_CALL(svr,isInAppContainer(Eq(fileName))).Times(1).WillOnce(Return(true));

    EXPECT_FALSE(svr.testIsBookmarkRequired(fileName,true));
}

//-------------------------------------------------------------------------------------------
