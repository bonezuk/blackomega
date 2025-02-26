#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "audioio/inc/FormatDescriptionUtils.h"

using namespace omega;
using namespace audioio;
using namespace testing;

//-------------------------------------------------------------------------------------------

class FormatDescriptionUtilsTest : public FormatDescriptionUtils
{
	public:
		FormatDescriptionUtilsTest();
		
		static int *testClosestFrequencyOrder(int freq);
		static void testClosestBitOrder(tint bitIndex,QVector<tint>& hList,QVector<tint>& lList);
		static void testFindClosestFormatTypeChannelList(const FormatDescription& format, bool isAscending, QVector<tint>& chList);
};

//-------------------------------------------------------------------------------------------

FormatDescriptionUtilsTest::FormatDescriptionUtilsTest() : FormatDescriptionUtils()
{}

//-------------------------------------------------------------------------------------------

int *FormatDescriptionUtilsTest::testClosestFrequencyOrder(int freq)
{
	return closestFrequencyOrder(freq);
}

//-------------------------------------------------------------------------------------------

void FormatDescriptionUtilsTest::testClosestBitOrder(tint bitIndex,QVector<tint>& hList,QVector<tint>& lList)
{
	closestBitOrder(bitIndex,hList,lList);
}

//-------------------------------------------------------------------------------------------

void FormatDescriptionUtilsTest::testFindClosestFormatTypeChannelList(const FormatDescription& format, bool isAscending, QVector<tint>& chList)
{
	findClosestFormatTypeChannelList(format,isAscending,chList);
}

