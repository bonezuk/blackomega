#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/DiskOps.h"
#include "common/inc/BIOMemoryStream.h"
#include "track/model/test/TrackDBTestEnviroment.h"

using namespace testing;
using namespace omega::common;
using namespace omega;

//-------------------------------------------------------------------------------------------
#if !defined(OMEGA_LINUX)
//-------------------------------------------------------------------------------------------

TEST(BIOMemoryStream,bookmarkAndPosition)
{
    const tubyte expectMem[11] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b };

    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "biostreamBookmark11Byte.dat");

    int i;
    int bkArray[5];
    tbyte mem[2];
    BIOMemoryStream file;

    EXPECT_TRUE(file.open(fileName));

    for(i=0;i<5;i++)
    {
        bkArray[i] = file.bookmark(1);
        EXPECT_TRUE(bkArray[i]>=0);
        EXPECT_TRUE(file.seek(2,e_Seek_Current));
    }

    for(i=0;i<5;i++)
    {
        EXPECT_TRUE(file.position(bkArray[i]));
        EXPECT_EQ((i*2)+1,file.offset());
        EXPECT_TRUE(file.read(mem,2)==2);
        tint cmp = ::memcmp(&expectMem[(i*2)+1],mem,2);
        EXPECT_EQ(0,cmp);
    }

    file.close();

    EXPECT_TRUE(file.open(fileName));

    for(int i=0;i<5;i++)
    {
        EXPECT_FALSE(file.position(bkArray[i]));
    }
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
