#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/model/inc/AlbumModelUtilities.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "track/db/test/SQLiteMock.h"
#include "track/model/test/ImageRepositaryMock.h"

using namespace testing;
using namespace omega::track::model;
using namespace omega::track;

//-------------------------------------------------------------------------------------------

class AlbumModelUtilitiesTest : public AlbumModelUtilities
{
    public:
        AlbumModelUtilitiesTest();
        AlbumModelUtilitiesTest(db::SQLiteQuerySPtr pQueryMock);
        AlbumModelUtilitiesTest(const AlbumModelKey& albumID);
        AlbumModelUtilitiesTest(const AlbumModelKey& albumID,const TrackModelKey& key);

        QString testGetArtistTypeString(int artistType) const;
        QString testGetQueryForAlbumArtist() const;
        QString testGetQueryPartForAlbumArtist(int artistType,const AlbumModelKey& key) const;
        QString testGetQueryConditionForFilter() const;

        QueryRecord testCreateRecordAlbumArtist(const AlbumModelKey& key,const QString& artist,int artistCount) const;
        AlbumModelKey testKeyAlbumArtistRecord(const QueryRecord& record) const;
        QString testArtistAlbumArtistRecord(const QueryRecord& record) const;
        int testCountAlbumArtistRecord(const QueryRecord& record) const;

        bool testRunAlbumArtistQuery(const QString& cmdQ,QueryResult& results) const;

        QString testPrimaryArtist(const QueryResult& results) const;

        QString testGetQueryForAlbumImage() const;

        QueryRecord testCreateRecordAlbumImage(const AlbumModelKey& key,int imageID,int imageCount,int imageType) const;
        AlbumModelKey testKeyAlbumImageRecord(const QueryRecord& record) const;
        int testImageIDAlbumImageRecord(const QueryRecord& record) const;
        int testImageCountAlbumImageRecord(const QueryRecord& record) const;
        omega::track::info::IDTagImageType testImageTypeAlbumImageRecord(const QueryRecord& record) const;

        bool testRunAlbumImageQuery(const QString& cmdQ,QueryResult& results) const;

        virtual db::SQLiteQuerySPtr getDBQuery() const;

        static bool testPreferedImageType(const info::IDTagImageType& a,const info::IDTagImageType& b);
        static bool testCompareImageResults(const QueryRecord& a,const QueryRecord& b);

        void testSortImageListByPreferredType(QueryResult& results) const;

        QString testGetQueryForYear() const;

    protected:

        db::SQLiteQuerySPtr m_queryMock;
};

//-------------------------------------------------------------------------------------------

AlbumModelUtilitiesTest::AlbumModelUtilitiesTest() : AlbumModelUtilities()
{}

//-------------------------------------------------------------------------------------------

AlbumModelUtilitiesTest::AlbumModelUtilitiesTest(db::SQLiteQuerySPtr pQueryMock) : AlbumModelUtilities(),
    m_queryMock(pQueryMock)
{}

//-------------------------------------------------------------------------------------------

AlbumModelUtilitiesTest::AlbumModelUtilitiesTest(const AlbumModelKey& albumID) : AlbumModelUtilities(albumID)
{}

//-------------------------------------------------------------------------------------------

AlbumModelUtilitiesTest::AlbumModelUtilitiesTest(const AlbumModelKey& albumID,const TrackModelKey& key) : AlbumModelUtilities(albumID,key)
{}

//-------------------------------------------------------------------------------------------

