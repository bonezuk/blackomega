#include "network/inc/Resource.h"
#include "track/db/inc/ITunesLocation.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "common/inc/DiskOps.h"
#include "common/inc/DiskIF.h"
#include "common/test/DiskMockIF.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;
using namespace omega;
using namespace omega::track::db;

//-------------------------------------------------------------------------------------------

class ITunesLocationTest : public ITunesLocation
{
    public:
        bool testIsValidPList(const QString& fileName) const;
        bool testIsLineXMLHeader(const QString& line) const;
        bool testIsLinePListHeader(const QString& line) const;

        QSet<int> testGetLocationIDSet();
        int testAddDirectoryLocation(const QString& pos);
        int testAddFileLocation(const QString& pos);
        bool testUpdateFileLocation(int ID,const QString& pos);
        QString testGetDirectoryLocation(int ID);
        QString testGetFileLocation(int ID);
        void testClearLocation(int ID);
        void testClearLocations();
        int testSizeLocations();

        bool testIsITunesXMLName(const QString& fileName) const;

        QString testFindITunesDBInDirectory(const QString& dirName);

        void testGetFileAndDirectoryLocationsFromSettings(QSet<QString>& confDirSet,QSet<QString>& confFileSet);
        QStringList testValidateDirectorySet(const QSet<QString>& dirSet);
        QStringList testValidateFileSet(const QSet<QString>& fileSet);
        QStringList testGetDefaultITuneDBs();
};

//-------------------------------------------------------------------------------------------

bool ITunesLocationTest::testIsValidPList(const QString& fileName) const
{
    return isValidPList(fileName);
}

//-------------------------------------------------------------------------------------------

bool ITunesLocationTest::testIsLineXMLHeader(const QString& line) const
{
    return isLineXMLHeader(line);
}

//-------------------------------------------------------------------------------------------

bool ITunesLocationTest::testIsLinePListHeader(const QString& line) const
{
    return isLinePListHeader(line);
}

//-------------------------------------------------------------------------------------------

QSet<int> ITunesLocationTest::testGetLocationIDSet()
{
    return getLocationIDSet();
}

//-------------------------------------------------------------------------------------------

int ITunesLocationTest::testAddDirectoryLocation(const QString& pos)
{
    return addDirectoryLocation(pos);
}

//-------------------------------------------------------------------------------------------

int ITunesLocationTest::testAddFileLocation(const QString& pos)
{
    return addFileLocation(pos);
}

//-------------------------------------------------------------------------------------------

bool ITunesLocationTest::testUpdateFileLocation(int ID,const QString& pos)
{
    return updateFileLocation(ID,pos);
}

//-------------------------------------------------------------------------------------------

QString ITunesLocationTest::testGetDirectoryLocation(int ID)
{
    return getDirectoryLocation(ID);
}

//-------------------------------------------------------------------------------------------

QString ITunesLocationTest::testGetFileLocation(int ID)
{
    return getFileLocation(ID);
}

//-------------------------------------------------------------------------------------------

void ITunesLocationTest::testClearLocation(int ID)
{
    clearLocation(ID);
}

//-------------------------------------------------------------------------------------------

void ITunesLocationTest::testClearLocations()
{
    clearLocations();
}

//-------------------------------------------------------------------------------------------

int ITunesLocationTest::testSizeLocations()
{
    return sizeLocations();
}

//-------------------------------------------------------------------------------------------

bool ITunesLocationTest::testIsITunesXMLName(const QString& fileName) const
{
    return isITunesXMLName(fileName);
}

//-------------------------------------------------------------------------------------------

QString ITunesLocationTest::testFindITunesDBInDirectory(const QString& dirName)
{
    return findITunesDBInDirectory(dirName);
}

//-------------------------------------------------------------------------------------------

void ITunesLocationTest::testGetFileAndDirectoryLocationsFromSettings(QSet<QString>& confDirSet,QSet<QString>& confFileSet)
{
    getFileAndDirectoryLocationsFromSettings(confDirSet,confFileSet);
}

//-------------------------------------------------------------------------------------------

QStringList ITunesLocationTest::testValidateDirectorySet(const QSet<QString>& dirSet)
{
    return validateDirectorySet(dirSet);
}

//-------------------------------------------------------------------------------------------

QStringList ITunesLocationTest::testValidateFileSet(const QSet<QString>& fileSet)
{
    return validateFileSet(fileSet);
}

//-------------------------------------------------------------------------------------------

