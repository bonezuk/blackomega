#include "common/inc/DiskOps.h"
#include "track/db/inc/TrackDB.h"
#include "common/inc/JaroWinklerDistance.h"

#include "gtest/gtest.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

class JaroMediaDB
{
    public:
        JaroMediaDB();
        virtual ~JaroMediaDB();
        bool open(const QString& name);
        void close();
        bool getAllTracks(QStringList& trackList);
        bool buildJaroTable(const QStringList& strList);
    private:
        track::db::SQLiteDatabase *m_jaroDB;

        void printError(const tchar *strR, const tchar *strE) const;
        void createJaro();
        bool insertEntry(const QString& str, const QMap<tfloat64, QString>& dMap, bool caseSensitive, int noRecords);
        bool buildJaroEntry(const QString& str, const QStringList& strList);
};

//-------------------------------------------------------------------------------------------

JaroMediaDB::JaroMediaDB() : m_jaroDB(0)
{}

//-------------------------------------------------------------------------------------------

JaroMediaDB::~JaroMediaDB()
{
    JaroMediaDB::close();
}

//-------------------------------------------------------------------------------------------

void JaroMediaDB::printError(const tchar *strR, const tchar *strE) const
{
    common::Log::g_Log << "JaroMediaDB::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool JaroMediaDB::open(const QString& name)
{
    bool res = false;

    close();
    if(common::DiskOps::exist(name))
    {
        common::DiskOps::deleteFile(name);
    }

    m_jaroDB = new track::db::SQLiteDatabase();
    if(m_jaroDB->open(name))
    {
        try
        {
            createJaro();
            res = true;
        }
        catch(const track::db::SQLiteException& e)
        {
            QString err = QString("Failed to build jaro database. %1").arg(e.error());
            printError("open", err.toUtf8().constData());
            res = false;
        }
    }
    else
    {
        QString err = QString("Failed to create SQLite database '%1'").arg(name);
        printError("open", err.toUtf8().constData());
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void JaroMediaDB::close()
{
    if(m_jaroDB != 0)
    {
        m_jaroDB->close();
        delete m_jaroDB;
        m_jaroDB = 0;
    }
}

//-------------------------------------------------------------------------------------------

void JaroMediaDB::createJaro()
{
    QString cmd;

    cmd  = "CREATE TABLE jaro (";
    cmd += "  orgName TEXT NOT NULL COLLATE NOCASE,";
    cmd += "  cmpName TEXT NOT NULL COLLATE NOCASE,";
    cmd += "  dist REAL NOT NULL";
    cmd += ");";
    m_jaroDB->exec(cmd);

    cmd  = "CREATE TABLE jaroNoCase (";
    cmd += "  orgName TEXT NOT NULL COLLATE NOCASE,";
    cmd += "  cmpName TEXT NOT NULL COLLATE NOCASE,";
    cmd += "  dist REAL NOT NULL";
    cmd += ");";
    m_jaroDB->exec(cmd);
}

//-------------------------------------------------------------------------------------------

bool JaroMediaDB::getAllTracks(QStringList& trackList)
{
    bool res = false;

    try
    {
        QString cmd = "SELECT trackName FROM track GROUP BY trackName ORDER BY trackName";
        track::db::SQLiteQuery nameQ(track::db::TrackDB::instance()->db());
        QString name;

        trackList.clear();
        nameQ.prepare(cmd);
        nameQ.bind(name);
        while(nameQ.next())
        {
            trackList.append(name);
        }
        res = true;
    }
    catch(const track::db::SQLiteException& e)
    {
        QString err = QString("Failed to get track names. %1").arg(e.error());
        printError("getAllTracks", err.toUtf8().constData());
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool JaroMediaDB::insertEntry(const QString& str, const QMap<tfloat64, QString>& dMap, bool caseSensitive, int noRecords)
{
    bool res = false;

    try
    {
        int i;
        QString cmd;
        QMap<tfloat64, QString>::const_iterator ppI;
        track::db::SQLiteInsert distI(m_jaroDB);

        cmd  = "INSERT INTO ";
        cmd += (caseSensitive) ? "jaro" : "jaroNoCase";
        cmd += " (orgName, cmpName, dist) VALUES (?,?,?);";
        distI.prepare(cmd);

        ppI = dMap.constEnd();
        for(i = 0; i < noRecords && ppI != dMap.constBegin(); i++)
        {
            --ppI;

            QString strI(str);
            QString cmpName(ppI.value());
            tfloat64 dist = ppI.key();
            distI.bind(strI);
            distI.bind(cmpName);
            distI.bind(dist);
            if(!distI.next())
            {
                printError("insertEntry", "Error inserting distance record");
                return false;
            }
        }
        res = true;
    }
    catch(const track::db::SQLiteException& e)
    {
        QString err = QString("Failed to get insert jaro records. %1").arg(e.error());
        printError("insertEntry", err.toUtf8().constData());
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool JaroMediaDB::buildJaroEntry(const QString& str, const QStringList& strList)
{
    const int c_noRecords = 10;
    tfloat64 d;
    QMap<tfloat64, QString> dMapCase, dMapNoCase;
    common::JaroWinklerDistance dist(str);

    for(QStringList::const_iterator ppI = strList.begin(); ppI != strList.end(); ppI++)
    {
        const QString& comp = *ppI;
        if(str != comp)
        {
            d = dist.distance(comp, true);
            dMapCase.insert(d, comp);
            d = dist.distance(comp, false);
            dMapNoCase.insert(d, comp);
        }
    }
    return insertEntry(str, dMapCase, true, c_noRecords) && insertEntry(str, dMapNoCase, false, c_noRecords);
}

//-------------------------------------------------------------------------------------------

bool JaroMediaDB::buildJaroTable(const QStringList& strList)
{
    int i = 1;
    for(QStringList::const_iterator ppI = strList.begin(); ppI != strList.end(); ppI++)
    {
        const QString& str = *ppI;
        if(!buildJaroEntry(str, strList))
        {
            return false;
        }
        fprintf(stdout, "%d - %s\n", i, str.toUtf8().constData());
        i++;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

TEST(JaroDBTest, buildJaroTracks)
{
    QString trackDBFilename = "/Users/bonez/Development/athena/Build/dbjaro/track_playlist_dev.db";
    QString jaroDBFilename = "/Users/bonez/Development/athena/Build/dbjaro/track_jaro.db";

    track::db::TrackDB *trackDB = track::db::TrackDB::instance();
    delete trackDB;

    trackDB = track::db::TrackDB::instance(trackDBFilename);
    EXPECT_TRUE(trackDB != 0);

    JaroMediaDB jaroDB;
    EXPECT_TRUE(jaroDB.open(jaroDBFilename));

    QStringList trackList;
    EXPECT_TRUE(jaroDB.getAllTracks(trackList));
    EXPECT_TRUE(jaroDB.buildJaroTable(trackList));

    jaroDB.close();
    delete trackDB;
}

//-------------------------------------------------------------------------------------------