db::SQLiteQuerySPtr AlbumModelUtilitiesTest::getDBQuery() const
{
    return m_queryMock;
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilitiesTest::testGetArtistTypeString(int artistType) const
{
    return getArtistTypeString(artistType);
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilitiesTest::testGetQueryForAlbumArtist() const
{
    return getQueryForAlbumArtist();
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilitiesTest::testGetQueryPartForAlbumArtist(int artistType,const AlbumModelKey& key) const
{
    return getQueryPartForAlbumArtist(artistType,key);
}

//-------------------------------------------------------------------------------------------

QueryRecord AlbumModelUtilitiesTest::testCreateRecordAlbumArtist(const AlbumModelKey& key,const QString& artist,int artistCount) const
{
    return createRecordAlbumArtist(key,artist,artistCount);
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilitiesTest::testGetQueryConditionForFilter() const
{
    return getQueryConditionForFilter();
}

//-------------------------------------------------------------------------------------------

AlbumModelKey AlbumModelUtilitiesTest::testKeyAlbumArtistRecord(const QueryRecord& record) const
{
    return keyAlbumArtistRecord(record);
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilitiesTest::testArtistAlbumArtistRecord(const QueryRecord& record) const
{
    return artistAlbumArtistRecord(record);
}

//-------------------------------------------------------------------------------------------

int AlbumModelUtilitiesTest::testCountAlbumArtistRecord(const QueryRecord& record) const
{
    return countAlbumArtistRecord(record);
}

//-------------------------------------------------------------------------------------------

bool AlbumModelUtilitiesTest::testRunAlbumArtistQuery(const QString& cmdQ,QueryResult& results) const
{
    return runAlbumArtistQuery(cmdQ,results);
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilitiesTest::testPrimaryArtist(const QueryResult& results) const
{
    return primaryArtist(results);
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilitiesTest::testGetQueryForAlbumImage() const
{
    return getQueryForAlbumImage();
}

//-------------------------------------------------------------------------------------------

QueryRecord AlbumModelUtilitiesTest::testCreateRecordAlbumImage(const AlbumModelKey& key,int imageID,int imageCount,int imageType) const
{
    return createRecordAlbumImage(key,imageID,imageCount,imageType);
}

//-------------------------------------------------------------------------------------------

AlbumModelKey AlbumModelUtilitiesTest::testKeyAlbumImageRecord(const QueryRecord& record) const
{
    return keyAlbumImageRecord(record);
}

//-------------------------------------------------------------------------------------------

int AlbumModelUtilitiesTest::testImageIDAlbumImageRecord(const QueryRecord& record) const
{
    return imageIDAlbumImageRecord(record);
}

//-------------------------------------------------------------------------------------------

int AlbumModelUtilitiesTest::testImageCountAlbumImageRecord(const QueryRecord& record) const
{
    return imageCountAlbumImageRecord(record);
}

//-------------------------------------------------------------------------------------------

omega::track::info::IDTagImageType AlbumModelUtilitiesTest::testImageTypeAlbumImageRecord(const QueryRecord& record) const
{
    return imageTypeAlbumImageRecord(record);
}

//-------------------------------------------------------------------------------------------

bool AlbumModelUtilitiesTest::testRunAlbumImageQuery(const QString& cmdQ,QueryResult& results) const
{
    return runAlbumImageQuery(cmdQ,results);
}

//-------------------------------------------------------------------------------------------

bool AlbumModelUtilitiesTest::testPreferedImageType(const info::IDTagImageType& a,const info::IDTagImageType& b)
{
    return preferedImageType(a,b);
}

//-------------------------------------------------------------------------------------------

bool AlbumModelUtilitiesTest::testCompareImageResults(const QueryRecord& a,const QueryRecord& b)
{
    return compareImageResults(a,b);
}

//-------------------------------------------------------------------------------------------

void AlbumModelUtilitiesTest::testSortImageListByPreferredType(QueryResult& results) const
{
    sortImageListByPreferredType(results);
}

//-------------------------------------------------------------------------------------------

QString AlbumModelUtilitiesTest::testGetQueryForYear() const
{
    return getQueryForYear();
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,constructorDefault)
{
    AlbumModelUtilities a;
    AlbumModelKey b;
    EXPECT_TRUE(a.id()==b);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,constructorAlbumID)
{
    AlbumModelKey keyA(std::pair<bool,int>(true,2));
    AlbumModelKey keyB(std::pair<bool,int>(true,2));
    AlbumModelUtilities a(keyA);
    EXPECT_TRUE(a.id()==keyB);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getArtistTypeString)
{
    AlbumModelUtilitiesTest a;
    EXPECT_TRUE(a.testGetArtistTypeString(0)=="artist");
    EXPECT_TRUE(a.testGetArtistTypeString(1)=="originalArtist");
    EXPECT_TRUE(a.testGetArtistTypeString(2)=="composer");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryPartForAlbumArtistNoAlbumKey)
{
    // SELECT 0 AS groupFlag, b.albumID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //     FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //     WHERE b.groupID=-1 AND a.artist NOT LIKE ''
    //     GROUP BY b.albumID, a.artist
    //   UNION
    // SELECT 1 AS groupFlag, b.groupID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //     FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //     WHERE b.groupID>=0 AND a.artist NOT LIKE ''
    //     GROUP BY b.groupID, a.artist

    QString expectSQL;

    expectSQL  = "SELECT 0 AS groupFlag, b.albumID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount";
    expectSQL += "    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID";
    expectSQL += "    WHERE b.groupID=-1 AND a.artist NOT LIKE ''";
    expectSQL += "    GROUP BY b.albumID, a.artist";
    expectSQL += "  UNION ";
    expectSQL += "SELECT 1 AS groupFlag, b.groupID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount";
    expectSQL += "    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID";
    expectSQL += "    WHERE b.groupID>=0 AND a.artist NOT LIKE ''";
    expectSQL += "    GROUP BY b.groupID, a.artist";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelUtilitiesTest a;
    AlbumModelKey keyA(std::pair<bool,int>(false,-1));
    QString testSQLA = a.testGetQueryPartForAlbumArtist(0,keyA);
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryPartForAlbumArtistForAlbum)
{
    // SELECT 0 AS groupFlag, b.albumID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //    WHERE b.albumID=2 AND a.artist NOT LIKE ''
    //    GROUP BY b.albumID, a.artist

    QString expectSQL;

    expectSQL  = "SELECT 0 AS groupFlag, b.albumID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount";
    expectSQL += "    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID";
    expectSQL += "    WHERE b.albumID=2 AND a.artist NOT LIKE ''";
    expectSQL += "    GROUP BY b.albumID, a.artist";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelUtilitiesTest a;
    QString testSQLA = a.testGetQueryPartForAlbumArtist(0,AlbumModelKey(std::pair<bool,int>(false,2)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryPartForAlbumArtistForGroup)
{
    // SELECT 1 AS groupFlag, b.groupID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //    WHERE b.groupID=15 AND a.artist NOT LIKE ''
    //    GROUP BY b.groupID, a.artist

    QString expectSQL;

    expectSQL  = "SELECT 1 AS groupFlag, b.groupID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount";
    expectSQL += "   FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID";
    expectSQL += "   WHERE b.groupID=15 AND a.artist NOT LIKE ''";
    expectSQL += "   GROUP BY b.groupID, a.artist";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelUtilitiesTest a;
    QString testSQLA = a.testGetQueryPartForAlbumArtist(0,AlbumModelKey(std::pair<bool,int>(true,15)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

bool testUtilitySQLAlbumModelUtilitiesAlbumArtist(const QString& cmd,int resultID)
{
    bool res = false;
    db::TrackDB *trackDB = db::TrackDB::instance();
    TrackDBTestEnviroment *resultDB = TrackDBTestEnviroment::instance();
    QVector<QVector<QVariant> > results;

    if(trackDB==0)
    {
        return false;
    }

    try
    {
        bool isGroupFlag;
        int albumID,artistCount;
        QString artist;
        db::SQLiteQuery dbQ(trackDB->db());

        dbQ.prepare(cmd);
        dbQ.bind(isGroupFlag);
        dbQ.bind(albumID);
        dbQ.bind(artist);
        dbQ.bind(artistCount);
        while(dbQ.next())
        {
            QVector<QVariant> record;
            record.push_back((isGroupFlag) ? 1 : 0);
            record.push_back(albumID);
            record.push_back(artist);
            record.push_back(artistCount);
            results.push_back(record);
        }

        res = resultDB->compareResults(results,"albumModelUtilities",resultID);
    }
    catch(const db::SQLiteException& e)
    {
        omega::common::Log::g_Log.print("SQLite exception on query\n%s\n%s\n",cmd.toUtf8().constData(),e.error().toUtf8().constData());
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumArtistUnit)
{
    // SELECT 0 AS groupFlag, b.albumID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //     FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //     WHERE b.groupID==-1 AND a.artist NOT LIKE ''
    //     GROUP BY b.albumID, a.artist
    //   UNION
    // SELECT 1 AS groupFlag, b.groupID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //     FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //     WHERE b.groupID>=0 AND a.artist NOT LIKE ''
    //     GROUP BY b.groupID, a.artist
    //   UNION
    // SELECT 0 AS groupFlag, b.albumID AS albumID, a.originalArtist AS artist, COUNT(a.originalArtist) AS artistCount
    //     FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //     WHERE b.groupID==-1 AND a.originalArtist NOT LIKE ''
    //     GROUP BY b.albumID, a.originalArtist
    //   UNION
    // SELECT 1 AS groupFlag, b.groupID AS albumID, a.originalArtist AS artist, COUNT(a.originalArtist) AS artistCount
    //     FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //     WHERE b.groupID>=0 AND a.originalArtist NOT LIKE ''
    //     GROUP BY b.groupID, a.originalArtist
    //   UNION
    // SELECT 0 AS groupFlag, b.albumID AS albumID, a.composer AS artist, COUNT(a.composer) AS artistCount
    //     FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //     WHERE b.groupID==-1 AND a.composer NOT LIKE ''
    //     GROUP BY b.albumID, a.composer
    //   UNION
    // SELECT 1 AS groupFlag, b.groupID AS albumID, a.composer AS artist, COUNT(a.composer) AS artistCount
    //     FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //     WHERE b.groupID>=0 AND a.composer NOT LIKE ''
    //     GROUP BY b.groupID, a.composer
    // ORDER BY groupFlag, albumID, artistCount DESC

    QString expectSQL;

    expectSQL  = "SELECT 0 AS groupFlag, b.albumID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount";
    expectSQL += "    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID";
    expectSQL += "    WHERE b.groupID=-1 AND a.artist NOT LIKE ''";
    expectSQL += "    GROUP BY b.albumID, a.artist";
    expectSQL += "  UNION ";
    expectSQL += "SELECT 1 AS groupFlag, b.groupID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount";
    expectSQL += "    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID";
    expectSQL += "    WHERE b.groupID>=0 AND a.artist NOT LIKE ''";
    expectSQL += "    GROUP BY b.groupID, a.artist";
    expectSQL += "  UNION ";
    expectSQL += "SELECT 0 AS groupFlag, b.albumID AS albumID, a.originalArtist AS artist, COUNT(a.originalArtist) AS artistCount";
    expectSQL += "    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID";
    expectSQL += "    WHERE b.groupID=-1 AND a.originalArtist NOT LIKE ''";
    expectSQL += "    GROUP BY b.albumID, a.originalArtist";
    expectSQL += "  UNION ";
    expectSQL += "SELECT 1 AS groupFlag, b.groupID AS albumID, a.originalArtist AS artist, COUNT(a.originalArtist) AS artistCount";
    expectSQL += "    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID";
    expectSQL += "    WHERE b.groupID>=0 AND a.originalArtist NOT LIKE ''";
    expectSQL += "    GROUP BY b.groupID, a.originalArtist";
    expectSQL += "  UNION ";
    expectSQL += "SELECT 0 AS groupFlag, b.albumID AS albumID, a.composer AS artist, COUNT(a.composer) AS artistCount";
    expectSQL += "    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID";
    expectSQL += "    WHERE b.groupID=-1 AND a.composer NOT LIKE ''";
    expectSQL += "    GROUP BY b.albumID, a.composer";
    expectSQL += "  UNION ";
    expectSQL += "SELECT 1 AS groupFlag, b.groupID AS albumID, a.composer AS artist, COUNT(a.composer) AS artistCount";
    expectSQL += "    FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID";
    expectSQL += "    WHERE b.groupID>=0 AND a.composer NOT LIKE ''";
    expectSQL += "    GROUP BY b.groupID, a.composer ";
    expectSQL += "ORDER BY groupFlag, albumID, artistCount DESC";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelUtilitiesTest a;
    QString testSQLA = a.testGetQueryForAlbumArtist();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumArtistIntegration)
{
    AlbumModelUtilitiesTest albumModel;
    QString testSQL = albumModel.testGetQueryForAlbumArtist();
    EXPECT_TRUE(testUtilitySQLAlbumModelUtilitiesAlbumArtist(testSQL,1));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,createAlbumArtistRecord)
{
    AlbumModelKey key(std::pair<bool,int>(true,3));
    QString artist = "Artist";
    int artistCount = 5;

    AlbumModelUtilitiesTest a;
    QueryRecord r = a.testCreateRecordAlbumArtist(key,artist,artistCount);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    EXPECT_TRUE(r.at(0).canConvert(QMetaType::Bool));
    EXPECT_TRUE(r.at(0).toBool()==true);
    EXPECT_TRUE(r.at(1).canConvert(QMetaType::Int));
    EXPECT_TRUE(r.at(1).toInt()==3);
    EXPECT_TRUE(r.at(2).canConvert(QMetaType::QString));
    EXPECT_TRUE(r.at(2).toString()=="Artist");
    EXPECT_TRUE(r.at(3).canConvert(QMetaType::Int));
    EXPECT_TRUE(r.at(3).toInt()==5);
#else
    EXPECT_TRUE(r.at(0).canConvert(QMetaType(QMetaType::Bool)));
    EXPECT_TRUE(r.at(0).toBool()==true);
    EXPECT_TRUE(r.at(1).canConvert(QMetaType(QMetaType::Int)));
    EXPECT_TRUE(r.at(1).toInt()==3);
    EXPECT_TRUE(r.at(2).canConvert(QMetaType(QMetaType::QString)));
    EXPECT_TRUE(r.at(2).toString()=="Artist");
    EXPECT_TRUE(r.at(3).canConvert(QMetaType(QMetaType::Int)));
    EXPECT_TRUE(r.at(3).toInt()==5);
#endif
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,keyAlbumArtistRecord)
{
    AlbumModelKey key(std::pair<bool,int>(true,3));
    QString artist = "Artist";
    int artistCount = 5;

    AlbumModelUtilitiesTest a;
    QueryRecord r = a.testCreateRecordAlbumArtist(key,artist,artistCount);

    EXPECT_TRUE(a.testKeyAlbumArtistRecord(r)==key);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,artistAlbumArtistRecord)
{
    AlbumModelKey key(std::pair<bool,int>(true,3));
    QString artist = "Artist";
    int artistCount = 5;

    AlbumModelUtilitiesTest a;
    QueryRecord r = a.testCreateRecordAlbumArtist(key,artist,artistCount);

    EXPECT_TRUE(a.testArtistAlbumArtistRecord(r)==artist);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,countAlbumArtistRecord)
{
    AlbumModelKey key(std::pair<bool,int>(true,3));
    QString artist = "Artist";
    int artistCount = 5;

    AlbumModelUtilitiesTest a;
    QueryRecord r = a.testCreateRecordAlbumArtist(key,artist,artistCount);

    EXPECT_TRUE(a.testCountAlbumArtistRecord(r)==artistCount);
}

//-------------------------------------------------------------------------------------------

class AMUTestAlbumArtistQuery
{
    public:
        bool *m_isGroup;
        int *m_albumID;
        QString *m_artist;
        int *m_albumCount;

        void bindIsGroup(bool& v) {m_isGroup = &v;}
        void bindAlbumID(int& v) {m_albumID = &v;}
        void bindArtist(QString& v) {m_artist = &v;}
        void bindAlbumCount(int& v) {m_albumCount = &v;}
        bool nextA();
        bool nextB();
};

bool AMUTestAlbumArtistQuery::nextA()
{
    *m_isGroup = true;
    *m_albumID = 2;
    *m_artist = "John";
    *m_albumCount = 3;
    return true;
}

bool AMUTestAlbumArtistQuery::nextB()
{
    *m_isGroup = false;
    *m_albumID = 4;
    *m_artist = "Bob";
    *m_albumCount = 5;
    return true;
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,runAlbumArtistQuery)
{
    AMUTestAlbumArtistQuery rMock;
    db::SQLiteQuerySPtr queryMock(new db::SQLiteQueryMock(db::TrackDB::instance()->db()));
    db::SQLiteQueryMock& query = dynamic_cast<db::SQLiteQueryMock&>(*(queryMock.data()));

    QString cmdQ;
    AlbumModelUtilitiesTest a(queryMock);

    cmdQ = "SELECT mock artist count";

    EXPECT_CALL(query,prepare(cmdQ)).Times(1);
    EXPECT_CALL(query,bind(A<bool&>())).Times(1)
            .WillOnce(Invoke(&rMock,&AMUTestAlbumArtistQuery::bindIsGroup));
    EXPECT_CALL(query,bind(A<int&>())).Times(2)
            .WillOnce(Invoke(&rMock,&AMUTestAlbumArtistQuery::bindAlbumID))
            .WillOnce(Invoke(&rMock,&AMUTestAlbumArtistQuery::bindAlbumCount));
    EXPECT_CALL(query,bind(A<QString&>())).Times(1)
            .WillOnce(Invoke(&rMock,&AMUTestAlbumArtistQuery::bindArtist));
    EXPECT_CALL(query,next()).WillOnce(Invoke(&rMock,&AMUTestAlbumArtistQuery::nextA))
            .WillOnce(Invoke(&rMock,&AMUTestAlbumArtistQuery::nextB))
            .WillRepeatedly(Return(false));

    QueryResult results;
    EXPECT_TRUE(a.testRunAlbumArtistQuery(cmdQ,results));

    EXPECT_TRUE(results.size()==2);

    const QueryRecord& recordA = results.at(0);
    EXPECT_TRUE(a.testKeyAlbumArtistRecord(recordA)==AlbumModelKey(std::pair<bool,int>(true,2)));
    EXPECT_TRUE(a.testArtistAlbumArtistRecord(recordA)=="John");
    EXPECT_TRUE(a.testCountAlbumArtistRecord(recordA)==3);

    const QueryRecord& recordB = results.at(1);
    EXPECT_TRUE(a.testKeyAlbumArtistRecord(recordB)==AlbumModelKey(std::pair<bool,int>(false,4)));
    EXPECT_TRUE(a.testArtistAlbumArtistRecord(recordB)=="Bob");
    EXPECT_TRUE(a.testCountAlbumArtistRecord(recordB)==5);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,primaryArtistNoResults)
{
    QueryResult results;
    AlbumModelUtilitiesTest a;
    EXPECT_TRUE(a.testPrimaryArtist(results)=="");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,primaryArtistSingleResult)
{
    QueryResult results;
    AlbumModelUtilitiesTest a;
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",3));
    EXPECT_TRUE(a.testPrimaryArtist(results)=="John");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,primaryArtistSingleArtistResult)
{
    QueryResult results;
    AlbumModelUtilitiesTest a;
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",3));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",4));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",5));
    EXPECT_TRUE(a.testPrimaryArtist(results)=="John");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,primaryArtistTwoWithOnePrimary)
{
    QueryResult results;
    AlbumModelUtilitiesTest a; // Alice : 61%, John : 39%
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",39));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Alice",61));
    EXPECT_TRUE(a.testPrimaryArtist(results)=="Alice");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,primaryArtistTwoWithVarious)
{
    QueryResult results;
    AlbumModelUtilitiesTest a; // Alice : 55% - 11, John : 45% - 9
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",3));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",4));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Alice",5));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Alice",3));
    EXPECT_TRUE(a.testPrimaryArtist(results)=="Various Artists");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,primaryArtistMultipleWithOnePrimary)
{
    QueryResult results;
    AlbumModelUtilitiesTest a; // Alice : 45% - 9, John : 30% - 6, Bob : 25% - 5
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",2));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",4));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Alice",6));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Alice",3));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Bob",5));
    EXPECT_TRUE(a.testPrimaryArtist(results)=="Alice");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,primaryArtistMultipleWithVarious)
{
    QueryResult results;
    AlbumModelUtilitiesTest a;
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",3));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"John",4));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Alice",5));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Alice",3));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Bob",5));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Bob",2));
    results.push_back(a.testCreateRecordAlbumArtist(AlbumModelKey(std::pair<bool,int>(true,1)),"Jim",7));
    EXPECT_TRUE(a.testPrimaryArtist(results)=="Various Artists");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,isVariousArtistsIntegration)
{
    AlbumModelKey keyA(std::pair<bool,int>(false,3));
    AlbumModelUtilities a(keyA);
    EXPECT_FALSE(a.isVariousArtists());

    AlbumModelKey keyB(std::pair<bool,int>(false,2));
    AlbumModelUtilities b(keyB);
    EXPECT_TRUE(b.isVariousArtists());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,artistIntegration)
{
    AlbumModelKey keyA(std::pair<bool,int>(false,1));
    AlbumModelUtilities a(keyA);
    EXPECT_TRUE(a.artist()=="Soundtrack");

    AlbumModelKey keyB(std::pair<bool,int>(false,2));
    AlbumModelUtilities b(keyB);
    EXPECT_TRUE(b.artist()=="Various Artists");

    AlbumModelKey keyC(std::pair<bool,int>(false,3));
    AlbumModelUtilities c(keyC);
    EXPECT_TRUE(c.artist()=="James Horner");

    AlbumModelKey keyD(std::pair<bool,int>(true,10));
    AlbumModelUtilities d(keyD);
    EXPECT_TRUE(d.artist()=="Jeremy Soule");

    AlbumModelKey keyE(std::pair<bool,int>(true,20));
    AlbumModelUtilities e(keyE);
    EXPECT_TRUE(e.artist()=="John Williams");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,composerIntegration)
{
    AlbumModelKey keyB(std::pair<bool,int>(false,2));
    AlbumModelUtilities b(keyB);
    EXPECT_TRUE(b.composer()=="Steve Jablonsky");

    AlbumModelKey keyC(std::pair<bool,int>(false,3));
    AlbumModelUtilities c(keyC);
    EXPECT_TRUE(c.composer()=="James Horner");

    AlbumModelKey keyD(std::pair<bool,int>(true,10));
    AlbumModelUtilities d(keyD);
    EXPECT_TRUE(d.composer()=="Jeremy Soule");

    AlbumModelKey keyE(std::pair<bool,int>(true,20));
    AlbumModelUtilities e(keyE);
    EXPECT_TRUE(e.composer()=="John Williams");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryConditionForFilterWithNoFilter)
{
    AlbumModelUtilitiesTest utilA(AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(utilA.testGetQueryConditionForFilter()=="");

    AlbumModelUtilitiesTest utilB(AlbumModelKey(std::pair<bool,int>(true,18)));
    EXPECT_TRUE(utilB.testGetQueryConditionForFilter()=="");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryConditionForFilterWithArtist)
{
    // AND (a.artist LIKE 'John Williams' OR
    //      a.originalArtist LIKE 'John Williams' OR
    //      a.composer LIKE 'John Williams')

    QString expectSQL;

    expectSQL  = "AND (a.artist LIKE 'John Williams' OR";
    expectSQL += "     a.originalArtist LIKE 'John Williams' OR";
    expectSQL += "     a.composer LIKE 'John Williams')";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "John Williams";
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(false,2)),key);
    QString testSQLA = a.testGetQueryConditionForFilter();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryConditionForFilterWithGenre)
{
    // AND a.genreID=1

    QString expectSQL;

    expectSQL  = "AND a.genreID=1";
    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.genre() = 1;
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(false,2)),key);
    QString testSQLA = a.testGetQueryConditionForFilter();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryConditionForFilterWithArtistAndGenre)
{
    // AND a.genreID=1
    // AND (a.artist LIKE 'John Williams' OR
    //      a.originalArtist LIKE 'John Williams' OR
    //      a.composer LIKE 'John Williams')

    QString expectSQL;

    expectSQL  = "AND a.genreID=1 ";
    expectSQL += "AND (a.artist LIKE 'John Williams' OR";
    expectSQL += "     a.originalArtist LIKE 'John Williams' OR";
    expectSQL += "     a.composer LIKE 'John Williams')";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "John Williams";
    key.genre() = 1;
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(false,2)),key);
    QString testSQLA = a.testGetQueryConditionForFilter();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryPartForAlbumArtistWithFilterArtistIntegration)
{
    // SELECT 0 AS groupFlag, b.albumID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //   FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //   WHERE b.albumID=2 AND a.artist NOT LIKE ''
    //     AND (a.artist LIKE 'Athlete' OR
    //          a.originalArtist LIKE 'Athlete' OR
    //          a.composer LIKE 'Athlete')
    //   GROUP BY b.albumID, a.artist

    TrackModelKey key;
    key.artist() = "Athlete";
    AlbumModelUtilitiesTest albumModel(AlbumModelKey(std::pair<bool,int>(false,2)),key);
    QString testSQL = albumModel.testGetQueryPartForAlbumArtist(0,albumModel.id());
    EXPECT_TRUE(testUtilitySQLAlbumModelUtilitiesAlbumArtist(testSQL,2));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryPartForAlbumArtistWithFilterGenreIntegration)
{
    // SELECT 0 AS groupFlag, b.albumID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //   FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //   WHERE b.albumID=2 AND a.artist NOT LIKE ''
    //     AND a.genreID=2
    //   GROUP BY b.albumID, a.artist

    TrackModelKey key;
    key.genre() = 2;
    AlbumModelUtilitiesTest albumModel(AlbumModelKey(std::pair<bool,int>(false,2)),key);
    QString testSQL = albumModel.testGetQueryPartForAlbumArtist(0,albumModel.id());
    EXPECT_TRUE(testUtilitySQLAlbumModelUtilitiesAlbumArtist(testSQL,3));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryPartForAlbumArtistWithFilterArtistAndGenreIntegration)
{
    // SELECT 0 AS groupFlag, b.albumID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //   FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //   WHERE b.albumID=2 AND a.artist NOT LIKE ''
    //     AND a.genreID=2
    //     AND (a.artist LIKE 'Athlete' OR
    //          a.originalArtist LIKE 'Athlete' OR
    //          a.composer LIKE 'Athlete')
    //   GROUP BY b.albumID, a.artist

    TrackModelKey key;
    key.genre() = 2;
    key.artist() = "Athlete";
    AlbumModelUtilitiesTest albumModel(AlbumModelKey(std::pair<bool,int>(false,2)),key);
    QString testSQL = albumModel.testGetQueryPartForAlbumArtist(0,albumModel.id());
    EXPECT_TRUE(testUtilitySQLAlbumModelUtilitiesAlbumArtist(testSQL,4));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryPartForGroupArtistWithFilterArtistIntegration)
{
    // SELECT 1 AS groupFlag, b.groupID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //   FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //   WHERE b.groupID=15 AND a.artist NOT LIKE ''
    //     AND (a.artist LIKE 'Jeremy Soule' OR
    //          a.originalArtist LIKE 'Jeremy Soule' OR
    //          a.composer LIKE 'Jeremy Soule')
    //   GROUP BY b.groupID, a.artist

    TrackModelKey key;
    key.artist() = "Jeremy Soule";
    AlbumModelUtilitiesTest albumModel(AlbumModelKey(std::pair<bool,int>(true,15)),key);
    QString testSQL = albumModel.testGetQueryPartForAlbumArtist(0,albumModel.id());
    EXPECT_TRUE(testUtilitySQLAlbumModelUtilitiesAlbumArtist(testSQL,5));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryPartForGroupArtistWithFilterGenreIntegration)
{
    // SELECT 1 AS groupFlag, b.groupID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //   FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //   WHERE b.groupID=15 AND a.artist NOT LIKE ''
    //     AND a.genreID=1
    //   GROUP BY b.groupID, a.artist

    TrackModelKey key;
    key.genre() = 1;
    AlbumModelUtilitiesTest albumModel(AlbumModelKey(std::pair<bool,int>(true,15)),key);
    QString testSQL = albumModel.testGetQueryPartForAlbumArtist(0,albumModel.id());
    EXPECT_TRUE(testUtilitySQLAlbumModelUtilitiesAlbumArtist(testSQL,6));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryPartForGroupArtistWithFilterGenreAndArtistIntegration)
{
    // SELECT 1 AS groupFlag, b.groupID AS albumID, a.artist AS artist, COUNT(a.artist) AS artistCount
    //   FROM track AS a INNER JOIN album AS b ON a.albumID=b.albumID
    //   WHERE b.groupID=15 AND a.artist NOT LIKE ''
    //     AND a.genreID=1
    //     AND (a.artist LIKE 'Jeremy Soule' OR
    //          a.originalArtist LIKE 'Jeremy Soule' OR
    //          a.composer LIKE 'Jeremy Soule')
    //   GROUP BY b.groupID, a.artist

    TrackModelKey key;
    key.artist() = "Jeremy Soule";
    key.genre() = 1;
    AlbumModelUtilitiesTest albumModel(AlbumModelKey(std::pair<bool,int>(true,15)),key);
    QString testSQL = albumModel.testGetQueryPartForAlbumArtist(0,albumModel.id());
    EXPECT_TRUE(testUtilitySQLAlbumModelUtilitiesAlbumArtist(testSQL,7));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumImageWithAlbumAndNoFilter)
{
    // SELECT 0 AS groupFlag, a.albumID AS albumID, b.imageID, COUNT(b.imageID) AS usage,
    //     b.type AS type
    //   FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)
    //     INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)
    //   WHERE c.albumID=2
    //   GROUP BY a.albumID, b.imageID
    //   ORDER BY a.albumID, b.imageID

    QString expectSQL;

    expectSQL += "SELECT 0 AS groupFlag, a.albumID AS albumID, b.imageID, COUNT(b.imageID) AS usage,";
    expectSQL += "    b.type AS type";
    expectSQL += "  FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)";
    expectSQL += "    INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)";
    expectSQL += "  WHERE c.albumID=2";
    expectSQL += "  GROUP BY a.albumID, b.imageID";
    expectSQL += "  ORDER BY a.albumID, b.imageID";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(false,2)));
    QString testSQLA = a.testGetQueryForAlbumImage();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumImageWithAlbumAndGenreFilter)
{
    // SELECT 0 AS groupFlag, a.albumID AS albumID, b.imageID, COUNT(b.imageID) AS usage,
    //     b.type AS type
    //   FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)
    //     INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)
    //   WHERE c.albumID=2
    //     AND a.genreID=2
    //   GROUP BY a.albumID, b.imageID
    //   ORDER BY a.albumID, b.imageID

    QString expectSQL;

    expectSQL += "SELECT 0 AS groupFlag, a.albumID AS albumID, b.imageID, COUNT(b.imageID) AS usage,";
    expectSQL += "    b.type AS type";
    expectSQL += "  FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)";
    expectSQL += "    INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)";
    expectSQL += "  WHERE c.albumID=2";
    expectSQL += "    AND a.genreID=2";
    expectSQL += "  GROUP BY a.albumID, b.imageID";
    expectSQL += "  ORDER BY a.albumID, b.imageID";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.genre() = 2;
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(false,2)),key);
    QString testSQLA = a.testGetQueryForAlbumImage();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumImageWithAlbumAndArtistFilter)
{
    // SELECT 0 AS groupFlag, a.albumID AS albumID, b.imageID, COUNT(b.imageID) AS usage,
    //     b.type AS type
    //   FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)
    //     INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)
    //   WHERE c.albumID=2
    //     AND (a.artist LIKE 'Athlete' OR
    //          a.originalArtist LIKE 'Athlete' OR
    //          a.composer LIKE 'Athlete')
    //   GROUP BY a.albumID, b.imageID
    //   ORDER BY a.albumID, b.imageID

    QString expectSQL;

    expectSQL += "SELECT 0 AS groupFlag, a.albumID AS albumID, b.imageID, COUNT(b.imageID) AS usage,";
    expectSQL += "    b.type AS type";
    expectSQL += "  FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)";
    expectSQL += "    INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)";
    expectSQL += "  WHERE c.albumID=2";
    expectSQL += "    AND (a.artist LIKE 'Athlete' OR";
    expectSQL += "         a.originalArtist LIKE 'Athlete' OR";
    expectSQL += "         a.composer LIKE 'Athlete')";
    expectSQL += "  GROUP BY a.albumID, b.imageID";
    expectSQL += "  ORDER BY a.albumID, b.imageID";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "Athlete";
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(false,2)),key);
    QString testSQLA = a.testGetQueryForAlbumImage();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumImageWithAlbumAndAlbumAndGenreFilter)
{
    // SELECT 0 AS groupFlag, a.albumID AS albumID, b.imageID, COUNT(b.imageID) AS usage,
    //     b.type AS type
    //   FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)
    //     INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)
    //   WHERE c.albumID=2
    //     AND a.genreID=2
    //     AND (a.artist LIKE 'Athlete' OR
    //          a.originalArtist LIKE 'Athlete' OR
    //          a.composer LIKE 'Athlete')
    //   GROUP BY a.albumID, b.imageID
    //   ORDER BY a.albumID, b.imageID

    QString expectSQL;

    expectSQL += "SELECT 0 AS groupFlag, a.albumID AS albumID, b.imageID, COUNT(b.imageID) AS usage,";
    expectSQL += "    b.type AS type";
    expectSQL += "  FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)";
    expectSQL += "    INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)";
    expectSQL += "  WHERE c.albumID=2";
    expectSQL += "    AND a.genreID=2";
    expectSQL += "    AND (a.artist LIKE 'Athlete' OR";
    expectSQL += "         a.originalArtist LIKE 'Athlete' OR";
    expectSQL += "         a.composer LIKE 'Athlete')";
    expectSQL += "  GROUP BY a.albumID, b.imageID";
    expectSQL += "  ORDER BY a.albumID, b.imageID";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.genre() = 2;
    key.artist() = "Athlete";
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(false,2)),key);
    QString testSQLA = a.testGetQueryForAlbumImage();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumImageWithGroupAndNoFilter)
{
    // SELECT 1 AS groupFlag, d.groupID AS albumID, b.imageID, COUNT(b.imageID) AS usage,
    //     b.type AS type
    //   FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)
    //     INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)
    //     INNER JOIN album AS d ON (a.albumID=d.albumID)
    //   WHERE d.groupID=15
    //   GROUP BY d.groupID, b.imageID
    //   ORDER BY d.groupID, b.imageID

    QString expectSQL;

    expectSQL += "SELECT 1 AS groupFlag, d.groupID AS albumID, b.imageID, COUNT(b.imageID) AS usage,";
    expectSQL += "    b.type AS type";
    expectSQL += "  FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)";
    expectSQL += "    INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)";
    expectSQL += "    INNER JOIN album AS d ON (a.albumID=d.albumID)";
    expectSQL += "  WHERE d.groupID=15";
    expectSQL += "  GROUP BY d.groupID, b.imageID";
    expectSQL += "  ORDER BY d.groupID, b.imageID";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(true,15)));
    QString testSQLA = a.testGetQueryForAlbumImage();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumImageWithGroupAndGenreFilter)
{
    // SELECT 1 AS groupFlag, d.groupID AS albumID, b.imageID, COUNT(b.imageID) AS usage,
    //     b.type AS type
    //   FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)
    //     INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)
    //     INNER JOIN album AS d ON (a.albumID=d.albumID)
    //   WHERE d.groupID=15
    //     AND a.genreID=1
    //   GROUP BY d.groupID, b.imageID
    //   ORDER BY d.groupID, b.imageID

    QString expectSQL;

    expectSQL += "SELECT 1 AS groupFlag, d.groupID AS albumID, b.imageID, COUNT(b.imageID) AS usage,";
    expectSQL += "    b.type AS type";
    expectSQL += "  FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)";
    expectSQL += "    INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)";
    expectSQL += "    INNER JOIN album AS d ON (a.albumID=d.albumID)";
    expectSQL += "  WHERE d.groupID=15";
    expectSQL += "    AND a.genreID=1";
    expectSQL += "  GROUP BY d.groupID, b.imageID";
    expectSQL += "  ORDER BY d.groupID, b.imageID";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.genre() = 1;
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(true,15)),key);
    QString testSQLA = a.testGetQueryForAlbumImage();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumImageWithGroupAndArtistFilter)
{
    // SELECT 1 AS groupFlag, d.groupID AS albumID, b.imageID, COUNT(b.imageID) AS usage,
    //     b.type AS type
    //   FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)
    //     INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)
    //     INNER JOIN album AS d ON (a.albumID=d.albumID)
    //   WHERE d.groupID=15
    //     AND (a.artist LIKE 'Jeremy Soule' OR
    //          a.originalArtist LIKE 'Jeremy Soule' OR
    //          a.composer LIKE 'Jeremy Soule')
    //   GROUP BY d.groupID, b.imageID
    //   ORDER BY d.groupID, b.imageID

    QString expectSQL;

    expectSQL += "SELECT 1 AS groupFlag, d.groupID AS albumID, b.imageID, COUNT(b.imageID) AS usage,";
    expectSQL += "    b.type AS type";
    expectSQL += "  FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)";
    expectSQL += "    INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)";
    expectSQL += "    INNER JOIN album AS d ON (a.albumID=d.albumID)";
    expectSQL += "  WHERE d.groupID=15";
    expectSQL += "    AND (a.artist LIKE 'Jeremy Soule' OR";
    expectSQL += "         a.originalArtist LIKE 'Jeremy Soule' OR";
    expectSQL += "         a.composer LIKE 'Jeremy Soule')";
    expectSQL += "  GROUP BY d.groupID, b.imageID";
    expectSQL += "  ORDER BY d.groupID, b.imageID";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "Jeremy Soule";
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(true,15)),key);
    QString testSQLA = a.testGetQueryForAlbumImage();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumImageWithGroupAndArtistAndGenreFilter)
{
    // SELECT 1 AS groupFlag, d.groupID AS albumID, b.imageID, COUNT(b.imageID) AS usage,
    //     b.type AS type
    //   FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)
    //     INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)
    //     INNER JOIN album AS d ON (a.albumID=d.albumID)
    //   WHERE d.groupID=15
    //     AND a.genreID=1
    //     AND (a.artist LIKE 'Jeremy Soule' OR
    //          a.originalArtist LIKE 'Jeremy Soule' OR
    //          a.composer LIKE 'Jeremy Soule')
    //   GROUP BY d.groupID, b.imageID
    //   ORDER BY d.groupID, b.imageID

    QString expectSQL;

    expectSQL += "SELECT 1 AS groupFlag, d.groupID AS albumID, b.imageID, COUNT(b.imageID) AS usage,";
    expectSQL += "    b.type AS type";
    expectSQL += "  FROM image AS b INNER JOIN imagemap AS c ON (b.imageID=c.imageID)";
    expectSQL += "    INNER JOIN track AS a ON (c.albumID=a.albumID AND c.trackID=a.trackID)";
    expectSQL += "    INNER JOIN album AS d ON (a.albumID=d.albumID)";
    expectSQL += "  WHERE d.groupID=15";
    expectSQL += "    AND a.genreID=1";
    expectSQL += "    AND (a.artist LIKE 'Jeremy Soule' OR";
    expectSQL += "         a.originalArtist LIKE 'Jeremy Soule' OR";
    expectSQL += "         a.composer LIKE 'Jeremy Soule')";
    expectSQL += "  GROUP BY d.groupID, b.imageID";
    expectSQL += "  ORDER BY d.groupID, b.imageID";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "Jeremy Soule";
    key.genre() = 1;
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(true,15)),key);
    QString testSQLA = a.testGetQueryForAlbumImage();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