QStringList ITunesLocationTest::testGetDefaultITuneDBs()
{
    return getDefaultITuneDBs();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLineXMLHeaderGivenEmptyLine)
{
    QString line;
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLineXMLHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLineXMLHeaderGivenRandomText)
{
    QString line = "RANDOM text";
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLineXMLHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLineXMLHeaderGivenXMLWithoutEnclosingBrackets)
{
    QString line = "?xml version=\"1.0\" encoding=\"UTF-8\"?";
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLineXMLHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLineXMLHeaderGivenXMLWithStartButNotEndBracket)
{
    QString line = "<?xml version=\"1.0\" encoding=\"UTF-8\"";
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLineXMLHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLineXMLHeaderGivenXMLWithEndButNotStartBracket)
{
    QString line = "xml version=\"1.0\" encoding=\"UTF-8\"?>";
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLineXMLHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLineXMLHeaderGivenXMLLowerCase)
{
    QString line = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    ITunesLocationTest location;
    EXPECT_TRUE(location.testIsLineXMLHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLineXMLHeaderGivenXMLUpperCase)
{
    QString line = "<? XML ?>";
    ITunesLocationTest location;
    EXPECT_TRUE(location.testIsLineXMLHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLinePListHeaderGivenEmptyLine)
{
    QString line = "";
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLinePListHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLinePListHeaderGivenRandomText)
{
    QString line = "RANDOM text";
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLinePListHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLinePListHeaderGivenXMLWithoutEnclosingBrackets)
{
    QString line = "plist version=\"1.0\"";
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLinePListHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLinePListHeaderGivenXMLWithStartButNotEndBracket)
{
    QString line = "<plist version=\"1.0\"";
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLinePListHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLinePListHeaderGivenXMLWithEndButNotStartBracket)
{
    QString line = "plist version=\"1.0\">";
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLinePListHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLinePListHeaderGivenEndPListTag)
{
    QString line = "</plist>";
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsLinePListHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLinePListHeaderGivenXMLLowerCase)
{
    QString line = "<plist version=\"1.0\">";
    ITunesLocationTest location;
    EXPECT_TRUE(location.testIsLinePListHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isLinePListHeaderGivenXMLUpperCase)
{
    QString line = "< PLIST >";
    ITunesLocationTest location;
    EXPECT_TRUE(location.testIsLinePListHeader(line));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isValidPListGivenCSVFile)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(),"albumModelUtilities1.csv");
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsValidPList(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isValidPListGivenCSVFileThatIsShort)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(),"albumModel12.csv");
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsValidPList(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isValidPListGivenNonPListXMLFile)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(),"iTunesNon.xml");
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsValidPList(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isValidPListGivenPListFile)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(),"iTunes.xml");
    ITunesLocationTest location;
    EXPECT_TRUE(location.testIsValidPList(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getAndSetMultipleITunesFileAndDirectory)
{
    {
        ITunesLocationTest locationA;
        locationA.testClearLocations();
        EXPECT_TRUE(locationA.testSizeLocations()==0);
        EXPECT_TRUE(locationA.testGetLocationIDSet().isEmpty());
    }

    {
        ITunesLocationTest locationB;

        EXPECT_TRUE(locationB.testAddDirectoryLocation("C:\\Music\\iTunes Media")==1);
        EXPECT_TRUE(locationB.testAddDirectoryLocation("/Users/Bonez/Music/iTunes Collection/Media/Music")==2);

        EXPECT_TRUE(locationB.testGetDirectoryLocation(1)=="C:\\Music\\iTunes Media");
        EXPECT_TRUE(locationB.testGetDirectoryLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music");

        EXPECT_TRUE(locationB.testGetFileLocation(1).isEmpty());
        EXPECT_TRUE(locationB.testGetFileLocation(2).isEmpty());

        EXPECT_TRUE(locationB.testSizeLocations()==2);

        EXPECT_TRUE(locationB.testGetLocationIDSet().size()==2);
        EXPECT_TRUE(locationB.testGetLocationIDSet().find(1)!=locationB.testGetLocationIDSet().end());
        EXPECT_TRUE(locationB.testGetLocationIDSet().find(2)!=locationB.testGetLocationIDSet().end());
    }

    {
        ITunesLocationTest locationC;

        EXPECT_TRUE(locationC.testGetDirectoryLocation(1)=="C:\\Music\\iTunes Media");
        EXPECT_TRUE(locationC.testGetDirectoryLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music");

        EXPECT_TRUE(locationC.testGetFileLocation(1).isEmpty());
        EXPECT_TRUE(locationC.testGetFileLocation(2).isEmpty());

        EXPECT_TRUE(locationC.testSizeLocations()==2);

        EXPECT_TRUE(locationC.testGetLocationIDSet().size()==2);
        EXPECT_TRUE(locationC.testGetLocationIDSet().find(1)!=locationC.testGetLocationIDSet().end());
        EXPECT_TRUE(locationC.testGetLocationIDSet().find(2)!=locationC.testGetLocationIDSet().end());
    }

    {
        ITunesLocationTest locationD;
        locationD.testClearLocations();
        EXPECT_TRUE(locationD.testSizeLocations()==0);
        EXPECT_TRUE(locationD.testGetLocationIDSet().isEmpty());
    }

    {
        ITunesLocationTest locationE;

        EXPECT_TRUE(locationE.testAddFileLocation("C:\\Music\\iTunes Media\\iTunes Music Library.xml")==1);
        EXPECT_TRUE(locationE.testAddFileLocation("/Users/Bonez/Music/iTunes Collection/Media/Music/iTunes Music Library.xml")==2);

        EXPECT_TRUE(locationE.testGetDirectoryLocation(1)=="C:\\Music\\iTunes Media");
        EXPECT_TRUE(locationE.testGetDirectoryLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music");

        EXPECT_TRUE(locationE.testGetFileLocation(1)=="C:\\Music\\iTunes Media\\iTunes Music Library.xml");
        EXPECT_TRUE(locationE.testGetFileLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music/iTunes Music Library.xml");

        EXPECT_TRUE(locationE.testSizeLocations()==2);
    }

    {
        ITunesLocationTest locationF;

        EXPECT_TRUE(locationF.testGetDirectoryLocation(1)=="C:\\Music\\iTunes Media");
        EXPECT_TRUE(locationF.testGetDirectoryLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music");

        EXPECT_TRUE(locationF.testGetFileLocation(1)=="C:\\Music\\iTunes Media\\iTunes Music Library.xml");
        EXPECT_TRUE(locationF.testGetFileLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music/iTunes Music Library.xml");

        EXPECT_TRUE(locationF.testSizeLocations()==2);

        EXPECT_TRUE(locationF.testGetLocationIDSet().size()==2);
        EXPECT_TRUE(locationF.testGetLocationIDSet().find(1)!=locationF.testGetLocationIDSet().end());
        EXPECT_TRUE(locationF.testGetLocationIDSet().find(2)!=locationF.testGetLocationIDSet().end());
    }

    {
        ITunesLocationTest locationG;
        locationG.testClearLocation(1);

        EXPECT_TRUE(locationG.testGetDirectoryLocation(1).isEmpty());
        EXPECT_TRUE(locationG.testGetDirectoryLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music");

        EXPECT_TRUE(locationG.testGetFileLocation(1).isEmpty());
        EXPECT_TRUE(locationG.testGetFileLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music/iTunes Music Library.xml");

        EXPECT_TRUE(locationG.testSizeLocations()==1);

        QSet<int> idSet = locationG.testGetLocationIDSet();
        EXPECT_TRUE(idSet.size()==1);
        EXPECT_TRUE(idSet.find(1)==idSet.end());
        EXPECT_TRUE(idSet.find(2)!=idSet.end());
    }

    {
        ITunesLocationTest locationH;
        locationH.testClearLocation(2);
        EXPECT_TRUE(locationH.testSizeLocations()==0);
        EXPECT_TRUE(locationH.testGetLocationIDSet().isEmpty());
    }

    {
        ITunesLocationTest locationI;

        EXPECT_TRUE(locationI.testAddDirectoryLocation("C:\\Music\\iTunes Media")==1);
        EXPECT_TRUE(locationI.testAddDirectoryLocation("/Users/Bonez/Music/iTunes Collection/Media/Music")==2);

        EXPECT_TRUE(locationI.testGetDirectoryLocation(1)=="C:\\Music\\iTunes Media");
        EXPECT_TRUE(locationI.testGetDirectoryLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music");
        EXPECT_TRUE(locationI.testGetFileLocation(1).isEmpty());
        EXPECT_TRUE(locationI.testGetFileLocation(2).isEmpty());
        EXPECT_TRUE(locationI.testSizeLocations()==2);

        EXPECT_TRUE(locationI.testUpdateFileLocation(1,"C:\\Music\\iTunes Media\\iTunes Music Library.xml"));
        EXPECT_TRUE(locationI.testUpdateFileLocation(2,"/Users/Bonez/Music/iTunes Collection/Media/Music/iTunes 1/iTunes Music Library.xml"));

        EXPECT_TRUE(locationI.testGetDirectoryLocation(1)=="C:\\Music\\iTunes Media");
        EXPECT_TRUE(locationI.testGetFileLocation(1)=="C:\\Music\\iTunes Media\\iTunes Music Library.xml");

        EXPECT_TRUE(locationI.testGetDirectoryLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music/iTunes 1");
        EXPECT_TRUE(locationI.testGetFileLocation(2)=="/Users/Bonez/Music/iTunes Collection/Media/Music/iTunes 1/iTunes Music Library.xml");

        EXPECT_TRUE(locationI.testSizeLocations()==2);

        locationI.testClearLocations();

        EXPECT_TRUE(locationI.testSizeLocations()==0);
    }
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isITunesXMLNameNoName)
{
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsITunesXMLName(""));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isITunesXMLNameNoXML)
{
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsITunesXMLName("/Users/bonez/Music/iTunes Library."));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isITunesXMLLibraryNameWithoutXML)
{
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsITunesXMLName("/Users/bonez/Music/iTunes Library.XM"));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isITunesXMLLibraryNameWithXML)
{
    ITunesLocationTest location;
    EXPECT_TRUE(location.testIsITunesXMLName("/Users/bonez/Music/iTunes Library.XML"));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isITunesXMLMusicLibraryNameWithoutXML)
{
    ITunesLocationTest location;
    EXPECT_FALSE(location.testIsITunesXMLName("/Users/bonez/Music/iTunes Music Library"));
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,isITunesXMLMusicLibraryNameWithXML)
{
    ITunesLocationTest location;
    EXPECT_TRUE(location.testIsITunesXMLName("/Users/bonez/Music/iTunes Music Library.xml"));
}

//-------------------------------------------------------------------------------------------

class ITunesLocationFindITunesDBInDirectoryTest : public ITunesLocationTest
{
    public:
        MOCK_METHOD4(findITunesDBInDirectoryRecursive,QString(const QString& dirName,CancelScanFunction cancelFunc,void *pUserData,int& progressCount));
        MOCK_METHOD1(getModifiedFileTime,common::TimeStamp(const QString& fileName));
        MOCK_CONST_METHOD1(isValidPList,bool(const QString& fileName));
};

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenNoDiskIF)
{
    QString rootName  = "/Users/bonez/Music";
    ITunesLocationFindITunesDBInDirectoryTest location;
    QString testDBName = location.testFindITunesDBInDirectory(rootName);
    EXPECT_TRUE(testDBName.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenNotADirectory)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString rootName  = "/Users/bonez/Music";
    EXPECT_CALL(pAPI,isDirectory(Eq(rootName))).Times(1).WillOnce(Return(false));

    ITunesLocationFindITunesDBInDirectoryTest location;
    QString testDBName = location.testFindITunesDBInDirectory(rootName);
    EXPECT_TRUE(testDBName.isEmpty());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenUnabletoOpenDirectoryForReading)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString rootName  = "/Users/bonez/Music";

    common::DiskIF::DirHandle h = common::DiskIF::invalidDirectory();

    EXPECT_CALL(pAPI,isDirectory(Eq(rootName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,openDirectory(Eq(rootName))).Times(1).WillOnce(Return(h));

    ITunesLocationFindITunesDBInDirectoryTest location;

    QString testDBName = location.testFindITunesDBInDirectory(rootName);

    EXPECT_TRUE(testDBName.isEmpty());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenEmptyDirectory)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString emptyName;
    QString rootName  = "/Users/bonez/Music";

    common::DiskIF::DirHandle h = (common::DiskIF::DirHandle)(1234);

    EXPECT_CALL(pAPI,isDirectory(Eq(rootName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,openDirectory(Eq(rootName))).Times(1).WillOnce(Return(h));
    EXPECT_CALL(pAPI,nextDirectoryEntry(h)).Times(1).WillOnce(Return(emptyName));
    EXPECT_CALL(pAPI,closeDirectory(h)).Times(1);

    ITunesLocationFindITunesDBInDirectoryTest location;

    QString testDBName = location.testFindITunesDBInDirectory(rootName);

    EXPECT_TRUE(testDBName.isEmpty());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenFilesButNoITunesLibrary)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QStringList parts;
    parts << "iTunes Music Library.xml" << "music.mp3" << "iTunes Library.xml";

    QString emptyName;
    QString rootName  = "/Users/bonez/Music";
    QString fileNameA = common::DiskOps::mergeName(rootName,parts.at(0));
    QString fileNameB = common::DiskOps::mergeName(rootName,parts.at(1));
    QString fileNameC = common::DiskOps::mergeName(rootName,parts.at(2));

    common::DiskIF::DirHandle h = (common::DiskIF::DirHandle)(1234);

    EXPECT_CALL(pAPI,isDirectory(Eq(rootName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,openDirectory(Eq(rootName))).Times(1).WillOnce(Return(h));
    EXPECT_CALL(pAPI,nextDirectoryEntry(h)).Times(4)
        .WillOnce(Return(parts.at(0)))
        .WillOnce(Return(parts.at(1)))
        .WillOnce(Return(parts.at(2)))
        .WillOnce(Return(emptyName));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameB))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameC))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,closeDirectory(h)).Times(1);

    ITunesLocationFindITunesDBInDirectoryTest location;
    EXPECT_CALL(location,isValidPList(Eq(fileNameA))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(location,isValidPList(Eq(fileNameC))).Times(1).WillOnce(Return(false));

    QString testDBName = location.testFindITunesDBInDirectory(rootName);

    EXPECT_TRUE(testDBName.isEmpty());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenFilesWithOneITunesLibrary)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QStringList parts;
    parts << "iTunes Music Library.xml" << "music.mp3" << "iTunes Library.xml";

    QString emptyName;
    QString rootName  = "/Users/bonez/Music";
    QString fileNameA = common::DiskOps::mergeName(rootName,parts.at(0));
    QString fileNameB = common::DiskOps::mergeName(rootName,parts.at(1));
    QString fileNameC = common::DiskOps::mergeName(rootName,parts.at(2));

    common::DiskIF::DirHandle h = (common::DiskIF::DirHandle)(1234);

    EXPECT_CALL(pAPI,isDirectory(Eq(rootName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,openDirectory(Eq(rootName))).Times(1).WillOnce(Return(h));
    EXPECT_CALL(pAPI,nextDirectoryEntry(h)).Times(4)
        .WillOnce(Return(parts.at(0)))
        .WillOnce(Return(parts.at(1)))
        .WillOnce(Return(parts.at(2)))
        .WillOnce(Return(emptyName));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameB))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameC))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,closeDirectory(h)).Times(1);

    common::TimeStamp fTimeA(3.0);

    ITunesLocationFindITunesDBInDirectoryTest location;
    EXPECT_CALL(location,isValidPList(Eq(fileNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(location,isValidPList(Eq(fileNameC))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(location,getModifiedFileTime(Eq(fileNameA))).Times(1).WillOnce(Return(fTimeA));

    QString testDBName = location.testFindITunesDBInDirectory(rootName);

    EXPECT_TRUE(testDBName==fileNameA);

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenFilesWithMultipleITunesLibrary)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QStringList parts;
    parts << "iTunes Music Library.xml" << "music.mp3" << "iTunes Library.xml";

    QString emptyName;
    QString rootName  = "/Users/bonez/Music";
    QString fileNameA = common::DiskOps::mergeName(rootName,parts.at(0));
    QString fileNameB = common::DiskOps::mergeName(rootName,parts.at(1));
    QString fileNameC = common::DiskOps::mergeName(rootName,parts.at(2));

    common::DiskIF::DirHandle h = (common::DiskIF::DirHandle)(1234);

    EXPECT_CALL(pAPI,isDirectory(Eq(rootName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,openDirectory(Eq(rootName))).Times(1).WillOnce(Return(h));
    EXPECT_CALL(pAPI,nextDirectoryEntry(h)).Times(4)
        .WillOnce(Return(parts.at(0)))
        .WillOnce(Return(parts.at(1)))
        .WillOnce(Return(parts.at(2)))
        .WillOnce(Return(emptyName));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameB))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameC))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,closeDirectory(h)).Times(1);

    common::TimeStamp fTimeA(3.0);
    common::TimeStamp fTimeC(5.0);

    ITunesLocationFindITunesDBInDirectoryTest location;
    EXPECT_CALL(location,isValidPList(Eq(fileNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(location,isValidPList(Eq(fileNameC))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(location,getModifiedFileTime(Eq(fileNameA))).Times(1).WillOnce(Return(fTimeA));
    EXPECT_CALL(location,getModifiedFileTime(Eq(fileNameC))).Times(1).WillOnce(Return(fTimeC));

    QString testDBName = location.testFindITunesDBInDirectory(rootName);

    EXPECT_TRUE(testDBName==fileNameC);

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenDirectoriesWithNoITunesLibrary)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QStringList parts;
    parts << "iTunes" << "Media" << "Groups";

    QString emptyName;
    QString rootName  = "/Users/bonez/Music";
    QString dirNameA  = common::DiskOps::mergeName(rootName,parts.at(0));
    QString dirNameB  = common::DiskOps::mergeName(rootName,parts.at(1));
    QString dirNameC  = common::DiskOps::mergeName(rootName,parts.at(2));

    common::DiskIF::DirHandle h = (common::DiskIF::DirHandle)(1234);

    EXPECT_CALL(pAPI,isDirectory(Eq(rootName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,openDirectory(Eq(rootName))).Times(1).WillOnce(Return(h));
    EXPECT_CALL(pAPI,nextDirectoryEntry(h)).Times(4)
        .WillOnce(Return(parts.at(0)))
        .WillOnce(Return(parts.at(1)))
        .WillOnce(Return(parts.at(2)))
        .WillOnce(Return(emptyName));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameA))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameB))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameB))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameC))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameC))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,closeDirectory(h)).Times(1);

    ITunesLocationFindITunesDBInDirectoryTest location;
    EXPECT_CALL(location,findITunesDBInDirectoryRecursive(Eq(dirNameA),A<track::db::CancelScanFunction>(),A<void *>(),A<int&>())).Times(1).WillOnce(Return(emptyName));
    EXPECT_CALL(location,findITunesDBInDirectoryRecursive(Eq(dirNameB),A<track::db::CancelScanFunction>(),A<void *>(),A<int&>())).Times(1).WillOnce(Return(emptyName));
    EXPECT_CALL(location,findITunesDBInDirectoryRecursive(Eq(dirNameC),A<track::db::CancelScanFunction>(),A<void *>(),A<int&>())).Times(1).WillOnce(Return(emptyName));

    QString testDBName = location.testFindITunesDBInDirectory(rootName);

    EXPECT_TRUE(testDBName.isEmpty());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenDirectoriesWithITunesLibrary)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QStringList parts;
    parts << "iTunes" << "Media" << "Groups";

    QString emptyName;
    QString rootName  = "/Users/bonez/Music";
    QString dirNameA  = common::DiskOps::mergeName(rootName,parts.at(0));
    QString dirNameB  = common::DiskOps::mergeName(rootName,parts.at(1));
    QString dirNameC  = common::DiskOps::mergeName(rootName,parts.at(2));

    common::DiskIF::DirHandle h = (common::DiskIF::DirHandle)(1234);

    EXPECT_CALL(pAPI,isDirectory(Eq(rootName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,openDirectory(Eq(rootName))).Times(1).WillOnce(Return(h));
    EXPECT_CALL(pAPI,nextDirectoryEntry(h)).Times(4)
        .WillOnce(Return(parts.at(0)))
        .WillOnce(Return(parts.at(1)))
        .WillOnce(Return(parts.at(2)))
        .WillOnce(Return(emptyName));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameA))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameB))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameB))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameC))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameC))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,closeDirectory(h)).Times(1);

    QString expectDBFileName = common::DiskOps::mergeName(dirNameA,"iTunes Library.xml");

    ITunesLocationFindITunesDBInDirectoryTest location;
    EXPECT_CALL(location,findITunesDBInDirectoryRecursive(Eq(dirNameA),A<track::db::CancelScanFunction>(),A<void *>(),A<int&>())).Times(1).WillOnce(Return(expectDBFileName));

    QString testDBName = location.testFindITunesDBInDirectory(rootName);

    EXPECT_TRUE(testDBName==expectDBFileName);

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenDirectoriesAndFilesWithITunesLibraryInDirectories)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QStringList parts;
    parts << "iTunes" << "Media" << "Groups" << "file.txt" << "music.mp3";

    QString emptyName;
    QString rootName  = "/Users/bonez/Music";
    QString dirNameA  = common::DiskOps::mergeName(rootName,parts.at(0));
    QString dirNameB  = common::DiskOps::mergeName(rootName,parts.at(1));
    QString dirNameC  = common::DiskOps::mergeName(rootName,parts.at(2));
    QString fileNameA = common::DiskOps::mergeName(rootName,parts.at(3));
    QString fileNameB = common::DiskOps::mergeName(rootName,parts.at(4));

    common::DiskIF::DirHandle h = (common::DiskIF::DirHandle)(1234);

    EXPECT_CALL(pAPI,isDirectory(Eq(rootName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,openDirectory(Eq(rootName))).Times(1).WillOnce(Return(h));
    EXPECT_CALL(pAPI,nextDirectoryEntry(h)).Times(6)
        .WillOnce(Return(parts.at(0)))
        .WillOnce(Return(parts.at(1)))
        .WillOnce(Return(parts.at(2)))
        .WillOnce(Return(parts.at(3)))
        .WillOnce(Return(parts.at(4)))
        .WillOnce(Return(emptyName));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameA))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameB))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameB))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameC))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameC))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameB))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,closeDirectory(h)).Times(1);

    QString expectDBFileName = common::DiskOps::mergeName(dirNameA,"iTunes Library.xml");

    ITunesLocationFindITunesDBInDirectoryTest location;
    EXPECT_CALL(location,findITunesDBInDirectoryRecursive(Eq(dirNameA),A<track::db::CancelScanFunction>(),A<void *>(),A<int&>())).Times(1).WillOnce(Return(emptyName));
    EXPECT_CALL(location,findITunesDBInDirectoryRecursive(Eq(dirNameB),A<track::db::CancelScanFunction>(),A<void *>(),A<int&>())).Times(1).WillOnce(Return(emptyName));
    EXPECT_CALL(location,findITunesDBInDirectoryRecursive(Eq(dirNameC),A<track::db::CancelScanFunction>(),A<void *>(),A<int&>())).Times(1).WillOnce(Return(expectDBFileName));

    QString testDBName = location.testFindITunesDBInDirectory(rootName);

    EXPECT_TRUE(testDBName==expectDBFileName);

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,findITunesDBInDirectoryGivenDirectoriesAndFilesWithITunesLibraryInRootDirectory)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QStringList parts;
    parts << "iTunes" << "Media" << "Groups" << "file.txt" << "music.mp3" << "iTunes Library.xml";

    QString emptyName;
    QString rootName  = "/Users/bonez/Music";
    QString dirNameA  = common::DiskOps::mergeName(rootName,parts.at(0));
    QString dirNameB  = common::DiskOps::mergeName(rootName,parts.at(1));
    QString dirNameC  = common::DiskOps::mergeName(rootName,parts.at(2));
    QString fileNameA = common::DiskOps::mergeName(rootName,parts.at(3));
    QString fileNameB = common::DiskOps::mergeName(rootName,parts.at(4));
    QString fileNameC = common::DiskOps::mergeName(rootName,parts.at(5));

    common::DiskIF::DirHandle h = (common::DiskIF::DirHandle)(1234);

    EXPECT_CALL(pAPI,isDirectory(Eq(rootName))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,openDirectory(Eq(rootName))).Times(1).WillOnce(Return(h));
    EXPECT_CALL(pAPI,nextDirectoryEntry(h)).Times(7)
        .WillOnce(Return(parts.at(0)))
        .WillOnce(Return(parts.at(1)))
        .WillOnce(Return(parts.at(2)))
        .WillOnce(Return(parts.at(3)))
        .WillOnce(Return(parts.at(4)))
        .WillOnce(Return(parts.at(5)))
        .WillOnce(Return(emptyName));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameA))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameB))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameB))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(dirNameC))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameC))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameB))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isFile(Eq(fileNameC))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,closeDirectory(h)).Times(1);

    common::TimeStamp fTimeA(3.0);

    ITunesLocationFindITunesDBInDirectoryTest location;
    EXPECT_CALL(location,isValidPList(Eq(fileNameC))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(location,getModifiedFileTime(Eq(fileNameC))).Times(1).WillOnce(Return(fTimeA));

    QString testDBName = location.testFindITunesDBInDirectory(rootName);

    EXPECT_TRUE(testDBName==fileNameC);

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getFileAndDirectoryLocationsFromSettingsWithNoDirectoriesOrFiles)
{
    ITunesLocationTest location;
    location.testClearLocations();

    QSet<QString> confDirSet,confFileSet;
    location.testGetFileAndDirectoryLocationsFromSettings(confDirSet,confFileSet);

    EXPECT_TRUE(confDirSet.size()==0);
    EXPECT_TRUE(confFileSet.size()==0);

    location.testClearLocations();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getFileAndDirectoryLocationsFromSettingsGivenOneDirectory)
{
    QString dirNameA = "/Users/bonez/Music/iTunes";

    ITunesLocationTest location;
    location.testClearLocations();
    location.testAddDirectoryLocation(dirNameA);

    QSet<QString> confDirSet,confFileSet;
    location.testGetFileAndDirectoryLocationsFromSettings(confDirSet,confFileSet);

    EXPECT_TRUE(confDirSet.size()==1);
    EXPECT_TRUE(confDirSet.find(dirNameA)!=confDirSet.end());

    EXPECT_TRUE(confFileSet.size()==0);

    location.testClearLocations();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getFileAndDirectoryLocationsFromSettingsGivenThreeDirectories)
{
    QString dirNameA = "/Users/bonez/Music/iTunes";
    QString dirNameB = "/Users/git/Music/iTunes";
    QString dirNameC = "/Volumes/LaCie/Music/iTunes";

    ITunesLocationTest location;

    location.testClearLocations();
    location.testAddDirectoryLocation(dirNameA);
    location.testAddDirectoryLocation(dirNameB);
    location.testAddDirectoryLocation(dirNameC);

    QSet<QString> confDirSet,confFileSet;
    location.testGetFileAndDirectoryLocationsFromSettings(confDirSet,confFileSet);

    EXPECT_TRUE(confDirSet.size()==3);
    EXPECT_TRUE(confDirSet.find(dirNameA)!=confDirSet.end());
    EXPECT_TRUE(confDirSet.find(dirNameB)!=confDirSet.end());
    EXPECT_TRUE(confDirSet.find(dirNameC)!=confDirSet.end());

    EXPECT_TRUE(confFileSet.size()==0);

    location.testClearLocations();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getFileAndDirectoryLocationsFromSettingsGivenOneFile)
{
    QString fileNameA = "/Volumes/Music/iTunes/iTunes Media/iTunes Music Library.xml";

    ITunesLocationTest location;

    location.testClearLocations();
    location.testAddFileLocation(fileNameA);

    QSet<QString> confDirSet,confFileSet;
    location.testGetFileAndDirectoryLocationsFromSettings(confDirSet,confFileSet);

    EXPECT_TRUE(confDirSet.size()==0);

    EXPECT_TRUE(confFileSet.size()==1);
    EXPECT_TRUE(confFileSet.find(fileNameA)!=confFileSet.end());

    location.testClearLocations();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getFileAndDirectoryLocationsFromSettingsGivenThreeFiles)
{
    QString fileNameA = "/Volumes/Music/iTunes/iTunes Media/iTunes Music Library.xml";
    QString fileNameB = "/Volumes/ITunes/iTunes/iTunes Media/iTunes Music Library.xml";
    QString fileNameC = "/Volumes/Data/iTunes Library.xml";

    ITunesLocationTest location;

    location.testClearLocations();
    location.testAddFileLocation(fileNameA);
    location.testAddFileLocation(fileNameB);
    location.testAddFileLocation(fileNameC);

    QSet<QString> confDirSet,confFileSet;
    location.testGetFileAndDirectoryLocationsFromSettings(confDirSet,confFileSet);

    EXPECT_TRUE(confDirSet.size()==0);

    EXPECT_TRUE(confFileSet.size()==3);
    EXPECT_TRUE(confFileSet.find(fileNameA)!=confFileSet.end());
    EXPECT_TRUE(confFileSet.find(fileNameB)!=confFileSet.end());
    EXPECT_TRUE(confFileSet.find(fileNameC)!=confFileSet.end());

    location.testClearLocations();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getFileAndDirectoryLocationsFromSettingsGivenThreeDirectoriesAndThreeFiles)
{
    QString dirNameA = "/Users/bonez/Music/iTunes";
    QString dirNameB = "/Users/git/Music/iTunes";
    QString dirNameC = "/Volumes/LaCie/Music/iTunes";
    QString fileNameA = "/Volumes/Music/iTunes/iTunes Media/iTunes Music Library.xml";
    QString fileNameB = "/Volumes/ITunes/iTunes/iTunes Media/iTunes Music Library.xml";
    QString fileNameC = "/Volumes/Data/iTunes Library.xml";

    ITunesLocationTest location;

    location.testClearLocations();
    location.testAddDirectoryLocation(dirNameA);
    location.testAddDirectoryLocation(dirNameB);
    location.testAddDirectoryLocation(dirNameC);
    location.testAddFileLocation(fileNameA);
    location.testAddFileLocation(fileNameB);
    location.testAddFileLocation(fileNameC);

    QSet<QString> confDirSet,confFileSet;
    location.testGetFileAndDirectoryLocationsFromSettings(confDirSet,confFileSet);

    EXPECT_TRUE(confDirSet.size()==3);
    EXPECT_TRUE(confDirSet.find(dirNameA)!=confDirSet.end());
    EXPECT_TRUE(confDirSet.find(dirNameB)!=confDirSet.end());
    EXPECT_TRUE(confDirSet.find(dirNameC)!=confDirSet.end());

    EXPECT_TRUE(confFileSet.size()==3);
    EXPECT_TRUE(confFileSet.find(fileNameA)!=confFileSet.end());
    EXPECT_TRUE(confFileSet.find(fileNameB)!=confFileSet.end());
    EXPECT_TRUE(confFileSet.find(fileNameC)!=confFileSet.end());

    location.testClearLocations();
}

