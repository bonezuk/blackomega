#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/model/inc/AlbumModel.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "track/db/test/SQLiteMock.h"

using namespace omega::track::model;
using namespace omega::track;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AlbumModelTest : public AlbumModel
{
    public:
        AlbumModelTest();
        AlbumModelTest(db::SQLiteQuerySPtr pQueryMock);
        AlbumModelTest(const TrackModelKey& filterKey);
        QString testGetQuery(const AlbumModelKey& albumID);
        QueryRecord testCreateRecordAlbum(const AlbumModelKey& key,const QString& name) const;
        AlbumModelKey testKeyRecordAlbum(const QueryRecord& record) const;
        QString testNameRecordAlbum(const QueryRecord& record) const;
        virtual db::SQLiteQuerySPtr getDBQuery() const;
        bool testRunAlbumQuery(const QString& cmdQ,QueryResult& results) const;
        bool testPopulate();
        QList<QPair<AlbumModelKey,QString> >& albumList();

        static bool testCompareIdenticalAlbumNameLessThan(const QueryRecord& a,const QueryRecord& b);

        QVector<QChar> testGetIndexAlphabet() const;
        void testBuildIndexMap(const QVector<QChar>& alphabet,QMap<QChar,int>& indexMap) const;
        QChar testGetFirstCharacter(const QString& name,const QMap<QChar,int>& indexMap,const QVector<QChar>& alphabet) const;
        void testMapResultsToAlphabetIndex(const QueryResult& results,const QMap<QChar,int>& indexMap,const QVector<QChar>& alphabet,QMap<QChar,QMultiMap<QString,int> >& sectionMap) const;

        void testEnumerateSections(const QueryResult& results);
        QVector<int>& getPositionIndex();
        void testInsertIntoAlbum(QVector<QueryRecord>& recordList);
        void testAddToModelForGivenMap(const QueryResult& results,const QMultiMap<QString,int>& map);
        void testBuildModelFromSortedIndex(const QueryResult& results,const QVector<QChar>& alphabet,const QMap<QChar,QMultiMap<QString,int> >& sectionMap);

    protected:
        db::SQLiteQuerySPtr m_pQuery;
};

//-------------------------------------------------------------------------------------------

AlbumModelTest::AlbumModelTest() : AlbumModel()
{}

//-------------------------------------------------------------------------------------------

AlbumModelTest::AlbumModelTest(db::SQLiteQuerySPtr pQueryMock) : AlbumModel(),
    m_pQuery(pQueryMock)
{}

//-------------------------------------------------------------------------------------------

AlbumModelTest::AlbumModelTest(const TrackModelKey& filterKey) : AlbumModel()
{
    m_filterKey = filterKey;
}

//-------------------------------------------------------------------------------------------

QString AlbumModelTest::testGetQuery(const AlbumModelKey& albumID)
{
    return getQuery(albumID);
}

//-------------------------------------------------------------------------------------------

QueryRecord AlbumModelTest::testCreateRecordAlbum(const AlbumModelKey& key,const QString& name) const
{
    return createRecordAlbum(key,name);
}

//-------------------------------------------------------------------------------------------

AlbumModelKey AlbumModelTest::testKeyRecordAlbum(const QueryRecord& record) const
{
    return keyRecordAlbum(record);
}

//-------------------------------------------------------------------------------------------

QString AlbumModelTest::testNameRecordAlbum(const QueryRecord& record) const
{
    return nameRecordAlbum(record);
}

//-------------------------------------------------------------------------------------------

db::SQLiteQuerySPtr AlbumModelTest::getDBQuery() const
{
    return m_pQuery;
}

//-------------------------------------------------------------------------------------------

bool AlbumModelTest::testRunAlbumQuery(const QString& cmdQ,QueryResult& results) const
{
    return runAlbumQuery(cmdQ,results);
}

//-------------------------------------------------------------------------------------------

bool AlbumModelTest::testPopulate()
{
    return populate();
}

//-------------------------------------------------------------------------------------------

QList<QPair<AlbumModelKey,QString> >& AlbumModelTest::albumList()
{
    return m_albums;
}

//-------------------------------------------------------------------------------------------

QVector<QChar> AlbumModelTest::testGetIndexAlphabet() const
{
    return getIndexAlphabet();
}

//-------------------------------------------------------------------------------------------

void AlbumModelTest::testBuildIndexMap(const QVector<QChar>& alphabet,QMap<QChar,int>& indexMap) const
{
    buildIndexMap(alphabet,indexMap);
}

//-------------------------------------------------------------------------------------------

QChar AlbumModelTest::testGetFirstCharacter(const QString& name,const QMap<QChar,int>& indexMap,const QVector<QChar>& alphabet) const
{
    return getFirstCharacter(name,indexMap,alphabet);
}

//-------------------------------------------------------------------------------------------

void AlbumModelTest::testMapResultsToAlphabetIndex(const QueryResult& results,const QMap<QChar,int>& indexMap,const QVector<QChar>& alphabet,QMap<QChar,QMultiMap<QString,int> >& sectionMap) const
{
    mapResultsToAlphabetIndex(results,indexMap,alphabet,sectionMap);
}

//-------------------------------------------------------------------------------------------

QVector<int>& AlbumModelTest::getPositionIndex()
{
    return m_index;
}

//-------------------------------------------------------------------------------------------

void AlbumModelTest::testInsertIntoAlbum(QVector<QueryRecord>& recordList)
{
    insertIntoAlbum(recordList);
}

//-------------------------------------------------------------------------------------------

void AlbumModelTest::testAddToModelForGivenMap(const QueryResult& results,const QMultiMap<QString,int>& map)
{
    addToModelForGivenMap(results,map);
}

//-------------------------------------------------------------------------------------------

void AlbumModelTest::testBuildModelFromSortedIndex(const QueryResult& results,const QVector<QChar>& alphabet,const QMap<QChar,QMultiMap<QString,int> >& sectionMap)
{
    buildModelFromSortedIndex(results,alphabet,sectionMap);
}

//-------------------------------------------------------------------------------------------

void AlbumModelTest::testEnumerateSections(const QueryResult& results)
{
    enumerateSections(results);
}

//-------------------------------------------------------------------------------------------

bool AlbumModelTest::testCompareIdenticalAlbumNameLessThan(const QueryRecord& a,const QueryRecord& b)
{
    return compareIdenticalAlbumNameLessThan(a,b);
}

//-------------------------------------------------------------------------------------------

