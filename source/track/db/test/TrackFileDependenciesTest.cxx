
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/db/inc/TrackFileDependencies.h"
#include "common/test/DiskMockIF.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;
using namespace track::db;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,findDependenciesInDirectory)
{
    common::DiskIF::instance("disk");

    QString fileCueNameA = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"cue/album.cue");
    QString fileCueNameB = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"cue/album.flac");

    QString fileNoneNameA = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"none/track1.flac");
    QString fileNoneNameB = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"none/track2.flac");
    QString fileNoneNameC = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"none/track3.flac");
    QString fileNoneNameD = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"none/unrelated.cue");

    QString filePicNameA = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"pic/cover.jpg");
    QString filePicNameB = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"pic/track1.flac");
    QString filePicNameC = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"pic/track2.flac");
    QString filePicNameD = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"pic/track3.flac");

    QString filePicCueNameA = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"piccue/album.cue");
    QString filePicCueNameB = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"piccue/album.flac");
    QString filePicCueNameC = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"piccue/cover.jpg");
    
    QString fileWavPackNameA = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"wavpack/track1.wv");
    QString fileWavPackNameB = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"wavpack/track1.wvc");
    QString fileWavPackNameC = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"wavpack/track2.wv");
    QString fileWavPackNameD = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"wavpack/track2.wvc");

    QSet<QString> set;
    QSet<QString>::iterator ppI;
    
    TrackFileDependencies dependency;
    
    ASSERT_TRUE(dependency.add(fileCueNameB));
    
    ASSERT_TRUE(dependency.add(fileNoneNameA));
    ASSERT_TRUE(dependency.add(fileNoneNameB));
    ASSERT_TRUE(dependency.add(fileNoneNameC));
    
    ASSERT_TRUE(dependency.add(filePicNameB));
    ASSERT_TRUE(dependency.add(filePicNameC));
    ASSERT_TRUE(dependency.add(filePicNameD));
    
    ASSERT_TRUE(dependency.add(filePicCueNameB));
    
    ASSERT_TRUE(dependency.add(fileWavPackNameA));
    ASSERT_TRUE(dependency.add(fileWavPackNameC));
    
    EXPECT_TRUE(dependency.hasDependency(fileCueNameB));
    set = dependency.dependency(fileCueNameB);
    EXPECT_EQ(1,set.size());
    EXPECT_TRUE(set.find(fileCueNameA)!=set.end());
    
    EXPECT_FALSE(dependency.hasDependency(fileNoneNameA));
    set = dependency.dependency(fileNoneNameA);
    EXPECT_TRUE(set.isEmpty());
    
    EXPECT_FALSE(dependency.hasDependency(fileNoneNameB));
    set = dependency.dependency(fileNoneNameB);
    EXPECT_TRUE(set.isEmpty());

    EXPECT_FALSE(dependency.hasDependency(fileNoneNameC));
    set = dependency.dependency(fileNoneNameC);
    EXPECT_TRUE(set.isEmpty());

    EXPECT_TRUE(dependency.hasDependency(filePicNameB));
    set = dependency.dependency(filePicNameB);
    EXPECT_EQ(1,set.size());
    EXPECT_TRUE(set.find(filePicNameA)!=set.end());

    EXPECT_TRUE(dependency.hasDependency(filePicNameC));
    set = dependency.dependency(filePicNameC);
    EXPECT_EQ(1,set.size());
    EXPECT_TRUE(set.find(filePicNameA)!=set.end());

    EXPECT_TRUE(dependency.hasDependency(filePicNameD));
    set = dependency.dependency(filePicNameD);
    EXPECT_EQ(1,set.size());
    EXPECT_TRUE(set.find(filePicNameA)!=set.end());

    EXPECT_TRUE(dependency.hasDependency(filePicCueNameB));
    set = dependency.dependency(filePicCueNameB);
    EXPECT_EQ(2,set.size());
    EXPECT_TRUE(set.find(filePicCueNameA)!=set.end());
    EXPECT_TRUE(set.find(filePicCueNameC)!=set.end());

    EXPECT_TRUE(dependency.hasDependency(fileWavPackNameA));
    set = dependency.dependency(fileWavPackNameA);
    EXPECT_EQ(1,set.size());
    EXPECT_TRUE(set.find(fileWavPackNameB)!=set.end());

    EXPECT_TRUE(dependency.hasDependency(fileWavPackNameC));
    set = dependency.dependency(fileWavPackNameC);
    EXPECT_EQ(1,set.size());
    EXPECT_TRUE(set.find(fileWavPackNameD)!=set.end());
    
    set = dependency.allDependencies();
    
    EXPECT_EQ(6,set.size());
    EXPECT_TRUE(set.find(fileCueNameA)!=set.end());
    EXPECT_TRUE(set.find(filePicNameA)!=set.end());
    EXPECT_TRUE(set.find(filePicCueNameA)!=set.end());
    EXPECT_TRUE(set.find(filePicCueNameC)!=set.end());
    EXPECT_TRUE(set.find(fileWavPackNameB)!=set.end());
    EXPECT_TRUE(set.find(fileWavPackNameD)!=set.end());
    
    EXPECT_FALSE(set.find(fileNoneNameD)!=set.end());
    
    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

class TrackFileDependenciesTest : public TrackFileDependencies
{
    public:
        TrackFileDependenciesTest();
        virtual ~TrackFileDependenciesTest();
        
        virtual QString testGetFileExtension(const QString& fileName) const;
        virtual FileType testGetTypeForFile(const QString& fileName) const;
        virtual QString testGetDirectoryNameFromFile(const QString& fileName) const;
        