//-------------------------------------------------------------------------------------------

class ITunesLocationValidateDirectorySetTest : public ITunesLocationTest
{
    public:
        MOCK_METHOD1(findITunesDBInDirectory,QString(const QString& dirName));
};

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,validateDirectorySetGivenValidDBFileOnNoDBFile)
{
    QString dirNameA = "/Users/bonez/Music/iTunes";
    QString fileNameA;

    QSet<QString> dirSet;
    dirSet.insert(dirNameA);

    ITunesLocationValidateDirectorySetTest location;
    EXPECT_CALL(location,findITunesDBInDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(fileNameA));

    QStringList dbFileList = location.testValidateDirectorySet(dirSet);

    EXPECT_TRUE(dbFileList.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,validateDirectorySetGivenValidDBFileOnScanAndNoneUpdated)
{
    QString dirNameA = "/Users/bonez/Music/iTunes";
    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Media/iTunes Library.xml";

    QSet<QString> dirSet;
    dirSet.insert(dirNameA);

    ITunesLocationValidateDirectorySetTest location;
    EXPECT_CALL(location,findITunesDBInDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(fileNameA));

    location.testClearLocations();

    QStringList dbFileList = location.testValidateDirectorySet(dirSet);

    EXPECT_TRUE(dbFileList.size()==1);
    EXPECT_TRUE(dbFileList.at(0)==fileNameA);

    location.testClearLocations();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,validateDirectorySetGivenValidDBFileOnScanAndUpdate)
{
    QString dirNameA = "/Users/bonez/Music/iTunes";
    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Media/iTunes Library.xml";

    QSet<QString> dirSet;
    dirSet.insert(dirNameA);

    ITunesLocationValidateDirectorySetTest location;
    EXPECT_CALL(location,findITunesDBInDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(fileNameA));

    location.testClearLocations();
    int idA = location.testAddDirectoryLocation(dirNameA);

    QStringList dbFileList = location.testValidateDirectorySet(dirSet);

    EXPECT_TRUE(dbFileList.size()==1);
    EXPECT_TRUE(dbFileList.at(0)==fileNameA);
    EXPECT_TRUE(location.testGetFileLocation(idA)==fileNameA);

    location.testClearLocations();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,validateDirectorySetGivenValidDBFileOnMultipleScanAndUpdate)
{
    QString dirNameA = "/Users/bonez/Music/iTunes";
    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Media/iTunes Library.xml";
    QString dirNameB = "/Users/git/Music/iTunes";
    QString fileNameB = "/Users/git/Music/iTunes/iTunes Media/iTunes Library.xml";

    QSet<QString> dirSet;
    dirSet.insert(dirNameA);
    dirSet.insert(dirNameB);

    ITunesLocationValidateDirectorySetTest location;
    EXPECT_CALL(location,findITunesDBInDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(fileNameA));
    EXPECT_CALL(location,findITunesDBInDirectory(Eq(dirNameB))).Times(1).WillOnce(Return(fileNameB));

    location.testClearLocations();
    int idA = location.testAddDirectoryLocation(dirNameA);
    int idB = location.testAddDirectoryLocation(dirNameB);

    QStringList dbFileList = location.testValidateDirectorySet(dirSet);

    QSet<QString> dbFileSet = QSet<QString>(dbFileList.begin(), dbFileList.end());
    EXPECT_TRUE(dbFileSet.size()==2);
    EXPECT_TRUE(dbFileSet.find(fileNameA)!=dbFileSet.end());
    EXPECT_TRUE(dbFileSet.find(fileNameB)!=dbFileSet.end());

    EXPECT_TRUE(location.testGetFileLocation(idA)==fileNameA);
    EXPECT_TRUE(location.testGetFileLocation(idB)==fileNameB);

    location.testClearLocations();
}

//-------------------------------------------------------------------------------------------

class ITunesLocationValidateFileSetTest : public ITunesLocationTest
{
    public:
        MOCK_CONST_METHOD1(isValidPList,bool(const QString& fileName));
};

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,validateFileSetGivenNoFiles)
{
    QSet<QString> fileSet;
    ITunesLocationValidateFileSetTest location;
    QStringList dbFileList = location.testValidateFileSet(fileSet);
    EXPECT_TRUE(dbFileList.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,validateFileSetGivenOneValidFile)
{
    QString fileNameA = "/Volumes/Music/iTunes/iTunes Media/notes.mp3";
    QString fileNameB = "/Volumes/ITunes/iTunes/iTunes Media/README";
    QString fileNameC = "/Volumes/Data/iTunes Library.xml";

    QSet<QString> fileSet;
    fileSet.insert(fileNameA);
    fileSet.insert(fileNameB);
    fileSet.insert(fileNameC);

    ITunesLocationValidateFileSetTest location;
    EXPECT_CALL(location,isValidPList(Eq(fileNameC))).Times(1).WillOnce(Return(true));

    QStringList dbFileList = location.testValidateFileSet(fileSet);

    QSet<QString> dbFileSet = QSet<QString>(dbFileList.begin(), dbFileList.end());
    EXPECT_TRUE(dbFileSet.size()==1);
    EXPECT_TRUE(dbFileSet.find(fileNameC)!=dbFileSet.end());
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,validateFileSetGivenTwoValidFiles)
{
    QString fileNameA = "/Volumes/Music/iTunes/iTunes Media/iTunes Music Library.xml";
    QString fileNameB = "/Volumes/ITunes/iTunes/iTunes Media/iTunes Music Library.xml";
    QString fileNameC = "/Volumes/Data/iTunes Library.xml";

    QSet<QString> fileSet;
    fileSet.insert(fileNameA);
    fileSet.insert(fileNameB);
    fileSet.insert(fileNameC);

    ITunesLocationValidateFileSetTest location;
    EXPECT_CALL(location,isValidPList(Eq(fileNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(location,isValidPList(Eq(fileNameB))).Times(1).WillOnce(Return(false));
    EXPECT_CALL(location,isValidPList(Eq(fileNameC))).Times(1).WillOnce(Return(true));

    QStringList dbFileList = location.testValidateFileSet(fileSet);

    QSet<QString> dbFileSet = QSet<QString>(dbFileList.begin(), dbFileList.end());
    EXPECT_TRUE(dbFileSet.size()==2);
    EXPECT_TRUE(dbFileSet.find(fileNameA)!=dbFileSet.end());
    EXPECT_TRUE(dbFileSet.find(fileNameC)!=dbFileSet.end());
}

//-------------------------------------------------------------------------------------------

class ITunesLocationGetDefaultITuneDBsTest : public ITunesLocationTest
{
    public:
        MOCK_METHOD0(defaultITunesDirectory,QStringList());
        MOCK_METHOD1(findITunesDBInDirectory,QString(const QString& dirName));
        MOCK_CONST_METHOD1(isValidPList,bool(const QString& fileName));
};

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getDefaultITuneDBsGivenNoDefaults)
{
    QStringList defaultDirList;

    ITunesLocationGetDefaultITuneDBsTest location;
    EXPECT_CALL(location,defaultITunesDirectory()).Times(1).WillOnce(Return(defaultDirList));

    location.testClearLocations();

    QStringList dbList = location.testGetDefaultITuneDBs();

    EXPECT_TRUE(dbList.size()==0);
    EXPECT_TRUE(location.testSizeLocations()==0);

    location.testClearLocations();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getDefaultITuneDBsGivenDirectoryWithNoDBFile)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString dirNameA = "/Users/bonez/Music";
    QString fileNameA;

    QStringList defaultDirList;
    defaultDirList << dirNameA;

    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(true));

    ITunesLocationGetDefaultITuneDBsTest location;
    EXPECT_CALL(location,defaultITunesDirectory()).Times(1).WillOnce(Return(defaultDirList));
    EXPECT_CALL(location,findITunesDBInDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(fileNameA));

    location.testClearLocations();

    QStringList dbList = location.testGetDefaultITuneDBs();

    EXPECT_TRUE(dbList.size()==0);
    EXPECT_TRUE(location.testSizeLocations()==0);

    location.testClearLocations();

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getDefaultITuneDBsGivenDirectoryWithDBFile)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString dirNameA = "/Users/bonez/Music";
    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Library.xml";

    QStringList defaultDirList;
    defaultDirList << dirNameA;

    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(true));

    ITunesLocationGetDefaultITuneDBsTest location;
    EXPECT_CALL(location,defaultITunesDirectory()).Times(1).WillOnce(Return(defaultDirList));
    EXPECT_CALL(location,findITunesDBInDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(fileNameA));

    location.testClearLocations();

    QStringList dbList = location.testGetDefaultITuneDBs();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==1);
    EXPECT_TRUE(dbSet.find(fileNameA)!=dbSet.end());

    EXPECT_TRUE(location.testGetFileLocation(1)==fileNameA);

    location.testClearLocations();

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getDefaultITuneDBsGivenTwoDirectoriesWithDBFile)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString dirNameA = "/Users/bonez/Music";
    QString dirNameB = "/Users/git/Music";
    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Library.xml";
    QString fileNameB = "/Users/git/Music/iTunes/iTunes Library.xml";

    QStringList defaultDirList;
    defaultDirList << dirNameA << dirNameB;

    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,isDirectory(Eq(dirNameB))).Times(1).WillOnce(Return(true));

    ITunesLocationGetDefaultITuneDBsTest location;
    EXPECT_CALL(location,defaultITunesDirectory()).Times(1).WillOnce(Return(defaultDirList));
    EXPECT_CALL(location,findITunesDBInDirectory(Eq(dirNameA))).Times(1).WillOnce(Return(fileNameA));
    EXPECT_CALL(location,findITunesDBInDirectory(Eq(dirNameB))).Times(1).WillOnce(Return(fileNameB));

    location.testClearLocations();

    QStringList dbList = location.testGetDefaultITuneDBs();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==2);
    EXPECT_TRUE(dbSet.find(fileNameA)!=dbSet.end());
    EXPECT_TRUE(dbSet.find(fileNameB)!=dbSet.end());

    EXPECT_TRUE(location.testGetFileLocation(1)==fileNameA);
    EXPECT_TRUE(location.testGetFileLocation(2)==fileNameB);

    location.testClearLocations();

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getDefaultITuneDBsGivenNoneDBFile)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Library.xml";

    QStringList defaultDirList;
    defaultDirList << fileNameA;

    EXPECT_CALL(pAPI,isDirectory(Eq(fileNameA))).Times(1).WillOnce(Return(false));

    ITunesLocationGetDefaultITuneDBsTest location;
    EXPECT_CALL(location,defaultITunesDirectory()).Times(1).WillOnce(Return(defaultDirList));
    EXPECT_CALL(location,isValidPList(Eq(fileNameA))).Times(1).WillOnce(Return(false));

    location.testClearLocations();

    QStringList dbList = location.testGetDefaultITuneDBs();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==0);

    location.testClearLocations();

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getDefaultITuneDBsGivenDBFile)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& pAPI = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Library.xml";

    QStringList defaultDirList;
    defaultDirList << fileNameA;

    EXPECT_CALL(pAPI,isDirectory(Eq(fileNameA))).Times(1).WillOnce(Return(false));

    ITunesLocationGetDefaultITuneDBsTest location;
    EXPECT_CALL(location,defaultITunesDirectory()).Times(1).WillOnce(Return(defaultDirList));
    EXPECT_CALL(location,isValidPList(Eq(fileNameA))).Times(1).WillOnce(Return(true));

    location.testClearLocations();

    QStringList dbList = location.testGetDefaultITuneDBs();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==1);
    EXPECT_TRUE(dbSet.find(fileNameA)!=dbSet.end());
    EXPECT_TRUE(location.testGetFileLocation(1)==fileNameA);

    location.testClearLocations();

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

