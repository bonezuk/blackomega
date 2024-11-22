#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <stdio.h>
#include <math.h>

#include "engine/inc/FormatType.h"

using namespace omega::engine;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(FormatType,to32BitSignedFromLittleEndianWhenZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to32BitSignedFromLittleEndian(y));
}

TEST(FormatType,to32BitSignedFromLittleEndianWhenUnsignedUpperValue)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x7f };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(2147483647,to32BitSignedFromLittleEndian(y));
}

TEST(FormatType,to32BitSignedFromLittleEndianWhenSignedLowerValue)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x80 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ( -2147483647 - 1 ,to32BitSignedFromLittleEndian(y));
}

TEST(FormatType,to32BitSignedFromLittleEndianArbitaryPositive)
{
	tubyte x[4] = { 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a2b3c4d,to32BitSignedFromLittleEndian(y));
}

TEST(FormatType,to32BitSignedFromLittleEndianArbitaryNegative)
{
	tubyte x[4] = { 0xeb, 0x32, 0xa4, 0xf8 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(-123456789,to32BitSignedFromLittleEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to32BitSignedFromLittleEndianNoMemory)
{
	EXPECT_EQ(0,to32BitSignedFromLittleEndian(0,0,4));
}

TEST(FormatType,to32BitSignedFromLittleEndianOverRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x3c4d5e6f,to32BitSignedFromLittleEndian(y,0,6));
	EXPECT_EQ(0x2b3c4d5e,to32BitSignedFromLittleEndian(y,1,6));
	EXPECT_EQ(0x1a2b3c4d,to32BitSignedFromLittleEndian(y,2,6));
	EXPECT_EQ(0,to32BitSignedFromLittleEndian(y,3,6));
}

TEST(FormatType,to32BitSignedFromLittleEndianOverNegativeRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(&x[2]);

	EXPECT_EQ(0x3c4d5e6f,to32BitSignedFromLittleEndian(y,-2,4));
	EXPECT_EQ(0x2b3c4d5e,to32BitSignedFromLittleEndian(y,-1,4));
	EXPECT_EQ(0x1a2b3c4d,to32BitSignedFromLittleEndian(y, 0,4));
	EXPECT_EQ(0,to32BitSignedFromLittleEndian(y,1,4));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to32BitSignedFromBigEndianWhenZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to32BitSignedFromBigEndian(y));
}

TEST(FormatType,to32BitSignedFromBigEndianWhenUnsignedUpperValue)
{
	tubyte x[4] = { 0x7f, 0xff, 0xff, 0xff };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(2147483647,to32BitSignedFromBigEndian(y));
}

TEST(FormatType,to32BitSignedFromBigEndianWhenSignedLowerValue)
{
	tubyte x[4] = { 0x80, 0x00, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ( -2147483647 - 1 ,to32BitSignedFromBigEndian(y));
}

TEST(FormatType,to32BitSignedFromBigEndianWhenArbitaryPositive)
{
	tubyte x[4] = { 0x1a, 0x2b, 0x3c, 0x4d };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a2b3c4d,to32BitSignedFromBigEndian(y));
}

TEST(FormatType,to32BitSignedFromBigEndianWhenArbitaryNegative)
{
	tubyte x[4] = { 0xf8, 0xa4, 0x32, 0xeb };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(-123456789,to32BitSignedFromBigEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to32BitSignedFromBigEndianNoMemory)
{
	EXPECT_EQ(0,to32BitSignedFromBigEndian(0,0,4));
}

TEST(FormatType,to32BitSignedFromBigEndianOverRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x1a2b3c4d,to32BitSignedFromBigEndian(y,0,6));
	EXPECT_EQ(0x2b3c4d5e,to32BitSignedFromBigEndian(y,1,6));
	EXPECT_EQ(0x3c4d5e6f,to32BitSignedFromBigEndian(y,2,6));
	EXPECT_EQ(0,to32BitSignedFromBigEndian(y,3,6));
}

TEST(FormatType,to32BitSignedFromBigEndianOverNegativeRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(&x[2]);
	
	EXPECT_EQ(0x1a2b3c4d,to32BitSignedFromBigEndian(y,-2,4));
	EXPECT_EQ(0x2b3c4d5e,to32BitSignedFromBigEndian(y,-1,4));
	EXPECT_EQ(0x3c4d5e6f,to32BitSignedFromBigEndian(y, 0,4));
	EXPECT_EQ(0,to32BitSignedFromBigEndian(y,1,4));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to32BitUnsignedFromLittleEndianWhenZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to32BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to32BitUnsignedFromLittleEndianWhenUnsignedUpperValue)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x7f };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x7fffffff,to32BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to32BitUnsignedFromLittleEndianWhenSignedLowerValue)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x80 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x80000000,to32BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to32BitUnsignedFromLittleEndianWhenArbitary)
{
	tubyte x[4] = { 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a2b3c4d,to32BitUnsignedFromLittleEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to32BitUnsignedFromLittleEndianNoMemory)
{
	EXPECT_EQ(0,to32BitUnsignedFromLittleEndian(0,0,4));
}

TEST(FormatType,to32BitUnsignedFromLittleEndianOverRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x3c4d5e6f,to32BitUnsignedFromLittleEndian(y,0,6));
	EXPECT_EQ(0x2b3c4d5e,to32BitUnsignedFromLittleEndian(y,1,6));
	EXPECT_EQ(0x1a2b3c4d,to32BitUnsignedFromLittleEndian(y,2,6));
	EXPECT_EQ(0,to32BitUnsignedFromLittleEndian(y,3,6));
}

TEST(FormatType,to32BitUnsignedFromLittleEndianOverNegativeRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(&x[2]);

	EXPECT_EQ(0x3c4d5e6f,to32BitUnsignedFromLittleEndian(y,-2,4));
	EXPECT_EQ(0x2b3c4d5e,to32BitUnsignedFromLittleEndian(y,-1,4));
	EXPECT_EQ(0x1a2b3c4d,to32BitUnsignedFromLittleEndian(y, 0,4));
	EXPECT_EQ(0,to32BitUnsignedFromLittleEndian(y,1,4));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to32BitUnsignedFromBigEndianWhenZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to32BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to32BitUnsignedFromBigEndianWhenUnsignedUpperValue)
{
	tubyte x[4] = { 0x7f, 0xff, 0xff, 0xff };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x7fffffff,to32BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to32BitUnsignedFromBigEndianWhenSignedLowerValue)
{
	tubyte x[4] = { 0x80, 0x00, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x80000000,to32BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to32BitUnsignedFromBigEndianWhenArbitary)
{
	tubyte x[4] = { 0x1a, 0x2b, 0x3c, 0x4d };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a2b3c4d,to32BitUnsignedFromBigEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to32BitUnsignedFromBigEndianNoMemory)
{
	EXPECT_EQ(0,to32BitUnsignedFromBigEndian(0,0,4));
}

TEST(FormatType,to32BitUnsignedFromBigEndianOverRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x1a2b3c4d,to32BitUnsignedFromBigEndian(y,0,6));
	EXPECT_EQ(0x2b3c4d5e,to32BitUnsignedFromBigEndian(y,1,6));
	EXPECT_EQ(0x3c4d5e6f,to32BitUnsignedFromBigEndian(y,2,6));
	EXPECT_EQ(0,to32BitUnsignedFromBigEndian(y,3,6));
}

TEST(FormatType,to32BitUnsignedFromBigEndianOverNegativeRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(&x[2]);
	
	EXPECT_EQ(0x1a2b3c4d,to32BitUnsignedFromBigEndian(y,-2,4));
	EXPECT_EQ(0x2b3c4d5e,to32BitUnsignedFromBigEndian(y,-1,4));
	EXPECT_EQ(0x3c4d5e6f,to32BitUnsignedFromBigEndian(y, 0,4));
	EXPECT_EQ(0,to32BitUnsignedFromBigEndian(y,1,4));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to24BitSignedFromLittleEndianWhenZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to24BitSignedFromLittleEndian(y));
}

TEST(FormatType,to24BitSignedFromLittleEndianWhenUnsignedUpperValue)
{
	tubyte x[3] = { 0xff, 0xff, 0x7f };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(8388607,to24BitSignedFromLittleEndian(y));
}

