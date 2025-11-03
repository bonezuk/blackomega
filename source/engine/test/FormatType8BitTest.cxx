#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <stdio.h>
#include <math.h>

#include "engine/inc/FormatType.h"
#include "engine/inc/FormatTypeFromFloat.h"

using namespace omega::engine;
using namespace testing;
using namespace std;

//-------------------------------------------------------------------------------------------

TEST(FormatType8Bit,to8BitSignedFromLittleEndianWhenSignedLowerValue)
{
    tubyte x[1] = { 0x80 };
    tchar *y = reinterpret_cast<tchar *>(x);
    EXPECT_EQ(0x80,static_cast<tubyte>(to8BitSignedFromLittleEndian(y)));
}

TEST(FormatType8Bit,to8BitSignedFromLittleEndianWhenSignedUpperValue)
{
    tubyte x[1] = { 0xff };
    tchar *y = reinterpret_cast<tchar *>(x);
    EXPECT_EQ(0xff,static_cast<tubyte>(to8BitSignedFromLittleEndian(y)));
}

TEST(FormatType8Bit,to8BitSignedFromLittleEndianArbitaryNegative)
{
    tubyte x[1] = { 0xf4 };
    tchar *y = reinterpret_cast<tchar *>(x);
    EXPECT_EQ(0xf4,static_cast<tubyte>(to8BitSignedFromLittleEndian(y)));
}

TEST(FormatType8Bit,to8BitSignedFromBigEndianWhenSignedLowerValue)
{
    tubyte x[1] = { 0x80 };
    tchar *y = reinterpret_cast<tchar *>(x);
    EXPECT_EQ(0x80,static_cast<tubyte>(to8BitSignedFromBigEndian(y)));
}

TEST(FormatType8Bit,to8BitSignedFromBigEndianWhenSignedUpperValue)
{
    tubyte x[1] = { 0xff };
    tchar *y = reinterpret_cast<tchar *>(x);
    EXPECT_EQ(0xff,static_cast<tubyte>(to8BitSignedFromBigEndian(y)));
}

TEST(FormatType8Bit,to8BitSignedFromBigEndianArbitaryNegative)
{
    tubyte x[1] = { 0xf4 };
    tchar *y = reinterpret_cast<tchar *>(x);
    EXPECT_EQ(0xf4,static_cast<tubyte>(to8BitSignedFromBigEndian(y)));
}

TEST(FormatType8Bit,to8BitSignedFromLittleEndianWhenNegativeWithOffset)
{
    tubyte x[2] = { 0x00, 0xf4 };
    tchar *y = reinterpret_cast<tchar *>(x);
    EXPECT_EQ(0xf4,static_cast<tubyte>(to8BitSignedFromLittleEndian(y,1,2)));
}

TEST(FormatType8Bit,to8BitSignedFromBigEndianWhenNegativeWithOffset)
{
    tubyte x[2] = { 0x00, 0xf4 };
    tchar *y = reinterpret_cast<tchar *>(x);
    EXPECT_EQ(0xf4,static_cast<tubyte>(to8BitSignedFromBigEndian(y,1,2)));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType8Bit,sampleFrom8BitSigned)
{
    const tfloat64 c_Tolerance = 0.00000001;

    const signed char c_input[11] = {
        -128, -90, -60, -30, -1, 0,
        1, 30, 60, 90, 127
    };

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_expectOutput[11] = {
        -128.0f / 128.0f, // 0
         -90.0f / 128.0f, // 1
         -60.0f / 128.0f, // 2
         -30.0f / 128.0f, // 3
          -1.0f / 128.0f, // 4
           0.0f,         // 5
           1.0f / 127.0f, // 6
          30.0f / 127.0f, // 7
          60.0f / 127.0f, // 8
          90.0f / 127.0f, // 9
         127.0f / 127.0f  // 10
    };
#else
    const sample_t c_expectOutput[11] = {
        -128.0 / 128.0, // 0
         -90.0 / 128.0, // 1
         -60.0 / 128.0, // 2
         -30.0 / 128.0, // 3
          -1.0 / 128.0, // 4
           0.0,         // 5
           1.0 / 127.0, // 6
          30.0 / 127.0, // 7
          60.0 / 127.0, // 8
          90.0 / 127.0, // 9
         127.0 / 127.0  // 10
    };
#endif

    for(tint i=0;i<11;i++)
    {
        EXPECT_NEAR(c_expectOutput[i],sampleFrom8Bit(c_input[i]),c_Tolerance);
    }
}

//-------------------------------------------------------------------------------------------

TEST(FormatType8Bit,sampleFrom8BitUnsigned)
{
    const tfloat64 c_Tolerance = 0.00000001;

    const signed char c_input[11] = {
        -128, -90, -60, -30, -1, 0,
        1, 30, 60, 90, 127
    };

#if defined(SINGLE_FLOAT_SAMPLE)
    const sample_t c_expectOutput[11] = {
        -128.0f / 128.0f, // 0
         -90.0f / 128.0f, // 1
         -60.0f / 128.0f, // 2
         -30.0f / 128.0f, // 3
          -1.0f / 128.0f, // 4
           0.0f,         // 5
           1.0f / 127.0f, // 6
          30.0f / 127.0f, // 7
          60.0f / 127.0f, // 8
          90.0f / 127.0f, // 9
         127.0f / 127.0f  // 10
    };
#else
    const sample_t c_expectOutput[11] = {
        -128.0 / 128.0, // 0
         -90.0 / 128.0, // 1
         -60.0 / 128.0, // 2
         -30.0 / 128.0, // 3
          -1.0 / 128.0, // 4
           0.0,         // 5
           1.0 / 127.0, // 6
          30.0 / 127.0, // 7
          60.0 / 127.0, // 8
          90.0 / 127.0, // 9
         127.0 / 127.0  // 10
    };
#endif

    for(tint i=0;i<11;i++)
    {
        EXPECT_NEAR(c_expectOutput[i],sampleFrom8Bit(static_cast<tubyte>(c_input[i])),c_Tolerance);
    }
}

//-------------------------------------------------------------------------------------------