bool testUtilitySQLAlbumModelUtilitiesAlbumImage(const QString& cmd,int resultID)
{
    bool res = false;
    db::TrackDB *trackDB = db::TrackDB::instance();
    TrackDBTestEnviroment *resultDB = TrackDBTestEnviroment::instance();
    QVector<QVector<QVariant> > results;

    if(trackDB==0)
    {
        return false;
    }

    try
    {
        bool isGroupFlag;
        int albumID,imageID,imageCount,type;
        QString artist;
        db::SQLiteQuery dbQ(trackDB->db());

        dbQ.prepare(cmd);
        dbQ.bind(isGroupFlag);
        dbQ.bind(albumID);
        dbQ.bind(imageID);
        dbQ.bind(imageCount);
        dbQ.bind(type);
        while(dbQ.next())
        {
            QVector<QVariant> record;
            record.push_back((isGroupFlag) ? 1 : 0);
            record.push_back(albumID);
            record.push_back(imageID);
            record.push_back(imageCount);
            record.push_back(type);
            results.push_back(record);
        }

        res = resultDB->compareResults(results,"albumModelUtilities",resultID);
    }
    catch(const db::SQLiteException& e)
    {
        omega::common::Log::g_Log.print("SQLite exception on query\n%s\n%s\n",cmd.toUtf8().constData(),e.error().toUtf8().constData());
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumImageWithAlbumIntegration)
{
    TrackModelKey key;
    key.genre() = 2;
    key.artist() = "Athlete";
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(false,2)),key);
    QString testSQL = a.testGetQueryForAlbumImage();
    EXPECT_TRUE(testUtilitySQLAlbumModelUtilitiesAlbumImage(testSQL,8));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForAlbumImageWithGroupIntegration)
{
    TrackModelKey key;
    key.artist() = "Jeremy Soule";
    key.genre() = 1;
    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(true,15)),key);
    QString testSQL = a.testGetQueryForAlbumImage();
    EXPECT_TRUE(testUtilitySQLAlbumModelUtilitiesAlbumImage(testSQL,9));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,createRecordAlbumImage)
{
    AlbumModelKey key(std::pair<bool,int>(true,3));
    AlbumModelUtilitiesTest util;
    QueryRecord record = util.testCreateRecordAlbumImage(key,4,5,6);
    EXPECT_TRUE(util.testKeyAlbumImageRecord(record)==key);
    EXPECT_TRUE(util.testImageIDAlbumImageRecord(record)==4);
    EXPECT_TRUE(util.testImageCountAlbumImageRecord(record)==5);
    EXPECT_TRUE(util.testImageTypeAlbumImageRecord(record)==info::e_TagImage_Media);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,imageTypeAlbumImageRecordOutOfRange)
{
    AlbumModelKey key(std::pair<bool,int>(true,3));
    AlbumModelUtilitiesTest util;
    QueryRecord recordA = util.testCreateRecordAlbumImage(key,4,5,-1);
    EXPECT_TRUE(util.testImageTypeAlbumImageRecord(recordA)==0);
    QueryRecord recordB = util.testCreateRecordAlbumImage(key,4,5,0x15);
    EXPECT_TRUE(util.testImageTypeAlbumImageRecord(recordB)==0);
    QueryRecord recordC = util.testCreateRecordAlbumImage(key,4,5,0x14);
    EXPECT_TRUE(util.testImageTypeAlbumImageRecord(recordC)==omega::track::info::e_TagImage_StudioLogo);
}