TEST(FormatType,to24BitSignedFromLittleEndianWhenUnsignedLowerValue)
{
	tubyte x[3] = { 0x01, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(1,to24BitSignedFromLittleEndian(y));
}

TEST(FormatType,to24BitSignedFromLittleEndianWhenSignedLowerValue)
{
	tubyte x[3] = { 0x00, 0x00, 0x80 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ( -8388608,to24BitSignedFromLittleEndian(y));
}

TEST(FormatType,to32BitSignedFromLittleEndianWhenSignedUpperValue)
{
	tubyte x[3] = { 0xff, 0xff, 0xff };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ( -1 ,to24BitSignedFromLittleEndian(y));
}

TEST(FormatType,to24BitSignedFromLittleEndianArbitaryPositive)
{
	tubyte x[3] = { 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x001a2b3c,to24BitSignedFromLittleEndian(y));
}

TEST(FormatType,to24BitSignedFromLittleEndianArbitaryNegative)
{
	tubyte x[3] = { 0x79, 0x29, 0xed };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(-1234567,to24BitSignedFromLittleEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to24BitUnsignedFromLittleEndianWhenZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to24BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to24BitUnsignedFromLittleEndianWhenUnsignedUpperValue)
{
	tubyte x[3] = { 0xff, 0xff, 0x7f };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x007fffff,to24BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to24BitUnsignedFromLittleEndianWhenSignedLowerValue)
{
	tubyte x[3] = { 0x00, 0x00, 0x80 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x00800000,to24BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to24BitUnsignedFromLittleEndianWhenArbitary)
{
	tubyte x[3] = { 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x001a2b3c,to24BitUnsignedFromLittleEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to24BitSignedFromBigEndianWhenZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to24BitSignedFromBigEndian(y));
}

TEST(FormatType,to24BitSignedFromBigEndianWhenUnsignedUpperValue)
{
	tubyte x[3] = { 0x7f, 0xff, 0xff };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(8388607,to24BitSignedFromBigEndian(y));
}

TEST(FormatType,to24BitSignedFromBigEndianWhenUnsignedLowerValue)
{
	tubyte x[3] = { 0x00, 0x00, 0x01 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(1,to24BitSignedFromBigEndian(y));
}

TEST(FormatType,to24BitSignedFromBigEndianWhenSignedLowerValue)
{
	tubyte x[3] = { 0x80, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ( -8388608,to24BitSignedFromBigEndian(y));
}

TEST(FormatType,to32BitSignedFromBigEndianWhenSignedUpperValue)
{
	tubyte x[3] = { 0xff, 0xff, 0xff };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ( -1 ,to24BitSignedFromBigEndian(y));
}

TEST(FormatType,to24BitSignedFromBigEndianArbitaryPositive)
{
	tubyte x[3] = { 0x1a, 0x2b, 0x3c };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x001a2b3c,to24BitSignedFromBigEndian(y));
}

TEST(FormatType,to24BitSignedFromBigEndianArbitaryNegative)
{
	tubyte x[3] = { 0xed, 0x29, 0x79 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(-1234567,to24BitSignedFromBigEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to24BitUnsignedFromBigEndianWhenZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to24BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to24BitUnsignedFromBigEndianWhenUnsignedUpperValue)
{
	tubyte x[3] = { 0x7f, 0xff, 0xff };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x007fffff,to24BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to24BitUnsignedFromBigEndianWhenSignedLowerValue)
{
	tubyte x[3] = { 0x80, 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x00800000,to24BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to24BitUnsignedFromBigEndianWhenArbitary)
{
	tubyte x[3] = { 0x1a, 0x2b, 0x3c };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x001a2b3c,to24BitUnsignedFromBigEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to24BitSignedFromLittleEndianNoMemory)
{
	EXPECT_EQ(0,to24BitSignedFromLittleEndian(0,0,4));
}

TEST(FormatType,to24BitSignedFromLittleEndianOverRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x004d5e6f,to24BitSignedFromLittleEndian(y,0,6));
	EXPECT_EQ(0x003c4d5e,to24BitSignedFromLittleEndian(y,1,6));
	EXPECT_EQ(0x002b3c4d,to24BitSignedFromLittleEndian(y,2,6));
	EXPECT_EQ(0x001a2b3c,to24BitSignedFromLittleEndian(y,3,6));
	EXPECT_EQ(0,to24BitSignedFromLittleEndian(y,4,6));
}

TEST(FormatType,to24BitSignedFromLittleEndianOverNegativeRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(&x[3]);

	EXPECT_EQ(0x004d5e6f,to24BitSignedFromLittleEndian(y,-3,3));
	EXPECT_EQ(0x003c4d5e,to24BitSignedFromLittleEndian(y,-2,3));
	EXPECT_EQ(0x002b3c4d,to24BitSignedFromLittleEndian(y,-1,3));
	EXPECT_EQ(0x001a2b3c,to24BitSignedFromLittleEndian(y, 0,3));
	EXPECT_EQ(0,to24BitSignedFromLittleEndian(y,1,3));
}

TEST(FormatType,to24BitSignedFromLittleEndianWhenNegativeWithOffset)
{
	tubyte x[4] = { 0x00, 0x79, 0x29, 0xed };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(-1234567,to24BitSignedFromLittleEndian(y,1,4));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to24BitUnsignedFromLittleEndianNoMemory)
{
	EXPECT_EQ(0,to24BitUnsignedFromLittleEndian(0,0,4));
}

TEST(FormatType,to24BitUnsignedFromLittleEndianOverRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x004d5e6f,to24BitUnsignedFromLittleEndian(y,0,6));
	EXPECT_EQ(0x003c4d5e,to24BitUnsignedFromLittleEndian(y,1,6));
	EXPECT_EQ(0x002b3c4d,to24BitUnsignedFromLittleEndian(y,2,6));
	EXPECT_EQ(0x001a2b3c,to24BitUnsignedFromLittleEndian(y,3,6));
	EXPECT_EQ(0,to24BitUnsignedFromLittleEndian(y,4,6));
}

TEST(FormatType,to24BitUnsignedFromLittleEndianOverNegativeRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(&x[3]);

	EXPECT_EQ(0x004d5e6f,to24BitUnsignedFromLittleEndian(y,-3,3));
	EXPECT_EQ(0x003c4d5e,to24BitUnsignedFromLittleEndian(y,-2,3));
	EXPECT_EQ(0x002b3c4d,to24BitUnsignedFromLittleEndian(y,-1,3));
	EXPECT_EQ(0x001a2b3c,to24BitUnsignedFromLittleEndian(y, 0,3));
	EXPECT_EQ(0,to24BitUnsignedFromLittleEndian(y,1,3));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to24BitSignedFromBigEndianNoMemory)
{
	EXPECT_EQ(0,to24BitSignedFromBigEndian(0,0,4));
}

TEST(FormatType,to24BitSignedFromBigEndianOverRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x001a2b3c,to24BitSignedFromBigEndian(y,0,6));
	EXPECT_EQ(0x002b3c4d,to24BitSignedFromBigEndian(y,1,6));
	EXPECT_EQ(0x003c4d5e,to24BitSignedFromBigEndian(y,2,6));
	EXPECT_EQ(0x004d5e6f,to24BitSignedFromBigEndian(y,3,6));
	EXPECT_EQ(0,to24BitSignedFromBigEndian(y,4,6));
}

TEST(FormatType,to24BitSignedFromBigEndianOverNegativeRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(&x[3]);
	
	EXPECT_EQ(0x001a2b3c,to24BitSignedFromBigEndian(y,-3,3));
	EXPECT_EQ(0x002b3c4d,to24BitSignedFromBigEndian(y,-2,3));
	EXPECT_EQ(0x003c4d5e,to24BitSignedFromBigEndian(y,-1,3));
	EXPECT_EQ(0x004d5e6f,to24BitSignedFromBigEndian(y, 0,3));
	EXPECT_EQ(0,to24BitSignedFromBigEndian(y,1,3));
}

TEST(FormatType,to24BitSignedFromBigEndianWhenNegativeWithOffset)
{
	tubyte x[4] = { 0x00, 0x79, 0x29, 0xed };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(-1234567,to24BitSignedFromLittleEndian(y,1,4));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to24BitUnsignedFromBigEndianNoMemory)
{
	EXPECT_EQ(0,to24BitUnsignedFromBigEndian(0,0,4));
}

TEST(FormatType,to24BitUnsignedFromBigEndianOverRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x001a2b3c,to24BitUnsignedFromBigEndian(y,0,6));
	EXPECT_EQ(0x002b3c4d,to24BitUnsignedFromBigEndian(y,1,6));
	EXPECT_EQ(0x003c4d5e,to24BitUnsignedFromBigEndian(y,2,6));
	EXPECT_EQ(0x004d5e6f,to24BitUnsignedFromBigEndian(y,3,6));
	EXPECT_EQ(0,to24BitUnsignedFromBigEndian(y,4,6));
}

TEST(FormatType,to24BitUnsignedFromBigEndianOverNegativeRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(&x[3]);
	
	EXPECT_EQ(0x001a2b3c,to24BitUnsignedFromBigEndian(y,-3,3));
	EXPECT_EQ(0x002b3c4d,to24BitUnsignedFromBigEndian(y,-2,3));
	EXPECT_EQ(0x003c4d5e,to24BitUnsignedFromBigEndian(y,-1,3));
	EXPECT_EQ(0x004d5e6f,to24BitUnsignedFromBigEndian(y, 0,3));
	EXPECT_EQ(0,to24BitUnsignedFromBigEndian(y,1,3));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to16BitSignedFromLittleEndianWhenZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to16BitSignedFromLittleEndian(y));
}

TEST(FormatType,to16BitSignedFromLittleEndianWhenUnsignedUpperValue)
{
	tubyte x[2] = { 0xff, 0x7f };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(32767,to16BitSignedFromLittleEndian(y));
}

TEST(FormatType,to16BitSignedFromLittleEndianWhenUnsignedLowerValue)
{
	tubyte x[2] = { 0x01, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(1,to16BitSignedFromLittleEndian(y));
}

TEST(FormatType,to16BitSignedFromLittleEndianWhenSignedLowerValue)
{
	tubyte x[2] = { 0x00, 0x80 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ( -32768,to16BitSignedFromLittleEndian(y));
}

TEST(FormatType,to16BitSignedFromLittleEndianWhenSignedUpperValue)
{
	tubyte x[2] = { 0xff, 0xff };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ( -1 ,to16BitSignedFromLittleEndian(y));
}

TEST(FormatType,to16BitSignedFromLittleEndianArbitaryPositive)
{
	tubyte x[2] = { 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a2b,to16BitSignedFromLittleEndian(y));
}

TEST(FormatType,to16BitSignedFromLittleEndianArbitaryNegative)
{
	tubyte x[2] = { 0x2e, 0xfb };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(-1234,to16BitSignedFromLittleEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to16BitUnsignedFromLittleEndianWhenZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to16BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to16BitUnsignedFromLittleEndianWhenUnsignedUpperValue)
{
	tubyte x[2] = {0xff, 0x7f };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x7fff,to16BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to16BitUnsignedFromLittleEndianWhenSignedLowerValue)
{
	tubyte x[2] = { 0x00, 0x80 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x8000,to16BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to16BitUnsignedFromLittleEndianWhenArbitary)
{
	tubyte x[2] = { 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a2b,to16BitUnsignedFromLittleEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to16BitSignedFromBigEndianWhenZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to16BitSignedFromBigEndian(y));
}

TEST(FormatType,to16BitSignedFromBigEndianWhenUnsignedUpperValue)
{
	tubyte x[2] = { 0x7f, 0xff };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(32767,to16BitSignedFromBigEndian(y));
}

TEST(FormatType,to16BitSignedFromBigEndianWhenUnsignedLowerValue)
{
	tubyte x[2] = { 0x00, 0x01 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(1,to16BitSignedFromBigEndian(y));
}

TEST(FormatType,to16BitSignedFromBigEndianWhenSignedLowerValue)
{
	tubyte x[2] = { 0x80, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ( -32768,to16BitSignedFromBigEndian(y));
}

TEST(FormatType,to16BitSignedFromBigEndianWhenSignedUpperValue)
{
	tubyte x[2] = { 0xff, 0xff };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ( -1 ,to16BitSignedFromBigEndian(y));
}

TEST(FormatType,to16BitSignedFromBigEndianArbitaryPositive)
{
	tubyte x[2] = { 0x1a, 0x2b };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a2b,to16BitSignedFromBigEndian(y));
}

TEST(FormatType,to16BitSignedFromBigEndianArbitaryNegative)
{
	tubyte x[2] = { 0xfb, 0x2e };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(-1234,to16BitSignedFromBigEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to16BitUnsignedFromBigEndianWhenZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to16BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to16BitUnsignedFromBigEndianWhenUnsignedUpperValue)
{
	tubyte x[2] = {0x7f, 0xff };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x7fff,to16BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to16BitUnsignedFromBigEndianWhenSignedLowerValue)
{
	tubyte x[2] = { 0x80, 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x8000,to16BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to16BitUnsignedFromBigEndianWhenArbitary)
{
	tubyte x[2] = { 0x1a, 0x2b };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a2b,to16BitUnsignedFromBigEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to16BitSignedFromLittleEndianNoMemory)
{
	EXPECT_EQ(0,to16BitSignedFromLittleEndian(0,0,4));
}

TEST(FormatType,to16BitSignedFromLittleEndianOverRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x5e6f,to16BitSignedFromLittleEndian(y,0,6));
	EXPECT_EQ(0x4d5e,to16BitSignedFromLittleEndian(y,1,6));
	EXPECT_EQ(0x3c4d,to16BitSignedFromLittleEndian(y,2,6));
	EXPECT_EQ(0x2b3c,to16BitSignedFromLittleEndian(y,3,6));
	EXPECT_EQ(0x1a2b,to16BitSignedFromLittleEndian(y,4,6));
	EXPECT_EQ(0,to16BitSignedFromLittleEndian(y,5,6));
}

TEST(FormatType,to16BitSignedFromLittleEndianOverNegativeRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(&x[4]);

	EXPECT_EQ(0x5e6f,to16BitSignedFromLittleEndian(y,-4,2));
	EXPECT_EQ(0x4d5e,to16BitSignedFromLittleEndian(y,-3,2));
	EXPECT_EQ(0x3c4d,to16BitSignedFromLittleEndian(y,-2,2));
	EXPECT_EQ(0x2b3c,to16BitSignedFromLittleEndian(y,-1,2));
	EXPECT_EQ(0x1a2b,to16BitSignedFromLittleEndian(y, 0,2));
	EXPECT_EQ(0,to16BitSignedFromLittleEndian(y,1,2));
}


TEST(FormatType,to16BitSignedFromLittleEndianWhenNegativeWithOffset)
{
	tubyte x[3] = { 0x00, 0x2e, 0xfb };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(-1234,to16BitSignedFromLittleEndian(y,1,3));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to16BitUnsignedFromLittleEndianNoMemory)
{
	EXPECT_EQ(0,to16BitUnsignedFromLittleEndian(0,0,4));
}

TEST(FormatType,to16BitUnsignedFromLittleEndianOverRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x5e6f,to16BitUnsignedFromLittleEndian(y,0,6));
	EXPECT_EQ(0x4d5e,to16BitUnsignedFromLittleEndian(y,1,6));
	EXPECT_EQ(0x3c4d,to16BitUnsignedFromLittleEndian(y,2,6));
	EXPECT_EQ(0x2b3c,to16BitUnsignedFromLittleEndian(y,3,6));
	EXPECT_EQ(0x1a2b,to16BitUnsignedFromLittleEndian(y,4,6));
	EXPECT_EQ(0,to16BitUnsignedFromLittleEndian(y,5,6));
}

TEST(FormatType,to16BitUnsignedFromLittleEndianOverNegativeRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(&x[4]);

	EXPECT_EQ(0x5e6f,to16BitUnsignedFromLittleEndian(y,-4,2));
	EXPECT_EQ(0x4d5e,to16BitUnsignedFromLittleEndian(y,-3,2));
	EXPECT_EQ(0x3c4d,to16BitUnsignedFromLittleEndian(y,-2,2));
	EXPECT_EQ(0x2b3c,to16BitUnsignedFromLittleEndian(y,-1,2));
	EXPECT_EQ(0x1a2b,to16BitUnsignedFromLittleEndian(y, 0,2));
	EXPECT_EQ(0,to16BitUnsignedFromLittleEndian(y,1,2));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to16BitSignedFromBigEndianNoMemory)
{
	EXPECT_EQ(0,to16BitSignedFromBigEndian(0,0,4));
}

TEST(FormatType,to16BitSignedFromBigEndianOverRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x1a2b,to16BitSignedFromBigEndian(y,0,6));
	EXPECT_EQ(0x2b3c,to16BitSignedFromBigEndian(y,1,6));
	EXPECT_EQ(0x3c4d,to16BitSignedFromBigEndian(y,2,6));
	EXPECT_EQ(0x4d5e,to16BitSignedFromBigEndian(y,3,6));
	EXPECT_EQ(0x5e6f,to16BitSignedFromBigEndian(y,4,6));
	EXPECT_EQ(0,to16BitSignedFromBigEndian(y,5,6));
}

TEST(FormatType,to16BitSignedFromBigEndianOverNegativeRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(&x[4]);
	
	EXPECT_EQ(0x1a2b,to16BitSignedFromBigEndian(y,-4,2));
	EXPECT_EQ(0x2b3c,to16BitSignedFromBigEndian(y,-3,2));
	EXPECT_EQ(0x3c4d,to16BitSignedFromBigEndian(y,-2,2));
	EXPECT_EQ(0x4d5e,to16BitSignedFromBigEndian(y,-1,2));
	EXPECT_EQ(0x5e6f,to16BitSignedFromBigEndian(y, 0,2));
	EXPECT_EQ(0,to16BitSignedFromBigEndian(y,1,2));
}

TEST(FormatType,to16BitSignedFromBigEndianWhenNegativeWithOffset)
{
	tubyte x[3] = { 0x00, 0xfb, 0x2e };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(-1234,to16BitSignedFromBigEndian(y,1,3));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to16BitUnsignedFromBigEndianNoMemory)
{
	EXPECT_EQ(0,to16BitUnsignedFromBigEndian(0,0,4));
}

TEST(FormatType,to16BitUnsignedFromBigEndianOverRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x1a2b,to16BitUnsignedFromBigEndian(y,0,6));
	EXPECT_EQ(0x2b3c,to16BitUnsignedFromBigEndian(y,1,6));
	EXPECT_EQ(0x3c4d,to16BitUnsignedFromBigEndian(y,2,6));
	EXPECT_EQ(0x4d5e,to16BitUnsignedFromBigEndian(y,3,6));
	EXPECT_EQ(0x5e6f,to16BitUnsignedFromBigEndian(y,4,6));
	EXPECT_EQ(0,to16BitUnsignedFromBigEndian(y,5,6));
}

TEST(FormatType,to16BitUnsignedFromBigEndianOverNegativeRange)
{
	tubyte x[6] = { 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f };
	tchar *y = reinterpret_cast<tchar *>(&x[4]);
	
	EXPECT_EQ(0x1a2b,to16BitUnsignedFromBigEndian(y,-4,2));
	EXPECT_EQ(0x2b3c,to16BitUnsignedFromBigEndian(y,-3,2));
	EXPECT_EQ(0x3c4d,to16BitUnsignedFromBigEndian(y,-2,2));
	EXPECT_EQ(0x4d5e,to16BitUnsignedFromBigEndian(y,-1,2));
	EXPECT_EQ(0x5e6f,to16BitUnsignedFromBigEndian(y, 0,2));
	EXPECT_EQ(0,to16BitUnsignedFromBigEndian(y,1,2));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to8BitSignedFromLittleEndianWhenZero)
{
	tubyte x[1] = { 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to8BitSignedFromLittleEndian(y));
}

TEST(FormatType,to8BitSignedFromLittleEndianWhenUnsignedUpperValue)
{
	tubyte x[1] = { 0x7f };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(127,to8BitSignedFromLittleEndian(y));
}

TEST(FormatType,to8BitSignedFromLittleEndianWhenUnsignedLowerValue)
{
	tubyte x[1] = { 0x01 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(1,to8BitSignedFromLittleEndian(y));
}

TEST(FormatType,to8BitSignedFromLittleEndianArbitaryPositive)
{
	tubyte x[1] = { 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a,to8BitSignedFromLittleEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to8BitUnsignedFromLittleEndianWhenZero)
{
	tubyte x[1] = { 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to8BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to8BitUnsignedFromLittleEndianWhenUnsignedUpperValue)
{
	tubyte x[1] = { 0x7f };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x7f,to8BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to8BitUnsignedFromLittleEndianWhenSignedLowerValue)
{
	tubyte x[1] = { 0x80 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x80,to8BitUnsignedFromLittleEndian(y));
}

TEST(FormatType,to8BitUnsignedFromLittleEndianWhenArbitary)
{
	tubyte x[1] = { 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a,to8BitUnsignedFromLittleEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to8BitSignedFromBigEndianWhenZero)
{
	tubyte x[1] = { 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to8BitSignedFromBigEndian(y));
}

TEST(FormatType,to8BitSignedFromBigEndianWhenUnsignedUpperValue)
{
	tubyte x[1] = { 0x7f };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(127,to8BitSignedFromBigEndian(y));
}

TEST(FormatType,to8BitSignedFromBigEndianWhenUnsignedLowerValue)
{
	tubyte x[1] = { 0x01 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(1,to8BitSignedFromBigEndian(y));
}

TEST(FormatType,to8BitSignedFromBigEndianArbitaryPositive)
{
	tubyte x[1] = { 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a,to8BitSignedFromBigEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to8BitUnsignedFromBigEndianWhenZero)
{
	tubyte x[1] = { 0x00 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0,to8BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to8BitUnsignedFromBigEndianWhenUnsignedUpperValue)
{
	tubyte x[1] = { 0x7f };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x7f,to8BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to8BitUnsignedFromBigEndianWhenSignedLowerValue)
{
	tubyte x[1] = { 0x80 };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x80,to8BitUnsignedFromBigEndian(y));
}

TEST(FormatType,to8BitUnsignedFromBigEndianWhenArbitary)
{
	tubyte x[1] = { 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	EXPECT_EQ(0x1a,to8BitUnsignedFromBigEndian(y));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to8BitSignedFromLittleEndianNoMemory)
{
	EXPECT_EQ(0,to8BitSignedFromLittleEndian(0,0,4));
}

TEST(FormatType,to8BitSignedFromLittleEndianOverRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x6f,to8BitSignedFromLittleEndian(y,0,6));
	EXPECT_EQ(0x5e,to8BitSignedFromLittleEndian(y,1,6));
	EXPECT_EQ(0x4d,to8BitSignedFromLittleEndian(y,2,6));
	EXPECT_EQ(0x3c,to8BitSignedFromLittleEndian(y,3,6));
	EXPECT_EQ(0x2b,to8BitSignedFromLittleEndian(y,4,6));
	EXPECT_EQ(0x1a,to8BitSignedFromLittleEndian(y,5,6));
	EXPECT_EQ(0,to8BitSignedFromLittleEndian(y,6,6));
}

TEST(FormatType,to8BitSignedFromLittleEndianOverNegativeRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(&x[5]);

	EXPECT_EQ(0x6f,to8BitSignedFromLittleEndian(y,-5,1));
	EXPECT_EQ(0x5e,to8BitSignedFromLittleEndian(y,-4,1));
	EXPECT_EQ(0x4d,to8BitSignedFromLittleEndian(y,-3,1));
	EXPECT_EQ(0x3c,to8BitSignedFromLittleEndian(y,-2,1));
	EXPECT_EQ(0x2b,to8BitSignedFromLittleEndian(y,-1,1));
	EXPECT_EQ(0x1a,to8BitSignedFromLittleEndian(y, 0,1));
	EXPECT_EQ(0,to8BitSignedFromLittleEndian(y,1,1));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to8BitUnsignedFromLittleEndianNoMemory)
{
	EXPECT_EQ(0,to8BitUnsignedFromLittleEndian(0,0,4));
}

TEST(FormatType,to8BitUnsignedFromLittleEndianOverRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x6f,to8BitUnsignedFromLittleEndian(y,0,6));
	EXPECT_EQ(0x5e,to8BitUnsignedFromLittleEndian(y,1,6));
	EXPECT_EQ(0x4d,to8BitUnsignedFromLittleEndian(y,2,6));
	EXPECT_EQ(0x3c,to8BitUnsignedFromLittleEndian(y,3,6));
	EXPECT_EQ(0x2b,to8BitUnsignedFromLittleEndian(y,4,6));
	EXPECT_EQ(0x1a,to8BitUnsignedFromLittleEndian(y,5,6));
	EXPECT_EQ(0,to8BitUnsignedFromLittleEndian(y,6,6));
}

TEST(FormatType,to8BitUnsignedFromLittleEndianOverNegativeRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(&x[5]);

	EXPECT_EQ(0x6f,to8BitUnsignedFromLittleEndian(y,-5,1));
	EXPECT_EQ(0x5e,to8BitUnsignedFromLittleEndian(y,-4,1));
	EXPECT_EQ(0x4d,to8BitUnsignedFromLittleEndian(y,-3,1));
	EXPECT_EQ(0x3c,to8BitUnsignedFromLittleEndian(y,-2,1));
	EXPECT_EQ(0x2b,to8BitUnsignedFromLittleEndian(y,-1,1));
	EXPECT_EQ(0x1a,to8BitUnsignedFromLittleEndian(y, 0,1));
	EXPECT_EQ(0,to8BitUnsignedFromLittleEndian(y,1,1));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to8BitSignedFromBigEndianNoMemory)
{
	EXPECT_EQ(0,to8BitSignedFromBigEndian(0,0,4));
}

TEST(FormatType,to8BitSignedFromBigEndianOverRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x6f,to8BitSignedFromBigEndian(y,0,6));
	EXPECT_EQ(0x5e,to8BitSignedFromBigEndian(y,1,6));
	EXPECT_EQ(0x4d,to8BitSignedFromBigEndian(y,2,6));
	EXPECT_EQ(0x3c,to8BitSignedFromBigEndian(y,3,6));
	EXPECT_EQ(0x2b,to8BitSignedFromBigEndian(y,4,6));
	EXPECT_EQ(0x1a,to8BitSignedFromBigEndian(y,5,6));
	EXPECT_EQ(0,to8BitSignedFromBigEndian(y,6,6));
}

TEST(FormatType,to8BitSignedFromBigEndianOverNegativeRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(&x[5]);

	EXPECT_EQ(0x6f,to8BitSignedFromBigEndian(y,-5,1));
	EXPECT_EQ(0x5e,to8BitSignedFromBigEndian(y,-4,1));
	EXPECT_EQ(0x4d,to8BitSignedFromBigEndian(y,-3,1));
	EXPECT_EQ(0x3c,to8BitSignedFromBigEndian(y,-2,1));
	EXPECT_EQ(0x2b,to8BitSignedFromBigEndian(y,-1,1));
	EXPECT_EQ(0x1a,to8BitSignedFromBigEndian(y, 0,1));
	EXPECT_EQ(0,to8BitSignedFromBigEndian(y,1,1));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,to8BitUnsignedFromBigEndianNoMemory)
{
	EXPECT_EQ(0,to8BitUnsignedFromBigEndian(0,0,4));
}

TEST(FormatType,to8BitUnsignedFromBigEndianOverRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(x);
	
	EXPECT_EQ(0x6f,to8BitUnsignedFromBigEndian(y,0,6));
	EXPECT_EQ(0x5e,to8BitUnsignedFromBigEndian(y,1,6));
	EXPECT_EQ(0x4d,to8BitUnsignedFromBigEndian(y,2,6));
	EXPECT_EQ(0x3c,to8BitUnsignedFromBigEndian(y,3,6));
	EXPECT_EQ(0x2b,to8BitUnsignedFromBigEndian(y,4,6));
	EXPECT_EQ(0x1a,to8BitUnsignedFromBigEndian(y,5,6));
	EXPECT_EQ(0,to8BitUnsignedFromBigEndian(y,6,6));
}

TEST(FormatType,to8BitUnsignedFromBigEndianOverNegativeRange)
{
	tubyte x[6] = { 0x6f, 0x5e, 0x4d, 0x3c, 0x2b, 0x1a };
	tchar *y = reinterpret_cast<tchar *>(&x[5]);

	EXPECT_EQ(0x6f,to8BitUnsignedFromBigEndian(y,-5,1));
	EXPECT_EQ(0x5e,to8BitUnsignedFromBigEndian(y,-4,1));
	EXPECT_EQ(0x4d,to8BitUnsignedFromBigEndian(y,-3,1));
	EXPECT_EQ(0x3c,to8BitUnsignedFromBigEndian(y,-2,1));
	EXPECT_EQ(0x2b,to8BitUnsignedFromBigEndian(y,-1,1));
	EXPECT_EQ(0x1a,to8BitUnsignedFromBigEndian(y, 0,1));
	EXPECT_EQ(0,to8BitUnsignedFromBigEndian(y,1,1));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom28BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xf0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom28BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom28BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x08 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom28BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom28BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom28BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom28BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom28BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom28BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x0c };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom28BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom28BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom28BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From28BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xf0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From28BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From28BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x08 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From28BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From28BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From28BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From28BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From28BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From28BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x0c };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From28BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From28BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From28BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom28BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xf0,
		0x00, 0x00, 0x00, 0x08,
		0xff, 0xff, 0xff, 0x07,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x0c,
		0xff, 0xff, 0xff, 0x03
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom28BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom28BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom28BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom28BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom28BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom28BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom28BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From28BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xf0,
		0x00, 0x00, 0x00, 0x08,
		0xff, 0xff, 0xff, 0x07,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x0c,
		0xff, 0xff, 0xff, 0x03
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From28BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From28BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From28BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From28BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From28BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From28BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From28BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom28BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0xf0, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom28BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom28BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x08, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom28BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom28BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x07, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom28BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom28BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom28BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom28BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x0c, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom28BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom28BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x03, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom28BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From28BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0xf0, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From28BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From28BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x08, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From28BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From28BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x07, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From28BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From28BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From28BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From28BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x0c, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From28BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From28BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x03, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From28BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom28BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xf0, 0x00, 0x00, 0x00,
		0x08, 0x00, 0x00, 0x00,
		0x07, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x0c, 0x00, 0x00, 0x00,
		0x03, 0xff, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom28BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom28BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom28BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom28BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom28BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom28BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom28BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From28BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xf0, 0x00, 0x00, 0x00,
		0x08, 0x00, 0x00, 0x00,
		0x07, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x0c, 0x00, 0x00, 0x00,
		0x03, 0xff, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From28BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From28BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From28BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From28BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From28BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From28BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From28BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom29BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xe0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom29BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom29BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x10 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom29BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom29BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom29BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom29BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom29BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom29BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x18 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom29BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom29BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom29BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From29BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xe0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From29BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From29BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x10 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From29BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From29BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From29BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From29BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From29BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From29BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x18 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From29BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From29BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From29BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom29BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xe0,
		0x00, 0x00, 0x00, 0x10,
		0xff, 0xff, 0xff, 0x0f,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x18,
		0xff, 0xff, 0xff, 0x07
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom29BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom29BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom29BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom29BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom29BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom29BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom29BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From29BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xe0,
		0x00, 0x00, 0x00, 0x10,
		0xff, 0xff, 0xff, 0x0f,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x18,
		0xff, 0xff, 0xff, 0x07
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From29BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From29BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From29BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From29BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From29BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From29BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From29BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom29BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xe0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom29BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom29BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x10, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom29BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom29BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x0f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom29BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom29BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom29BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom29BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x18, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom29BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom29BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x07, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom29BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From29BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xe0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From29BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From29BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x10, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From29BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From29BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x0f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From29BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From29BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From29BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From29BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x18, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From29BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From29BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x07, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From29BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom29BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xe0, 0x00, 0x00, 0x00,
		0x10, 0x00, 0x00, 0x00,
		0x0f, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x18, 0x00, 0x00, 0x00,
		0x07, 0xff, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom29BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom29BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom29BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom29BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom29BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom29BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom29BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From29BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xe0, 0x00, 0x00, 0x00,
		0x10, 0x00, 0x00, 0x00,
		0x0f, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x18, 0x00, 0x00, 0x00,
		0x07, 0xff, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From29BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From29BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From29BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From29BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From29BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From29BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From29BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom30BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom30BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom30BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x20 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom30BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom30BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom30BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom30BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom30BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom30BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x30 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom30BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom30BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom30BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From30BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From30BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From30BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x20 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From30BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From30BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From30BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From30BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From30BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From30BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x30 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From30BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From30BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From30BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom30BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xc0,
		0x00, 0x00, 0x00, 0x20,
		0xff, 0xff, 0xff, 0x1f,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x30,
		0xff, 0xff, 0xff, 0x0f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom30BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom30BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom30BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom30BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom30BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom30BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom30BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From30BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xc0,
		0x00, 0x00, 0x00, 0x20,
		0xff, 0xff, 0xff, 0x1f,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x30,
		0xff, 0xff, 0xff, 0x0f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From30BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From30BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From30BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From30BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From30BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From30BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From30BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom30BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0xc0, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom30BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom30BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x20, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom30BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom30BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x1f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom30BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom30BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom30BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom30BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x30, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom30BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom30BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x0f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom30BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From30BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0xc0, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From30BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From30BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x20, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From30BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From30BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x1f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From30BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From30BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From30BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From30BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x30, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From30BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From30BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x0f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From30BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom30BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xc0, 0x00, 0x00, 0x00,
		0x20, 0x00, 0x00, 0x00,
		0x1f, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x30, 0x00, 0x00, 0x00,
		0x0f, 0xff, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom30BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom30BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom30BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom30BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom30BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom30BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom30BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From30BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xc0, 0x00, 0x00, 0x00,
		0x20, 0x00, 0x00, 0x00,
		0x1f, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x30, 0x00, 0x00, 0x00,
		0x0f, 0xff, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From30BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From30BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From30BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From30BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From30BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From30BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From30BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom31BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom31BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom31BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x40 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom31BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom31BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom31BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom31BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom31BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom31BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x60 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom31BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom31BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom31BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From31BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From31BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From31BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x40 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From31BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From31BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From31BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From31BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From31BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From31BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x60 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From31BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From31BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From31BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom31BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0x80,
		0x00, 0x00, 0x00, 0x40,
		0xff, 0xff, 0xff, 0x3f,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x60,
		0xff, 0xff, 0xff, 0x1f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom31BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom31BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom31BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom31BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom31BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom31BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom31BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From31BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0x80,
		0x00, 0x00, 0x00, 0x40,
		0xff, 0xff, 0xff, 0x3f,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x60,
		0xff, 0xff, 0xff, 0x1f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From31BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From31BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From31BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From31BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From31BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From31BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From31BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom31BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x80, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom31BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom31BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x40, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom31BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom31BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x3f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom31BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom31BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom31BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom31BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x60, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom31BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom31BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x1f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom31BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From31BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x80, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From31BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From31BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x40, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From31BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From31BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x3f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From31BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From31BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From31BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From31BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x60, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From31BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From31BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x1f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From31BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom31BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0x80,
		0x40, 0x00, 0x00, 0x00,
		0x3f, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x60, 0x00, 0x00, 0x00,
		0x1f, 0xff, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom31BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom31BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom31BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom31BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom31BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom31BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom31BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From31BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0x80,
		0x40, 0x00, 0x00, 0x00,
		0x3f, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x60, 0x00, 0x00, 0x00,
		0x1f, 0xff, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From31BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From31BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From31BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From31BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From31BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From31BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From31BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom32BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom32BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom32BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x7f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom32BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom32BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom32BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom32BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom32BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom32BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom32BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From32BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From32BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From32BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x7f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From32BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From32BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From32BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From32BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From32BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From32BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From32BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom32BitsFromLittleEndianInMemory)
{
	tubyte x[20] = { 
		0x00, 0x00, 0x00, 0x80,
		0xff, 0xff, 0xff, 0x7f,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xc0,
		0xff, 0xff, 0xff, 0x3f 
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0f,toSampleFrom32BitsFromLittleEndian(mem, 0,20),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom32BitsFromLittleEndian(mem, 4,20),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom32BitsFromLittleEndian(mem, 8,20),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom32BitsFromLittleEndian(mem,12,20),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom32BitsFromLittleEndian(mem,16,20),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom32BitsFromLittleEndian(mem,20,20),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From32BitsFromLittleEndianInMemory)
{
	tubyte x[20] = { 
		0x00, 0x00, 0x00, 0x80,
		0xff, 0xff, 0xff, 0x7f,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xc0,
		0xff, 0xff, 0xff, 0x3f 
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0,toSample64From32BitsFromLittleEndian(mem, 0,20),0.000001);
	ASSERT_NEAR( 1.0,toSample64From32BitsFromLittleEndian(mem, 4,20),0.000001);
	ASSERT_NEAR( 0.0,toSample64From32BitsFromLittleEndian(mem, 8,20),0.000001);
	ASSERT_NEAR(-0.5,toSample64From32BitsFromLittleEndian(mem,12,20),0.000001);
	ASSERT_NEAR( 0.5,toSample64From32BitsFromLittleEndian(mem,16,20),0.000001);
	ASSERT_NEAR( 0.0,toSample64From32BitsFromLittleEndian(mem,20,20),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom32BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x80, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom32BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom32BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x7f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom32BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom32BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom32BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom32BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0xc0, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom32BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom32BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x3f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom32BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From32BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x80, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From32BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From32BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x7f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From32BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From32BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From32BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From32BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0xc0, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From32BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From32BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x3f, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From32BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom32BitsFromBigEndianInMemory)
{
	tubyte x[20] = { 
		0x80, 0x00, 0x00, 0x00,
		0x7f, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0xc0, 0x00, 0x00, 0x00,
		0x3f, 0xff, 0xff, 0xff 
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0f,toSampleFrom32BitsFromBigEndian(mem, 0,20),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom32BitsFromBigEndian(mem, 4,20),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom32BitsFromBigEndian(mem, 8,20),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom32BitsFromBigEndian(mem,12,20),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom32BitsFromBigEndian(mem,16,20),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom32BitsFromBigEndian(mem,20,20),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From32BitsFromBigEndianInMemory)
{
	tubyte x[20] = { 
		0x80, 0x00, 0x00, 0x00,
		0x7f, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0xc0, 0x00, 0x00, 0x00,
		0x3f, 0xff, 0xff, 0xff 
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0,toSample64From32BitsFromBigEndian(mem, 0,20),0.000001);
	ASSERT_NEAR( 1.0,toSample64From32BitsFromBigEndian(mem, 4,20),0.000001);
	ASSERT_NEAR( 0.0,toSample64From32BitsFromBigEndian(mem, 8,20),0.000001);
	ASSERT_NEAR(-0.5,toSample64From32BitsFromBigEndian(mem,12,20),0.000001);
	ASSERT_NEAR( 0.5,toSample64From32BitsFromBigEndian(mem,16,20),0.000001);
	ASSERT_NEAR( 0.0,toSample64From32BitsFromBigEndian(mem,20,20),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom1BitLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom1Bit(y),0.000001);
}

TEST(FormatType,toSampleFrom1BitUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom1Bit(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From1BitLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From1Bit(y),0.000001);
}

TEST(FormatType,toSample64From1BitUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From1Bit(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom1BitInMemory)
{
	tubyte x[2] = {
        0x00,
        0x01
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0f,toSampleFrom1Bit(mem,0,2),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom1Bit(mem,1,2),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom1Bit(mem,2,2),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From1BitInMemory)
{
	tubyte x[2] = {
        0x00,
        0x01
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0,toSample64From1Bit(mem,0,2),0.000001);
	ASSERT_NEAR( 1.0,toSample64From1Bit(mem,1,2),0.000001);
	ASSERT_NEAR(-1.0,toSample64From1Bit(mem,2,2),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom2BitsIgnoresTopBit)
{
	tubyte x[1] = { 0xfc };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom2Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom2BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x02 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom2Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom2BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom2Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom2BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom2Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom2BitsMinusHalf)
{
	tubyte x[1] = { 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom2Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From2BitsIgnoresTopBit)
{
	tubyte x[1] = { 0xfc };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From2Bits(y),0.000001);
}

TEST(FormatType,toSample64From2BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x02 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From2Bits(y),0.000001);
}

TEST(FormatType,toSample64From2BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From2Bits(y),0.000001);
}

TEST(FormatType,toSample64From2BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From2Bits(y),0.000001);
}

TEST(FormatType,toSample64From2BitsMinusHalf)
{
	tubyte x[1] = { 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From2Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom2BitsInMemory)
{
    tubyte x[5] = {
		0xfc,
		0x02,
		0x01,
		0xff,
		0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom2Bits(mem,0,4),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom2Bits(mem,1,4),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom2Bits(mem,2,4),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom2Bits(mem,3,4),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom2Bits(mem,4,4),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom2Bits(mem,5,4),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From2BitsInMemory)
{
    tubyte x[5] = {
		0xfc,
		0x02,
		0x01,
		0xff,
		0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From2Bits(mem,0,4),0.000001);
	ASSERT_NEAR(-1.0,toSample64From2Bits(mem,1,4),0.000001);
	ASSERT_NEAR( 1.0,toSample64From2Bits(mem,2,4),0.000001);
	ASSERT_NEAR(-0.5,toSample64From2Bits(mem,3,4),0.000001);
	ASSERT_NEAR( 0.0,toSample64From2Bits(mem,4,4),0.000001);
	ASSERT_NEAR( 0.0,toSample64From2Bits(mem,5,4),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom3BitsIgnoresTopBit)
{
	tubyte x[1] = { 0xf8 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom3Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom3BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x04 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom3Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom3BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom3Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom3BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom3Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom3BitsNegativeHalf)
{
	tubyte x[1] = { 0x06 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom3Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom3BitsPositiveHalf)
{
	tubyte x[2] = { 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.333333333f,toSampleFrom3Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From3BitsIgnoresTopBit)
{
	tubyte x[1] = { 0xf8 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From3Bits(y),0.000001);
}

TEST(FormatType,toSample64From3BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x04 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From3Bits(y),0.000001);
}

TEST(FormatType,toSample64From3BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From3Bits(y),0.000001);
}

TEST(FormatType,toSample64From3BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From3Bits(y),0.000001);
}

TEST(FormatType,toSample64From3BitsNegativeHalf)
{
	tubyte x[1] = { 0x06 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From3Bits(y),0.000001);
}

TEST(FormatType,toSample64From3BitsPositiveHalf)
{
	tubyte x[2] = { 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.333333333,toSample64From3Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom3BitsInMemory)
{
	tubyte x[6] = {
		0xf8,
		0x04,
		0x03,
		0x00,
		0x06,
		0x01
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom3Bits(mem,0,6),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom3Bits(mem,1,6),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom3Bits(mem,2,6),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom3Bits(mem,3,6),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom3Bits(mem,4,6),0.000001);
	ASSERT_NEAR( 0.333333333f,toSampleFrom3Bits(mem,5,6),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom3Bits(mem,6,6),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From3BitsInMemory)
{
	tubyte x[6] = {
		0xf8,
		0x04,
		0x03,
		0x00,
		0x06,
		0x01
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From3Bits(mem,0,6),0.000001);
	ASSERT_NEAR(-1.0,toSample64From3Bits(mem,1,6),0.000001);
	ASSERT_NEAR( 1.0,toSample64From3Bits(mem,2,6),0.000001);
	ASSERT_NEAR( 0.0,toSample64From3Bits(mem,3,6),0.000001);
	ASSERT_NEAR(-0.5,toSample64From3Bits(mem,4,6),0.000001);
	ASSERT_NEAR( 0.333333333,toSample64From3Bits(mem,5,6),0.000001);
	ASSERT_NEAR( 0.0,toSample64From3Bits(mem,6,6),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom4BitsIgnoresTopBit)
{
	tubyte x[1] = { 0xf0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom4Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom4BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x08 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom4Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom4BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom4Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom4BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom4Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom4BitsNegativeHalf)
{
	tubyte x[1] = { 0x0c };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom4Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom4BitsPositiveHalf)
{
	tubyte x[2] = { 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.42857143f,toSampleFrom4Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From4BitsIgnoresTopBit)
{
	tubyte x[1] = { 0xf0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From4Bits(y),0.000001);
}

TEST(FormatType,toSample64From4BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x08 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From4Bits(y),0.000001);
}

TEST(FormatType,toSample64From4BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From4Bits(y),0.000001);
}

TEST(FormatType,toSample64From4BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From4Bits(y),0.000001);
}

TEST(FormatType,toSample64From4BitsNegativeHalf)
{
	tubyte x[1] = { 0x0c };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From4Bits(y),0.000001);
}

TEST(FormatType,toSample64From4BitsPositiveHalf)
{
	tubyte x[2] = { 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.42857143,toSample64From4Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom4BitsInMemory)
{
	tubyte x[6] = {
		0xf0,
		0x08,
		0x07,
		0x00,
		0x0c,
		0x03
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom4Bits(mem,0,6),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom4Bits(mem,1,6),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom4Bits(mem,2,6),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom4Bits(mem,3,6),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom4Bits(mem,4,6),0.000001);
	ASSERT_NEAR( 0.42857143f,toSampleFrom4Bits(mem,5,6),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom4Bits(mem,6,6),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From4BitsInMemory)
{
	tubyte x[6] = {
		0xf0,
		0x08,
		0x07,
		0x00,
		0x0c,
		0x03
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From4Bits(mem,0,6),0.000001);
	ASSERT_NEAR(-1.0,toSample64From4Bits(mem,1,6),0.000001);
	ASSERT_NEAR( 1.0,toSample64From4Bits(mem,2,6),0.000001);
	ASSERT_NEAR( 0.0,toSample64From4Bits(mem,3,6),0.000001);
	ASSERT_NEAR(-0.5,toSample64From4Bits(mem,4,6),0.000001);
	ASSERT_NEAR( 0.42857143,toSample64From4Bits(mem,5,6),0.000001);
	ASSERT_NEAR( 0.0,toSample64From4Bits(mem,6,6),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom5BitsIgnoresTopBit)
{
	tubyte x[1] = { 0xe0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom5Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom5BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x10 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom5Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom5BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom5Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom5BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom5Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom5BitsNegativeHalf)
{
	tubyte x[1] = { 0x18 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom5Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom5BitsPositiveHalf)
{
	tubyte x[2] = { 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.46666667f,toSampleFrom5Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From5BitsIgnoresTopBit)
{
	tubyte x[1] = { 0xe0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From5Bits(y),0.000001);
}

TEST(FormatType,toSample64From5BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x10 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From5Bits(y),0.000001);
}

TEST(FormatType,toSample64From5BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From5Bits(y),0.000001);
}

TEST(FormatType,toSample64From5BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From5Bits(y),0.000001);
}

TEST(FormatType,toSample64From5BitsNegativeHalf)
{
	tubyte x[1] = { 0x18 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From5Bits(y),0.000001);
}

TEST(FormatType,toSample64From5BitsPositiveHalf)
{
	tubyte x[2] = { 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.46666667,toSample64From5Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom5BitsInMemory)
{
	tubyte x[6] = {
		0xe0,
		0x10,
		0x0f,
		0x00,
		0x18,
		0x07
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom5Bits(mem,0,6),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom5Bits(mem,1,6),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom5Bits(mem,2,6),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom5Bits(mem,3,6),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom5Bits(mem,4,6),0.000001);
	ASSERT_NEAR( 0.46666667f,toSampleFrom5Bits(mem,5,6),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom5Bits(mem,6,6),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From5BitsInMemory)
{
	tubyte x[6] = {
		0xe0,
		0x10,
		0x0f,
		0x00,
		0x18,
		0x07
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From5Bits(mem,0,6),0.000001);
	ASSERT_NEAR(-1.0,toSample64From5Bits(mem,1,6),0.000001);
	ASSERT_NEAR( 1.0,toSample64From5Bits(mem,2,6),0.000001);
	ASSERT_NEAR( 0.0,toSample64From5Bits(mem,3,6),0.000001);
	ASSERT_NEAR(-0.5,toSample64From5Bits(mem,4,6),0.000001);
	ASSERT_NEAR( 0.46666667,toSample64From5Bits(mem,5,6),0.000001);
	ASSERT_NEAR( 0.0,toSample64From5Bits(mem,6,6),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom6BitsIgnoresTopBit)
{
	tubyte x[1] = { 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom6Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom6BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x20 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom6Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom6BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom6Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom6BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom6Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom6BitsNegativeHalf)
{
	tubyte x[1] = { 0x30 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom6Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom6BitsPositiveHalf)
{
	tubyte x[2] = { 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.48387097f,toSampleFrom6Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSample64From6BitsIgnoresTopBit)
{
	tubyte x[1] = { 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From6Bits(y),0.000001);
}

TEST(FormatType,toSample64From6BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x20 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From6Bits(y),0.000001);
}

TEST(FormatType,toSample64From6BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From6Bits(y),0.000001);
}

TEST(FormatType,toSample64From6BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From6Bits(y),0.000001);
}

TEST(FormatType,toSample64From6BitsNegativeHalf)
{
	tubyte x[1] = { 0x30 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From6Bits(y),0.000001);
}

TEST(FormatType,toSample64From6BitsPositiveHalf)
{
	tubyte x[2] = { 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.48387097,toSample64From6Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom6BitsInMemory)
{
	tubyte x[6] = {
		0xc0,
		0x20,
		0x1f,
		0x00,
		0x30,
		0x0f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom6Bits(mem,0,6),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom6Bits(mem,1,6),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom6Bits(mem,2,6),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom6Bits(mem,3,6),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom6Bits(mem,4,6),0.000001);
	ASSERT_NEAR( 0.48387097f,toSampleFrom6Bits(mem,5,6),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom6Bits(mem,6,6),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSample64From6BitsInMemory)
{
	tubyte x[6] = {
		0xc0,
		0x20,
		0x1f,
		0x00,
		0x30,
		0x0f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From6Bits(mem,0,6),0.000001);
	ASSERT_NEAR(-1.0,toSample64From6Bits(mem,1,6),0.000001);
	ASSERT_NEAR( 1.0,toSample64From6Bits(mem,2,6),0.000001);
	ASSERT_NEAR( 0.0,toSample64From6Bits(mem,3,6),0.000001);
	ASSERT_NEAR(-0.5,toSample64From6Bits(mem,4,6),0.000001);
	ASSERT_NEAR( 0.48387097,toSample64From6Bits(mem,5,6),0.000001);
	ASSERT_NEAR( 0.0,toSample64From6Bits(mem,6,6),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom7BitsIgnoresTopBit)
{
	tubyte x[1] = { 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom7Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom7BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x40 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom7Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom7BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom7Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom7BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom7Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom7BitsNegativeHalf)
{
	tubyte x[1] = { 0x60 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom7Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom7BitsPositiveHalf)
{
	tubyte x[2] = { 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49206349f,toSampleFrom7Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From7BitsIgnoresTopBit)
{
	tubyte x[1] = { 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From7Bits(y),0.000001);
}

TEST(FormatType,toSample64From7BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x40 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From7Bits(y),0.000001);
}

TEST(FormatType,toSample64From7BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From7Bits(y),0.000001);
}

TEST(FormatType,toSample64From7BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From7Bits(y),0.000001);
}

TEST(FormatType,toSample64From7BitsNegativeHalf)
{
	tubyte x[1] = { 0x60 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From7Bits(y),0.000001);
}

TEST(FormatType,toSample64From7BitsPositiveHalf)
{
	tubyte x[2] = { 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49206349,toSample64From7Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom7BitsInMemory)
{
	tubyte x[6] = {
		0x80,
		0x40,
		0x3f,
		0x00,
		0x60,
		0x1f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom7Bits(mem,0,6),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom7Bits(mem,1,6),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom7Bits(mem,2,6),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom7Bits(mem,3,6),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom7Bits(mem,4,6),0.000001);
	ASSERT_NEAR( 0.49206349f,toSampleFrom7Bits(mem,5,6),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom7Bits(mem,6,6),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From7BitsInMemory)
{
	tubyte x[6] = {
		0x80,
		0x40,
		0x3f,
		0x00,
		0x60,
		0x1f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From7Bits(mem,0,6),0.000001);
	ASSERT_NEAR(-1.0,toSample64From7Bits(mem,1,6),0.000001);
	ASSERT_NEAR( 1.0,toSample64From7Bits(mem,2,6),0.000001);
	ASSERT_NEAR( 0.0,toSample64From7Bits(mem,3,6),0.000001);
	ASSERT_NEAR(-0.5,toSample64From7Bits(mem,4,6),0.000001);
	ASSERT_NEAR( 0.49206349,toSample64From7Bits(mem,5,6),0.000001);
	ASSERT_NEAR( 0.0,toSample64From7Bits(mem,6,6),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom8BitsIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom8Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom8BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom8Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom8BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x7f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom8Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom8BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom8Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom8BitsNegativeHalf)
{
	tubyte x[1] = { 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom8Bits(y),0.000001);
}

TEST(FormatType,toSampleFrom8BitsPositiveHalf)
{
	tubyte x[2] = { 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49606299f,toSampleFrom8Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From8BitsIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From8Bits(y),0.000001);
}

TEST(FormatType,toSample64From8BitsLowerLimitIsMinusOne)
{
	tubyte x[1] = { 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From8Bits(y),0.000001);
}

TEST(FormatType,toSample64From8BitsUpperLimitIsPlusOne)
{
	tubyte x[1] = { 0x7f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From8Bits(y),0.000001);
}

TEST(FormatType,toSample64From8BitsZeroIsZero)
{
	tubyte x[1] = { 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From8Bits(y),0.000001);
}

TEST(FormatType,toSample64From8BitsNegativeHalf)
{
	tubyte x[1] = { 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From8Bits(y),0.000001);
}

TEST(FormatType,toSample64From8BitsPositiveHalf)
{
	tubyte x[2] = { 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49606299,toSample64From8Bits(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom8BitsInMemory)
{
	tubyte x[5] = {
		0x80,
		0x7f,
		0x00,
		0xc0,
		0x3f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0f,toSampleFrom8Bits(mem,0,5),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom8Bits(mem,1,5),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom8Bits(mem,2,5),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom8Bits(mem,3,5),0.000001);
	ASSERT_NEAR( 0.49606299f,toSampleFrom8Bits(mem,4,5),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom8Bits(mem,5,5),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From8BitsInMemory)
{
	tubyte x[5] = {
		0x80,
		0x7f,
		0x00,
		0xc0,
		0x3f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0,toSample64From8Bits(mem,0,5),0.000001);
	ASSERT_NEAR( 1.0,toSample64From8Bits(mem,1,5),0.000001);
	ASSERT_NEAR( 0.0,toSample64From8Bits(mem,2,5),0.000001);
	ASSERT_NEAR(-0.5,toSample64From8Bits(mem,3,5),0.000001);
	ASSERT_NEAR( 0.49606299,toSample64From8Bits(mem,4,5),0.000001);
	ASSERT_NEAR( 0.0,toSample64From8Bits(mem,5,5),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom9BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xfe };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom9BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom9BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom9BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom9BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom9BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom9BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom9BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom9BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x80, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom9BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom9BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0x7f, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49803922f,toSampleFrom9BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64 

TEST(FormatType,toSample64From9BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xfe };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From9BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From9BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From9BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From9BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From9BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From9BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From9BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From9BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x80, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From9BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From9BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0x7f, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49803922,toSample64From9BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom9BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xfe,
		0x00, 0x01,
		0xff, 0x00,
		0x00, 0x00,
		0x80, 0x01,
		0x7f, 0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom9BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom9BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom9BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom9BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom9BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49803922f,toSampleFrom9BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom9BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From9BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xfe,
		0x00, 0x01,
		0xff, 0x00,
		0x00, 0x00,
		0x80, 0x01,
		0x7f, 0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From9BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From9BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From9BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From9BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From9BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49803922,toSample64From9BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From9BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom9BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xfe, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom9BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom9BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x01, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom9BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom9BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom9BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom9BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom9BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom9BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x01, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom9BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom9BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x00, 0x7f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49803922f,toSampleFrom9BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From9BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xfe, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From9BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From9BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x01, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From9BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From9BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From9BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From9BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From9BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From9BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x01, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From9BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From9BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x00, 0x7f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49803922,toSample64From9BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom9BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xfe, 0x00,
		0x01, 0x00,
		0x00, 0xff,
		0x00, 0x00,
		0x01, 0x80,
		0x00, 0x7f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom9BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom9BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom9BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom9BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom9BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49803922f,toSampleFrom9BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom9BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSample64From9BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xfe, 0x00,
		0x01, 0x00,
		0x00, 0xff,
		0x00, 0x00,
		0x01, 0x80,
		0x00, 0x7f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From9BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From9BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From9BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From9BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From9BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49803922,toSample64From9BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From9BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom10BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xfc };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom10BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom10BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x02 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom10BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom10BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom10BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom10BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom10BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom10BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom10BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom10BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49902153f,toSampleFrom10BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From10BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xfc };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From10BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From10BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x02 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From10BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From10BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From10BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From10BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From10BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From10BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From10BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From10BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49902153,toSample64From10BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom10BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xfc,
		0x00, 0x02,
		0xff, 0x01,
		0x00, 0x00,
		0x00, 0x03,
		0xff, 0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom10BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom10BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom10BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom10BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom10BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49902153f,toSampleFrom10BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom10BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From10BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xfc,
		0x00, 0x02,
		0xff, 0x01,
		0x00, 0x00,
		0x00, 0x03,
		0xff, 0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From10BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From10BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From10BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From10BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From10BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49902153,toSample64From10BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From10BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom10BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xfc, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom10BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom10BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x02, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom10BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom10BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x01, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom10BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom10BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom10BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom10BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x03, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom10BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom10BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49902153f,toSampleFrom10BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From10BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xfc, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From10BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From10BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x02, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From10BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From10BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x01, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From10BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From10BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From10BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From10BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x03, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From10BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From10BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49902153,toSample64From10BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom10BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xfc, 0x00,
		0x02, 0x00,
		0x01, 0xff,
		0x00, 0x00,
		0x03, 0x00,
		0x00, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom10BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom10BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom10BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom10BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom10BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49902153f,toSampleFrom10BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom10BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From10BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xfc, 0x00,
		0x02, 0x00,
		0x01, 0xff,
		0x00, 0x00,
		0x03, 0x00,
		0x00, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From10BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From10BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From10BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From10BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From10BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49902153,toSample64From10BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From10BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom11BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xf8 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom11BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom11BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x04 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom11BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom11BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom11BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom11BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom11BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom11BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x06 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom11BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom11BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49951124f,toSampleFrom11BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From11BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xf8 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From11BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From11BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x04 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From11BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From11BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From11BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From11BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From11BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From11BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x06 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From11BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From11BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49951124,toSample64From11BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom11BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xf8,
		0x00, 0x04,
		0xff, 0x03,
		0x00, 0x00,
		0x00, 0x06,
		0xff, 0x01
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom11BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom11BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom11BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom11BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom11BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49951124f,toSampleFrom11BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom11BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From11BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xf8,
		0x00, 0x04,
		0xff, 0x03,
		0x00, 0x00,
		0x00, 0x06,
		0xff, 0x01
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From11BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From11BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From11BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From11BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From11BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49951124,toSample64From11BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From11BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom11BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xf8, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom11BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom11BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x04, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom11BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom11BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x03, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom11BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom11BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom11BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom11BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x06, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom11BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom11BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x01, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49951124f,toSampleFrom11BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From11BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xf8, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From11BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From11BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x04, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From11BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From11BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x03, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From11BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From11BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From11BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From11BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x06, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From11BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From11BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x01, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49951124,toSample64From11BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom11BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xf8, 0x00,
		0x04, 0x00,
		0x03, 0xff,
		0x00, 0x00,
		0x06, 0x00,
		0x01, 0xff
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom11BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom11BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom11BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom11BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom11BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49951124f,toSampleFrom11BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom11BitsFromBigEndian(mem,12,12),0.000001);
}
//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From11BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xf8, 0x00,
		0x04, 0x00,
		0x03, 0xff,
		0x00, 0x00,
		0x06, 0x00,
		0x01, 0xff
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From11BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From11BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From11BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From11BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From11BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49951124,toSample64From11BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From11BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom12BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xf0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom12BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom12BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x08 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom12BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom12BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom12BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom12BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom12BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom12BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x0c };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom12BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom12BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49975574f,toSampleFrom12BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From12BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xf0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From12BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From12BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x08 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From12BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From12BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From12BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From12BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From12BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From12BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x0c };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From12BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From12BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49975574,toSample64From12BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom12BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xf0,
		0x00, 0x08,
		0xff, 0x07,
		0x00, 0x00,
		0x00, 0x0c,
		0xff, 0x03
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom12BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom12BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom12BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom12BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom12BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49975574f,toSampleFrom12BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom12BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From12BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xf0,
		0x00, 0x08,
		0xff, 0x07,
		0x00, 0x00,
		0x00, 0x0c,
		0xff, 0x03
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From12BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From12BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From12BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From12BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From12BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49975574,toSample64From12BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From12BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom12BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xf0, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom12BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom12BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x08, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom12BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom12BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x07, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom12BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom12BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom12BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom12BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x0c, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom12BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom12BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x03, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49975574f,toSampleFrom12BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
//64

TEST(FormatType,toSample64From12BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xf0, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From12BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From12BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x08, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From12BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From12BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x07, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From12BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From12BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From12BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From12BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x0c, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From12BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From12BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x03, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49975574,toSample64From12BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom12BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xf0, 0x00,
		0x08, 0x00,
		0x07, 0xff,
		0x00, 0x00,
		0x0c, 0x00,
		0x03, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom12BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom12BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom12BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom12BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom12BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49975574f,toSampleFrom12BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom12BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSampl64eFrom12BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xf0, 0x00,
		0x08, 0x00,
		0x07, 0xff,
		0x00, 0x00,
		0x0c, 0x00,
		0x03, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From12BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From12BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From12BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From12BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From12BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49975574,toSample64From12BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From12BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom13BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xe0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom13BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom13BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x10 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom13BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom13BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom13BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom13BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom13BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom13BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x18 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom13BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom13BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49987790f,toSampleFrom13BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From13BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xe0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From13BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From13BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x10 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From13BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From13BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From13BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From13BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From13BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From13BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x18 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From13BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From13BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49987790,toSample64From13BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom13BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xe0,
		0x00, 0x10,
		0xff, 0x0f,
		0x00, 0x00,
		0x00, 0x18,
		0xff, 0x07
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom13BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom13BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom13BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom13BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom13BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49987790f,toSampleFrom13BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom13BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From13BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xe0,
		0x00, 0x10,
		0xff, 0x0f,
		0x00, 0x00,
		0x00, 0x18,
		0xff, 0x07
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From13BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From13BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From13BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From13BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From13BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49987790,toSample64From13BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From13BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom13BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xe0, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom13BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom13BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x10, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom13BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom13BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x0f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom13BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom13BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom13BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom13BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x18, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom13BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom13BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x07, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49987790f,toSampleFrom13BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From13BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xe0, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From13BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From13BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x10, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From13BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From13BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x0f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From13BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From13BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From13BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From13BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x18, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From13BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From13BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x07, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49987790,toSample64From13BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom13BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xe0, 0x00,
		0x10, 0x00,
		0x0f, 0xff,
		0x00, 0x00,
		0x18, 0x00,
		0x07, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom13BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom13BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom13BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom13BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom13BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49987790f,toSampleFrom13BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom13BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSample64From13BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xe0, 0x00,
		0x10, 0x00,
		0x0f, 0xff,
		0x00, 0x00,
		0x18, 0x00,
		0x07, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From13BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From13BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From13BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From13BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From13BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49987790,toSample64From13BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From13BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom14BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom14BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom14BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x20 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom14BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom14BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom14BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom14BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom14BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom14BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x30 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom14BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom14BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49993896f,toSampleFrom14BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From14BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From14BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From14BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x20 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From14BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From14BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From14BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From14BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From14BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From14BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x30 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From14BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From14BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49993896,toSample64From14BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom14BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xc0,
		0x00, 0x20,
		0xff, 0x1f,
		0x00, 0x00,
		0x00, 0x30,
		0xff, 0x0f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom14BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom14BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom14BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom14BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom14BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49993896f,toSampleFrom14BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom14BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From14BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0xc0,
		0x00, 0x20,
		0xff, 0x1f,
		0x00, 0x00,
		0x00, 0x30,
		0xff, 0x0f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From14BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From14BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From14BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From14BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From14BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49993896,toSample64From14BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From14BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom14BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xc0, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom14BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom14BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x20, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom14BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom14BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x1f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom14BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom14BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom14BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom14BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x30, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom14BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom14BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x0f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49993896f,toSampleFrom14BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From14BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[2] = { 0xc0, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From14BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From14BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x20, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From14BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From14BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x1f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From14BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From14BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From14BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From14BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x30, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From14BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From14BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x0f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49993896,toSample64From14BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom14BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xc0, 0x00,
		0x20, 0x00,
		0x1f, 0xff,
		0x00, 0x00,
		0x30, 0x00,
		0x0f, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom14BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom14BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom14BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom14BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom14BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49993896f,toSampleFrom14BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom14BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From14BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0xc0, 0x00,
		0x20, 0x00,
		0x1f, 0xff,
		0x00, 0x00,
		0x30, 0x00,
		0x0f, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From14BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From14BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From14BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From14BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From14BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49993896,toSample64From14BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From14BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom15BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom15BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom15BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x40 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom15BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom15BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom15BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom15BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom15BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom15BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x60 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom15BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom15BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49996948f,toSampleFrom15BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From15BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[2] = { 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From15BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From15BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x40 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From15BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From15BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From15BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From15BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From15BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From15BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0x60 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From15BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From15BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49996948,toSample64From15BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom15BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0x80,
		0x00, 0x40,
		0xff, 0x3f,
		0x00, 0x00,
		0x00, 0x60,
		0xff, 0x1f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom15BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom15BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom15BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom15BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom15BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49996948f,toSampleFrom15BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom15BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From15BitsFromLittleEndianInMemory)
{
	tubyte x[12] = {
		0x00, 0x80,
		0x00, 0x40,
		0xff, 0x3f,
		0x00, 0x00,
		0x00, 0x60,
		0xff, 0x1f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From15BitsFromLittleEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From15BitsFromLittleEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From15BitsFromLittleEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From15BitsFromLittleEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From15BitsFromLittleEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49996948,toSample64From15BitsFromLittleEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From15BitsFromLittleEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom15BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x80, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom15BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom15BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x40, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom15BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom15BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x3f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom15BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom15BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom15BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom15BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x60, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom15BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom15BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x1f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49996948f,toSampleFrom15BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From15BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x80, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From15BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From15BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x40, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From15BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From15BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x3f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From15BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From15BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From15BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From15BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0x60, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From15BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From15BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x1f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49996948,toSample64From15BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom15BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0x80, 0x00,
		0x40, 0x00,
		0x3f, 0xff,
		0x00, 0x00,
		0x60, 0x00,
		0x1f, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom15BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom15BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom15BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom15BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom15BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49996948f,toSampleFrom15BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom15BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From15BitsFromBigEndianInMemory)
{
	tubyte x[12] = {
		0x80, 0x00,
		0x40, 0x00,
		0x3f, 0xff,
		0x00, 0x00,
		0x60, 0x00,
		0x1f, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From15BitsFromBigEndian(mem, 0,12),0.000001);
	ASSERT_NEAR(-1.0,toSample64From15BitsFromBigEndian(mem, 2,12),0.000001);
	ASSERT_NEAR( 1.0,toSample64From15BitsFromBigEndian(mem, 4,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From15BitsFromBigEndian(mem, 6,12),0.000001);
	ASSERT_NEAR(-0.5,toSample64From15BitsFromBigEndian(mem, 8,12),0.000001);
	ASSERT_NEAR( 0.49996948,toSample64From15BitsFromBigEndian(mem,10,12),0.000001);
	ASSERT_NEAR( 0.0,toSample64From15BitsFromBigEndian(mem,12,12),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom16BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom16BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom16BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom16BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom16BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x7f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom16BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom16BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom16BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom16BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom16BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom16BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49998474f,toSampleFrom16BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From16BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From16BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From16BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From16BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From16BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0xff, 0x7f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From16BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From16BitsFromLittleEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From16BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From16BitsFromLittleEndianNegativeHalf)
{
	tubyte x[2] = { 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From16BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From16BitsFromLittleEndianPositiveHalf)
{
	tubyte x[2] = { 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49998474,toSample64From16BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom16BitsFromLittleEndianInMemory)
{
	tubyte x[10] = {
		0x00, 0x80,
		0xff, 0x7f,
		0x00, 0x00,
		0x00, 0xc0,
		0xff, 0x3f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0f,toSampleFrom16BitsFromLittleEndian(mem, 0,10),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom16BitsFromLittleEndian(mem, 2,10),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom16BitsFromLittleEndian(mem, 4,10),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom16BitsFromLittleEndian(mem, 6,10),0.000001);
	ASSERT_NEAR( 0.49998474f,toSampleFrom16BitsFromLittleEndian(mem, 8,10),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom16BitsFromLittleEndian(mem,10,10),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From16BitsFromLittleEndianInMemory)
{
	tubyte x[10] = {
		0x00, 0x80,
		0xff, 0x7f,
		0x00, 0x00,
		0x00, 0xc0,
		0xff, 0x3f
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0,toSample64From16BitsFromLittleEndian(mem, 0,10),0.000001);
	ASSERT_NEAR( 1.0,toSample64From16BitsFromLittleEndian(mem, 2,10),0.000001);
	ASSERT_NEAR( 0.0,toSample64From16BitsFromLittleEndian(mem, 4,10),0.000001);
	ASSERT_NEAR(-0.5,toSample64From16BitsFromLittleEndian(mem, 6,10),0.000001);
	ASSERT_NEAR( 0.49998474,toSample64From16BitsFromLittleEndian(mem, 8,10),0.000001);
	ASSERT_NEAR( 0.0,toSample64From16BitsFromLittleEndian(mem,10,10),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom16BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom16BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom16BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x80, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom16BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom16BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x7f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom16BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom16BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom16BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom16BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0xc0, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom16BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom16BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x3f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49998474f,toSampleFrom16BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From16BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From16BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From16BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[2] = { 0x80, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From16BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From16BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[2] = { 0x7f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From16BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From16BitsFromBigEndianZeroIsZero)
{
	tubyte x[2] = { 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From16BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From16BitsFromBigEndianNegativeHalf)
{
	tubyte x[2] = { 0xc0, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From16BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From16BitsFromBigEndianPositiveHalf)
{
	tubyte x[2] = { 0x3f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49998474,toSample64From16BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom16BitsFromBigEndianInMemory)
{
	tubyte x[10] = {
		0x80, 0x00,
		0x7f, 0xff,
		0x00, 0x00,
		0xc0, 0x00,
		0x3f, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0f,toSampleFrom16BitsFromBigEndian(mem, 0,10),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom16BitsFromBigEndian(mem, 2,10),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom16BitsFromBigEndian(mem, 4,10),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom16BitsFromBigEndian(mem, 6,10),0.000001);
	ASSERT_NEAR( 0.49998474f,toSampleFrom16BitsFromBigEndian(mem, 8,10),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom16BitsFromBigEndian(mem,10,10),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From16BitsFromBigEndianInMemory)
{
	tubyte x[10] = {
		0x80, 0x00,
		0x7f, 0xff,
		0x00, 0x00,
		0xc0, 0x00,
		0x3f, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0,toSample64From16BitsFromBigEndian(mem, 0,10),0.000001);
	ASSERT_NEAR( 1.0,toSample64From16BitsFromBigEndian(mem, 2,10),0.000001);
	ASSERT_NEAR( 0.0,toSample64From16BitsFromBigEndian(mem, 4,10),0.000001);
	ASSERT_NEAR(-0.5,toSample64From16BitsFromBigEndian(mem, 6,10),0.000001);
	ASSERT_NEAR( 0.49998474,toSample64From16BitsFromBigEndian(mem, 8,10),0.000001);
	ASSERT_NEAR( 0.0,toSample64From16BitsFromBigEndian(mem,10,10),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom17BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xfe };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom17BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom17BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom17BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom17BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom17BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom17BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom17BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom17BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x80, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom17BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom17BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0x7f, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999237f,toSampleFrom17BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From17BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xfe };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From17BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From17BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From17BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From17BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From17BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From17BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From17BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From17BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x80, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From17BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From17BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0x7f, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999237,toSample64From17BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom17BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xfe,
		0x00, 0x00, 0x01,
		0xff, 0xff, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x80, 0x01,
		0xff, 0x7f, 0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom17BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom17BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom17BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom17BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom17BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999237f,toSampleFrom17BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom17BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From17BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xfe,
		0x00, 0x00, 0x01,
		0xff, 0xff, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x80, 0x01,
		0xff, 0x7f, 0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From17BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From17BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From17BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From17BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From17BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999237,toSample64From17BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From17BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom17BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xfe, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom17BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom17BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x01, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom17BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom17BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x00, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom17BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom17BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom17BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom17BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x01, 0x80, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom17BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom17BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x00, 0x7f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999237f,toSampleFrom17BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From17BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xfe, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From17BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From17BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x01, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From17BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From17BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x00, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From17BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From17BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From17BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From17BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x01, 0x80, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From17BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From17BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x00, 0x7f, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999237,toSample64From17BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom17BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xfe, 0x00, 0x00,
		0x01, 0x00, 0x00,
		0x00, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x01, 0x80, 0x00,
		0x00, 0x7f, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom17BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom17BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom17BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom17BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom17BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999237f,toSampleFrom17BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom17BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From17BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xfe, 0x00, 0x00,
		0x01, 0x00, 0x00,
		0x00, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x01, 0x80, 0x00,
		0x00, 0x7f, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From17BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From17BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From17BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From17BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From17BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999237,toSample64From17BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From17BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom18BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xfc };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom18BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom18BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x02 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom18BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom18BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom18BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom18BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom18BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom18BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom18BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom18BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999619f,toSampleFrom18BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From18BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xfc };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From18BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From18BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x02 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From18BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From18BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From18BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From18BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From18BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From18BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From18BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From18BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999619,toSample64From18BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom18BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xfc,
		0x00, 0x00, 0x02,
		0xff, 0xff, 0x01,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x03,
		0xff, 0xff, 0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom18BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom18BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom18BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom18BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom18BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999619f,toSampleFrom18BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom18BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From18BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xfc,
		0x00, 0x00, 0x02,
		0xff, 0xff, 0x01,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x03,
		0xff, 0xff, 0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From18BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From18BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From18BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From18BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From18BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999619,toSample64From18BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From18BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom18BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xfc, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom18BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom18BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x02, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom18BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom18BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x01, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom18BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom18BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom18BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom18BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x03, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom18BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom18BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x00, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999619f,toSampleFrom18BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From18BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xfc, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From18BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From18BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x02, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From18BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From18BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x01, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From18BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From18BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From18BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From18BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x03, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From18BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From18BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x00, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999619,toSample64From18BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom18BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xfc, 0x00, 0x00,
		0x02, 0x00, 0x00,
		0x01, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x03, 0x00, 0x00,
		0x00, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom18BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom18BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom18BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom18BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom18BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999619f,toSampleFrom18BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom18BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From18BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xfc, 0x00, 0x00,
		0x02, 0x00, 0x00,
		0x01, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x03, 0x00, 0x00,
		0x00, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From18BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From18BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From18BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From18BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From18BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999619,toSample64From18BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From18BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom19BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xf8 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom19BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom19BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x04 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom19BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom19BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom19BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom19BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom19BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom19BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x06 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom19BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom19BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999809f,toSampleFrom19BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From19BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xf8 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From19BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From19BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x04 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From19BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From19BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From19BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From19BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From19BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From19BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x06 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From19BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From19BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999809,toSample64From19BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom19BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xf8,
		0x00, 0x00, 0x04,
		0xff, 0xff, 0x03,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x06,
		0xff, 0xff, 0x01
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom19BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom19BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom19BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom19BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom19BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999809f,toSampleFrom19BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom19BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From19BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xf8,
		0x00, 0x00, 0x04,
		0xff, 0xff, 0x03,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x06,
		0xff, 0xff, 0x01
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From19BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From19BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From19BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From19BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From19BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999809,toSample64From19BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From19BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom19BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xf8, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom19BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom19BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x04, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom19BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom19BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x03, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom19BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom19BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom19BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom19BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x06, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom19BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom19BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x01, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999809f,toSampleFrom19BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From19BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xf8, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From19BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From19BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x04, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From19BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From19BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x03, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From19BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From19BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From19BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From19BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x06, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From19BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From19BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x01, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.49999809,toSample64From19BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom19BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xf8, 0x00, 0x00,
		0x04, 0x00, 0x00,
		0x03, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x06, 0x00, 0x00,
		0x01, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom19BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom19BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom19BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom19BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom19BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999809f,toSampleFrom19BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom19BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From19BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xf8, 0x00, 0x00,
		0x04, 0x00, 0x00,
		0x03, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x06, 0x00, 0x00,
		0x01, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From19BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From19BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From19BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From19BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From19BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.49999809,toSample64From19BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From19BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom20BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xf0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom20BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom20BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x08 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom20BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom20BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom20BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom20BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom20BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom20BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x0c };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom20BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom20BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom20BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
//64

TEST(FormatType,toSample64From20BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xf0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From20BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From20BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x08 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From20BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From20BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From20BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From20BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From20BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From20BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x0c };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From20BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From20BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From20BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom20BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xf0,
		0x00, 0x00, 0x08,
		0xff, 0xff, 0x07,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x0c,
		0xff, 0xff, 0x03
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);

	ASSERT_NEAR( 0.0f,toSampleFrom20BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom20BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom20BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom20BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom20BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom20BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom20BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From20BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xf0,
		0x00, 0x00, 0x08,
		0xff, 0xff, 0x07,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x0c,
		0xff, 0xff, 0x03
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);

	ASSERT_NEAR( 0.0,toSample64From20BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From20BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From20BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From20BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From20BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5,toSample64From20BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From20BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom20BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xf0, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom20BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom20BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x08, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom20BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom20BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x07, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom20BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom20BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom20BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom20BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x0c, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom20BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom20BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x03, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom20BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From20BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xf0, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From20BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From20BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x08, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From20BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From20BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x07, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From20BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From20BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From20BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From20BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x0c, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From20BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From20BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x03, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From20BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom20BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xf0, 0x00, 0x00,
		0x08, 0x00, 0x00,
		0x07, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x0c, 0x00, 0x00,
		0x03, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);

	ASSERT_NEAR( 0.0f,toSampleFrom20BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom20BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom20BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom20BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom20BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom20BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom20BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From20BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xf0, 0x00, 0x00,
		0x08, 0x00, 0x00,
		0x07, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x0c, 0x00, 0x00,
		0x03, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);

	ASSERT_NEAR( 0.0,toSample64From20BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From20BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From20BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From20BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From20BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5,toSample64From20BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From20BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom21BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xe0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom21BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom21BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x10 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom21BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom21BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom21BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom21BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom21BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom21BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x18 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom21BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom21BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom21BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From21BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xe0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From21BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From21BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x10 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From21BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From21BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From21BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From21BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From21BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From21BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x18 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From21BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From21BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x07 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From21BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom21BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xe0,
		0x00, 0x00, 0x10,
		0xff, 0xff, 0x0f,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x18,
		0xff, 0xff, 0x07
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom21BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom21BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom21BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom21BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom21BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom21BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom21BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From21BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xe0,
		0x00, 0x00, 0x10,
		0xff, 0xff, 0x0f,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x18,
		0xff, 0xff, 0x07
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From21BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From21BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From21BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From21BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From21BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5,toSample64From21BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From21BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom21BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xe0, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom21BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom21BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x10, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom21BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom21BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x0f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom21BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom21BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom21BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom21BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x18, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom21BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom21BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x07, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom21BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From21BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xe0, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From21BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From21BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x10, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From21BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From21BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x0f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From21BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From21BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From21BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From21BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x18, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From21BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From21BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x07, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From21BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom21BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xe0, 0x00, 0x00,
		0x10, 0x00, 0x00,
		0x0f, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x18, 0x00, 0x00,
		0x07, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom21BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom21BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom21BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom21BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom21BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom21BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom21BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From21BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xe0, 0x00, 0x00,
		0x10, 0x00, 0x00,
		0x0f, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x18, 0x00, 0x00,
		0x07, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From21BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From21BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From21BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From21BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From21BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5,toSample64From21BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From21BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom22BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom22BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom22BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x20 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom22BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom22BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom22BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom22BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom22BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom22BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x30 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom22BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom22BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom22BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From22BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From22BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From22BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x20 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From22BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From22BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From22BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From22BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From22BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From22BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x30 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From22BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From22BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x0f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From22BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom22BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xc0,
		0x00, 0x00, 0x20,
		0xff, 0xff, 0x1f,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x30,
		0xff, 0xff, 0x0f
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom22BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom22BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom22BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom22BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom22BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom22BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom22BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From22BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0xc0,
		0x00, 0x00, 0x20,
		0xff, 0xff, 0x1f,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x30,
		0xff, 0xff, 0x0f
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From22BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From22BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From22BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From22BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From22BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5,toSample64From22BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From22BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom22BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xc0, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom22BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom22BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x20, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom22BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom22BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x1f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom22BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom22BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom22BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom22BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x30, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom22BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom22BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x0f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom22BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From22BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0xc0, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From22BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From22BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x20, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From22BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From22BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x1f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From22BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From22BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From22BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From22BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x30, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From22BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From22BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x0f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From22BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom22BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xc0, 0x00, 0x00,
		0x20, 0x00, 0x00,
		0x1f, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x30, 0x00, 0x00,
		0x0f, 0xff, 0xff
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom22BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom22BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom22BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom22BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom22BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom22BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom22BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From22BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0xc0, 0x00, 0x00,
		0x20, 0x00, 0x00,
		0x1f, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x30, 0x00, 0x00,
		0x0f, 0xff, 0xff
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From22BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From22BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From22BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From22BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From22BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5,toSample64From22BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From22BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom23BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom23BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom23BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x40 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom23BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom23BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom23BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom23BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom23BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom23BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x60 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom23BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom23BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom23BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From23BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x00, 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From23BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From23BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x40 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From23BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From23BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From23BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From23BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From23BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From23BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0x60 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From23BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From23BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x1f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From23BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom23BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0x80,
		0x00, 0x00, 0x40,
		0xff, 0xff, 0x3f,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x60,
		0xff, 0xff, 0x1f
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom23BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom23BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom23BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom23BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom23BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom23BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom23BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From23BitsFromLittleEndianInMemory)
{
	tubyte x[18] = {
		0x00, 0x00, 0x80,
		0x00, 0x00, 0x40,
		0xff, 0xff, 0x3f,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x60,
		0xff, 0xff, 0x1f
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From23BitsFromLittleEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From23BitsFromLittleEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From23BitsFromLittleEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From23BitsFromLittleEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From23BitsFromLittleEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5,toSample64From23BitsFromLittleEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From23BitsFromLittleEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom23BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x80, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom23BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom23BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x40, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom23BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom23BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x3f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom23BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom23BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom23BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom23BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x60, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom23BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom23BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x1f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom23BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From23BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[3] = { 0x80, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From23BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From23BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x40, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From23BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From23BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x3f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From23BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From23BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From23BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From23BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0x60, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From23BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From23BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x1f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From23BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom23BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0x80, 0x00, 0x00,
		0x40, 0x00, 0x00,
		0x3f, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x60, 0x00, 0x00,
		0x1f, 0xff, 0xff
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom23BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom23BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom23BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom23BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom23BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom23BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom23BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From23BitsFromBigEndianInMemory)
{
	tubyte x[18] = {
		0x80, 0x00, 0x00,
		0x40, 0x00, 0x00,
		0x3f, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0x60, 0x00, 0x00,
		0x1f, 0xff, 0xff
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From23BitsFromBigEndian(mem, 0,18),0.000001);
	ASSERT_NEAR(-1.0,toSample64From23BitsFromBigEndian(mem, 3,18),0.000001);
	ASSERT_NEAR( 1.0,toSample64From23BitsFromBigEndian(mem, 6,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From23BitsFromBigEndian(mem, 9,18),0.000001);
	ASSERT_NEAR(-0.5,toSample64From23BitsFromBigEndian(mem,12,18),0.000001);
	ASSERT_NEAR( 0.5,toSample64From23BitsFromBigEndian(mem,15,18),0.000001);
	ASSERT_NEAR( 0.0,toSample64From23BitsFromBigEndian(mem,18,18),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom24BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom24BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom24BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom24BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom24BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x7f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom24BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom24BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom24BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom24BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom24BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom24BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom24BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From24BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From24BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From24BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x00, 0x00, 0x80 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From24BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From24BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0xff, 0xff, 0x7f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From24BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From24BitsFromLittleEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From24BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From24BitsFromLittleEndianNegativeHalf)
{
	tubyte x[3] = { 0x00, 0x00, 0xc0 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From24BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From24BitsFromLittleEndianPositiveHalf)
{
	tubyte x[3] = { 0xff, 0xff, 0x3f };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From24BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom24BitsFromLittleEndianInMemory)
{
	tubyte x[15] = {
		0x00, 0x00, 0x80,
		0xff, 0xff, 0x7f,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0xc0,
		0xff, 0xff, 0x3f
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0f,toSampleFrom24BitsFromLittleEndian(mem, 0,15),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom24BitsFromLittleEndian(mem, 3,15),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom24BitsFromLittleEndian(mem, 6,15),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom24BitsFromLittleEndian(mem, 9,15),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom24BitsFromLittleEndian(mem,12,15),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom24BitsFromLittleEndian(mem,15,15),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From24BitsFromLittleEndianInMemory)
{
	tubyte x[15] = {
		0x00, 0x00, 0x80,
		0xff, 0xff, 0x7f,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0xc0,
		0xff, 0xff, 0x3f
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0,toSample64From24BitsFromLittleEndian(mem, 0,15),0.000001);
	ASSERT_NEAR( 1.0,toSample64From24BitsFromLittleEndian(mem, 3,15),0.000001);
	ASSERT_NEAR( 0.0,toSample64From24BitsFromLittleEndian(mem, 6,15),0.000001);
	ASSERT_NEAR(-0.5,toSample64From24BitsFromLittleEndian(mem, 9,15),0.000001);
	ASSERT_NEAR( 0.5,toSample64From24BitsFromLittleEndian(mem,12,15),0.000001);
	ASSERT_NEAR( 0.0,toSample64From24BitsFromLittleEndian(mem,15,15),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom24BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom24BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom24BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x80, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom24BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom24BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x7f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom24BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom24BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom24BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom24BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0xc0, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom24BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom24BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x3f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom24BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From24BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From24BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From24BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[3] = { 0x80, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From24BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From24BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[3] = { 0x7f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From24BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From24BitsFromBigEndianZeroIsZero)
{
	tubyte x[3] = { 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From24BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From24BitsFromBigEndianNegativeHalf)
{
	tubyte x[3] = { 0xc0, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From24BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From24BitsFromBigEndianPositiveHalf)
{
	tubyte x[3] = { 0x3f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From24BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom24BitsFromBigEndianInMemory)
{
	tubyte x[15] = {
		0x80, 0x00, 0x00,
		0x7f, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0xc0, 0x00, 0x00,
		0x3f, 0xff, 0xff
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0f,toSampleFrom24BitsFromBigEndian(mem, 0,15),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom24BitsFromBigEndian(mem, 3,15),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom24BitsFromBigEndian(mem, 6,15),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom24BitsFromBigEndian(mem, 9,15),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom24BitsFromBigEndian(mem,12,15),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom24BitsFromBigEndian(mem,15,15),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From24BitsFromBigEndianInMemory)
{
	tubyte x[15] = {
		0x80, 0x00, 0x00,
		0x7f, 0xff, 0xff,
		0x00, 0x00, 0x00,
		0xc0, 0x00, 0x00,
		0x3f, 0xff, 0xff
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR(-1.0,toSample64From24BitsFromBigEndian(mem, 0,15),0.000001);
	ASSERT_NEAR( 1.0,toSample64From24BitsFromBigEndian(mem, 3,15),0.000001);
	ASSERT_NEAR( 0.0,toSample64From24BitsFromBigEndian(mem, 6,15),0.000001);
	ASSERT_NEAR(-0.5,toSample64From24BitsFromBigEndian(mem, 9,15),0.000001);
	ASSERT_NEAR( 0.5,toSample64From24BitsFromBigEndian(mem,12,15),0.000001);
	ASSERT_NEAR( 0.0,toSample64From24BitsFromBigEndian(mem,15,15),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom25BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xfe };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom25BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom25BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom25BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom25BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom25BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom25BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom25BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom25BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x80, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom25BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom25BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0x7f, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom25BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From25BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xfe };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From25BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From25BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From25BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From25BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From25BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From25BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From25BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From25BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x80, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From25BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From25BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0x7f, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From25BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom25BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xfe,
		0x00, 0x00, 0x00, 0x01,
		0xff, 0xff, 0xff, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x80, 0x01,
		0xff, 0xff, 0x7f, 0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom25BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom25BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom25BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom25BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom25BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom25BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom25BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From25BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xfe,
		0x00, 0x00, 0x00, 0x01,
		0xff, 0xff, 0xff, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x80, 0x01,
		0xff, 0xff, 0x7f, 0x00
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From25BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From25BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From25BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From25BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From25BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From25BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From25BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom25BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0xfe, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom25BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom25BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x01, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom25BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom25BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x00, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom25BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom25BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom25BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom25BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x01, 0x80, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom25BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom25BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x00, 0x7f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom25BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From25BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0xfe, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From25BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From25BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x01, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From25BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From25BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x00, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From25BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From25BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From25BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From25BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x01, 0x80, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From25BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From25BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x00, 0x7f, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From25BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom25BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xfe, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00,
		0x00, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x01, 0x80, 0x00, 0x00,
		0x00, 0x7f, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom25BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom25BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom25BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom25BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom25BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom25BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom25BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From25BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xfe, 0x00, 0x00, 0x00,
		0x01, 0x00, 0x00, 0x00,
		0x00, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x01, 0x80, 0x00, 0x00,
		0x00, 0x7f, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From25BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From25BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From25BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From25BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From25BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From25BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From25BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom26BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xfc };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom26BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom26BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x02 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom26BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom26BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom26BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom26BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom26BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom26BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom26BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom26BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom26BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From26BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xfc };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From26BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From26BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x02 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From26BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From26BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From26BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From26BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From26BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From26BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From26BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From26BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From26BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom26BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xfc,
		0x00, 0x00, 0x00, 0x02,
		0xff, 0xff, 0xff, 0x01,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x03,
		0xff, 0xff, 0xff, 0x00
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom26BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom26BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom26BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom26BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom26BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom26BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom26BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From26BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xfc,
		0x00, 0x00, 0x00, 0x02,
		0xff, 0xff, 0xff, 0x01,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x03,
		0xff, 0xff, 0xff, 0x00
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From26BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From26BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From26BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From26BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From26BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From26BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From26BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom26BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0xfc, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom26BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom26BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x02, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom26BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom26BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x01, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom26BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom26BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom26BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom26BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x03, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom26BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom26BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x00, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom26BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From26BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0xfc, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From26BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From26BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x02, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From26BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From26BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x01, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From26BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From26BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From26BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From26BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x03, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From26BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From26BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x00, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From26BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom26BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xfc, 0x00, 0x00, 0x00,
		0x02, 0x00, 0x00, 0x00,
		0x01, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x00, 0x00,
		0x00, 0xff, 0xff, 0xff
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom26BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom26BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom26BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom26BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom26BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom26BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom26BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From26BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xfc, 0x00, 0x00, 0x00,
		0x02, 0x00, 0x00, 0x00,
		0x01, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x03, 0x00, 0x00, 0x00,
		0x00, 0xff, 0xff, 0xff
	};
    const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From26BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From26BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From26BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From26BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From26BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From26BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From26BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom27BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xf8 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom27BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom27BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x04 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom27BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom27BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom27BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom27BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom27BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom27BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x06 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom27BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom27BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom27BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From27BitsFromLittleEndianIgnoresTopBit)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0xf8 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From27BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From27BitsFromLittleEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x04 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From27BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From27BitsFromLittleEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x03 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From27BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From27BitsFromLittleEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From27BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From27BitsFromLittleEndianNegativeHalf)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x06 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From27BitsFromLittleEndian(y),0.000001);
}

TEST(FormatType,toSample64From27BitsFromLittleEndianPositiveHalf)
{
	tubyte x[4] = { 0xff, 0xff, 0xff, 0x01 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From27BitsFromLittleEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom27BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xf8,
		0x00, 0x00, 0x00, 0x04,
		0xff, 0xff, 0xff, 0x03,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x06,
		0xff, 0xff, 0xff, 0x01
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom27BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom27BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom27BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom27BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom27BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom27BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom27BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From27BitsFromLittleEndianInMemory)
{
	tubyte x[24] = {
		0x00, 0x00, 0x00, 0xf8,
		0x00, 0x00, 0x00, 0x04,
		0xff, 0xff, 0xff, 0x03,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x06,
		0xff, 0xff, 0xff, 0x01
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From27BitsFromLittleEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From27BitsFromLittleEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From27BitsFromLittleEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From27BitsFromLittleEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From27BitsFromLittleEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From27BitsFromLittleEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From27BitsFromLittleEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom27BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0xf8, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom27BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom27BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x04, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0f,toSampleFrom27BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom27BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x03, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0f,toSampleFrom27BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom27BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0f,toSampleFrom27BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom27BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x06, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5f,toSampleFrom27BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSampleFrom27BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x01, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5f,toSampleFrom27BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------
// 64

TEST(FormatType,toSample64From27BitsFromBigEndianIgnoresTopBit)
{
	tubyte x[4] = { 0xf8, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From27BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From27BitsFromBigEndianLowerLimitIsMinusOne)
{
	tubyte x[4] = { 0x04, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-1.0,toSample64From27BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From27BitsFromBigEndianUpperLimitIsPlusOne)
{
	tubyte x[4] = { 0x03, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(1.0,toSample64From27BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From27BitsFromBigEndianZeroIsZero)
{
	tubyte x[4] = { 0x00, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.0,toSample64From27BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From27BitsFromBigEndianNegativeHalf)
{
	tubyte x[4] = { 0x06, 0x00, 0x00, 0x00 };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(-0.5,toSample64From27BitsFromBigEndian(y),0.000001);
}

TEST(FormatType,toSample64From27BitsFromBigEndianPositiveHalf)
{
	tubyte x[4] = { 0x01, 0xff, 0xff, 0xff };
	const tbyte *y = reinterpret_cast<const tbyte *>(x);
	ASSERT_NEAR(0.5,toSample64From27BitsFromBigEndian(y),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSampleFrom27BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xf8, 0x00, 0x00, 0x00,
		0x04, 0x00, 0x00, 0x00,
		0x03, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x06, 0x00, 0x00, 0x00,
		0x01, 0xff, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0f,toSampleFrom27BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0f,toSampleFrom27BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0f,toSampleFrom27BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom27BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5f,toSampleFrom27BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5f,toSampleFrom27BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0f,toSampleFrom27BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,toSample64From27BitsFromBigEndianInMemory)
{
	tubyte x[24] = {
		0xf8, 0x00, 0x00, 0x00,
		0x04, 0x00, 0x00, 0x00,
		0x03, 0xff, 0xff, 0xff,
		0x00, 0x00, 0x00, 0x00,
		0x06, 0x00, 0x00, 0x00,
		0x01, 0xff, 0xff, 0xff
	};
	const tbyte *mem = reinterpret_cast<const tbyte *>(x);
	
	ASSERT_NEAR( 0.0,toSample64From27BitsFromBigEndian(mem, 0,24),0.000001);
	ASSERT_NEAR(-1.0,toSample64From27BitsFromBigEndian(mem, 4,24),0.000001);
	ASSERT_NEAR( 1.0,toSample64From27BitsFromBigEndian(mem, 8,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From27BitsFromBigEndian(mem,12,24),0.000001);
	ASSERT_NEAR(-0.5,toSample64From27BitsFromBigEndian(mem,16,24),0.000001);
	ASSERT_NEAR( 0.5,toSample64From27BitsFromBigEndian(mem,20,24),0.000001);
	ASSERT_NEAR( 0.0,toSample64From27BitsFromBigEndian(mem,24,24),0.000001);
}

//-------------------------------------------------------------------------------------------
// Apple code for converting double to IEEE-80 encoded
//-------------------------------------------------------------------------------------------

#define FloatToUnsigned(f) ((unsigned long)(((long)(f - 2147483648.0)) + 2147483647L) + 1)

void ConvertToIeeeExtended(double num,char *bytes)
{
	int sign;
	int expon;
	double fMant, fsMant;
	unsigned long hiMant, loMant;

	if(num < 0) 
	{
		sign = 0x8000;
		num *= -1;
	}
	else 
	{
		sign = 0;
	}

	if(num == 0) 
	{
		expon = 0;
		hiMant = 0;
		loMant = 0;
	}
	else
	{
		fMant = ::frexp(num, &expon);

		if ((expon > 16384) || !(fMant < 1))
		{
			/* Infinity or NaN */
			expon = sign|0x7FFF; hiMant = 0; loMant = 0; /* infinity */
		}
		else 
		{    
			/* Finite */
			expon += 16382;
			if (expon < 0) 
			{    
				/* denormalized */
				fMant = ::ldexp(fMant, expon);
				expon = 0;
			}
			expon |= sign;
			fMant = ::ldexp(fMant, 32);          
			fsMant = ::floor(fMant); 
			hiMant = FloatToUnsigned(fsMant);
			fMant = ::ldexp(fMant - fsMant, 32); 
			fsMant = ::floor(fMant); 
			loMant = FloatToUnsigned(fsMant);
		}
	}
    
    bytes[0] = static_cast<tubyte>((expon >> 8) & 0x000000ff);
    bytes[1] = static_cast<tubyte>((expon) & 0x000000ff);
    bytes[2] = static_cast<tubyte>((hiMant >> 24) & 0x000000ff);
    bytes[3] = static_cast<tubyte>((hiMant >> 16) & 0x000000ff);
    bytes[4] = static_cast<tubyte>((hiMant >>  8) & 0x000000ff);
    bytes[5] = static_cast<tubyte>((hiMant) & 0x000000ff);
    bytes[6] = static_cast<tubyte>((loMant >> 24) & 0x000000ff);
    bytes[7] = static_cast<tubyte>((loMant >> 16) & 0x000000ff);
    bytes[8] = static_cast<tubyte>((loMant >>  8) & 0x000000ff);
    bytes[9] = static_cast<tubyte>((loMant) & 0x000000ff);
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,doubleFromExtendedFrequencyRange)
{
	int i;
	tfloat64 org,exp;
	tbyte mem[10];
	
	for(i=-192000;i<192000;i++)
	{
		org = static_cast<tfloat64>(i);
		ConvertToIeeeExtended(org,mem);
		exp = doubleFromExtended(mem);
		EXPECT_NEAR(org,exp,0.000001);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType,doubleFromExtendedPower10Values)
{
	int i;
	tfloat64 org,exp;
	tbyte mem[10];
	
	for(i=0;i<10;i++)
	{
		org = ::pow(1.0,i);
		ConvertToIeeeExtended(org,mem);
		exp = doubleFromExtended(mem);
		EXPECT_NEAR(org,exp,0.000001);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, write16BitsLittleEndianFromSampleInt16)
{
	const tuint16 c_int16Samples[12] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000,
		0x7fff, 0x8000
	};
	
	const tubyte c_expectIntegerSamples[24] = {
		0xff, 0x7f, 
		0x66, 0x66, 
		0xcc, 0x4c, 
		0x33, 0x33, 
		0x99, 0x19,
		0x66, 0xe6, 
		0xcd, 0xcc, 
		0x33, 0xb3, 
		0x9a, 0x99, 
		0x00, 0x80,
		0xff, 0x7f, 
		0x00, 0x80
	};
	
	int i;
	tbyte mem[24];
	
	for(i = 0; i < 12; i++)
	{
		write16BitsLittleEndianFromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 2]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 24));
}

TEST(FormatType, write16BitsLittleEndianFromSampleInt24)
{
	const tuint32 c_int24Samples[15] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80, 0xffffff7f
	};
	
	const tubyte c_expectIntegerSamples[30] = {
		0xff, 0x7f,
		0x66, 0x66, 
		0xcd, 0x4c,
		0x33, 0x33, 
		0x9a, 0x19,
		0x66, 0xe6, 
		0xcd, 0xcc, 
		0x33, 0xb3, 
		0x9a, 0x99, 
		0x00, 0x80,
		0x01, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0xff, 0xff
	};
	
	int i;
	tbyte mem[30];
	
	for(i = 0; i < 15; i++)
	{
		write16BitsLittleEndianFromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 2]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 30));
}

TEST(FormatType, write16BitsLittleEndianFromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00008000, 0x00007fff, 0x00000000, 0xffff8000, 0xffff7fff
	};
	
	const tubyte c_expectIntegerSamples[30] = {
		0xff, 0x7f,
		0x66, 0x66, 
		0xcd, 0x4c,
		0x33, 0x33, 
		0x9a, 0x19,
		0x66, 0xe6, 
		0xcd, 0xcc, 
		0x33, 0xb3, 
		0x9a, 0x99, 
		0x00, 0x80,
		0x01, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0xff, 0xff
	};
	
	int i;
	tbyte mem[30];
	
	for(i = 0; i < 15; i++)
	{
		write16BitsLittleEndianFromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 2]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 30));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, write16BitsBigEndianFromSampleInt16)
{
	const tuint16 c_int16Samples[12] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000,
		0x7fff, 0x8000
	};
	
	const tubyte c_expectIntegerSamples[24] = {
		0x7f, 0xff, 
		0x66, 0x66, 
		0x4c, 0xcc, 
		0x33, 0x33, 
		0x19, 0x99,
		0xe6, 0x66, 
		0xcc, 0xcd, 
		0xb3, 0x33, 
		0x99, 0x9a, 
		0x80, 0x00,
		0x7f, 0xff, 
		0x80, 0x00
	};
	
	int i;
	tbyte mem[24];
	
	for(i = 0; i < 12; i++)
	{
		write16BitsBigEndianFromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 2]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 24));
}

TEST(FormatType, write16BitsBigEndianFromSampleInt24)
{
	const tuint32 c_int24Samples[15] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80, 0xffffff7f
	};

	const tubyte c_expectIntegerSamples[30] = {
		0x7f, 0xff, 
		0x66, 0x66, 
		0x4c, 0xcd, 
		0x33, 0x33, 
		0x19, 0x9a,
		0xe6, 0x66, 
		0xcc, 0xcd, 
		0xb3, 0x33, 
		0x99, 0x9a,
		0x80, 0x00,
		0x00, 0x01,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0xff, 0xff
	};
		
	int i;
	tbyte mem[30];
	
	for(i = 0; i < 15; i++)
	{
		write16BitsBigEndianFromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 2]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 30));
}

TEST(FormatType, write16BitsBigEndianFromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00008000, 0x00007fff, 0x00000000, 0xffff8000, 0xffff7fff
	};

	const tubyte c_expectIntegerSamples[30] = {
		0x7f, 0xff, 
		0x66, 0x66, 
		0x4c, 0xcd, 
		0x33, 0x33, 
		0x19, 0x9a,
		0xe6, 0x66, 
		0xcc, 0xcd, 
		0xb3, 0x33, 
		0x99, 0x9a,
		0x80, 0x00,
		0x00, 0x01,
		0x00, 0x00,
		0x00, 0x00,
		0x00, 0x00,
		0xff, 0xff
	};
	
	int i;
	tbyte mem[30];
	
	for(i = 0; i < 15; i++)
	{
		write16BitsBigEndianFromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 2]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 30));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, write24BitsLittleEndianFromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamples[30] = {
		0x00, 0xff, 0x7f,
		0x00, 0x66, 0x66,
		0x00, 0xcc, 0x4c,
		0x00, 0x33, 0x33,
		0x00, 0x99, 0x19,
		0x00, 0x66, 0xe6,
		0x00, 0xcd, 0xcc,
		0x00, 0x33, 0xb3,
		0x00, 0x9a, 0x99,
		0x00, 0x00, 0x80
	};
	
	int i;
	tbyte mem[30];
	
	for(i = 0; i < 10; i++)
	{
		write24BitsLittleEndianFromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 3]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 30));
}

TEST(FormatType, write24BitsLittleEndianFromSampleInt24)
{
	const tuint32 c_int24Samples[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamples[30] = {
		0xff, 0xff, 0x7f,
		0x66, 0x66, 0x66,
		0xcc, 0xcc, 0x4c,
		0x33, 0x33, 0x33,
		0x99, 0x99, 0x19,
		0x66, 0x66, 0xe6,
		0xcd, 0xcc, 0xcc,
		0x33, 0x33, 0xb3,
		0x9a, 0x99, 0x99,
		0x00, 0x00, 0x80
	};
		
	int i;
	tbyte mem[30];
	
	for(i = 0; i < 10; i++)
	{
		write24BitsLittleEndianFromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 3]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 30));
}

TEST(FormatType, write24BitsLittleEndianFromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80, 0xffffff7f
	};

	const tubyte c_expectIntegerSamples[45] = {
		0xff, 0xff, 0x7f,
		0x66, 0x66, 0x66,
		0xcd, 0xcc, 0x4c,
		0x33, 0x33, 0x33,
		0x9a, 0x99, 0x19,
		0x66, 0x66, 0xe6,
		0xcd, 0xcc, 0xcc,
		0x33, 0x33, 0xb3,
		0x9a, 0x99, 0x99,
		0x00, 0x00, 0x80,
		0x01, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[45];
	
	for(i = 0; i < 15; i++)
	{
		write24BitsLittleEndianFromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 3]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 45));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, write24BitsBigEndianFromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamples[30] = {
		0x7f, 0xff, 0x00,
		0x66, 0x66, 0x00,
		0x4c, 0xcc, 0x00,
		0x33, 0x33, 0x00,
		0x19, 0x99, 0x00,
		0xe6, 0x66, 0x00,
		0xcc, 0xcd, 0x00,
		0xb3, 0x33, 0x00,
		0x99, 0x9a, 0x00,
		0x80, 0x00, 0x00
	};
	
	int i;
	tbyte mem[30];
	
	for(i = 0; i < 10; i++)
	{
		write24BitsBigEndianFromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 3]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 30));
}

TEST(FormatType, write24BitsBigEndianFromSampleInt24)
{
	const tuint32 c_int24Samples[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamples[30] = {
		0x7f, 0xff, 0xff,
		0x66, 0x66, 0x66,
		0x4c, 0xcc, 0xcc,
		0x33, 0x33, 0x33,
		0x19, 0x99, 0x99,
		0xe6, 0x66, 0x66,
		0xcc, 0xcc, 0xcd,
		0xb3, 0x33, 0x33,
		0x99, 0x99, 0x9a,
		0x80, 0x00, 0x00
	};
		
	int i;
	tbyte mem[30];
	
	for(i = 0; i < 10; i++)
	{
		write24BitsBigEndianFromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 3]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 30));
}

TEST(FormatType, write24BitsBigEndianFromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80, 0xffffff7f
	};

	const tubyte c_expectIntegerSamples[45] = {
		0x7f, 0xff, 0xff,
		0x66, 0x66, 0x66,
		0x4c, 0xcc, 0xcd,
		0x33, 0x33, 0x33,
		0x19, 0x99, 0x9a,
		0xe6, 0x66, 0x66,
		0xcc, 0xcc, 0xcd,
		0xb3, 0x33, 0x33,
		0x99, 0x99, 0x9a,
		0x80, 0x00, 0x00,
		0x00, 0x00, 0x01,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0x00, 0x00, 0x00,
		0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[45];
	
	for(i = 0; i < 15; i++)
	{
		write24BitsBigEndianFromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 3]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 45));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, write32BitsLittleEndianFromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamples[40] = {
		0x00, 0x00, 0xff, 0x7f,
		0x00, 0x00, 0x66, 0x66,
		0x00, 0x00, 0xcc, 0x4c,
		0x00, 0x00, 0x33, 0x33,
		0x00, 0x00, 0x99, 0x19,
		0x00, 0x00, 0x66, 0xe6,
		0x00, 0x00, 0xcd, 0xcc,
		0x00, 0x00, 0x33, 0xb3,
		0x00, 0x00, 0x9a, 0x99,
		0x00, 0x00, 0x00, 0x80
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		write32BitsLittleEndianFromSampleInt16(static_cast<tint32>(c_int16Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, write32BitsLittleEndianFromSampleInt24)
{
	const tuint32 c_int24Samples[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamples[40] = {
		0x00, 0xff, 0xff, 0x7f,
		0x00, 0x66, 0x66, 0x66,
		0x00, 0xcc, 0xcc, 0x4c,
		0x00, 0x33, 0x33, 0x33,
		0x00, 0x99, 0x99, 0x19,
		0x00, 0x66, 0x66, 0xe6,
		0x00, 0xcd, 0xcc, 0xcc,
		0x00, 0x33, 0x33, 0xb3,
		0x00, 0x9a, 0x99, 0x99,
		0x00, 0x00, 0x00, 0x80
	};
		
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		write32BitsLittleEndianFromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, write32BitsLittleEndianFromSampleInt32)
{
	const tuint32 c_int32Samples[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
	};

	const tubyte c_expectIntegerSamples[40] = {
		0xff, 0xff, 0xff, 0x7f,
		0x66, 0x66, 0x66, 0x66,
		0xcc, 0xcc, 0xcc, 0x4c,
		0x33, 0x33, 0x33, 0x33,
		0x99, 0x99, 0x99, 0x19,
		0x66, 0x66, 0x66, 0xe6,
		0xcd, 0xcc, 0xcc, 0xcc,
		0x33, 0x33, 0x33, 0xb3,
		0x9a, 0x99, 0x99, 0x99,
		0x00, 0x00, 0x00, 0x80
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		write32BitsLittleEndianFromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, write32BitsBigEndianFromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamples[40] = {
		0x7f, 0xff, 0x00, 0x00,
		0x66, 0x66, 0x00, 0x00,
		0x4c, 0xcc, 0x00, 0x00,
		0x33, 0x33, 0x00, 0x00,
		0x19, 0x99, 0x00, 0x00,
		0xe6, 0x66, 0x00, 0x00,
		0xcc, 0xcd, 0x00, 0x00,
		0xb3, 0x33, 0x00, 0x00,
		0x99, 0x9a, 0x00, 0x00,
		0x80, 0x00, 0x00, 0x00
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		write32BitsBigEndianFromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, write32BitsBigEndianFromSampleInt24)
{
	const tuint32 c_int24Samples[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamples[40] = {
		0x7f, 0xff, 0xff, 0x00,
		0x66, 0x66, 0x66, 0x00,
		0x4c, 0xcc, 0xcc, 0x00,
		0x33, 0x33, 0x33, 0x00,
		0x19, 0x99, 0x99, 0x00,
		0xe6, 0x66, 0x66, 0x00,
		0xcc, 0xcc, 0xcd, 0x00,
		0xb3, 0x33, 0x33, 0x00,
		0x99, 0x99, 0x9a, 0x00,
		0x80, 0x00, 0x00, 0x00
	};
		
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		write32BitsBigEndianFromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, write32BitsBigEndianFromSampleInt32)
{
	const tuint32 c_int32Samples[10] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000
	};

	const tubyte c_expectIntegerSamples[40] = {
		0x7f, 0xff, 0xff, 0xff,
		0x66, 0x66, 0x66, 0x66,
		0x4c, 0xcc, 0xcc, 0xcc,
		0x33, 0x33, 0x33, 0x33,
		0x19, 0x99, 0x99, 0x99,
		0xe6, 0x66, 0x66, 0x66,
		0xcc, 0xcc, 0xcc, 0xcd,
		0xb3, 0x33, 0x33, 0x33,
		0x99, 0x99, 0x99, 0x9a,
		0x80, 0x00, 0x00, 0x00
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		write32BitsBigEndianFromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, writeInt32LSB16FromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamples[40] = {
		0xff, 0x7f, 0x00, 0x00, // 0
		0x66, 0x66, 0x00, 0x00, // 1
		0xcc, 0x4c, 0x00, 0x00, // 2
		0x33, 0x33, 0x00, 0x00, // 3
		0x99, 0x19, 0x00, 0x00, // 4
		0x66, 0xe6, 0xff, 0xff, // 5
		0xcd, 0xcc, 0xff, 0xff, // 6
		0x33, 0xb3, 0xff, 0xff, // 7
		0x9a, 0x99, 0xff, 0xff, // 8
		0x00, 0x80, 0xff, 0xff  // 9
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		writeInt32LSB16FromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, writeInt32LSB16FromSampleInt24)
{
	const tuint32 c_int24Samples[15] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80, 0xffffff7f
	};

	const tubyte c_expectIntegerSamples[60] = {
		0xff, 0x7f, 0x00, 0x00, // 0
		0x66, 0x66, 0x00, 0x00, // 1
		0xcd, 0x4c, 0x00, 0x00, // 2
		0x33, 0x33, 0x00, 0x00, // 3
		0x9a, 0x19, 0x00, 0x00, // 4
		0x66, 0xe6, 0xff, 0xff, // 5
		0xcd, 0xcc, 0xff, 0xff, // 6
		0x33, 0xb3, 0xff, 0xff, // 7
		0x9a, 0x99, 0xff, 0xff, // 8
		0x00, 0x80, 0xff, 0xff, // 9
		0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32LSB16FromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

TEST(FormatType, writeInt32LSB16FromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00008000, 0x00007fff, 0x00000000, 0xffff8000, 0xffff7fff
	};

	const tubyte c_expectIntegerSamples[60] = {
		0xff, 0x7f, 0x00, 0x00, // 0
		0x66, 0x66, 0x00, 0x00, // 1
		0xcd, 0x4c, 0x00, 0x00, // 2
		0x33, 0x33, 0x00, 0x00, // 3
		0x9a, 0x19, 0x00, 0x00, // 4
		0x66, 0xe6, 0xff, 0xff, // 5
		0xcd, 0xcc, 0xff, 0xff, // 6
		0x33, 0xb3, 0xff, 0xff, // 7
		0x9a, 0x99, 0xff, 0xff, // 8
		0x00, 0x80, 0xff, 0xff, // 9
		0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32LSB16FromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, writeInt32MSB16FromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamples[40] = {
		0x00, 0x00, 0x7f, 0xff, // 0
		0x00, 0x00, 0x66, 0x66, // 1
		0x00, 0x00, 0x4c, 0xcc, // 2
		0x00, 0x00, 0x33, 0x33, // 3
		0x00, 0x00, 0x19, 0x99, // 4
		0xff, 0xff, 0xe6, 0x66, // 5
		0xff, 0xff, 0xcc, 0xcd, // 6
		0xff, 0xff, 0xb3, 0x33, // 7
		0xff, 0xff, 0x99, 0x9a, // 8
		0xff, 0xff, 0x80, 0x00  // 9
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		writeInt32MSB16FromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, writeInt32MSB16FromSampleInt24)
{
	const tuint32 c_int24Samples[15] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80, 0xffffff7f
	};

	const tubyte c_expectIntegerSamples[60] = {
		0x00, 0x00, 0x7f, 0xff, // 0
		0x00, 0x00, 0x66, 0x66, // 1
		0x00, 0x00, 0x4c, 0xcd, // 2
		0x00, 0x00, 0x33, 0x33, // 3
		0x00, 0x00, 0x19, 0x9a, // 4
		0xff, 0xff, 0xe6, 0x66, // 5
		0xff, 0xff, 0xcc, 0xcd, // 6
		0xff, 0xff, 0xb3, 0x33, // 7
		0xff, 0xff, 0x99, 0x9a, // 8
		0xff, 0xff, 0x80, 0x00, // 9
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32MSB16FromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

TEST(FormatType, writeInt32MSB16FromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00008000, 0x00007fff, 0x00000000, 0xffff8000, 0xffff7fff
	};

	const tubyte c_expectIntegerSamples[60] = {
		0x00, 0x00, 0x7f, 0xff, // 0
		0x00, 0x00, 0x66, 0x66, // 1
		0x00, 0x00, 0x4c, 0xcd, // 2
		0x00, 0x00, 0x33, 0x33, // 3
		0x00, 0x00, 0x19, 0x9a, // 4
		0xff, 0xff, 0xe6, 0x66, // 5
		0xff, 0xff, 0xcc, 0xcd, // 6
		0xff, 0xff, 0xb3, 0x33, // 7
		0xff, 0xff, 0x99, 0x9a, // 8
		0xff, 0xff, 0x80, 0x00, // 9
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32MSB16FromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, writeInt32LSB18FromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};

	const tubyte c_expectIntegerSamples[40] = {
		0xfc, 0xff, 0x01, 0x00, // 0
		0x98, 0x99, 0x01, 0x00, // 1
		0x30, 0x33, 0x01, 0x00, // 2
		0xcc, 0xcc, 0x00, 0x00, // 3
		0x64, 0x66, 0x00, 0x00, // 4
		0x98, 0x99, 0xff, 0xff, // 5
		0x34, 0x33, 0xff, 0xff, // 6
		0xcc, 0xcc, 0xfe, 0xff, // 7
		0x68, 0x66, 0xfe, 0xff, // 8
		0x00, 0x00, 0xfe, 0xff  // 9
	};

	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		writeInt32LSB18FromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, writeInt32LSB18FromSampleInt24)
{
	const tuint32 c_int24Samples[15] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000020, 0x0000001f, 0x00000000, 0xffffffe0, 0xffffffdf
	};

	const tubyte c_expectIntegerSamples[60] = {
		0xff, 0xff, 0x01, 0x00, // 0
		0x9a, 0x99, 0x01, 0x00, // 1
		0x33, 0x33, 0x01, 0x00, // 2
		0xcd, 0xcc, 0x00, 0x00, // 3
		0x66, 0x66, 0x00, 0x00, // 4
		0x9a, 0x99, 0xff, 0xff, // 5
		0x33, 0x33, 0xff, 0xff, // 6
		0xcd, 0xcc, 0xfe, 0xff, // 7
		0x66, 0x66, 0xfe, 0xff, // 8
		0x00, 0x00, 0xfe, 0xff, // 9
		0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff
	};

	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32LSB18FromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

TEST(FormatType, writeInt32LSB18FromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00002000, 0x00001fff, 0x00000000, 0xffffe000, 0xffffdfff
	};

	const tubyte c_expectIntegerSamples[60] = {
		0xff, 0xff, 0x01, 0x00, // 0
		0x9a, 0x99, 0x01, 0x00, // 1
		0x33, 0x33, 0x01, 0x00, // 2
		0xcd, 0xcc, 0x00, 0x00, // 3
		0x66, 0x66, 0x00, 0x00, // 4
		0x9a, 0x99, 0xff, 0xff, // 5
		0x33, 0x33, 0xff, 0xff, // 6
		0xcd, 0xcc, 0xfe, 0xff, // 7
		0x66, 0x66, 0xfe, 0xff, // 8
		0x00, 0x00, 0xfe, 0xff, // 9
		0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff
	};

	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32LSB18FromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, writeInt32MSB18FromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};

	const tubyte c_expectIntegerSamples[40] = {
		0x00, 0x01, 0xff, 0xfc, // 0
		0x00, 0x01, 0x99, 0x98, // 1
		0x00, 0x01, 0x33, 0x30, // 2
		0x00, 0x00, 0xcc, 0xcc, // 3
		0x00, 0x00, 0x66, 0x64, // 4
		0xff, 0xff, 0x99, 0x98, // 5
		0xff, 0xff, 0x33, 0x34, // 6
		0xff, 0xfe, 0xcc, 0xcc, // 7
		0xff, 0xfe, 0x66, 0x68, // 8
		0xff, 0xfe, 0x00, 0x00  // 9
	};

	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		writeInt32MSB18FromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, writeInt32MSB18FromSampleInt24)
{
	const tuint32 c_int24Samples[15] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000020, 0x0000001f, 0x00000000, 0xffffffe0, 0xffffffdf
	};

	const tubyte c_expectIntegerSamples[60] = {
		0x00, 0x01, 0xff, 0xff, // 0
		0x00, 0x01, 0x99, 0x9a, // 1
		0x00, 0x01, 0x33, 0x33, // 2
		0x00, 0x00, 0xcc, 0xcd, // 3
		0x00, 0x00, 0x66, 0x66, // 4
		0xff, 0xff, 0x99, 0x9a, // 5
		0xff, 0xff, 0x33, 0x33, // 6
		0xff, 0xfe, 0xcc, 0xcd, // 7
		0xff, 0xfe, 0x66, 0x66, // 8
		0xff, 0xfe, 0x00, 0x00, // 9
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff
	};

	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32MSB18FromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

TEST(FormatType, writeInt32MSB18FromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00002000, 0x00001fff, 0x00000000, 0xffffe000, 0xffffdfff
	};
	
	const tubyte c_expectIntegerSamples[60] = {
		0x00, 0x01, 0xff, 0xff, // 0
		0x00, 0x01, 0x99, 0x9a, // 1
		0x00, 0x01, 0x33, 0x33, // 2
		0x00, 0x00, 0xcc, 0xcd, // 3
		0x00, 0x00, 0x66, 0x66, // 4
		0xff, 0xff, 0x99, 0x9a, // 5
		0xff, 0xff, 0x33, 0x33, // 6
		0xff, 0xfe, 0xcc, 0xcd, // 7
		0xff, 0xfe, 0x66, 0x66, // 8
		0xff, 0xfe, 0x00, 0x00, // 9
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0xff, 0xff, 0xff, 0xff
	};

	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32MSB18FromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, writeInt32LSB20FromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};

	const tubyte c_expectIntegerSamples[40] = {
		0xf0, 0xff, 0x07, 0x00,
		0x60, 0x66, 0x06, 0x00,
		0xc0, 0xcc, 0x04, 0x00,
		0x30, 0x33, 0x03, 0x00,
		0x90, 0x99, 0x01, 0x00,
		0x60, 0x66, 0xfe, 0xff,
		0xd0, 0xcc, 0xfc, 0xff,
		0x30, 0x33, 0xfb, 0xff,
		0xa0, 0x99, 0xf9, 0xff,
		0x00, 0x00, 0xf8, 0xff
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		writeInt32LSB20FromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, writeInt32LSB20FromSampleInt24)
{
	const tuint32 c_int24Samples[15] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000008, 0x00000007, 0x00000000, 0xfffffff8, 0xfffffff7
	};

	const tubyte c_expectIntegerSamples[60] = {
		0xff, 0xff, 0x07, 0x00,
		0x66, 0x66, 0x06, 0x00,
		0xcd, 0xcc, 0x04, 0x00,
		0x33, 0x33, 0x03, 0x00,
		0x9a, 0x99, 0x01, 0x00,
		0x66, 0x66, 0xfe, 0xff,
		0xcd, 0xcc, 0xfc, 0xff,
		0x33, 0x33, 0xfb, 0xff,
		0x9a, 0x99, 0xf9, 0xff,
		0x00, 0x00, 0xf8, 0xff,
		0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 
		0xff, 0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32LSB20FromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

TEST(FormatType, writeInt32LSB20FromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00000800, 0x00000700, 0x00000000, 0xfffff800, 0xfffff7ff
	};

	const tubyte c_expectIntegerSamples[60] = {
		0xff, 0xff, 0x07, 0x00,
		0x66, 0x66, 0x06, 0x00,
		0xcd, 0xcc, 0x04, 0x00,
		0x33, 0x33, 0x03, 0x00,
		0x9a, 0x99, 0x01, 0x00,
		0x66, 0x66, 0xfe, 0xff,
		0xcd, 0xcc, 0xfc, 0xff,
		0x33, 0x33, 0xfb, 0xff,
		0x9a, 0x99, 0xf9, 0xff,
		0x00, 0x00, 0xf8, 0xff,
		0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 
		0xff, 0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32LSB20FromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, writeInt32MSB20FromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};
	
	const tubyte c_expectIntegerSamples[40] = {
		0x00, 0x07, 0xff, 0xf0,
		0x00, 0x06, 0x66, 0x60,
		0x00, 0x04, 0xcc, 0xc0,
		0x00, 0x03, 0x33, 0x30,
		0x00, 0x01, 0x99, 0x90,
		0xff, 0xfe, 0x66, 0x60,
		0xff, 0xfc, 0xcc, 0xd0,
		0xff, 0xfb, 0x33, 0x30,
		0xff, 0xf9, 0x99, 0xa0,
		0xff, 0xf8, 0x00, 0x00
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		writeInt32MSB20FromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, writeInt32MSB20FromSampleInt24)
{
	const tuint32 c_int24Samples[15] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000,
		0x00000008, 0x00000007, 0x00000000, 0xfffffff8, 0xfffffff7
	};

	const tubyte c_expectIntegerSamples[60] = {
		0x00, 0x07, 0xff, 0xff,
		0x00, 0x06, 0x66, 0x66,
		0x00, 0x04, 0xcc, 0xcd,
		0x00, 0x03, 0x33, 0x33,
		0x00, 0x01, 0x99, 0x9a,
		0xff, 0xfe, 0x66, 0x66,
		0xff, 0xfc, 0xcc, 0xcd, 
		0xff, 0xfb, 0x33, 0x33,
		0xff, 0xf9, 0x99, 0x9a, 
		0xff, 0xf8, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 
		0xff, 0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32MSB20FromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

TEST(FormatType, writeInt32MSB20FromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00000800, 0x000007ff, 0x00000000, 0xfffff800, 0xfffff7ff
	};

	const tubyte c_expectIntegerSamples[60] = {
		0x00, 0x07, 0xff, 0xff,
		0x00, 0x06, 0x66, 0x66,
		0x00, 0x04, 0xcc, 0xcd,
		0x00, 0x03, 0x33, 0x33,
		0x00, 0x01, 0x99, 0x9a,
		0xff, 0xfe, 0x66, 0x66,
		0xff, 0xfc, 0xcc, 0xcd, 
		0xff, 0xfb, 0x33, 0x33,
		0xff, 0xf9, 0x99, 0x9a, 
		0xff, 0xf8, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 
		0xff, 0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32MSB20FromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, writeInt32LSB24FromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};

	const tubyte c_expectIntegerSamples[40] = {
		0x00, 0xff, 0x7f, 0x00,
		0x00, 0x66, 0x66, 0x00,
		0x00, 0xcc, 0x4c, 0x00,
		0x00, 0x33, 0x33, 0x00,
		0x00, 0x99, 0x19, 0x00,
		0x00, 0x66, 0xe6, 0xff,
		0x00, 0xcd, 0xcc, 0xff,
		0x00, 0x33, 0xb3, 0xff,
		0x00, 0x9a, 0x99, 0xff,
		0x00, 0x00, 0x80, 0xff
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		writeInt32LSB24FromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, writeInt32LSB24FromSampleInt24)
{
	const tuint32 c_int24Samples[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamples[60] = {
		0xff, 0xff, 0x7f, 0x00,
		0x66, 0x66, 0x66, 0x00,
		0xcc, 0xcc, 0x4c, 0x00,
		0x33, 0x33, 0x33, 0x00,
		0x99, 0x99, 0x19, 0x00,
		0x66, 0x66, 0xe6, 0xff,
		0xcd, 0xcc, 0xcc, 0xff,
		0x33, 0x33, 0xb3, 0xff,
		0x9a, 0x99, 0x99, 0xff,
		0x00, 0x00, 0x80, 0xff
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		writeInt32LSB24FromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, writeInt32LSB24FromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80, 0xffffff7f
	};

	const tubyte c_expectIntegerSamples[60] = {
		0xff, 0xff, 0x7f, 0x00,
		0x66, 0x66, 0x66, 0x00,
		0xcd, 0xcc, 0x4c, 0x00,
		0x33, 0x33, 0x33, 0x00,
		0x9a, 0x99, 0x19, 0x00,
		0x66, 0x66, 0xe6, 0xff,
		0xcd, 0xcc, 0xcc, 0xff,
		0x33, 0x33, 0xb3, 0xff,
		0x9a, 0x99, 0x99, 0xff,
		0x00, 0x00, 0x80, 0xff,
		0x01, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 
		0xff, 0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32LSB24FromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, writeInt32MSB24FromSampleInt16)
{
	const tuint16 c_int16Samples[10] = {
		0x7fff, 0x6666, 0x4ccc, 0x3333, 0x1999,
		0xe666, 0xcccd, 0xb333, 0x999a, 0x8000
	};

	const tubyte c_expectIntegerSamples[40] = {
		0x00, 0x7f, 0xff, 0x00,
		0x00, 0x66, 0x66, 0x00,
		0x00, 0x4c, 0xcc, 0x00,
		0x00, 0x33, 0x33, 0x00,
		0x00, 0x19, 0x99, 0x00,
		0xff, 0xe6, 0x66, 0x00,
		0xff, 0xcc, 0xcd, 0x00,
		0xff, 0xb3, 0x33, 0x00,
		0xff, 0x99, 0x9a, 0x00,
		0xff, 0x80, 0x00, 0x00
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		writeInt32MSB24FromSampleInt16(static_cast<tint16>(c_int16Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, writeInt32MSB24FromSampleInt24)
{
	const tuint32 c_int24Samples[10] = {
		0x007fffff, 0x00666666, 0x004ccccc, 0x00333333, 0x00199999,
		0xffe66666, 0xffcccccd, 0xffb33333, 0xff99999a, 0xff800000
	};

	const tubyte c_expectIntegerSamples[40] = {
		0x00, 0x7f, 0xff, 0xff,
		0x00, 0x66, 0x66, 0x66,
		0x00, 0x4c, 0xcc, 0xcc,
		0x00, 0x33, 0x33, 0x33,
		0x00, 0x19, 0x99, 0x99,
		0xff, 0xe6, 0x66, 0x66,
		0xff, 0xcc, 0xcc, 0xcd,
		0xff, 0xb3, 0x33, 0x33,
		0xff, 0x99, 0x99, 0x9a,
		0xff, 0x80, 0x00, 0x00
	};
	
	int i;
	tbyte mem[40];
	
	for(i = 0; i < 10; i++)
	{
		writeInt32MSB24FromSampleInt24(static_cast<tint32>(c_int24Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 40));
}

TEST(FormatType, writeInt32MSB24FromSampleInt32)
{
	const tuint32 c_int32Samples[15] = {
		0x7fffffff, 0x66666666, 0x4ccccccc, 0x33333333, 0x19999999,
		0xe6666666, 0xcccccccd, 0xb3333333, 0x9999999a, 0x80000000,
		0x00000080, 0x0000007f, 0x00000000, 0xffffff80, 0xffffff7f
	};

	const tubyte c_expectIntegerSamples[60] = {
		0x00, 0x7f, 0xff, 0xff,
		0x00, 0x66, 0x66, 0x66,
		0x00, 0x4c, 0xcc, 0xcd,
		0x00, 0x33, 0x33, 0x33,
		0x00, 0x19, 0x99, 0x9a,
		0xff, 0xe6, 0x66, 0x66,
		0xff, 0xcc, 0xcc, 0xcd,
		0xff, 0xb3, 0x33, 0x33,
		0xff, 0x99, 0x99, 0x9a,
		0xff, 0x80, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 
		0xff, 0xff, 0xff, 0xff
	};
	
	int i;
	tbyte mem[60];
	
	for(i = 0; i < 15; i++)
	{
		writeInt32MSB24FromSampleInt32(static_cast<tint32>(c_int32Samples[i]), &mem[i * 4]);
	}
	EXPECT_EQ(0, memcmp(mem, c_expectIntegerSamples, 60));
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16From8BitSample)
{
	const tubyte c_input[10] = {
		0x7f,
		0x66,
		0x4d,
		0x33,
		0x19,
		0xe6,
		0xcc,
		0xb3,
		0x99,
		0x80
	};
    const tuint16 c_expectOutputA[10] = {
		0x7f00,
		0x6600,
		0x4d00,
		0x3300,
		0x1900,
		0xe600,
		0xcc00,
		0xb300,
		0x9900,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleLittleEndian(&c_input[i], 8), c_expectOutput[i]);
		EXPECT_EQ(readInt16SampleBigEndian(&c_input[i], 8), c_expectOutput[i]);
	}	
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24From8BitSample)
{
	const tubyte c_input[10] = {
		0x7f,
		0x66,
		0x4d,
		0x33,
		0x19,
		0xe6,
		0xcc,
		0xb3,
		0x99,
		0x80
	};
    const tuint32 c_expectOutputA[10] = {
		0x007f0000,
		0x00660000,
		0x004d0000,
		0x00330000,
		0x00190000,
		0xffe60000,
		0xffcc0000,
		0xffb30000,
		0xff990000,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleLittleEndian(&c_input[i], 8), c_expectOutput[i]);
		EXPECT_EQ(readInt24SampleBigEndian(&c_input[i], 8), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32From8BitSample)
{
	const tubyte c_input[10] = {
		0x7f,
		0x66,
		0x4d,
		0x33,
		0x19,
		0xe6,
		0xcc,
		0xb3,
		0x99,
		0x80
	};
	const tuint32 c_expectOutputA[10] = {
		0x7f000000,
		0x66000000,
		0x4d000000,
		0x33000000,
		0x19000000,
		0xe6000000,
		0xcc000000,
		0xb3000000,
		0x99000000,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleLittleEndian(&c_input[i], 8), c_expectOutput[i]);
		EXPECT_EQ(readInt32SampleBigEndian(&c_input[i], 8), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromLE12BitSample)
{
	const tubyte c_inputLE[20] = {
        0xff, 0x07,
		0x66, 0x06,
		0xcd, 0x04,
		0x33, 0x03,
		0x9a, 0x01,
		0x66, 0xfe,
		0xcd, 0xfc,
		0x33, 0xfb,
		0x9a, 0xf9,
        0x00, 0xf8
	};

	const tuint16 c_expectOutputA[10] = {
		0x7ff0,
		0x6660,
		0x4cd0,
		0x3330,
		0x19a0,
		0xe660,
		0xccd0,
		0xb330,
		0x99a0,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleLittleEndian(&c_inputLE[i * 2], 12), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromBE12BitSample)
{
	const tubyte c_inputBE[20] = {
		0x07, 0xff,
		0x06, 0x66,
		0x04, 0xcd,
		0x03, 0x33,
		0x01, 0x9a,
		0xfe, 0x66,
		0xfc, 0xcd,
		0xfb, 0x33,
		0xf9, 0x9a,
        0xf8, 0x00
	};

	const tuint16 c_expectOutputA[10] = {
		0x7ff0,
		0x6660,
		0x4cd0,
		0x3330,
		0x19a0,
		0xe660,
		0xccd0,
		0xb330,
		0x99a0,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleBigEndian(&c_inputBE[i * 2], 12), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromLE12BitSample)
{
	const tubyte c_inputLE[20] = {
		0xff, 0x07,
		0x66, 0x06,
		0xcd, 0x04,
		0x33, 0x03,
		0x9a, 0x01,
		0x66, 0xfe,
		0xcd, 0xfc,
		0x33, 0xfb,
		0x9a, 0xf9,
        0x00, 0xf8
	};

	const tuint32 c_expectOutputA[10] = {
		0x007ff000,
		0x00666000,
		0x004cd000,
		0x00333000,
		0x0019a000,
		0xffe66000,
		0xffccd000,
		0xffb33000,
		0xff99a000,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleLittleEndian(&c_inputLE[i * 2], 12), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromBE12BitSample)
{
	const tubyte c_inputBE[20] = {
		0x07, 0xff,
		0x06, 0x66,
		0x04, 0xcd,
		0x03, 0x33,
		0x01, 0x9a,
		0xfe, 0x66,
		0xfc, 0xcd,
		0xfb, 0x33,
		0xf9, 0x9a,
        0xf8, 0x00
	};

	const tuint32 c_expectOutputA[10] = {
		0x007ff000,
		0x00666000,
		0x004cd000,
		0x00333000,
		0x0019a000,
		0xffe66000,
		0xffccd000,
		0xffb33000,
		0xff99a000,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleBigEndian(&c_inputBE[i * 2], 12), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromLE12BitSample)
{
	const tubyte c_inputLE[20] = {
		0xff, 0x07,
		0x66, 0x06,
		0xcd, 0x04,
		0x33, 0x03,
		0x9a, 0x01,
		0x66, 0xfe,
		0xcd, 0xfc,
		0x33, 0xfb,
		0x9a, 0xf9,
        0x00, 0xf8
	};

	const tuint32 c_expectOutputA[10] = {
		0x7ff00000,
		0x66600000,
		0x4cd00000,
		0x33300000,
		0x19a00000,
		0xe6600000,
		0xccd00000,
		0xb3300000,
		0x99a00000,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleLittleEndian(&c_inputLE[i * 2], 12), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromBE12BitSample)
{
	const tubyte c_inputBE[20] = {
		0x07, 0xff,
		0x06, 0x66,
		0x04, 0xcd,
		0x03, 0x33,
		0x01, 0x9a,
		0xfe, 0x66,
		0xfc, 0xcd,
		0xfb, 0x33,
		0xf9, 0x9a,
        0xf8, 0x00
	};

	const tuint32 c_expectOutputA[10] = {
		0x7ff00000,
		0x66600000,
		0x4cd00000,
		0x33300000,
		0x19a00000,
		0xe6600000,
		0xccd00000,
		0xb3300000,
		0x99a00000,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleBigEndian(&c_inputBE[i * 2], 12), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromLE16BitSample)
{
	const tubyte c_inputLE[20] = {
		0xff, 0x7f,
		0x66, 0x66,
		0xcd, 0x4c,
		0x33, 0x33,
		0x9a, 0x19,
		0x66, 0xe6,
		0xcd, 0xcc,
		0x33, 0xb3,
        0x9a, 0x99,
		0x00, 0x80
    };

	const tuint16 c_expectOutputA[10] = {
		0x7fff,
		0x6666,
		0x4ccd,
		0x3333,
		0x199a,
		0xe666,
		0xcccd,
		0xb333,
		0x999a,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleLittleEndian(&c_inputLE[i * 2], 16), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromBE16BitSample)
{
	const tubyte c_inputBE[20] = {
		0x7f, 0xff,
		0x66, 0x66,
		0x4c, 0xcd,
		0x33, 0x33,
		0x19, 0x9a,
		0xe6, 0x66,
		0xcc, 0xcd,
		0xb3, 0x33,
		0x99, 0x9a,
		0x80, 0x00
	};

	const tuint16 c_expectOutputA[10] = {
		0x7fff,
		0x6666,
		0x4ccd,
		0x3333,
		0x199a,
		0xe666,
		0xcccd,
		0xb333,
		0x999a,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleBigEndian(&c_inputBE[i * 2], 16), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromLE16BitSample)
{
	const tubyte c_inputLE[20] = {
		0xff, 0x7f,
		0x66, 0x66,
		0xcd, 0x4c,
		0x33, 0x33,
		0x9a, 0x19,
		0x66, 0xe6,
		0xcd, 0xcc,
		0x33, 0xb3,
        0x9a, 0x99,
		0x00, 0x80
    };

	const tuint32 c_expectOutputA[10] = {
		0x007fff00,
		0x00666600,
		0x004ccd00,
		0x00333300,
		0x00199a00,
		0xffe66600,
		0xffcccd00,
		0xffb33300,
		0xff999a00,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleLittleEndian(&c_inputLE[i * 2], 16), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromBE16BitSample)
{
	const tubyte c_inputBE[20] = {
		0x7f, 0xff,
		0x66, 0x66,
		0x4c, 0xcd,
		0x33, 0x33,
		0x19, 0x9a,
		0xe6, 0x66,
		0xcc, 0xcd,
		0xb3, 0x33,
		0x99, 0x9a,
		0x80, 0x00
	};

	const tuint32 c_expectOutputA[10] = {
		0x007fff00,
		0x00666600,
		0x004ccd00,
		0x00333300,
		0x00199a00,
		0xffe66600,
		0xffcccd00,
		0xffb33300,
		0xff999a00,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleBigEndian(&c_inputBE[i * 2], 16), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromLE16BitSample)
{
	const tubyte c_inputLE[20] = {
		0xff, 0x7f,
		0x66, 0x66,
		0xcd, 0x4c,
		0x33, 0x33,
		0x9a, 0x19,
		0x66, 0xe6,
		0xcd, 0xcc,
		0x33, 0xb3,
        0x9a, 0x99,
		0x00, 0x80
	};

	const tuint32 c_expectOutputA[10] = {
		0x7fff0000,
		0x66660000,
		0x4ccd0000,
		0x33330000,
		0x199a0000,
		0xe6660000,
		0xcccd0000,
		0xb3330000,
		0x999a0000,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleLittleEndian(&c_inputLE[i * 2], 16), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromBE16BitSample)
{
	const tubyte c_inputBE[20] = {
		0x7f, 0xff,
		0x66, 0x66,
		0x4c, 0xcd,
		0x33, 0x33,
		0x19, 0x9a,
		0xe6, 0x66,
		0xcc, 0xcd,
		0xb3, 0x33,
		0x99, 0x9a,
		0x80, 0x00
	};

	const tuint32 c_expectOutputA[10] = {
		0x7fff0000,
		0x66660000,
		0x4ccd0000,
		0x33330000,
		0x199a0000,
		0xe6660000,
		0xcccd0000,
		0xb3330000,
		0x999a0000,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleBigEndian(&c_inputBE[i * 2], 16), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromLE20BitSample)
{
	const tubyte c_inputLE[30] = {
		0xff, 0xff, 0x07,
		0x66, 0x66, 0x06,
		0xcd, 0xcc, 0x04,
		0x33, 0x33, 0x03,
		0x9a, 0x99, 0x01,
		0x66, 0x66, 0xfe,
		0xcd, 0xcc, 0xfc,
		0x33, 0x33, 0xfb,
		0x9a, 0x99, 0xf9,
		0x00, 0x00, 0xf8
	};

	const tuint16 c_expectOutputA[10] = {
		0x7fff,
		0x6666,
		0x4ccd,
		0x3333,
		0x199a,
		0xe666,
		0xcccd,
		0xb333,
		0x999a,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleLittleEndian(&c_inputLE[i * 3], 20), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromBE20BitSample)
{
	const tubyte c_inputBE[30] = {
		0x07, 0xff, 0xff,
		0x06, 0x66, 0x66,
		0x04, 0xcc, 0xcd,
		0x03, 0x33, 0x33,
		0x01, 0x99, 0x9a,
		0xfe, 0x66, 0x66,
		0xfc, 0xcc, 0xcd,
		0xfb, 0x33, 0x33,
		0xf9, 0x99, 0x9a,
		0xf8, 0x00, 0x00
	};
	
	const tuint16 c_expectOutputA[10] = {
		0x7fff,
		0x6666,
		0x4ccd,
		0x3333,
		0x199a,
		0xe666,
		0xcccd,
		0xb333,
		0x999a,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleBigEndian(&c_inputBE[i * 3], 20), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromLE20BitSample)
{
	const tubyte c_inputLE[30] = {
		0xff, 0xff, 0x07,
		0x66, 0x66, 0x06,
		0xcd, 0xcc, 0x04,
		0x33, 0x33, 0x03,
		0x9a, 0x99, 0x01,
		0x66, 0x66, 0xfe,
		0xcd, 0xcc, 0xfc,
		0x33, 0x33, 0xfb,
		0x9a, 0x99, 0xf9,
		0x00, 0x00, 0xf8
	};

	const tuint32 c_expectOutputA[10] = {
		0x007ffff0,
		0x00666660,
		0x004cccd0,
		0x00333330,
		0x001999a0,
		0xffe66660,
		0xffccccd0,
		0xffb33330,
		0xff9999a0,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleLittleEndian(&c_inputLE[i * 3], 20), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromBE20BitSample)
{
	const tubyte c_inputBE[30] = {
		0x07, 0xff, 0xff,
		0x06, 0x66, 0x66,
		0x04, 0xcc, 0xcd,
		0x03, 0x33, 0x33,
		0x01, 0x99, 0x9a,
		0xfe, 0x66, 0x66,
		0xfc, 0xcc, 0xcd,
		0xfb, 0x33, 0x33,
		0xf9, 0x99, 0x9a,
		0xf8, 0x00, 0x00
	};

	const tuint32 c_expectOutputA[10] = {
		0x007ffff0,
		0x00666660,
		0x004cccd0,
		0x00333330,
		0x001999a0,
		0xffe66660,
		0xffccccd0,
		0xffb33330,
		0xff9999a0,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleBigEndian(&c_inputBE[i * 3], 20), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromLE20BitSample)
{
	const tubyte c_inputLE[30] = {
		0xff, 0xff, 0x07,
		0x66, 0x66, 0x06,
		0xcd, 0xcc, 0x04,
		0x33, 0x33, 0x03,
		0x9a, 0x99, 0x01,
		0x66, 0x66, 0xfe,
		0xcd, 0xcc, 0xfc,
		0x33, 0x33, 0xfb,
		0x9a, 0x99, 0xf9,
		0x00, 0x00, 0xf8
	};

	const tuint32 c_expectOutputA[10] = {
		0x7ffff000,
		0x66666000,
		0x4cccd000,
		0x33333000,
		0x1999a000,
		0xe6666000,
		0xccccd000,
		0xb3333000,
		0x9999a000,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleLittleEndian(&c_inputLE[i * 3], 20), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromBE20BitSample)
{
	const tubyte c_inputBE[30] = {
		0x07, 0xff, 0xff,
		0x06, 0x66, 0x66,
		0x04, 0xcc, 0xcd,
		0x03, 0x33, 0x33,
		0x01, 0x99, 0x9a,
		0xfe, 0x66, 0x66,
		0xfc, 0xcc, 0xcd,
		0xfb, 0x33, 0x33,
		0xf9, 0x99, 0x9a,
		0xf8, 0x00, 0x00
	};

	const tuint32 c_expectOutputA[10] = {
		0x7ffff000,
		0x66666000,
		0x4cccd000,
		0x33333000,
		0x1999a000,
		0xe6666000,
		0xccccd000,
		0xb3333000,
		0x9999a000,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleBigEndian(&c_inputBE[i * 3], 20), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromLE24BitSample)
{
	const tubyte c_inputLE[30] = {
		0xff, 0xff, 0x7f,
		0x66, 0x66, 0x66,
		0xcd, 0xcc, 0x4c,
		0x33, 0x33, 0x33,
		0x9a, 0x99, 0x19,
		0x66, 0x66, 0xe6,
		0xcd, 0xcc, 0xcc,
		0x33, 0x33, 0xb3,
		0x9a, 0x99, 0x99,
		0x00, 0x00, 0x80
	};

	const tuint16 c_expectOutputA[10] = {
		0x7fff,
		0x6666,
		0x4ccd,
		0x3333,
		0x199a,
		0xe666,
		0xcccd,
		0xb333,
		0x999a,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleLittleEndian(&c_inputLE[i * 3], 24), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromBE24BitSample)
{
	const tubyte c_inputBE[30] = {
		0x7f, 0xff, 0xff,
		0x66, 0x66, 0x66,
		0x4c, 0xcc, 0xcd,
		0x33, 0x33, 0x33,
		0x19, 0x99, 0x9a,
		0xe6, 0x66, 0x66,
		0xcc, 0xcc, 0xcd,
		0xb3, 0x33, 0x33,
		0x99, 0x99, 0x9a,
		0x80, 0x00, 0x00
	};

    const tuint16 c_expectOutputA[10] = {
		0x7fff,
		0x6666,
		0x4ccd,
		0x3333,
		0x199a,
		0xe666,
		0xcccd,
		0xb333,
		0x999a,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleBigEndian(&c_inputBE[i * 3], 24), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromLE24BitSample)
{
	const tubyte c_inputLE[30] = {
		0xff, 0xff, 0x7f,
		0x66, 0x66, 0x66,
		0xcd, 0xcc, 0x4c,
		0x33, 0x33, 0x33,
		0x9a, 0x99, 0x19,
		0x66, 0x66, 0xe6,
		0xcd, 0xcc, 0xcc,
		0x33, 0x33, 0xb3,
		0x9a, 0x99, 0x99,
		0x00, 0x00, 0x80
	};

	const tuint32 c_expectOutputA[10] = {
		0x007fffff,
		0x00666666,
		0x004ccccd,
		0x00333333,
		0x0019999a,
		0xffe66666,
		0xffcccccd,
		0xffb33333,
		0xff99999a,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleLittleEndian(&c_inputLE[i * 3], 24), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromBE24BitSample)
{
	const tubyte c_inputBE[30] = {
		0x7f, 0xff, 0xff,
		0x66, 0x66, 0x66,
		0x4c, 0xcc, 0xcd,
		0x33, 0x33, 0x33,
		0x19, 0x99, 0x9a,
		0xe6, 0x66, 0x66,
		0xcc, 0xcc, 0xcd,
		0xb3, 0x33, 0x33,
		0x99, 0x99, 0x9a,
		0x80, 0x00, 0x00
	};

    const tuint32 c_expectOutputA[10] = {
		0x007fffff,
		0x00666666,
		0x004ccccd,
		0x00333333,
		0x0019999a,
		0xffe66666,
		0xffcccccd,
		0xffb33333,
		0xff99999a,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleBigEndian(&c_inputBE[i * 3], 24), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromLE24BitSample)
{
	const tubyte c_inputLE[30] = {
		0xff, 0xff, 0x7f,
		0x66, 0x66, 0x66,
		0xcd, 0xcc, 0x4c,
		0x33, 0x33, 0x33,
		0x9a, 0x99, 0x19,
		0x66, 0x66, 0xe6,
		0xcd, 0xcc, 0xcc,
		0x33, 0x33, 0xb3,
		0x9a, 0x99, 0x99,
		0x00, 0x00, 0x80
	};

    const tuint32 c_expectOutputA[10] = {
		0x7fffff00,
		0x66666600,
		0x4ccccd00,
		0x33333300,
		0x19999a00,
		0xe6666600,
		0xcccccd00,
		0xb3333300,
		0x99999a00,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleLittleEndian(&c_inputLE[i * 3], 24), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromBE24BitSample)
{
	const tubyte c_inputBE[30] = {
		0x7f, 0xff, 0xff,
		0x66, 0x66, 0x66,
		0x4c, 0xcc, 0xcd,
		0x33, 0x33, 0x33,
		0x19, 0x99, 0x9a,
		0xe6, 0x66, 0x66,
		0xcc, 0xcc, 0xcd,
		0xb3, 0x33, 0x33,
		0x99, 0x99, 0x9a,
		0x80, 0x00, 0x00
	};

    const tuint32 c_expectOutputA[10] = {
		0x7fffff00,
		0x66666600,
		0x4ccccd00,
		0x33333300,
		0x19999a00,
		0xe6666600,
		0xcccccd00,
		0xb3333300,
		0x99999a00,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleBigEndian(&c_inputBE[i * 3], 24), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromLE28BitSample)
{
	const tubyte c_inputLE[40] = {
		0xff, 0xff, 0xff, 0x07,
		0x66, 0x66, 0x66, 0x06,
		0xcd, 0xcc, 0xcc, 0x04,
		0x33, 0x33, 0x33, 0x03,
		0x9a, 0x99, 0x99, 0x01,
		0x66, 0x66, 0x66, 0xfe,
		0xcd, 0xcc, 0xcc, 0xfc,
		0x33, 0x33, 0x33, 0xfb,
		0x9a, 0x99, 0x99, 0xf9,
		0x00, 0x00, 0x00, 0xf8,
	};

	const tuint16 c_expectOutputA[10] = {
		0x7fff,
		0x6666,
		0x4ccd,
		0x3333,
		0x199a,
		0xe666,
		0xcccd,
		0xb333,
		0x999a,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleLittleEndian(&c_inputLE[i * 4], 28), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromBE28BitSample)
{
	const tubyte c_inputBE[40] = {
		0x07, 0xff, 0xff, 0xff,
		0x06, 0x66, 0x66, 0x66,
		0x04, 0xcc, 0xcc, 0xcd,
		0x03, 0x33, 0x33, 0x33,
		0x01, 0x99, 0x99, 0x9a,
		0xfe, 0x66, 0x66, 0x66,
		0xfc, 0xcc, 0xcc, 0xcd,
		0xfb, 0x33, 0x33, 0x33,
		0xf9, 0x99, 0x99, 0x9a,
		0xf8, 0x00, 0x00, 0x00
	};
	
	const tuint16 c_expectOutputA[10] = {
		0x7fff,
		0x6666,
		0x4ccd,
		0x3333,
		0x199a,
		0xe666,
		0xcccd,
		0xb333,
		0x999a,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleBigEndian(&c_inputBE[i * 4], 28), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromLE28BitSample)
{
	const tubyte c_inputLE[40] = {
		0xff, 0xff, 0xff, 0x07,
		0x66, 0x66, 0x66, 0x06,
		0xcd, 0xcc, 0xcc, 0x04,
		0x33, 0x33, 0x33, 0x03,
		0x9a, 0x99, 0x99, 0x01,
		0x66, 0x66, 0x66, 0xfe,
		0xcd, 0xcc, 0xcc, 0xfc,
		0x33, 0x33, 0x33, 0xfb,
		0x9a, 0x99, 0x99, 0xf9,
		0x00, 0x00, 0x00, 0xf8,
	};

    const tuint32 c_expectOutputA[10] = {
		0x007fffff,
		0x00666666,
		0x004ccccd,
		0x00333333,
		0x0019999a,
		0xffe66666,
		0xffcccccd,
		0xffb33333,
		0xff99999a,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleLittleEndian(&c_inputLE[i * 4], 28), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromBE28BitSample)
{
	const tubyte c_inputBE[40] = {
		0x07, 0xff, 0xff, 0xff,
		0x06, 0x66, 0x66, 0x66,
		0x04, 0xcc, 0xcc, 0xcd,
		0x03, 0x33, 0x33, 0x33,
		0x01, 0x99, 0x99, 0x9a,
		0xfe, 0x66, 0x66, 0x66,
		0xfc, 0xcc, 0xcc, 0xcd,
		0xfb, 0x33, 0x33, 0x33,
		0xf9, 0x99, 0x99, 0x9a,
		0xf8, 0x00, 0x00, 0x00
	};

    const tuint32 c_expectOutputA[10] = {
		0x007fffff,
		0x00666666,
		0x004ccccd,
		0x00333333,
		0x0019999a,
		0xffe66666,
		0xffcccccd,
		0xffb33333,
		0xff99999a,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
        EXPECT_EQ(readInt24SampleBigEndian(&c_inputBE[i * 4], 28), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromLE28BitSample)
{
	const tubyte c_inputLE[40] = {
		0xff, 0xff, 0xff, 0x07,
		0x66, 0x66, 0x66, 0x06,
		0xcd, 0xcc, 0xcc, 0x04,
		0x33, 0x33, 0x33, 0x03,
		0x9a, 0x99, 0x99, 0x01,
		0x66, 0x66, 0x66, 0xfe,
		0xcd, 0xcc, 0xcc, 0xfc,
		0x33, 0x33, 0x33, 0xfb,
		0x9a, 0x99, 0x99, 0xf9,
		0x00, 0x00, 0x00, 0xf8,
	};

    const tuint32 c_expectOutputA[10] = {
		0x7ffffff0,
		0x66666660,
		0x4cccccd0,
		0x33333330,
		0x199999a0,
		0xe6666660,
		0xccccccd0,
		0xb3333330,
		0x999999a0,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleLittleEndian(&c_inputLE[i * 4], 28), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromBE28BitSample)
{
	const tubyte c_inputBE[40] = {
		0x07, 0xff, 0xff, 0xff,
		0x06, 0x66, 0x66, 0x66,
		0x04, 0xcc, 0xcc, 0xcd,
		0x03, 0x33, 0x33, 0x33,
		0x01, 0x99, 0x99, 0x9a,
		0xfe, 0x66, 0x66, 0x66,
		0xfc, 0xcc, 0xcc, 0xcd,
		0xfb, 0x33, 0x33, 0x33,
		0xf9, 0x99, 0x99, 0x9a,
		0xf8, 0x00, 0x00, 0x00
	};

    const tuint32 c_expectOutputA[10] = {
		0x7ffffff0,
		0x66666660,
		0x4cccccd0,
		0x33333330,
		0x199999a0,
		0xe6666660,
		0xccccccd0,
		0xb3333330,
		0x999999a0,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleBigEndian(&c_inputBE[i * 4], 28), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromLE32BitSample)
{
	const tubyte c_inputLE[40] = {
		0xff, 0xff, 0xff, 0x7f,
		0x66, 0x66, 0x66, 0x66,
		0xcd, 0xcc, 0xcc, 0x4c,
		0x33, 0x33, 0x33, 0x33,
		0x9a, 0x99, 0x99, 0x19,
		0x66, 0x66, 0x66, 0xe6,
		0xcd, 0xcc, 0xcc, 0xcc,
		0x33, 0x33, 0x33, 0xb3,
		0x9a, 0x99, 0x99, 0x99,
		0x00, 0x00, 0x00, 0x80,
	};

    const tuint16 c_expectOutputA[10] = {
		0x7fff,
		0x6666,
		0x4ccd,
		0x3333,
		0x199a,
		0xe666,
		0xcccd,
		0xb333,
		0x999a,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleLittleEndian(&c_inputLE[i * 4], 32), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt16FromBE32BitSample)
{
	const tubyte c_inputBE[40] = {
		0x7f, 0xff, 0xff, 0xff,
		0x66, 0x66, 0x66, 0x66,
		0x4c, 0xcc, 0xcc, 0xcd,
		0x33, 0x33, 0x33, 0x33,
		0x19, 0x99, 0x99, 0x9a,
		0xe6, 0x66, 0x66, 0x66,
		0xcc, 0xcc, 0xcc, 0xcd,
		0xb3, 0x33, 0x33, 0x33,
		0x99, 0x99, 0x99, 0x9a,
		0x80, 0x00, 0x00, 0x00
	};

    const tuint16 c_expectOutputA[10] = {
		0x7fff,
		0x6666,
		0x4ccd,
		0x3333,
		0x199a,
		0xe666,
		0xcccd,
		0xb333,
		0x999a,
		0x8000
	};
	const tint16 *c_expectOutput = reinterpret_cast<const tint16 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt16SampleBigEndian(&c_inputBE[i * 4], 32), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromLE32BitSample)
{
	const tubyte c_inputLE[40] = {
		0xff, 0xff, 0xff, 0x7f,
		0x66, 0x66, 0x66, 0x66,
		0xcd, 0xcc, 0xcc, 0x4c,
		0x33, 0x33, 0x33, 0x33,
		0x9a, 0x99, 0x99, 0x19,
		0x66, 0x66, 0x66, 0xe6,
		0xcd, 0xcc, 0xcc, 0xcc,
		0x33, 0x33, 0x33, 0xb3,
		0x9a, 0x99, 0x99, 0x99,
		0x00, 0x00, 0x00, 0x80,
	};

    const tuint32 c_expectOutputA[10] = {
		0x007fffff,
		0x00666666,
		0x004ccccd,
		0x00333333,
		0x0019999a,
		0xffe66666,
		0xffcccccd,
		0xffb33333,
		0xff99999a,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleLittleEndian(&c_inputLE[i * 4], 32), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt24FromBE32BitSample)
{
	const tubyte c_inputBE[40] = {
		0x7f, 0xff, 0xff, 0xff,
		0x66, 0x66, 0x66, 0x66,
		0x4c, 0xcc, 0xcc, 0xcd,
		0x33, 0x33, 0x33, 0x33,
		0x19, 0x99, 0x99, 0x9a,
		0xe6, 0x66, 0x66, 0x66,
		0xcc, 0xcc, 0xcc, 0xcd,
		0xb3, 0x33, 0x33, 0x33,
		0x99, 0x99, 0x99, 0x9a,
		0x80, 0x00, 0x00, 0x00
	};

	const tuint32 c_expectOutputA[10] = {
		0x007fffff,
		0x00666666,
		0x004ccccd,
		0x00333333,
		0x0019999a,
		0xffe66666,
		0xffcccccd,
		0xffb33333,
		0xff99999a,
		0xff800000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt24SampleBigEndian(&c_inputBE[i * 4], 32), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromLE32BitSample)
{
	const tubyte c_inputLE[40] = {
		0xff, 0xff, 0xff, 0x7f,
		0x66, 0x66, 0x66, 0x66,
		0xcd, 0xcc, 0xcc, 0x4c,
		0x33, 0x33, 0x33, 0x33,
		0x9a, 0x99, 0x99, 0x19,
		0x66, 0x66, 0x66, 0xe6,
		0xcd, 0xcc, 0xcc, 0xcc,
		0x33, 0x33, 0x33, 0xb3,
		0x9a, 0x99, 0x99, 0x99,
		0x00, 0x00, 0x00, 0x80,
	};

	const tuint32 c_expectOutputA[10] = {
		0x7fffffff,
		0x66666666,
		0x4ccccccd,
		0x33333333,
		0x1999999a,
		0xe6666666,
		0xcccccccd,
		0xb3333333,
		0x9999999a,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
    for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleLittleEndian(&c_inputLE[i * 4], 32), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatType, readInt32FromBE32BitSample)
{
	const tubyte c_inputBE[40] = {
		0x7f, 0xff, 0xff, 0xff,
		0x66, 0x66, 0x66, 0x66,
		0x4c, 0xcc, 0xcc, 0xcd,
		0x33, 0x33, 0x33, 0x33,
		0x19, 0x99, 0x99, 0x9a,
		0xe6, 0x66, 0x66, 0x66,
		0xcc, 0xcc, 0xcc, 0xcd,
		0xb3, 0x33, 0x33, 0x33,
		0x99, 0x99, 0x99, 0x9a,
		0x80, 0x00, 0x00, 0x00
	};

	const tuint32 c_expectOutputA[10] = {
		0x7fffffff,
		0x66666666,
		0x4ccccccd,
		0x33333333,
		0x1999999a,
		0xe6666666,
		0xcccccccd,
		0xb3333333,
		0x9999999a,
		0x80000000
	};
	const tint32 *c_expectOutput = reinterpret_cast<const tint32 *>(c_expectOutputA);
	
	for(tint i = 0; i < 10; i++)
	{
		EXPECT_EQ(readInt32SampleBigEndian(&c_inputBE[i * 4], 32), c_expectOutput[i]);
	}
}

//-------------------------------------------------------------------------------------------
