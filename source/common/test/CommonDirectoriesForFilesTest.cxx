#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/CommonDirectoriesForFiles.h"

using namespace testing;
using namespace omega;
using namespace omega::common;

//-------------------------------------------------------------------------------------------

class CommonDirectoriesForFilesTest : public CommonDirectoriesForFiles
{
    public:
        QString testGetVolumeName(const QString& fileName,int& index) const;
        QString testNextResourceName(const QString& fileName,int& index) const;
};

//-------------------------------------------------------------------------------------------

QString CommonDirectoriesForFilesTest::testGetVolumeName(const QString& fileName,int& index) const
{
    return getVolumeName(fileName,index);
}

//-------------------------------------------------------------------------------------------

QString CommonDirectoriesForFilesTest::testNextResourceName(const QString& fileName,int& index) const
{
    return nextResourceName(fileName,index);
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,getVolumeNameForDriveLetter)
{
    int index;
    QString volumeName;
    CommonDirectoriesForFilesTest commonDir;
    
    volumeName = commonDir.testGetVolumeName("A:",index);
    EXPECT_TRUE(volumeName=="A:");
    EXPECT_TRUE(index==2);

    volumeName = commonDir.testGetVolumeName("d:\\",index);
    EXPECT_TRUE(volumeName=="d:");
    EXPECT_TRUE(index==3);

    volumeName = commonDir.testGetVolumeName("E:\\//",index);
    EXPECT_TRUE(volumeName=="E:");
    EXPECT_TRUE(index==5);

    volumeName = commonDir.testGetVolumeName("C:\\Root\\Dir",index);
    EXPECT_TRUE(volumeName=="C:");
    EXPECT_TRUE(index==3);

    volumeName = commonDir.testGetVolumeName("Z:Root\\Dir",index);
    EXPECT_TRUE(volumeName=="Z:");
    EXPECT_TRUE(index==2);

    volumeName = commonDir.testGetVolumeName("Z:///Root",index);
    EXPECT_TRUE(volumeName=="Z:");
    EXPECT_TRUE(index==5);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,getVolumeNameForNetworkShare)
{
    int index;
    QString volumeName;
    CommonDirectoriesForFilesTest commonDir;
    
    volumeName = commonDir.testGetVolumeName("\\\\Oracle\\Movies",index);
    EXPECT_TRUE(volumeName=="\\\\Oracle\\Movies");
    EXPECT_TRUE(index==15);

    volumeName = commonDir.testGetVolumeName("\\\\Oracle\\Movies\\",index);
    EXPECT_TRUE(volumeName=="\\\\Oracle\\Movies");
    EXPECT_TRUE(index==16);

    volumeName = commonDir.testGetVolumeName("\\\\Oracle\\Movies\\Path\\To\\Resource",index);
    EXPECT_TRUE(volumeName=="\\\\Oracle\\Movies");
    EXPECT_TRUE(index==16);

    volumeName = commonDir.testGetVolumeName("//A/B",index);
    EXPECT_TRUE(volumeName=="\\\\A\\B");
    EXPECT_TRUE(index==5);

    volumeName = commonDir.testGetVolumeName("//a/b/path/to/resource",index);
    EXPECT_TRUE(volumeName=="\\\\a\\b");
    EXPECT_TRUE(index==6);

    volumeName = commonDir.testGetVolumeName("\\\\name\\\\resource\\path",index);
    EXPECT_TRUE(volumeName=="\\\\name\\resource");
    EXPECT_TRUE(index==17);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,getVolumeNameWhenInvalid)
{
    int index;
    QString volumeName;
    CommonDirectoriesForFilesTest commonDir;
    
    volumeName = commonDir.testGetVolumeName(".:/path",index);
    EXPECT_TRUE(volumeName.isEmpty());

    volumeName = commonDir.testGetVolumeName("",index);
    EXPECT_TRUE(volumeName.isEmpty());

    volumeName = commonDir.testGetVolumeName("name",index);
    EXPECT_TRUE(volumeName.isEmpty());

    volumeName = commonDir.testGetVolumeName("/n:/ame",index);
    EXPECT_TRUE(volumeName.isEmpty());

    volumeName = commonDir.testGetVolumeName("_n:/ame",index);
    EXPECT_TRUE(volumeName.isEmpty());

    volumeName = commonDir.testGetVolumeName("//name",index);
    EXPECT_TRUE(volumeName.isEmpty());

    volumeName = commonDir.testGetVolumeName("\\name\\resource\\path",index);
    EXPECT_TRUE(volumeName.isEmpty());

    volumeName = commonDir.testGetVolumeName("///name\\resource\\path",index);
    EXPECT_TRUE(volumeName.isEmpty());
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,getVolumeNameInRootDirectory)
{
    int index;
    QString volumeName;
    CommonDirectoriesForFilesTest commonDir;
    
    volumeName = commonDir.testGetVolumeName("/",index);
    EXPECT_TRUE(volumeName=="/");
    EXPECT_TRUE(index==1);

    volumeName = commonDir.testGetVolumeName("/var",index);
    EXPECT_TRUE(volumeName=="/");
    EXPECT_TRUE(index==1);

    volumeName = commonDir.testGetVolumeName("/usr/bin",index);
    EXPECT_TRUE(volumeName=="/");
    EXPECT_TRUE(index==1);

    volumeName = commonDir.testGetVolumeName("///usr//local/bin",index);
    EXPECT_TRUE(volumeName=="/");
    EXPECT_TRUE(index==3);

    volumeName = commonDir.testGetVolumeName("///usr name//local/bin",index);
    EXPECT_TRUE(volumeName=="/");
    EXPECT_TRUE(index==3);

    volumeName = commonDir.testGetVolumeName("/volumes",index);
    EXPECT_TRUE(volumeName=="/");
    EXPECT_TRUE(index==1);

    volumeName = commonDir.testGetVolumeName("/Users",index);
    EXPECT_TRUE(volumeName=="/");
    EXPECT_TRUE(index==1);

    volumeName = commonDir.testGetVolumeName("/Volumes",index);
    EXPECT_TRUE(volumeName=="/");
    EXPECT_TRUE(index==1);

    volumeName = commonDir.testGetVolumeName("/users/bonez",index);
    EXPECT_TRUE(volumeName=="/");
    EXPECT_TRUE(index==1);

    volumeName = commonDir.testGetVolumeName("/volumes/LaCie",index);
    EXPECT_TRUE(volumeName=="/");
    EXPECT_TRUE(index==1);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,getVolumeNameForUsersDirectory)
{
    int index;
    QString volumeName;
    CommonDirectoriesForFilesTest commonDir;
    
    volumeName = commonDir.testGetVolumeName("/Users/Bonez",index);
    EXPECT_TRUE(volumeName=="/Users/Bonez");
    EXPECT_TRUE(index==12);

    volumeName = commonDir.testGetVolumeName("/Users/Bonez/dir",index);
    EXPECT_TRUE(volumeName=="/Users/Bonez");
    EXPECT_TRUE(index==13);

    volumeName = commonDir.testGetVolumeName("/Users/Stuart MacLean",index);
    EXPECT_TRUE(volumeName=="/Users/Stuart MacLean");
    EXPECT_TRUE(index==21);

    volumeName = commonDir.testGetVolumeName("/Users/Stuart MacLean///dir/path",index);
    EXPECT_TRUE(volumeName=="/Users/Stuart MacLean");
    EXPECT_TRUE(index==24);

    volumeName = commonDir.testGetVolumeName("\\\\\\Users\\\\git\\\\path",index);
    EXPECT_TRUE(volumeName=="/Users/git");
    EXPECT_TRUE(index==15);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,getVolumeNameForMountedVolumes)
{
    int index;
    QString volumeName;
    CommonDirectoriesForFilesTest commonDir;
    
    volumeName = commonDir.testGetVolumeName("/Volumes/LaCie",index);
    EXPECT_TRUE(volumeName=="/Volumes/LaCie");
    EXPECT_TRUE(index==14);

    volumeName = commonDir.testGetVolumeName("/Volumes/LaCie/directory",index);
    EXPECT_TRUE(volumeName=="/Volumes/LaCie");
    EXPECT_TRUE(index==15);

    volumeName = commonDir.testGetVolumeName("/Volumes/LaCie 1",index);
    EXPECT_TRUE(volumeName=="/Volumes/LaCie 1");
    EXPECT_TRUE(index==16);

    volumeName = commonDir.testGetVolumeName("///Volumes///LaCie 1//Music",index);
    EXPECT_TRUE(volumeName=="/Volumes/LaCie 1");
    EXPECT_TRUE(index==22);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,getVolumeNameWhenInvalid)
{
    int index;
    QString volumeName;
    CommonDirectoriesForFilesTest commonDir;
    
    volumeName = commonDir.testGetVolumeName("name/",index);
    EXPECT_TRUE(volumeName.isEmpty());

    volumeName = commonDir.testGetVolumeName("C:/dir",index);
    EXPECT_TRUE(volumeName.isEmpty());

    volumeName = commonDir.testGetVolumeName("",index);
    EXPECT_TRUE(volumeName.isEmpty());
}