//-------------------------------------------------------------------------------------------

class AMUTestAlbumImageQuery
{
    public:
        bool *m_isGroup;
        int *m_albumID;
        int *m_imageID;
        int *m_imageCount;
        int *m_imageType;

        void bindIsGroup(bool& v) {m_isGroup = &v;}
        void bindAlbumID(int& v) {m_albumID = &v;}
        void bindImageID(int& v) {m_imageID = &v;}
        void bindImageCount(int& v) {m_imageCount = &v;}
        void bindImageType(int& v) {m_imageType = &v;}

        bool nextA();
        bool nextB();
};

bool AMUTestAlbumImageQuery::nextA()
{
    *m_isGroup = true;
    *m_albumID = 2;
    *m_imageID = 5;
    *m_imageCount = 9;
    *m_imageType = 8;
    return true;
}

bool AMUTestAlbumImageQuery::nextB()
{
    *m_isGroup = false;
    *m_albumID = 4;
    *m_imageID = 6;
    *m_imageCount = 11;
    *m_imageType = 3;
    return true;
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,runAlbumImageQuery)
{
    AMUTestAlbumImageQuery rMock;
    db::SQLiteQuerySPtr queryMock(new db::SQLiteQueryMock(db::TrackDB::instance()->db()));
    db::SQLiteQueryMock& query = dynamic_cast<db::SQLiteQueryMock&>(*(queryMock.data()));

    QString cmdQ;
    AlbumModelUtilitiesTest a(queryMock);

    cmdQ = "SELECT mock image count";

    EXPECT_CALL(query,prepare(cmdQ)).Times(1);
    EXPECT_CALL(query,bind(A<bool&>())).Times(1)
            .WillOnce(Invoke(&rMock,&AMUTestAlbumImageQuery::bindIsGroup));
    EXPECT_CALL(query,bind(A<int&>())).Times(4)
            .WillOnce(Invoke(&rMock,&AMUTestAlbumImageQuery::bindAlbumID))
            .WillOnce(Invoke(&rMock,&AMUTestAlbumImageQuery::bindImageID))
            .WillOnce(Invoke(&rMock,&AMUTestAlbumImageQuery::bindImageCount))
            .WillOnce(Invoke(&rMock,&AMUTestAlbumImageQuery::bindImageType));
    EXPECT_CALL(query,next()).WillOnce(Invoke(&rMock,&AMUTestAlbumImageQuery::nextA))
            .WillOnce(Invoke(&rMock,&AMUTestAlbumImageQuery::nextB))
            .WillRepeatedly(Return(false));

    QueryResult results;
    EXPECT_TRUE(a.testRunAlbumImageQuery(cmdQ,results));

    EXPECT_TRUE(results.size()==2);

    const QueryRecord& recordA = results.at(0);
    EXPECT_TRUE(a.testKeyAlbumImageRecord(recordA)==AlbumModelKey(std::pair<bool,int>(true,2)));
    EXPECT_TRUE(a.testImageIDAlbumImageRecord(recordA)==5);
    EXPECT_TRUE(a.testImageCountAlbumImageRecord(recordA)==9);
    EXPECT_TRUE(a.testImageTypeAlbumImageRecord(recordA)==8);

    const QueryRecord& recordB = results.at(1);
    EXPECT_TRUE(a.testKeyAlbumImageRecord(recordB)==AlbumModelKey(std::pair<bool,int>(false,4)));
    EXPECT_TRUE(a.testImageIDAlbumImageRecord(recordB)==6);
    EXPECT_TRUE(a.testImageCountAlbumImageRecord(recordB)==11);
    EXPECT_TRUE(a.testImageTypeAlbumImageRecord(recordB)==3);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,preferedImageTypeOutOfRange)
{
    omega::track::info::IDTagImageType a,b;

    a = static_cast<omega::track::info::IDTagImageType>(-1);
    b = static_cast<omega::track::info::IDTagImageType>(-1);
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = omega::track::info::e_TagImage_Band;
    b = static_cast<omega::track::info::IDTagImageType>(-1);
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = static_cast<omega::track::info::IDTagImageType>(-1);
    b = omega::track::info::e_TagImage_Band;
    EXPECT_FALSE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,preferedImageTypeOrdering)
{
    omega::track::info::IDTagImageType a,b;

    b = omega::track::info::e_TagImage_CoverFront;

    a = b;
    b = omega::track::info::e_TagImage_Band;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Leaflet;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Media;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Performance;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_CoverBack;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_LeadArtist;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Artist;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Conductor;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Composer;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Lyricist;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_RecLocation;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Recording;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Illustration;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_BandLogo;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_StudioLogo;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Icon32x32;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Other;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_OtherIcon;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_VidCaptute;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));

    a = b;
    b = omega::track::info::e_TagImage_Fish;
    EXPECT_TRUE(AlbumModelUtilitiesTest::testPreferedImageType(a,b));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,compareImageResultsOnImageType)
{
    AlbumModelKey key(std::pair<bool,int>(false,2));
    AlbumModelUtilitiesTest util;
    QueryRecord aRec = util.testCreateRecordAlbumImage(key,4,5,3);
    QueryRecord bRec = util.testCreateRecordAlbumImage(key,4,5,10);
    EXPECT_TRUE(AlbumModelUtilitiesTest::testCompareImageResults(aRec,bRec));
    EXPECT_FALSE(AlbumModelUtilitiesTest::testCompareImageResults(bRec,aRec));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,compareImageResultsOnImageCount)
{
    AlbumModelKey key(std::pair<bool,int>(false,2));
    AlbumModelUtilitiesTest util;
    QueryRecord aRec = util.testCreateRecordAlbumImage(key,4,6,3);
    QueryRecord bRec = util.testCreateRecordAlbumImage(key,4,5,3);
    EXPECT_TRUE(AlbumModelUtilitiesTest::testCompareImageResults(aRec,bRec));
    EXPECT_FALSE(AlbumModelUtilitiesTest::testCompareImageResults(bRec,aRec));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,compareImageResultsOnImageID)
{
    AlbumModelKey key(std::pair<bool,int>(false,2));
    AlbumModelUtilitiesTest util;
    QueryRecord aRec = util.testCreateRecordAlbumImage(key,2,5,3);
    QueryRecord bRec = util.testCreateRecordAlbumImage(key,4,5,3);
    EXPECT_TRUE(AlbumModelUtilitiesTest::testCompareImageResults(aRec,bRec));
    EXPECT_FALSE(AlbumModelUtilitiesTest::testCompareImageResults(bRec,aRec));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,sortImageListByPreferredType)
{
    AlbumModelKey key(std::pair<bool,int>(false,2));
    AlbumModelUtilitiesTest util;
    QueryRecord aRec = util.testCreateRecordAlbumImage(key,2,5,3);
    QueryRecord bRec = util.testCreateRecordAlbumImage(key,4,5,10);
    QueryRecord cRec = util.testCreateRecordAlbumImage(key,3,7,3);
    QueryRecord dRec = util.testCreateRecordAlbumImage(key,6,5,10);
    QueryRecord eRec = util.testCreateRecordAlbumImage(key,8,8,3);
    QueryRecord fRec = util.testCreateRecordAlbumImage(key,9,5,10);

    QueryResult result;
    result.push_back(aRec);
    result.push_back(bRec);
    result.push_back(cRec);
    result.push_back(dRec);
    result.push_back(eRec);
    result.push_back(fRec);

    util.testSortImageListByPreferredType(result);

    EXPECT_TRUE(util.testImageIDAlbumImageRecord( result.at(0) ) == 8);
    EXPECT_TRUE(util.testImageCountAlbumImageRecord( result.at(0) ) == 8);
    EXPECT_TRUE(util.testImageTypeAlbumImageRecord( result.at(0) ) == 3);

    EXPECT_TRUE(util.testImageIDAlbumImageRecord( result.at(1) ) == 3);
    EXPECT_TRUE(util.testImageCountAlbumImageRecord( result.at(1) ) == 7);
    EXPECT_TRUE(util.testImageTypeAlbumImageRecord( result.at(1) ) == 3);

    EXPECT_TRUE(util.testImageIDAlbumImageRecord( result.at(2) ) == 2);
    EXPECT_TRUE(util.testImageCountAlbumImageRecord( result.at(2) ) == 5);
    EXPECT_TRUE(util.testImageTypeAlbumImageRecord( result.at(2) ) == 3);

    EXPECT_TRUE(util.testImageIDAlbumImageRecord( result.at(3) ) == 4);
    EXPECT_TRUE(util.testImageCountAlbumImageRecord( result.at(3) ) == 5);
    EXPECT_TRUE(util.testImageTypeAlbumImageRecord( result.at(3) ) == 10);

    EXPECT_TRUE(util.testImageIDAlbumImageRecord( result.at(4) ) == 6);
    EXPECT_TRUE(util.testImageCountAlbumImageRecord( result.at(4) ) == 5);
    EXPECT_TRUE(util.testImageTypeAlbumImageRecord( result.at(4) ) == 10);

    EXPECT_TRUE(util.testImageIDAlbumImageRecord( result.at(5) ) == 9);
    EXPECT_TRUE(util.testImageCountAlbumImageRecord( result.at(5) ) == 5);
    EXPECT_TRUE(util.testImageTypeAlbumImageRecord( result.at(5) ) == 10);
}