bool testUtilitySQLAlbumModel(const QString& cmd,int resultID)
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
        int albumID;
        QString albumName;
        db::SQLiteQuery dbQ(trackDB->db());

        dbQ.prepare(cmd);
        dbQ.bind(isGroupFlag);
        dbQ.bind(albumID);
        dbQ.bind(albumName);
        while(dbQ.next())
        {
            QVector<QVariant> record;
            record.push_back((isGroupFlag) ? 1 : 0);
            record.push_back(albumID);
            record.push_back(albumName);
            results.push_back(record);
        }

        res = resultDB->compareResults(results,"albumModel",resultID);
    }
    catch(const db::SQLiteException& e)
    {
        omega::common::Log::g_Log.print("SQLite exception on query\n%s\n%s\n",cmd.toUtf8().constData(),e.error().toUtf8().constData());
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// An album weither single or as part of larger compiliation are
// is formed by "Album Name" + "Year"
// The groupID is used to form a compiliation album that is kept
// in the same directory space but can be split into multiple
// sub-folders e.g. "Album/CD1", "Album/CD2"
// The AlbumModel should have a list entry for either
// - an album as a single entity
// - an album compiliation
// i.e. where a given album record is part of a compliation
// then an entry for it should not be shown in the model. But
// rather an single entry should be appear for the entire
// album compiliation.
//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryNoAlbumNoKeyUnit)
{
    //SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //       a.albumname
    //  FROM album AS a
    //  GROUP BY id
    //  ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelTest albumModel;
    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,-1)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);

    QString testSQLB = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,-1)));
    testSQLB = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLB);
    EXPECT_TRUE(expectSQL==testSQLB);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryNoAlbumNoKeyIntegration)
{
    AlbumModelTest albumModel;
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,-1)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,1));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryAlbumNoKeyUnit)
{
    //SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //       a.albumname
    //  FROM album AS a
    //  WHERE a.albumID=2
    //  GROUP BY id
    //  ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a";
    expectSQL += "  WHERE a.albumID=2";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelTest albumModel;
    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,2)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryAlbumNoKeyIntegration)
{
    AlbumModelTest albumModel;
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,2));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryGroupNoKeyUnit)
{
    //SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //       a.albumname
    //  FROM album AS a
    //  WHERE a.groupID=5
    //  GROUP BY id
    //  ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a";
    expectSQL += "  WHERE a.groupID=5";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    AlbumModelTest albumModel;
    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,5)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryGroupNoKeyIntegration)
{
    AlbumModelTest albumModel;
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,5)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,3));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryNoAlbumArtistKeyUnit)
{
    // SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //        CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //        a.albumname
    //   FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID
    //   WHERE (b.artist LIKE 'Jeremy Soule' OR
    //          b.originalArtist LIKE 'Jeremy Soule' OR
    //          b.composer LIKE 'Jeremy Soule')
    //   GROUP BY id
    //   ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
    expectSQL += "  WHERE (b.artist LIKE 'Jeremy Soule' OR";
    expectSQL += "         b.originalArtist LIKE 'Jeremy Soule' OR";
    expectSQL += "         b.composer LIKE 'Jeremy Soule')";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "Jeremy Soule";

    AlbumModelTest albumModel(key);

    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,-1)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);

    QString testSQLB = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,-1)));
    testSQLB = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLB);
    EXPECT_TRUE(expectSQL==testSQLB);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryNoAlbumArtistKeyIntegration)
{
    TrackModelKey key;
    key.artist() = "Jeremy Soule";
    AlbumModelTest albumModel(key);
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,-1)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,4));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryAlbumArtistKeyUnit)
{
    // SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //        CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //        a.albumname
    //   FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID
    //   WHERE a.albumID=2 AND
    //         (b.artist LIKE 'Steve Jablonsky' OR
    //          b.originalArtist LIKE 'Steve Jablonsky' OR
    //          b.composer LIKE 'Steve Jablonsky')
    //   GROUP BY id
    //   ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
    expectSQL += "  WHERE a.albumID=2 AND";
    expectSQL += "        (b.artist LIKE 'Steve Jablonsky' OR";
    expectSQL += "         b.originalArtist LIKE 'Steve Jablonsky' OR";
    expectSQL += "         b.composer LIKE 'Steve Jablonsky')";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "Steve Jablonsky";

    AlbumModelTest albumModel(key);

    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,2)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);

    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryAlbumArtistKeyIntegration)
{
    TrackModelKey key;
    key.artist() = "Steve Jablonsky";
    AlbumModelTest albumModel(key);
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,5));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryGroupArtistKeyUnit)
{
    // SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //        CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //        a.albumname
    //   FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID
    //   WHERE a.groupID=10 AND
    //         (b.artist LIKE 'Jeremy Soule' OR
    //          b.originalArtist LIKE 'Jeremy Soule' OR
    //          b.composer LIKE 'Jeremy Soule')
    //   GROUP BY id
    //   ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
    expectSQL += "  WHERE a.groupID=10 AND";
    expectSQL += "        (b.artist LIKE 'Jeremy Soule' OR";
    expectSQL += "         b.originalArtist LIKE 'Jeremy Soule' OR";
    expectSQL += "         b.composer LIKE 'Jeremy Soule')";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "Jeremy Soule";

    AlbumModelTest albumModel(key);

    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,10)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryGroupArtistKeyIntegration)
{
    TrackModelKey key;
    key.artist() = "Jeremy Soule";
    AlbumModelTest albumModel(key);
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,10)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,6));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryNoAlbumArtistGenreKeyUnit)
{
    // SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //        CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //        a.albumname
    //   FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID
    //   WHERE b.genreID=1 AND
    //         (b.artist LIKE 'John Williams' OR
    //          b.originalArtist LIKE 'John Williams' OR
    //          b.composer LIKE 'John Williams')
    //   GROUP BY id
    //   ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
    expectSQL += "  WHERE b.genreID=1 AND";
    expectSQL += "        (b.artist LIKE 'John Williams' OR";
    expectSQL += "         b.originalArtist LIKE 'John Williams' OR";
    expectSQL += "         b.composer LIKE 'John Williams')";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "John Williams";
    key.genre() = 1;

    AlbumModelTest albumModel(key);

    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,-1)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);

    QString testSQLB = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,-1)));
    testSQLB = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLB);
    EXPECT_TRUE(expectSQL==testSQLB);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryNoAlbumArtistGenreKeyIntegration)
{
    TrackModelKey key;
    key.artist() = "John Williams";
    key.genre() = 1;
    AlbumModelTest albumModel(key);
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,-1)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,7));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryAlbumArtistGenreKeyUnit)
{
    // SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //        CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //        a.albumname
    //   FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID
    //   WHERE a.albumID=2 AND
    //         b.genreID=3 AND
    //         (b.artist LIKE 'Steve Jablonsky' OR
    //          b.originalArtist LIKE 'Steve Jablonsky' OR
    //          b.composer LIKE 'Steve Jablonsky')
    //   GROUP BY id
    //   ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
    expectSQL += "  WHERE a.albumID=2 AND";
    expectSQL += "        b.genreID=3 AND";
    expectSQL += "        (b.artist LIKE 'Steve Jablonsky' OR";
    expectSQL += "         b.originalArtist LIKE 'Steve Jablonsky' OR";
    expectSQL += "         b.composer LIKE 'Steve Jablonsky')";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "Steve Jablonsky";
    key.genre() = 3;

    AlbumModelTest albumModel(key);

    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,2)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryAlbumArtistGenreKeyIntegration)
{
    TrackModelKey key;
    key.artist() = "Steve Jablonsky";
    key.genre() = 3;
    AlbumModelTest albumModel(key);
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,8));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryGroupArtistGenreKeyUnit)
{
    // SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //        CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //        a.albumname
    //   FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID
    //   WHERE a.groupID=20 AND
    //         b.genreID=1 AND
    //         (b.artist LIKE 'John Williams' OR
    //          b.originalArtist LIKE 'John Williams' OR
    //          b.composer LIKE 'John Williams')
    //   GROUP BY id
    //   ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
    expectSQL += "  WHERE a.groupID=20 AND";
    expectSQL += "        b.genreID=1 AND";
    expectSQL += "        (b.artist LIKE 'John Williams' OR";
    expectSQL += "         b.originalArtist LIKE 'John Williams' OR";
    expectSQL += "         b.composer LIKE 'John Williams')";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.artist() = "John Williams";
    key.genre() = 1;

    AlbumModelTest albumModel(key);

    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,20)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryGroupArtistGenreKeyIntegration)
{
    TrackModelKey key;
    key.artist() = "John Williams";
    key.genre() = 1;
    AlbumModelTest albumModel(key);
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,20)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,9));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryNoAlbumGenreKeyUnit)
{
    // SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //        CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //        a.albumname
    //   FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID
    //   WHERE b.genreID=1
    //   GROUP BY id
    //   ORDER BY a.albumname

    QString expectSQL;

    expectSQL = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
    expectSQL += "  WHERE b.genreID=1";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.genre() = 1;

    AlbumModelTest albumModel(key);

    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,-1)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);

    QString testSQLB = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,-1)));
    testSQLB = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLB);
    EXPECT_TRUE(expectSQL==testSQLB);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryNoAlbumGenreKeyIntegration)
{
    TrackModelKey key;
    key.genre() = 1;
    AlbumModelTest albumModel(key);
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,-1)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,10));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryAlbumGenreKeyUnit)
{
    // SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //        CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //        a.albumname
    //   FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID
    //   WHERE a.albumID=3 AND b.genreID=1
    //   GROUP BY id
    //   ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
    expectSQL += "  WHERE a.albumID=3 AND b.genreID=1";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.genre() = 1;

    AlbumModelTest albumModel(key);

    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,3)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryAlbumGenreKeyIntergration)
{
    TrackModelKey key;
    key.genre() = 1;
    AlbumModelTest albumModel(key);
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,11));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryGroupGenreKeyUnit)
{
    // SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,
    //        CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,
    //        a.albumname
    //   FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID
    //   WHERE a.groupID=20 AND b.genreID=1
    //   GROUP BY id
    //   ORDER BY a.albumname

    QString expectSQL;

    expectSQL  = "SELECT CASE WHEN a.groupid>=0 THEN 1 ELSE 0 END AS flag,";
    expectSQL += "       CASE WHEN a.groupid>=0 THEN a.groupid ELSE a.albumid END AS id,";
    expectSQL += "       a.albumname";
    expectSQL += "  FROM album AS a INNER JOIN track AS b ON a.albumID=b.albumID";
    expectSQL += "  WHERE a.groupID=20 AND b.genreID=1";
    expectSQL += "  GROUP BY id";
    expectSQL += "  ORDER BY a.albumname";

    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);

    TrackModelKey key;
    key.genre() = 1;

    AlbumModelTest albumModel(key);

    QString testSQLA = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,20)));
    testSQLA = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQLA);
    EXPECT_TRUE(expectSQL==testSQLA);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getQueryGroupGenreKeyIntergration)
{
    TrackModelKey key;
    key.genre() = 1;
    AlbumModelTest albumModel(key);
    QString testSQL = albumModel.testGetQuery(AlbumModelKey(std::pair<bool,int>(true,20)));
    EXPECT_TRUE(testUtilitySQLAlbumModel(testSQL,12));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,createRecordAlbum)
{
    AlbumModelTest model;

    AlbumModelKey keyA(std::pair<bool,int>(true,2));
    QueryRecord recordA = model.testCreateRecordAlbum(keyA,"Record A");
    EXPECT_TRUE(model.testKeyRecordAlbum(recordA)==AlbumModelKey(std::pair<bool,int>(true,2)));
    EXPECT_TRUE(model.testNameRecordAlbum(recordA)=="Record A");

    AlbumModelKey keyB(std::pair<bool,int>(false,3));
    QueryRecord recordB = model.testCreateRecordAlbum(keyB,"Record B");
    EXPECT_TRUE(model.testKeyRecordAlbum(recordB)==AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(model.testNameRecordAlbum(recordB)=="Record B");
}

//-------------------------------------------------------------------------------------------

class AMTestAlbumNameQuery
{
    public:
        bool *m_isGroup;
        int *m_albumID;
        QString *m_albumName;

        void bindIsGroup(bool& v) {m_isGroup = &v;}
        void bindAlbumID(int& v) {m_albumID = &v;}
        void bindAlbumName(QString& v) {m_albumName = &v;}
        bool nextA();
        bool nextB();
};

bool AMTestAlbumNameQuery::nextA()
{
    *m_isGroup = true;
    *m_albumID = 15;
    *m_albumName = "Star Wars";
    return true;
}

bool AMTestAlbumNameQuery::nextB()
{
    *m_isGroup = false;
    *m_albumID = 2;
    *m_albumName = "Transformers";
    return true;
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,runAlbumQuery)
{
    AMTestAlbumNameQuery rMock;
    db::SQLiteQuerySPtr queryMock(new db::SQLiteQueryMock(db::TrackDB::instance()->db()));
    db::SQLiteQueryMock& query = dynamic_cast<db::SQLiteQueryMock&>(*(queryMock.data()));

    QString cmdQ;
    AlbumModelTest a(queryMock);

    cmdQ = "SELECT mock artist count";

    EXPECT_CALL(query,prepare(cmdQ)).Times(1);
    EXPECT_CALL(query,bind(A<bool&>())).Times(1)
            .WillOnce(Invoke(&rMock,&AMTestAlbumNameQuery::bindIsGroup));
    EXPECT_CALL(query,bind(A<int&>())).Times(1)
            .WillOnce(Invoke(&rMock,&AMTestAlbumNameQuery::bindAlbumID));
    EXPECT_CALL(query,bind(A<QString&>())).Times(1)
            .WillOnce(Invoke(&rMock,&AMTestAlbumNameQuery::bindAlbumName));
    EXPECT_CALL(query,next()).Times(3)
            .WillOnce(Invoke(&rMock,&AMTestAlbumNameQuery::nextA))
            .WillOnce(Invoke(&rMock,&AMTestAlbumNameQuery::nextB))
            .WillOnce(Return(false));

    QueryResult results;
    EXPECT_TRUE(a.testRunAlbumQuery(cmdQ,results));

    EXPECT_TRUE(results.size()==2);

    const QueryRecord& recordA = results.at(0);
    EXPECT_TRUE(a.testKeyRecordAlbum(recordA)==AlbumModelKey(std::pair<bool,int>(true,15)));
    EXPECT_TRUE(a.testNameRecordAlbum(recordA)=="Star Wars");

    const QueryRecord& recordB = results.at(1);
    EXPECT_TRUE(a.testKeyRecordAlbum(recordB)==AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(a.testNameRecordAlbum(recordB)=="Transformers");
}

//-------------------------------------------------------------------------------------------

class AlbumModelPopulateTest : public AlbumModelTest
{
    public:
        MOCK_CONST_METHOD1(getQuery,QString(const AlbumModelKey& albumID));
        MOCK_CONST_METHOD2(runAlbumQuery,bool(const QString& cmdQ,QueryResult& results));
        MOCK_METHOD1(enumerateSections,void(const QueryResult& results));
};

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,populateFailOnQueryError)
{
    AlbumModelPopulateTest model;

    AlbumModelKey nullKey;
    QString cmdQ = "SELECT mock";

    QueryResult testResults;

    EXPECT_CALL(model,getQuery(nullKey)).Times(1).WillOnce(Return(cmdQ));
    EXPECT_CALL(model,runAlbumQuery(cmdQ,A<QueryResult&>())).Times(1)
        .WillOnce(Return(false));

    EXPECT_FALSE(model.testPopulate());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,populateSuccess)
{
    AlbumModelPopulateTest model;

    AlbumModelKey nullKey;
    QString cmdQ = "SELECT mock";

    QueryResult testResults;
    testResults.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,15)),"Album A"));
    testResults.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Album B"));

    model.albumList().push_back(QPair<AlbumModelKey,QString>(AlbumModelKey(std::pair<bool,int>(true,15)),"Album A"));

    EXPECT_CALL(model,getQuery(nullKey)).Times(1).WillOnce(Return(cmdQ));
    EXPECT_CALL(model,runAlbumQuery(cmdQ,A<QueryResult&>())).Times(1)
        .WillOnce(DoAll(SetArgReferee<1>(testResults),Return(true)));
    EXPECT_CALL(model,enumerateSections(A<const QueryResult&>())).Times(1);

    EXPECT_TRUE(model.testPopulate());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanGroupAlbumIDLessThan)
{
    AlbumModelTest model;
    // (a.group,b.album) (a.id<b.id) -> false
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,1)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    EXPECT_FALSE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanGroupAlbumIDEqual)
{
    AlbumModelTest model;
    // (a.group,b.album) (a.id=b.id) -> false
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,1)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    EXPECT_FALSE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanGroupAlbumIDGreaterThan)
{
    AlbumModelTest model;
    // (a.group,b.album) (a.id>b.id) -> false
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,2)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    EXPECT_FALSE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanAlbumGroupIDLessThan)
{
    AlbumModelTest model;
    // (a.album,b.group) (a.id<b.id) -> true
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,2)),"Name");
    EXPECT_TRUE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanAlbumGroupIDEqual)
{
    AlbumModelTest model;
    // (a.album,b.group) (a.id=b.id) -> true
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,1)),"Name");
    EXPECT_TRUE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanAlbumGroupIDGreaterThan)
{
    AlbumModelTest model;
    // (a.album,b.group) (a.id>b.id) -> true
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,1)),"Name");
    EXPECT_TRUE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanGroupGroupIDLessThan)
{
    AlbumModelTest model;
    // (a.group,b.group) (a.id<b.id) -> true
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,1)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,2)),"Name");
    EXPECT_TRUE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanGroupGroupIDEqual)
{
    AlbumModelTest model;
    // (a.group,b.group) (a.id=b.id) -> false
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,1)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,1)),"Name");
    EXPECT_FALSE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanGroupGroupIDGreaterThan)
{
    AlbumModelTest model;
    // (a.group,b.group) (a.id>b.id) -> false
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,2)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,1)),"Name");
    EXPECT_FALSE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanAlbumAlbumIDLessThan)
{
    AlbumModelTest model;
    // (a.album,b.album) (a.id<b.id) -> true
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    EXPECT_TRUE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanAlbumAlbumIDEqual)
{
    AlbumModelTest model;
    // (a.album,b.album) (a.id=b.id) -> false
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    EXPECT_FALSE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,compareIdenticalAlbumNameLessThanAlbumAlbumIDGreaterThan)
{
    AlbumModelTest model;
    // (a.album,b.album) (a.id>b.id) -> false
    QueryRecord recA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    QueryRecord recB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    EXPECT_FALSE(AlbumModelTest::testCompareIdenticalAlbumNameLessThan(recA,recB));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getIndexAlphabetIsEnglish)
{
    const tchar expectAlphabet[] = {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', // 10
        'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', // 20
        'u', 'v', 'w', 'x', 'y', 'z', '#' // 27
    };

    int i;
    AlbumModelTest model;
    QVector<QChar> testAlphabet = model.testGetIndexAlphabet();
    QVector<QChar>::const_iterator ppI;

    EXPECT_TRUE(testAlphabet.size()==27);
    for(i=0,ppI=testAlphabet.begin();ppI!=testAlphabet.end();i++,ppI++)
    {
        const QChar& v = *ppI;
        EXPECT_TRUE(v==QChar(expectAlphabet[i]));
    }
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getIndexAlphabetIsLower)
{
    AlbumModelTest model;
    QVector<QChar> testAlphabet = model.testGetIndexAlphabet();
    QVector<QChar>::const_iterator ppI;

    for(ppI=testAlphabet.begin();ppI!=testAlphabet.end();ppI++)
    {
        const QChar& v = *ppI;
        EXPECT_TRUE(v==v.toLower());
    }
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,buildIndexMapNoIndexWhenEmpty)
{
    QVector<QChar> alphabet;
    QMap<QChar,int> indexMap;
    AlbumModelTest model;
    model.testBuildIndexMap(alphabet,indexMap);
    EXPECT_TRUE(indexMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,buildIndexMapNoDuplicates)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('c'));

    QMap<QChar,int> indexMap;
    QMap<QChar,int>::iterator ppI;

    AlbumModelTest model;
    model.testBuildIndexMap(alphabet,indexMap);

    EXPECT_TRUE(indexMap.size()==3);

    ppI = indexMap.find(QChar('a'));
    EXPECT_TRUE(ppI!=indexMap.end());
    EXPECT_TRUE(ppI.key()==QChar('a'));
    EXPECT_TRUE(ppI.value()==1);

    ppI = indexMap.find(QChar('b'));
    EXPECT_TRUE(ppI!=indexMap.end());
    EXPECT_TRUE(ppI.key()==QChar('b'));
    EXPECT_TRUE(ppI.value()==3);

    ppI = indexMap.find(QChar('c'));
    EXPECT_TRUE(ppI!=indexMap.end());
    EXPECT_TRUE(ppI.key()==QChar('c'));
    EXPECT_TRUE(ppI.value()==5);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,buildIndexMapIndexIsOrderOfGivenAlphabet)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('c'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('a'));

    QMap<QChar,int> indexMap;
    QMap<QChar,int>::iterator ppI;

    AlbumModelTest model;
    model.testBuildIndexMap(alphabet,indexMap);

    EXPECT_TRUE(indexMap.size()==3);

    ppI = indexMap.find(QChar('c'));
    EXPECT_TRUE(ppI!=indexMap.end());
    EXPECT_TRUE(ppI.key()==QChar('c'));
    EXPECT_TRUE(ppI.value()==0);

    ppI = indexMap.find(QChar('b'));
    EXPECT_TRUE(ppI!=indexMap.end());
    EXPECT_TRUE(ppI.key()==QChar('b'));
    EXPECT_TRUE(ppI.value()==1);

    ppI = indexMap.find(QChar('a'));
    EXPECT_TRUE(ppI!=indexMap.end());
    EXPECT_TRUE(ppI.key()==QChar('a'));
    EXPECT_TRUE(ppI.value()==2);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getFirstCharacterLastCharacterGivenEmptyName)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('#'));

    AlbumModelTest model;
    QMap<QChar,int> indexMap;
    model.testBuildIndexMap(alphabet,indexMap);

    EXPECT_TRUE(model.testGetFirstCharacter("",indexMap,alphabet)==QChar('#'));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getFirstCharacterIsFirstCharacterOfTrimmedName)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('#'));

    AlbumModelTest model;
    QMap<QChar,int> indexMap;
    model.testBuildIndexMap(alphabet,indexMap);

    EXPECT_TRUE(model.testGetFirstCharacter(" Bravo ",indexMap,alphabet)==QChar('b'));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getFirstCharacterSingleCharaterNameLower)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('#'));

    AlbumModelTest model;
    QMap<QChar,int> indexMap;
    model.testBuildIndexMap(alphabet,indexMap);

    EXPECT_TRUE(model.testGetFirstCharacter("c",indexMap,alphabet)==QChar('c'));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getFirstCharacterSingleCharaterNameUpper)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('#'));

    AlbumModelTest model;
    QMap<QChar,int> indexMap;
    model.testBuildIndexMap(alphabet,indexMap);

    EXPECT_TRUE(model.testGetFirstCharacter("C",indexMap,alphabet)==QChar('c'));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getFirstCharacterNameLower)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('#'));

    AlbumModelTest model;
    QMap<QChar,int> indexMap;
    model.testBuildIndexMap(alphabet,indexMap);

    EXPECT_TRUE(model.testGetFirstCharacter("bravo",indexMap,alphabet)==QChar('b'));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getFirstCharacterNameUpper)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('#'));

    AlbumModelTest model;
    QMap<QChar,int> indexMap;
    model.testBuildIndexMap(alphabet,indexMap);

    EXPECT_TRUE(model.testGetFirstCharacter("Bravo",indexMap,alphabet)==QChar('b'));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getFirstCharacterWhenFirstCharacterNotInAlphabetUpper)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('#'));

    AlbumModelTest model;
    QMap<QChar,int> indexMap;
    model.testBuildIndexMap(alphabet,indexMap);

    EXPECT_TRUE(model.testGetFirstCharacter("Delta",indexMap,alphabet)==QChar('#'));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,getFirstCharacterWhenFirstCharacterNotInAlphabetLower)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('#'));

    AlbumModelTest model;
    QMap<QChar,int> indexMap;
    model.testBuildIndexMap(alphabet,indexMap);

    EXPECT_TRUE(model.testGetFirstCharacter("delta",indexMap,alphabet)==QChar('#'));
}

