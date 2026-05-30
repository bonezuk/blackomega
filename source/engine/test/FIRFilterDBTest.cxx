#include "gtest/gtest.h"

#include "engine/inc/FIRFilterDB.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

void testGetFIRFilterFromDB(engine::FIRFilterType type, int expectLen)
{
    int len = 0;
    double *filter = engine::getFIRFilterFromDB(type, len);
    ASSERT_TRUE(filter != NULL);
    ASSERT_EQ(len, expectLen);
    delete [] filter;
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, NoFilter)
{
    int len = 0;
    double *filter = engine::getFIRFilterFromDB(engine::e_NoFilter, len);
    ASSERT_TRUE(filter == NULL);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassHalf8192)
{
    testGetFIRFilterFromDB(engine::e_lowPassHalf_8192, 8192);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassHalf4097)
{
    testGetFIRFilterFromDB(engine::e_lowPassHalf_4097, 4097);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassHalfDSD0_5)
{
    testGetFIRFilterFromDB(engine::e_lpHalf_DSD0_5, 1025);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassHalfDSD1)
{
    testGetFIRFilterFromDB(engine::e_lpHalf_DSD1, 2049);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassHalfDSD2)
{
    testGetFIRFilterFromDB(engine::e_lpHalf_DSD2, 4097);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassQuarterDSD2)
{
    testGetFIRFilterFromDB(engine::e_lpQuarter_DSD2, 4097);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassHalfDSD4)
{
    testGetFIRFilterFromDB(engine::e_lpHalf_DSD4, 8193);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassQuarterDSD4)
{
    testGetFIRFilterFromDB(engine::e_lpQuarter_DSD4, 8193);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassHalfDSD8)
{
    testGetFIRFilterFromDB(engine::e_lpHalf_DSD8, 16385);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassQuarterDSD8)
{
    testGetFIRFilterFromDB(engine::e_lpQuarter_DSD8, 16385);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassQuarterDSD16)
{
    testGetFIRFilterFromDB(engine::e_lpQuarter_DSD16, 32769);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassQuarterDSD32)
{
    testGetFIRFilterFromDB(engine::e_lpQuarter_DSD32, 65537);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassQuarterDSD64)
{
    testGetFIRFilterFromDB(engine::e_lpQuarter_DSD64, 131073);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassQuarterDSD128)
{
    testGetFIRFilterFromDB(engine::e_lpQuarter_DSD128, 262145);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassQuarterDSD256)
{
    testGetFIRFilterFromDB(engine::e_lpQuarter_DSD256, 524289);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassQuarterDSD512)
{
    testGetFIRFilterFromDB(engine::e_lpQuarter_DSD512, 1048577);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassQuarterDSD1024)
{
    testGetFIRFilterFromDB(engine::e_lpQuarter_DSD1024, 2097153);
}

//-------------------------------------------------------------------------------------------
