#include "gtest/gtest.h"

#include "common/inc/CommonTypes.h"

//-------------------------------------------------------------------------------------------

TEST(CommonFunctions,toDegreesAndRadians)
{
    int iDegree;

    for(iDegree=0;iDegree<360;iDegree++)
    {
        tfloat32 eRad = static_cast<tfloat32>(iDegree) * (c_PI_F / 180.0f);
        tfloat32 iRad = toRadians(static_cast<tfloat32>(iDegree));
        EXPECT_FLOAT_EQ(eRad,iRad);
        tfloat32 iDeg= toDegrees(iRad);
        EXPECT_FLOAT_EQ(static_cast<tfloat32>(iDegree),iDeg);
    }
}

//-------------------------------------------------------------------------------------------
