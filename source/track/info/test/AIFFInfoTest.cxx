#include "track/info/test/AIFFInfoTest.h"
#include <QApplication>

//-------------------------------------------------------------------------------------------

void AIFFInfoQtUnitTest::readInfo()
{
    QString fileName = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"sample2.aiff");

    common::BIOStream ioFile(common::e_BIOStream_FileRead);
    ASSERT_TRUE(ioFile.open(fileName));

    InfoSPtr pInfo = Info::readInfo(&ioFile);
    ASSERT_FALSE(pInfo.isNull());

    EXPECT_TRUE(pInfo->artist()=="James Horner");
    EXPECT_TRUE(pInfo->title()=="Gathering All the Na'vi Clans for Battle");
    EXPECT_TRUE(pInfo->album()=="Avatar");
    EXPECT_TRUE(pInfo->comment()=="Encoded by Audacity");
    EXPECT_TRUE(pInfo->genre()=="Soundtrack");
    EXPECT_TRUE(pInfo->track()=="12");

    EXPECT_EQ(pInfo->hashID(), 197876668749787937);
}

//-------------------------------------------------------------------------------------------

AIFFInfoQtUnitApplication::AIFFInfoQtUnitApplication(int argc,char **argv) : QCoreApplication(argc,argv),
    m_succeeded(false)
{
    QTimer::singleShot(10,this,SLOT(runTests()));
}

//-------------------------------------------------------------------------------------------

AIFFInfoQtUnitApplication::~AIFFInfoQtUnitApplication()
{}

//-------------------------------------------------------------------------------------------

bool AIFFInfoQtUnitApplication::testSucceeded() const
{
    return m_succeeded;
}

//-------------------------------------------------------------------------------------------

void AIFFInfoQtUnitApplication::runTests()
{
    AIFFInfoQtUnitTest tests;
    m_succeeded = (QTest::qExec(&tests)==0) ? true : false;
    quit();
}

//-------------------------------------------------------------------------------------------

TEST(AIFFInfo,readInfo)
{
    QStringList libPaths = QApplication::libraryPaths();
    {
        char *argv = 0;
        AIFFInfoQtUnitApplication tester(0,&argv);
        tester.exec();
        ASSERT_TRUE(tester.testSucceeded());
    }
    QApplication::setLibraryPaths(libPaths);
}

//-------------------------------------------------------------------------------------------