//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,getVolumeNameWhenRelative)
{
    int index;
    QString volumeName;
    CommonDirectoriesForFilesTest commonDir;
    
    volumeName = commonDir.testGetVolumeName(".",index);
    EXPECT_TRUE(volumeName.isEmpty());    

    volumeName = commonDir.testGetVolumeName("./",index);
    EXPECT_TRUE(volumeName.isEmpty());    

    volumeName = commonDir.testGetVolumeName("./path/to/resource",index);
    EXPECT_TRUE(volumeName.isEmpty());    

    volumeName = commonDir.testGetVolumeName("..",index);
    EXPECT_TRUE(volumeName.isEmpty());    

    volumeName = commonDir.testGetVolumeName("../",index);
    EXPECT_TRUE(volumeName.isEmpty());    

    volumeName = commonDir.testGetVolumeName("../path/to/resource",index);
    EXPECT_TRUE(volumeName.isEmpty());    
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,nextResourceNameGivenEmptyPath)
{
    int index = 0;
    QString resourceName;
    CommonDirectoriesForFilesTest commonDir;
    
    resourceName = commonDir.testNextResourceName("",index);
    EXPECT_TRUE(resourceName.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,nextResourceNameGivenSingleResource)
{
    int index = 0;
    QString resourceName;
    CommonDirectoriesForFilesTest commonDir;
    
    resourceName = commonDir.testNextResourceName("bonez",index);
    EXPECT_TRUE(resourceName=="bonez");
    EXPECT_TRUE(index==5);
    
    index = 3;
    resourceName = commonDir.testNextResourceName("bonez",index);
    EXPECT_TRUE(resourceName=="ez");
    EXPECT_TRUE(index==5);
    
    index = 5;
    resourceName = commonDir.testNextResourceName("bonez",index);
    EXPECT_TRUE(resourceName=="");
    EXPECT_TRUE(index==5);

    index = 1;
    resourceName = commonDir.testNextResourceName("/bonez/",index);
    EXPECT_TRUE(resourceName=="bonez");
    EXPECT_TRUE(index==7);

    index = 1;
    resourceName = commonDir.testNextResourceName("\\bonez\\",index);
    EXPECT_TRUE(resourceName=="bonez");
    EXPECT_TRUE(index==7);

    index = 0;
    resourceName = commonDir.testNextResourceName("\\bonez\\",index);
    EXPECT_TRUE(resourceName=="bonez");
    EXPECT_TRUE(index==7);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,nextResourceNameGivenMultipleSeparators)
{
    int index = 0;
    QString resourceName,fileName;
    CommonDirectoriesForFilesTest commonDir;
    
    //          0         10          20         30        
    //          01234567890123456 7 8901234567 890123456 789
    fileName = "///Users///stuart\\\\Music////\\An Album\\//";

    index = 0;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="Users");
    EXPECT_TRUE(index==11);

    index = 8;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="stuart");
    EXPECT_TRUE(index==19);

    index = 19;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="Music");
    EXPECT_TRUE(index==29);
    
    index = 24;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="An Album");
    EXPECT_TRUE(index==40);
    
    index = 38;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName.isEmpty());
    EXPECT_TRUE(index==40);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,nextResourceNameGivenPositiveOffset)
{
    int index = 0;
    QString resourceName,fileName;
    CommonDirectoriesForFilesTest commonDir;
    
    //          0         10        20    
    //          0123456789012345678901234567
    fileName = "/Users/stuart/Music/An Album";

    index = 5;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="s");
    EXPECT_TRUE(index==7);

    index = 9;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="uart");
    EXPECT_TRUE(index==14);

    index = 15;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="usic");
    EXPECT_TRUE(index==20);
    
    index = 22;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName==" Album");
    EXPECT_TRUE(index==28);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,nextResourceNameGivenMultipleResourceWindowsType)
{
    int index = 0;
    QString resourceName,fileName;
    CommonDirectoriesForFilesTest commonDir;
    
    //          0           10         20          30        40         50
    //          012 34567 8901234 567890 123456789 0123456789012 345678901234
    fileName = "C:\\Users\\stuart\\Music\\An Album\\By An Artist\\track01.mp3";

    index = 3;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="Users");
    EXPECT_TRUE(index==9);

    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="stuart");
    EXPECT_TRUE(index==16);

    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="Music");
    EXPECT_TRUE(index==22);

    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="An Album");
    EXPECT_TRUE(index==31);

    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="By An Artist");
    EXPECT_TRUE(index==44);

    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="track01.mp3");
    EXPECT_TRUE(index==55);
    
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName.isEmpty());
    EXPECT_TRUE(index==55);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,nextResourceNameGivenMultipleResourceMacType)
{
    int index = 0;
    QString resourceName,fileName;
    CommonDirectoriesForFilesTest commonDir;
    
    //          0         10        20        30        40        50
    //          01234567890123456789012345678901234567890123456789012
    fileName = "/Users/stuart/Music/An Album/By An Artist/track01.mp3";

    index = 1;
    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="Users");
    EXPECT_TRUE(index==7);

    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="stuart");
    EXPECT_TRUE(index==14);

    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="Music");
    EXPECT_TRUE(index==20);

    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="An Album");
    EXPECT_TRUE(index==29);

    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="By An Artist");
    EXPECT_TRUE(index==42);

    resourceName = commonDir.testNextResourceName(fileName,index);
    EXPECT_TRUE(resourceName=="track01.mp3");
    EXPECT_TRUE(index=53);
}