class ITunesLocationGetITunesConfigFileNamesTest : public ITunesLocationTest
{
    public:
        MOCK_METHOD2(getFileAndDirectoryLocationsFromSettings,void(QSet<QString>& confDirSet,QSet<QString>& confFileSet));
        MOCK_METHOD1(validateDirectorySet,QStringList(const QSet<QString>& dirSet));
        MOCK_METHOD1(validateFileSet,QStringList(const QSet<QString>& fileSet));
        MOCK_METHOD0(getDefaultITuneDBs,QStringList());
};

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getITunesConfigFileNamesGivenDefaultHasNoOutputDatabase)
{
    QSet<QString> dirSet,fileSet;
    QStringList dirList,fileList,defaultList;

    ITunesLocationGetITunesConfigFileNamesTest location;
    EXPECT_CALL(location,getFileAndDirectoryLocationsFromSettings(A<QSet<QString>& >(),A<QSet<QString>& >())).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(dirSet),SetArgReferee<1>(fileSet)));
    EXPECT_CALL(location,validateDirectorySet(Eq(dirSet))).Times(1).WillOnce(Return(dirList));
    EXPECT_CALL(location,validateFileSet(Eq(fileSet))).Times(1).WillOnce(Return(fileList));
    EXPECT_CALL(location,getDefaultITuneDBs()).Times(1).WillOnce(Return(defaultList));

    QStringList dbList = location.getITunesConfigFileNames();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getITunesConfigFileNamesGivenDefaultHasDatabase)
{
    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Library.xml";
    QString fileNameB = "/Users/git/Music/iTunes/iTunes Library.xml";

    QSet<QString> dirSet,fileSet;
    QStringList dirList,fileList,defaultList;

    defaultList << fileNameA << fileNameB;

    ITunesLocationGetITunesConfigFileNamesTest location;
    EXPECT_CALL(location,getFileAndDirectoryLocationsFromSettings(A<QSet<QString>& >(),A<QSet<QString>& >())).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(dirSet),SetArgReferee<1>(fileSet)));
    EXPECT_CALL(location,validateDirectorySet(Eq(dirSet))).Times(1).WillOnce(Return(dirList));
    EXPECT_CALL(location,validateFileSet(Eq(fileSet))).Times(1).WillOnce(Return(fileList));
    EXPECT_CALL(location,getDefaultITuneDBs()).Times(1).WillOnce(Return(defaultList));

    QStringList dbList = location.getITunesConfigFileNames();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==2);
    EXPECT_TRUE(dbSet.find(fileNameA)!=dbSet.end());
    EXPECT_TRUE(dbSet.find(fileNameB)!=dbSet.end());
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getITunesConfigFileNamesGivenFileSetHasADatabase)
{
    QString fileNameC = "/Volumes/LaCie/Music/iTunes/iTunes Media/iTunes Music Library.xml";

    QSet<QString> dirSet,fileSet;
    fileSet.insert(fileNameC);

    QStringList dirList,fileList;
    fileList << fileNameC;

    ITunesLocationGetITunesConfigFileNamesTest location;
    EXPECT_CALL(location,getFileAndDirectoryLocationsFromSettings(A<QSet<QString>& >(),A<QSet<QString>& >())).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(dirSet),SetArgReferee<1>(fileSet)));
    EXPECT_CALL(location,validateDirectorySet(Eq(dirSet))).Times(1).WillOnce(Return(dirList));
    EXPECT_CALL(location,validateFileSet(Eq(fileSet))).Times(1).WillOnce(Return(fileList));

    QStringList dbList = location.getITunesConfigFileNames();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==1);
    EXPECT_TRUE(dbSet.find(fileNameC)!=dbSet.end());
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getITunesConfigFileNamesGivenFileSetHasMultipleDatabases)
{
    QString fileNameC = "/Volumes/LaCie/Music/iTunes/iTunes Media/iTunes Music Library.xml";
    QString fileNameD = "/Volumes/Music/iTunes/iTunes Media/iTunes Music Library.xml";

    QSet<QString> dirSet,fileSet;
    fileSet.insert(fileNameC);
    fileSet.insert(fileNameD);

    QStringList dirList,fileList;
    fileList << fileNameC << fileNameD;

    ITunesLocationGetITunesConfigFileNamesTest location;
    EXPECT_CALL(location,getFileAndDirectoryLocationsFromSettings(A<QSet<QString>& >(),A<QSet<QString>& >())).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(dirSet),SetArgReferee<1>(fileSet)));
    EXPECT_CALL(location,validateDirectorySet(Eq(dirSet))).Times(1).WillOnce(Return(dirList));
    EXPECT_CALL(location,validateFileSet(Eq(fileSet))).Times(1).WillOnce(Return(fileList));

    QStringList dbList = location.getITunesConfigFileNames();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==2);
    EXPECT_TRUE(dbSet.find(fileNameC)!=dbSet.end());
    EXPECT_TRUE(dbSet.find(fileNameD)!=dbSet.end());
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getITunesConfigFileNamesGivenDirectorySetHasADatabase)
{
    QString dirNameA  = "/Users/bonez/Music/iTunes";
    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Library.xml";

    QSet<QString> dirSet,fileSet;
    dirSet.insert(dirNameA);

    QStringList dirList,fileList;
    dirList << fileNameA;

    ITunesLocationGetITunesConfigFileNamesTest location;
    EXPECT_CALL(location,getFileAndDirectoryLocationsFromSettings(A<QSet<QString>& >(),A<QSet<QString>& >())).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(dirSet),SetArgReferee<1>(fileSet)));
    EXPECT_CALL(location,validateDirectorySet(Eq(dirSet))).Times(1).WillOnce(Return(dirList));
    EXPECT_CALL(location,validateFileSet(Eq(fileSet))).Times(1).WillOnce(Return(fileList));

    QStringList dbList = location.getITunesConfigFileNames();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==1);
    EXPECT_TRUE(dbSet.find(fileNameA)!=dbSet.end());
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getITunesConfigFileNamesGivenDirectorySetHasMultipleDatabases)
{
    QString dirNameA  = "/Users/bonez/Music/iTunes";
    QString dirNameB  = "/Users/git/Music/iTunes";
    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Library.xml";
    QString fileNameB = "/Users/git/Music/iTunes/iTunes Library.xml";

    QSet<QString> dirSet,fileSet;
    dirSet.insert(dirNameA);
    dirSet.insert(dirNameB);

    QStringList dirList,fileList;
    dirList << fileNameA << fileNameB;

    ITunesLocationGetITunesConfigFileNamesTest location;
    EXPECT_CALL(location,getFileAndDirectoryLocationsFromSettings(A<QSet<QString>& >(),A<QSet<QString>& >())).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(dirSet),SetArgReferee<1>(fileSet)));
    EXPECT_CALL(location,validateDirectorySet(Eq(dirSet))).Times(1).WillOnce(Return(dirList));
    EXPECT_CALL(location,validateFileSet(Eq(fileSet))).Times(1).WillOnce(Return(fileList));

    QStringList dbList = location.getITunesConfigFileNames();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==2);
    EXPECT_TRUE(dbSet.find(fileNameA)!=dbSet.end());
    EXPECT_TRUE(dbSet.find(fileNameB)!=dbSet.end());
}