//-------------------------------------------------------------------------------------------

class AlbumModelMapResultsTest : public AlbumModelTest
{
    public:
        MOCK_CONST_METHOD1(nameRecordAlbum,QString(const QueryRecord& record));
        MOCK_CONST_METHOD3(getFirstCharacter,QChar(const QString& name,const QMap<QChar,int>& indexMap,const QVector<QChar>& alphabet));
};

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,mapResultsToAlphabetIndexNoResultsEmptyMap)
{
    QueryResult mResults;
    AlbumModelMapResultsTest model;

    QMap<QChar,int> indexMap;
    QVector<QChar> alphabet;

    QMap<QChar,QMultiMap<QString,int> > sectionMap;
    sectionMap.insert(QChar('a'),QMultiMap<QString,int>());

    model.testMapResultsToAlphabetIndex(mResults,indexMap,alphabet,sectionMap);

    EXPECT_TRUE(sectionMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,mapResultsToAlphabetIndexDuplicateAlbumNameButDifferentCaseBothInMap)
{
    QueryResult mResults;
    for(int i=0;i<2;i++)
    {
        mResults.push_back(QueryRecord());
    }

    AlbumModelMapResultsTest model;
    EXPECT_CALL(model,nameRecordAlbum(A<const QueryRecord&>())).Times(2)
        .WillOnce(Return("Alpha"))
        .WillOnce(Return("aLPHA"));
    EXPECT_CALL(model,getFirstCharacter(A<const QString&>(),A<const QMap<QChar,int>&>(),A<const QVector<QChar>&>())).Times(2)
        .WillOnce(Return(QChar('a')))
        .WillOnce(Return(QChar('a')));

    QMap<QChar,int> indexMap;
    QVector<QChar> alphabet;
    QMap<QChar,QMultiMap<QString,int> > sectionMap;
    QMap<QChar, QMultiMap<QString,int> >::iterator ppI;

    model.testMapResultsToAlphabetIndex(mResults,indexMap,alphabet,sectionMap);

    EXPECT_TRUE(sectionMap.size()==1);

    ppI = sectionMap.find(QChar('a'));
    EXPECT_TRUE(ppI!=sectionMap.end());
    QMultiMap<QString,int>& mapA = ppI.value();
    EXPECT_TRUE(mapA.size()==2);
    for(QMultiMap<QString,int>::iterator ppI=mapA.begin();ppI!=mapA.end();ppI++)
    {
        EXPECT_TRUE(ppI.key()=="alpha");
        EXPECT_TRUE(ppI.value()==0 || ppI.value()==1);
    }
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,mapResultsToAlphabetIndexAlbumsWithDifferentFirstLettersInSeperateMaps)
{
    QueryResult mResults;
    for(int i=0;i<3;i++)
    {
        mResults.push_back(QueryRecord());
    }

    AlbumModelMapResultsTest model;
    EXPECT_CALL(model,nameRecordAlbum(A<const QueryRecord&>())).Times(3)
        .WillOnce(Return("Alpha"))
        .WillOnce(Return("Charlie"))
        .WillOnce(Return("Bravo"));
    EXPECT_CALL(model,getFirstCharacter(A<const QString&>(),A<const QMap<QChar,int>&>(),A<const QVector<QChar>&>())).Times(3)
        .WillOnce(Return(QChar('a')))
        .WillOnce(Return(QChar('c')))
        .WillOnce(Return(QChar('b')));

    QMap<QChar,int> indexMap;
    QVector<QChar> alphabet;
    QMap<QChar,QMultiMap<QString,int> > sectionMap;
    QMap<QChar, QMultiMap<QString,int> >::iterator ppI;

    model.testMapResultsToAlphabetIndex(mResults,indexMap,alphabet,sectionMap);

    EXPECT_TRUE(sectionMap.size()==3);

    ppI = sectionMap.find(QChar('a'));
    EXPECT_TRUE(ppI!=sectionMap.end());
    QMultiMap<QString,int>& mapA = ppI.value();
    EXPECT_TRUE(mapA.size()==1);
    EXPECT_TRUE(mapA.find("alpha").value()==0);

    ppI = sectionMap.find(QChar('b'));
    EXPECT_TRUE(ppI!=sectionMap.end());
    QMultiMap<QString,int>& mapB = ppI.value();
    EXPECT_TRUE(mapB.size()==1);
    EXPECT_TRUE(mapB.find("bravo").value()==2);

    ppI = sectionMap.find(QChar('c'));
    EXPECT_TRUE(ppI!=sectionMap.end());
    QMultiMap<QString,int>& mapC = ppI.value();
    EXPECT_TRUE(mapC.size()==1);
    EXPECT_TRUE(mapC.find("charlie").value()==1);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,mapResultsToAlphabetIndexAlbumsWithSameFirstLettersAreMappedInToTheirRespectiveMaps)
{
    QueryResult mResults;
    for(int i=0;i<6;i++)
    {
        mResults.push_back(QueryRecord());
    }

    AlbumModelMapResultsTest model;
    EXPECT_CALL(model,nameRecordAlbum(A<const QueryRecord&>())).Times(6)
        .WillOnce(Return("Alpha"))
        .WillOnce(Return(" Charlie"))
        .WillOnce(Return(" bravo "))
        .WillOnce(Return("Beta"))
        .WillOnce(Return(" Amazon "))
        .WillOnce(Return(" Copper"));
    EXPECT_CALL(model,getFirstCharacter(A<const QString&>(),A<const QMap<QChar,int>&>(),A<const QVector<QChar>&>())).Times(6)
        .WillOnce(Return(QChar('a')))
        .WillOnce(Return(QChar('c')))
        .WillOnce(Return(QChar('b')))
        .WillOnce(Return(QChar('b')))
        .WillOnce(Return(QChar('a')))
        .WillOnce(Return(QChar('c')));

    QMap<QChar,int> indexMap;
    QVector<QChar> alphabet;
    QMap<QChar,QMultiMap<QString,int> > sectionMap;
    QMap<QChar, QMultiMap<QString,int> >::iterator ppI;

    model.testMapResultsToAlphabetIndex(mResults,indexMap,alphabet,sectionMap);

    EXPECT_TRUE(sectionMap.size()==3);

    ppI = sectionMap.find(QChar('a'));
    EXPECT_TRUE(ppI!=sectionMap.end());
    QMultiMap<QString,int>& mapA = ppI.value();
    EXPECT_TRUE(mapA.size()==2);
    EXPECT_TRUE(mapA.find("alpha").value()==0);
    EXPECT_TRUE(mapA.find("amazon").value()==4);

    ppI = sectionMap.find(QChar('b'));
    EXPECT_TRUE(ppI!=sectionMap.end());
    QMultiMap<QString,int>& mapB = ppI.value();
    EXPECT_TRUE(mapB.size()==2);
    EXPECT_TRUE(mapB.find("bravo").value()==2);
    EXPECT_TRUE(mapB.find("beta").value()==3);

    ppI = sectionMap.find(QChar('c'));
    EXPECT_TRUE(ppI!=sectionMap.end());
    QMultiMap<QString,int>& mapC = ppI.value();
    EXPECT_TRUE(mapC.size()==2);
    EXPECT_TRUE(mapC.find("charlie").value()==1);
    EXPECT_TRUE(mapC.find("copper").value()==5);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,insertIntoAlbumNoInsertionWithEmptyList)
{
    AlbumModelTest model;
    QVector<QueryRecord> results;
    model.testInsertIntoAlbum(results);
    EXPECT_TRUE(model.albumList().size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,insertIntoAlbumGivenOneAlbum)
{
    AlbumModelTest model;
    QueryRecord recordA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");

    QVector<QueryRecord> results;
    results.push_back(recordA);

    model.testInsertIntoAlbum(results);

    EXPECT_TRUE(model.albumList().size()==1);
    EXPECT_TRUE(model.albumList().at(0).first==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.albumList().at(0).second=="Name");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,insertIntoAlbumGivenMultipleAlbumsSortedByAlbumID)
{
    AlbumModelTest model;
    QueryRecord recordA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    QueryRecord recordB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    QueryRecord recordC = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,3)),"Name");
    QueryRecord recordD = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,1)),"Name");
    QueryRecord recordE = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,2)),"Name");
    QueryRecord recordF = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(true,3)),"Name");

    QVector<QueryRecord> results;
    results.push_back(recordF);
    results.push_back(recordC);
    results.push_back(recordE);
    results.push_back(recordB);
    results.push_back(recordD);
    results.push_back(recordA);

    model.testInsertIntoAlbum(results);

    EXPECT_TRUE(model.albumList().size()==6);
    EXPECT_TRUE(model.albumList().at(0).first==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.albumList().at(1).first==AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(model.albumList().at(2).first==AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(model.albumList().at(3).first==AlbumModelKey(std::pair<bool,int>(true,1)));
    EXPECT_TRUE(model.albumList().at(4).first==AlbumModelKey(std::pair<bool,int>(true,2)));
    EXPECT_TRUE(model.albumList().at(5).first==AlbumModelKey(std::pair<bool,int>(true,3)));
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,addToModelForGivenMapWhenMapIsEmpty)
{
    AlbumModelTest model;
    QueryResult result;
    QMultiMap<QString,int> sectionMap;

    model.testAddToModelForGivenMap(result,sectionMap);

    EXPECT_TRUE(model.albumList().size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,addToModelForGivenMapGivenSingleName)
{
    AlbumModelTest model;

    QueryResult result;
    QueryRecord recordA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    result.push_back(recordA);

    QMultiMap<QString,int> sectionMap;
    sectionMap.insert("name",0);

    model.testAddToModelForGivenMap(result,sectionMap);

    EXPECT_TRUE(model.albumList().size()==1);
    EXPECT_TRUE(model.albumList().at(0).first==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.albumList().at(0).second=="Name");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,addToModelForGivenMapGivenDuplicateNameTwice)
{
    AlbumModelTest model;

    QueryResult result;
    QueryRecord recordA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    QueryRecord recordB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    result.push_back(recordA);
    result.push_back(recordB);

    QMultiMap<QString,int> sectionMap;
    sectionMap.insert("name",0);
    sectionMap.insert("name",1);

    model.testAddToModelForGivenMap(result,sectionMap);

    EXPECT_TRUE(model.albumList().size()==2);
    EXPECT_TRUE(model.albumList().at(0).first==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.albumList().at(0).second=="Name");
    EXPECT_TRUE(model.albumList().at(1).first==AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(model.albumList().at(1).second=="Name");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,addToModelForGivenMapGivenDuplicateNameFourTimes)
{
    AlbumModelTest model;

    QueryResult result;
    QueryRecord recordA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    QueryRecord recordB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    QueryRecord recordC = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,3)),"Name");
    QueryRecord recordD = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,4)),"Name");
    result.push_back(recordA);
    result.push_back(recordB);
    result.push_back(recordC);
    result.push_back(recordD);

    QMultiMap<QString,int> sectionMap;
    sectionMap.insert("name",0);
    sectionMap.insert("name",1);
    sectionMap.insert("name",2);
    sectionMap.insert("name",3);

    model.testAddToModelForGivenMap(result,sectionMap);

    EXPECT_TRUE(model.albumList().size()==4);
    EXPECT_TRUE(model.albumList().at(0).first==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.albumList().at(0).second=="Name");
    EXPECT_TRUE(model.albumList().at(1).first==AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(model.albumList().at(1).second=="Name");
    EXPECT_TRUE(model.albumList().at(2).first==AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(model.albumList().at(2).second=="Name");
    EXPECT_TRUE(model.albumList().at(3).first==AlbumModelKey(std::pair<bool,int>(false,4)));
    EXPECT_TRUE(model.albumList().at(3).second=="Name");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,addToModelForGivenMapDuplicateNameAppended)
{
    AlbumModelTest model;

    QueryResult result;
    QueryRecord recordA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"NameU");
    QueryRecord recordB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    QueryRecord recordC = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,3)),"Name");
    result.push_back(recordA);
    result.push_back(recordB);
    result.push_back(recordC);

    QMultiMap<QString, int> sectionMap;
    sectionMap.insert("nameu",0);
    sectionMap.insert("name",1);
    sectionMap.insert("name",2);

    model.testAddToModelForGivenMap(result,sectionMap);

    EXPECT_TRUE(model.albumList().size()==3);
    EXPECT_TRUE(model.albumList().at(0).first==AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(model.albumList().at(0).second=="Name");
    EXPECT_TRUE(model.albumList().at(1).first==AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(model.albumList().at(1).second=="Name");
    EXPECT_TRUE(model.albumList().at(2).first==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.albumList().at(2).second=="NameU");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,addToModelForGivenMapDuplicateNamePrepended)
{
    AlbumModelTest model;

    QueryResult result;
    QueryRecord recordA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name");
    QueryRecord recordB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    QueryRecord recordC = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,3)),"NameS");
    result.push_back(recordA);
    result.push_back(recordB);
    result.push_back(recordC);

    QMultiMap<QString,int> sectionMap;
    sectionMap.insert("name",0);
    sectionMap.insert("name",1);
    sectionMap.insert("names",2);

    model.testAddToModelForGivenMap(result,sectionMap);

    EXPECT_TRUE(model.albumList().size()==3);
    EXPECT_TRUE(model.albumList().at(0).first==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.albumList().at(0).second=="Name");
    EXPECT_TRUE(model.albumList().at(1).first==AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(model.albumList().at(1).second=="Name");
    EXPECT_TRUE(model.albumList().at(2).first==AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(model.albumList().at(2).second=="NameS");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,addToModelForGivenMapDuplicateNameInTheMiddle)
{
    AlbumModelTest model;

    QueryResult result;
    QueryRecord recordA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"NameU");
    QueryRecord recordB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    QueryRecord recordC = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,3)),"Name");
    QueryRecord recordD = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,4)),"NameS");
    result.push_back(recordA);
    result.push_back(recordB);
    result.push_back(recordC);
    result.push_back(recordD);

    QMultiMap<QString,int> sectionMap;
    sectionMap.insert("nameu",0);
    sectionMap.insert("name",1);
    sectionMap.insert("name",2);
    sectionMap.insert("names",3);

    model.testAddToModelForGivenMap(result,sectionMap);

    EXPECT_TRUE(model.albumList().size()==4);
    EXPECT_TRUE(model.albumList().at(0).first==AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(model.albumList().at(0).second=="Name");
    EXPECT_TRUE(model.albumList().at(1).first==AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(model.albumList().at(1).second=="Name");
    EXPECT_TRUE(model.albumList().at(2).first==AlbumModelKey(std::pair<bool,int>(false,4)));
    EXPECT_TRUE(model.albumList().at(2).second=="NameS");
    EXPECT_TRUE(model.albumList().at(3).first==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.albumList().at(3).second=="NameU");
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,addToModelForGivenMapNoDuplicates)
{
    AlbumModelTest model;

    QueryResult result;
    QueryRecord recordA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"NameU");
    QueryRecord recordB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name");
    QueryRecord recordC = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,3)),"NameS");
    result.push_back(recordA);
    result.push_back(recordB);
    result.push_back(recordC);

    QMultiMap<QString,int> sectionMap;
    sectionMap.insert("nameu",0);
    sectionMap.insert("name",1);
    sectionMap.insert("names",2);

    model.testAddToModelForGivenMap(result,sectionMap);

    EXPECT_TRUE(model.albumList().size()==3);
    EXPECT_TRUE(model.albumList().at(0).first==AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(model.albumList().at(0).second=="Name");
    EXPECT_TRUE(model.albumList().at(1).first==AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(model.albumList().at(1).second=="NameS");
    EXPECT_TRUE(model.albumList().at(2).first==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.albumList().at(2).second=="NameU");
}

//-------------------------------------------------------------------------------------------

class AlbumModelBuildModelSortedIndexTest : public AlbumModelTest
{
    public:
        AlbumModelBuildModelSortedIndexTest();
        MOCK_METHOD2(addToModelForGivenMap,void(const QueryResult& results,const QMultiMap<QString,int>& map));
        void dummyAddToModelForGivenMap(const QueryResult& results,const QMultiMap<QString,int>& map);
    protected:
        int m_count;
};

AlbumModelBuildModelSortedIndexTest::AlbumModelBuildModelSortedIndexTest() : m_count(1)
{}

void AlbumModelBuildModelSortedIndexTest::dummyAddToModelForGivenMap(const QueryResult& results,const QMultiMap<QString,int>& map)
{
    for(int i=0;i<m_count;i++)
    {
        m_albums.append(QPair<AlbumModelKey,QString>(AlbumModelKey(std::pair<bool,int>(false,(i==0) ? 1 : 0)),"Name"));
    }
    m_count++;
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,buildModelFromSortedIndexEmptyModelWhenIndexIsEmpty)
{    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('d'));
    alphabet.append(QChar('e'));
    alphabet.append(QChar('f'));
    alphabet.append(QChar('g'));

    QueryResult results;
    QMap<QChar,QMultiMap<QString,int> > sectionMap;
    AlbumModelBuildModelSortedIndexTest model;

    model.testBuildModelFromSortedIndex(results,alphabet,sectionMap);

    EXPECT_TRUE(model.getPositionIndex().size()==7);
    EXPECT_TRUE(model.getPositionIndex().at(0)==0);
    EXPECT_TRUE(model.getPositionIndex().at(1)==0);
    EXPECT_TRUE(model.getPositionIndex().at(2)==0);
    EXPECT_TRUE(model.getPositionIndex().at(3)==0);
    EXPECT_TRUE(model.getPositionIndex().at(4)==0);
    EXPECT_TRUE(model.getPositionIndex().at(5)==0);
    EXPECT_TRUE(model.getPositionIndex().at(6)==0);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,buildModelFromSortedIndexIsIndexedAtExpectedPositions)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('d'));
    alphabet.append(QChar('e'));
    alphabet.append(QChar('f'));
    alphabet.append(QChar('g'));

    QueryResult results;

    QMultiMap<QString,int> bMap,dMap,fMap;
    QMap<QChar, QMultiMap<QString,int> > sectionMap;
    sectionMap.insert(QChar('b'),bMap);
    sectionMap.insert(QChar('d'),dMap);
    sectionMap.insert(QChar('f'),fMap);

    AlbumModelBuildModelSortedIndexTest model;
    EXPECT_CALL(model,addToModelForGivenMap(A<const QueryResult&>(),A<const QMultiMap<QString,int>& >())).Times(3)
        .WillRepeatedly(Invoke(&model,&AlbumModelBuildModelSortedIndexTest::dummyAddToModelForGivenMap));

    model.testBuildModelFromSortedIndex(results,alphabet,sectionMap);

    EXPECT_TRUE(model.getPositionIndex().size()==7);
    // a - 0
    EXPECT_TRUE(model.getPositionIndex().at(0)==0);
    // b - 0 (1)
    EXPECT_TRUE(model.getPositionIndex().at(1)==0);
    EXPECT_TRUE(model.albumList().at(0).first.id()==1);
    // c - 1
    EXPECT_TRUE(model.getPositionIndex().at(2)==1);
    // d - 1 (1,2)
    EXPECT_TRUE(model.getPositionIndex().at(3)==1);
    EXPECT_TRUE(model.albumList().at(1).first.id()==1);
    EXPECT_TRUE(model.albumList().at(2).first.id()==0);
    // e - 3
    EXPECT_TRUE(model.getPositionIndex().at(4)==3);
    // f - 3 (1,2,3)
    EXPECT_TRUE(model.getPositionIndex().at(5)==3);
    EXPECT_TRUE(model.albumList().at(3).first.id()==1);
    EXPECT_TRUE(model.albumList().at(4).first.id()==0);
    EXPECT_TRUE(model.albumList().at(5).first.id()==0);
    // g - 6
    EXPECT_TRUE(model.getPositionIndex().at(6)==6);
}

