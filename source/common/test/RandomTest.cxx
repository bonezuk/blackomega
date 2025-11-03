#include "gtest/gtest.h"

#include "common/inc/Random.h"

using namespace omega::common;

//-------------------------------------------------------------------------------------------

TEST(Random,rangeRandomUInt32)
{
    int i;
    Random *rand = Random::instance();
    rand->reset();
    
    for(i=0;i<5000;i++)
    {
        tuint64 v = static_cast<tuint64>(rand->randomUInt32());
        EXPECT_TRUE(v < 0x00000000ffffffffULL);
    }
}

//-------------------------------------------------------------------------------------------

TEST(Random,rangeRandomInt32)
{
    int i;
    Random *rand = Random::instance();
    rand->reset();
    
    for(i=0;i<5000;i++)
    {
        tuint64 v = static_cast<tuint64>(rand->randomInt31());
        EXPECT_TRUE(v < 0x000000007fffffffULL);
    }
}

//-------------------------------------------------------------------------------------------

TEST(Random,rangeRandomReal1)
{
    int i;
    Random *rand = Random::instance();
    rand->reset();
    
    for(i=0;i<5000;i++)
    {
        tfloat64 v = rand->randomReal1();
        EXPECT_TRUE(v>=0.0 && v<=1.0);
    }
}

//-------------------------------------------------------------------------------------------

TEST(Random,rangeRandomReal2)
{
    int i;
    Random *rand = Random::instance();
    rand->reset();
    
    for(i=0;i<5000;i++)
    {
        tfloat64 v = rand->randomReal2();
        EXPECT_TRUE(v>=0.0 && v<1.0);
    }
}

//-------------------------------------------------------------------------------------------

TEST(Random,rangeRandomReal3)
{
    int i;
    Random *rand = Random::instance();
    rand->reset();
    
    for(i=0;i<5000;i++)
    {
        tfloat64 v = rand->randomReal3();
        EXPECT_TRUE(v>0.0 && v<1.0);
    }
}

//-------------------------------------------------------------------------------------------

TEST(Random,rangeRandomReal53)
{
    int i;
    Random *rand = Random::instance();
    rand->reset();
    
    for(i=0;i<5000;i++)
    {
        tfloat64 v = rand->randomReal53();
        EXPECT_TRUE(v>=0.0 && v<=1.0);
    }
}

//-------------------------------------------------------------------------------------------

TEST(Random,repeatRandom)
{
    int i;
    tuint32 rA[512],rB[512];
    
    Random *rand = Random::instance();
    rand->reset();
    for(i=0;i<512;i++)
    {
        rA[i] = rand->randomUInt32();
    }
    rand->reset();
    for(i=0;i<512;i++)
    {
        rB[i] = rand->randomUInt32();
    }
    
    for(i=0;i<512;i++)
    {
        EXPECT_TRUE(rA[i]==rB[i]);
    }
}

//-------------------------------------------------------------------------------------------

TEST(Random,seedRandom)
{
    int i;
    tuint32 rA[512],rB[512];
    
    Random *rand = Random::instance();
    rand->seed(0x12345678);
    for(i=0;i<512;i++)
    {
        rA[i] = rand->randomUInt32();
    }
    rand->seed(0x12345678);
    for(i=0;i<512;i++)
    {
        rB[i] = rand->randomUInt32();
    }
    
    for(i=0;i<512;i++)
    {
        EXPECT_TRUE(rA[i]==rB[i]);
    }
}

//-------------------------------------------------------------------------------------------

TEST(Random,seedArrayRandom)
{
    int i;
    tuint32 rA[512],rB[512];
    tuint32 init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    
    Random *rand = Random::instance();
    rand->seed(init,length);
    for(i=0;i<512;i++)
    {
        rA[i] = rand->randomUInt32();
    }
    rand->seed(init,length);
    for(i=0;i<512;i++)
    {
        rB[i] = rand->randomUInt32();
    }
    
    for(i=0;i<512;i++)
    {
        EXPECT_TRUE(rA[i]==rB[i]);
    }
}

//-------------------------------------------------------------------------------------------
