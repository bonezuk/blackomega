#include "gtest/gtest.h"

#include "audioio/inc/IOTimeStamp.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(IOTimeStamp,constructorDefault)
{
    IOTimeStamp t;
    EXPECT_FALSE(t.isValid());
    EXPECT_TRUE(t.time().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(IOTimeStamp,constructorSettingsWhenValid)
{
    common::TimeStamp tStamp(1.0);
    IOTimeStamp t(true,tStamp);
    EXPECT_TRUE(t.isValid());
    EXPECT_EQ(tStamp,t.time());
}

//-------------------------------------------------------------------------------------------

TEST(IOTimeStamp,constructorSettingsWhenInvalid)
{
    common::TimeStamp tStamp(1.0);
    IOTimeStamp t(false,tStamp);
    EXPECT_FALSE(t.isValid());
    EXPECT_EQ(tStamp,t.time());    
}

//-------------------------------------------------------------------------------------------

TEST(IOTimeStamp,constructorCopy)
{
    common::TimeStamp tStamp(4.0);
    IOTimeStamp tA(true,tStamp);
    IOTimeStamp tB(tA);
    EXPECT_TRUE(tB.isValid());
    EXPECT_EQ(tStamp,tB.time());
}

//-------------------------------------------------------------------------------------------

TEST(IOTimeStamp,equalsOperator)
{
    common::TimeStamp tStamp(4.0);
    IOTimeStamp tA(true,tStamp);
    IOTimeStamp tB;
    tB = tA;
    EXPECT_TRUE(tB.isValid());
    EXPECT_EQ(tStamp,tB.time());
}

//-------------------------------------------------------------------------------------------

TEST(IOTimeStamp,comparisonEqualsWhenInvalid)
{
    common::TimeStamp tStampA(4.0),tStampB(5.0);
    
    IOTimeStamp tA(false,tStampA);
    IOTimeStamp tB(false,tStampB);
    IOTimeStamp tC(true,tStampA);
    
    EXPECT_TRUE(tA==tA);
    EXPECT_TRUE(tA==tB);
    EXPECT_FALSE(tA==tC);
    EXPECT_FALSE(tB==tC);
}

//-------------------------------------------------------------------------------------------

TEST(IOTimeStamp,comparisonEqualsWhenValid)
{
    common::TimeStamp tStampA(4.0),tStampB(5.0);
    
    IOTimeStamp tA(true,tStampA);
    IOTimeStamp tB(false,tStampB);
    IOTimeStamp tC(true,tStampA);
    IOTimeStamp tD(false,tStampA);
    
    EXPECT_TRUE(tA==tA);
    
    EXPECT_FALSE(tA==tB);
    EXPECT_TRUE(tA==tC);
    EXPECT_FALSE(tA==tD);
    
    EXPECT_FALSE(tB==tC);
    EXPECT_FALSE(tC==tD);
}

//-------------------------------------------------------------------------------------------

TEST(IOTimeStamp,comparisonEqualsWhenNotInvalid)
{
    common::TimeStamp tStampA(4.0),tStampB(5.0);
    
    IOTimeStamp tA(false,tStampA);
    IOTimeStamp tB(false,tStampB);
    IOTimeStamp tC(true,tStampA);
    
    EXPECT_FALSE(tA!=tA);
    EXPECT_FALSE(tA!=tB);
    EXPECT_TRUE(tA!=tC);
    EXPECT_TRUE(tB!=tC);
}

//-------------------------------------------------------------------------------------------

TEST(IOTimeStamp,comparisonEqualsWhenNotValid)
{
    common::TimeStamp tStampA(4.0),tStampB(5.0);
    
    IOTimeStamp tA(true,tStampA);
    IOTimeStamp tB(false,tStampB);
    IOTimeStamp tC(true,tStampA);
    IOTimeStamp tD(false,tStampA);
    
    EXPECT_FALSE(tA!=tA);
    
    EXPECT_TRUE(tA!=tB);
    EXPECT_FALSE(tA!=tC);
    EXPECT_TRUE(tA!=tD);
    
    EXPECT_TRUE(tB!=tC);
    EXPECT_TRUE(tC!=tD);
}


//-------------------------------------------------------------------------------------------