//-------------------------------------------------------------------------------------------

class AlbumModelEnumerateSectionsTest : public AlbumModelTest
{
    public:
        MOCK_CONST_METHOD0(getIndexAlphabet,QVector<QChar>());
        MOCK_CONST_METHOD2(buildIndexMap,void(const QVector<QChar>& alphabet,QMap<QChar,int>& indexMap));
        MOCK_CONST_METHOD4(mapResultsToAlphabetIndex,void(const QueryResult& results,const QMap<QChar,int>& indexMap,const QVector<QChar>& alphabet,QMap<QChar,QMultiMap<QString,int> >& sectionMap));
        MOCK_METHOD3(buildModelFromSortedIndex,void(const QueryResult& results,const QVector<QChar>& alphabet,const QMap<QChar,QMultiMap<QString,int> >& sectionMap));
};

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,enumerateSectionsMethods)
{
    QVector<QChar> alphabet;
    QueryResult result;

    AlbumModelEnumerateSectionsTest model;
    EXPECT_CALL(model,getIndexAlphabet()).Times(1).WillOnce(Return(alphabet));
    EXPECT_CALL(model,buildIndexMap(alphabet,A<QMap<QChar,int>&>())).Times(1);
    EXPECT_CALL(model,mapResultsToAlphabetIndex(A<const QueryResult&>(),A<const QMap<QChar,int>&>(),alphabet,A<QMap<QChar,QMultiMap<QString,int> >& >())).Times(1);
    EXPECT_CALL(model,buildModelFromSortedIndex(A<const QueryResult&>(),alphabet,A<const QMap<QChar,QMultiMap<QString,int> >& >())).Times(1);

    model.testEnumerateSections(result);
}

