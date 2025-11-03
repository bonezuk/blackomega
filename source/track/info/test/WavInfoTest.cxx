#include "track/info/test/WavInfoTest.h"
#include <QApplication>

//-------------------------------------------------------------------------------------------

void WavInfoQtUnitTest::readAndTestSample()
{
    QString fileName = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"info1.wav");

    common::BIOStream ioFile(common::e_BIOStream_FileRead);
    ASSERT_TRUE(ioFile.open(fileName));

    InfoSPtr pInfo = Info::readInfo(&ioFile);
    ASSERT_FALSE(pInfo.isNull());

    EXPECT_TRUE(pInfo->artist()=="Hans Zimmer");
    EXPECT_TRUE(pInfo->title()=="A Way Of Life");
    EXPECT_TRUE(pInfo->album()=="The Last Samuria");
    EXPECT_TRUE(pInfo->comment()=="Sample of Comments");
    EXPECT_TRUE(pInfo->genre()=="Soundtrack");
    EXPECT_TRUE(pInfo->track()=="1");

    EXPECT_EQ(pInfo->hashID(), 125719584407950382); // TODO : get ID
}

//-------------------------------------------------------------------------------------------

WavInfoQtUnitApplication::WavInfoQtUnitApplication(int argc,char **argv) : QCoreApplication(argc,argv),
    m_succeeded(false)
{
    QTimer::singleShot(10,this,SLOT(runTests()));
}

//-------------------------------------------------------------------------------------------

WavInfoQtUnitApplication::~WavInfoQtUnitApplication()
{}

//-------------------------------------------------------------------------------------------

bool WavInfoQtUnitApplication::testSucceeded() const
{
    return m_succeeded;
}

//-------------------------------------------------------------------------------------------

void WavInfoQtUnitApplication::runTests()
{
    WavInfoQtUnitTest tests;
    m_succeeded = (QTest::qExec(&tests)==0) ? true : false;
    quit();
}

//-------------------------------------------------------------------------------------------

TEST(WavInfo,readAndTestSample)
{
    QStringList libPaths = QApplication::libraryPaths();
    {
        char *argv = 0;
        WavInfoQtUnitApplication tester(0,&argv);
        tester.exec();
        ASSERT_TRUE(tester.testSucceeded());
    }
    QApplication::setLibraryPaths(libPaths);
}

//-------------------------------------------------------------------------------------------