        virtual bool testIsDependencyForImage(const QString& ext) const;
        virtual bool testIsDependencyForCueSheet(const QString& ext) const;
        virtual bool testIsDependencyForWavPackCorrection(const QString& ext) const;
        
        virtual QString testGetDependentImage(const QString& fileName,const QSet<QString>& fSet) const;
        virtual QString testGetDependentOfName(const QString& fileName,const QSet<QString>& fSet) const;
        
        virtual QString testGetFileNameWithExtension(const QString& fileName,const QString& ext) const;
};

//-------------------------------------------------------------------------------------------

TrackFileDependenciesTest::TrackFileDependenciesTest() : TrackFileDependencies()
{}

//-------------------------------------------------------------------------------------------

TrackFileDependenciesTest::~TrackFileDependenciesTest()
{}

//-------------------------------------------------------------------------------------------

QString TrackFileDependenciesTest::testGetFileExtension(const QString& fileName) const
{
    return getFileExtension(fileName);
}

//-------------------------------------------------------------------------------------------

TrackFileDependencies::FileType TrackFileDependenciesTest::testGetTypeForFile(const QString& fileName) const
{
    return getTypeForFile(fileName);
}

//-------------------------------------------------------------------------------------------

QString TrackFileDependenciesTest::testGetDirectoryNameFromFile(const QString& fileName) const
{
    return getDirectoryNameFromFile(fileName);
}

//-------------------------------------------------------------------------------------------

bool TrackFileDependenciesTest::testIsDependencyForImage(const QString& ext) const
{
    return isDependencyForImage(ext);
}

//-------------------------------------------------------------------------------------------

bool TrackFileDependenciesTest::testIsDependencyForCueSheet(const QString& ext) const
{
    return isDependencyForCueSheet(ext);
}

//-------------------------------------------------------------------------------------------

bool TrackFileDependenciesTest::testIsDependencyForWavPackCorrection(const QString& ext) const
{
    return isDependencyForWavPackCorrection(ext);
}

//-------------------------------------------------------------------------------------------

QString TrackFileDependenciesTest::testGetDependentImage(const QString& fileName,const QSet<QString>& fSet) const
{
    return getDependentImage(fileName,fSet);
}

//-------------------------------------------------------------------------------------------

QString TrackFileDependenciesTest::testGetDependentOfName(const QString& fileName,const QSet<QString>& fSet) const
{
    return getDependentOfName(fileName,fSet);
}

//-------------------------------------------------------------------------------------------

