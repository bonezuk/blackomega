#include "gtest/gtest.h"

#include "common/inc/BinaryArrayStream.h"
#include "common/inc/Random.h"
#include "common/inc/DiskOps.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

void testSaveAndLoadIntegerArrayOfSize(const int len)
{
    track::model::TrackDBTestEnviroment *pTrackDBTest = track::model::TrackDBTestEnviroment::instance();
    QString tmpDir = pTrackDBTest->getTempDirectory();
    QString fileName = common::DiskOps::mergeName(pTrackDBTest->getTempDirectory(), "inttest.dat");

	common::Random *rand = common::Random::instance();
	rand->seed(0);
    int *arrayOrg = new int [len];
    for(int i = 0; i < len; i++)
    {
        arrayOrg[i] = rand->randomInt31();
    }

    {
        common::BinaryArrayStream<int> saver;
        ASSERT_TRUE(saver.save(fileName, arrayOrg, len));
    }

    {
        int loadLen = -1;
        common::BinaryArrayStream<int> loader;
        int *loadArray = loader.load(fileName, loadLen);
        ASSERT_TRUE(loadArray != NULL);
        ASSERT_EQ(loadLen, len);
        for(int i = 0; i < len; i++)
        {
            EXPECT_EQ(loadArray[i], arrayOrg[i]);
        }
        delete [] loadArray;
    }

    delete [] arrayOrg;
    common::DiskOps::deleteFile(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BinaryArrayStream, saveAndLoadIntegerArraySize10)
{
    testSaveAndLoadIntegerArrayOfSize(10);
}

//-------------------------------------------------------------------------------------------

TEST(BinaryArrayStream, saveAndLoadIntegerArraySize256)
{
    testSaveAndLoadIntegerArrayOfSize(256);
}

//-------------------------------------------------------------------------------------------

TEST(BinaryArrayStream, saveAndLoadIntegerArraySize8192)
{
    testSaveAndLoadIntegerArrayOfSize(8192);
}

//-------------------------------------------------------------------------------------------

TEST(BinaryArrayStream, saveAndLoadIntegerArraySize1000000)
{
    testSaveAndLoadIntegerArrayOfSize(1000000);
}

//-------------------------------------------------------------------------------------------

void testSaveAndLoadDoubleArrayOfSize(const int len)
{
    const tfloat64 c_TOLERANCE = 0.00000001;
    track::model::TrackDBTestEnviroment *pTrackDBTest = track::model::TrackDBTestEnviroment::instance();
    QString tmpDir = pTrackDBTest->getTempDirectory();
    QString fileName = common::DiskOps::mergeName(pTrackDBTest->getTempDirectory(), "inttest.dat");

	common::Random *rand = common::Random::instance();
	rand->seed(0);
    double *arrayOrg = new double [len];
    for(int i = 0; i < len; i++)
    {
        arrayOrg[i] = rand->randomReal1();
    }

    {
        common::BinaryArrayStream<double> saver;
        ASSERT_TRUE(saver.save(fileName, arrayOrg, len));
    }

    {
        int loadLen = -1;
        common::BinaryArrayStream<double> loader;
        double *loadArray = loader.load(fileName, loadLen);
        ASSERT_TRUE(loadArray != NULL);
        ASSERT_EQ(loadLen, len);
        for(int i = 0; i < len; i++)
        {
            EXPECT_NEAR(loadArray[i], arrayOrg[i], c_TOLERANCE);
        }
        delete [] loadArray;
    }

    delete [] arrayOrg;
    common::DiskOps::deleteFile(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BinaryArrayStream, saveAndLoadDoubleArraySize10)
{
    testSaveAndLoadDoubleArrayOfSize(10);
}

//-------------------------------------------------------------------------------------------

TEST(BinaryArrayStream, saveAndLoadDoubleArraySize256)
{
    testSaveAndLoadDoubleArrayOfSize(256);
}

//-------------------------------------------------------------------------------------------

TEST(BinaryArrayStream, saveAndLoadDoubleArraySize8192)
{
    testSaveAndLoadDoubleArrayOfSize(8192);
}

//-------------------------------------------------------------------------------------------

TEST(BinaryArrayStream, saveAndLoadDoubleArraySize1000000)
{
    testSaveAndLoadDoubleArrayOfSize(1000000);
}

//-------------------------------------------------------------------------------------------