//-------------------------------------------------------------------------------------------

class AlbumModelEnumerateSectionsResultTest : public AlbumModelTest
{
    public:
        MOCK_CONST_METHOD0(getIndexAlphabet,QVector<QChar>());
};

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,enumerateSectionsResults)
{
    QVector<QChar> alphabet;
    alphabet.append(QChar('a'));
    alphabet.append(QChar('b'));
    alphabet.append(QChar('c'));
    alphabet.append(QChar('#'));

    AlbumModelEnumerateSectionsResultTest model;

    QueryResult result;
    QueryRecord recordA = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Alpha");
    QueryRecord recordB = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Bravo");
    QueryRecord recordC = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,3)),"Bravo");
    QueryRecord recordD = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,4)),"Camp");
    QueryRecord recordE = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,5)),"Charlie");
    QueryRecord recordF = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,6)),"Custard");
    QueryRecord recordG = model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,7)),"Delta");
    result.push_back(recordA);
    result.push_back(recordB);
    result.push_back(recordC);
    result.push_back(recordD);
    result.push_back(recordE);
    result.push_back(recordF);
    result.push_back(recordG);

    EXPECT_CALL(model,getIndexAlphabet()).Times(1).WillOnce(Return(alphabet));

    model.testEnumerateSections(result);

    EXPECT_TRUE(model.albumList().size()==7);
    EXPECT_TRUE(model.albumList().at(0).first==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.albumList().at(0).second=="Alpha");
    EXPECT_TRUE(model.albumList().at(1).first==AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(model.albumList().at(1).second=="Bravo");
    EXPECT_TRUE(model.albumList().at(2).first==AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(model.albumList().at(2).second=="Bravo");
    EXPECT_TRUE(model.albumList().at(3).first==AlbumModelKey(std::pair<bool,int>(false,4)));
    EXPECT_TRUE(model.albumList().at(3).second=="Camp");
    EXPECT_TRUE(model.albumList().at(4).first==AlbumModelKey(std::pair<bool,int>(false,5)));
    EXPECT_TRUE(model.albumList().at(4).second=="Charlie");
    EXPECT_TRUE(model.albumList().at(5).first==AlbumModelKey(std::pair<bool,int>(false,6)));
    EXPECT_TRUE(model.albumList().at(5).second=="Custard");
    EXPECT_TRUE(model.albumList().at(6).first==AlbumModelKey(std::pair<bool,int>(false,7)));
    EXPECT_TRUE(model.albumList().at(6).second=="Delta");

    EXPECT_TRUE(model.getPositionIndex().size()==4);
    EXPECT_TRUE(model.getPositionIndex().at(0)==0);
    EXPECT_TRUE(model.getPositionIndex().at(1)==1);
    EXPECT_TRUE(model.getPositionIndex().at(2)==3);
    EXPECT_TRUE(model.getPositionIndex().at(3)==6);
}

//-------------------------------------------------------------------------------------------

class AlbumModelBuildTest : public AlbumModelTest
{
    public:
        MOCK_METHOD0(populate,bool());
};

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,buildFailure)
{
    AlbumModelBuildTest model;
    EXPECT_CALL(model,populate()).Times(1).WillOnce(Return(false));
    EXPECT_FALSE(model.build());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,buildSuccess)
{
    AlbumModelBuildTest model;
    EXPECT_CALL(model,populate()).Times(1).WillOnce(Return(true));
    EXPECT_TRUE(model.build());
}

