#include "gtest/gtest.h"

#include "common/inc/BinaryDoubleArrayFile.h"
#include "common/inc/DiskOps.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(BinaryDoubleArrayFile, saveAndLoadSineWave)
{
    const int c_numberOfEntries = 8193;
    int idx;
    double *sineWave = new double [c_numberOfEntries];
    double incAngle = (2.0 * c_PI_D) / (static_cast<double>(c_numberOfEntries));
    double angle = 0.0;
    for(idx = 0; idx < c_numberOfEntries; idx++, angle += incAngle)
    {
        sineWave[idx] = sin(angle);
    }

	track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
	QString fileName = common::DiskOps::mergeName(testEnv->getTempDirectory(), "sinewave.bin");
    common::DiskOps::deleteFile(fileName);

    {
        common::BinaryDoubleArrayFile file;
        ASSERT_TRUE(file.save(fileName, sineWave, c_numberOfEntries));
    }

    for(int reads = 0; reads < 10; reads++)
    {
        int len;
        common::BinaryDoubleArrayFile file;
        double *X = file.load(fileName, len);
        ASSERT_TRUE(X != NULL);
        ASSERT_EQ(len, c_numberOfEntries);
        for(idx = 0; idx < len; idx++)
        {
            ASSERT_NEAR(X[idx], sineWave[idx], 0.00000001);
        }
        delete [] X;
    }
    
    delete [] sineWave;
}

//-------------------------------------------------------------------------------------------