//-------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenAllFilesAreInTheSameDirectory)
{
    QString fileA = "C:\\Music\\iTunes\\CompA\\track01.mp3";
    QString fileB = "C:\\Music\\iTunes\\Album\\track02.mp3";
    QString fileC = "C:\\Music\\iTunes\\Album\\track03.mp3";
    QString fileE = "C:\\Music\\iTunes\\CompB\\SubFolder\\track03.mp3";
    QString fileF = "C:\\Music\\iTunes\\CompB\\track03.mp3";
    QString fileG = "C:\\Music\\iTunes\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("C:\\Music\\iTunes")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenAllFilesAreInTheSameDirectoryAndNamesAreMixedCase)
{
    QString fileA = "C:\\Music\\iTunes\\CompA\\track01.mp3";
    QString fileB = "C:\\music\\ITUNES\\Album\\track02.mp3";
    QString fileC = "c:\\music\\itunes\\album\\track03.mp3";
    QString fileE = "c:\\MuSiC\\ItUnEs\\COMPB\\SubFolder\\track03.mp3";
    QString fileF = "C:\\Music\\iTunes\\compb\\TRACK03.MP3";
    QString fileG = "c:\\Music\\iTunes\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("c:\\music\\itunes")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenAllFilesAreOnTheSameDriveInSubDirectories)
{
    QString fileA = "C:\\Music\\iTunes\\CompA\\track01.mp3";
    QString fileB = "C:\\Music\\iTunes\\Album\\track02.mp3";
    QString fileC = "C:\\Music\\iTunes\\Album\\track03.mp3";
    QString fileE = "C:\\Development\\Source\\file1.cxx";
    QString fileF = "C:\\Development\\Project\\file2.cxx";
    QString fileG = "C:\\Temp\\track03.mp3";
    QString fileH = "C:\\Temp\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("C:")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenAllFilesAreOnTheSameDriveInSubDirectoriesAndNamesAreMixedCase)
{
    QString fileA = "C:/Music/iTunes/CompA/track01.mp3";
    QString fileB = "C://Music/iTunes/Album/track02.mp3";
    QString fileC = "C:\\\\Music\\iTunes\\Album\\track03.mp3";
    QString fileE = "c:\\Development\\Source\\file1.cxx";
    QString fileF = "C:\\Development\\Project\\file2.cxx";
    QString fileG = "C:\\TEMP\\track03.mp3";
    QString fileH = "c:\\temp\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("c:")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenAllFilesAreOnTheSameDriveInSubDirectoriesWithSomeNamesBeingDirectories)
{
    QString fileA = "C:\\Temp\\Music\\iTunes\\CompA\\track01.mp3";
    QString fileB = "C:\\Temp\\Music\\iTunes\\Album\\track02.mp3";
    QString fileC = "C:\\Temp\\Music\\iTunes\\Album";
    QString fileE = "C:\\Temp\\Development\\Source\\file1.cxx";
    QString fileF = "C:\\Temp\\Development\\Project\\file2.cxx";
    QString fileG = "C:\\Temp\\Development\\Project\\";
    QString fileH = "C:\\Temp\\Development";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("C:\\Temp")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenAllFilesAreOnTheSameDriveInBothSubDirectoriesAndRoot)
{
    QString fileA = "C:\\Music\\iTunes\\CompA\\track01.mp3";
    QString fileB = "C:\\Music\\iTunes\\Album\\track02.mp3";
    QString fileC = "C:\\track03.mp3";
    QString fileE = "C:\\Development\\Source\\file1.cxx";
    QString fileF = "C:\\file2.cxx";
    QString fileG = "C:\\Temp\\track03.mp3";
    QString fileH = "C:\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("C:")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenAllFilesAreOnTheSameDriveInBothSubDirectoriesAndRootAndNamesAreMixedCase)
{
    QString fileA = "C:\\Music\\iTunes\\CompA\\track01.mp3";
    QString fileB = "c:\\\\Music\\\\ITUNES\\\\ALBUM\\\\track02.mp3";
    QString fileC = "C:track03.mp3";
    QString fileE = "c:\\Development\\Source\\file1.cxx";
    QString fileF = "C:\\FILE2.cxx";
    QString fileG = "c:\\Temp\\track03.mp3";
    QString fileH = "c:\\TRack04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("c:")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenFilesAreLocatedOnSeparateDrives)
{
    QString fileA = "C:\\Music\\iTunes\\CompA\\track01.mp3";
    QString fileB = "C:\\Music\\iTunes\\Album\\track02.mp3";
    QString fileC = "C:\\Music\\iTunes\\Album\\track03.mp3";
    QString fileE = "D:\\file1.cxx";
    QString fileF = "D:\\Development\\Project\\file2.cxx";
    QString fileG = "E:\\Temp\\track03.mp3";
    QString fileH = "E:\\Temp\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==3);
    EXPECT_TRUE(dirList.find("C:\\Music\\iTunes")!=dirList.end());
    EXPECT_TRUE(dirList.find("D:")!=dirList.end());
    EXPECT_TRUE(dirList.find("E:\\Temp")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenFilesAreLocatedOnSeparateDrivesAreMixedCase)
{
    QString fileA = "C:/Music/iTunes/CompA/track01.mp3";
    QString fileB = "C://Music/iTunes/Album/track02.mp3";
    QString fileC = "C:\\\\Music\\iTunes\\Album\\track03.mp3";
    QString fileE = "d:\\Development\\Source\\file1.cxx";
    QString fileF = "d:\\Development\\Project\\file2.cxx";
    QString fileG = "E:\\TEMP\\track03.mp3";
    QString fileH = "E:\\temp\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==3);
    EXPECT_TRUE(dirList.find("C:\\Music\\iTunes")!=dirList.end());
    EXPECT_TRUE(dirList.find("d:\\Development")!=dirList.end());
    EXPECT_TRUE(dirList.find("E:\\temp")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenFilesAreLocatedOnNetworkSpecifiedDrives)
{
    QString fileA = "\\\\Oracle\\Music\\iTunes\\CompA\\track01.mp3";
    QString fileB = "\\\\Oracle\\Music\\iTunes\\Album\\track02.mp3";
    QString fileC = "\\\\Oracle\\Music\\iTunes\\Album\\track03.mp3";
    QString fileE = "\\\\Oracle\\Documents\\Development\\Source\\file1.cxx";
    QString fileF = "\\\\Oracle\\Documents\\Development\\Project\\file2.cxx";
    QString fileG = "\\\\Oracle\\Documents\\Temp\\track03.mp3";
    QString fileH = "\\\\Oracle\\Documents\\Temp\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==2);
    EXPECT_TRUE(dirList.find("\\\\Oracle\\Music\\iTunes")!=dirList.end());
    EXPECT_TRUE(dirList.find("\\\\Oracle\\Documents")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenFilesAreLocatedOnNetworkSpecifiedDrivesAndMixedCase)
{
    QString fileA = "\\\\Oracle\\Music\\iTunes\\CompA\\track01.mp3";
    QString fileB = "\\\\ORACLE\\Music\\iTunes\\Album\\track02.mp3";
    QString fileC = "\\\\ORACLE\\Music\\ITunes\\Album\\track03.mp3";
    QString fileE = "\\\\o\\DoCuMeNts Dir\\Development\\Source\\file1.cxx";
    QString fileF = "\\\\O\\DoCuMeNts Dir\\Development\\Project\\file2.cxx";
    QString fileG = "\\\\O\\DoCuMeNts Dir\\Temp Dir\\track03.mp3";
    QString fileH = "\\\\O\\DoCuMeNts Dir\\Temp Dir\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==2);
    EXPECT_TRUE(dirList.find("\\\\oracle\\music\\itunes")!=dirList.end());
    EXPECT_TRUE(dirList.find("\\\\o\\documents dir")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenFilesAreLocatedOnLocalAndNetworkDrives)
{
    QString fileA = "\\\\Oracle\\Music\\iTunes\\CompA\\track01.mp3";
    QString fileB = "C:\\Music\\iTunes\\Album\\track02.mp3";
    QString fileC = "\\\\Oracle\\Music\\iTunes\\Album\\track03.mp3";
    QString fileE = "D:\\Documents\\Development\\Source\\file1.cxx";
    QString fileF = "E:\\Documents\\Development\\Project\\file2.cxx";
    QString fileG = "\\\\Firestorm\\Documents\\Temp\\track03.mp3";
    QString fileH = "\\\\Oracle\\Documents\\Temp\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==6);
    EXPECT_TRUE(dirList.find("\\\\Oracle\\Music\\iTunes")!=dirList.end());
    EXPECT_TRUE(dirList.find("C:\\Music\\iTunes\\Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("D:\\Documents\\Development\\Source")!=dirList.end());
    EXPECT_TRUE(dirList.find("E:\\Documents\\Development\\Project")!=dirList.end());
    EXPECT_TRUE(dirList.find("\\\\Firestorm\\Documents\\Temp")!=dirList.end());
    EXPECT_TRUE(dirList.find("\\\\Oracle\\Documents\\Temp")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenFilesAreLocatedOnLocalAndNetworkDrivesAndMixedCase)
{
    QString fileA = "\\\\ORACLE\\MUSIC\\iTunes\\CompA\\track01.mp3";
    QString fileB = "C:\\\\Music/iTunes/Album\\track02.mp3";
    QString fileC = "\\\\Oracle\\Music\\iTunes\\Album\\track03.mp3";
    QString fileE = "D:///Documents\\Development Source\\file1.cxx";
    QString fileF = "e:documents/development/project/file2.cxx";
    QString fileG = "//Firestorm/////Documents\\Temp\\track03.mp3";
    QString fileH = "//192.168.5.9////Documents\\Temp\\track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==6);
    EXPECT_TRUE(dirList.find("\\\\oracle\\music\\iTunes")!=dirList.end());
    EXPECT_TRUE(dirList.find("C:\\Music\\iTunes\\Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("D:\\Documents\\Development Source")!=dirList.end());
    EXPECT_TRUE(dirList.find("e:\\documents\\development\\project")!=dirList.end());
    EXPECT_TRUE(dirList.find("\\\\Firestorm\\Documents\\Temp")!=dirList.end());
    EXPECT_TRUE(dirList.find("\\\\192.168.5.9\\Documents\\Temp")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenRelativePathNames)
{
    QString fileA = "track01.mp3";
    QString fileB = ".\\track02.mp3";
    QString fileC = "..\\..\\..\\iTunes\\Album\\track03.mp3";
    QString fileE = ".\\Development\\..\\..\\Source\\file1.cxx";
    QString fileF = "E:\\Documents\\Development\\..\\project\\file2.cxx";
    QString fileG = "E:\\Documents\\.\\iTunes\\..\\Development\\..\\project\\file2.cxx";
    
    QStringList fileList;
    //fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    fileList << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("E:\\Documents\\project")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenRelativePathNamesAndMixedCase)
{
    QString fileA = "track01.mp3";
    QString fileB = ".\\track02.mp3";
    QString fileC = "..\\..\\..\\iTunes\\Album\\track03.mp3";
    QString fileE = ".\\Development\\..\\..\\Source\\file1.cxx";
    QString fileF = "E:/Documents/development/../Project In\\file2.cxx";
    QString fileG = "e:\\Documents\\.\\iTunes\\..\\Development\\..\\PROJECT In\\file2.cxx";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("e:\\Documents\\project in")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenInvalidPathNames)
{
    QString fileA = "";
    QString fileB = "a";
    QString fileC = "A sentence not a path";
    QString fileE = "NN:\\Documents\\file.cxx";
    QString fileF = "\\Oracle\\Documents\\No Folder";
    QString fileH = "/usr/bin/local";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileH;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onWindowsGivenEmptyList)
{
    QStringList fileList;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==0);
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreInTheSameDirectoryWhichIsChildOfRootWithSomeNamesBeingDirectories)
{
    QString fileA = "/Users/bonez/Music/iTunes/Album/track01.mp3";
    QString fileB = "/Users/bonez/Music/iTunes/Album/";
    QString fileC = "/Users/bonez/Music/iTunes";
    QString fileE = "/Users/bonez/Music/";
    QString fileF = "/Users/bonez/Music/iTunes/CompB/track03.mp3";
    QString fileG = "/Users/bonez/Music/iTunes/track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("/Users/bonez/Music")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreInTheSameDirectoryWhichIsChildOfRootWithSomeNamesBeingDirectoriesNamedSpaces)
{
    QString fileA = "/Users/bonez/Music Storage/iTunes Media/Album/track01.mp3";
    QString fileB = "/Users/bonez/Music Storage/iTunes Media/Album/";
    QString fileF = "/Users/bonez/Music Storage/iTunes Media/CompB/track03.mp3";
    QString fileG = "/Users/bonez/Music Storage/iTunes Media/Compiation Collection/";
    
    QStringList fileList;
    fileList << fileA << fileB << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("/Users/bonez/Music Storage/iTunes Media")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreInTheSameDirectoryWhichIsChildOfRoot)
{
    QString fileA = "/Users/bonez/Music/iTunes/Album/track01.mp3";
    QString fileB = "/Users/bonez/Music/iTunes/Album/track02.mp3";
    QString fileC = "/Users/bonez/Music/iTunes/Album/track03.mp3";
    QString fileE = "/Users/bonez/Music/iTunes/CompB/SubFolder/track03.mp3";
    QString fileF = "/Users/bonez/Music/iTunes/CompB/track03.mp3";
    QString fileG = "/Users/bonez/Music/iTunes/track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("/Users/bonez/Music/iTunes")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacAllFilesAreInDifferentRootDirectories)
{
    QString fileA = "/Users/bonez/Music/iTunes/Album/track01.mp3";
    QString fileB = "/Users/bonez/Music/iTunes/Album/track02.mp3";
    QString fileC = "/Users/git/Music/iTunes/Album/track03.mp3";
    QString fileE = "/Users/git/Music/iTunes/Albums/SubFolder/track03.mp3";
    QString fileF = "/usr/local/bin/qmake";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==3);
    EXPECT_TRUE(dirList.find("/Users/bonez/Music/iTunes/Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Users/git/Music/iTunes")!=dirList.end());
    EXPECT_TRUE(dirList.find("/usr/local/bin")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreInTheSameDirectoryWhichIsChildOfRootAndNamesAreMixedCase)
{
    QString fileA = "/Users/bonez/Music/iTunes/Album/track01.mp3";
    QString fileB = "///Users/bonez/Music/iTunes/Album/track02.mp3";
    QString fileC = "\\Users\\bonez\\Music\\iTunes\\Album\\track03.mp3";
    QString fileE = "/users/bonez/Music/ITUNES/CompB/SubFolder/track03.mp3";
    QString fileF = "/Users/Bonez/MUSIC/ITUNES/track03.mp3";
    QString fileG = "/Users/Bonez/MUSIC/iTunes/track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==3);
    EXPECT_TRUE(dirList.find("/Users/bonez/Music/iTunes/Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("/users/bonez/Music/ITUNES/CompB/SubFolder")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Users/Bonez/MUSIC")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreInTheSameDirectoryWhichIsChildOfAMountedVolume)
{
    QString fileA = "/Volumes/LaCie/Music/iTunes/Album/track01.mp3";
    QString fileB = "/Volumes/LaCie/Music/iTunes/Album/track02.mp3";
    QString fileC = "/Volumes/LaCie/Music/iTunes/Album/track03.mp3";
    QString fileE = "/Volumes/LaCie/Music/iTunes/CompB/SubFolder/track03.mp3";
    QString fileF = "/Volumes/LaCie/Music/iTunes/CompB/track03.mp3";
    QString fileG = "/Volumes/LaCie/Music/iTunes/track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("/Volumes/LaCie/Music/iTunes")!=dirList.end());    
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreInTheSameDirectoryIsChildOfAMountedVolumeAndNamesAreMixedCase)
{
    QString fileA = "/Volumes/LaCie/Music Storage Location/iTunes Media/Album/track01.mp3";
    QString fileB = "/Volumes/LaCie/Music Storage Location/iTunes Media/Album/track02.mp3";
    QString fileC = "/Volumes/LaCie/Music Storage Location/iTunes Media/Album/track03.mp3";
    QString fileE = "/Volumes/LaCie/Music Storage Location/iTunes Media/AlBuM/SubFolder/track03.mp3";
    QString fileF = "/Volumes/LaCie/Music Storage Location/iTunes Media/album/track03.mp3";
    QString fileG = "/Volumes/LaCie/Music Storage Location/iTunes Media/album/track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("/Volumes/LaCie/Music Storage Location/iTunes Media")!=dirList.end());    
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreOnTheSameVolume)
{
    QString fileA = "/Volumes/LaCie/Music/iTunes Media/Album/track01.mp3";
    QString fileB = "/Volumes/LaCie/Music/iTunes Media/Album/track02.mp3";
    QString fileC = "/Volumes/LaCie/Music Storage Location/iTunes Media/Album/track03.mp3";
    QString fileE = "/Volumes/LaCie/Music Storage Location/iTunes Media/AlBuM/SubFolder/track03.mp3";
    QString fileF = "/Volumes/LaCie/track03.mp3";
    QString fileG = "/Volumes/LaCie/track04.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("/Volumes/LaCie")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreOnTheSameVolumeAndNamesAreMixedCase)
{
    QString fileA = "/Volumes/LaCie/Music/iTunes Media/album tracks/track01.mp3";
    QString fileB = "/Volumes/LaCie/Music/iTunes Media/Album Tracks/track02.mp3";
    QString fileC = "/Volumes/LaCie/Music Storage Location/iTunes Media/Album/track03.mp3";
    QString fileE = "/Volumes/LaCie/music storage location/iTunes Media/AlBuM/SubFolder/";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("/Volumes/LaCie")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreOnDifferentVolumes)
{
    QString fileA = "/Volumes/LaCie/Music/iTunes Media/Album/track01.mp3";
    QString fileB = "/Volumes/LaCie/Music/iTunes Media/Album/track02.mp3";
    QString fileC = "/Volumes/LaCie/Music/iTunes Media/Album/track03.mp3";
    QString fileD = "/Volumes/LaCie/Music/Albums/BandA/track03.mp3";
    QString fileE = "/Volumes/Black Omega/Music/iTunes Media/Album/track01.mp3";
    QString fileF = "/Volumes/Black Omega/Music/iTunes Media/Album/track02.mp3";
    QString fileG = "/Volumes/LaCie 1/Music/iTunes Media/Album/track03.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileD << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==3);
    EXPECT_TRUE(dirList.find("/Volumes/LaCie/Music")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Volumes/Black Omega/Music/iTunes Media/Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Volumes/LaCie 1/Music/iTunes Media/Album")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreOnDifferentVolumesAndNamesAreMixedCase)
{
    QString fileA = "/Volumes/LACIE/Music/iTunes Media/Album/track01.mp3";
    QString fileB = "/Volumes/LaCie/MUSIC/iTunes Media/Album/TRACK02";
    QString fileC = "/Volumes\\LaCie\\MUSIC\\iTunes Media\\Album\\track02/file";
    QString fileD = "\\\\Volumes\\LaCie/MUSIC/Albums/BandA/track03.mp3";
    QString fileE = "/Volumes/Black Omega/Music/iTunes Media/Album/track01.mp3";
    QString fileF = "/Volumes/Black Omega/Music/iTunes Media/Album/track02.mp3";
    QString fileG = "/Volumes/BLACK OMEGA/Music/iTunes Media/Album/track03.mp3/";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileD << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==4);
    EXPECT_TRUE(dirList.find("/Volumes/LACIE/Music/iTunes Media/Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Volumes/LaCie/MUSIC")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Volumes/Black Omega/Music/iTunes Media/Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Volumes/BLACK OMEGA/Music/iTunes Media/Album/track03.mp3")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreOnDifferentVolumesOrChildOfRoot)
{
    QString fileA = "/Users/bonez/Music/iTunes/Album/track01.mp3";
    QString fileB = "/Volumes/LaCie/Music/iTunes Media/Album/track01.mp3";
    QString fileC = "/Users/bonez/Music/iTunes/Album/";
    QString fileD = "/Volumes/LaCie/Music/iTunes Media/Album/track02.mp3";
    QString fileE = "/Users/bonez/Music/iTunes/Album/CompB/track03.mp3";
    QString fileF = "/Volumes/LaCie/Music/iTunes Media/Album/track03.mp3";
    QString fileG = "/Users/bonez/Music/iTunes/track04.mp3";
    QString fileH = "/Volumes/LaCie/Music/Albums/BandA/track03.mp3";
    QString fileI = "/Volumes/Black Omega/Music/iTunes Media/Album/track01.mp3";
    QString fileJ = "/Volumes/Black Omega/Music/iTunes Media/Album/track02.mp3";
    QString fileK = "/Volumes/LaCie 1/Music/iTunes Media/Album/track03.mp3";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileD << fileE << fileF << fileG;
    fileList << fileH << fileI << fileJ << fileK;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==4);
    EXPECT_TRUE(dirList.find("/Volumes/LaCie/Music")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Volumes/Black Omega/Music/iTunes Media/Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Users/bonez/Music/iTunes")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Volumes/LaCie 1/Music/iTunes Media/Album")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreOnDifferentVolumesOrChildOfRootAndNamesAreMixedCase)
{
    QString fileA = "/Volumes/Black Omega/Music/iTunes Media/Album/track02.mp3";
    QString fileB = "/volumes/Black Omega/Music/iTunes Media/Album/track02.mp3";
    QString fileC = "/Users/bonez/Music/iTunes/Album/track02.mp3";
    QString fileD = "/Users/git/Music/iTunes/Album/track03.mp3";

    QStringList fileList;
    fileList << fileA << fileB << fileC << fileD;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==4);
    EXPECT_TRUE(dirList.find("/Volumes/Black Omega/Music/iTunes Media/Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("/volumes/Black Omega/Music/iTunes Media/Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Users/bonez/Music/iTunes/Album")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Users/git/Music/iTunes/Album")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenSomeFilesAreLocatedInRootDirectory)
{
    QString fileA = "/file.txt";
    QString fileB = "/var/logs";
    QString fileC = "/var/log.txt";
    QString fileD = "/usr/local/bin/";
    QString fileE = "/usr/bin/";

    QStringList fileList;
    fileList << fileA << fileB << fileC << fileD << fileE;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==2);
    EXPECT_TRUE(dirList.find("/var")!=dirList.end());
    EXPECT_TRUE(dirList.find("/usr")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenAllFilesAreLocatedInRootDirectory)
{
    QString fileA = "/file.txt";
    QString fileB = "/vars";
    QString fileD = "/usr";

    QStringList fileList;
    fileList << fileA << fileB << fileD;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenRelativePathNames)
{
    QString fileA = "track01.mp3";
    QString fileB = "./track02.mp3";
    QString fileC = "../../../iTunes/Album/track03.mp3";
    QString fileE = "./Development/../../Source/file1.cxx";
    QString fileF = "/Users/bonez/Documents/Development/../project/file2.cxx";
    QString fileG = "/Users/bonez/Documents/./iTunes/../Development/../project/file2.cxx";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileF << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==1);
    EXPECT_TRUE(dirList.find("/Users/bonez/Documents/project")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenRelativePathNamesAndNamesAreMixedCase)
{
    QString fileA = "/Users/bonez/Documents/development/../project/file2.cxx";
    QString fileB = "/Users/bonez/Documents/./iTunes/../Development/../project/file2.cxx";
    QString fileC = "/Users/git/Documents/development/../project/file2.cxx";
    QString fileD = "/Users/git/Documents/./iTunes/../Development/../Project/file2.cxx";
        
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileD;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==2);
    EXPECT_TRUE(dirList.find("/Users/bonez/Documents/project")!=dirList.end());
    EXPECT_TRUE(dirList.find("/Users/git/Documents")!=dirList.end());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenInvalidPathNames)
{
    QString fileA = "";
    QString fileB = "a";
    QString fileC = "A sentence not a path";
    QString fileE = "NN:\\Documents\\file.cxx";
    QString fileG = "e::\\Documents\\.\\iTunes\\..\\Development\\..\\PROJECT\\file2.cxx";
    
    QStringList fileList;
    fileList << fileA << fileB << fileC << fileE << fileG;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,onMacGivenEmptyList)
{
    QStringList fileList;
    
    QSet<QString> dirList;
    dirList = common::CommonDirectoriesForFiles::find(fileList);
    
    EXPECT_TRUE(dirList.size()==0);
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathWhenFileNameIsEmpty)
{
    QString path;
    QString fileName = "";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInRootOfLocalDrive)
{
    QString path;
    QString fileName = "C:\\file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="C:\\file.txt");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="C:");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInRootOfNetworkDrive)
{
    QString path;
    QString fileName = "\\\\Oracle\\Music\\file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\file.txt");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="\\\\Oracle\\Music");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInRootOfLocalDriveAndIsRelativeValid)
{
    QString path;
    QString fileName = "C:\\Music\\Album\\..\\..\\file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="C:\\file.txt");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="C:");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInRootOfLocalDriveAndIsRelativeInvalid)
{
    QString path;
    QString fileName = "C:\\Music\\..\\..\\file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInRootOfNetworkDriveAndIsRelative)
{
    QString path;
    QString fileName = "\\\\Oracle\\Music\\Album\\Collection\\..\\..\\file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\file.txt");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="\\\\Oracle\\Music");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInRootOfNetworkDriveAndIsRelativeInvalid)
{
    QString path;
    QString fileName = "\\\\Oracle\\Music\\Album\\..\\..\\file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInSubDirectoryOfLocalDrive)
{
    QString path;
    QString fileName = "C:\\Music\\Album\\Of A Group\\I\\Indie\\Band\\track.mp3";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="C:\\Music\\Album\\Of A Group\\I\\Indie\\Band\\track.mp3");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="C:\\Music\\Album\\Of A Group\\I\\Indie\\Band");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path=="C:\\Music\\Album\\Of A Group\\I\\Indie");
    path = commonDir.path(fileName,3);
    EXPECT_TRUE(path=="C:\\Music\\Album\\Of A Group\\I");
    path = commonDir.path(fileName,4);
    EXPECT_TRUE(path=="C:\\Music\\Album\\Of A Group");
    path = commonDir.path(fileName,5);
    EXPECT_TRUE(path=="C:\\Music\\Album");
    path = commonDir.path(fileName,6);
    EXPECT_TRUE(path=="C:\\Music");
    path = commonDir.path(fileName,7);
    EXPECT_TRUE(path=="C:");
    path = commonDir.path(fileName,8);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInSubDirectoryOfNetworkDrive)
{
    QString path;
    QString fileName = "\\\\Oracle\\Music\\Music\\Album\\Of A Group\\I\\Indie\\Band\\track.mp3";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album\\Of A Group\\I\\Indie\\Band\\track.mp3");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album\\Of A Group\\I\\Indie\\Band");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album\\Of A Group\\I\\Indie");
    path = commonDir.path(fileName,3);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album\\Of A Group\\I");
    path = commonDir.path(fileName,4);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album\\Of A Group");
    path = commonDir.path(fileName,5);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album");
    path = commonDir.path(fileName,6);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music");
    path = commonDir.path(fileName,7);
    EXPECT_TRUE(path=="\\\\Oracle\\Music");
    path = commonDir.path(fileName,8);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInSubDirectoryOfLocalDriveAndIsRelativeValid)
{
    QString path;
    QString fileName = "C:\\Music\\Album\\iTunes\\P\\..\\Queen\\..\\..\\Of A Group\\I\\Indie\\Band\\track.mp3";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="C:\\Music\\Album\\Of A Group\\I\\Indie\\Band\\track.mp3");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="C:\\Music\\Album\\Of A Group\\I\\Indie\\Band");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path=="C:\\Music\\Album\\Of A Group\\I\\Indie");
    path = commonDir.path(fileName,3);
    EXPECT_TRUE(path=="C:\\Music\\Album\\Of A Group\\I");
    path = commonDir.path(fileName,4);
    EXPECT_TRUE(path=="C:\\Music\\Album\\Of A Group");
    path = commonDir.path(fileName,5);
    EXPECT_TRUE(path=="C:\\Music\\Album");
    path = commonDir.path(fileName,6);
    EXPECT_TRUE(path=="C:\\Music");
    path = commonDir.path(fileName,7);
    EXPECT_TRUE(path=="C:");
    path = commonDir.path(fileName,8);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInSubDirectoryOfNetworkDriveAndIsRelativeValid)
{
    QString path;
    QString fileName = "\\\\Oracle\\Music\\Music\\Album\\iTunes\\P\\..\\Queen\\..\\..\\Of A Group\\I\\Indie\\Band\\track.mp3";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album\\Of A Group\\I\\Indie\\Band\\track.mp3");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album\\Of A Group\\I\\Indie\\Band");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album\\Of A Group\\I\\Indie");
    path = commonDir.path(fileName,3);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album\\Of A Group\\I");
    path = commonDir.path(fileName,4);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album\\Of A Group");
    path = commonDir.path(fileName,5);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music\\Album");
    path = commonDir.path(fileName,6);
    EXPECT_TRUE(path=="\\\\Oracle\\Music\\Music");
    path = commonDir.path(fileName,7);
    EXPECT_TRUE(path=="\\\\Oracle\\Music");
    path = commonDir.path(fileName,8);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInSubDirectoryOfLocalDriveAndIsRelativeInvalid)
{
    QString path;
    QString fileName = "C:\\Music\\..\\Album\\..\\iTunes\\P\\..\\Queen\\..\\..\\..\\I\\track.mp3";
    CommonDirectoriesForFiles commonDir;

    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnWindowsForFileInSubDirectoryOfNetworkDriveAndIsRelativeInvalid)
{
    QString path;
    QString fileName = "\\\\Firestorm\\Share\\Music\\..\\Album\\..\\iTunes\\P\\..\\Queen\\..\\..\\..\\I\\track.mp3";
    CommonDirectoriesForFiles commonDir;

    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_MACOSX)
//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInRootDirectoryOfMain)
{
    QString path;
    QString fileName = "/usr/file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/usr/file.txt");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/usr");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInRootDirectoryOfUsers)
{
    QString path;
    QString fileName = "/Users/bonez/file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/Users/bonez/file.txt");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/Users/bonez");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path.isEmpty());    
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInRootDirectoryOfVolumes)
{
    QString path;
    QString fileName = "/Volumes/LaCie/file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/Volumes/LaCie/file.txt");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/Volumes/LaCie");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInRootDirectoryOfMainIsRelativeValid)
{
    QString path;
    QString fileName = "/usr/local/music/../album/../../file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/usr/file.txt");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/usr");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInRootDirectoryOfUsersIsRelativeValid)
{
    QString path;
    QString fileName = "/Users/bonez/local/music/../album/../../file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/Users/bonez/file.txt");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/Users/bonez");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path.isEmpty());    
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInRootDirectoryOfVolumesIsRelativeValid)
{
    QString path;
    QString fileName = "/Volumes/LaCie/local/music/../album/../../file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/Volumes/LaCie/file.txt");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/Volumes/LaCie");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInRootDirectoryOfMainIsRelativeInvalid)
{
    QString path;
    QString fileName = "/usr/local/music/../../album/../../file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInRootDirectoryOfUsersIsRelativeInvalid)
{
    QString path;
    QString fileName = "/Users/bonez/local/music/../../album/../../file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInRootDirectoryOfVolumesIsRelativeInvalid)
{
    QString path;
    QString fileName = "/Volumes/LaCie/local/music/../../album/../../file.txt";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInSubDirectoryOfMain)
{
    QString path;
    QString fileName = "/usr/Music/Album/Of A Group/I/Indie/Band/track.mp3";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/usr/Music/Album/Of A Group/I/Indie/Band/track.mp3");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/usr/Music/Album/Of A Group/I/Indie/Band");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path=="/usr/Music/Album/Of A Group/I/Indie");
    path = commonDir.path(fileName,3);
    EXPECT_TRUE(path=="/usr/Music/Album/Of A Group/I");
    path = commonDir.path(fileName,4);
    EXPECT_TRUE(path=="/usr/Music/Album/Of A Group");
    path = commonDir.path(fileName,5);
    EXPECT_TRUE(path=="/usr/Music/Album");
    path = commonDir.path(fileName,6);
    EXPECT_TRUE(path=="/usr/Music");
    path = commonDir.path(fileName,7);
    EXPECT_TRUE(path=="/usr");
    path = commonDir.path(fileName,8);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInSubDirectoryOfUsers)
{
    QString path;
    QString fileName = "/Users/bonez/Music/Album/Of A Group/I/Indie/Band/track.mp3";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album/Of A Group/I/Indie/Band/track.mp3");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album/Of A Group/I/Indie/Band");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album/Of A Group/I/Indie");
    path = commonDir.path(fileName,3);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album/Of A Group/I");
    path = commonDir.path(fileName,4);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album/Of A Group");
    path = commonDir.path(fileName,5);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album");
    path = commonDir.path(fileName,6);
    EXPECT_TRUE(path=="/Users/bonez/Music");
    path = commonDir.path(fileName,7);
    EXPECT_TRUE(path=="/Users/bonez");
    path = commonDir.path(fileName,8);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInSubDirectoryOfVolumes)
{
    QString path;
    QString fileName = "/Volumes/LaCie/Music/Album/Of A Group/I/Indie/Band/track.mp3";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album/Of A Group/I/Indie/Band/track.mp3");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album/Of A Group/I/Indie/Band");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album/Of A Group/I/Indie");
    path = commonDir.path(fileName,3);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album/Of A Group/I");
    path = commonDir.path(fileName,4);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album/Of A Group");
    path = commonDir.path(fileName,5);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album");
    path = commonDir.path(fileName,6);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music");
    path = commonDir.path(fileName,7);
    EXPECT_TRUE(path=="/Volumes/LaCie");
    path = commonDir.path(fileName,8);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInSubDirectoryOfMainIsRelativeValid)
{
    QString path;
    QString fileName = "/usr/Music/Album/iTunes/P/../Queen/../../Of A Group/I/Indie/Band/track.mp3";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/usr/Music/Album/Of A Group/I/Indie/Band/track.mp3");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/usr/Music/Album/Of A Group/I/Indie/Band");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path=="/usr/Music/Album/Of A Group/I/Indie");
    path = commonDir.path(fileName,3);
    EXPECT_TRUE(path=="/usr/Music/Album/Of A Group/I");
    path = commonDir.path(fileName,4);
    EXPECT_TRUE(path=="/usr/Music/Album/Of A Group");
    path = commonDir.path(fileName,5);
    EXPECT_TRUE(path=="/usr/Music/Album");
    path = commonDir.path(fileName,6);
    EXPECT_TRUE(path=="/usr/Music");
    path = commonDir.path(fileName,7);
    EXPECT_TRUE(path=="/usr");
    path = commonDir.path(fileName,8);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInSubDirectoryOfUsersIsRelativeValid)
{
    QString path;
    QString fileName = "/Users/bonez/Music/Album/iTunes/P/../Queen/../../Of A Group/I/Indie/Band/track.mp3";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album/Of A Group/I/Indie/Band/track.mp3");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album/Of A Group/I/Indie/Band");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album/Of A Group/I/Indie");
    path = commonDir.path(fileName,3);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album/Of A Group/I");
    path = commonDir.path(fileName,4);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album/Of A Group");
    path = commonDir.path(fileName,5);
    EXPECT_TRUE(path=="/Users/bonez/Music/Album");
    path = commonDir.path(fileName,6);
    EXPECT_TRUE(path=="/Users/bonez/Music");
    path = commonDir.path(fileName,7);
    EXPECT_TRUE(path=="/Users/bonez");
    path = commonDir.path(fileName,8);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInSubDirectoryOfVolumesIsRelativeValid)
{
    QString path;
    QString fileName = "/Volumes/LaCie/Music/Album/iTunes/P/../Queen/../../Of A Group/I/Indie/Band/track.mp3";
    CommonDirectoriesForFiles commonDir;
    
    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album/Of A Group/I/Indie/Band/track.mp3");
    path = commonDir.path(fileName,1);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album/Of A Group/I/Indie/Band");
    path = commonDir.path(fileName,2);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album/Of A Group/I/Indie");
    path = commonDir.path(fileName,3);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album/Of A Group/I");
    path = commonDir.path(fileName,4);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album/Of A Group");
    path = commonDir.path(fileName,5);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music/Album");
    path = commonDir.path(fileName,6);
    EXPECT_TRUE(path=="/Volumes/LaCie/Music");
    path = commonDir.path(fileName,7);
    EXPECT_TRUE(path=="/Volumes/LaCie");
    path = commonDir.path(fileName,8);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInSubDirectoryOfMainIsRelativeInvalid)
{
    QString path;
    QString fileName = "/usr/Music/../Album/../iTunes/P/../Queen/../../../../I/track.mp3";
    CommonDirectoriesForFiles commonDir;

    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInSubDirectoryOfUsersIsRelativeInvalid)
{
    QString path;
    QString fileName = "/Users/bonez/Music/../Album/../iTunes/P/../Queen/../../../I/track.mp3";
    CommonDirectoriesForFiles commonDir;

    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CommonDirectoriesForFiles,pathOnMacForFileInSubDirectoryOfVolumesIsRelativeInvalid)
{
    QString path;
    QString fileName = "/Volumes/LaCie/Music/../Album/../iTunes/P/../Queen/../../../I/track.mp3";
    CommonDirectoriesForFiles commonDir;

    path = commonDir.path(fileName,0);
    EXPECT_TRUE(path.isEmpty());
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