//-------------------------------------------------------------------------------------------
TEST(FormatDescriptionUtils,closestFrequencyOrderWhen44100Hz)
{
	const tint expectedOrder[NUMBER_MAXFREQUENCIES] = {
		44100, 88200, 176400, 352800, 705600, 1411200, 2822400,
		48000, 64000, 96000, 192000, 384000, 768000, 1536000, 3072000,
		32000, 24000, 22050, 16000, 12000, 11025, 8000
	};
	
    int *order = FormatDescriptionUtilsTest::testClosestFrequencyOrder(44100);
	
	for(int i=0;i<NUMBER_MAXFREQUENCIES;i++)
	{
		FormatDescription format;
		EXPECT_TRUE(format.setFrequencyIndex(order[i]));
        EXPECT_EQ(expectedOrder[i],format.frequency());
	}
	
	delete [] order;
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestFrequencyOrderWhen48000Hz)
{
	const tint expectedOrder[NUMBER_MAXFREQUENCIES] = {
		48000, 96000, 192000, 384000, 768000, 1536000, 3072000,
		64000, 88200, 176400, 352800, 705600, 1411200, 2822400,
		44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000
	};

    int *order = FormatDescriptionUtilsTest::testClosestFrequencyOrder(48000);
	
	for(int i=0;i<NUMBER_MAXFREQUENCIES;i++)
	{
		FormatDescription format;
		EXPECT_TRUE(format.setFrequencyIndex(order[i]));
        EXPECT_EQ(expectedOrder[i],format.frequency());
	}
	
	delete [] order;
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestFrequencyOrderWhen8000Hz)
{
	const tint expectedOrder[NUMBER_MAXFREQUENCIES] = {
		8000, 16000, 32000, 64000,
		11025, 12000, 22050, 24000, 44100,
		48000, 88200, 96000, 176400, 192000, 352800,
		384000, 705600, 768000,
		1411200, 1536000, 2822400, 3072000
	};

    int *order = FormatDescriptionUtilsTest::testClosestFrequencyOrder(8000);
	
	for(int i=0;i<NUMBER_MAXFREQUENCIES;i++)
	{
		FormatDescription format;
		EXPECT_TRUE(format.setFrequencyIndex(order[i]));
        EXPECT_EQ(expectedOrder[i],format.frequency());
	}
	
	delete [] order;
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestFrequencyOrderWhen768000Hz)
{
	const tint expectedOrder[NUMBER_MAXFREQUENCIES] = {
		768000, 1536000, 3072000,
		1411200, 2822400,
		705600, 384000,
		352800, 192000, 176400, 96000, 88200,
		64000, 48000, 44100, 32000, 24000,
		22050, 16000, 12000, 11025, 8000
	};
	
    int *order = FormatDescriptionUtilsTest::testClosestFrequencyOrder(768000);
	
	for(int i=0;i<NUMBER_MAXFREQUENCIES;i++)
	{
		FormatDescription format;
		EXPECT_TRUE(format.setFrequencyIndex(order[i]));
        EXPECT_EQ(expectedOrder[i],format.frequency());
	}
	
	delete [] order;
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestBitOrderForFloatDoublePercision)
{
	const tint c_highSize = 1;
	const tint c_lowSize  = 33;
	
	const tint c_expectedOrderHigh[c_highSize] = { 
		5 // 64(F)
	};
	const tint c_expectedOrderLow[c_lowSize] = {
		 3, 33, 32, 31, 30, 29, 28, 27, // 32(I), 31(I), 30(I), 29(I), 28(I), 27(I), 26(I), 25(I),
		 2, 26, 25, 24, 23, 22, 21, 20, // 24(I), 23(I), 22(I), 21(I), 20(I), 19(I), 18(I), 17(I),
		 1,  4, 19, 18, 17, 16, 15, 14, // 16(I), 32(F), 15(I), 14(I), 13(I), 12(I), 11(I), 10(I),
		13,  0, 12, 11, 10,  9,  8,  7, //  9(I),  8(I),  7(I),  6(I),  5(I),  4(I),  3(I),  2(I),
		 6                              //  1(I)
	};
	
	QVector<tint> hList,lList;
	FormatDescriptionUtilsTest::testClosestBitOrder(5,hList,lList);
	
	ASSERT_EQ(c_highSize,hList.size());
	for(tint i=0;i<c_highSize;i++)
	{
		EXPECT_EQ(c_expectedOrderHigh[i],hList.at(i));
	}
	
	ASSERT_EQ(c_lowSize,lList.size());
	for(tint i=0;i<c_lowSize;i++)
	{
		EXPECT_EQ(c_expectedOrderLow[i],lList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestBitOrderForFloatSinglePercision)
{
	const tint c_highSize = 2;
	const tint c_lowSize  = 32;

	const tint c_expectedOrderHigh[c_highSize] = {
		4, 5 // 32(F), 64(F)
	};
	const tint c_expectedOrderLow[c_lowSize] = {
		 3, 33, 32, 31, 30, 29, 28, 27, // 32(I), 31(I), 30(I), 29(I), 28(I), 27(I), 26(I), 25(I), 
		 2, 26, 25, 24, 23, 22, 21, 20, // 24(I), 23(I), 22(I), 21(I), 20(I), 19(I), 18(I), 17(I), 
		 1, 19, 18, 17, 16, 15, 14, 13, // 16(I), 15(I), 14(I), 13(I), 12(I), 11(I), 10(I),  9(I),  
		 0, 12, 11, 10,  9,  8,  7,  6  //  8(I),  7(I),  6(I),  5(I),  4(I),  3(I),  2(I),  1(I)
	};
	
	QVector<tint> hList,lList;
	FormatDescriptionUtilsTest::testClosestBitOrder(4,hList,lList);
	
	ASSERT_EQ(c_highSize,hList.size());
	for(tint i=0;i<c_highSize;i++)
	{
		EXPECT_EQ(c_expectedOrderHigh[i],hList.at(i));
	}
	
	ASSERT_EQ(c_lowSize,lList.size());
	for(tint i=0;i<c_lowSize;i++)
	{
		EXPECT_EQ(c_expectedOrderLow[i],lList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestBitOrderForInteger32Bits)
{
	const tint c_highSize = 2;
	const tint c_lowSize  = 32;

	const tint c_expectedOrderHigh[c_highSize] = {
		3, 5 // 32(I), 64(F)
	};
	const tint c_expectedOrderLow[c_lowSize] = {
		33, 32, 31, 30, 29, 28, 27,  2, // 31(I), 30(I), 29(I), 28(I), 27(I), 26(I), 25(I), 24(I), 
		26, 25, 24, 23, 22, 21, 20,  1, // 23(I), 22(I), 21(I), 20(I), 19(I), 18(I), 17(I), 16(I), 
		 4, 19, 18, 17, 16, 15, 14, 13, // 32(F), 15(I), 14(I), 13(I), 12(I), 11(I), 10(I),  9(I),
		 0, 12, 11, 10,  9,  8,  7,  6  //  8(I),  7(I),  6(I),  5(I),  4(I),  3(I),  2(I),  1(I)
	};

	QVector<tint> hList,lList;
	FormatDescriptionUtilsTest::testClosestBitOrder(3,hList,lList);
	
	ASSERT_EQ(c_highSize,hList.size());
	for(tint i=0;i<c_highSize;i++)
	{
		EXPECT_EQ(c_expectedOrderHigh[i],hList.at(i));
	}
	
	ASSERT_EQ(c_lowSize,lList.size());
	for(tint i=0;i<c_lowSize;i++)
	{
		EXPECT_EQ(c_expectedOrderLow[i],lList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestBitOrderForInteger24Bits)
{
	const tint c_highSize = 10;
	const tint c_lowSize  = 24;

	const tint c_expectedOrderHigh[c_highSize] = {
		 2,  3, 33, 32, 31, 30, 29, 28, // 24(I), 32(I), 31(I), 30(I), 29(I), 28(I), 27(I), 26(I), 
		27,  5                          // 25(I), 64(F)
	};
	const tint c_expectedOrderLow[c_lowSize] = {
		26, 25, 24, 23, 22, 21, 20,  1, // 23(I), 22(I), 21(I), 20(I), 19(I), 18(I), 17(I), 16(I), 
		 4, 19, 18, 17, 16, 15, 14, 13, // 32(F), 15(I), 14(I), 13(I), 12(I), 11(I), 10(I),  9(I),
		 0, 12, 11, 10,  9,  8,  7,  6  //  8(I),  7(I),  6(I),  5(I),  4(I),  3(I),  2(I),  1(I)
	};
	
	QVector<tint> hList,lList;
	FormatDescriptionUtilsTest::testClosestBitOrder(2,hList,lList);
	
	ASSERT_EQ(c_highSize,hList.size());
	for(tint i=0;i<c_highSize;i++)
	{
		EXPECT_EQ(c_expectedOrderHigh[i],hList.at(i));
	}
	
	ASSERT_EQ(c_lowSize,lList.size());
	for(tint i=0;i<c_lowSize;i++)
	{
		EXPECT_EQ(c_expectedOrderLow[i],lList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestBitOrderForInteger16Bits)
{
	const tint c_highSize = 18;
	const tint c_lowSize  = 16;

	const tint c_expectedOrderHigh[c_highSize] = {
		 1,  3, 33, 32, 31, 30, 29, 28, // 16(I), 32(I), 31(I), 30(I), 29(I), 28(I), 27(I), 26(I),
		27,  2, 26, 25, 24, 23, 22, 21, // 25(I), 24(I), 23(I), 22(I), 21(I), 20(I), 19(I), 18(I),
		20,  5                          // 17(I), 64(F)
	};
	const tint c_expectedOrderLow[c_lowSize] = {
		 4, 19, 18, 17, 16, 15, 14, 13, // 32(F), 15(I), 14(I), 13(I), 12(I), 11(I), 10(I),  9(I),
		 0, 12, 11, 10,  9,  8,  7,  6  //  8(I),  7(I),  6(I),  5(I),  4(I),  3(I),  2(I),  1(I)
	};

	QVector<tint> hList,lList;
	FormatDescriptionUtilsTest::testClosestBitOrder(1,hList,lList);
	
	ASSERT_EQ(c_highSize,hList.size());
	for(tint i=0;i<c_highSize;i++)
	{
		EXPECT_EQ(c_expectedOrderHigh[i],hList.at(i));
	}
	
	ASSERT_EQ(c_lowSize,lList.size());
	for(tint i=0;i<c_lowSize;i++)
	{
		EXPECT_EQ(c_expectedOrderLow[i],lList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestBitOrderForInteger8Bits)
{
	const tint c_highSize = 27;
	const tint c_lowSize  = 7;

	const tint c_expectedOrderHigh[c_highSize] = {
		 0,  3, 33, 32, 31, 30, 29, 28, //  8(I), 32(I), 31(I), 30(I), 29(I), 28(I), 27(I), 26(I),
		27,  2, 26, 25, 24, 23, 22, 21, // 25(I), 24(I), 23(I), 22(I), 21(I), 20(I), 19(I), 18(I),
		20,  1, 19, 18, 17, 16, 15, 14, // 17(I), 16(I), 15(I), 14(I), 13(I), 12(I), 11(I), 10(I),
		13,  5,  4                      //  9(I), 64(F), 32(F)
	};
	const tint c_expectedOrderLow[c_lowSize] = {
		12, 11, 10,  9,  8,  7,  6      //  7(I),  6(I),  5(I),  4(I),  3(I),  2(I),  1(I)
	};

	QVector<tint> hList,lList;
	FormatDescriptionUtilsTest::testClosestBitOrder(0,hList,lList);
	
	ASSERT_EQ(c_highSize,hList.size());
	for(tint i=0;i<c_highSize;i++)
	{
		EXPECT_EQ(c_expectedOrderHigh[i],hList.at(i));
	}
	
	ASSERT_EQ(c_lowSize,lList.size());
	for(tint i=0;i<c_lowSize;i++)
	{
		EXPECT_EQ(c_expectedOrderLow[i],lList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestBitOrderForInteger28Bits)
{
	const tint c_highSize = 6;
	const tint c_lowSize  = 28;
	
	const tint c_expectedOrderHigh[c_highSize] = {
		30,  3, 33, 32, 31,  5          // 28(I), 32(I), 31(I), 30(I), 29(I), 64(F)
	};
	const tint c_expectedOrderLow[c_lowSize] = {
		29, 28, 27,  2, 26, 25, 24, 23, // 27(I), 26(I), 25(I), 24(I), 23(I), 22(I), 21(I), 20(I),
		22, 21, 20,  1,  4, 19, 18, 17, // 19(I), 18(I), 17(I), 16(I), 32(F), 15(I), 14(I), 13(I),
		16, 15, 14, 13,  0, 12, 11, 10, // 12(I), 11(I), 10(I),  9(I),  8(I),  7(I),  6(I),  5(I),
		 9,  8,  7,  6                  //  4(I),  3(I),  2(I),  1(I)
	};

	QVector<tint> hList,lList;
	FormatDescriptionUtilsTest::testClosestBitOrder(30,hList,lList);
	
	ASSERT_EQ(c_highSize,hList.size());
	for(tint i=0;i<c_highSize;i++)
	{
		EXPECT_EQ(c_expectedOrderHigh[i],hList.at(i));
	}
	
	ASSERT_EQ(c_lowSize,lList.size());
	for(tint i=0;i<c_lowSize;i++)
	{
		EXPECT_EQ(c_expectedOrderLow[i],lList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestBitOrderForInteger20Bits)
{
	const tint c_highSize = 14;
	const tint c_lowSize  = 20;

	const tint c_expectedOrderHigh[c_highSize] = {
		23,  3, 33, 32, 31, 30, 29, 28, // 20(I), 32(I), 31(I), 30(I), 29(I), 28(I), 27(I), 26(I), 
		27,  2, 26, 25, 24,  5          // 25(I), 24(I), 23(I), 22(I), 21(I), 64(F)
	};
	const tint c_expectedOrderLow[c_lowSize] = {
		22, 21, 20,  1,  4, 19, 18, 17, // 19(I), 18(I), 17(I), 16(I), 32(F), 15(I), 14(I), 13(I),
		16, 15, 14, 13,  0, 12, 11, 10, // 12(I), 11(I), 10(I),  9(I),  8(I),  7(I),  6(I),  5(I),
		 9,  8,  7,  6                  //  4(I),  3(I),  2(I),  1(I)
	};

	QVector<tint> hList,lList;
	FormatDescriptionUtilsTest::testClosestBitOrder(23,hList,lList);
	
	ASSERT_EQ(c_highSize,hList.size());
	for(tint i=0;i<c_highSize;i++)
	{
		EXPECT_EQ(c_expectedOrderHigh[i],hList.at(i));
	}
	
	ASSERT_EQ(c_lowSize,lList.size());
	for(tint i=0;i<c_lowSize;i++)
	{
		EXPECT_EQ(c_expectedOrderLow[i],lList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestBitOrderForInteger9Bits)
{
	const tint c_highSize = 26;
	const tint c_lowSize  = 8;
	
	const tint c_expectedOrderHigh[c_highSize] = {
		13,  3, 33, 32, 31, 30, 29, 28, //  9(I), 32(I), 31(I), 30(I), 29(I), 28(I), 27(I), 26(I),
		27,  2, 26, 25, 24, 23, 22, 21, // 25(I), 24(I), 23(I), 22(I), 21(I), 20(I), 19(I), 18(I),
		20,  1, 19, 18, 17, 16, 15, 14, // 17(I), 16(I), 15(I), 14(I), 13(I), 12(I), 11(I), 10(I),
		 5,  4                          //  64(F), 32(F)
	};
	const tint c_expectedOrderLow[c_lowSize] = {
		 0, 12, 11, 10,  9,  8,  7,  6  //  8(I),  7(I),  6(I),  5(I),  4(I),  3(I),  2(I),  1(I)
	};

	QVector<tint> hList,lList;
	FormatDescriptionUtilsTest::testClosestBitOrder(13,hList,lList);
	
	ASSERT_EQ(c_highSize,hList.size());
	for(tint i=0;i<c_highSize;i++)
	{
		EXPECT_EQ(c_expectedOrderHigh[i],hList.at(i));
	}
	
	ASSERT_EQ(c_lowSize,lList.size());
	for(tint i=0;i<c_lowSize;i++)
	{
		EXPECT_EQ(c_expectedOrderLow[i],lList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,closestBitOrderForInteger3Bits)
{
	const tint c_highSize = 32;
	const tint c_lowSize  = 2;

	const tint c_expectedOrderHigh[c_highSize] = {
		 8,  3, 33, 32, 31, 30, 29, 28, //  3(I), 32(I), 31(I), 30(I), 29(I), 28(I), 27(I), 26(I),
		27,  2, 26, 25, 24, 23, 22, 21, // 25(I), 24(I), 23(I), 22(I), 21(I), 20(I), 19(I), 18(I),
		20,  1, 19, 18, 17, 16, 15, 14, // 17(I), 16(I), 15(I), 14(I), 13(I), 12(I), 11(I), 10(I),  
		13,  0, 12, 11, 10,  9,  5,  4  //  9(I),  8(I),  7(I),  6(I),  5(I),  4(I), 64(F), 32(F)
	};
	const tint c_expectedOrderLow[c_lowSize] = {
		 7,  6                          //  2(I),  1(I)
	};
	
	QVector<tint> hList,lList;
	FormatDescriptionUtilsTest::testClosestBitOrder(8,hList,lList);
	
	ASSERT_EQ(c_highSize,hList.size());
	for(tint i=0;i<c_highSize;i++)
	{
		EXPECT_EQ(c_expectedOrderHigh[i],hList.at(i));
	}
	
	ASSERT_EQ(c_lowSize,lList.size());
	for(tint i=0;i<c_lowSize;i++)
	{
		EXPECT_EQ(c_expectedOrderLow[i],lList.at(i));
	}
}

//-------------------------------------------------------------------------------------------
// No Formats Supported
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedWhenNoneAreSupported)
{
	FormatsSupported support;
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,2,44100);
	
	FormatDescription formatClose;
	ASSERT_FALSE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
}

//-------------------------------------------------------------------------------------------
// Same
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits44100HzAnd2ChannelsWhen16Bits44100HzAnd2ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,16,2,44100);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,2,44100);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(16,formatClose.bits());
	EXPECT_EQ(2,formatClose.channels());
	EXPECT_EQ(44100,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor32Bits192000HzAnd8ChannelsWhen32Bits192000HzAnd8ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,16,2,44100);
	FormatDescription formatB(FormatDescription::e_DataSignedInteger,16,8,192000);
	FormatDescription formatC(FormatDescription::e_DataSignedInteger,32,8,192000);
	FormatsSupported support;
	support.add(formatA);
	support.add(formatB);
	support.add(formatC);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,32,8,192000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(32,formatClose.bits());
	EXPECT_EQ(8,formatClose.channels());
	EXPECT_EQ(192000,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 0,0 - A
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits44100HzAnd2ChannelsWhen24Bits44100HzAnd2ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,24,2,44100);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,2,44100);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(24,formatClose.bits());
	EXPECT_EQ(2,formatClose.channels());
	EXPECT_EQ(44100,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 0,0 - B
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits48000HzAnd2ChannelsWhen16Bits96000HzAnd8ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,16,8,96000);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,2,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(16,formatClose.bits());
	EXPECT_EQ(8,formatClose.channels());
	EXPECT_EQ(96000,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 0,0 - C
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits48000HzAnd2ChannelsWhen24Bit88200HzAnd6ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,24,6,88200);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,2,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(24,formatClose.bits());
	EXPECT_EQ(6,formatClose.channels());
	EXPECT_EQ(88200,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits48000HzAnd2ChannelsWhen32BitFloat88200HzAnd6ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataFloatSingle,32,6,88200);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,2,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataFloatSingle,formatClose.typeOfData());
	EXPECT_EQ(32,formatClose.bits());
	EXPECT_EQ(6,formatClose.channels());
	EXPECT_EQ(88200,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 0,1 - D
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits44100HzAnd2ChannelsWhen16Bits22050HzAnd2ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,16,2,22050);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,2,44100);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(16,formatClose.bits());
	EXPECT_EQ(2,formatClose.channels());
	EXPECT_EQ(22050,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 0,1 - E
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits48000HzAnd2ChannelsWhen24Bits32000HzAnd8ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,24,8,32000);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,2,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(24,formatClose.bits());
	EXPECT_EQ(8,formatClose.channels());
	EXPECT_EQ(32000,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 1,0 - B
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits48000HzAnd4ChannelsWhen16Bits96000HzAnd2ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,16,2,96000);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,4,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(16,formatClose.bits());
	EXPECT_EQ(2,formatClose.channels());
	EXPECT_EQ(96000,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 1,0 - C
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits48000HzAnd6ChannelsWhen24Bit88200HzAnd1ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,24,1,88200);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,6,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(24,formatClose.bits());
	EXPECT_EQ(1,formatClose.channels());
	EXPECT_EQ(88200,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits48000HzAnd6ChannelsWhen32BitFloat88200HzAnd1ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataFloatSingle,32,1,88200);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,6,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataFloatSingle,formatClose.typeOfData());
	EXPECT_EQ(32,formatClose.bits());
	EXPECT_EQ(1,formatClose.channels());
	EXPECT_EQ(88200,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 1,1 - E
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits48000HzAnd8ChannelsWhen24Bits32000HzAnd4ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,24,4,32000);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,8,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(24,formatClose.bits());
	EXPECT_EQ(4,formatClose.channels());
	EXPECT_EQ(32000,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 2,0 - A
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor24Bits44100HzAnd2ChannelsWhen8Bits44100HzAnd2ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,8,2,44100);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,24,2,44100);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(8,formatClose.bits());
	EXPECT_EQ(2,formatClose.channels());
	EXPECT_EQ(44100,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 2,0 - C
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor32BitFloat48000HzAnd6ChannelsWhen16Bits88200HzAnd2ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,16,2,48000);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataFloatSingle,32,6,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(16,formatClose.bits());
	EXPECT_EQ(2,formatClose.channels());
	EXPECT_EQ(48000,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 2,1 - E
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits48000HzAnd2ChannelsWhen8Bits32000HzAnd8ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,8,8,32000);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,2,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(8,formatClose.bits());
	EXPECT_EQ(8,formatClose.channels());
	EXPECT_EQ(32000,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 3,0 - C
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor32BitFloat48000HzAnd4ChannelsWhen8Bits88200HzAnd3ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,8,3,88200);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataFloatSingle,32,4,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(8,formatClose.bits());
	EXPECT_EQ(3,formatClose.channels());
	EXPECT_EQ(88200,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------
// 3,1 - E
//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeSupportedFor16Bits48000HzAnd8ChannelsWhen8Bits32000HzAnd2ChannelsSupported)
{
	FormatDescription formatA(FormatDescription::e_DataSignedInteger,8,2,32000);
	FormatsSupported support;
	support.add(formatA);
	
	FormatDescription formatActual(FormatDescription::e_DataSignedInteger,16,8,48000);
	
	FormatDescription formatClose;
	ASSERT_TRUE(FormatDescriptionUtils::findClosestFormatType(formatActual,support,formatClose));
	
	EXPECT_EQ(FormatDescription::e_DataSignedInteger,formatClose.typeOfData());
	EXPECT_EQ(8,formatClose.bits());
	EXPECT_EQ(2,formatClose.channels());
	EXPECT_EQ(32000,formatClose.frequency());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeChannelListFor1ChannelAscending)
{
	const tint c_outSize = 8;
	const tint c_expectedChannels[c_outSize] = {
		0, 1, 2, 3, 4, 5, 6, 7
	};
	
	QVector<tint> chList;
	FormatDescription format(FormatDescription::e_DataSignedInteger,16,1,44100);
	FormatDescriptionUtilsTest::testFindClosestFormatTypeChannelList(format,true,chList);
	
	ASSERT_EQ(c_outSize,chList.size());
	for(tint i=0;i<c_outSize;i++)
	{
		EXPECT_EQ(c_expectedChannels[i],chList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeChannelListFor1ChannelDecending)
{
	const tint c_outSize = 0;
	
	QVector<tint> chList;
	FormatDescription format(FormatDescription::e_DataSignedInteger,16,1,44100);
	FormatDescriptionUtilsTest::testFindClosestFormatTypeChannelList(format,false,chList);
	
	ASSERT_EQ(c_outSize,chList.size());
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeChannelListFor8ChannelsAscending)
{
	const tint c_outSize = 1;
	const tint c_expectedChannels[c_outSize] = { 7 };
	
	QVector<tint> chList;
	FormatDescription format(FormatDescription::e_DataSignedInteger,16,8,44100);
	FormatDescriptionUtilsTest::testFindClosestFormatTypeChannelList(format,true,chList);
	
	ASSERT_EQ(c_outSize,chList.size());
	for(tint i=0;i<c_outSize;i++)
	{
		EXPECT_EQ(c_expectedChannels[i],chList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeChannelListFor8ChannelDecending)
{
	const tint c_outSize = 7;
	const tint c_expectedChannels[c_outSize] = {
		6, 5, 4, 3, 2, 1, 0
	};
	
	QVector<tint> chList;
	FormatDescription format(FormatDescription::e_DataSignedInteger,16,8,44100);
	FormatDescriptionUtilsTest::testFindClosestFormatTypeChannelList(format,false,chList);
	
	ASSERT_EQ(c_outSize,chList.size());
	for(tint i=0;i<c_outSize;i++)
	{
		EXPECT_EQ(c_expectedChannels[i],chList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeChannelListFor4ChannelsAscending)
{
	const tint c_outSize = 5;
	const tint c_expectedChannels[c_outSize] = {
		3, 4, 5, 6, 7
	};
	
	QVector<tint> chList;
	FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,44100);
	FormatDescriptionUtilsTest::testFindClosestFormatTypeChannelList(format,true,chList);
	
	ASSERT_EQ(c_outSize,chList.size());
	for(tint i=0;i<c_outSize;i++)
	{
		EXPECT_EQ(c_expectedChannels[i],chList.at(i));
	}
}

//-------------------------------------------------------------------------------------------

TEST(FormatDescriptionUtils,findClosestFormatTypeChannelListFor4ChannelsDecending)
{
	const tint c_outSize = 3;
	const tint c_expectedChannels[c_outSize] = {
		2, 1, 0
	};
	
	QVector<tint> chList;
	FormatDescription format(FormatDescription::e_DataSignedInteger,16,4,44100);
	FormatDescriptionUtilsTest::testFindClosestFormatTypeChannelList(format,false,chList);
	
	ASSERT_EQ(c_outSize,chList.size());
	for(tint i=0;i<c_outSize;i++)
	{
		EXPECT_EQ(c_expectedChannels[i],chList.at(i));
	}
}

//-------------------------------------------------------------------------------------------
