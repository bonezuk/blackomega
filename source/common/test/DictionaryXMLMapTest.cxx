#include "gtest/gtest.h"

#include "common/inc/DictionaryXMLMap.h"
#include "common/inc/DiskOps.h"
#include "track/model/test/TrackDBTestEnviroment.h"

#include <QDir>

using namespace omega::common;

//-------------------------------------------------------------------------------------------

TEST(DictionaryXMLMap,read)
{
    QMap<QString,QString> dictMap = DictionaryXMLMap::read(":/build/Resources/buildInfo.xml");
    
    EXPECT_TRUE(dictMap.size()==5);
    EXPECT_TRUE(dictMap.find("VersionMajor").value()=="2");
    EXPECT_TRUE(dictMap.find("VersionMinor").value()=="1");
    EXPECT_TRUE(dictMap.find("Revision").value()=="2");
    EXPECT_TRUE(dictMap.find("Build").value()=="34");
    EXPECT_TRUE(dictMap.find("Copyright").value()=="Black Omega 2 by Stuart A MacLean (c) 2014");
}

//-------------------------------------------------------------------------------------------

TEST(DictionaryXMLMap,write)
{
    QString tempFileName = omega::track::model::TrackDBTestEnviroment::instance()->getTempDirectory();
    if(tempFileName.at(tempFileName.length()-1)!=QChar('/') && tempFileName.at(tempFileName.length()-1)!=QChar('\\'))
    {
        tempFileName += "/";
    }
    tempFileName += "DictionaryXMLMap_write.xml";
    tempFileName = QDir::toNativeSeparators(tempFileName);
    
    QMap<QString,QString> loadMap = DictionaryXMLMap::read(":/build/Resources/buildInfo.xml");
    EXPECT_TRUE(DictionaryXMLMap::write(tempFileName,loadMap));
    
    QMap<QString,QString> dictMap = DictionaryXMLMap::read(tempFileName);

    EXPECT_TRUE(dictMap.size()==5);
    EXPECT_TRUE(dictMap.find("VersionMajor").value()=="2");
    EXPECT_TRUE(dictMap.find("VersionMinor").value()=="1");
    EXPECT_TRUE(dictMap.find("Revision").value()=="2");
    EXPECT_TRUE(dictMap.find("Build").value()=="34");
    EXPECT_TRUE(dictMap.find("Copyright").value()=="Black Omega 2 by Stuart A MacLean (c) 2014");
    
    DiskOps::remove(tempFileName);
}

//-------------------------------------------------------------------------------------------