//-------------------------------------------------------------------------------------------

void buildTestAlbumModel(AlbumModelTest& model)
{
    QList<QPair<AlbumModelKey,QString> >& albums = model.albumList();
    QVector<int>& index = model.getPositionIndex();

    albums.clear();
    index.clear();

    // 0 - Bravo
    albums.append(QPair<AlbumModelKey,QString>(AlbumModelKey(std::pair<bool,int>(false,1)),"Bravo"));
    // 1 - Cast
    albums.append(QPair<AlbumModelKey,QString>(AlbumModelKey(std::pair<bool,int>(false,2)),"Cast"));
    // 2 - Cut
    albums.append(QPair<AlbumModelKey,QString>(AlbumModelKey(std::pair<bool,int>(false,3)),"Cut"));
    // 3 - Dance
    albums.append(QPair<AlbumModelKey,QString>(AlbumModelKey(std::pair<bool,int>(false,4)),"Dance"));
    // 4 - Delta
    albums.append(QPair<AlbumModelKey,QString>(AlbumModelKey(std::pair<bool,int>(true,5)),"Delta"));
    // 5 - Disco
    albums.append(QPair<AlbumModelKey,QString>(AlbumModelKey(std::pair<bool,int>(false,6)),"Disco"));
    // 6 - Ghost
    albums.append(QPair<AlbumModelKey,QString>(AlbumModelKey(std::pair<bool,int>(true,7)),"Ghost"));
    // 7 - Golf
    albums.append(QPair<AlbumModelKey,QString>(AlbumModelKey(std::pair<bool,int>(true,8)),"Golf"));

    // A
    index.push_back(0);
    // B
    index.push_back(0);
    // C
    index.push_back(1);
    // D
    index.push_back(3);
    // E
    index.push_back(6);
    // F
    index.push_back(6);
    // G
    index.push_back(6);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,numberSectionsIsEmpty)
{
    AlbumModel model;
    EXPECT_TRUE(model.numberSections()==0);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,numberSections)
{
    AlbumModelTest model;
    buildTestAlbumModel(model);
    EXPECT_TRUE(model.numberSections()==7);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,sizeIsEmpty)
{
    AlbumModel model;
    EXPECT_TRUE(model.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,size)
{
    AlbumModelTest model;
    buildTestAlbumModel(model);
    EXPECT_TRUE(model.size()==8);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,numberRowsInSection)
{
    AlbumModelTest model;
    buildTestAlbumModel(model);
    EXPECT_TRUE(model.numberRowsInSection(-1)==0);
    EXPECT_TRUE(model.numberRowsInSection(0)==0);
    EXPECT_TRUE(model.numberRowsInSection(1)==1);
    EXPECT_TRUE(model.numberRowsInSection(2)==2);
    EXPECT_TRUE(model.numberRowsInSection(3)==3);
    EXPECT_TRUE(model.numberRowsInSection(4)==0);
    EXPECT_TRUE(model.numberRowsInSection(5)==0);
    EXPECT_TRUE(model.numberRowsInSection(6)==2);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,dataIsEmpty)
{
    AlbumModel model;
    EXPECT_TRUE(model.data(0,0,0).isNull());
    EXPECT_TRUE(model.data(0,0,1).isNull());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,data)
{
    AlbumModelTest model;
    buildTestAlbumModel(model);
    EXPECT_TRUE(model.data(-1,0,0).isNull());
    EXPECT_TRUE(model.data(-1,0,1).isNull());

    EXPECT_TRUE(model.data(0,-1,0).isNull());
    EXPECT_TRUE(model.data(0,-1,1).isNull());
    EXPECT_TRUE(model.data(0,0,0).isNull());
    EXPECT_TRUE(model.data(0,0,1).isNull());
    EXPECT_TRUE(model.data(0,1,0).isNull());
    EXPECT_TRUE(model.data(0,1,1).isNull());

    EXPECT_TRUE(model.data(1,-1,0).isNull());
    EXPECT_TRUE(model.data(1,-1,1).isNull());
    EXPECT_TRUE(AlbumModelKey(model.data(1,0,0)) == AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.data(1,0,1).toString()=="Bravo");
    EXPECT_TRUE(model.data(1,1,0).isNull());
    EXPECT_TRUE(model.data(1,1,1).isNull());

    EXPECT_TRUE(model.data(2,-1,0).isNull());
    EXPECT_TRUE(model.data(2,-1,1).isNull());
    EXPECT_TRUE(AlbumModelKey(model.data(2,0,0)) == AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(model.data(2,0,1).toString()=="Cast");
    EXPECT_TRUE(AlbumModelKey(model.data(2,1,0)) == AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(model.data(2,1,1).toString()=="Cut");
    EXPECT_TRUE(model.data(2,2,0).isNull());
    EXPECT_TRUE(model.data(2,2,1).isNull());

    EXPECT_TRUE(model.data(3,-1,0).isNull());
    EXPECT_TRUE(model.data(3,-1,1).isNull());
    EXPECT_TRUE(AlbumModelKey(model.data(3,0,0)) == AlbumModelKey(std::pair<bool,int>(false,4)));
    EXPECT_TRUE(model.data(3,0,1).toString()=="Dance");
    EXPECT_TRUE(AlbumModelKey(model.data(3,1,0)) == AlbumModelKey(std::pair<bool,int>(true,5)));
    EXPECT_TRUE(model.data(3,1,1).toString()=="Delta");
    EXPECT_TRUE(AlbumModelKey(model.data(3,2,0)) == AlbumModelKey(std::pair<bool,int>(false,6)));
    EXPECT_TRUE(model.data(3,2,1).toString()=="Disco");
    EXPECT_TRUE(model.data(3,3,0).isNull());
    EXPECT_TRUE(model.data(3,3,1).isNull());

    EXPECT_TRUE(model.data(4,-1,0).isNull());
    EXPECT_TRUE(model.data(4,-1,1).isNull());
    EXPECT_TRUE(model.data(4,0,0).isNull());
    EXPECT_TRUE(model.data(4,0,1).isNull());
    EXPECT_TRUE(model.data(4,1,0).isNull());
    EXPECT_TRUE(model.data(4,1,1).isNull());

    EXPECT_TRUE(model.data(5,-1,0).isNull());
    EXPECT_TRUE(model.data(5,-1,1).isNull());
    EXPECT_TRUE(model.data(5,0,0).isNull());
    EXPECT_TRUE(model.data(5,0,1).isNull());
    EXPECT_TRUE(model.data(5,1,0).isNull());
    EXPECT_TRUE(model.data(5,1,1).isNull());

    EXPECT_TRUE(model.data(6,-1,0).isNull());
    EXPECT_TRUE(model.data(6,-1,1).isNull());
    EXPECT_TRUE(AlbumModelKey(model.data(6,0,0)) == AlbumModelKey(std::pair<bool,int>(true,7)));
    EXPECT_TRUE(model.data(6,0,1).toString()=="Ghost");
    EXPECT_TRUE(AlbumModelKey(model.data(6,1,0)) == AlbumModelKey(std::pair<bool,int>(true,8)));
    EXPECT_TRUE(model.data(6,1,1).toString()=="Golf");
    EXPECT_TRUE(model.data(6,2,0).isNull());
    EXPECT_TRUE(model.data(6,2,1).isNull());

    EXPECT_TRUE(model.data(7,-1,0).isNull());
    EXPECT_TRUE(model.data(7,-1,1).isNull());
    EXPECT_TRUE(model.data(7,0,0).isNull());
    EXPECT_TRUE(model.data(7,0,1).isNull());
    EXPECT_TRUE(model.data(7,1,0).isNull());
    EXPECT_TRUE(model.data(7,1,1).isNull());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,enumerateSectionBuildsExpectedModel)
{
    AlbumModelTest model;
    QueryResult results;

    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),"Name A"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,2)),"Name B"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,3)),"Name C"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,4)),""));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,20)),"123"));

    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,6)),"Bella"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,7)),"Charlie"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,8)),"Delta A"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,9)),"Delta B"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,10)),"Foxtrot"));

    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,11)),"Hotel A"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,12)),"Kilo C"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,13)),"Mike A"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,14)),"Lima B"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,15)),"Juillet B"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,16)),"Kilo A"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,17)),"Kilo B"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,18)),"Lima A"));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,19)),"Juillet A"));

    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,5)),"123"));

    model.testEnumerateSections(results);

    EXPECT_TRUE(model.size()==20);
    EXPECT_TRUE(model.numberSections()==27);

    // A
    EXPECT_TRUE(model.numberRowsInSection(0)==0);
    EXPECT_TRUE(model.data(0,0,0).isNull());
    EXPECT_TRUE(model.data(0,0,1).isNull());

    // B
    // Bella
    EXPECT_TRUE(model.numberRowsInSection(1)==1);
    EXPECT_TRUE(AlbumModelKey(model.data(1,0,0))==AlbumModelKey(std::pair<bool,int>(false,6)));
    EXPECT_TRUE(model.data(1,0,1).toString()=="Bella");
    EXPECT_TRUE(model.data(1,1,0).isNull());
    EXPECT_TRUE(model.data(1,1,1).isNull());

    // C
    // Charlie
    EXPECT_TRUE(model.numberRowsInSection(2)==1);
    EXPECT_TRUE(AlbumModelKey(model.data(2,0,0))==AlbumModelKey(std::pair<bool,int>(false,7)));
    EXPECT_TRUE(model.data(2,0,1).toString()=="Charlie");
    EXPECT_TRUE(model.data(2,1,0).isNull());
    EXPECT_TRUE(model.data(2,1,1).isNull());

    // D
    // Delta A
    // Delta B
    EXPECT_TRUE(model.numberRowsInSection(3)==2);
    EXPECT_TRUE(AlbumModelKey(model.data(3,0,0))==AlbumModelKey(std::pair<bool,int>(false,8)));
    EXPECT_TRUE(model.data(3,0,1).toString()=="Delta A");
    EXPECT_TRUE(AlbumModelKey(model.data(3,1,0))==AlbumModelKey(std::pair<bool,int>(false,9)));
    EXPECT_TRUE(model.data(3,1,1).toString()=="Delta B");
    EXPECT_TRUE(model.data(3,2,0).isNull());
    EXPECT_TRUE(model.data(3,2,1).isNull());

    // E
    EXPECT_TRUE(model.numberRowsInSection(4)==0);
    EXPECT_TRUE(model.data(4,0,0).isNull());
    EXPECT_TRUE(model.data(4,0,1).isNull());

    // F
    // Foxtrot
    EXPECT_TRUE(model.numberRowsInSection(5)==1);
    EXPECT_TRUE(AlbumModelKey(model.data(5,0,0))==AlbumModelKey(std::pair<bool,int>(false,10)));
    EXPECT_TRUE(model.data(5,0,1).toString()=="Foxtrot");
    EXPECT_TRUE(model.data(5,1,0).isNull());
    EXPECT_TRUE(model.data(5,1,1).isNull());

    // G
    EXPECT_TRUE(model.numberRowsInSection(6)==0);
    EXPECT_TRUE(model.data(6,0,0).isNull());
    EXPECT_TRUE(model.data(6,0,1).isNull());

    // H
    // Hotel A
    EXPECT_TRUE(model.numberRowsInSection(7)==1);
    EXPECT_TRUE(AlbumModelKey(model.data(7,0,0))==AlbumModelKey(std::pair<bool,int>(false,11)));
    EXPECT_TRUE(model.data(7,0,1).toString()=="Hotel A");
    EXPECT_TRUE(model.data(7,1,0).isNull());
    EXPECT_TRUE(model.data(7,1,1).isNull());

    // I
    EXPECT_TRUE(model.numberRowsInSection(8)==0);
    EXPECT_TRUE(model.data(8,0,0).isNull());
    EXPECT_TRUE(model.data(8,0,1).isNull());

    // J
    // Juillet A
    // Juillet B
    EXPECT_TRUE(model.numberRowsInSection(9)==2);
    EXPECT_TRUE(AlbumModelKey(model.data(9,0,0))==AlbumModelKey(std::pair<bool,int>(false,19)));
    EXPECT_TRUE(model.data(9,0,1).toString()=="Juillet A");
    EXPECT_TRUE(AlbumModelKey(model.data(9,1,0))==AlbumModelKey(std::pair<bool,int>(false,15)));
    EXPECT_TRUE(model.data(9,1,1).toString()=="Juillet B");
    EXPECT_TRUE(model.data(9,2,0).isNull());
    EXPECT_TRUE(model.data(9,2,1).isNull());

    // K
    // Kilo A
    // Kilo B
    // Kilo C
    EXPECT_TRUE(model.numberRowsInSection(10)==3);
    EXPECT_TRUE(AlbumModelKey(model.data(10,0,0))==AlbumModelKey(std::pair<bool,int>(false,16)));
    EXPECT_TRUE(model.data(10,0,1).toString()=="Kilo A");
    EXPECT_TRUE(AlbumModelKey(model.data(10,1,0))==AlbumModelKey(std::pair<bool,int>(false,17)));
    EXPECT_TRUE(model.data(10,1,1).toString()=="Kilo B");
    EXPECT_TRUE(AlbumModelKey(model.data(10,2,0))==AlbumModelKey(std::pair<bool,int>(false,12)));
    EXPECT_TRUE(model.data(10,2,1).toString()=="Kilo C");
    EXPECT_TRUE(model.data(10,3,0).isNull());
    EXPECT_TRUE(model.data(10,3,1).isNull());

    // L
    // Lima A
    // Lime B
    EXPECT_TRUE(model.numberRowsInSection(11)==2);
    EXPECT_TRUE(AlbumModelKey(model.data(11,0,0))==AlbumModelKey(std::pair<bool,int>(false,18)));
    EXPECT_TRUE(model.data(11,0,1).toString()=="Lima A");
    EXPECT_TRUE(AlbumModelKey(model.data(11,1,0))==AlbumModelKey(std::pair<bool,int>(false,14)));
    EXPECT_TRUE(model.data(11,1,1).toString()=="Lima B");
    EXPECT_TRUE(model.data(11,2,0).isNull());
    EXPECT_TRUE(model.data(11,2,1).isNull());

    // M
    // Mike A
    EXPECT_TRUE(model.numberRowsInSection(12)==1);
    EXPECT_TRUE(AlbumModelKey(model.data(12,0,0))==AlbumModelKey(std::pair<bool,int>(false,13)));
    EXPECT_TRUE(model.data(12,0,1).toString()=="Mike A");
    EXPECT_TRUE(model.data(12,1,0).isNull());
    EXPECT_TRUE(model.data(12,1,1).isNull());

    // N
    EXPECT_TRUE(model.numberRowsInSection(13)==3);
    EXPECT_TRUE(AlbumModelKey(model.data(13,0,0))==AlbumModelKey(std::pair<bool,int>(false,1)));
    EXPECT_TRUE(model.data(13,0,1).toString()=="Name A");
    EXPECT_TRUE(AlbumModelKey(model.data(13,1,0))==AlbumModelKey(std::pair<bool,int>(false,2)));
    EXPECT_TRUE(model.data(13,1,1).toString()=="Name B");
    EXPECT_TRUE(AlbumModelKey(model.data(13,2,0))==AlbumModelKey(std::pair<bool,int>(false,3)));
    EXPECT_TRUE(model.data(13,2,1).toString()=="Name C");
    EXPECT_TRUE(model.data(13,3,0).isNull());
    EXPECT_TRUE(model.data(13,3,1).isNull());

    // O
    EXPECT_TRUE(model.numberRowsInSection(14)==0);
    EXPECT_TRUE(model.data(14,0,0).isNull());
    EXPECT_TRUE(model.data(14,0,1).isNull());

    // P
    EXPECT_TRUE(model.numberRowsInSection(15)==0);
    EXPECT_TRUE(model.data(15,0,0).isNull());
    EXPECT_TRUE(model.data(15,0,1).isNull());

    // Q
    EXPECT_TRUE(model.numberRowsInSection(16)==0);
    EXPECT_TRUE(model.data(16,0,0).isNull());
    EXPECT_TRUE(model.data(16,0,1).isNull());

    // R
    EXPECT_TRUE(model.numberRowsInSection(17)==0);
    EXPECT_TRUE(model.data(17,0,0).isNull());
    EXPECT_TRUE(model.data(17,0,1).isNull());

    // S
    EXPECT_TRUE(model.numberRowsInSection(18)==0);
    EXPECT_TRUE(model.data(18,0,0).isNull());
    EXPECT_TRUE(model.data(18,0,1).isNull());

    // T
    EXPECT_TRUE(model.numberRowsInSection(19)==0);
    EXPECT_TRUE(model.data(19,0,0).isNull());
    EXPECT_TRUE(model.data(19,0,1).isNull());

    // U
    EXPECT_TRUE(model.numberRowsInSection(20)==0);
    EXPECT_TRUE(model.data(20,0,0).isNull());
    EXPECT_TRUE(model.data(20,0,1).isNull());

    // V
    EXPECT_TRUE(model.numberRowsInSection(21)==0);
    EXPECT_TRUE(model.data(21,0,0).isNull());
    EXPECT_TRUE(model.data(21,0,1).isNull());

    // W
    EXPECT_TRUE(model.numberRowsInSection(22)==0);
    EXPECT_TRUE(model.data(22,0,0).isNull());
    EXPECT_TRUE(model.data(22,0,1).isNull());

    // X
    EXPECT_TRUE(model.numberRowsInSection(23)==0);
    EXPECT_TRUE(model.data(23,0,0).isNull());
    EXPECT_TRUE(model.data(23,0,1).isNull());

    // Y
    EXPECT_TRUE(model.numberRowsInSection(24)==0);
    EXPECT_TRUE(model.data(24,0,0).isNull());
    EXPECT_TRUE(model.data(24,0,1).isNull());

    // Z
    EXPECT_TRUE(model.numberRowsInSection(25)==0);
    EXPECT_TRUE(model.data(25,0,0).isNull());
    EXPECT_TRUE(model.data(25,0,1).isNull());

    // #
    // ""
    // 123
    // 123
    EXPECT_TRUE(model.numberRowsInSection(26)==3);
    EXPECT_TRUE(AlbumModelKey(model.data(26,0,0))==AlbumModelKey(std::pair<bool,int>(false,4)));
    EXPECT_TRUE(model.data(26,0,1).toString()=="");
    EXPECT_TRUE(AlbumModelKey(model.data(26,1,0))==AlbumModelKey(std::pair<bool,int>(false,5)));
    EXPECT_TRUE(model.data(26,1,1).toString()=="123");
    EXPECT_TRUE(AlbumModelKey(model.data(26,2,0))==AlbumModelKey(std::pair<bool,int>(false,20)));
    EXPECT_TRUE(model.data(26,2,1).toString()=="123");
    EXPECT_TRUE(model.data(26,3,0).isNull());
    EXPECT_TRUE(model.data(26,3,1).isNull());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModel,modelHandlesUnicode)
{
    AlbumModelTest model;
    QueryResult results;

    tuint16 inTxt[] = { 0xBA0, 0x1C00, 0x1C10, 0x06A0, 0x2230, 0x0000 };
    QString inString = QString::fromUtf16(reinterpret_cast<const char16_t *>(inTxt));
    results.push_back(model.testCreateRecordAlbum(AlbumModelKey(std::pair<bool,int>(false,1)),inString));

    model.testEnumerateSections(results);

    EXPECT_TRUE(model.numberRowsInSection(26)==1);

    QString testString = model.data(26,0,1).toString();

    EXPECT_TRUE(testString==inString);
}

//-------------------------------------------------------------------------------------------

