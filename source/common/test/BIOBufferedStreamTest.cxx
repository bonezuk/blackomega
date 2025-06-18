#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/BIOBufferedStream.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "common/inc/DiskOps.h"

using namespace testing;
using namespace omega::common;
using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStream,bookmarkAndPosition)
{
    const tubyte expectMem[11] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b };

    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "biostreamBookmark11Byte.dat");

    int bkArray[5];
    tbyte mem[2];
    BIOBufferedStream file(e_BIOStream_FileRead);

    EXPECT_TRUE(file.open(fileName));

    for(int i=0;i<5;i++)
    {
        bkArray[i] = file.bookmark(1);
        EXPECT_TRUE(bkArray[i]>=0);
        EXPECT_TRUE(file.seek(2,e_Seek_Current));
    }

    for(int i=0;i<5;i++)
    {
        EXPECT_TRUE(file.position(bkArray[i]));
        EXPECT_TRUE(file.read(mem,2)==2);
        EXPECT_TRUE(::memcmp(&expectMem[(i*2)+1],mem,2)==0);
    }

    file.close();

    EXPECT_TRUE(file.open(fileName));

    for(int i=0;i<5;i++)
    {
        EXPECT_FALSE(file.position(bkArray[i]));
    }
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStream,externalBufferMallocAllocation)
{
    const tint c_bufferSize = 100000;

    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "BIOexternalBufferMallocAllocation.dat");

    if(DiskOps::exist(fileName))
    {
        DiskOps::deleteFile(fileName);
    }

    tint i;
    tbyte inMem[256];
    tubyte cntMem[256];
    tbyte *rBuffer,*wBuffer;

    for(i=0;i<256;i++)
    {
        cntMem[i] = static_cast<tubyte>(i);
    }

    wBuffer = reinterpret_cast<tbyte *>(malloc(c_bufferSize));
    ASSERT_TRUE(wBuffer!=0);

    BIOBufferedStream wFile(e_BIOStream_FileCreate | e_BIOStream_FileWrite,wBuffer,c_bufferSize);
    ASSERT_TRUE(wFile.open(fileName));
    for(i=0;i<1000;i++)
    {
        ASSERT_EQ(256,wFile.write(cntMem,256));
    }
    wFile.close();

    rBuffer = reinterpret_cast<tbyte *>(malloc(c_bufferSize));
    ASSERT_TRUE(rBuffer!=0);

    BIOBufferedStream rFile(e_BIOStream_FileRead);
    ASSERT_TRUE(rFile.open(fileName));
    for(i=0;i<1000;i++)
    {
        ASSERT_EQ(256,rFile.read(inMem,256));
        ASSERT_EQ(0,memcmp(inMem,cntMem,256 * sizeof(tbyte)));
    }
    rFile.close();

    DiskOps::deleteFile(fileName);
}

//-------------------------------------------------------------------------------------------
