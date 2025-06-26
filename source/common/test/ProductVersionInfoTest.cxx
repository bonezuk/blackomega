#include "gtest/gtest.h"

#include <QDir>

#include "common/inc/ProductVersionInfo.h"
#include "common/inc/DiskOps.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega::common;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(ProductVersionInfo,read)
{
    ProductVersionInfo versionInfo;
    EXPECT_TRUE(versionInfo.read(":/build/Resources/buildInfo.xml"));
    EXPECT_TRUE(versionInfo.versionMajor()==2);
    EXPECT_TRUE(versionInfo.versionMinor()==1);
    EXPECT_TRUE(versionInfo.versionRevision()==2);
    EXPECT_TRUE(versionInfo.build()==34);
    EXPECT_TRUE(versionInfo.version()=="2.1.2");
    EXPECT_TRUE(versionInfo.copyright()=="Black Omega 2 by Stuart A MacLean (c) 2014");
}

//-------------------------------------------------------------------------------------------

TEST(ProductVersionInfo,write)
{
    QString tempFileName = track::model::TrackDBTestEnviroment::instance()->getTempDirectory();
    if(tempFileName.at(tempFileName.length()-1)!=QChar('/') && tempFileName.at(tempFileName.length()-1)!=QChar('\\'))
    {
        tempFileName += "/";
    }
    tempFileName += "ProductVersionInfo_write.xml";
    tempFileName = QDir::toNativeSeparators(tempFileName);

    ProductVersionInfo inInfo;
    EXPECT_TRUE(inInfo.read(":/build/Resources/buildInfo.xml"));
    EXPECT_TRUE(inInfo.write(tempFileName));

    ProductVersionInfo versionInfo;
    EXPECT_TRUE(versionInfo.read(tempFileName));
    EXPECT_TRUE(versionInfo.versionMajor()==2);
    EXPECT_TRUE(versionInfo.versionMinor()==1);
    EXPECT_TRUE(versionInfo.versionRevision()==2);
    EXPECT_TRUE(versionInfo.build()==34);
    EXPECT_TRUE(versionInfo.version()=="2.1.2");
    EXPECT_TRUE(versionInfo.copyright()=="Black Omega 2 by Stuart A MacLean (c) 2014");

    DiskOps::remove(tempFileName);
}

//-------------------------------------------------------------------------------------------