//-------------------------------------------------------------------------------------------

class AMUAlbumArtTest : public AlbumModelUtilities
{
    public:
        MOCK_CONST_METHOD0(getQueryForAlbumImage,QString());
        MOCK_CONST_METHOD2(runAlbumImageQuery,bool(const QString& cmdQ,QueryResult& results));
};

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,albumArtImageQueryFail)
{
    ImageRepositarySPtr imgEmpty;
    ImageRepositarySPtr pImgRep = ImageRepositary::instance("mock");
    ImageRepositaryMock& iRepMock = dynamic_cast<ImageRepositaryMock&>(*(pImgRep.data()));

    QImage *refImage = new QImage;
    AMUAlbumArtTest util;

    EXPECT_CALL(iRepMock,getReference(32,48)).Times(1).WillOnce(Return(refImage));

    QString mQuery = "SELECT imageID";
    EXPECT_CALL(util,getQueryForAlbumImage()).Times(1).WillOnce(Return(mQuery));
    EXPECT_CALL(util,runAlbumImageQuery(mQuery,A<QueryResult&>())).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(util.albumArt(32,48)==refImage);

    delete refImage;

    pImgRep = imgEmpty;
    ImageRepositary::release();
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,albumArtNoImageResults)
{
    ImageRepositarySPtr imgEmpty;
    ImageRepositarySPtr pImgRep = ImageRepositary::instance("mock");
    ImageRepositaryMock& iRepMock = dynamic_cast<ImageRepositaryMock&>(*(pImgRep.data()));

    QImage *refImage = new QImage;
    AMUAlbumArtTest util;

    EXPECT_CALL(iRepMock,getReference(32,48)).Times(1).WillOnce(Return(refImage));

    QueryResult results;

    QString mQuery = "SELECT imageID";
    EXPECT_CALL(util,getQueryForAlbumImage()).Times(1).WillOnce(Return(mQuery));
    EXPECT_CALL(util,runAlbumImageQuery(mQuery,A<QueryResult&>())).Times(1)
        .WillOnce(DoAll(SetArgReferee<1>(results),Return(true)));

    EXPECT_TRUE(util.albumArt(32,48)==refImage);

    delete refImage;

    pImgRep = imgEmpty;
    ImageRepositary::release();
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,albumArtFailToGetImageFromRepositary)
{
    ImageRepositarySPtr imgEmpty;
    ImageRepositarySPtr pImgRep = ImageRepositary::instance("mock");
    ImageRepositaryMock& iRepMock = dynamic_cast<ImageRepositaryMock&>(*(pImgRep.data()));

    QImage *refImage = new QImage;
    AMUAlbumArtTest util;

    EXPECT_CALL(iRepMock,getImage(2,32,48)).Times(1).WillOnce(Return((QImage *)(0)));
    EXPECT_CALL(iRepMock,getReference(32,48)).Times(1).WillOnce(Return(refImage));

    QueryResult results;
    std::pair<bool,int> rKey(false,2);
    AlbumModelUtilitiesTest u;
    results.push_back(u.testCreateRecordAlbumImage(rKey,2,3,4));

    QString mQuery = "SELECT imageID";
    EXPECT_CALL(util,getQueryForAlbumImage()).Times(1).WillOnce(Return(mQuery));
    EXPECT_CALL(util,runAlbumImageQuery(mQuery,A<QueryResult&>())).Times(1)
        .WillOnce(DoAll(SetArgReferee<1>(results),Return(true)));

    EXPECT_TRUE(util.albumArt(32,48)==refImage);

    delete refImage;

    pImgRep = imgEmpty;
    ImageRepositary::release();
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,albumArtSuccess)
{
    ImageRepositarySPtr imgEmpty;
    ImageRepositarySPtr pImgRep = ImageRepositary::instance("mock");
    ImageRepositaryMock& iRepMock = dynamic_cast<ImageRepositaryMock&>(*(pImgRep.data()));

    QImage *pImage = new QImage;
    AMUAlbumArtTest util;

    EXPECT_CALL(iRepMock,getImage(2,32,48)).Times(1).WillOnce(Return(pImage));

    QueryResult results;
    std::pair<bool,int> rKey(false,2);
    AlbumModelUtilitiesTest u;
    results.push_back(u.testCreateRecordAlbumImage(rKey,2,3,4));

    QString mQuery = "SELECT imageID";
    EXPECT_CALL(util,getQueryForAlbumImage()).Times(1).WillOnce(Return(mQuery));
    EXPECT_CALL(util,runAlbumImageQuery(mQuery,A<QueryResult&>())).Times(1)
        .WillOnce(DoAll(SetArgReferee<1>(results),Return(true)));

    EXPECT_TRUE(util.albumArt(32,48)==pImage);

    delete pImage;

    pImgRep = imgEmpty;
    ImageRepositary::release();
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,albumArtIntegration)
{
    ImageRepositarySPtr imgEmpty;
    ImageRepositarySPtr pImgRep = ImageRepositary::instance("image");

    AlbumModelUtilities utilA(std::pair<bool,int>(false,2));
    QImage *imgA = utilA.albumArt(32,48);
    ASSERT_TRUE(imgA!=0);

    TrackModelKey keyB;
    keyB.artist() = "Steve Jablonsky";
    AlbumModelUtilities utilB(AlbumModelKey(std::pair<bool,int>(false,2)),keyB);
    QImage *imgB = utilB.albumArt(32,48);
    ASSERT_TRUE(imgB!=0);

    EXPECT_TRUE(imgA!=imgB);

    pImgRep = imgEmpty;
    ImageRepositary::release();
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForYearWhenAlbum)
{
    // SELECT year FROM album
    //   WHERE albumID=2

    QString expectSQL;

    expectSQL += "SELECT year FROM album";
    expectSQL += "  WHERE albumID=2";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(false,2)));
    QString testSQLA = a.testGetQueryForYear();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,getQueryForYearWhenGroup)
{
    // SELECT avg(year) AS year FROM album
    //   WHERE groupID=15

    QString expectSQL;

    expectSQL += "SELECT avg(year) AS year FROM album";
    expectSQL += "  WHERE groupID=15";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelUtilitiesTest a(AlbumModelKey(std::pair<bool,int>(true,15)));
    QString testSQLA = a.testGetQueryForYear();
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

class AMUTestYearQuery
{
    public:
        int *m_year;

        void bindYear(int& v) {m_year = &v;}
        bool next();
};

bool AMUTestYearQuery::next()
{
    *m_year = 2014;
    return true;
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,yearQueryUnitTestSuccess)
{
    AMUTestYearQuery rMock;
    db::SQLiteQuerySPtr queryMock(new db::SQLiteQueryMock(db::TrackDB::instance()->db()));
    db::SQLiteQueryMock& query = dynamic_cast<db::SQLiteQueryMock&>(*(queryMock.data()));

    QString cmdQ;
    AlbumModelUtilitiesTest a(queryMock);

    cmdQ = a.testGetQueryForYear();

    EXPECT_CALL(query,prepare(cmdQ)).Times(1);
    EXPECT_CALL(query,bind(A<int&>())).Times(1)
            .WillOnce(Invoke(&rMock,&AMUTestYearQuery::bindYear));
    EXPECT_CALL(query,next()).Times(1)
        .WillOnce(Invoke(&rMock,&AMUTestYearQuery::next));

    EXPECT_TRUE(a.year()==2014);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,yearQueryUnitTestFail)
{
    AMUTestYearQuery rMock;
    db::SQLiteQuerySPtr queryMock(new db::SQLiteQueryMock(db::TrackDB::instance()->db()));
    db::SQLiteQueryMock& query = dynamic_cast<db::SQLiteQueryMock&>(*(queryMock.data()));

    QString cmdQ;
    AlbumModelUtilitiesTest a(queryMock);

    cmdQ = a.testGetQueryForYear();

    EXPECT_CALL(query,prepare(cmdQ)).Times(1);
    EXPECT_CALL(query,bind(A<int&>())).Times(1)
            .WillOnce(Invoke(&rMock,&AMUTestYearQuery::bindYear));
    EXPECT_CALL(query,next()).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(a.year()==0);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,yearWhenAlbumIntegration)
{
    AlbumModelUtilities util(AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(util.year()==2013);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelUtilities,yearWhenGroupIntegration)
{
    AlbumModelUtilities util(AlbumModelKey(std::pair<bool,int>(true,15)));
    EXPECT_TRUE(util.year()==2011);
}

//-------------------------------------------------------------------------------------------