//-------------------------------------------------------------------------------------------

TEST(ITunesLocation,getITunesConfigFileNamesGivenFileAndDirectorySetHasMultipleDatabases)
{
    QString dirNameA  = "/Users/bonez/Music/iTunes";
    QString dirNameB  = "/Users/git/Music/iTunes";
    QString fileNameA = "/Users/bonez/Music/iTunes/iTunes Library.xml";
    QString fileNameB = "/Users/git/Music/iTunes/iTunes Library.xml";
    QString fileNameC = "/Volumes/LaCie/Music/iTunes/iTunes Media/iTunes Music Library.xml";
    QString fileNameD = "/Volumes/Music/iTunes/iTunes Media/iTunes Music Library.xml";

    QSet<QString> dirSet,fileSet;
    dirSet.insert(dirNameA);
    dirSet.insert(dirNameB);
    fileSet.insert(fileNameC);
    fileSet.insert(fileNameD);

    QStringList dirList,fileList;
    dirList << fileNameA << fileNameB;
    fileList << fileNameC << fileNameD;

    ITunesLocationGetITunesConfigFileNamesTest location;
    EXPECT_CALL(location,getFileAndDirectoryLocationsFromSettings(A<QSet<QString>& >(),A<QSet<QString>& >())).Times(1)
        .WillOnce(DoAll(SetArgReferee<0>(dirSet),SetArgReferee<1>(fileSet)));
    EXPECT_CALL(location,validateDirectorySet(Eq(dirSet))).Times(1).WillOnce(Return(dirList));
    EXPECT_CALL(location,validateFileSet(Eq(fileSet))).Times(1).WillOnce(Return(fileList));

    QStringList dbList = location.getITunesConfigFileNames();

    QSet<QString> dbSet = QSet<QString>(dbList.begin(), dbList.end());
    EXPECT_TRUE(dbSet.size()==4);
    EXPECT_TRUE(dbSet.find(fileNameA)!=dbSet.end());
    EXPECT_TRUE(dbSet.find(fileNameB)!=dbSet.end());
    EXPECT_TRUE(dbSet.find(fileNameC)!=dbSet.end());
    EXPECT_TRUE(dbSet.find(fileNameD)!=dbSet.end());
}

//-------------------------------------------------------------------------------------------
