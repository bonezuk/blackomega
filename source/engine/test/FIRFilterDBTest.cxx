#include "gtest/gtest.h"

#include "engine/inc/FIRFilterDB.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassHalf8192)
{
    int len = 0;
    double *filter = engine::getFIRFilterFromDB(engine::e_lowPassHalf_8192, len);
    ASSERT_TRUE(filter != NULL);
    ASSERT_EQ(len, 8192);
    delete [] filter;
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilterDB, GetFIRLowPassHalf4097)
{
    int len = 0;
    double *filter = engine::getFIRFilterFromDB(engine::e_lowPassHalf_4097, len);
    ASSERT_TRUE(filter != NULL);
    ASSERT_EQ(len, 4097);
    delete [] filter;
}

//-------------------------------------------------------------------------------------------