QString TrackFileDependenciesTest::testGetFileNameWithExtension(const QString& fileName,const QString& ext) const
{
    return getFileNameWithExtension(fileName,ext);
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getFileExtensionGivesLowerCaseGivenLowerCaseName)
{
    QString fileName = "/Users/Bonez/Music/track1.flac";
    TrackFileDependenciesTest dependency;
    QString ext = dependency.testGetFileExtension(fileName);
    EXPECT_TRUE(ext=="flac");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getFileExtensionGivesLowerCaseGivenUpperCaseName)
{
    QString fileName = "/Users/Bonez/Music/TRACK1.FLAC";
    TrackFileDependenciesTest dependency;
    QString ext = dependency.testGetFileExtension(fileName);
    EXPECT_TRUE(ext=="flac");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getFileExtensionGivesLowerCaseGivenMixedCaseName)
{
    QString fileName = "/Users/Bonez/Music/TrAcK.FlAc";
    TrackFileDependenciesTest dependency;
    QString ext = dependency.testGetFileExtension(fileName);
    EXPECT_TRUE(ext=="flac");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getFileExtensionGivesLowerCaseGivenUpperCaseNameWithWhiteSpaces)
{
    QString fileName = "  \t /Users/Bonez/Music/TRACK1.FLAC   \t   ";
    TrackFileDependenciesTest dependency;
    QString ext = dependency.testGetFileExtension(fileName);
    EXPECT_TRUE(ext=="flac");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getTypeForFileIsImageForJPEG)
{
    QString fileName = "file.jpeg";
    TrackFileDependenciesTest dependency;
    EXPECT_EQ(TrackFileDependencies::e_Image,dependency.testGetTypeForFile(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getTypeForFileIsImageForJPG)
{
    QString fileName = "file.jpg";
    TrackFileDependenciesTest dependency;
    EXPECT_EQ(TrackFileDependencies::e_Image,dependency.testGetTypeForFile(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getTypeForFileIsImageForGIF)
{
    QString fileName = "file.gif";
    TrackFileDependenciesTest dependency;
    EXPECT_EQ(TrackFileDependencies::e_Image,dependency.testGetTypeForFile(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getTypeForFileIsImageForPNG)
{
    QString fileName = "file.png";
    TrackFileDependenciesTest dependency;
    EXPECT_EQ(TrackFileDependencies::e_Image,dependency.testGetTypeForFile(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getTypeForFileIsCueSheetForCUE)
{
    QString fileName = "file.cue";
    TrackFileDependenciesTest dependency;
    EXPECT_EQ(TrackFileDependencies::e_CueSheet,dependency.testGetTypeForFile(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getTypeForFileIsWavPackCorrectionForWVC)
{
    QString fileName = "file.wvc";
    TrackFileDependenciesTest dependency;
    EXPECT_EQ(TrackFileDependencies::e_WavPackCorrection,dependency.testGetTypeForFile(fileName));
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getTypeForFileIsNoDependencyForTXT)
{
    QString fileName = "file.txt";
    TrackFileDependenciesTest dependency;
    EXPECT_EQ(TrackFileDependencies::e_NoDependency,dependency.testGetTypeForFile(fileName));
}

//-------------------------------------------------------------------------------------------

class TrackFileDependenciesCacheFileTest : public TrackFileDependenciesTest
{
    public:
        TrackFileDependenciesCacheFileTest();
        virtual ~TrackFileDependenciesCacheFileTest();
        MOCK_METHOD0(dirCacheMap,QMap<QString,QMap<FileType,QSet<QString> > >&());
        void testCacheFileBasedOnExtension(const QString& dirName,const QString& fileName);
};

//-------------------------------------------------------------------------------------------

TrackFileDependenciesCacheFileTest::TrackFileDependenciesCacheFileTest() : TrackFileDependenciesTest()
{}

//-------------------------------------------------------------------------------------------

TrackFileDependenciesCacheFileTest::~TrackFileDependenciesCacheFileTest()
{}

//-------------------------------------------------------------------------------------------

void TrackFileDependenciesCacheFileTest::testCacheFileBasedOnExtension(const QString& dirName,const QString& fileName)
{
    cacheFileBasedOnExtension(dirName,fileName);
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,cacheFileBasedOnExtensionGivenFileIsNonDependancy)
{
    QString dirName = "/Users/Bonez/Music";
    QString fileName = "cover.txt";
    TrackFileDependenciesCacheFileTest dependency;
    dependency.testCacheFileBasedOnExtension(dirName,fileName);
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,cacheFileBasedOnExtensionGivenFileAlreadyCached)
{
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >::iterator ppI;
    QMap<TrackFileDependencies::FileType,QSet<QString> >::iterator ppJ;
    QSet<QString>::iterator ppK;

    QString dirName = "/Users/Bonez/Music";
    QString fileName = "cover.jpg";
    
    QString fileNameB = "cover.jpg";
    QSet<QString> fileSet;
    fileSet.insert(fileNameB);
    QMap<TrackFileDependencies::FileType,QSet<QString> > typeMap;
    typeMap.insert(TrackFileDependencies::e_Image,fileSet);
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    dirCacheMap.insert(dirName,typeMap);
    
    TrackFileDependenciesCacheFileTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    dependency.testCacheFileBasedOnExtension(dirName,fileName);
    
    ppI = dirCacheMap.find(dirName);
    ASSERT_TRUE(ppI!=dirCacheMap.end());
    ppJ = ppI.value().find(TrackFileDependencies::e_Image);
    ASSERT_TRUE(ppJ!=ppI.value().end());
    ppK = ppJ.value().find(fileName);
    ASSERT_TRUE(ppK!=ppJ.value().end());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,cacheFileBasedOnExtensionGivenFileTypeCachedForAnotherFile)
{
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >::iterator ppI;
    QMap<TrackFileDependencies::FileType,QSet<QString> >::iterator ppJ;
    QSet<QString>::iterator ppK;

    QString dirName = "/Users/Bonez/Music";
    QString fileName = "cover.jpg";
    
    QString fileNameB = "album.jpg";
    QSet<QString> fileSet;
    fileSet.insert(fileNameB);
    QMap<TrackFileDependencies::FileType,QSet<QString> > typeMap;
    typeMap.insert(TrackFileDependencies::e_Image,fileSet);
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    dirCacheMap.insert(dirName,typeMap);
    
    TrackFileDependenciesCacheFileTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    dependency.testCacheFileBasedOnExtension(dirName,fileName);
    
    ppI = dirCacheMap.find(dirName);
    ASSERT_TRUE(ppI!=dirCacheMap.end());
    ppJ = ppI.value().find(TrackFileDependencies::e_Image);
    ASSERT_TRUE(ppJ!=ppI.value().end());
    ppK = ppJ.value().find(fileName);
    ASSERT_TRUE(ppK!=ppJ.value().end());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,cacheFileBasedOnExtensionGivenFileTypeNotCachedButDirectoryMapExists)
{
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >::iterator ppI;
    QMap<TrackFileDependencies::FileType,QSet<QString> >::iterator ppJ;
    QSet<QString>::iterator ppK;
    
    QString dirName = "/Users/Bonez/Music";
    QString fileName = "cover.jpg";
    
    QString fileNameB = "cover.cue";
    QSet<QString> fileSet;
    fileSet.insert(fileNameB);
    QMap<TrackFileDependencies::FileType,QSet<QString> > typeMap;
    typeMap.insert(TrackFileDependencies::e_CueSheet,fileSet);
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    dirCacheMap.insert(dirName,typeMap);
    
    TrackFileDependenciesCacheFileTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
        
    dependency.testCacheFileBasedOnExtension(dirName,fileName);
    
    ppI = dirCacheMap.find(dirName);
    ASSERT_TRUE(ppI!=dirCacheMap.end());
    ppJ = ppI.value().find(TrackFileDependencies::e_Image);
    ASSERT_TRUE(ppJ!=ppI.value().end());
    ppK = ppJ.value().find(fileName);
    ASSERT_TRUE(ppK!=ppJ.value().end());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,cacheFileBasedOnExtensionGivenNoEntryExistsForDirectory)
{
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >::iterator ppI;
    QMap<TrackFileDependencies::FileType,QSet<QString> >::iterator ppJ;
    QSet<QString>::iterator ppK;
    
    QString dirName = "/Users/Bonez/Music";
    QString fileName = "cover.jpg";
    
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    
    TrackFileDependenciesCacheFileTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    dependency.testCacheFileBasedOnExtension(dirName,fileName);
    
    ppI = dirCacheMap.find(dirName);
    ASSERT_TRUE(ppI!=dirCacheMap.end());
    ppJ = ppI.value().find(TrackFileDependencies::e_Image);
    ASSERT_TRUE(ppJ!=ppI.value().end());
    ppK = ppJ.value().find(fileName);
    ASSERT_TRUE(ppK!=ppJ.value().end());
}

//-------------------------------------------------------------------------------------------

class TrackFileDependenciesScanAndCacheDirectoryTest : public TrackFileDependenciesTest
{
    public:
        TrackFileDependenciesScanAndCacheDirectoryTest();
        virtual ~TrackFileDependenciesScanAndCacheDirectoryTest();
        MOCK_METHOD0(dirCacheMap,QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >&());
        bool testScanAndCacheDirectory(const QString& dirName);
};

//-------------------------------------------------------------------------------------------

TrackFileDependenciesScanAndCacheDirectoryTest::TrackFileDependenciesScanAndCacheDirectoryTest() : TrackFileDependenciesTest()
{}

//-------------------------------------------------------------------------------------------

TrackFileDependenciesScanAndCacheDirectoryTest::~TrackFileDependenciesScanAndCacheDirectoryTest()
{}

//-------------------------------------------------------------------------------------------

bool TrackFileDependenciesScanAndCacheDirectoryTest::testScanAndCacheDirectory(const QString& dirName)
{
    return scanAndCacheDirectory(dirName);
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,scanAndCacheDirectoryWhenDirectoryAlreadyCached)
{
    QString dirName = "/Users/Bonez/Music";
    
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    QMap<TrackFileDependencies::FileType,QSet<QString> > fileMap;
    dirCacheMap.insert(dirName,fileMap);
    
    TrackFileDependenciesScanAndCacheDirectoryTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    EXPECT_TRUE(dependency.testScanAndCacheDirectory(dirName));
    
    EXPECT_EQ(1,dirCacheMap.size());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,scanAndCacheDirectoryWhenGivenNameIsNotADirectory)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));
    
    QString dirName = "/Users/Bonez/Music";
    
    EXPECT_CALL(diskIF,isDirectory(dirName)).Times(1).WillOnce(Return(false));
    
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    
    TrackFileDependenciesScanAndCacheDirectoryTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    EXPECT_FALSE(dependency.testScanAndCacheDirectory(dirName));
    
    EXPECT_EQ(0,dirCacheMap.size());
    
    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,scanAndCacheDirectoryWhenDirectoryCannotBeOpenedToBeScanned)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));
    
    QString dirName = "/Users/Bonez/Music";
    
    EXPECT_CALL(diskIF,isDirectory(dirName)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(diskIF,openDirectory(dirName)).Times(1)
        .WillOnce(Return(common::DiskIF::c_invalidDirectoryHandle));
    
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >::iterator ppI;
        
    TrackFileDependenciesScanAndCacheDirectoryTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    EXPECT_FALSE(dependency.testScanAndCacheDirectory(dirName));
    
    EXPECT_EQ(0,dirCacheMap.size());
    
    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,scanAndCacheDirectoryWhenEmptyDirectory)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));
    
    QString dirName = "/Users/Bonez/Music";
    QString emptyName;
    
    EXPECT_CALL(diskIF,isDirectory(dirName)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(diskIF,openDirectory(dirName)).Times(1).WillOnce(Return((common::DiskIF::DirHandle)(1)));
    EXPECT_CALL(diskIF,nextDirectoryEntry((common::DiskIF::DirHandle)(1))).Times(1)
        .WillOnce(Return(emptyName));
    EXPECT_CALL(diskIF,closeDirectory((common::DiskIF::DirHandle)(1))).Times(1);
    
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >::iterator ppI;
        
    TrackFileDependenciesScanAndCacheDirectoryTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    EXPECT_TRUE(dependency.testScanAndCacheDirectory(dirName));
    
    EXPECT_EQ(1,dirCacheMap.size());
    
    ppI = dirCacheMap.find(dirName);
    ASSERT_TRUE(ppI!=dirCacheMap.end());
    EXPECT_EQ(0,ppI.value().size());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,scanAndCacheDirectoryWithFourFilesThreeFileDependAndOneSubDirectory)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));
    
    QString dirName = "/Users/Bonez/Music";
    QString fileNameA = "thealbum.cue";
    QString fileNameB = "thealbum.flac";
    QString fileNameC = "thealbum.jpg";
    QString fileNameD = "cover.jpg";
    QString subDirectoryNameA = "subdir";
    QString emptyName;
    
    QString fullNameA = common::DiskOps::mergeName(dirName,fileNameA);
    QString fullNameB = common::DiskOps::mergeName(dirName,fileNameB);
    QString fullNameC = common::DiskOps::mergeName(dirName,fileNameC);
    QString fullNameD = common::DiskOps::mergeName(dirName,fileNameD);
    QString fullNameE = common::DiskOps::mergeName(dirName,subDirectoryNameA);
    
    EXPECT_CALL(diskIF,isDirectory(dirName)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(diskIF,openDirectory(dirName)).Times(1).WillOnce(Return((common::DiskIF::DirHandle)(1)));
    
    EXPECT_CALL(diskIF,nextDirectoryEntry((common::DiskIF::DirHandle)(1))).Times(6)
        .WillOnce(Return(fileNameA))
        .WillOnce(Return(fileNameB))
        .WillOnce(Return(fileNameC))
        .WillOnce(Return(fileNameD))
        .WillOnce(Return(subDirectoryNameA))
        .WillOnce(Return(emptyName));
        
    EXPECT_CALL(diskIF,isFile(fullNameA)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(diskIF,isFile(fullNameB)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(diskIF,isFile(fullNameC)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(diskIF,isFile(fullNameD)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(diskIF,isFile(fullNameE)).Times(1).WillOnce(Return(false));
    
    EXPECT_CALL(diskIF,closeDirectory((common::DiskIF::DirHandle)(1))).Times(1);
    
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >::iterator ppI;
    QMap<TrackFileDependencies::FileType,QSet<QString> >::iterator ppJ;
    QSet<QString>::iterator ppK;
    
    TrackFileDependenciesScanAndCacheDirectoryTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    EXPECT_TRUE(dependency.testScanAndCacheDirectory(dirName));
    
    EXPECT_EQ(1,dirCacheMap.size());
    
    ppI = dirCacheMap.find(dirName);
    ASSERT_TRUE(ppI!=dirCacheMap.end());
    EXPECT_EQ(2,ppI.value().size());
    
    ppJ = ppI.value().find(TrackFileDependencies::e_CueSheet);
    ASSERT_TRUE(ppJ!=ppI.value().end());
    
    EXPECT_EQ(1,ppJ.value().size());
    ppK = ppJ.value().find(fileNameA);
    ASSERT_TRUE(ppK!=ppJ.value().end());
    
    ppJ = ppI.value().find(TrackFileDependencies::e_Image);
    ASSERT_TRUE(ppJ!=ppI.value().end());
    
    EXPECT_EQ(2,ppJ.value().size());
    ppK=ppJ.value().find(fileNameC);
    ASSERT_TRUE(ppK!=ppJ.value().end());
    ppK=ppJ.value().find(fileNameD);
    ASSERT_TRUE(ppK!=ppJ.value().end());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDirectoryNameFromFileWhenEmpty)
{
    QString fileName = "";
    TrackFileDependenciesTest dependency;
    QString dirName = dependency.testGetDirectoryNameFromFile(fileName);
    EXPECT_TRUE(dirName.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDirectoryNameFromFileWhenUNIXRoot)
{
    QString fileName = "/album.flac";
    TrackFileDependenciesTest dependency;
    QString dirName = dependency.testGetDirectoryNameFromFile(fileName);
    EXPECT_TRUE(dirName=="");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDirectoryNameFromFileWhenDOSDriveLetter)
{
    QString fileName = "C:\\\\album.flac";
    TrackFileDependenciesTest dependency;
    QString dirName = dependency.testGetDirectoryNameFromFile(fileName);
    EXPECT_TRUE(dirName=="C:\\");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDirectoryNameFromFileWhenDOSNetworkShare)
{
    QString fileName = "\\\\ORACLE\\Music\\album.flac";
    TrackFileDependenciesTest dependency;
    QString dirName = dependency.testGetDirectoryNameFromFile(fileName);
    EXPECT_TRUE(dirName=="\\\\ORACLE\\Music");    
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDirectoryNameFromFileWhenUNIXInSubDirectory)
{
    QString fileName = "/Users/Bonez/Music/album.flac";
    TrackFileDependenciesTest dependency;
    QString dirName = dependency.testGetDirectoryNameFromFile(fileName);
    EXPECT_TRUE(dirName=="/Users/Bonez/Music");    
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDirectoryNameFromFileWhenDOSInSubDirectory)
{
    QString fileName = "C:\\\\Music\\Film\\album.flac";
    TrackFileDependenciesTest dependency;
    QString dirName = dependency.testGetDirectoryNameFromFile(fileName);
    EXPECT_TRUE(dirName=="C:\\\\Music\\Film");    
}

//-------------------------------------------------------------------------------------------

class TrackFileDependenciesAddTest : public TrackFileDependenciesTest
{
    public:
        MOCK_METHOD0(files,QSet<QString>&());
        MOCK_METHOD1(scanAndCacheDirectory,bool(const QString& dirName));
        MOCK_METHOD2(cacheDependentFileIfExists,void(const QString& fileName,const QString& ext));
};

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,addGivenFileAlreadyAdded)
{
    QString fileName = "/Users/Bonez/Music/Album/track.flac";

    QSet<QString> files;
    files.insert(fileName);
    
    TrackFileDependenciesAddTest dependency;
    EXPECT_CALL(dependency,files()).WillRepeatedly(ReturnRef(files));
    
    EXPECT_TRUE(dependency.add(fileName));
    
    EXPECT_TRUE(files.find(fileName)!=files.end());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,addGivenFileIsNotAFile)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString fileName = "/Users/Bonez/Music/Album/track.flac";

    QSet<QString> files;
    
    EXPECT_CALL(diskIF,isFile(fileName)).Times(1).WillOnce(Return(false));
    
    TrackFileDependenciesAddTest dependency;
    EXPECT_CALL(dependency,files()).WillRepeatedly(ReturnRef(files));
    
    EXPECT_FALSE(dependency.add(fileName));
    
    EXPECT_TRUE(files.find(fileName)==files.end());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,addGivenCacheAndScanOfDirectoryFailsWithNoSameNameDependency)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString fileName = "/Users/Bonez/Music/Album/track.mp3";
    QString dirName = "/Users/Bonez/Music/Album";

    QSet<QString> files;
    
    EXPECT_CALL(diskIF,isFile(fileName)).Times(1).WillOnce(Return(true));
    
    TrackFileDependenciesAddTest dependency;
    EXPECT_CALL(dependency,files()).WillRepeatedly(ReturnRef(files));
    EXPECT_CALL(dependency,scanAndCacheDirectory(dirName)).Times(1).WillOnce(Return(false));
    
    EXPECT_TRUE(dependency.add(fileName));
    
    EXPECT_TRUE(files.find(fileName)!=files.end());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,addGivenCacheAndScanOfDirectoryFailsWithSameNameDependency)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString fileName = "/Users/Bonez/Music/Album/track.wv";
    QString dirName = "/Users/Bonez/Music/Album";
    QString cueExt = "cue";
    QString wvcExt = "wvc";

    QSet<QString> files;
    
    EXPECT_CALL(diskIF,isFile(fileName)).Times(1).WillOnce(Return(true));
    
    TrackFileDependenciesAddTest dependency;
    EXPECT_CALL(dependency,files()).WillRepeatedly(ReturnRef(files));
    EXPECT_CALL(dependency,scanAndCacheDirectory(dirName)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(dependency,cacheDependentFileIfExists(fileName,cueExt)).Times(1);
    EXPECT_CALL(dependency,cacheDependentFileIfExists(fileName,wvcExt)).Times(1);
    
    EXPECT_TRUE(dependency.add(fileName));
    
    EXPECT_TRUE(files.find(fileName)!=files.end());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,addGivenCacheAndScanOfDirectorySucceeds)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));

    QString fileName = "/Users/Bonez/Music/Album/track.flac";
    QString dirName = "/Users/Bonez/Music/Album";

    QSet<QString> files;

    EXPECT_CALL(diskIF,isFile(fileName)).Times(1).WillOnce(Return(true));
    
    TrackFileDependenciesAddTest dependency;
    EXPECT_CALL(dependency,files()).WillRepeatedly(ReturnRef(files));
    EXPECT_CALL(dependency,scanAndCacheDirectory(dirName)).Times(1).WillOnce(Return(true));
    
    EXPECT_TRUE(dependency.add(fileName));
    
    EXPECT_TRUE(files.find(fileName)!=files.end());

    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,isDependencyForImageCheckMediaTypes)
{
    TrackFileDependenciesTest dependency;
    
    EXPECT_TRUE(dependency.testIsDependencyForImage("mp3"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("m4a"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("m4b"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("ogg"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("wav"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("flac"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("aif"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("aiff"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("mpc"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("mp+"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("mpp"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("ape"));
    EXPECT_TRUE(dependency.testIsDependencyForImage("wv"));
    
    EXPECT_FALSE(dependency.testIsDependencyForImage("m3u"));
    EXPECT_FALSE(dependency.testIsDependencyForImage("pls"));
    EXPECT_FALSE(dependency.testIsDependencyForImage("xspf"));
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,isDependencyForCueSheetCheckMediaTypes)
{
    TrackFileDependenciesTest dependency;
    
    EXPECT_FALSE(dependency.testIsDependencyForCueSheet("mp3"));
    EXPECT_FALSE(dependency.testIsDependencyForCueSheet("m4a"));
    EXPECT_FALSE(dependency.testIsDependencyForCueSheet("m4b"));
    EXPECT_FALSE(dependency.testIsDependencyForCueSheet("ogg"));
    
    EXPECT_TRUE(dependency.testIsDependencyForCueSheet("wav"));
    EXPECT_TRUE(dependency.testIsDependencyForCueSheet("flac"));
    EXPECT_TRUE(dependency.testIsDependencyForCueSheet("aif"));
    EXPECT_TRUE(dependency.testIsDependencyForCueSheet("aiff"));
    EXPECT_TRUE(dependency.testIsDependencyForCueSheet("mpc"));
    EXPECT_TRUE(dependency.testIsDependencyForCueSheet("mp+"));
    EXPECT_TRUE(dependency.testIsDependencyForCueSheet("mpp"));
    EXPECT_TRUE(dependency.testIsDependencyForCueSheet("ape"));
    EXPECT_TRUE(dependency.testIsDependencyForCueSheet("wv"));
    
    EXPECT_FALSE(dependency.testIsDependencyForCueSheet("m3u"));
    EXPECT_FALSE(dependency.testIsDependencyForCueSheet("pls"));
    EXPECT_FALSE(dependency.testIsDependencyForCueSheet("xspf"));
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,isDependencyForWavPackCorrectionCheckMediaTypes)
{
    TrackFileDependenciesTest dependency;
    
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("mp3"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("m4a"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("m4b"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("ogg"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("wav"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("flac"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("aif"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("aiff"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("mpc"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("mp+"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("mpp"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("ape"));
    
    EXPECT_TRUE(dependency.testIsDependencyForWavPackCorrection("wv"));
    
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("m3u"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("pls"));
    EXPECT_FALSE(dependency.testIsDependencyForWavPackCorrection("xspf"));
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDependentImageGivesNoneWhenNoFilesAreGiven)
{
    QSet<QString> fSet;
    QString fileName = "/Users/Bonez/Music/album.flac";
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetDependentImage(fileName,fSet)=="");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDependentImageGivesTheOnlyImage)
{
    QSet<QString> fSet;
    fSet.insert("cover.jpg");
    QString fileName = "/Users/Bonez/Music/album.flac";
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetDependentImage(fileName,fSet)=="cover.jpg");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDependentImageGivesTheClosestImageNameToFileName)
{
    QSet<QString> fSet;
    fSet.insert("cover.jpg");
    fSet.insert("albm.png");
    QString fileName = "/Users/Bonez/Music/album.flac";
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetDependentImage(fileName,fSet)=="albm.png");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDependentOfNameWhenNoneInSet)
{
    QSet<QString> fSet;
    QString fileName = "/Users/Bonez/Music/bill daily.flac";
    
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetDependentOfName(fileName,fSet).isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDependentOfNameWhenNoMatchingName)
{
    QSet<QString> fSet;
    fSet.insert("bill erona.cue");
    fSet.insert("gale summer.cue");
    fSet.insert("word.cue");
    
    QString fileName = "/Users/Bonez/Music/bill daily.flac";
    
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetDependentOfName(fileName,fSet).isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDependentOfNameWithMatchingNameSameCase)
{
    QSet<QString> fSet;
    fSet.insert("Bill Daily.cue");
    fSet.insert("Gale Summer.cue");
    fSet.insert("Word.cue");
    
    QString fileName = "/Users/Bonez/Music/Bill Daily.flac";
    
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetDependentOfName(fileName,fSet)=="Bill Daily.cue");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getDependentOfNameWithMatchingNameDifferentCase)
{
    QSet<QString> fSet;
    fSet.insert("bill daily.CUE");
    fSet.insert("Gale Summer.cue");
    fSet.insert("Word.cue");
    
    QString fileName = "/Users/Bonez/Music/BILL DAILY.flac";
    
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetDependentOfName(fileName,fSet)=="bill daily.CUE");
}

//-------------------------------------------------------------------------------------------

class TrackFileDependenciesDependencyTest : public TrackFileDependencies
{
    public:
        MOCK_CONST_METHOD0(dirCacheMapConst,const QMap<QString,QMap<FileType,QSet<QString> > >&());
};

//-------------------------------------------------------------------------------------------

void TrackFileDependenciesBuildDependencyCache(QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >& dirCacheMap)
{
    QSet<QString> imageSet;
    imageSet.insert("randy whatson.png");
    imageSet.insert("clegg of dale.jpg");
    imageSet.insert("hi again from today.gif");
    
    QSet<QString> cueSet;
    cueSet.insert("clegg of dale.cue");
    cueSet.insert("randy whatson.cue");
    
    QSet<QString> wvcSet;
    wvcSet.insert("clegg of dale - track 1.wvc");
    wvcSet.insert("clegg of dale - track 2.wvc");
    wvcSet.insert("clegg of dale - track 3.wvc");
    wvcSet.insert("clegg of dale - track 4.wvc");
    
    QMap<TrackFileDependencies::FileType,QSet<QString> > typeMap;
    typeMap.insert(TrackFileDependencies::e_Image,imageSet);
    typeMap.insert(TrackFileDependencies::e_CueSheet,cueSet);
    typeMap.insert(TrackFileDependencies::e_WavPackCorrection,wvcSet);
    
    dirCacheMap.clear();
    dirCacheMap.insert("/Users/Bonez/Music",typeMap);
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,dependencyWithNoFileName)
{
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    TrackFileDependenciesBuildDependencyCache(dirCacheMap);
    
    TrackFileDependenciesDependencyTest dependency;
    EXPECT_CALL(dependency,dirCacheMapConst()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    QSet<QString> fSet = dependency.dependency("");
    
    EXPECT_TRUE(fSet.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,dependencyWithNoMatchingDirectory)
{
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    TrackFileDependenciesBuildDependencyCache(dirCacheMap);
    
    TrackFileDependenciesDependencyTest dependency;
    EXPECT_CALL(dependency,dirCacheMapConst()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    QSet<QString> fSet = dependency.dependency("/Users/Bonez/clegg of dale - track 1.wv");
    
    EXPECT_TRUE(fSet.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,dependencyWithNoEmptyDirectory)
{
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    TrackFileDependenciesBuildDependencyCache(dirCacheMap);
    QMap<TrackFileDependencies::FileType,QSet<QString> > typeMap;
    dirCacheMap.insert("/Users/Bonez",typeMap);
    
    TrackFileDependenciesDependencyTest dependency;
    EXPECT_CALL(dependency,dirCacheMapConst()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    QSet<QString> fSet = dependency.dependency("/Users/Bonez/clegg of dale - track 1.wv");
    
    EXPECT_TRUE(fSet.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,dependencyWithImage)
{
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    TrackFileDependenciesBuildDependencyCache(dirCacheMap);
    QMap<TrackFileDependencies::FileType,QSet<QString> > typeMap;
    dirCacheMap.insert("/Users/Bonez",typeMap);
    
    TrackFileDependenciesDependencyTest dependency;
    EXPECT_CALL(dependency,dirCacheMapConst()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    QSet<QString> fSet = dependency.dependency("/Users/Bonez/Music/hi again from today.flac");
    
    EXPECT_EQ(1,fSet.size());
    EXPECT_TRUE(fSet.find(QDir::toNativeSeparators("/Users/Bonez/Music/hi again from today.gif"))!=fSet.end());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,dependencyWithImageAndCue)
{
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    TrackFileDependenciesBuildDependencyCache(dirCacheMap);
    QMap<TrackFileDependencies::FileType,QSet<QString> > typeMap;
    dirCacheMap.insert("/Users/Bonez",typeMap);
    
    TrackFileDependenciesDependencyTest dependency;
    EXPECT_CALL(dependency,dirCacheMapConst()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    QSet<QString> fSet = dependency.dependency("/Users/Bonez/Music/clegg of dale.flac");
    
    EXPECT_EQ(2,fSet.size());
    EXPECT_TRUE(fSet.find(QDir::toNativeSeparators("/Users/Bonez/Music/clegg of dale.jpg"))!=fSet.end());
    EXPECT_TRUE(fSet.find(QDir::toNativeSeparators("/Users/Bonez/Music/clegg of dale.cue"))!=fSet.end());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,dependencyWithImageAndCorrection)
{
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    TrackFileDependenciesBuildDependencyCache(dirCacheMap);
    QMap<TrackFileDependencies::FileType,QSet<QString> > typeMap;
    dirCacheMap.insert("/Users/Bonez",typeMap);
    
    TrackFileDependenciesDependencyTest dependency;
    EXPECT_CALL(dependency,dirCacheMapConst()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    QSet<QString> fSet = dependency.dependency("/Users/Bonez/Music/clegg of dale - track 2.wv");
    
    EXPECT_EQ(2,fSet.size());
    EXPECT_TRUE(fSet.find(QDir::toNativeSeparators("/Users/Bonez/Music/clegg of dale.jpg"))!=fSet.end());
    EXPECT_TRUE(fSet.find(QDir::toNativeSeparators("/Users/Bonez/Music/clegg of dale - track 2.wvc"))!=fSet.end());
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getFileNameWithExtensionNoNameAndNoExtension)
{
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetFileNameWithExtension("","")==".");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getFileNameWithExtensionNoNameAndExtension)
{
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetFileNameWithExtension("","cue")==".cue");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getFileNameWithExtensionNameAndNoExtension)
{
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetFileNameWithExtension("The Name","")=="The Name.");
    EXPECT_TRUE(dependency.testGetFileNameWithExtension("The Name.wav","")=="The Name.");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getFileNameWithExtensionNameAndExtension)
{
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetFileNameWithExtension("The Name","cue")=="The Name.cue");
    EXPECT_TRUE(dependency.testGetFileNameWithExtension("The Name.","cue")=="The Name.cue");
    EXPECT_TRUE(dependency.testGetFileNameWithExtension("The Name.wav","cue")=="The Name.cue");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getFileNameWithExtensionDOSPathNameAndExtension)
{
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetFileNameWithExtension("C:\\\\Music\\Album\\The Name","cue")=="C:\\\\Music\\Album\\The Name.cue");
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,getFileNameWithExtensionUnixPathNameAndExtension)
{
    TrackFileDependenciesTest dependency;
    EXPECT_TRUE(dependency.testGetFileNameWithExtension("/Users/Bonez/Music/Album/The Name","cue")=="/Users/Bonez/Music/Album/The Name.cue");
}

//-------------------------------------------------------------------------------------------

class TrackFileDependenciesCacheDependentFileIfExistsTest : public TrackFileDependenciesTest
{
    public:
        MOCK_METHOD0(dirCacheMap,QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >&());
        void testCacheDependentFileIfExists(const QString& fileName,const QString& ext);
};

//-------------------------------------------------------------------------------------------

void TrackFileDependenciesCacheDependentFileIfExistsTest::testCacheDependentFileIfExists(const QString& fileName,const QString& ext)
{
    cacheDependentFileIfExists(fileName,ext);
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,cacheDependentFileIfExistsFileDoesNotExist)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));
    
    QString fileName = "C:\\\\Users\\Bonez\\Music\\Album\\The Name.flac";
    QString cueFileName = "C:\\\\Users\\Bonez\\Music\\Album\\The Name.cue";
    
    EXPECT_CALL(diskIF,isFile(cueFileName)).Times(1).WillOnce(Return(false));
    
    TrackFileDependenciesCacheDependentFileIfExistsTest dependency;
    
    dependency.testCacheDependentFileIfExists(fileName,"cue");
    
    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,cacheDependentFileIfExistsFileWithDOSPathName)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));
    
    QString fileName = "C:\\\\Users\\Bonez\\Music\\Album\\The Name.flac";
    QString cueFileName = "C:\\\\Users\\Bonez\\Music\\Album\\The Name.cue";
    
    EXPECT_CALL(diskIF,isFile(cueFileName)).Times(1).WillOnce(Return(true));
    
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >::iterator ppI;
    QMap<TrackFileDependencies::FileType,QSet<QString> >::iterator ppJ;
    QSet<QString>::iterator ppK;
    
    TrackFileDependenciesCacheDependentFileIfExistsTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    dependency.testCacheDependentFileIfExists(fileName,"cue");
    
    ASSERT_EQ(1,dirCacheMap.size());
    ppI = dirCacheMap.find("C:\\\\Users\\Bonez\\Music\\Album");
    ASSERT_TRUE(ppI!=dirCacheMap.end());
    ASSERT_EQ(1,ppI.value().size());
    ppJ = ppI.value().find(TrackFileDependencies::e_CueSheet);
    ASSERT_TRUE(ppJ!=ppI.value().end());
    ASSERT_EQ(1,ppJ.value().size());
    ppK = ppJ.value().find("The Name.cue");
    ASSERT_TRUE(ppK!=ppJ.value().end());
    
    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(TrackFileDependencies,cacheDependentFileIfExistsFileWithUNIXPathName)
{
    common::DiskIFSPtr pMockAPI = common::DiskIF::instance("mock");
    common::DiskMockIF& diskIF = dynamic_cast<common::DiskMockIF&>(*(pMockAPI.data()));
    
    QString fileName = "/Users/Bonez/Music/Album/The Name.flac";
    QString cueFileName = "/Users/Bonez/Music/Album/The Name.cue";
    
    EXPECT_CALL(diskIF,isFile(cueFileName)).Times(1).WillOnce(Return(true));
    
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > > dirCacheMap;
    QMap<QString,QMap<TrackFileDependencies::FileType,QSet<QString> > >::iterator ppI;
    QMap<TrackFileDependencies::FileType,QSet<QString> >::iterator ppJ;
    QSet<QString>::iterator ppK;
    
    TrackFileDependenciesCacheDependentFileIfExistsTest dependency;
    EXPECT_CALL(dependency,dirCacheMap()).WillRepeatedly(ReturnRef(dirCacheMap));
    
    dependency.testCacheDependentFileIfExists(fileName,"cue");
    
    ASSERT_EQ(1,dirCacheMap.size());
    ppI = dirCacheMap.find("/Users/Bonez/Music/Album");
    ASSERT_TRUE(ppI!=dirCacheMap.end());
    ASSERT_EQ(1,ppI.value().size());
    ppJ = ppI.value().find(TrackFileDependencies::e_CueSheet);
    ASSERT_TRUE(ppJ!=ppI.value().end());
    ASSERT_EQ(1,ppJ.value().size());
    ppK = ppJ.value().find("The Name.cue");
    ASSERT_TRUE(ppK!=ppJ.value().end());
    
    common::DiskIF::release();
}

//-------------------------------------------------------------------------------------------
